#include <SFML/Graphics.hpp>
#include <iostream>
#include <sstream>
#include <fstream>
#include "mazeGame.h"

enum gameState {
    MENU,
    MAZE,
    END_SCREEN
};
gameState currentState = MENU;

int main(){

    int difficulty = 0;
    const float mainButtonSpacing = 250.f;
    const float diffButtonSpacing = 25.f;

    // Create Window and define dimensions
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    const int screenWidth = desktop.width;
    const int screenHeight= desktop.height;
    sf::RenderWindow window(desktop, "Maze", sf::Style::Fullscreen);

    sf::Texture textureBackground;
    if(!textureBackground.loadFromFile("assets/menuCarRain.jpg")){
        return -1;
    }
    sf::Sprite menuBackground(textureBackground);
    menuBackground.setPosition(0.f, 0.f);
    menuBackground.setScale((float)(window.getSize().x/(float)(textureBackground.getSize().x)), 
    (float)(window.getSize().y/(float)(textureBackground.getSize().y)));

    // Create Start Button + Start Text
    sf::RectangleShape startButton({400.f, 200.f});
    startButton.setOutlineColor(sf::Color::White);
    startButton.setOutlineThickness(5.f);
    sf::FloatRect startBounds = startButton.getLocalBounds();
    startButton.setOrigin(startBounds.width/2.f, startBounds.height/2.f);
    startButton.setPosition(screenWidth/2.f, screenHeight/2.f);

    sf::Text startText;
    sf::Font font;
    if(!font.loadFromFile("/usr/share/fonts/truetype/msttcorefonts/arial.ttf")){
        return -1;
    }
    startText.setFont(font);
    startText.setFillColor(sf::Color::White);
    startText.setString("START");
    startText.setCharacterSize(100);
    sf::FloatRect startTextBounds = startText.getLocalBounds();
    startText.setOrigin(startTextBounds.left + startTextBounds.width / 2.f, startTextBounds.top + startTextBounds.height / 2.f);
    startText.setPosition(startButton.getPosition());

    // Create Easy / Medium / Hard Buttons
    sf::RectangleShape easyButton({400.f, 200.f});
    easyButton.setOutlineColor(sf::Color::Green);
    easyButton.setOutlineThickness(5.f);
    sf::FloatRect easyBounds = easyButton.getLocalBounds();
    easyButton.setOrigin(easyBounds.width/2.f, easyBounds.height/2.f);
    easyButton.setPosition(startButton.getPosition().x + startButton.getSize().x + mainButtonSpacing, screenHeight/3.f);

    sf::Text easyButtonText;
    easyButtonText.setFont(font);
    easyButtonText.setFillColor(sf::Color::White);
    easyButtonText.setString("EASY");
    easyButtonText.setCharacterSize(100);
    sf::FloatRect easyButtonTextBounds = easyButtonText.getLocalBounds();
    easyButtonText.setOrigin(easyButtonTextBounds.left + easyButtonTextBounds.width / 2.f, easyButtonTextBounds.top + easyButtonTextBounds.height / 2.f);
    easyButtonText.setPosition(easyButton.getPosition());
    
    sf::RectangleShape mediumButton({400.f, 200.f});
    mediumButton.setOutlineColor(sf::Color::White);
    mediumButton.setOutlineThickness(5.f);
    sf::FloatRect mediumBounds = mediumButton.getLocalBounds();
    mediumButton.setOrigin(mediumBounds.width/2.f, mediumBounds.height/2.f);
    mediumButton.setPosition(startButton.getPosition().x + startButton.getSize().x + mainButtonSpacing, 
        screenHeight/3.f + easyButton.getSize().y + diffButtonSpacing);

    sf::Text mediumButtonText;
    mediumButtonText.setFont(font);
    mediumButtonText.setFillColor(sf::Color::White);
    mediumButtonText.setString("MEDIUM");
    mediumButtonText.setCharacterSize(100);
    sf::FloatRect mediumButtonTextBounds = mediumButtonText.getLocalBounds();
    mediumButtonText.setOrigin(mediumButtonTextBounds.left + mediumButtonTextBounds.width / 2.f, mediumButtonTextBounds.top + mediumButtonTextBounds.height / 2.f);
    mediumButtonText.setPosition(mediumButton.getPosition());

    sf::RectangleShape hardButton({400.f, 200.f});
    hardButton.setOutlineColor(sf::Color::White);
    hardButton.setOutlineThickness(5.f);
    sf::FloatRect hardBounds = hardButton.getLocalBounds();
    hardButton.setOrigin(hardBounds.width/2.f, hardBounds.height/2.f);
    hardButton.setPosition(startButton.getPosition().x + startButton.getSize().x + mainButtonSpacing, 
        screenHeight/3.f + 2*easyButton.getSize().y + 2*diffButtonSpacing);

    sf::Text hardButtonText;
    hardButtonText.setFont(font);
    hardButtonText.setFillColor(sf::Color::White);
    hardButtonText.setString("HARD");
    hardButtonText.setCharacterSize(100);
    sf::FloatRect hardButtonTextBounds = hardButtonText.getLocalBounds();
    hardButtonText.setOrigin(hardButtonTextBounds.left + hardButtonTextBounds.width / 2.f, hardButtonTextBounds.top + hardButtonTextBounds.height / 2.f);
    hardButtonText.setPosition(hardButton.getPosition());

    bool showScore = false;

    float finalTime;
    sf::Text finalTimeText;
    finalTimeText.setFont(font);
    finalTimeText.setCharacterSize(100);
    finalTimeText.setFillColor({204, 204, 0, 200});

    sf::Text highScoreText;
    highScoreText.setFont(font);
    highScoreText.setCharacterSize(70);
    highScoreText.setFillColor({139, 0, 0}); // Change to {178, 34, 34} if needed

    float highScoreEasy, highScoreMedium, highScoreHard;
    std::ifstream inFile("highScore.txt");
    if(inFile.is_open()){
        inFile >> highScoreEasy;
        inFile >> highScoreMedium;
        inFile >> highScoreHard;
        inFile.close();
    }
    sf::Event event;
    while(window.isOpen())
    {
        //Make mouse position relative to window and converted to float for comparison with rectangle
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        sf::Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

        while(window.pollEvent(event))
        {
            // Close window event
            if (event.type == sf::Event::Closed) window.close();

            // Close if the escape key is pressed
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) window.close();

            // Run main maze game
            if(currentState == MENU){
                if (event.type == sf::Event::MouseButtonPressed && easyButton.getGlobalBounds().contains(mousePosF) && 
                    event.mouseButton.button == sf::Mouse::Button::Left){
                    easyButton.setOutlineColor(sf::Color::Green);
                    mediumButton.setOutlineColor(sf::Color::White);
                    hardButton.setOutlineColor(sf::Color::White);
                    difficulty = 0;
                }
                else if (event.type == sf::Event::MouseButtonPressed && mediumButton.getGlobalBounds().contains(mousePosF) && 
                    event.mouseButton.button == sf::Mouse::Button::Left){
                    easyButton.setOutlineColor(sf::Color::White);
                    mediumButton.setOutlineColor(sf::Color::Green);
                    hardButton.setOutlineColor(sf::Color::White);
                    difficulty = 1;
                }
                else if (event.type == sf::Event::MouseButtonPressed && hardButton.getGlobalBounds().contains(mousePosF) && 
                    event.mouseButton.button == sf::Mouse::Button::Left){
                    easyButton.setOutlineColor(sf::Color::White);
                    mediumButton.setOutlineColor(sf::Color::White);
                    hardButton.setOutlineColor(sf::Color::Green);
                    difficulty = 2;
                }
                else if (event.type == sf::Event::MouseButtonPressed && startButton.getGlobalBounds().contains(mousePosF) && 
                event.mouseButton.button == sf::Mouse::Button::Left){
                    displayCountDown(window);
                    finalTime = displayMaze(window, difficulty);
                    if(finalTime != 0.0f) showScore = true;
                    else showScore = false;

                    std::ofstream outFile;
                    if( (difficulty == 0 && finalTime < highScoreEasy || highScoreEasy == 0.0f) && (finalTime != 0.0f) ){
                        outFile.open("highScore.txt");
                        highScoreEasy = finalTime;
                    }
                    else if( (finalTime < highScoreMedium || highScoreMedium == 0.0f) && (finalTime != 0.0f && difficulty == 1) ){
                        outFile.open("highScore.txt");
                        highScoreMedium = finalTime;
                    }
                    else if( (finalTime < highScoreHard || highScoreHard == 0.0f) && (finalTime != 0.0f && difficulty == 2) ){
                        outFile.open("highScore.txt");
                        highScoreHard = finalTime;
                    }
                    if(outFile.is_open()){
                        outFile << highScoreEasy << std::endl;
                        outFile << highScoreMedium << std::endl;
                        outFile << highScoreHard << std::endl;
                        outFile.close();
                    }
                }
            }
            else if(currentState == MAZE){

            }
            else if(currentState == END_SCREEN){

            }

        }
        // Check if mouse is hovering over start button / Easy / Medium / Hard
        if(startButton.getGlobalBounds().contains(mousePosF)) startButton.setFillColor({200, 200, 200, 50});
        else startButton.setFillColor({255, 255, 255, 50});
        // Easy
        if(easyButton.getGlobalBounds().contains(mousePosF)) easyButton.setFillColor({200, 200, 200, 50});
        else easyButton.setFillColor({255, 255, 255, 50});
        // Medium
        if(mediumButton.getGlobalBounds().contains(mousePosF)) mediumButton.setFillColor({200, 200, 200, 50});
        else mediumButton.setFillColor({255, 255, 255, 50});
        // Hard
        if(hardButton.getGlobalBounds().contains(mousePosF)) hardButton.setFillColor({200, 200, 200, 50});
        else hardButton.setFillColor({255, 255, 255, 50});

        window.draw(menuBackground);
        window.draw(startButton);
        window.draw(startText);
        window.draw(easyButton);
        window.draw(easyButtonText);
        window.draw(mediumButton);
        window.draw(mediumButtonText);
        window.draw(hardButton);
        window.draw(hardButtonText);
        if(showScore){
            std::ostringstream ss;
            ss.precision(2);
            ss << std::fixed << "Last Time: " << finalTime << " s";
            finalTimeText.setString(ss.str());
            sf::FloatRect timerBounds = finalTimeText.getLocalBounds();
            finalTimeText.setOrigin(timerBounds.width/2.f, timerBounds.height/2.f);
            finalTimeText.setPosition(screenWidth/2.f, 125.f);
            window.draw(finalTimeText);
        }
        if(highScoreEasy != 0.0f && difficulty == 0){
            std::ostringstream ss;
            ss.precision(2);
            ss << std::fixed << "High Score: " << highScoreEasy << " s";
            highScoreText.setString(ss.str());
            sf::FloatRect highScoreBounds = highScoreText.getLocalBounds();
            highScoreText.setOrigin(highScoreBounds.width/2.f, highScoreBounds.height/2.f);
            highScoreText.setPosition(screenWidth/2.f, 800.f);
            window.draw(highScoreText);
        }
        else if(highScoreMedium != 0.0f && difficulty == 1){
            std::ostringstream ss;
            ss.precision(2);
            ss << std::fixed << "High Score: " << highScoreMedium << " s";
            highScoreText.setString(ss.str());
            sf::FloatRect highScoreBounds = highScoreText.getLocalBounds();
            highScoreText.setOrigin(highScoreBounds.width/2.f, highScoreBounds.height/2.f);
            highScoreText.setPosition(screenWidth/2.f, 800.f);
            window.draw(highScoreText);
        }
        else if(highScoreHard != 0.0f && difficulty == 2){
            std::ostringstream ss;
            ss.precision(2);
            ss << std::fixed << "High Score: " << highScoreHard << " s";
            highScoreText.setString(ss.str());
            sf::FloatRect highScoreBounds = highScoreText.getLocalBounds();
            highScoreText.setOrigin(highScoreBounds.width/2.f, highScoreBounds.height/2.f);
            highScoreText.setPosition(screenWidth/2.f, 800.f);
            window.draw(highScoreText);
        }
        window.display();
    }
    return 0;
}