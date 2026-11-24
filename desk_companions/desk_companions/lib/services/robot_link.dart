import 'dart:async';

import '../models/pet.dart';

enum StudyMode { pomodoro, cronometro }

/// Estado que o robô manda a cada 1 segundo (mensagem P do PROTOCOLO.md).
class RobotStatus {
  const RobotStatus({
    required this.onde,
    required this.pet,
    required this.estudo,
    required this.fase,
    required this.saude,
    required this.fome,
    required this.felicidade,
    required this.higiene,
    required this.energia,
    required this.lanches,
    required this.vivo,
  });

  /// 'R' no robô, 'A' no app, 'V' viajando pelo portal.
  final String onde;
  final PetKind pet;

  /// 'N' nenhum, 'P' Pomodoro, 'C' cronômetro.
  final String estudo;

  /// 'F' foco, 'P' pausa do Pomodoro, 'Z' cronômetro pausado pelo ENTER.
  final String fase;
  final int saude, fome, felicidade, higiene, energia;
  final int lanches;
  final bool vivo;
}

/// Mensagens que chegam do robô.
sealed class RobotEvent {}

/// Saúde e felicidade, de 0 a 4 (os cuidados acontecem no robô).
class StatsEvent extends RobotEvent {
  StatsEvent(this.health, this.happiness);
  final int health;
  final int happiness;
}

/// O Enter do robô pausou ou retomou o cronômetro.
class StudyPauseEvent extends RobotEvent {
  StudyPauseEvent(this.paused);
  final bool paused;
}

/// Chegou o estado completo do robô.
class RobotStatusEvent extends RobotEvent {
  RobotStatusEvent(this.status);
  final RobotStatus status;
}

/// A conexão mudou (procurando, conectado, desconectado...).
class ConnectionEvent extends RobotEvent {}

/// Tudo que o app sabe pedir ao robô.
///
/// [SimulatedRobotLink] finge ser o robô (bom para testar no simulador).
/// [BleRobotLink] (ble_robot_link.dart) fala com o robô de verdade pelo Bluetooth.
/// As telas não sabem qual dos dois está sendo usado.
abstract class RobotLink {
  /// Texto curto mostrado no topo da tela, ex.: "Robô conectado".
  String get connectionLabel;
  bool get isConnected;
  Stream<RobotEvent> get events;

  /// Manda o mascote para a LCD. Retorna true quando o robô confirma a chegada.
  Future<bool> sendPetToRobot(PetKind pet);

  /// Traz o mascote de volta. Retorna true quando o robô confirma a saída.
  Future<bool> bringPetBack(PetKind pet);

  Future<void> startStudy(StudyMode mode);
  Future<void> stopStudy();

  /// Manda [quantidade] comidas compradas na loja para a lancheira do robô.
  Future<void> sendFood(int quantidade);

  void dispose();
}

/// Robô de mentirinha, para testar as telas sem hardware.
class SimulatedRobotLink implements RobotLink {
  final _events = StreamController<RobotEvent>.broadcast();
  bool _paused = false;

  @override
  String get connectionLabel => 'Robô simulado';

  @override
  bool get isConnected => true;

  @override
  Stream<RobotEvent> get events => _events.stream;

  @override
  Future<bool> sendPetToRobot(PetKind pet) async {
    await Future<void>.delayed(const Duration(seconds: 3));
    return true;
  }

  @override
  Future<bool> bringPetBack(PetKind pet) async {
    await Future<void>.delayed(const Duration(seconds: 3));
    return true;
  }

  @override
  Future<void> startStudy(StudyMode mode) async {
    _paused = false;
  }

  @override
  Future<void> stopStudy() async {
    _paused = false;
  }

  @override
  Future<void> sendFood(int quantidade) async {}

  /// Finge que alguém apertou Enter na protoboard.
  void simulateEnter() {
    _paused = !_paused;
    _events.add(StudyPauseEvent(_paused));
  }

  @override
  void dispose() => _events.close();
}
