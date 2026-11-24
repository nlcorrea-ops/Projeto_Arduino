import '../pixel/sprite.dart';
import '../pixel/sprites.dart';

/// Os três mascotes do app.
enum PetKind { tubarao, sapo, recruta }

/// Os cenários que o app sabe desenhar.
enum SceneKind { fundoDoMar, pantano, quartel, biblioteca }

/// Onde o mascote está agora. Ele só pode estar em um lugar por vez.
enum PetPlace { app, robo, viajando }

class PetInfo {
  const PetInfo({
    required this.kind,
    required this.defaultName,
    required this.habitat,
    required this.scene,
    required this.sprite,
    required this.lcdReady,
    required this.homeX,
    required this.homeY,
  });

  final PetKind kind;
  final String defaultName;
  final String habitat;
  final SceneKind scene;
  final PixelSprite sprite;

  /// Se o desenho deste mascote já existe na LCD do robô.
  final bool lcdReady;

  /// Posição do mascote no cenário (grade de 98 x 90).
  final double homeX;
  final double homeY;

  static PetInfo of(PetKind kind) => all[kind]!;

  static const Map<PetKind, PetInfo> all = {
    PetKind.tubarao: PetInfo(
      kind: PetKind.tubarao,
      defaultName: 'Tubarão',
      habitat: 'Fundo do mar',
      scene: SceneKind.fundoDoMar,
      sprite: Sprites.tubarao,
      lcdReady: true,
      homeX: 26,
      homeY: 40,
    ),
    PetKind.sapo: PetInfo(
      kind: PetKind.sapo,
      defaultName: 'Sapinho',
      habitat: 'Pântano',
      scene: SceneKind.pantano,
      sprite: Sprites.sapo,
      lcdReady: true,
      homeX: 28,
      homeY: 52,
    ),
    PetKind.recruta: PetInfo(
      kind: PetKind.recruta,
      defaultName: 'Recruta',
      habitat: 'Quartel-general',
      scene: SceneKind.quartel,
      sprite: Sprites.recruta,
      lcdReady: false,
      homeX: 33,
      homeY: 30,
    ),
  };
}
