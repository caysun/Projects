import 'package:flutter/material.dart';
import 'package:flutter/services.dart' show rootBundle;
import 'package:html_unescape/html_unescape.dart';
import 'package:audioplayers/audioplayers.dart';
import 'package:shared_preferences/shared_preferences.dart';
import 'package:flutter_svg/flutter_svg.dart';
import 'package:auto_size_text/auto_size_text.dart';
import 'dart:io';
import 'dart:async';
import 'dart:convert';
import 'dart:math';

final unescape = HtmlUnescape();

Future<String> readQuestions() async {

  try {
    final String contents = await rootBundle.loadString('assets/trivia_test.json');
    return contents;
  } catch(e) {
    print('File does not exist');
    return "";
  }

}

Future<void> loadStats() async {
  final prefs = await SharedPreferences.getInstance();
  int? saved = prefs.getInt('numRounds');
  if (saved != null) numRounds = saved;
  double? saved2 = prefs.getDouble('average');
  if (saved2 != null) average = saved2;
  int? saved3 = prefs.getInt('highScore');
  if (saved3 != null) highScore = saved3;
}

class Question {
  String triviaQuestion;
  String category;
  String correctAnswer;
  List<dynamic> combinedAnswers;

  Question(this.triviaQuestion, this.category, 
  this.correctAnswer, this.combinedAnswers);
}

String scoreName (double score){
  if(score >= 18.00){
    return "Socrates";
  } else if(score >= 14.00){
    return "Jennings";
  } else if(score >= 8.00){
    return "Joe";
  } else if(score >= 5.00){
    return "Beetlejuice";
  } else if(score >= 0.00){
    return "Dexter";
  }
  return "Dexter";
}

int latestScore = -1;
int highScore = -1;
int numRounds = 0;
double average = 0.0;
const Color creamColor = Color.fromARGB(255, 253, 249, 228);

void main() async {
  WidgetsFlutterBinding.ensureInitialized();
  final questions = await readQuestions();
  await loadStats();
  List<dynamic> triviaInfo = jsonDecode(questions);
  List<Question> allQuestions = [];
  for(var info in triviaInfo){
    if(info is Map<String, dynamic>){
      if(info['choices'].length == 2) continue;
      info['choices'].shuffle(Random());
      Question question = Question(
        info['question'],
        info['category'],
        info['answer'],
        info['choices'],
      );
      allQuestions.add(question);
    }
  }
  runApp(MainApp(allQuestions: allQuestions));
}

class MainApp extends StatelessWidget {
  List<Question> allQuestions;
  MainApp({super.key, required this.allQuestions});

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: "Triva Master",
      theme: ThemeData(primarySwatch: Colors.blue),
      home: Menu(allQuestions: allQuestions),  
    );
  }
}

class Menu extends StatefulWidget {
  List<Question> allQuestions;
  Menu({super.key, required this.allQuestions});

  _MenuState createState() => _MenuState();
}

class _MenuState extends State<Menu> {

  @override
  void initState() {
    super.initState();
    _loadStatsToState();
  }

  Future<void> _loadStatsToState() async {
    final prefs = await SharedPreferences.getInstance();
    setState(() {
      int? saved = prefs.getInt('numRounds');
      if (saved != null) numRounds = saved;
      double? saved2 = prefs.getDouble('average');
      if (saved2 != null) average = saved2;
      int? saved3 = prefs.getInt('highScore');
      if (saved3 != null) highScore = saved3;
      // You could also optionally reset latestScore to -1 here if needed
    });
  }

  @override
  Widget build(BuildContext context) {
    return  Scaffold(
      backgroundColor: Colors.black,
      body: SafeArea(
        child: Stack(
          children: [
          // Latest Score at the top center
          if (latestScore != -1)
            Align(
              alignment: Alignment.topCenter,
              child: Padding(
                padding: const EdgeInsets.only(top: 300),
                child: Text(
                  'Last Score: ${latestScore}  ${scoreName(latestScore.toDouble())}',
                  style: const TextStyle(fontSize: 20, fontWeight: FontWeight.bold, color: creamColor),
                ),
              ),
            ),
            // High Score under last score
            if (highScore != -1)
              Align(
                alignment: Alignment.topCenter,
                child: Padding(
                  padding: const EdgeInsets.only(top: 180),
                  child: Text(
                    'High Score: ${highScore}  ${scoreName(highScore.toDouble())}',
                    style: const TextStyle(fontSize: 20, fontWeight: FontWeight.bold, color: creamColor),
                  ),
                ),
              ),
            // Average Score under High Score
            if (highScore != -1)
              Align(
                alignment: Alignment.topCenter,
                child: Padding(
                  padding: const EdgeInsets.only(top: 240),
                  child: Text(
                    'Average Score: ${average.toStringAsFixed(2)}  ${scoreName(average)}',
                    style: const TextStyle(fontSize: 20, fontWeight: FontWeight.bold, color: creamColor),
                  ),
                ),
              ),
            Center(
              child: ElevatedButton(
                child: const Text('START NEW GAME', style: TextStyle(fontSize: 20.0)),
                onPressed: (){
                  widget.allQuestions.shuffle(Random());
                  List<Question> triviaList = widget.allQuestions.take(20).toList();
                  Navigator.pushReplacement(
                    context,
                    MaterialPageRoute(
                      builder: (context) => Game(triviaList: triviaList, allQuestions: widget.allQuestions),
                    ),
                  );
                },
              )
            ),
            Align(
              alignment: Alignment.bottomCenter,
              child: Padding(
                padding: const EdgeInsets.only(bottom: 100.0),
                child: Column(
                  mainAxisSize: MainAxisSize.min,
                  children: const [
                    Text("Awards", style: TextStyle(fontSize: 20, color: creamColor)),
                    SizedBox(height: 8),
                    Text("Socrates Mastery Award: 18–20", style: TextStyle(fontSize: 17, color: creamColor)),
                    SizedBox(height: 8),
                    Text("Ken Jennings Excellence Award: 14–17", style: TextStyle(fontSize: 17, color: creamColor)),
                    SizedBox(height: 8),
                    Text("Joe Mediocrity Award: 8–13", style: TextStyle(fontSize: 17, color: creamColor)),
                    SizedBox(height: 8),
                    Text("Beetlejuice Try Again Award: 5–7", style: TextStyle(fontSize: 17, color: creamColor)),
                    SizedBox(height: 8),
                    Text("Timothy Dexter Original Thinker Award: 0–4", style: TextStyle(fontSize: 17, color: creamColor)),
                    SizedBox(height: 8),
                  ],
                ),
              ),
            ),
            Container(
              color: Colors.black,
              padding: const EdgeInsets.symmetric(vertical: 12),
              child: Row(
                mainAxisAlignment: MainAxisAlignment.center,
                children: [
                  SvgPicture.asset(
                    'assets/images/chiliPepperVector.svg',
                    height: 80, // try 80–100 if needed
                  ),
                  const SizedBox(width: 10),
                  const Text(
                    'Trivia Spice',
                    style: TextStyle(
                      fontSize: 35,
                      fontWeight: FontWeight.bold,
                      color: Color.fromARGB(255, 137, 16, 16),
                    ),
                  ),
                ],
              ),
            ),
          ]
        )
      ),
    );
  }
}

class Game extends StatefulWidget {
  final List<Question> triviaList;
  final List<Question> allQuestions;
  const Game({super.key, required this.triviaList, required this.allQuestions});

  @override
  State<Game> createState() => _MainAppState();
}

class _MainAppState extends State<Game> {
  int currentQuestionIndex = 0;
  int correctAnswerCount = 0;
  int totalAnswerCount = 0;
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

    if (currentQuestionIndex < widget.triviaList.length - 1) {
      setState(() {
        currentQuestionIndex++;
        selectedAnswer = "";
        showCorrectState = false;
        answered = false;
      });
    } else {
      latestScore = correctAnswerCount;
      highScore = max(correctAnswerCount, highScore);
      average = ((average*numRounds)+latestScore) / ++numRounds;
      final prefs = await SharedPreferences.getInstance();
      prefs.setInt('highScore', highScore);
      prefs.setInt('numRounds', numRounds);
      prefs.setDouble('average', average);

      Navigator.pushReplacement(
        context,
        MaterialPageRoute(
          builder: (context) => Menu(allQuestions: widget.allQuestions),
        ),
      );
    }
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
    if(isCorrect) correctAnswerCount++;
    totalAnswerCount++;
    await _playSound(isCorrect);
    await Future.delayed(Duration(milliseconds:2000));
    _showNextQuestion();
  }

  Color _getAnswerColor(String answer){
    if(!answered){
      return selectedAnswer == answer ? Colors.blue[200]! : creamColor;
    }
    if(answer == widget.triviaList[currentQuestionIndex].correctAnswer) return Colors.green[200]!;
    if(answer == selectedAnswer) return Colors.red[200]!;
    return creamColor;
  }
  
  @override
  Widget build(BuildContext context) {
    final question = widget.triviaList[currentQuestionIndex];

    return Scaffold(
      backgroundColor: Colors.black,
      body: SafeArea(
        child: Column(
          children: [
            // Header with title and score
            Container(
              padding: const EdgeInsets.symmetric(vertical: 12),
              color: Colors.black,
              child: Column(
                children: [
                  Row(
                    mainAxisAlignment: MainAxisAlignment.center,
                    children: [
                      SvgPicture.asset(
                        'assets/images/chiliPepperVector.svg',
                        height: 65,
                      ),
                      const SizedBox(width: 10),
                      const Text(
                        'Trivia Spice',
                        style: TextStyle(
                          fontSize: 30,
                          fontWeight: FontWeight.bold,
                          color: Color.fromARGB(255, 137, 16, 16),
                        ),
                      ),
                    ],
                  ),
                  const SizedBox(height: 20),
                  Text(
                    'Score: $correctAnswerCount / $totalAnswerCount',
                    style: const TextStyle(
                      fontSize: 19.0,
                      fontWeight: FontWeight.bold,
                      color: creamColor,
                    ),
                  ),
                ],
              ),
            ),

            // Scrollable content below fixed header
            Expanded(
              child: RawScrollbar(
                thumbVisibility: true,
                thumbColor: Colors.white,
                radius: const Radius.circular(20),
                thickness: 8, 
                child: SingleChildScrollView(
                  primary: true,
                  padding: const EdgeInsets.symmetric(horizontal: 16.0, vertical: 16.0),
                  child: Center(
                    child: ConstrainedBox(
                      constraints: BoxConstraints(
                        minHeight: MediaQuery.of(context).size.height * 0.65,
                      ),
                      child: Column(
                        mainAxisAlignment: MainAxisAlignment.center,
                        children: [
                          Text(
                            unescape.convert(question.triviaQuestion),
                            style: const TextStyle(
                              fontSize: 20.0,
                              fontWeight: FontWeight.w100,
                              color: Color.fromARGB(255, 110, 172, 207),
                            ),
                            textAlign: TextAlign.center,
                          ),
                          const SizedBox(height: 20.0),
                          ...question.combinedAnswers.map(
                            (answer) => Padding(
                              padding: const EdgeInsets.symmetric(vertical: 6),
                              child: InkWell(
                                onTap: () => _handleAnswerTap(answer),
                                borderRadius: BorderRadius.circular(12),
                                splashColor: Colors.blueAccent.withOpacity(0.3),
                                child: Container(
                                  padding: const EdgeInsets.all(14),
                                  decoration: BoxDecoration(
                                    color: _getAnswerColor(answer),
                                    borderRadius: BorderRadius.circular(12),
                                    border: Border.all(color: creamColor),
                                  ),
                                  child: Center(
                                    child: Text(
                                      unescape.convert(answer),
                                      style: const TextStyle(
                                        fontSize: 16,
                                        fontWeight: FontWeight.bold,
                                      ),
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
              ),
            )
          ],
        ),
      ),
    );
  }
}
