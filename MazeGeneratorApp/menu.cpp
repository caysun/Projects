#include <SFML/Graphics.hpp>
#include <iostream>
#include "mazeGame.h"

enum gameState {
    MENU,
    MAZE,
    END_SCREEN
};
gameState currentState = MENU;

int main(){

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
    startButton.setPosition(screenWidth/2.f, screenHeight/3.f);

    sf::Text startText;
    sf::Font font;
    if(!font.loadFromFile("/usr/share/fonts/truetype/msttcorefonts/arial.ttf")){
        return -1;
    }
    startText.setFont(font);
    startText.setFillColor(sf::Color::White);
    startText.setString("START");
    startText.setCharacterSize(100.f);
    sf::FloatRect startTextBounds = startText.getLocalBounds();
    startText.setOrigin(startTextBounds.left + startTextBounds.width / 2.f, startTextBounds.top + startTextBounds.height / 2.f);
    startText.setPosition(startButton.getPosition());

    bool showScore = false;

    float finalTime;
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
                if (event.type == sf::Event::MouseButtonPressed && startButton.getGlobalBounds().contains(mousePosF) && 
                    event.mouseButton.button == sf::Mouse::Button::Left){
                    finalTime = displayMaze(window);
                    showScore = true;
                }
            }
            else if(currentState == MAZE){

            }
            else if(currentState == END_SCREEN){

            }

        }
        // Check if mouse is hovering over start button
        if(startButton.getGlobalBounds().contains(mousePosF)) startButton.setFillColor({200, 200, 200, 50});
        else startButton.setFillColor({255, 255, 255, 50});
        window.draw(menuBackground);
        window.draw(startButton);
        window.draw(startText);
        if(showScore){
            std::cout << finalTime << std::endl;
            window.close();
        }
        window.display();
    }
    return 0;
}