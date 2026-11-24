import 'package:flutter/material.dart';

import 'app_state.dart';
import 'screens/escolha_screen.dart';
import 'services/ble_robot_link.dart';
import 'services/robot_link.dart';
import 'theme.dart';

/// true = robô de mentirinha (para testar no simulador, que não tem Bluetooth).
/// false = robô de verdade pelo Bluetooth.
const bool usarRoboSimulado = false;

void main() {
  final RobotLink link = usarRoboSimulado ? SimulatedRobotLink() : BleRobotLink();
  final state = AppState(link: link);
  runApp(DeskCompanionsApp(state: state));
}

class DeskCompanionsApp extends StatelessWidget {
  const DeskCompanionsApp({super.key, required this.state});

  final AppState state;

  @override
  Widget build(BuildContext context) {
    return AppScope(
      state: state,
      child: MaterialApp(
        title: 'Desk Companions',
        debugShowCheckedModeBanner: false,
        theme: buildTheme(),
        builder: (context, child) => _PhoneFrame(child: child!),
        home: const EscolhaScreen(),
      ),
    );
  }
}

/// No Mac a janela pode ficar larga; aqui limitamos o app à largura de um celular.
class _PhoneFrame extends StatelessWidget {
  const _PhoneFrame({required this.child});

  final Widget child;

  @override
  Widget build(BuildContext context) {
    return ColoredBox(
      color: const Color(0xFF3A3546),
      child: Center(
        child: ConstrainedBox(
          constraints: const BoxConstraints(maxWidth: 430),
          child: child,
        ),
      ),
    );
  }
}
