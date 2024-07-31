import 'package:flutter/material.dart';

class AlertColor {
  static const failure = Color.fromARGB(255, 244, 67, 54);
  static const success = Color.fromARGB(255, 56, 142, 60);
  static const warning = Color.fromARGB(255, 204, 123, 2);
  static const info = Color.fromARGB(255, 90, 90, 90);
  static const disabled = Color.fromARGB(255, 158, 158, 158);
}

void showSnackBar(BuildContext context, String message,
    {Color color = AlertColor.info,
    Duration duration = const Duration(seconds: 4)}) {
  ScaffoldMessenger.of(context).showSnackBar(SnackBar(
    content: Text(message),
    backgroundColor: color,
    duration: duration,
  ));
}