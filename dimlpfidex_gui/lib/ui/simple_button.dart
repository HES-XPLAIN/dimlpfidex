import 'package:flutter/material.dart';

enum ButtonType { action, confirm, cancel }

class SimpleButton extends StatelessWidget {
  final String label;
  final Function()? onPressed;
  final EdgeInsets padding;
  final bool isEnabled;
  final ButtonType buttonType;

  const SimpleButton(
      {super.key,
      required this.label,
      required this.onPressed,
      this.isEnabled = true,
      this.buttonType = ButtonType.confirm,
      this.padding =
          const EdgeInsets.symmetric(vertical: 20.0, horizontal: 30.0)});

  @override
  Widget build(BuildContext context) {
    List<Color> colors = [
      Theme.of(context).colorScheme.primary,
      Theme.of(context).colorScheme.primary,
      Theme.of(context).colorScheme.error,
    ];

    return ElevatedButton(
      onPressed: isEnabled ? onPressed : null,
      style: ElevatedButton.styleFrom(
          backgroundColor:
              isEnabled ? colors[buttonType.index] : Colors.grey[400],
          padding: padding),
      child: Text(label),
    );
  }
}