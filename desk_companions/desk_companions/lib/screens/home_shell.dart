import 'package:flutter/material.dart';

import '../widgets/common.dart';
import 'loja_tab.dart';
import 'pet_tab.dart';

/// Tela principal com as abas Pet e Loja.
class HomeShell extends StatefulWidget {
  const HomeShell({super.key});

  @override
  State<HomeShell> createState() => _HomeShellState();
}

class _HomeShellState extends State<HomeShell> {
  int _tab = 0;

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      body: IndexedStack(
        index: _tab,
        children: const [PetTab(), LojaTab()],
      ),
      bottomNavigationBar: PixelNavBar(
        index: _tab,
        onChanged: (i) => setState(() => _tab = i),
      ),
    );
  }
}
