import 'package:flutter/material.dart';

class MainMenuView extends StatelessWidget {
  const MainMenuView({super.key});

  @override
  Widget build(BuildContext context) {
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
            ],
          ),
        ));
  }
}
