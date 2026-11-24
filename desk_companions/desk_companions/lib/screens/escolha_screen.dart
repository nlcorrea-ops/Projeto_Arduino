import 'package:flutter/material.dart';

import '../app_state.dart';
import '../models/pet.dart';
import '../pixel/scenes.dart';
import '../theme.dart';
import '../widgets/common.dart';
import 'home_shell.dart';

/// Primeira tela: escolher qual mascote vai para a tela do robô.
/// Também é usada para trocar de mascote (isSwitching = true).
class EscolhaScreen extends StatelessWidget {
  const EscolhaScreen({super.key, this.isSwitching = false});

  final bool isSwitching;

  @override
  Widget build(BuildContext context) {
    final state = AppScope.of(context);
    return Scaffold(
      body: SafeArea(
        child: Column(
          crossAxisAlignment: CrossAxisAlignment.stretch,
          children: [
            Expanded(
              child: ListView(
                padding: const EdgeInsets.fromLTRB(20, 20, 20, 20),
                children: [
                  if (isSwitching) ...[
                    Align(
                      alignment: Alignment.centerLeft,
                      child: BackSquareButton(onTap: () => Navigator.pop(context)),
                    ),
                    const SizedBox(height: 16),
                  ],
                  Text('Quem vai para a mesa?', style: pixel(26)),
                  const SizedBox(height: 8),
                  Text('Escolha o companheiro que vai morar na tela do robô.', style: body(16, color: Px.muted)),
                  const SizedBox(height: 22),
                  for (final kind in PetKind.values) ...[
                    _PetCard(
                      info: PetInfo.of(kind),
                      selected: state.selected == kind,
                      onTap: () => AppScope.read(context).selectPet(kind),
                    ),
                    const SizedBox(height: 18),
                  ],
                ],
              ),
            ),
            Padding(
              padding: const EdgeInsets.fromLTRB(20, 0, 20, 20),
              child: ChunkyButton(
                color: Px.yellow,
                onTap: () {
                  if (isSwitching) {
                    Navigator.pop(context);
                  } else {
                    Navigator.pushReplacement(
                      context,
                      MaterialPageRoute<void>(builder: (_) => const HomeShell()),
                    );
                  }
                },
                child: SizedBox(
                  height: 32,
                  child: Center(
                    child: Text(
                      isSwitching ? 'Trocar para ${state.petName}' : 'Conectar ao robô',
                      style: pixel(18),
                    ),
                  ),
                ),
              ),
            ),
          ],
        ),
      ),
    );
  }
}

class _PetCard extends StatelessWidget {
  const _PetCard({required this.info, required this.selected, required this.onTap});

  final PetInfo info;
  final bool selected;
  final VoidCallback onTap;

  @override
  Widget build(BuildContext context) {
    final card = ChunkyButton(
      onTap: onTap,
      radius: 18,
      padding: const EdgeInsets.all(12),
      semanticLabel: '${info.defaultName}, ${info.habitat}${selected ? ', selecionado' : ''}',
      child: Row(
        children: [
          Container(
            width: 112,
            height: 112,
            decoration: BoxDecoration(
              border: Border.all(color: Px.ink, width: 3),
              borderRadius: BorderRadius.circular(12),
            ),
            child: ClipRRect(
              borderRadius: BorderRadius.circular(9),
              child: CustomPaint(
                painter: ScenePainter(
                  scene: info.scene,
                  pet: info.kind,
                  petX: info.homeX,
                  petY: info.homeY,
                  view: const Rect.fromLTWH(12, 20, 74, 70),
                  itens: AppScope.of(context).itemsOf(info.kind),
                ),
              ),
            ),
          ),
          const SizedBox(width: 16),
          Expanded(
            child: Column(
              crossAxisAlignment: CrossAxisAlignment.start,
              children: [
                if (selected) ...[
                  Text('Selecionado', style: pixel(11, color: Px.blue, bold: false)),
                  const SizedBox(height: 4),
                ],
                Text(info.defaultName, style: pixel(22)),
                const SizedBox(height: 4),
                Text(info.habitat, style: body(15, color: Px.muted)),
                const SizedBox(height: 8),
                Container(
                  padding: const EdgeInsets.symmetric(horizontal: 8, vertical: 3),
                  decoration: BoxDecoration(
                    color: info.lcdReady ? const Color(0xFFBFE8C8) : const Color(0xFFFFE1A6),
                    borderRadius: BorderRadius.circular(6),
                    border: Border.all(color: Px.ink, width: 2),
                  ),
                  child: Text(info.lcdReady ? 'LCD pronta' : 'LCD em breve', style: body(12, weight: 600)),
                ),
              ],
            ),
          ),
        ],
      ),
    );

    if (!selected) return card;
    return Container(
      padding: const EdgeInsets.all(3),
      decoration: BoxDecoration(
        borderRadius: BorderRadius.circular(24),
        border: Border.all(color: Px.blue, width: 4),
      ),
      child: card,
    );
  }
}
