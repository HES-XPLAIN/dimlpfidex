import 'dart:io';

import 'package:dimlpfidex_gui/ui/input_field.dart';
import 'package:dimlpfidex_gui/ui/simple_button.dart';
import 'package:file_selector/file_selector.dart';
import 'package:flutter/foundation.dart';
import 'package:flutter/material.dart';
import 'package:flutter_form_builder/flutter_form_builder.dart';
import 'package:shared_preferences/shared_preferences.dart';

class MainMenuView extends StatelessWidget {
  const MainMenuView({super.key});

  @override
  Widget build(BuildContext context) {
    // ask for default generation path when it is not defined
    if (!kIsWeb) {
      _askForGenerationPath(context);
    }

    return Container(
        padding: const EdgeInsets.all(20.0),
        child: SingleChildScrollView(
          child: Column(
            crossAxisAlignment: CrossAxisAlignment.center,
            mainAxisAlignment: MainAxisAlignment.center,
            children: [
              const SizedBox(height: 50.0),
              const Text(
                'Welcome to Dimlpfidex Config Generator',
                style: TextStyle(
                  fontSize: 34.0,
                  fontWeight: FontWeight.bold,
                ),
              ),
              const SizedBox(height: 50.0),
              Card(
                color: Colors.grey[300],
                elevation: 5.0,
                child: Container(
                  padding: const EdgeInsets.all(20.0),
                  width: 600.0,
                  child: const Column(
                    crossAxisAlignment: CrossAxisAlignment.start,
                    children: [
                      Text(
                        'What is Dimlpfidex Config Generator?',
                        style: TextStyle(
                          fontSize: 22.0,
                          fontWeight: FontWeight.bold,
                        ),
                      ),
                      SizedBox(height: 10.0),
                      Text(
                          "Dimlpfidex Config Generator is a helpful tool designed for users who are not familiar with JSON configuration files. These files are used to run various Dimlpfidex algorithms. \n\nThis application allows you to generate JSON configuration files through a user-friendly GUI with input fields.",
                          style: TextStyle(fontSize: 17.0)),
                    ],
                  ),
                ),
              ),
              const SizedBox(height: 20.0),
              Card(
                color: Colors.grey[300],
                elevation: 5.0,
                child: Container(
                  padding: const EdgeInsets.all(20.0),
                  width: 600.0,
                  child: const Column(
                    crossAxisAlignment: CrossAxisAlignment.start,
                    children: [
                      Text(
                        'How to use:',
                        style: TextStyle(
                          fontSize: 20.0,
                          fontWeight: FontWeight.bold,
                        ),
                      ),
                      SizedBox(height: 10.0),
                      Text(
                        '1. Fill in the necessary fields in the GUI.',
                        style: TextStyle(
                          fontSize: 17.0,
                        ),
                      ),
                      Text(
                        '2. Use the glossary to understand each field.',
                        style: TextStyle(
                          fontSize: 17.0,
                        ),
                      ),
                      Text(
                        '3. Generate your JSON configuration file easily by clicking on a button.',
                        style: TextStyle(
                          fontSize: 17.0,
                        ),
                      ),
                    ],
                  ),
                ),
              ),
              ElevatedButton(
                  onPressed: () => _openSettingsDialog(context),
                  child: const Icon(Icons.settings))
            ],
          ),
        ));
  }
}

void _askForGenerationPath(BuildContext context) async {
  String confKey = "generationPath";
  await SharedPreferences.getInstance()
      .then((value) => value.getString(confKey) ?? "")
      .then((value) {
    if (value.isEmpty) {
      return showDialog(
          context: context,
          builder: (BuildContext context) {
            return AlertDialog(
              title: const Text("Welcome !"),
              content: const Text(
                  "Before starting, we need to know where, in your computer, would you\nlike do keep all your future generated files.\n\nClick 'understood' to proceed."),
              actions: [
                SimpleButton(
                    onPressed: () {
                      _openSettingsDialog(context);
                      Navigator.of(context).pop();
                    },
                    label: "Understood"),
              ],
            );
          });
    }
  });
}

void _openSettingsDialog(BuildContext context) async {
  GlobalKey<FormBuilderFieldState> key = GlobalKey<FormBuilderFieldState>();
  String confKey = "generationPath";

  await SharedPreferences.getInstance().then((sp) {
    String path = sp.getString(confKey) ?? "";

    return showDialog(
        context: context,
        builder: (BuildContext context) {
          return AlertDialog(
            content: Row(
                mainAxisAlignment: MainAxisAlignment.center,
                crossAxisAlignment: CrossAxisAlignment.center,
                children: [
                  Expanded(
                      child: FormBuilderTextField(
                    key: key,
                    name: confKey,
                    initialValue: path,
                    decoration: const InputDecoration(
                        hintText: "Path where config files will be generated"),
                    onChanged: (changes) => path = changes ?? "",
                  )),
                  IconButton(
                      onPressed: () async {
                        getDirectoryPath().then((value) {
                          if (value != null && value.isNotEmpty) {
                            key.currentState?.didChange(value);
                            path = value;
                          }
                        }).catchError((e) {
                          showSnackBar(context, "Something went wrong: $e");
                        });
                      },
                      icon: const Icon(Icons.file_open))
                ]),
            actions: [
              SimpleButton(
                  onPressed: () {
                    bool isValidDir = Directory.fromUri(Uri(path:path)).existsSync();

                    if (!isValidDir || path.isEmpty) {
                      showSnackBar(context,
                          "Path '$path' is not valid a valid directory path. Please specify a valid path.",
                          color: Colors.red[500]!);
                    } else {
                      sp.setString(confKey, path);
                      showSnackBar(context,
                          "Successfully changed generation path to: $path",
                          color: Colors.green[700]!);
                      Navigator.of(context).pop();
                    }
                  },
                  label: "Save")
            ],
          );
        });
  });
}
