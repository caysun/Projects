#include <SFML/Graphics.hpp>
#include <iostream>
#include <sstream>
#include <stack>
#include <queue>
#include <map>
#include <cmath>
#include <cctype>
#include <climits>


enum Assoc { LEFT, RIGHT };

struct Operator {
    int precedence;
    Assoc associativity;
};

// Supported operators
const std::map<wchar_t, Operator> ops = {
    {L'+', {1, LEFT}},
    {L'-', {1, LEFT}},
    {L'*', {2, LEFT}},
    {L'÷', {2, LEFT}},
    {L'^', {3, RIGHT}},
};

// Check if a character is an operator
bool isOperator(wchar_t c) {
    return ops.find(c) != ops.end();
}

// Apply the operator to two operands
double applyOperator(wchar_t op, double a, double b) {
    switch (op) {
        case L'+': return a + b;
        case L'-': return a - b;
        case L'*': return a * b;
        case L'÷': return a / b;
        case L'^': return std::pow(a, b);
        default: throw std::runtime_error("Unknown operator");
    }
}

// Tokenize input expression
std::queue<std::wstring> tokenize(const std::wstring& expr) {
    std::queue<std::wstring> tokens;
    std::wstringstream iss(expr);
    wchar_t c;
    wchar_t prev = L'\0'; // track previous char

    while (iss >> std::noskipws >> c) {
        if(std::iswspace(c)){
            continue; // Skip over spaces
        }
        if (std::iswdigit(c) || c == L'.' || (c == L'-' && 
            (tokens.empty() || prev == L'(' || isOperator(prev)))) {

            std::wstring number(1, c);
            while (iss.peek() != WEOF && (std::iswdigit(iss.peek()) || iss.peek() == L'.')) {
                wchar_t next;
                iss >> next;
                number += next;
            }
            tokens.push(number);
            prev = number.back(); // Update prev to last token
        } else if (isOperator(c) || c == L'(' || c == L')') {
            tokens.push(std::wstring(1, c));
            prev = c; // Update prev to last token
        }
    }
    return tokens;
}

// Convert infix to postfix using shunting yard
std::queue<std::wstring> toPostfix(const std::queue<std::wstring>& input) {
    std::queue<std::wstring> output;
    std::stack<std::wstring> opStack;
    std::queue<std::wstring> tokens = input;

    while (!tokens.empty()) {
        std::wstring token = tokens.front();
        tokens.pop();

        if (std::isdigit(token[0]) || (token.size() > 1 && token[0] == L'-' && std::isdigit(token[1]))) {
            output.push(token);
        } else if (isOperator(token[0])) {
            while (!opStack.empty()) {
                std::wstring top = opStack.top();
                if (isOperator(top[0])) {
                    Operator topOp = ops.at(top[0]);
                    Operator currOp = ops.at(token[0]);
                    if ((currOp.associativity == LEFT && currOp.precedence <= topOp.precedence) ||
                        (currOp.associativity == RIGHT && currOp.precedence < topOp.precedence)) {
                        output.push(top);
                        opStack.pop();
                        continue;
                    }
                }
                break;
            }
            opStack.push(token);
        } else if (token == L"(") {
            opStack.push(token);
        } else if (token == L")") {
            while (!opStack.empty() && opStack.top() != L"(") {
                output.push(opStack.top());
                opStack.pop();
            }
            if (!opStack.empty()) opStack.pop(); // pop the '('
        }
    }

    while (!opStack.empty()) {
        output.push(opStack.top());
        opStack.pop();
    }

    return output;
}

// Evaluate postfix expression
double evaluatePostfix(std::queue<std::wstring> postfix) {
    std::stack<double> values;

    while (!postfix.empty()) {
        std::wstring token = postfix.front();
        postfix.pop();

        if (isOperator(token[0]) && token.size() == 1) {
            double b = values.top(); values.pop();
            double a = values.top(); values.pop();
            values.push(applyOperator(token[0], a, b));
        } else {
            values.push(std::stod(token));
        }
    }

    return values.top();
}

// Main evaluation function
double getOutput(const std::wstring& expression) {
    auto tokens = tokenize(expression);
    auto postfix = toPostfix(tokens);
    return evaluatePostfix(postfix);
}

std::string trimTrailingZeros(const std::string& numStr) {
    std::string result = numStr;
    // Find decimal point
    size_t dotPos = result.find('.');
    if (dotPos != std::string::npos) {
        // Remove trailing zeros
        size_t lastNonZero = result.find_last_not_of('0');
        if (lastNonZero != std::string::npos && lastNonZero > dotPos) {
            result.erase(lastNonZero + 1);
        } else {
            // If all zeros after decimal point, remove them and decimal point
            result.erase(dotPos);
        }
    }
    return result;
}

std::wstring addParentheses(const std::wstring& expr) {
    std::wstring result;
    for (size_t i = 0; i < expr.size(); ++i) {
        wchar_t c = expr[i];

        if (c == L'-') {
            // Check if it's a unary minus (start of expr or follows an operator or '(')
            bool isUnary = (i == 0 || 
                            expr[i - 1] == L'+' || 
                            expr[i - 1] == L'-' || 
                            expr[i - 1] == L'*' || 
                            expr[i - 1] == L'÷' || 
                            expr[i - 1] == L'(');

            if (isUnary && (i + 1 < expr.size()) && iswdigit(expr[i + 1])) {
                result += L"(-";

                // Copy the number
                ++i;
                while (i < expr.size() && (iswdigit(expr[i]) || expr[i] == L'.')) {
                    result += expr[i++];
                }
                result += L')';
                --i; // adjust since loop will increment again
                continue;
            }
        }

        if(c == L'.'){
            // Check if it's a unary decimal (start of expr or follows an operator or '(')
            bool isUnary = (i == 0 || 
                            expr[i - 1] == L'+' || 
                            expr[i - 1] == L'-' || 
                            expr[i - 1] == L'*' || 
                            expr[i - 1] == L'÷' || 
                            expr[i - 1] == L'(');
            if (isUnary && (i + 1 < expr.size()) && iswdigit(expr[i + 1])) {
                result += L"0.";

                // Copy the number
                ++i;
                while (i < expr.size() && (iswdigit(expr[i]) || expr[i] == L'.')) {
                    result += expr[i++];
                }
                --i; // adjust since loop will increment again
                continue;
            }
        }

        result += c;
    }

    return result;
}
// (  - 2 )  + 3
std::wstring addSpaces(const std::wstring& expr) {
    std::wstring spaced;
    for (wchar_t c : expr) {
        if (c == L'+' || c == L'-' || c == L'*' || c == L'÷' || c == L'(' || c == L')') {
            spaced += L' ';
            spaced += c;
            spaced += L' ';
        } else {
            spaced += c;
        }
    }
    return spaced;
}

bool validDecimal(const sf::String &str){
    int ct = 0;
    for(auto x:str){
        if(ct>1) return false;
        if(x==L'.') ct++;
        if(x == L'*' || x == L'÷' || x == L'+' || x == L'-') ct = 0;
    }
    return true;
}

int main()
{
    // Create a window
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    
    //Define dimensions
    int screenWidth = desktop.width;
    int screenHeight = desktop.height;
    
    sf::RenderWindow window(desktop, "Calculator", sf::Style::Fullscreen);
    
    //Create a cursor
    sf::Cursor cursor;
    if (cursor.loadFromSystem(sf::Cursor::Arrow)) window.setMouseCursor(cursor);

    // Draw calculator body
    sf::RectangleShape rect({screenHeight/2.0f, screenHeight/1.5f});
    rect.setFillColor({100, 100, 100});
    rect.setOrigin(rect.getSize().x/2.0f, rect.getSize().y/2.0f);
    rect.setPosition({screenWidth/2.0f, screenHeight/2.0f});
    //window.draw(rect);

    //Draw rectangles
    std::vector<sf::RectangleShape> buttons;
    float startingX = rect.getPosition().x - rect.getSize().x/2.0f + 5.0f;
    float startingY = rect.getPosition().y - rect.getSize().y/2.0f + 10.0f + rect.getSize().y/5.0f;
    float offsetX = rect.getSize().x/4.0f;
    float offsetY = rect.getSize().y/5.0f;
    for(int i=0; i<4; i++){
        for(int j=0; j<4; j++){
            sf::RectangleShape symbol({(rect.getSize().x-40.0f)/4.0f, (rect.getSize().y-50.0f)/5.0f});
            symbol.setPosition({startingX + j*offsetX, startingY + i*offsetY});
            buttons.push_back(symbol);
        }
    }

    std::vector<sf::RectangleShape> lines;
    //Draw horizontal calculator lines
    startingY = rect.getPosition().y - rect.getSize().y/2.0f;
    startingX = rect.getPosition().x - rect.getSize().x/2.0f;
    for(int i=0; i<6; i++){
        sf::RectangleShape line({screenHeight/2.0f, 10.0f});
        line.setFillColor({255, 140, 0});
        line.setPosition({startingX, startingY + i*rect.getSize().y/5.0f});
        if(i==5){
            line.setSize({screenHeight/2.0f+10.0f, 10.0f});
            line.setPosition({startingX-5.0f, startingY + i*rect.getSize().y/5.0f});
        }
        lines.push_back(line);
    }

    //Draw vertical calculator lines
    for(int i=0; i<5; i++){
        sf::RectangleShape line({10.0f, rect.getSize().y*0.8f});
        line.setOrigin({line.getSize().x/2.0f, 0.0f});
        line.setFillColor({255, 140, 0});

        //If line is not at the end don't draw through output screen
        if(i!=0 && i!= 4) line.setPosition({startingX + i*rect.getSize().x/4.0f, startingY + rect.getSize().y/5.0f});
        else {
            line.setSize({10.f, rect.getSize().y});
            line.setPosition({startingX + i*rect.getSize().x/4.0f, startingY});
        }
        lines.push_back(line);
    }

    std::vector<sf::Text> symbols(16);
    //Display symbols on calculator
    std::vector<std::wstring> v = {L"7", L"8", L"9", L"÷", L"4", L"5", L"6", L"*", L"1", L"2", L"3", L"-", L"0", L".", L"=", L"+"};
    sf::Font font;
    if(!font.loadFromFile("/usr/share/fonts/truetype/msttcorefonts/arial.ttf")){
        return -1;
    }
    for(int i=0; i<16; i++){
        sf::Text text(v[i], font);
        text.setCharacterSize(48);
        text.setFillColor(sf::Color::Black);
        sf::FloatRect bounds = text.getLocalBounds();
        text.setOrigin(bounds.left + bounds.width/2.f, bounds.top + bounds.height/2.f);
        text.setPosition(buttons[i].getPosition() + buttons[i].getSize()/2.f);
        symbols[i] = text;
    }
    
    std::map<sf::RectangleShape*, sf::Text> rectToText;
    for(int i=0; i<16; i++) rectToText[&buttons[i]] = symbols[i];
    std::vector<int> buttonPosFromNumber = {12, 8, 9, 10, 4, 5, 6, 0, 1, 2};

    //Calculator Header
    sf::Text text("", font);
    text.setCharacterSize(48);
    text.setFillColor(sf::Color::Black);
    text.setOrigin(text.getGlobalBounds().getSize()/2.f + text.getLocalBounds().getPosition());

    const float headerY = rect.getPosition().y  - (rect.getSize().y)/2.f  + rect.getSize().y/6.f;
    const float headerX = rect.getPosition().x + rect.getSize().x/2.f - 5.0f;
    float offsetHeaderX = 0.f;
    int decimalCt = 0;
    sf::Event event;
    // run the program as long as the window is open
    while (window.isOpen())
    {
        //Make mouse position relative to window and converted to float for comparison with rectangle
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        sf::Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

        // Clear the window with black color
        window.clear(sf::Color::Black);
        for(const auto &line : lines) window.draw(line);
        //Check if cursor is over button
        for(auto &button : buttons){
            if(button.getGlobalBounds().contains(mousePosF)) button.setFillColor({175, 175, 175});
            else button.setFillColor(sf::Color::White);
            window.draw(button);
        }
        //Draw symbols
        for(const auto &symbol : symbols) window.draw(symbol);
        
        // Process events
        while (window.pollEvent(event))
        {
            // Close window event
            if (event.type == sf::Event::Closed) window.close();

            // Close if the escape key is pressed
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                window.close();
            }

            //Erase symbols in calculator header
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::BackSpace) {
                sf::String newString = text.getString();
                if(!newString.isEmpty()){
                    sf::Text tempText;
                    tempText.setFont(font);
                    tempText.setCharacterSize(text.getCharacterSize());
                    tempText.setString(newString[newString.getSize()-1]);
                    if(newString[newString.getSize()-1] == L'.') decimalCt--;
                    newString.erase(newString.getSize()-1);
                    text.setString(newString);
                }
            }

            //Calculate Input if Enter or Equal keys are pressed
            if (event.type == sf::Event::KeyPressed && !sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) && 
                !sf::Keyboard::isKeyPressed(sf::Keyboard::RShift) && (event.key.code == sf::Keyboard::Enter 
                || event.key.code == sf::Keyboard::Equal)) {
                if(!isOperator(text.getString()[text.getString().getSize()-1]) && 
                    text.getString()[text.getString().getSize()-1] != L'.' && (validDecimal(text.getString()+symbols[13].getString()))){
                    decimalCt = 0;
                    try{
                        std::wstring realInput = (addParentheses(text.getString().toWideString()));
                        std::string output = trimTrailingZeros(std::to_string(getOutput(realInput)));
                        text.setString(output);
                    } catch (const std::exception &e){
                        text.setString("Error");
                    }
                }
            }

            //If any number is pressed
            if (event.type == sf::Event::KeyPressed && !sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) && 
                !sf::Keyboard::isKeyPressed(sf::Keyboard::RShift) && event.key.code >= sf::Keyboard::Num0 && 
                event.key.code <= sf::Keyboard::Num9 && text.getString().getSize()<16) {
                int number = event.key.code - sf::Keyboard::Num0;
                int key = buttonPosFromNumber[number];
                sf::Text curText = symbols[key];
                text.setString(text.getString() + curText.getString());
            }

            //If any number is pressed on numpad
            if (event.type == sf::Event::KeyPressed && !sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) && 
                !sf::Keyboard::isKeyPressed(sf::Keyboard::RShift) && event.key.code >= sf::Keyboard::Numpad0 && 
                event.key.code <= sf::Keyboard::Numpad9 && text.getString().getSize()<16) {
                int number = event.key.code - sf::Keyboard::Numpad0;
                int key = buttonPosFromNumber[number];
                sf::Text curText = symbols[key];
                text.setString(text.getString() + curText.getString());
            }
            
            //If anything else is pressed
            if (event.type == sf::Event::KeyPressed && text.getString().getSize()<16) {
                // Check for SHIFT + '=' → '+' key
                if ((sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) || 
                    sf::Keyboard::isKeyPressed(sf::Keyboard::RShift)) && 
                    event.key.code == sf::Keyboard::Equal && text.getString().getSize()>0 
                    && (!isOperator(text.getString()[text.getString().getSize()-1]))) {
                    text.setString(text.getString() + symbols[15].getString());  // '+'
                }
                // Check for '-'
                else if (event.key.code == sf::Keyboard::Dash && text.getString()[text.getString().getSize()-1] != L'-') {
                    text.setString(text.getString() + symbols[11].getString());  // '-'
                }
                // Check for SHIFT + '8' → '*'
                else if ((sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) || 
                        sf::Keyboard::isKeyPressed(sf::Keyboard::RShift)) && 
                        event.key.code == sf::Keyboard::Num8 && !isOperator(text.getString()[text.getString().getSize()-1])
                        && text.getString().getSize()>0) {
                    text.setString(text.getString() + symbols[7].getString());   // '*'
                }
                // Check for '/'
                else if (event.key.code == sf::Keyboard::Slash && !isOperator(text.getString()[text.getString().getSize()-1])
                    && text.getString().getSize()>0) {
                    text.setString(text.getString() + symbols[3].getString());   // '/'
                }
                // Check for '.'
                else if (event.key.code == sf::Keyboard::Period) {
                    decimalCt++;
                    if (validDecimal(text.getString()+symbols[13].getString())) text.setString(text.getString() + symbols[13].getString());  // '.'
                }
            }

            //If button pressed, display on screen
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left){
                sf::Text curText;
                for(auto &button : buttons){
                    if(button.getFillColor() == sf::Color(175, 175, 175) && rectToText[&button].getString() != L"="){
                        if(text.getString().getSize() > 15) break;
                        curText = rectToText[&button];
                        if(isOperator(curText.getString()[0]) && curText.getString()[0] != L'-' && text.getString().getSize()==0) break;
                        if(curText.getString()[0] != L'-' && isOperator(curText.getString()[0]) && 
                            (isOperator(text.getString()[text.getString().getSize()-1]))) break;
                        else if(curText.getString()[0] == L'-' && text.getString()[text.getString().getSize()-1] == L'-') break;
                        if(curText.getString() == L".") decimalCt++;
                        if(!validDecimal(text.getString() + curText.getString())) break;
                        text.setString(text.getString() + curText.getString());
                        break;
                    }
                    else if(button.getFillColor() == sf::Color(175, 175, 175) && rectToText[&button].getString() == L"="
                        && (validDecimal(text.getString()+symbols[13].getString()))
                        && !isOperator(text.getString()[text.getString().getSize()-1])){
                        decimalCt = 0;
                        try{
                            std::wstring realInput = (addParentheses(text.getString().toWideString()));
                            std::string output = trimTrailingZeros(std::to_string(getOutput(realInput)));
                            text.setString(output);
                        } catch (const std::exception &e){
                            text.setString("Error");
                        }
                    }
                }
            }
        }
        //Update origin to always match digit bordering the rights side of the calculator
        text.setFillColor(sf::Color::White);
        sf::FloatRect bounds = text.getLocalBounds();
        text.setOrigin(bounds.left + bounds.width, bounds.top + bounds.height/2.f);
        text.setPosition(headerX, headerY);
        // Draw calculator header text
        window.draw(text);
        // Display what was drawn
        window.display();
    }
    return 0;
}