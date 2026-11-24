import '../pixel/sprite.dart';
import '../pixel/sprites.dart';

enum ShopCategory {
  comida('Comida'),
  roupas('Roupas'),
  acessorios('Acessórios'),
  decoracao('Decoração');

  const ShopCategory(this.label);
  final String label;
}

class ShopItem {
  const ShopItem({
    required this.id,
    required this.name,
    required this.price,
    required this.category,
    required this.icon,
  });

  final String id;
  final String name;
  final int price;
  final ShopCategory category;
  final PixelSprite icon;
}

/// Catálogo da loja. Os preços são exemplos: ajuste à vontade.
const List<ShopItem> shopCatalog = [
  ShopItem(id: 'peixinho', name: 'Peixinho', price: 10, category: ShopCategory.comida, icon: Sprites.fish),
  ShopItem(id: 'camarao', name: 'Camarão', price: 15, category: ShopCategory.comida, icon: Sprites.shrimp),
  ShopItem(id: 'mosca', name: 'Mosca', price: 8, category: ShopCategory.comida, icon: Sprites.fly),
  ShopItem(id: 'bolinho', name: 'Bolinho', price: 12, category: ShopCategory.comida, icon: Sprites.cake),
  ShopItem(id: 'alga', name: 'Alga', price: 6, category: ShopCategory.comida, icon: Sprites.seaweed),
  ShopItem(id: 'sorvete', name: 'Sorvete', price: 30, category: ShopCategory.comida, icon: Sprites.iceCream),
  ShopItem(id: 'camiseta', name: 'Camiseta', price: 20, category: ShopCategory.roupas, icon: Sprites.shirt),
  ShopItem(id: 'cachecol', name: 'Cachecol', price: 18, category: ShopCategory.roupas, icon: Sprites.scarf),
  ShopItem(id: 'lacinho', name: 'Lacinho', price: 12, category: ShopCategory.acessorios, icon: Sprites.bow),
  ShopItem(id: 'chapeu', name: 'Chapéu', price: 22, category: ShopCategory.acessorios, icon: Sprites.hat),
  ShopItem(id: 'plantinha', name: 'Plantinha', price: 14, category: ShopCategory.decoracao, icon: Sprites.plant),
  ShopItem(id: 'luminaria', name: 'Luminária', price: 25, category: ShopCategory.decoracao, icon: Sprites.lamp),
];
