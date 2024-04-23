import 'dart:io';
import 'package:window_size/window_size.dart';

import 'views/app_view.dart';
import 'package:flutter/material.dart';

void main() {
  WidgetsFlutterBinding.ensureInitialized();

  if (Platform.isWindows || Platform.isLinux || Platform.isMacOS) {
    const Size minScreenSize = Size(1100, 800);
    setWindowMinSize(minScreenSize);
  }

  runApp(const App());
}