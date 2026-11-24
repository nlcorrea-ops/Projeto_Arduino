import '../models/pet.dart';
import 'robot_link.dart';

/// Formato das mensagens entre o app e o robô (o mesmo do PROTOCOLO.md do Arduino):
///
///     $<corpo>*<conferência>\n
///
/// A conferência é o XOR de todos os caracteres do corpo, em 2 dígitos hexadecimais.
class Protocolo {
  static int _xor(String corpo) {
    var x = 0;
    for (final c in corpo.codeUnits) {
      x ^= c;
    }
    return x;
  }

  /// Monta a mensagem completa para enviar. Ex.: monta('O') == '\$O*4F\n'.
  static String monta(String corpo) {
    final hex = _xor(corpo).toRadixString(16).toUpperCase().padLeft(2, '0');
    return '\$$corpo*$hex\n';
  }

  /// Confere uma linha recebida e devolve só o corpo, ou null se ela veio com erro.
  static String? abre(String linha) {
    final inicio = linha.lastIndexOf('\$');
    if (inicio < 0) return null;
    final resto = linha.substring(inicio + 1);
    final estrela = resto.indexOf('*');
    if (estrela < 1 || resto.length < estrela + 3) return null;
    final corpo = resto.substring(0, estrela);
    final conferencia = int.tryParse(resto.substring(estrela + 1, estrela + 3), radix: 16);
    if (conferencia == null || conferencia != _xor(corpo)) return null;
    return corpo;
  }

  static String letraDoPet(PetKind pet) => switch (pet) {
        PetKind.tubarao => 'T',
        PetKind.sapo => 'S',
        PetKind.recruta => 'R',
      };

  static PetKind petDaLetra(String letra) => switch (letra) {
        'S' => PetKind.sapo,
        'R' => PetKind.recruta,
        _ => PetKind.tubarao,
      };

  /// Lê a mensagem de estado, ex.: "PRTNF100080080080080021".
  static RobotStatus? leEstado(String corpo) {
    if (corpo.length < 22 || corpo[0] != 'P') return null;
    int numero(int i) => int.tryParse(corpo.substring(i, i + 3)) ?? 0;
    return RobotStatus(
      onde: corpo[1],
      pet: petDaLetra(corpo[2]),
      estudo: corpo[3],
      fase: corpo[4],
      saude: numero(5),
      fome: numero(8),
      felicidade: numero(11),
      higiene: numero(14),
      energia: numero(17),
      lanches: int.tryParse(corpo[20]) ?? 0,
      vivo: corpo[21] == '1',
    );
  }
}
