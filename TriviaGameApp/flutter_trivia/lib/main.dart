import 'package:flutter/material.dart';
import 'package:flutter/services.dart' show rootBundle;
import 'dart:io';
import 'dart:async';
import 'dart:convert';

Future<String> readQuestions() async {

  try {
    final String contents = await rootBundle.loadString('assets/trivia_test_s.json');
    return contents;
  } catch(e) {
    print('File does not exist');
    return "";
  }

}

// cl

void main() async {
  WidgetsFlutterBinding.ensureInitialized();
  final questions = await readQuestions();
  Map<String, dynamic> data = jsonDecode(questions);
  data.forEach((key, value) {
    print('Key: $key');
    if (value is Map<String, dynamic>) {
      value.forEach((k, v) {
        print('  $k: $v');
      });
    } else {
      print('  Value: $value');
    }
    print('---');
  });

  runApp(const MainApp());
}

class MainApp extends StatelessWidget {
  const MainApp({super.key});

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'Trivia Game',
      home: Scaffold(
        appBar: AppBar(title: const Text('Trivia Game')),
        body: const Center(
          child: Text('Welcome to Trivia!'),
        ),
      ),
    );
  }
}
