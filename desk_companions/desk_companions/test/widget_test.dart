import 'package:desk_companions/app_state.dart';
import 'package:desk_companions/main.dart';
import 'package:desk_companions/services/robot_link.dart';
import 'package:flutter_test/flutter_test.dart';

void main() {
  testWidgets('Abre na tela de escolha do mascote', (tester) async {
    await tester.pumpWidget(DeskCompanionsApp(state: AppState(link: SimulatedRobotLink())));
    expect(find.text('Quem vai para a mesa?'), findsOneWidget);
  });
}
