import 'package:flutter/material.dart';

import '../app_state.dart';
import '../models/pet.dart';
import '../pixel/scenes.dart';
import '../pixel/sprite.dart';
import '../pixel/sprites.dart';
import '../theme.dart';
import '../widgets/common.dart';
import 'escolha_screen.dart';
import 'estudar_screen.dart';
import 'guarda_roupa_screen.dart';
import 'portal_screen.dart';

/// Aba "Pet": o mascote no seu cenário, com Estudar e Teletransporte.
class PetTab extends StatelessWidget {
  const PetTab({super.key});

  @override
  Widget build(BuildContext context) {
    final state = AppScope.of(context);
    final info = state.pet;
    final name = state.petName;
    final top = MediaQuery.paddingOf(context).top;
    final onRobot = state.place == PetPlace.robo;
    final traveling = state.place == PetPlace.viajando;
    final sceneHeight = 330 + top;

    return SingleChildScrollView(
      child: Column(
        children: [
          SizedBox(
            height: sceneHeight + 44,
            child: Stack(
              children: [
                Positioned(
                  left: 0,
                  right: 0,
                  top: 0,
                  height: sceneHeight,
                  child: SceneView(
                    label: onRobot ? '${info.habitat} vazio' : '$name no cenário ${info.habitat}',
                    painter: ScenePainter(
                      scene: info.scene,
                      pet: onRobot || traveling ? null : info.kind,
                      petX: info.homeX,
                      petY: info.homeY,
                      dim: onRobot ? 0.5 : 0,
                      itens: state.itemsOf(info.kind),
                    ),
                  ),
                ),
                Positioned(
                  top: top + 16,
                  left: 0,
                  right: 0,
                  child: Column(
                    children: [
                      OutlinedTitle(name),
                      const SizedBox(height: 8),
                      onRobot
                          ? const StatusPill('No robô agora', dotColor: Px.purple)
                          : StatusPill(
                              state.link.connectionLabel,
                              dotColor: state.link.isConnected ? Px.green : const Color(0xFF9A95A3),
                            ),
                    ],
                  ),
                ),
                Positioned(
                  top: top + 12,
                  left: 16,
                  child: _SwitcherButton(info: info, onTap: () => _openSwitcher(context)),
                ),
                Positioned(top: top + 12, right: 16, child: CoinChip(state.coins)),
                Positioned(
                  top: sceneHeight - 100,
                  right: 16,
                  child: SizedBox(
                    width: 56,
                    height: 56,
                    child: ChunkyButton(
                      radius: 14,
                      padding: EdgeInsets.zero,
                      semanticLabel: 'Abrir guarda-roupa',
                      onTap: () => Navigator.push(
                        context,
                        MaterialPageRoute<void>(builder: (_) => const GuardaRoupaScreen()),
                      ),
                      child: const Center(child: SpriteView(Sprites.hanger, pixel: 4)),
                    ),
                  ),
                ),
                if (onRobot)
                  Positioned(
                    top: top + 120,
                    left: 0,
                    right: 0,
                    child: Center(child: _OffAirCard(name: name)),
                  ),
                Positioned(
                  top: sceneHeight - 34,
                  left: 0,
                  right: 0,
                  child: Center(
                    child: StatsCard(
                      health: state.health,
                      happiness: state.happiness,
                      note: 'Vem do robô, onde ficam os cuidados',
                    ),
                  ),
                ),
              ],
            ),
          ),
          Padding(
            padding: const EdgeInsets.fromLTRB(20, 30, 20, 0),
            child: Row(
              children: [
                Expanded(
                  child: BigActionButton(
                    icon: Sprites.book,
                    label: 'Estudar',
                    subtitle: 'Pomodoro ou cronômetro',
                    color: Px.study,
                    onTap: () => Navigator.push(
                      context,
                      MaterialPageRoute<void>(builder: (_) => const EstudarScreen()),
                    ),
                  ),
                ),
                const SizedBox(width: 14),
                Expanded(
                  child: BigActionButton(
                    icon: Sprites.portal,
                    label: 'Teletransporte',
                    subtitle: onRobot ? 'Trazer de volta' : 'Mandar para o robô',
                    color: Px.teleport,
                    onTap: traveling
                        ? null
                        : () => Navigator.push(
                              context,
                              MaterialPageRoute<void>(builder: (_) => const PortalScreen()),
                            ),
                  ),
                ),
              ],
            ),
          ),
          const SizedBox(height: 20),
          Row(
            mainAxisAlignment: MainAxisAlignment.center,
            children: [
              Container(
                width: 12,
                height: 12,
                decoration: BoxDecoration(
                  color: onRobot ? Px.purple : Px.green,
                  border: Border.all(color: Px.ink, width: 2),
                ),
              ),
              const SizedBox(width: 8),
              Text(
                onRobot ? '$name está no robô' : '$name está aqui no app',
                style: body(14, color: Px.muted),
              ),
            ],
          ),
          const SizedBox(height: 24),
        ],
      ),
    );
  }

  void _openSwitcher(BuildContext context) {
    final state = AppScope.read(context);
    if (state.place != PetPlace.app) {
      ScaffoldMessenger.of(context).showSnackBar(
        SnackBar(content: Text('Traga o ${state.petName} de volta antes de trocar de mascote.')),
      );
      return;
    }
    Navigator.push(
      context,
      MaterialPageRoute<void>(builder: (_) => const EscolhaScreen(isSwitching: true)),
    );
  }
}

class _SwitcherButton extends StatelessWidget {
  const _SwitcherButton({required this.info, required this.onTap});

  final PetInfo info;
  final VoidCallback onTap;

  @override
  Widget build(BuildContext context) {
    return SizedBox(
      height: 52,
      child: ChunkyButton(
        onTap: onTap,
        radius: 12,
        padding: const EdgeInsets.symmetric(horizontal: 10),
        semanticLabel: 'Trocar de mascote',
        child: Row(
          mainAxisSize: MainAxisSize.min,
          children: [
            SpriteView(info.sprite, pixel: 34 / info.sprite.width),
            const SizedBox(width: 6),
            const SpriteView(Sprites.caret, pixel: 2),
          ],
        ),
      ),
    );
  }
}

class _OffAirCard extends StatelessWidget {
  const _OffAirCard({required this.name});

  final String name;

  @override
  Widget build(BuildContext context) {
    return Container(
      width: 240,
      padding: const EdgeInsets.all(16),
      decoration: chunky(),
      child: Column(
        mainAxisSize: MainAxisSize.min,
        children: [
          const SpriteView(Sprites.noSignal, pixel: 4),
          const SizedBox(height: 10),
          Text('Pet fora do ar', style: pixel(18)),
          const SizedBox(height: 6),
          Text(
            'O $name está passeando na tela do robô.',
            style: body(14),
            textAlign: TextAlign.center,
          ),
        ],
      ),
    );
  }
}
