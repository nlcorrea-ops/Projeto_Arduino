import 'package:flutter/material.dart';

import '../app_state.dart';
import '../models/pet.dart';
import '../pixel/scenes.dart';
import '../pixel/sprite.dart';
import '../pixel/sprites.dart';
import '../theme.dart';
import '../widgets/common.dart';

/// Animação do portal enquanto o mascote viaja entre o app e o robô.
/// A tela só fecha quando o robô confirma a viagem.
class PortalScreen extends StatefulWidget {
  const PortalScreen({super.key});

  @override
  State<PortalScreen> createState() => _PortalScreenState();
}

class _PortalScreenState extends State<PortalScreen> with TickerProviderStateMixin {
  late final AnimationController _fx = AnimationController(
    vsync: this,
    duration: const Duration(milliseconds: 1600),
  )..repeat();

  late final AnimationController _travel = AnimationController(
    vsync: this,
    duration: const Duration(milliseconds: 2400),
  );

  bool _started = false;
  late bool _toRobot;
  late PetInfo _info;
  late String _name;

  bool _done = false;
  bool _ok = false;

  @override
  void didChangeDependencies() {
    super.didChangeDependencies();
    if (_started) return;
    _started = true;
    final state = AppScope.read(context);
    _toRobot = state.place == PetPlace.app;
    _info = state.pet;
    _name = state.petName;
    WidgetsBinding.instance.addPostFrameCallback((_) => _run());
  }

  Future<void> _run() async {
    final state = AppScope.read(context);
    _travel.forward();
    final ok = await state.teleport();
    if (!mounted) return;
    setState(() {
      _done = true;
      _ok = ok;
    });
    if (ok) {
      await Future<void>.delayed(const Duration(milliseconds: 900));
      if (mounted) Navigator.pop(context);
    }
  }

  @override
  void dispose() {
    _fx.dispose();
    _travel.dispose();
    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    final top = MediaQuery.paddingOf(context).top;
    return PopScope(
      canPop: _done,
      child: Scaffold(
        body: AnimatedBuilder(
          animation: Listenable.merge([_fx, _travel]),
          builder: (context, _) {
            final t = Curves.easeInOut.transform(_travel.value);
            final double petX;
            final double opacity;
            if (_toRobot) {
              petX = _info.homeX + (74 - _info.homeX) * t;
              opacity = t < 0.75 ? 1.0 : 1.0 - (t - 0.75) / 0.25;
            } else {
              petX = 74 + (_info.homeX - 74) * t;
              opacity = t < 0.25 ? t / 0.25 : 1.0;
            }
            return SingleChildScrollView(
              child: Column(
                crossAxisAlignment: CrossAxisAlignment.stretch,
                children: [
                  SizedBox(
                    height: 340 + top,
                    child: Stack(
                      children: [
                        Positioned.fill(
                          child: SceneView(
                            label: _toRobot ? '$_name entrando no portal' : '$_name saindo do portal',
                            painter: ScenePainter(
                              scene: _info.scene,
                              pet: _info.kind,
                              petX: petX,
                              petY: _info.homeY,
                              petOpacity: opacity.clamp(0, 1).toDouble(),
                              dim: 0.6,
                              portal: PortalFx(pulse: _fx.value, sparkle: _fx.value),
                              itens: AppScope.read(context).itemsOf(_info.kind),
                            ),
                          ),
                        ),
                        Positioned(
                          top: top + 16,
                          left: 0,
                          right: 0,
                          child: Center(child: OutlinedTitle(_name)),
                        ),
                      ],
                    ),
                  ),
                  Transform.translate(
                    offset: const Offset(0, -30),
                    child: Padding(
                      padding: const EdgeInsets.symmetric(horizontal: 20),
                      child: _statusCard(),
                    ),
                  ),
                ],
              ),
            );
          },
        ),
      ),
    );
  }

  Widget _statusCard() {
    final String headline;
    final String detail;
    if (!_done) {
      headline = 'Indo para outra dimensão…';
      detail = _toRobot ? 'Esperando o robô confirmar a chegada' : 'Esperando o robô liberar o $_name';
    } else if (_ok) {
      headline = _toRobot ? 'Chegou no robô!' : 'De volta ao app!';
      detail = _toRobot ? 'Agora o $_name está na tela do robô.' : 'O $_name voltou para casa.';
    } else {
      headline = 'O portal fechou';
      final onde = _toRobot ? 'no app' : 'no robô';
      detail = AppScope.read(context).link.isConnected
          ? 'O robô não aceitou agora (talvez outro bichinho já esteja lá). O $_name continua $onde.'
          : 'O robô não está conectado. O $_name continua $onde.';
    }
    final lit = _done ? 10 : (_fx.value * 10).floor() + 1;

    return Container(
      padding: const EdgeInsets.fromLTRB(18, 22, 18, 22),
      decoration: chunky(color: Px.night, radius: 18),
      child: Column(
        children: [
          const SpriteView(Sprites.portal, pixel: 5),
          const SizedBox(height: 14),
          Text(headline, style: pixel(19, color: Colors.white), textAlign: TextAlign.center),
          const SizedBox(height: 14),
          Semantics(
            label: _done ? 'Viagem concluída' : 'Viagem em andamento',
            excludeSemantics: true,
            child: Row(
              children: [
                for (var i = 0; i < 10; i++)
                  Expanded(
                    child: Container(
                      height: 16,
                      margin: EdgeInsets.only(right: i < 9 ? 4 : 0),
                      decoration: BoxDecoration(
                        color: i < lit ? Px.purple : Px.line,
                        border: Border.all(color: Px.ink, width: 2),
                      ),
                    ),
                  ),
              ],
            ),
          ),
          const SizedBox(height: 14),
          Text(detail, style: body(14, color: Px.lilac), textAlign: TextAlign.center),
          if (_done && !_ok) ...[
            const SizedBox(height: 16),
            ChunkyButton(
              color: Px.yellow,
              onTap: () => Navigator.pop(context),
              child: SizedBox(height: 28, child: Center(child: Text('Voltar', style: pixel(16)))),
            ),
          ],
        ],
      ),
    );
  }
}

