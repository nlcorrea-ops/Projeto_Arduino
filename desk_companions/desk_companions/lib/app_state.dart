import 'dart:async';

import 'package:flutter/widgets.dart';

import 'models/pet.dart';
import 'models/shop.dart';
import 'services/robot_link.dart';

/// Guarda tudo o que as telas mostram e avisa quando algo muda.
///
/// O robô é a referência para saber onde cada bichinho está, a saúde, a felicidade e o
/// estudo: o app se ajusta pelo estado que ele manda a cada 1 s.
/// Moedas e compras ficam só na memória por enquanto (salvar no aparelho é a próxima etapa).
class AppState extends ChangeNotifier {
  AppState({required this.link}) {
    _subscription = link.events.listen(_onRobotEvent);
  }

  final RobotLink link;
  late final StreamSubscription<RobotEvent> _subscription;

  PetKind selected = PetKind.tubarao;
  final Map<PetKind, PetPlace> _places = {for (final pet in PetKind.values) pet: PetPlace.app};

  int coins = 25;
  final Map<String, int> owned = {};

  /// O que cada bichinho está vestindo: um item por tipo (roupa, acessório, decoração).
  /// As compras são de todos; o que cada um veste é só dele.
  final Map<PetKind, Map<ShopCategory, String>> _vestindo = {for (final pet in PetKind.values) pet: {}};

  int health = 4;
  int happiness = 3;

  StudyMode? studyMode;
  bool studyPaused = false;

  /// true quando o Pomodoro está na pausa automática (informado pelo robô).
  bool pomodoroPausa = false;

  /// Comidas esperando na lancheira do robô.
  int robotLanches = 0;

  bool _teleportando = false;

  PetInfo get pet => PetInfo.of(selected);
  String get petName => pet.defaultName;
  PetPlace get place => _places[selected]!;

  /// Se algum mascote está no robô agora.
  bool get anyPetOnRobot => _places.values.any((p) => p != PetPlace.app);

  void selectPet(PetKind kind) {
    selected = kind;
    notifyListeners();
  }

  /// Leva o mascote para o robô ou traz de volta, conforme onde ele está.
  Future<bool> teleport() async {
    final pet = selected;
    final goingToRobot = _places[pet] == PetPlace.app;
    _places[pet] = PetPlace.viajando;
    _teleportando = true;
    notifyListeners();

    bool ok;
    try {
      ok = goingToRobot ? await link.sendPetToRobot(pet) : await link.bringPetBack(pet);
    } catch (_) {
      ok = false;
    }

    // Sem confirmação do robô, o mascote fica onde estava.
    if (goingToRobot) {
      _places[pet] = ok ? PetPlace.robo : PetPlace.app;
    } else {
      _places[pet] = ok ? PetPlace.app : PetPlace.robo;
    }
    _teleportando = false;
    notifyListeners();
    return ok;
  }

  int ownedCount(ShopItem item) => owned[item.id] ?? 0;

  bool canAfford(ShopItem item) => coins >= item.price;

  /// Roupas, acessórios e decorações (a comida não se veste: ela é gasta).
  bool isWearable(ShopItem item) => item.category != ShopCategory.comida;

  bool isOwned(ShopItem item) => ownedCount(item) > 0;

  bool buy(ShopItem item) {
    if (!canAfford(item)) return false;
    if (isWearable(item) && isOwned(item)) return false;   // roupa se compra uma vez só
    coins -= item.price;
    owned[item.id] = ownedCount(item) + 1;
    if (item.category == ShopCategory.comida) {
      link.sendFood(1);                                    // vai para a lancheira do robô
    } else {
      _vestindo[selected]![item.category] = item.id;       // já veste quem está selecionado
    }
    notifyListeners();
    return true;
  }

  // ---- Guarda-roupa ----

  ShopItem? _itemPorId(String? id) {
    if (id == null) return null;
    for (final item in shopCatalog) {
      if (item.id == id) return item;
    }
    return null;
  }

  /// O que o bichinho [pet] está usando no tipo [categoria] (ou null).
  ShopItem? wearing(PetKind pet, ShopCategory categoria) => _itemPorId(_vestindo[pet]![categoria]);

  /// Tudo o que o bichinho [pet] está usando.
  List<ShopItem> itemsOf(PetKind pet) => [
        for (final id in _vestindo[pet]!.values)
          if (_itemPorId(id) case final item?) item,
      ];

  bool isWearing(ShopItem item) => _vestindo[selected]![item.category] == item.id;

  /// Veste o item no bichinho selecionado (troca o que estava no mesmo tipo).
  void wear(ShopItem item) {
    if (!isWearable(item) || !isOwned(item)) return;
    _vestindo[selected]![item.category] = item.id;
    notifyListeners();
  }

  /// Tira o item do tipo [categoria] do bichinho selecionado.
  void takeOff(ShopCategory categoria) {
    _vestindo[selected]!.remove(categoria);
    notifyListeners();
  }

  Future<void> startStudy(StudyMode mode) async {
    studyMode = mode;
    studyPaused = false;
    notifyListeners();
    await link.startStudy(mode);
  }

  Future<void> stopStudy() async {
    studyMode = null;
    studyPaused = false;
    notifyListeners();
    await link.stopStudy();
  }

  void _onRobotEvent(RobotEvent event) {
    switch (event) {
      case StatsEvent(:final health, :final happiness):
        this.health = health;
        this.happiness = happiness;
      case StudyPauseEvent(:final paused):
        studyPaused = paused;
      case RobotStatusEvent(:final status):
        _aplicaEstadoDoRobo(status);
      case ConnectionEvent():
        break;
    }
    notifyListeners();
  }

  /// Ajusta o app ao que o robô informou.
  void _aplicaEstadoDoRobo(RobotStatus status) {
    robotLanches = status.lanches;
    pomodoroPausa = status.fase == 'P';

    // Onde está cada bichinho (durante o portal, quem manda é a animação do app).
    if (!_teleportando) {
      for (final kind in PetKind.values) {
        if (kind == status.pet) {
          _places[kind] = switch (status.onde) {
            'R' => PetPlace.robo,
            'V' => PetPlace.viajando,
            _ => PetPlace.app,
          };
        } else if (_places[kind] != PetPlace.app) {
          _places[kind] = PetPlace.app;            // o robô só guarda um bichinho por vez
        }
      }
    }

    // Se o robô está estudando e o app não sabe (ex.: o app foi reaberto), o app acompanha.
    if (studyMode == null && status.estudo != 'N') {
      studyMode = status.estudo == 'C' ? StudyMode.cronometro : StudyMode.pomodoro;
    }
  }

  @override
  void dispose() {
    _subscription.cancel();
    link.dispose();
    super.dispose();
  }
}

/// Deixa o [AppState] disponível para todas as telas.
class AppScope extends InheritedNotifier<AppState> {
  const AppScope({super.key, required AppState state, required super.child})
      : super(notifier: state);

  /// Use dentro de build(): a tela se redesenha quando o estado muda.
  static AppState of(BuildContext context) =>
      context.dependOnInheritedWidgetOfExactType<AppScope>()!.notifier!;

  /// Use em botões e ações: só lê, sem redesenhar.
  static AppState read(BuildContext context) =>
      context.getInheritedWidgetOfExactType<AppScope>()!.notifier!;
}
