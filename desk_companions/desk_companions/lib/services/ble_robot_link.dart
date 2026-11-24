import 'dart:async';

import 'package:flutter_blue_plus/flutter_blue_plus.dart';

import '../models/pet.dart';
import 'protocolo.dart';
import 'robot_link.dart';

/// Nome que o módulo Bluetooth do robô anuncia (o mesmo que aparece no LightBlue).
const String nomeDoRobo = 'HC-06';

enum _Estado { semBluetooth, bluetoothDesligado, procurando, conectando, conectado }

/// Conversa com o robô de verdade pelo Bluetooth Low Energy (BLE).
///
/// O módulo do robô funciona como um "cabo serial sem fio":
///   - serviço FFE0, característica FFE1;
///   - o que o app escreve na FFE1 chega no Arduino (pinos D2/D3);
///   - o que o Arduino envia chega no app como notificação da FFE1.
///
/// A conexão é automática: o app procura o robô, conecta e, se a conexão cair,
/// volta a procurar sozinho.
class BleRobotLink implements RobotLink {
  BleRobotLink() {
    _iniciar();
  }

  final _events = StreamController<RobotEvent>.broadcast();

  BluetoothDevice? _device;
  BluetoothCharacteristic? _chr;
  _Estado _estado = _Estado.procurando;
  bool _conectando = false;
  bool _fechado = false;

  StreamSubscription<BluetoothAdapterState>? _adapterSub;
  StreamSubscription<List<ScanResult>>? _scanSub;
  StreamSubscription<BluetoothConnectionState>? _connSub;
  StreamSubscription<List<int>>? _valSub;
  Timer? _novaBusca;

  /// Pedaços de mensagem que ainda não terminaram (o BLE entrega em partes).
  String _recebido = '';

  /// Pedidos esperando confirmação do robô. A chave é o que o robô responde depois do "K".
  final Map<String, Completer<bool>> _aguardando = {};

  int _idComida = 0;

  // ------------------------------------------------------------------------------------------
  // Estado da conexão
  // ------------------------------------------------------------------------------------------

  @override
  String get connectionLabel => switch (_estado) {
        _Estado.semBluetooth => 'Sem Bluetooth',
        _Estado.bluetoothDesligado => 'Ligue o Bluetooth',
        _Estado.procurando => 'Procurando o robô…',
        _Estado.conectando => 'Conectando…',
        _Estado.conectado => 'Robô conectado',
      };

  @override
  bool get isConnected => _estado == _Estado.conectado && _chr != null;

  @override
  Stream<RobotEvent> get events => _events.stream;

  void _muda(_Estado novo) {
    if (_estado == novo) return;
    _estado = novo;
    if (!_events.isClosed) _events.add(ConnectionEvent());
  }

  // ------------------------------------------------------------------------------------------
  // Procurar e conectar
  // ------------------------------------------------------------------------------------------

  Future<void> _iniciar() async {
    if (await FlutterBluePlus.isSupported == false) {
      _muda(_Estado.semBluetooth);
      return;
    }
    _adapterSub = FlutterBluePlus.adapterState.listen((estado) {
      if (estado == BluetoothAdapterState.on) {
        if (!isConnected) _procurar();
      } else if (estado == BluetoothAdapterState.off) {
        _perdeuConexao();
        _muda(_Estado.bluetoothDesligado);
      }
    });
  }

  bool _ehORobo(ScanResult r) {
    final nome = r.advertisementData.advName.isNotEmpty ? r.advertisementData.advName : r.device.platformName;
    if (nome.toUpperCase().contains(nomeDoRobo)) return true;
    return r.advertisementData.serviceUuids.any((u) => u.toString().toUpperCase().contains('FFE0'));
  }

  Future<void> _procurar() async {
    if (_fechado || _conectando || isConnected || FlutterBluePlus.isScanningNow) return;
    _novaBusca?.cancel();
    _muda(_Estado.procurando);

    await _scanSub?.cancel();
    _scanSub = FlutterBluePlus.onScanResults.listen(
      (resultados) {
        for (final r in resultados) {
          if (_ehORobo(r)) {
            _conectar(r.device);
            break;
          }
        }
      },
      onError: (Object _) {},
    );

    try {
      await FlutterBluePlus.startScan(timeout: const Duration(seconds: 15));
      await FlutterBluePlus.isScanning.where((procurando) => procurando == false).first;
    } catch (_) {
      // se a busca falhar, tenta de novo daqui a pouco
    }
    if (!isConnected && !_conectando) _agendaNovaBusca();
  }

  void _agendaNovaBusca() {
    if (_fechado) return;
    _novaBusca?.cancel();
    _novaBusca = Timer(const Duration(seconds: 3), _procurar);
  }

  Future<void> _conectar(BluetoothDevice device) async {
    if (_conectando || isConnected) return;
    _conectando = true;
    _muda(_Estado.conectando);
    try {
      await FlutterBluePlus.stopScan();

      await _connSub?.cancel();
      _connSub = device.connectionState.listen((estado) {
        if (estado == BluetoothConnectionState.disconnected && _device == device) {
          _perdeuConexao();
          _agendaNovaBusca();
        }
      });

      // "free" = uso pessoal ou educacional, que é o caso deste projeto acadêmico
      await device.connect(license: License.free);
      final servicos = await device.discoverServices();

      BluetoothCharacteristic? caracteristica;
      for (final servico in servicos) {
        for (final c in servico.characteristics) {
          if (c.uuid.toString().toUpperCase().contains('FFE1')) caracteristica = c;
        }
      }
      if (caracteristica == null) throw Exception('O robô não tem a característica FFE1');

      await _valSub?.cancel();
      _valSub = caracteristica.onValueReceived.listen(_recebe);
      await caracteristica.setNotifyValue(true);

      _device = device;
      _chr = caracteristica;
      _recebido = '';
      _muda(_Estado.conectado);
    } catch (_) {
      try {
        await device.disconnect();
      } catch (_) {}
      _device = null;
      _chr = null;
      _muda(_Estado.procurando);
      _agendaNovaBusca();
    } finally {
      _conectando = false;
    }
  }

  void _perdeuConexao() {
    _chr = null;
    _device = null;
    for (final espera in _aguardando.values) {
      if (!espera.isCompleted) espera.complete(false);
    }
    _aguardando.clear();
    if (_estado == _Estado.conectado) _muda(_Estado.procurando);
  }

  // ------------------------------------------------------------------------------------------
  // Receber
  // ------------------------------------------------------------------------------------------

  void _recebe(List<int> bytes) {
    _recebido += String.fromCharCodes(bytes);
    var fim = _recebido.indexOf('\n');
    while (fim >= 0) {
      final linha = _recebido.substring(0, fim).trim();
      _recebido = _recebido.substring(fim + 1);
      final corpo = Protocolo.abre(linha);
      if (corpo != null) _trata(corpo);
      fim = _recebido.indexOf('\n');
    }
    if (_recebido.length > 200) _recebido = ''; // lixo: descarta
  }

  void _trata(String corpo) {
    if (corpo.startsWith('P')) {
      final estado = Protocolo.leEstado(corpo);
      if (estado == null) return;
      _events.add(StatsEvent(_coracoes(estado.saude), _coracoes(estado.felicidade)));
      _events.add(StudyPauseEvent(estado.fase == 'Z'));
      _events.add(RobotStatusEvent(estado));
    } else if (corpo.startsWith('K') && corpo.length >= 2) {
      final resposta = corpo.substring(1);
      if (resposta == 'N') {
        // pedido negado: vale para o pedido de portal ou de estudo que estiver esperando
        for (final chave in ['I', 'O', 'E']) {
          final espera = _aguardando[chave];
          if (espera != null && !espera.isCompleted) espera.complete(false);
        }
        return;
      }
      final chave = resposta.startsWith('C') ? resposta : resposta[0];
      final espera = _aguardando[chave];
      if (espera != null && !espera.isCompleted) espera.complete(true);
    }
  }

  /// 0 a 100 vira 0 a 4 corações.
  int _coracoes(int valor) => ((valor + 12) ~/ 25).clamp(0, 4);

  // ------------------------------------------------------------------------------------------
  // Enviar
  // ------------------------------------------------------------------------------------------

  Future<void> _escreve(String mensagem) async {
    final c = _chr;
    if (c == null) return;
    final bytes = mensagem.codeUnits;
    final semResposta = c.properties.writeWithoutResponse;
    for (var i = 0; i < bytes.length; i += 20) {
      final pedaco = bytes.sublist(i, i + 20 > bytes.length ? bytes.length : i + 20);
      await c.write(pedaco, withoutResponse: semResposta);
    }
  }

  /// Envia o pedido a cada 1 s até o robô confirmar (ou até [limite]).
  Future<bool> _pedido(String corpo, String chave, {Duration limite = const Duration(seconds: 12)}) async {
    if (!isConnected) return false;
    final espera = Completer<bool>();
    _aguardando[chave] = espera;
    final prazo = DateTime.now().add(limite);
    try {
      while (!espera.isCompleted && isConnected && DateTime.now().isBefore(prazo)) {
        await _escreve(Protocolo.monta(corpo));
        await Future.any([espera.future, Future<void>.delayed(const Duration(seconds: 1))]);
      }
    } catch (_) {
      // falha de escrita: trata como "não confirmado"
    }
    _aguardando.remove(chave);
    return espera.isCompleted ? await espera.future : false;
  }

  @override
  Future<bool> sendPetToRobot(PetKind pet) => _pedido('I${Protocolo.letraDoPet(pet)}', 'I');

  @override
  Future<bool> bringPetBack(PetKind pet) => _pedido('O', 'O');

  @override
  Future<void> startStudy(StudyMode mode) async {
    await _pedido(mode == StudyMode.pomodoro ? 'EP' : 'EC', 'E', limite: const Duration(seconds: 6));
  }

  @override
  Future<void> stopStudy() async {
    await _pedido('X', 'X', limite: const Duration(seconds: 6));
  }

  @override
  Future<void> sendFood(int quantidade) async {
    final n = quantidade.clamp(1, 9);
    final id = String.fromCharCode('a'.codeUnitAt(0) + _idComida);
    _idComida = (_idComida + 1) % 26;
    await _pedido('C$n$id', 'C$id', limite: const Duration(seconds: 8));
  }

  @override
  void dispose() {
    _fechado = true;
    _novaBusca?.cancel();
    _adapterSub?.cancel();
    _scanSub?.cancel();
    _connSub?.cancel();
    _valSub?.cancel();
    _device?.disconnect();
    _events.close();
  }
}
