import 'package:flutter/material.dart';
import 'package:flutter/services.dart' show rootBundle;
import 'dart:io';
import 'dart:async';
import 'dart:convert';

Future<String> readQuestions() async {
  final dir = await rootBundle.loadString('assets/trivia_test_s.json');
  // final file = File('${dir.path}/trivia_test_s.json');

  // if(await file.exists()){
  //   final contents = await file.readAsString();
  //   return contents;
  // } else {
  //   print('File does not exist');
  //   return "";
  // }
  return "";
}


void main() async {
  WidgetsFlutterBinding.ensureInitialized();
  final questions = await readQuestions();
  // List<dynamic> data = jsonDecode(questions);

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
