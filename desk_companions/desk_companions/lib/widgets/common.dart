import 'package:flutter/material.dart';

import '../pixel/scenes.dart';
import '../pixel/sprite.dart';
import '../pixel/sprites.dart';
import '../theme.dart';

/// Botão "gordinho" com borda grossa e sombra dura.
class ChunkyButton extends StatelessWidget {
  const ChunkyButton({
    super.key,
    required this.child,
    required this.onTap,
    this.color = Px.paper,
    this.radius = 16,
    this.padding = const EdgeInsets.all(14),
    this.semanticLabel,
  });

  final Widget child;
  final VoidCallback? onTap;
  final Color color;
  final double radius;
  final EdgeInsets padding;
  final String? semanticLabel;

  @override
  Widget build(BuildContext context) {
    final enabled = onTap != null;
    return Semantics(
      button: true,
      enabled: enabled,
      label: semanticLabel,
      child: Material(
        type: MaterialType.transparency,
        child: Ink(
          decoration: enabled
              ? chunky(color: color, radius: radius)
              : BoxDecoration(
                  color: const Color(0xFFEDE8DC),
                  borderRadius: BorderRadius.circular(radius),
                  border: Border.all(color: const Color(0xFF8A8494), width: 3),
                ),
          child: InkWell(
            onTap: onTap,
            customBorder: RoundedRectangleBorder(borderRadius: BorderRadius.circular(radius)),
            child: Padding(padding: padding, child: child),
          ),
        ),
      ),
    );
  }
}

/// Cenário em pixel art que preenche o espaço disponível.
class SceneView extends StatelessWidget {
  const SceneView({super.key, required this.painter, required this.label});

  final ScenePainter painter;
  final String label;

  @override
  Widget build(BuildContext context) {
    return Semantics(
      label: label,
      image: true,
      child: CustomPaint(painter: painter, child: const SizedBox.expand()),
    );
  }
}

/// Nome do mascote em letras brancas com contorno escuro.
class OutlinedTitle extends StatelessWidget {
  const OutlinedTitle(this.text, {super.key, this.size = 30});

  final String text;
  final double size;

  @override
  Widget build(BuildContext context) {
    final base = TextStyle(fontFamily: 'Silkscreen', fontSize: size, fontWeight: FontWeight.w700, height: 1.1);
    return Stack(
      children: [
        Text(
          text,
          style: base.copyWith(
            foreground: Paint()
              ..style = PaintingStyle.stroke
              ..strokeWidth = 6
              ..strokeJoin = StrokeJoin.miter
              ..color = Px.ink,
          ),
        ),
        Text(text, style: base.copyWith(color: Colors.white)),
      ],
    );
  }
}

/// Etiqueta arredondada com uma bolinha de status.
class StatusPill extends StatelessWidget {
  const StatusPill(this.text, {super.key, this.dotColor = Px.green});

  final String text;
  final Color dotColor;

  @override
  Widget build(BuildContext context) {
    return Container(
      padding: const EdgeInsets.symmetric(horizontal: 10, vertical: 4),
      decoration: BoxDecoration(
        color: Px.paper.withValues(alpha: 0.95),
        borderRadius: BorderRadius.circular(999),
        border: Border.all(color: Px.ink, width: 2),
      ),
      child: Row(
        mainAxisSize: MainAxisSize.min,
        children: [
          Container(
            width: 10,
            height: 10,
            decoration: BoxDecoration(color: dotColor, border: Border.all(color: Px.ink, width: 2)),
          ),
          const SizedBox(width: 6),
          Text(text, style: body(13, weight: 600)),
        ],
      ),
    );
  }
}

/// Quantidade de moedas.
class CoinChip extends StatelessWidget {
  const CoinChip(this.coins, {super.key});

  final int coins;

  @override
  Widget build(BuildContext context) {
    return Semantics(
      label: '$coins moedas',
      excludeSemantics: true,
      child: Container(
        height: 40,
        padding: const EdgeInsets.symmetric(horizontal: 10),
        decoration: BoxDecoration(
          color: Px.paper,
          borderRadius: BorderRadius.circular(10),
          border: Border.all(color: Px.ink, width: 3),
        ),
        child: Row(
          mainAxisSize: MainAxisSize.min,
          children: [
            const SpriteView(Sprites.coin, pixel: 3),
            const SizedBox(width: 6),
            Text('$coins', style: pixel(14, bold: false)),
          ],
        ),
      ),
    );
  }
}

/// Barra de saúde e felicidade (os valores vêm do robô).
class StatsCard extends StatelessWidget {
  const StatsCard({super.key, required this.health, required this.happiness, required this.note});

  final int health;
  final int happiness;
  final String note;

  @override
  Widget build(BuildContext context) {
    return Container(
      width: 318,
      padding: const EdgeInsets.fromLTRB(14, 10, 14, 8),
      decoration: chunky(radius: 14),
      child: Column(
        mainAxisSize: MainAxisSize.min,
        children: [
          Row(
            mainAxisAlignment: MainAxisAlignment.spaceBetween,
            children: [
              Semantics(
                label: 'Saúde: $health de 4',
                excludeSemantics: true,
                child: Row(
                  children: [
                    for (var i = 0; i < 4; i++)
                      Padding(
                        padding: const EdgeInsets.only(right: 5),
                        child: SpriteView(i < health ? Sprites.heart : Sprites.heartEmpty, pixel: 4),
                      ),
                  ],
                ),
              ),
              Container(width: 3, height: 28, color: Px.line),
              Semantics(
                label: 'Felicidade: $happiness de 4',
                excludeSemantics: true,
                child: Row(
                  children: [
                    for (var i = 0; i < 4; i++)
                      Padding(
                        padding: const EdgeInsets.only(left: 5),
                        child: SpriteView(i < happiness ? Sprites.smile : Sprites.smileEmpty, pixel: 4),
                      ),
                  ],
                ),
              ),
            ],
          ),
          const SizedBox(height: 6),
          Text(note, style: body(12, color: Px.muted)),
        ],
      ),
    );
  }
}

/// Os dois botões grandes da tela do mascote.
class BigActionButton extends StatelessWidget {
  const BigActionButton({
    super.key,
    required this.icon,
    required this.label,
    required this.subtitle,
    required this.color,
    required this.onTap,
  });

  final PixelSprite icon;
  final String label;
  final String subtitle;
  final Color color;
  final VoidCallback? onTap;

  @override
  Widget build(BuildContext context) {
    return ChunkyButton(
      onTap: onTap,
      color: color,
      radius: 18,
      padding: const EdgeInsets.symmetric(horizontal: 10, vertical: 18),
      child: SizedBox(
        height: 110,
        width: double.infinity,
        child: Column(
          mainAxisAlignment: MainAxisAlignment.center,
          children: [
            SpriteView(icon, pixel: 4),
            const SizedBox(height: 10),
            FittedBox(fit: BoxFit.scaleDown, child: Text(label, style: pixel(16), maxLines: 1)),
            const SizedBox(height: 4),
            Text(subtitle, style: body(13, weight: 500), textAlign: TextAlign.center),
          ],
        ),
      ),
    );
  }
}

/// Botão quadrado de voltar.
class BackSquareButton extends StatelessWidget {
  const BackSquareButton({super.key, required this.onTap});

  final VoidCallback onTap;

  @override
  Widget build(BuildContext context) {
    return SizedBox(
      width: 52,
      height: 52,
      child: ChunkyButton(
        onTap: onTap,
        radius: 12,
        padding: EdgeInsets.zero,
        semanticLabel: 'Voltar',
        child: Center(child: Text('<', style: pixel(20))),
      ),
    );
  }
}

/// Barra de navegação inferior com as abas Pet e Loja.
class PixelNavBar extends StatelessWidget {
  const PixelNavBar({super.key, required this.index, required this.onChanged});

  final int index;
  final ValueChanged<int> onChanged;

  @override
  Widget build(BuildContext context) {
    final bottom = MediaQuery.paddingOf(context).bottom;
    return Container(
      decoration: const BoxDecoration(
        color: Px.paper,
        border: Border(top: BorderSide(color: Px.ink, width: 3)),
      ),
      padding: EdgeInsets.fromLTRB(24, 8, 24, bottom > 0 ? bottom : 12),
      child: Row(
        children: [
          _item(0, 'Pet', Sprites.paw),
          const SizedBox(width: 8),
          _item(1, 'Loja', Sprites.bag),
        ],
      ),
    );
  }

  Widget _item(int i, String label, PixelSprite sprite) {
    final on = i == index;
    final color = on ? Px.blue : const Color(0xFF5E5A66);
    final icon = sprite.recolor({'k': color, 'y': on ? Px.yellow : Px.line});
    return Expanded(
      child: Semantics(
        selected: on,
        button: true,
        label: label,
        excludeSemantics: true,
        child: InkWell(
          onTap: () => onChanged(i),
          borderRadius: BorderRadius.circular(12),
          child: SizedBox(
            height: 60,
            child: Column(
              mainAxisAlignment: MainAxisAlignment.center,
              children: [
                SpriteView(icon, pixel: 3),
                const SizedBox(height: 6),
                Text(label, style: pixel(13, color: color, bold: false)),
              ],
            ),
          ),
        ),
      ),
    );
  }
}
