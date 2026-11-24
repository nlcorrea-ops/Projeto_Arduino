import 'package:flutter/foundation.dart';
import 'package:flutter/material.dart';

import '../app_state.dart';
import '../pixel/sprite.dart';
import '../pixel/sprites.dart';
import '../services/robot_link.dart';
import '../theme.dart';
import '../widgets/common.dart';
import 'estudar_screen.dart';

/// Sessão de estudo em andamento. O tempo conta só na LCD do robô;
/// aqui o app mostra o estado e é o único lugar que encerra a sessão.
class SessaoScreen extends StatelessWidget {
  const SessaoScreen({super.key});

  @override
  Widget build(BuildContext context) {
    final state = AppScope.of(context);
    final mode = state.studyMode ?? StudyMode.pomodoro;
    final paused = mode == StudyMode.cronometro && state.studyPaused;
    final name = state.petName;

    final String title;
    final String cardTitle;
    final String cardText;
    final PixelSprite cardIcon;
    if (mode == StudyMode.pomodoro) {
      title = 'Foco';
      cardTitle = 'Pomodoro em andamento';
      cardText = 'O tempo está na tela do robô, alternando com o rostinho do $name.';
      cardIcon = Sprites.lcd;
    } else if (paused) {
      title = 'Pausa';
      cardTitle = 'Cronômetro pausado';
      cardText = 'Aperte Enter no robô para continuar. Enquanto isso, o $name faz a animação de sempre.';
      cardIcon = Sprites.stopwatch;
    } else {
      title = 'Estudando';
      cardTitle = 'Cronômetro rodando';
      cardText = 'O tempo está na tela do robô. Aperte Enter na protoboard para pausar.';
      cardIcon = Sprites.stopwatch;
    }

    final link = state.link;

    return PopScope(
      // Não dá para sair pelo gesto de voltar: só pelo botão Encerrar.
      canPop: false,
      child: Scaffold(
        body: SingleChildScrollView(
          child: Column(
            crossAxisAlignment: CrossAxisAlignment.stretch,
            children: [
              LibraryHeader(
                title: title,
                showPause: paused,
                pill: paused
                    ? const StatusPill('Pausado no robô', dotColor: Color(0xFF9A95A3))
                    : const StatusPill('Contando no robô'),
              ),
              Padding(
                padding: const EdgeInsets.fromLTRB(20, 22, 20, 0),
                child: Container(
                  padding: const EdgeInsets.all(16),
                  decoration: chunky(),
                  child: Row(
                    children: [
                      SpriteView(cardIcon, pixel: 5),
                      const SizedBox(width: 14),
                      Expanded(
                        child: Column(
                          crossAxisAlignment: CrossAxisAlignment.start,
                          children: [
                            Text(cardTitle, style: pixel(16)),
                            const SizedBox(height: 4),
                            Text(cardText, style: body(14)),
                          ],
                        ),
                      ),
                    ],
                  ),
                ),
              ),
              if (mode == StudyMode.pomodoro)
                Padding(
                  padding: const EdgeInsets.fromLTRB(20, 20, 20, 0),
                  child: Semantics(
                    label: state.pomodoroPausa ? 'Etapa atual: pausa' : 'Etapa atual: foco',
                    excludeSemantics: true,
                    child: Row(
                      children: [
                        _Step('Foco 25', active: !state.pomodoroPausa),
                        const SizedBox(width: 8),
                        _Step('Pausa', active: state.pomodoroPausa),
                      ],
                    ),
                  ),
                ),
              if (mode == StudyMode.cronometro && kDebugMode && link is SimulatedRobotLink)
                Padding(
                  padding: const EdgeInsets.only(top: 12),
                  child: Center(
                    child: TextButton(
                      onPressed: link.simulateEnter,
                      child: Text('Teste: simular Enter no robô', style: body(14, color: Px.blue, weight: 600)),
                    ),
                  ),
                ),
              Padding(
                padding: const EdgeInsets.fromLTRB(20, 24, 20, 8),
                child: ChunkyButton(
                  color: Px.danger,
                  onTap: () async {
                    await AppScope.read(context).stopStudy();
                    if (context.mounted) Navigator.pop(context);
                  },
                  child: SizedBox(
                    height: 30,
                    child: Center(child: Text('Encerrar sessão', style: pixel(17))),
                  ),
                ),
              ),
              Text(
                'A sessão só pode ser encerrada pelo app.',
                textAlign: TextAlign.center,
                style: body(13, color: Px.muted),
              ),
              const SizedBox(height: 32),
            ],
          ),
        ),
      ),
    );
  }
}

class _Step extends StatelessWidget {
  const _Step(this.label, {this.active = false});

  final String label;
  final bool active;

  @override
  Widget build(BuildContext context) {
    return Expanded(
      child: Column(
        children: [
          Container(
            height: 16,
            decoration: BoxDecoration(
              color: active ? Px.yellow : Px.line,
              borderRadius: BorderRadius.circular(4),
              border: Border.all(color: Px.ink, width: 3),
            ),
          ),
          const SizedBox(height: 6),
          Text(label, style: body(13, weight: active ? 700 : 500)),
        ],
      ),
    );
  }
}
