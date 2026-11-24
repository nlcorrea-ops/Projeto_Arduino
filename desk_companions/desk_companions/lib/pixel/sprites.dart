import 'package:flutter/material.dart';

import 'sprite.dart';

/// Todos os desenhos em pixel art do app.
/// Para mudar um desenho, edite as letras: cada letra é uma cor da paleta.
class Sprites {
  static const ink = Color(0xFF23202B);

  static const tubarao = PixelSprite([
    '..........kk............',
    '..........kbk...........',
    '.........kbbbk..........',
    'kk......kbbbbbkkkkk.....',
    'kbk...kkbbbbbbbbbbbkk...',
    '.kbk.kbbbbbbbbbbbbbbbk..',
    '..kbkbbbbbbbbbbbbkbbbbk.',
    '..kbbbbbbbbbbbbbbkbbbbbk',
    '..kbkbbbbbbwwwwwwwwwpwwk',
    '.kbk.kbbbwwwwwwwwwwwkwk.',
    'kbk...kkkbbkkkkkkkkkkk..',
    'kk.......kkk............',
  ], {
    'k': ink,
    'b': Color(0xFF5D7898),
    'w': Color(0xFFF6F1E6),
    'p': Color(0xFFF29BB0),
  });

  static const sapo = PixelSprite([
    '...kkkk.......kkkk...',
    '..kwwwwk.....kwwwwk..',
    '..kwkkwkkkkkkkwkkwk..',
    '.kgwkkwgggggggwkkwgk.',
    '.kggwwgggggggggwwggk.',
    'kgggggggggggggggggggk',
    'kgpggggkgggggkggggpgk',
    'kgggggggkkkkkgggggggk',
    'kdddggggggyggggggdddk',
    '.kdddgggyyyyygggdddk.',
    '..kkdddkyyyyykdddkk..',
    '....kkkkkkkkkkkkk....',
  ], {
    'k': ink,
    'w': Color(0xFFFBF6E8),
    'g': Color(0xFF7DB35A),
    'd': Color(0xFF3E9A62),
    'y': Color(0xFFD7F06A),
    'p': Color(0xFFF2A2A8),
  });

  static const recruta = PixelSprite([
    '.....kkkkkk.....',
    '...kkooooookk...',
    '..kooooooooOOk..',
    '.koooooooooooOk.',
    '.kOyyyyOOyyyyOk.',
    '.kOykkyOOykkyOk.',
    '.kOyyyyOOyyyyOk.',
    '.kffffffffffffk.',
    'mkffkkffffkkffk.',
    'mkffffffffffffk.',
    '.kfpffkffkffpfk.',
    '..kffffkkffffk..',
    '...kkkkkkkkkk...',
    '..kvvvvvvvvvvk..',
    '.kvvVvvvvvvVvvk.',
    '.kfkvvvvvvvvkfk.',
    '..kkvvvvvvvvkk..',
    '...kbbk..kbbk...',
    '...kkkk..kkkk...',
  ], {
    'k': ink,
    'o': Color(0xFF7C8B4E),
    'O': Color(0xFF5E6B39),
    'y': Color(0xFFF2B544),
    'f': Color(0xFFF4C9A0),
    'p': Color(0xFFEE9A8E),
    'm': Color(0xFF3A3A44),
    'v': Color(0xFFC98A3F),
    'V': Color(0xFFA86E2C),
    'b': Color(0xFF4A4A55),
  });

  static const _heartRows = [
    '.kk.kk.',
    'krrkrrk',
    'krrrrrk',
    '.krrrk.',
    '..krk..',
    '...k...',
  ];
  static const heart = PixelSprite(_heartRows, {'k': ink, 'r': Color(0xFFE5484D)});
  static const heartEmpty = PixelSprite(_heartRows, {'k': ink, 'r': Color(0xFFE8E2D2)});
  static const heartPink = PixelSprite(_heartRows, {'k': Color(0xFFC2185B), 'r': Color(0xFFFF6FA8)});

  static const _smileRows = [
    '.kkkkk.',
    'kyyyyyk',
    'kykykyk',
    'kyyyyyk',
    'kyyyyyk',
    'kykkkyk',
    '.kkkkk.',
  ];
  static const smile = PixelSprite(_smileRows, {'k': ink, 'y': Color(0xFFF7C948)});
  static const smileEmpty = PixelSprite(_smileRows, {'k': ink, 'y': Color(0xFFE8E2D2)});

  static const coin = PixelSprite([
    '.kkkkk.',
    'kyyyyyk',
    'kyykyyk',
    'kyykyyk',
    'kyykyyk',
    'kyyyyyk',
    '.kkkkk.',
  ], {'k': ink, 'y': Color(0xFFF2B544)});

  static const book = PixelSprite([
    'kkkkkk.kkkkkk',
    'kwwwwkkkwwwwk',
    'kwkkwwkwwkkwk',
    'kwwwwwkwwwwwk',
    'kwkkkwkwkkkwk',
    'kwwwwwkwwwwwk',
    'kwkkwwkwwkkwk',
    'kwwwwwkwwwwwk',
    'kkkkkkkkkkkkk',
    '.kbbbbkbbbbk.',
  ], {'k': ink, 'w': Color(0xFFFFFDF6), 'b': Color(0xFF2F5FB3)});

  static const portal = PixelSprite([
    '...kkkkk...',
    '.kkpppppkk.',
    '.kppkkkppk.',
    'kppkwwwkppk',
    'kpkwkkkwkpk',
    'kpkwkskwkpk',
    'kpkwkkwwkpk',
    'kppkwwwkppk',
    '.kppkkkppk.',
    '.kkpppppkk.',
    '...kkkkk...',
  ], {
    'k': ink,
    'p': Color(0xFF8A4FE0),
    'w': Color(0xFFF5D7FF),
    's': Color(0xFFFFF3A8),
  });

  static const paw = PixelSprite([
    '..k...k..',
    '.kkk.kkk.',
    '.kkk.kkk.',
    'k.k...k.k',
    'kkk...kkk',
    '...kkk...',
    '..kkkkk..',
    '..kkkkk..',
    '...k.k...',
  ], {'k': ink});

  static const bag = PixelSprite([
    '...kkk...',
    '..k...k..',
    '..k...k..',
    'kkkkkkkkk',
    'kyyyyyyyk',
    'kyyyyyyyk',
    'kyyyyyyyk',
    'kyyyyyyyk',
    'kkkkkkkkk',
  ], {'k': ink, 'y': Color(0xFFF7C948)});

  static const pause = PixelSprite([
    'kkkkk.kkkkk',
    'kwwwk.kwwwk',
    'kwwwk.kwwwk',
    'kwwwk.kwwwk',
    'kwwwk.kwwwk',
    'kwwwk.kwwwk',
    'kwwwk.kwwwk',
    'kkkkk.kkkkk',
  ], {'k': ink, 'w': Color(0xFFFFFFFF)});

  static const clock = PixelSprite([
    '..kkkkk..',
    '.kwwwwwk.',
    'kwwwkwwwk',
    'kwwwkwwwk',
    'kwwwkkkwk',
    'kwwwwwwwk',
    'kwwwwwwwk',
    '.kwwwwwk.',
    '..kkkkk..',
  ], {'k': ink, 'w': Color(0xFFFFE3DA)});

  static const stopwatch = PixelSprite([
    '...kkk...',
    '....k....',
    '..kkkkk..',
    '.kwwwwwk.',
    'kwwwkwwwk',
    'kwwwkwwwk',
    'kwwwwkwwk',
    '.kwwwwwk.',
    '..kkkkk..',
  ], {'k': ink, 'w': Color(0xFFE2F5E6)});

  static const lcd = PixelSprite([
    'kkkkkkkkkkkkk',
    'kgggggggggggk',
    'kgdddgggdgggk',
    'kggggggggdggk',
    'kgdgdggdddggk',
    'kgggggggggggk',
    'kkkkkkkkkkkkk',
  ], {'k': ink, 'g': Color(0xFF9BD36A), 'd': Color(0xFF2E5B2A)});

  static const noSignal = PixelSprite([
    'k...k...k',
    'kk..k..kk',
    '.kk.k.kk.',
    '..kkkkk..',
    '....k....',
    '....k....',
    'rr..k..rr',
    '.rr...rr.',
    '..rr.rr..',
    '...rrr...',
    '..rr.rr..',
    '.rr...rr.',
  ], {'k': ink, 'r': Color(0xFFE5484D)});

  static const caret = PixelSprite([
    'kkkkk',
    '.kkk.',
    '..k..',
  ], {'k': ink});

  // ---- Itens da loja ----
  static const fish = PixelSprite([
    '....kkk....',
    '..kkoook.k.',
    '.koooooookk',
    'kokoooooook',
    'kooooooookk',
    '.kooooook.k',
    '..kkkkk....',
  ], {'k': ink, 'o': Color(0xFFF08A3C)});

  static const shrimp = PixelSprite([
    '..kkkk...',
    'kkpppk...',
    'kpppppk..',
    '..kppppk.',
    '.kpppppk.',
    'kpppkkk..',
    'kpk......',
    'kk.......',
  ], {'k': ink, 'p': Color(0xFFF28C8C)});

  static const fly = PixelSprite([
    'kk...kk',
    'kwk.kwk',
    '.kwkwk.',
    '..kkk..',
    '.kbbbk.',
    '..kbk..',
    '...k...',
  ], {'k': ink, 'w': Color(0xFFCFE8FF), 'b': Color(0xFF3A3A44)});

  static const cake = PixelSprite([
    '...kk...',
    '...ry...',
    'kkkkkkkk',
    'kppppppk',
    'kwwwwwwk',
    'kcccccck',
    'kkkkkkkk',
  ], {
    'k': ink,
    'r': Color(0xFFE5484D),
    'y': Color(0xFFF7C948),
    'p': Color(0xFFF2A2C0),
    'w': Color(0xFFFFF6E3),
    'c': Color(0xFFB07440),
  });

  static const seaweed = PixelSprite([
    '..k...k.',
    '..gk.kg.',
    '.kgk.kgk',
    '.kgkkgk.',
    '..kggk..',
    '..kggk..',
    '.kgggk..',
    '.kkkkk..',
  ], {'k': ink, 'g': Color(0xFF45A96B)});

  static const iceCream = PixelSprite([
    '..kkkk..',
    '.kppppk.',
    'kpwppppk',
    'kppppppk',
    '.kkkkkk.',
    '..kyyk..',
    '..kyyk..',
    '...kk...',
  ], {'k': ink, 'p': Color(0xFFF7A8D8), 'w': Color(0xFFFFFFFF), 'y': Color(0xFFE6B36B)});

  static const shirt = PixelSprite([
    'kkk...kkk',
    'kbbkkkbbk',
    'kbbbbbbbk',
    'kkbbbbbkk',
    '.kbbbbbk.',
    '.kbbbbbk.',
    '.kbbbbbk.',
    '.kkkkkkk.',
  ], {'k': ink, 'b': Color(0xFF4A7BD0)});

  static const scarf = PixelSprite([
    '.kkkkkkk.',
    'krrwrrwrk',
    'kkkkkkrrk',
    '.....krwk',
    '.....krrk',
    '.....kwrk',
    '.....kkkk',
  ], {'k': ink, 'r': Color(0xFFE5484D), 'w': Color(0xFFFFF6E3)});

  static const bow = PixelSprite([
    'kk.....kk',
    'kpk.k.kpk',
    'kppkpkppk',
    'kpk.k.kpk',
    'kk.....kk',
  ], {'k': ink, 'p': Color(0xFFF27BB0)});

  static const hat = PixelSprite([
    '...kkkk...',
    '..kyyyyk..',
    '..kyyyyk..',
    '..krrrrk..',
    'kkkkkkkkkk',
    'kyyyyyyyyk',
    'kkkkkkkkkk',
  ], {'k': ink, 'y': Color(0xFFF2B544), 'r': Color(0xFFE5484D)});

  static const plant = PixelSprite([
    '.k..k...',
    'kgk.kgk.',
    'kggkkgk.',
    '.kgkgk..',
    '..kgk...',
    '.kkkkk..',
    '.kccck..',
    '.kccck..',
    '..kkk...',
  ], {'k': ink, 'g': Color(0xFF5BAA6B), 'c': Color(0xFFC96A3B)});

  static const lamp = PixelSprite([
    '..kkkk..',
    '.kyyyyk.',
    'kyyyyyyk',
    'kkkkkkkk',
    '...kk...',
    '...kk...',
    '...kk...',
    '.kkkkkk.',
    'kkkkkkkk',
  ], {'k': ink, 'y': Color(0xFFF7D36B)});

  // ---- Versões das roupas que acompanham o formato de cada bichinho ----
  /// Cachecol na vertical, em volta do corpo do tubarão.
  static const scarfVertical = PixelSprite([
    'kkkk',
    'krwk',
    'krrk',
    'kwrk',
    'krrk',
    'krwk',
    'kkkk',
    '.krk',
    '.kk.',
  ], {'k': ink, 'r': Color(0xFFE5484D), 'w': Color(0xFFFFF6E3)});

  /// Camiseta do tubarão: no meio do corpo, entre a barbatana e a cauda.
  static const shirtShark = PixelSprite([
    'kkkk',
    'kbbk',
    'kbbk',
    'kbbk',
    'kbbk',
    'kbbk',
    'kkkk',
  ], {'k': ink, 'b': Color(0xFF4A7BD0)});

  /// Camiseta do sapinho: uma faixa azul na barriga, sem cobrir o rosto.
  static const shirtFrog = PixelSprite([
    'bbbbbbbbbbbbbbbbbbb',
    'ccccccccccccccccccc',
  ], {'b': Color(0xFF4A7BD0), 'c': Color(0xFF2F5FB3)});

  /// Ícone do botão do guarda-roupa.
  static const hanger = PixelSprite([
    '....kk.....',
    '...k..k....',
    '......k....',
    '.....k.....',
    '...kkkkk...',
    '..k.....k..',
    '.k.......k.',
    'kkkkkkkkkkk',
  ], {'k': ink});
}
