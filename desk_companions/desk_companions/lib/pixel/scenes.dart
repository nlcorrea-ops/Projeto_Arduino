import 'dart:math';

import 'package:flutter/foundation.dart' show listEquals;
import 'package:flutter/material.dart';

import '../models/pet.dart';
import '../models/shop.dart';
import 'sprite.dart';
import 'sprites.dart';
import 'vestir.dart';

/// Os cenários são desenhados numa grade de 98 x 90 "pixels grandes"
/// e depois ampliados para preencher o espaço da tela.
const double sceneW = 98;
const double sceneH = 90;

typedef _Block = ({double x, double y, double w, double h, Color c});

class _Builder {
  final List<_Block> blocks = [];
  void r(num x, num y, num w, num h, int color) {
    blocks.add((x: x.toDouble(), y: y.toDouble(), w: w.toDouble(), h: h.toDouble(), c: Color(color)));
  }
}

final Map<SceneKind, List<_Block>> _cache = {};

List<_Block> _sceneBlocks(SceneKind kind) {
  return _cache.putIfAbsent(kind, () {
    final b = _Builder();
    final rng = Random(kind.index + 7);
    switch (kind) {
      case SceneKind.fundoDoMar:
        _fundoDoMar(b, rng);
      case SceneKind.pantano:
        _pantano(b, rng);
      case SceneKind.quartel:
        _quartel(b, rng);
      case SceneKind.biblioteca:
        _biblioteca(b, rng);
    }
    return b.blocks;
  });
}

void _fundoDoMar(_Builder b, Random rng) {
  const bands = [
    (0, 14, 0xFF9BE7F2),
    (14, 28, 0xFF7AD7EA),
    (28, 44, 0xFF5BC0DE),
    (44, 62, 0xFF43A6CD),
    (62, 78, 0xFF348BB8),
  ];
  for (final (top, bottom, color) in bands) {
    b.r(0, top, sceneW, bottom - top, color);
  }
  for (var i = 0; i < 6; i++) {
    final x = 10 + i * 16;
    b.r(x, 0, 3, 26, 0xFFB5EEF6);
    b.r(x + 1, 26, 2, 10, 0xFF94E2F0);
  }
  b.r(0, 76, sceneW, 4, 0xFFEBD49B);
  b.r(0, 80, sceneW, 10, 0xFFDCBE80);
  for (var i = 0; i < 18; i++) {
    b.r(rng.nextInt(97), 81 + rng.nextInt(8), 1, 1, 0xFFC7A766);
  }
  for (final (x, w) in const [(20, 8), (70, 10)]) {
    b.r(x, 72, w, 4, 0xFF7F8794);
    b.r(x + 1, 70, w - 2, 2, 0xFF949CA8);
  }
  for (final (x, h) in const [(5, 30), (11, 22), (84, 26), (90, 34), (60, 16)]) {
    for (var j = 0; j < h; j++) {
      final off = (j ~/ 3).isOdd ? 1 : 0;
      b.r(x + off, 76 - j, 2, 1, 0xFF3FAE6E);
      b.r(x + off + 2, 76 - j, 1, 1, 0xFF2B8A55);
    }
  }
  for (var i = 0; i < 9; i++) {
    final x = 4 + rng.nextInt(89);
    final y = 6 + rng.nextInt(55);
    if (rng.nextBool()) {
      b.r(x, y, 1, 1, 0xFFE9FBFF);
    } else {
      b.r(x + 1, y, 1, 1, 0xFFE9FBFF);
      b.r(x, y + 1, 1, 1, 0xFFE9FBFF);
      b.r(x + 2, y + 1, 1, 1, 0xFFE9FBFF);
      b.r(x + 1, y + 2, 1, 1, 0xFFE9FBFF);
    }
  }
}

void _pantano(_Builder b, Random rng) {
  b.r(0, 0, sceneW, 20, 0xFFDDEFC6);
  b.r(0, 20, sceneW, 18, 0xFFC8E3AC);
  for (var x = -4; x < sceneW; x += 11) {
    final h = 8 + rng.nextInt(7);
    b.r(x, 38 - h, 9, h, 0xFF86AE6B);
    b.r(x + 2, 38 - h - 3, 5, 3, 0xFF86AE6B);
  }
  for (var x = 2; x < sceneW; x += 17) {
    b.r(x, 30, 2, 8, 0xFF6E5536);
  }
  b.r(0, 38, sceneW, 4, 0xFF76A76E);
  b.r(0, 42, sceneW, 16, 0xFF6B9E73);
  b.r(0, 58, sceneW, 16, 0xFF5B8C64);
  b.r(0, 74, sceneW, 16, 0xFF4C7A56);
  for (var i = 0; i < 14; i++) {
    b.r(rng.nextInt(91), 46 + rng.nextInt(43), 3 + rng.nextInt(5), 1, 0xFF8FBF95);
  }
  for (final (x, y, w) in const [(8, 60, 14), (74, 52, 12), (60, 80, 16)]) {
    b.r(x + 1, y, w - 2, 1, 0xFF4F8A33);
    b.r(x, y + 1, w, 3, 0xFF86C45A);
    b.r(x + 1, y + 4, w - 2, 1, 0xFF4F8A33);
    b.r(x + w ~/ 2, y + 1, 1, 2, 0xFF4F8A33);
  }
  for (final x in const [4, 30, 88, 93]) {
    b.r(x, 44, 1, 22, 0xFF5E7F3A);
    b.r(x - 1, 40, 3, 6, 0xFF6B4426);
  }
  b.r(79, 49, 2, 2, 0xFFF5B5C8);
  b.r(78, 50, 1, 1, 0xFFF5B5C8);
  b.r(81, 50, 1, 1, 0xFFF5B5C8);
}

void _quartel(_Builder b, Random rng) {
  b.r(0, 0, sceneW, 64, 0xFF4E5D4A);
  for (var y = 9; y < 64; y += 11) {
    b.r(0, y, sceneW, 1, 0xFF44523F);
  }
  for (var x = 0; x < sceneW; x += 14) {
    for (var y = 3; y < 64; y += 11) {
      b.r(x + 2, y, 1, 1, 0xFF6A7A63);
    }
  }
  b.r(0, 0, sceneW, 3, 0xFF3B4738);
  b.r(30, 3, 38, 2, 0xFFF7D36B);
  // Mapa na parede
  b.r(6, 10, 34, 24, 0xFF8B6A3E);
  b.r(7, 11, 32, 22, 0xFFE2D3A8);
  b.r(10, 16, 10, 1, 0xFF7A6A4E);
  b.r(10, 24, 14, 1, 0xFF7A6A4E);
  b.r(23, 16, 1, 9, 0xFF7A6A4E);
  b.r(28, 20, 8, 1, 0xFF7A6A4E);
  for (final (x, y) in const [(11, 15), (24, 23), (34, 19)]) {
    b.r(x, y, 2, 2, 0xFFD9534F);
  }
  // Radar
  b.r(60, 10, 30, 22, 0xFF2A332C);
  b.r(62, 12, 26, 18, 0xFF17211B);
  for (var y = 13; y < 30; y += 3) {
    b.r(63, y, 24, 1, 0xFF1D2A22);
  }
  b.r(74, 14, 1, 14, 0xFF3E8E5E);
  b.r(64, 21, 22, 1, 0xFF3E8E5E);
  b.r(77, 17, 2, 2, 0xFF7CF0A6);
  b.r(68, 24, 2, 2, 0xFF7CF0A6);
  // Chão
  b.r(0, 64, sceneW, 26, 0xFF7C6B55);
  for (var x = 0; x < sceneW; x += 12) {
    b.r(x, 64, 6, 13, 0xFF74644F);
    b.r(x + 6, 77, 6, 13, 0xFF74644F);
  }
  b.r(0, 64, sceneW, 1, 0xFF5E5141);
  // Caixas
  for (final (x, y, w, h) in const [(4, 54, 16, 12), (8, 44, 12, 10), (78, 56, 16, 10)]) {
    b.r(x, y, w, h, 0xFFB9894B);
    b.r(x, y, w, 1, 0xFF8A6532);
    b.r(x, y + h - 1, w, 1, 0xFF8A6532);
    b.r(x, y, 1, h, 0xFF8A6532);
    b.r(x + w - 1, y, 1, h, 0xFF8A6532);
    b.r(x + 2, y + h ~/ 2, w - 4, 1, 0xFFA47940);
  }
  // Porta
  b.r(46, 40, 8, 24, 0xFF5A6A55);
  b.r(47, 41, 6, 6, 0xFF3B4738);
  b.r(48, 42, 1, 1, 0xFFE5484D);
  b.r(50, 42, 1, 1, 0xFF7CF0A6);
}

void _biblioteca(_Builder b, Random rng) {
  const bookColors = [0xFFD9534F, 0xFF4A7BD0, 0xFFE9A23B, 0xFF5BAA6B, 0xFF9B6BC9, 0xFFE07A9A, 0xFF3E8E9E];
  b.r(0, 0, sceneW, 64, 0xFFF0DDB6);
  for (final shelfX in const [3, 59]) {
    b.r(shelfX, 4, 36, 58, 0xFF8C5A34);
    b.r(shelfX + 2, 6, 32, 56, 0xFF6E4428);
    for (var shelfY = 6; shelfY < 60; shelfY += 13) {
      var x = shelfX + 2;
      while (x < shelfX + 33) {
        final w = rng.nextInt(3) == 0 ? 3 : 2;
        final h = 8 + rng.nextInt(4);
        b.r(x, shelfY + 11 - h, w, h, bookColors[rng.nextInt(bookColors.length)]);
        b.r(x, shelfY + 13 - h, w, 1, 0xFFFFF6E3);
        x += w + (rng.nextDouble() < 0.25 ? 1 : 0);
      }
      b.r(shelfX, shelfY + 11, 36, 2, 0xFF8C5A34);
    }
  }
  // Janela
  b.r(42, 8, 14, 22, 0xFF8C5A34);
  b.r(43, 9, 12, 20, 0xFFBDE6F5);
  b.r(48, 9, 1, 20, 0xFF8C5A34);
  b.r(43, 18, 12, 1, 0xFF8C5A34);
  b.r(44, 10, 3, 2, 0xFFFFFFFF);
  // Chão e tapete
  b.r(0, 64, sceneW, 26, 0xFFC8925B);
  for (var y = 68; y < sceneH; y += 5) {
    b.r(0, y, sceneW, 1, 0xFFB78350);
  }
  b.r(24, 74, 50, 12, 0xFFC95B5B);
  b.r(26, 76, 46, 8, 0xFFD77373);
}

/// Efeitos do portal (usados na tela de teletransporte).
class PortalFx {
  const PortalFx({required this.pulse, required this.sparkle});

  /// 0 a 1, faz o portal "respirar".
  final double pulse;

  /// 0 a 1, faz as partículas piscarem e se moverem.
  final double sparkle;
}

/// Pinta um cenário completo: fundo, mascote e efeitos.
class ScenePainter extends CustomPainter {
  ScenePainter({
    required this.scene,
    this.pet,
    this.petX = 0,
    this.petY = 0,
    this.petOpacity = 1,
    this.dim = 0,
    this.portal,
    this.showPause = false,
    this.view = const Rect.fromLTWH(0, 0, sceneW, sceneH),
    this.itens = const [],
  });

  final SceneKind scene;
  final PetKind? pet;

  /// Posição do mascote na grade do cenário (0 a 98, 0 a 90).
  final double petX;
  final double petY;
  final double petOpacity;

  /// 0 a 1: escurece o cenário.
  final double dim;
  final PortalFx? portal;
  final bool showPause;

  /// Parte do cenário que aparece (útil para miniaturas).
  final Rect view;

  /// Roupas, acessórios e decorações que o bichinho está usando.
  final List<ShopItem> itens;

  @override
  void paint(Canvas canvas, Size size) {
    // "Cobrir" o espaço, como background-size: cover.
    final scale = max(size.width / view.width, size.height / view.height);
    final dx = (size.width - view.width * scale) / 2 - view.left * scale;
    final dy = (size.height - view.height * scale) / 2 - view.top * scale;

    canvas.save();
    canvas.clipRect(Offset.zero & size);
    canvas.translate(dx, dy);
    canvas.scale(scale);

    final paint = Paint()..isAntiAlias = false;
    for (final block in _sceneBlocks(scene)) {
      paint.color = block.c;
      canvas.drawRect(Rect.fromLTWH(block.x, block.y, block.w + 0.02, block.h + 0.02), paint);
    }

    pintaDecoracao(canvas, scene, itens);

    if (dim > 0) {
      paint.color = Sprites.ink.withValues(alpha: 0.45 * dim);
      canvas.drawRect(const Rect.fromLTWH(0, 0, sceneW, sceneH), paint);
    }

    final currentPet = pet;
    if (currentPet != null && petOpacity > 0) {
      final sprite = PetInfo.of(currentPet).sprite;
      if (petOpacity < 1) {
        canvas.saveLayer(null, Paint()..color = Color.fromRGBO(0, 0, 0, petOpacity));
        paintSprite(canvas, sprite, petX, petY, 2);
        pintaRoupas(canvas, currentPet, petX, petY, itens);
        canvas.restore();
      } else {
        paintSprite(canvas, sprite, petX, petY, 2);
        pintaRoupas(canvas, currentPet, petX, petY, itens);
      }
    }

    final fx = portal;
    if (fx != null) _paintPortal(canvas, fx);

    if (showPause) paintSprite(canvas, Sprites.pause, 62, 12, 2);

    canvas.restore();
  }

  void _paintPortal(Canvas canvas, PortalFx fx) {
    const cx = 88.0;
    const cy = 46.0;
    final grow = 1 + 0.08 * sin(fx.pulse * 2 * pi);
    _ellipse(canvas, cx, cy, 14 * grow, 26 * grow, const Color(0x59D6A0FF));
    _ellipse(canvas, cx, cy, 11 * grow, 22 * grow, const Color(0xFF7B3FE4));
    _ellipse(canvas, cx, cy, 9 * grow, 19 * grow, const Color(0xFFB15EF2));
    _ellipse(canvas, cx, cy, 7 * grow, 15 * grow, const Color(0xFFE58BF2));
    _ellipse(canvas, cx, cy, 4 * grow, 10 * grow, const Color(0xFFFFE8FF));

    final paint = Paint()..isAntiAlias = false;
    const sparkles = [(70.0, 14.0), (94.0, 72.0), (66.0, 74.0), (76.0, 8.0), (60.0, 30.0)];
    for (var i = 0; i < sparkles.length; i++) {
      final visible = sin((fx.sparkle + i / sparkles.length) * 2 * pi) > -0.3;
      if (!visible) continue;
      final (x, y) = sparkles[i];
      paint.color = const Color(0xFFFFF3A8);
      canvas.drawRect(Rect.fromLTWH(x, y - 2, 1, 5), paint);
      canvas.drawRect(Rect.fromLTWH(x - 2, y, 5, 1), paint);
    }
    for (final (x, y) in const [(72.0, 22.0), (64.0, 60.0), (80.0, 80.0)]) {
      final bob = sin((fx.sparkle + x / 98) * 2 * pi) * 1.5;
      paintSprite(canvas, Sprites.heartPink, x, y + bob, 1);
    }
    const particles = [
      (58.0, 40.0, 0xFFC9A6FF),
      (54.0, 48.0, 0xFF7FE3FF),
      (62.0, 52.0, 0xFFC9A6FF),
      (50.0, 42.0, 0xFFFFFFFF),
      (66.0, 36.0, 0xFF7FE3FF),
      (46.0, 50.0, 0xFFC9A6FF),
      (57.0, 56.0, 0xFFFFFFFF),
    ];
    for (var i = 0; i < particles.length; i++) {
      final (x, y, color) = particles[i];
      final drift = ((fx.sparkle + i / particles.length) % 1) * 10;
      paint.color = Color(color);
      canvas.drawRect(Rect.fromLTWH(x + drift, y, 2, 2), paint);
    }
  }

  void _ellipse(Canvas canvas, double cx, double cy, double rx, double ry, Color color) {
    final paint = Paint()
      ..color = color
      ..isAntiAlias = false;
    final rows = ry.round();
    for (var dy = -rows; dy <= rows; dy++) {
      final t = 1 - pow(dy / ry, 2);
      if (t < 0) continue;
      final half = (rx * sqrt(t)).roundToDouble();
      canvas.drawRect(Rect.fromLTWH(cx - half, cy + dy, 2 * half + 1, 1.02), paint);
    }
  }

  @override
  bool shouldRepaint(ScenePainter old) =>
      old.scene != scene ||
      old.pet != pet ||
      old.petX != petX ||
      old.petY != petY ||
      old.petOpacity != petOpacity ||
      old.dim != dim ||
      old.portal?.pulse != portal?.pulse ||
      old.portal?.sparkle != portal?.sparkle ||
      old.showPause != showPause ||
      old.view != view ||
      !listEquals(old.itens, itens);
}
