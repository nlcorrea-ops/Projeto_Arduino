import 'package:flutter/material.dart';

import '../models/pet.dart';
import '../models/shop.dart';
import 'sprite.dart';
import 'sprites.dart';

/// Onde cada roupa ou acessório fica em cada bichinho, em pixels do desenho do bichinho
/// (coluna, linha a partir do canto de cima à esquerda; valores negativos ficam acima dele).
const Map<PetKind, Map<String, (int, int)>> _ancoras = {
  PetKind.tubarao: {'chapeu': (14, -4), 'lacinho': (7, -3), 'cachecol': (13, 3), 'camiseta': (5, 4)},
  PetKind.sapo: {'chapeu': (6, -4), 'lacinho': (12, -3), 'cachecol': (6, 5), 'camiseta': (1, 8)},
  PetKind.recruta: {'chapeu': (3, -5), 'lacinho': (10, 0), 'cachecol': (4, 11), 'camiseta': (4, 11)},
};

/// Desenho do item para aquele bichinho (alguns mudam de formato para caber no corpo).
PixelSprite formaDoItem(PetKind pet, ShopItem item) {
  if (pet == PetKind.tubarao && item.id == 'cachecol') return Sprites.scarfVertical;
  if (pet == PetKind.tubarao && item.id == 'camiseta') return Sprites.shirtShark;
  if (pet == PetKind.sapo && item.id == 'camiseta') return Sprites.shirtFrog;
  return item.icon;
}

/// Pinta roupas e acessórios por cima do bichinho desenhado em (x, y) com pixels de tamanho 2.
void pintaRoupas(Canvas canvas, PetKind pet, double x, double y, List<ShopItem> itens) {
  // primeiro a roupa, depois o acessório (que fica por cima)
  for (final categoria in const [ShopCategory.roupas, ShopCategory.acessorios]) {
    for (final item in itens) {
      if (item.category != categoria) continue;
      final ancora = _ancoras[pet]?[item.id];
      if (ancora == null) continue;
      final (coluna, linha) = ancora;
      paintSprite(canvas, formaDoItem(pet, item), x + coluna * 2, y + linha * 2, 2);
    }
  }
}

/// Linha do chão de cada cenário (onde as decorações ficam apoiadas).
const Map<SceneKind, double> _chao = {
  SceneKind.fundoDoMar: 76,
  SceneKind.pantano: 56,
  SceneKind.quartel: 64,
};

/// Pinta as decorações no cenário: a plantinha à direita e a luminária à esquerda.
void pintaDecoracao(Canvas canvas, SceneKind cena, List<ShopItem> itens) {
  final chao = _chao[cena];
  if (chao == null) return;                    // na biblioteca não tem decoração
  for (final item in itens) {
    if (item.category != ShopCategory.decoracao) continue;
    final x = item.id == 'plantinha' ? 76.0 : 8.0;
    paintSprite(canvas, item.icon, x, chao - item.icon.height * 2, 2);
  }
}
