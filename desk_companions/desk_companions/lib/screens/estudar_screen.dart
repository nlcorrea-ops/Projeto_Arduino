import 'package:flutter/material.dart';

import '../app_state.dart';
import '../models/pet.dart';
import '../models/shop.dart';
import '../pixel/scenes.dart';
import '../pixel/sprite.dart';
import '../pixel/sprites.dart';
import '../services/robot_link.dart';
import '../theme.dart';
import '../widgets/common.dart';
import 'sessao_screen.dart';

/// Posição do mascote sentado no chão da biblioteca.
({double x, double y}) libraryPosition(PetInfo info) => (
      x: 49 - info.sprite.width.toDouble(),
      y: 74 - info.sprite.height * 2.0,
    );

/// Topo com a biblioteca, usado nas telas de estudo.
class LibraryHeader extends StatelessWidget {
  const LibraryHeader({
    super.key,
    required this.title,
    required this.pill,
    this.onBack,
    this.showPause = false,
  });

  final String title;
  final Widget pill;
  final VoidCallback? onBack;
  final bool showPause;

  @override
  Widget build(BuildContext context) {
    final state = AppScope.of(context);
    final info = state.pet;
    final top = MediaQuery.paddingOf(context).top;
    final pos = libraryPosition(info);
    final showPet = state.place == PetPlace.app;

    return SizedBox(
      height: 310 + top,
      child: Stack(
        children: [
          Positioned.fill(
            child: SceneView(
              label: showPet ? '${state.petName} na biblioteca' : 'Biblioteca',
              painter: ScenePainter(
                scene: SceneKind.biblioteca,
                pet: showPet ? info.kind : null,
                petX: pos.x,
                petY: pos.y,
                showPause: showPause,
                // na biblioteca vão só as roupas e acessórios (a decoração fica em casa)
                itens: state.itemsOf(info.kind).where((i) => i.category != ShopCategory.decoracao).toList(),
              ),
            ),
          ),
          Positioned(
            top: top + 16,
            left: 0,
            right: 0,
            child: Column(
              children: [OutlinedTitle(title), const SizedBox(height: 8), pill],
            ),
          ),
          if (onBack != null)
            Positioned(top: top + 12, left: 16, child: BackSquareButton(onTap: onBack!)),
        ],
      ),
    );
  }
}

/// Escolha entre Pomodoro e cronômetro.
class EstudarScreen extends StatelessWidget {
  const EstudarScreen({super.key});

  @override
  Widget build(BuildContext context) {
    final state = AppScope.of(context);
    return Scaffold(
      body: SingleChildScrollView(
        child: Column(
          crossAxisAlignment: CrossAxisAlignment.stretch,
          children: [
            LibraryHeader(
              title: 'Biblioteca',
              pill: StatusPill(
                state.link.connectionLabel,
                dotColor: state.link.isConnected ? Px.green : const Color(0xFF9A95A3),
              ),
              onBack: () => Navigator.pop(context),
            ),
            Padding(
              padding: const EdgeInsets.fromLTRB(20, 22, 20, 24),
              child: Column(
                crossAxisAlignment: CrossAxisAlignment.stretch,
                children: [
                  Text('Como vai ser o estudo?', style: pixel(20)),
                  const SizedBox(height: 14),
                  _OptionCard(
                    icon: Sprites.clock,
                    title: 'Pomodoro',
                    description: '25 min de foco, com pausas automáticas',
                    color: Px.pomodoro,
                    onTap: () => _start(context, StudyMode.pomodoro),
                  ),
                  const SizedBox(height: 16),
                  _OptionCard(
                    icon: Sprites.stopwatch,
                    title: 'Cronômetro',
                    description: 'Tempo livre. Enter no robô pausa e retoma',
                    color: Px.stopwatch,
                    onTap: () => _start(context, StudyMode.cronometro),
                  ),
                  const SizedBox(height: 14),
                  Text(
                    'O tempo conta na tela do robô. Para encerrar a sessão, volte aqui no app.',
                    style: body(14, color: Px.muted),
                  ),
                ],
              ),
            ),
          ],
        ),
      ),
    );
  }

  Future<void> _start(BuildContext context, StudyMode mode) async {
    await AppScope.read(context).startStudy(mode);
    if (!context.mounted) return;
    Navigator.pushReplacement(
      context,
      MaterialPageRoute<void>(builder: (_) => const SessaoScreen()),
    );
  }
}

class _OptionCard extends StatelessWidget {
  const _OptionCard({
    required this.icon,
    required this.title,
    required this.description,
    required this.color,
    required this.onTap,
  });

  final PixelSprite icon;
  final String title;
  final String description;
  final Color color;
  final VoidCallback onTap;

  @override
  Widget build(BuildContext context) {
    return ChunkyButton(
      onTap: onTap,
      color: color,
      padding: const EdgeInsets.all(16),
      child: Row(
        children: [
          Container(
            width: 60,
            height: 60,
            decoration: BoxDecoration(
              color: Px.paper,
              borderRadius: BorderRadius.circular(12),
              border: Border.all(color: Px.ink, width: 3),
            ),
            child: Center(child: SpriteView(icon, pixel: 5)),
          ),
          const SizedBox(width: 16),
          Expanded(
            child: Column(
              crossAxisAlignment: CrossAxisAlignment.start,
              children: [
                Text(title, style: pixel(18)),
                const SizedBox(height: 4),
                Text(description, style: body(14)),
              ],
            ),
          ),
        ],
      ),
    );
  }
}
