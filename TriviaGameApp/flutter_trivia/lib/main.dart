import 'package:flutter/material.dart';
import 'package:flutter/services.dart' show rootBundle;
import 'package:html_unescape/html_unescape.dart';
import 'package:audioplayers/audioplayers.dart';
import 'dart:io';
import 'dart:async';
import 'dart:convert';
import 'dart:math';

final unescape = HtmlUnescape();

Future<String> readQuestions() async {

  try {
    final String contents = await rootBundle.loadString('assets/trivia_test_s.json');
    return contents;
  } catch(e) {
    print('File does not exist');
    return "";
  }

}

List<String> getShuffledAnswers(String correctAnswer, List<dynamic> incorrectAnswers) {
  // Combine correct and incorrect answers
  List<String> allAnswers = List.from(incorrectAnswers); // Make a copy to avoid modifying original list
  allAnswers.add(correctAnswer);
  // Shuffle the list
  allAnswers.shuffle(Random());
  return allAnswers;
}

class Question {
  String type;
  String difficulty;
  String category;
  String triviaQuestion;
  String correctAnswer;
  List<dynamic> incorrectAnswers;
  List<String> combinedAnswers;

  Question(this.type, this.difficulty, this.category, this.triviaQuestion, 
  this.correctAnswer, this.incorrectAnswers, this.combinedAnswers);
}

void main() async {
  WidgetsFlutterBinding.ensureInitialized();
  final questions = await readQuestions();
  Map<String, dynamic> data = jsonDecode(questions);
  List<dynamic> triviaInfo = data['results'];
  List<Question> triviaList = [];
  for(var info in triviaInfo){
    if(info is Map<String, dynamic>){
      Question question = Question(
        info['type'],
        info['difficulty'],
        info['category'],
        info['question'],
        info['correct_answer'],
        info['incorrect_answers'],
        getShuffledAnswers(info['correct_answer'], info['incorrect_answers']),
      );
      triviaList.add(question);
    }
  }
  runApp(MainApp(triviaList: triviaList));
}


class MainApp extends StatefulWidget {
  final List<Question> triviaList;

  const MainApp({super.key, required this.triviaList});

  @override
  State<MainApp> createState() => _MainAppState();
}

class _MainAppState extends State<MainApp> {
  int currentQuestionIndex = 0;
  int correctAnswers = 0;
  int totalAnswers = 0;
  String selectedAnswer = "";
  bool showCorrectState = false;
  bool answered = false;
  bool isCorrect = false;
  final AudioPlayer player = AudioPlayer();

  Future<void> _playSound(bool isCorrect) async{
    await player.play(AssetSource(isCorrect ? 'sounds/correctAnswer.mp3' : 
      'sounds/incorrectAnswer.mp3'));
  }

  void _showNextQuestion() async{

    setState(() {
      if (currentQuestionIndex < widget.triviaList.length - 1) {
        currentQuestionIndex++;
      } else {
        currentQuestionIndex = 0; // Restart or end
      }
      selectedAnswer = "";
      showCorrectState = false;
      answered = false;
    });
  }

  void _handleAnswerTap(String answer) async{
    if(answered) return;

    setState((){
      selectedAnswer = answer;
      answered = true;
    });

    // Check if selected answer is correct
    String correctAnswer = widget.triviaList[currentQuestionIndex].correctAnswer;
    isCorrect = answer == correctAnswer;
    if(isCorrect) showCorrectState = true;
    await _playSound(isCorrect);
    await Future.delayed(Duration(milliseconds:2000));
    _showNextQuestion();
  }

  Color _getAnswerColor(String answer){
    if(!answered){
      return selectedAnswer == answer ? Colors.blue[200]! : Colors.grey[200]!;
    }
    if(answer == widget.triviaList[currentQuestionIndex].correctAnswer) return Colors.green[200]!;
    if(answer == selectedAnswer) return Colors.red[200]!;
    return Colors.grey[200]!;
  }

  @override
  Widget build(BuildContext context) {
    final question = widget.triviaList[currentQuestionIndex];

    return MaterialApp(
      title: 'Trivia Game',
      home: Scaffold(
        appBar: AppBar(title: const Text('Trivia Game')),
        body: Center(
          child: Padding(
            padding: const EdgeInsets.all(16.0),
            child: Column(
              mainAxisAlignment: MainAxisAlignment.center,
              children: [
                Text(
                  unescape.convert(question.triviaQuestion),
                  style: const TextStyle(fontSize: 20.0, fontWeight: FontWeight.bold),
                  textAlign: TextAlign.center,
                ),
                const SizedBox(height: 20),
                ...question.combinedAnswers.map(
                  (answer) => Padding(
                    padding: const EdgeInsets.symmetric(vertical: 6.0),
                    child: InkWell( 
                      onTap: () => _handleAnswerTap(answer),
                      borderRadius: BorderRadius.circular(12),
                      splashColor: Colors.blueAccent.withOpacity(0.3),
                      child: Container(
                        padding: const EdgeInsets.all(14),
                        decoration: BoxDecoration(
                          color: _getAnswerColor(answer),
                          borderRadius: BorderRadius.circular(12),
                          border: Border.all(color: Colors.grey.shade400),
                        ),
                        child: Center(
                          child: Text(
                            unescape.convert(answer),
                            style: const TextStyle(fontSize: 16),
                          ),
                        ),
                      ),
                    ),
                  ),
                ),
              ],
            ),
          ),
        ),
      ),
    );
  }
}
