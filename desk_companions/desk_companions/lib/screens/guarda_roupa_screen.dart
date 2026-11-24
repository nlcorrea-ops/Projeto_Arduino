import 'package:flutter/material.dart';

import '../app_state.dart';
import '../models/shop.dart';
import '../pixel/scenes.dart';
import '../pixel/sprite.dart';
import '../theme.dart';
import '../widgets/common.dart';

/// Guarda-roupa: escolher o que o bichinho selecionado veste.
/// Um item de cada tipo por vez; aparece só aqui no app (não na LCD do robô).
class GuardaRoupaScreen extends StatefulWidget {
  const GuardaRoupaScreen({super.key});

  @override
  State<GuardaRoupaScreen> createState() => _GuardaRoupaScreenState();
}

class _GuardaRoupaScreenState extends State<GuardaRoupaScreen> {
  ShopCategory _categoria = ShopCategory.roupas;

  static const _tipos = [ShopCategory.roupas, ShopCategory.acessorios, ShopCategory.decoracao];

  String _nomeDoTipo(ShopCategory c) => switch (c) {
        ShopCategory.roupas => 'Roupa',
        ShopCategory.acessorios => 'Acessório',
        _ => 'Decoração',
      };

  @override
  Widget build(BuildContext context) {
    final state = AppScope.of(context);
    final info = state.pet;
    final top = MediaQuery.paddingOf(context).top;
    final comprados = shopCatalog.where((i) => i.category == _categoria && state.isOwned(i)).toList();

    return Scaffold(
      body: Column(
        crossAxisAlignment: CrossAxisAlignment.stretch,
        children: [
          SizedBox(
            height: 240 + top,
            child: Stack(
              children: [
                Positioned.fill(
                  child: SceneView(
                    label: '${state.petName} com o que está vestindo',
                    painter: ScenePainter(
                      scene: info.scene,
                      pet: info.kind,
                      petX: info.homeX,
                      petY: info.homeY,
                      itens: state.itemsOf(info.kind),
                    ),
                  ),
                ),
                Positioned(
                  top: top + 12,
                  left: 16,
                  child: BackSquareButton(onTap: () => Navigator.pop(context)),
                ),
                Positioned(
                  top: top + 22,
                  left: 0,
                  right: 0,
                  child: const IgnorePointer(child: Center(child: OutlinedTitle('Guarda-roupa', size: 22))),
                ),
              ],
            ),
          ),
          Expanded(
            child: ListView(
              padding: const EdgeInsets.fromLTRB(20, 16, 20, 20),
              children: [
                Row(
                  children: [
                    for (final tipo in _tipos) ...[
                      Expanded(child: _SlotChip(tipo: _nomeDoTipo(tipo), item: state.wearing(info.kind, tipo))),
                      if (tipo != _tipos.last) const SizedBox(width: 8),
                    ],
                  ],
                ),
                const SizedBox(height: 16),
                SingleChildScrollView(
                  scrollDirection: Axis.horizontal,
                  child: Row(
                    children: [
                      for (final tipo in _tipos)
                        Padding(
                          padding: const EdgeInsets.only(right: 8),
                          child: _Aba(
                            label: tipo.label,
                            selected: tipo == _categoria,
                            onTap: () => setState(() => _categoria = tipo),
                          ),
                        ),
                    ],
                  ),
                ),
                const SizedBox(height: 12),
                Text('Um item de cada tipo por vez. Aparece só aqui no app.', style: body(13, color: Px.muted)),
                const SizedBox(height: 12),
                if (comprados.isEmpty)
                  Padding(
                    padding: const EdgeInsets.symmetric(vertical: 24),
                    child: Text(
                      'Você ainda não tem nada aqui. Compre na Loja!',
                      textAlign: TextAlign.center,
                      style: body(15, color: Px.muted),
                    ),
                  )
                else
                  GridView.count(
                    crossAxisCount: 3,
                    shrinkWrap: true,
                    physics: const NeverScrollableScrollPhysics(),
                    mainAxisSpacing: 10,
                    crossAxisSpacing: 10,
                    childAspectRatio: 0.62,
                    children: [
                      for (final item in comprados) _RoupaCard(item: item),
                      _NenhumCard(categoria: _categoria),
                    ],
                  ),
              ],
            ),
          ),
          Padding(
            padding: EdgeInsets.fromLTRB(20, 0, 20, 16 + MediaQuery.paddingOf(context).bottom),
            child: ChunkyButton(
              color: Px.yellow,
              onTap: () => Navigator.pop(context),
              child: SizedBox(height: 30, child: Center(child: Text('Pronto', style: pixel(17)))),
            ),
          ),
        ],
      ),
    );
  }
}

class _SlotChip extends StatelessWidget {
  const _SlotChip({required this.tipo, required this.item});

  final String tipo;
  final ShopItem? item;

  @override
  Widget build(BuildContext context) {
    return Container(
      padding: const EdgeInsets.symmetric(horizontal: 10, vertical: 8),
      decoration: BoxDecoration(
        color: item == null ? Px.shopTile : Px.paper,
        borderRadius: BorderRadius.circular(12),
        border: Border.all(color: Px.ink, width: 3),
      ),
      child: Column(
        crossAxisAlignment: CrossAxisAlignment.start,
        children: [
          Text(tipo, style: body(11, color: Px.muted)),
          const SizedBox(height: 2),
          Text(item?.name ?? 'Nada', style: pixel(12, bold: false), maxLines: 1, overflow: TextOverflow.ellipsis),
        ],
      ),
    );
  }
}

class _Aba extends StatelessWidget {
  const _Aba({required this.label, required this.selected, required this.onTap});

  final String label;
  final bool selected;
  final VoidCallback onTap;

  @override
  Widget build(BuildContext context) {
    return Semantics(
      button: true,
      selected: selected,
      child: Material(
        color: selected ? Px.ink : Px.paper,
        shape: const StadiumBorder(side: BorderSide(color: Px.ink, width: 3)),
        child: InkWell(
          onTap: onTap,
          customBorder: const StadiumBorder(),
          child: Container(
            height: 44,
            padding: const EdgeInsets.symmetric(horizontal: 14),
            alignment: Alignment.center,
            child: Text(label, style: pixel(13, color: selected ? Colors.white : Px.ink, bold: false)),
          ),
        ),
      ),
    );
  }
}

class _RoupaCard extends StatelessWidget {
  const _RoupaCard({required this.item});

  final ShopItem item;

  @override
  Widget build(BuildContext context) {
    final state = AppScope.of(context);
    final usando = state.isWearing(item);
    return Container(
      padding: const EdgeInsets.all(8),
      decoration: BoxDecoration(
        color: Px.paper,
        borderRadius: BorderRadius.circular(14),
        border: Border.all(color: usando ? Px.green : Px.ink, width: usando ? 4 : 3),
      ),
      child: Column(
        crossAxisAlignment: CrossAxisAlignment.stretch,
        children: [
          Container(
            height: 52,
            decoration: BoxDecoration(color: Px.shopTile, borderRadius: BorderRadius.circular(8)),
            child: Center(child: SpriteView(item.icon, pixel: 4)),
          ),
          const SizedBox(height: 6),
          Text(item.name, textAlign: TextAlign.center, style: body(14, weight: 600), maxLines: 1, overflow: TextOverflow.ellipsis),
          const Spacer(),
          ChunkyButton(
            color: usando ? Px.green : Px.paper,
            radius: 10,
            padding: EdgeInsets.zero,
            semanticLabel: usando ? '${item.name}: usando. Toque para tirar' : 'Usar ${item.name}',
            onTap: () {
              final s = AppScope.read(context);
              usando ? s.takeOff(item.category) : s.wear(item);
            },
            child: SizedBox(
              height: 34,
              child: Center(
                child: FittedBox(
                  fit: BoxFit.scaleDown,
                  child: Text(usando ? 'Usando' : 'Usar', style: pixel(12)),
                ),
              ),
            ),
          ),
          const SizedBox(height: 5),
        ],
      ),
    );
  }
}

class _NenhumCard extends StatelessWidget {
  const _NenhumCard({required this.categoria});

  final ShopCategory categoria;

  @override
  Widget build(BuildContext context) {
    final state = AppScope.of(context);
    final vazio = state.wearing(state.selected, categoria) == null;
    return Container(
      padding: const EdgeInsets.all(8),
      decoration: BoxDecoration(
        color: Px.paper,
        borderRadius: BorderRadius.circular(14),
        border: Border.all(color: vazio ? Px.green : const Color(0xFF8A8494), width: vazio ? 4 : 3),
      ),
      child: Column(
        crossAxisAlignment: CrossAxisAlignment.stretch,
        children: [
          SizedBox(height: 52, child: Center(child: Text('—', style: pixel(20, color: const Color(0xFF8A8494))))),
          const SizedBox(height: 6),
          Text('Nenhum', textAlign: TextAlign.center, style: body(14, weight: 600)),
          const Spacer(),
          ChunkyButton(
            color: Px.paper,
            radius: 10,
            padding: EdgeInsets.zero,
            semanticLabel: 'Tirar o que está usando',
            onTap: vazio ? null : () => AppScope.read(context).takeOff(categoria),
            child: SizedBox(height: 34, child: Center(child: Text('Tirar', style: pixel(12)))),
          ),
          const SizedBox(height: 5),
        ],
      ),
    );
  }
}
