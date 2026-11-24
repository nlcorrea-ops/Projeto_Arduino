import 'dart:ui' show FontVariation;

import 'package:flutter/material.dart';

/// Cores do app.
class Px {
  static const ink = Color(0xFF23202B);
  static const cream = Color(0xFFFFF6E3);
  static const paper = Color(0xFFFFFDF6);
  static const muted = Color(0xFF4A4652);
  static const line = Color(0xFFE8E2D2);
  static const blue = Color(0xFF1F5FC2);
  static const yellow = Color(0xFFF7C948);
  static const study = Color(0xFF9CCBFF);
  static const teleport = Color(0xFFE3BEFF);
  static const pomodoro = Color(0xFFFFC7B8);
  static const stopwatch = Color(0xFFBDEBC6);
  static const danger = Color(0xFFFF9E8F);
  static const green = Color(0xFF2FBF71);
  static const purple = Color(0xFFB15EF2);
  static const night = Color(0xFF2B2440);
  static const lilac = Color(0xFFE9E2FF);
  static const shopTile = Color(0xFFF3ECDA);
}

/// Fonte pixelada dos títulos e botões.
TextStyle pixel(double size, {Color color = Px.ink, bool bold = true}) => TextStyle(
      fontFamily: 'Silkscreen',
      fontSize: size,
      fontWeight: bold ? FontWeight.w700 : FontWeight.w400,
      color: color,
      height: 1.15,
    );

/// Fonte dos textos corridos.
TextStyle body(double size, {Color color = Px.ink, double weight = 400}) => TextStyle(
      fontFamily: 'PixelifySans',
      fontSize: size,
      color: color,
      fontVariations: [FontVariation('wght', weight)],
      height: 1.3,
    );

ThemeData buildTheme() => ThemeData(
      useMaterial3: true,
      scaffoldBackgroundColor: Px.cream,
      fontFamily: 'PixelifySans',
      colorScheme: ColorScheme.fromSeed(seedColor: Px.blue, surface: Px.cream),
      snackBarTheme: SnackBarThemeData(
        backgroundColor: Px.ink,
        contentTextStyle: body(15, color: Colors.white, weight: 500),
        behavior: SnackBarBehavior.floating,
      ),
    );

/// A "caixinha" com borda grossa e sombra dura, usada no app todo.
BoxDecoration chunky({Color color = Px.paper, double radius = 16, bool shadow = true}) => BoxDecoration(
      color: color,
      borderRadius: BorderRadius.circular(radius),
      border: Border.all(color: Px.ink, width: 3),
      boxShadow: shadow ? const [BoxShadow(color: Px.ink, offset: Offset(0, 5))] : null,
    );
