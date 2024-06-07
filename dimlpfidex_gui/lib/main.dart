import 'dart:io';
import 'views/app_view.dart';
import 'package:flutter/material.dart';
import 'package:flutter/foundation.dart';
import 'package:window_size/window_size.dart';

Future<void> main() async {
  WidgetsFlutterBinding.ensureInitialized();

  if (!kIsWeb) {
    if (Platform.isWindows || Platform.isLinux || Platform.isMacOS) {
      const Size minScreenSize = Size(1100, 800);
      setWindowMinSize(minScreenSize);
    }
  }

  runApp(const App());
}