import 'package:flutter/material.dart';

import '../app_state.dart';
import '../models/shop.dart';
import '../pixel/sprite.dart';
import '../pixel/sprites.dart';
import '../theme.dart';
import '../widgets/common.dart';

/// Aba "Loja": comprar comida, roupas, acessórios e decoração.
class LojaTab extends StatefulWidget {
  const LojaTab({super.key});

  @override
  State<LojaTab> createState() => _LojaTabState();
}

class _LojaTabState extends State<LojaTab> {
  ShopCategory _category = ShopCategory.comida;

  @override
  Widget build(BuildContext context) {
    final state = AppScope.of(context);
    final items = shopCatalog.where((item) => item.category == _category).toList();
    final note = _category == ShopCategory.comida
        ? 'A comida vai para a lancheira do robô, onde você alimenta o ${state.petName}. '
            'Na lancheira agora: ${state.robotLanches}.'
        : 'Aparece só aqui no app. Comprou uma vez, pode vestir em qualquer bichinho.';

    return SafeArea(
      bottom: false,
      child: ListView(
        padding: const EdgeInsets.fromLTRB(20, 20, 20, 24),
        children: [
          Row(
            mainAxisAlignment: MainAxisAlignment.spaceBetween,
            children: [
              Text('Loja', style: pixel(28)),
              CoinChip(state.coins),
            ],
          ),
          const SizedBox(height: 16),
          SingleChildScrollView(
            scrollDirection: Axis.horizontal,
            child: Row(
              children: [
                for (final category in ShopCategory.values)
                  Padding(
                    padding: const EdgeInsets.only(right: 8),
                    child: _CategoryChip(
                      label: category.label,
                      selected: category == _category,
                      onTap: () => setState(() => _category = category),
                    ),
                  ),
              ],
            ),
          ),
          const SizedBox(height: 12),
          Text(note, style: body(13, color: Px.muted)),
          const SizedBox(height: 12),
          GridView.builder(
            shrinkWrap: true,
            physics: const NeverScrollableScrollPhysics(),
            itemCount: items.length,
            gridDelegate: const SliverGridDelegateWithFixedCrossAxisCount(
              crossAxisCount: 2,
              mainAxisSpacing: 12,
              crossAxisSpacing: 12,
              mainAxisExtent: 200,
            ),
            itemBuilder: (context, i) => _ItemCard(item: items[i]),
          ),
        ],
      ),
    );
  }
}

class _CategoryChip extends StatelessWidget {
  const _CategoryChip({required this.label, required this.selected, required this.onTap});

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
        shape: RoundedRectangleBorder(
          borderRadius: BorderRadius.circular(999),
          side: const BorderSide(color: Px.ink, width: 3),
        ),
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

class _ItemCard extends StatelessWidget {
  const _ItemCard({required this.item});

  final ShopItem item;

  @override
  Widget build(BuildContext context) {
    final state = AppScope.of(context);
    final count = state.ownedCount(item);
    final canBuy = state.canAfford(item);
    final vestivel = state.isWearable(item);
    final comprado = vestivel && state.isOwned(item);
    final usando = comprado && state.isWearing(item);

    return Container(
      padding: const EdgeInsets.all(12),
      decoration: BoxDecoration(
        color: Px.paper,
        borderRadius: BorderRadius.circular(14),
        border: Border.all(color: Px.ink, width: 3),
      ),
      child: Column(
        crossAxisAlignment: CrossAxisAlignment.stretch,
        children: [
          Container(
            height: 52,
            decoration: BoxDecoration(color: Px.shopTile, borderRadius: BorderRadius.circular(8)),
            child: Center(child: SpriteView(item.icon, pixel: 4)),
          ),
          const SizedBox(height: 8),
          Row(
            children: [
              Expanded(child: Text(item.name, style: body(15, weight: 600), overflow: TextOverflow.ellipsis)),
              if (!comprado) ...[
                const SpriteView(Sprites.coin, pixel: 2),
                const SizedBox(width: 4),
                Text('${item.price}', style: pixel(13, bold: false)),
              ],
            ],
          ),
          const SizedBox(height: 4),
          SizedBox(
            height: 18,
            child: Text(
              comprado ? 'Comprado' : (count > 0 ? 'Você tem $count' : ''),
              style: body(12, color: Px.muted),
            ),
          ),
          const Spacer(),
          if (comprado)
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
                height: 36,
                child: Center(child: Text(usando ? 'Usando' : 'Usar', style: pixel(13))),
              ),
            )
          else
          ChunkyButton(
            color: Px.yellow,
            radius: 10,
            padding: EdgeInsets.zero,
            semanticLabel: canBuy ? 'Comprar ${item.name}' : '${item.name}: faltam ${item.price - state.coins} moedas',
            onTap: canBuy ? () => _buy(context) : null,
            child: SizedBox(
              height: 36,
              child: Center(
                child: Text(
                  canBuy ? 'Comprar' : 'Faltam ${item.price - state.coins}',
                  style: pixel(13, color: canBuy ? Px.ink : Px.muted, bold: canBuy),
                ),
              ),
            ),
          ),
          const SizedBox(height: 5),
        ],
      ),
    );
  }

  void _buy(BuildContext context) {
    final state = AppScope.read(context);
    if (!state.buy(item)) return;
    ScaffoldMessenger.of(context)
      ..hideCurrentSnackBar()
      ..showSnackBar(SnackBar(
        content: Text(state.isWearable(item)
            ? '${state.petName} já está usando: ${item.name}!'
            : 'Você comprou: ${item.name}!'),
      ));
  }
}

