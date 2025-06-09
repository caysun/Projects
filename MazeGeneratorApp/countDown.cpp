#include<SFML/Graphics.hpp>
#include<cmath>
#include<sstream>

void displayCountDown(sf::RenderWindow &window){

    // Create Window and define dimensions
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    const int screenWidth = desktop.width;
    const int screenHeight= desktop.height;

    sf::Texture textureBackground;
    if(!textureBackground.loadFromFile("assets/menuCarRain.jpg")){
        return;
    }
    sf::Sprite menuBackground(textureBackground);
    menuBackground.setPosition(0.f, 0.f);
    menuBackground.setScale((float)(window.getSize().x/(float)(textureBackground.getSize().x)), 
    (float)(window.getSize().y/(float)(textureBackground.getSize().y)));

    sf::Text counter;
    sf::Font font;
    if(!font.loadFromFile("/usr/share/fonts/truetype/msttcorefonts/arial.ttf")){
        return;
    }
    counter.setFont(font);
    counter.setFillColor(sf::Color::White);
    counter.setCharacterSize(200);
    int numberDisplay = 3;

    sf::Event event;
    sf::Clock clock;
    int lastWholeSecond = 0;
    while(window.isOpen())
    {
        while(window.pollEvent(event))
        {
            // Close window event
            if (event.type == sf::Event::Closed) window.close();

            // Close if the escape key is pressed
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) window.close();

        }
        if(numberDisplay == 0) return;
        else if(std::floor(clock.getElapsedTime().asSeconds()) != lastWholeSecond){
            numberDisplay--;
            lastWholeSecond = std::floor(clock.getElapsedTime().asSeconds());
        }

        std::ostringstream ss;
        ss << numberDisplay;
        counter.setString(ss.str());
        sf::FloatRect counterBounds = counter.getLocalBounds();
        counter.setOrigin(counterBounds.width/2.f, counterBounds.height/2.f);
        counter.setPosition(screenWidth/2.f, screenHeight/2.f);

        window.draw(menuBackground);
        window.draw(counter);
        window.display();
    }
}