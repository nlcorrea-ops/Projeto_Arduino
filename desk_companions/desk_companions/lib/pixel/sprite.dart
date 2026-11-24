import 'package:flutter/material.dart';

/// Um desenho em pixel art descrito por linhas de texto.
/// Cada letra é um pixel; a cor vem da paleta. "." é transparente.
class PixelSprite {
  const PixelSprite(this.rows, this.palette);

  final List<String> rows;
  final Map<String, Color> palette;

  int get width => rows.fold(0, (max, row) => row.length > max ? row.length : max);
  int get height => rows.length;

  /// Mesma forma com outras cores (usado nos ícones da barra de navegação).
  PixelSprite recolor(Map<String, Color> newPalette) => PixelSprite(rows, newPalette);
}

/// Desenha um sprite no canvas. [unit] é o tamanho de cada pixel.
void paintSprite(Canvas canvas, PixelSprite sprite, double x, double y, double unit) {
  final paint = Paint()..isAntiAlias = false;
  for (var row = 0; row < sprite.rows.length; row++) {
    final line = sprite.rows[row];
    var col = 0;
    while (col < line.length) {
      final char = line[col];
      final color = sprite.palette[char];
      if (color == null) {
        col++;
        continue;
      }
      var end = col;
      while (end < line.length && line[end] == char) {
        end++;
      }
      paint.color = color;
      // O "+ 0.02 * unit" evita linhas finas entre os pixels.
      canvas.drawRect(
        Rect.fromLTWH(
          x + col * unit,
          y + row * unit,
          (end - col) * unit + 0.02 * unit,
          unit + 0.02 * unit,
        ),
        paint,
      );
      col = end;
    }
  }
}

/// Widget que mostra um sprite em um tamanho fixo.
class SpriteView extends StatelessWidget {
  const SpriteView(this.sprite, {super.key, this.pixel = 4, this.semanticLabel});

  final PixelSprite sprite;
  final double pixel;
  final String? semanticLabel;

  @override
  Widget build(BuildContext context) {
    final view = CustomPaint(
      size: Size(sprite.width * pixel, sprite.height * pixel),
      painter: _SpritePainter(sprite),
    );
    if (semanticLabel == null) return ExcludeSemantics(child: view);
    return Semantics(label: semanticLabel, image: true, child: view);
  }
}

class _SpritePainter extends CustomPainter {
  _SpritePainter(this.sprite);
  final PixelSprite sprite;

  @override
  void paint(Canvas canvas, Size size) {
    final unit = size.width / sprite.width;
    paintSprite(canvas, sprite, 0, 0, unit);
  }

  @override
  bool shouldRepaint(_SpritePainter oldDelegate) => oldDelegate.sprite != sprite;
}
