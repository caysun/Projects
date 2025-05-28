#include <SFML/Graphics.hpp>
#include <iostream>
#include <random>

std::vector<std::pair<int, int>> dfs(std::pair<int, int> curCell, std::pair<int, int> endCell, 
    std::vector<std::pair<int, int>> &pathCells, std::vector<std::vector<bool>> &visited){
    
    if(curCell == endCell) return pathCells;
    pathCells.push_back(curCell);
    visited[curCell.first][curCell.second] = true;
    std::vector<int> dr = {0, -1, 0, 1};
    std::vector<int> dc = {-1, 0, 1, 0};
    for(int i=0; i<4; i++){
        int nr = curCell.first + dr[i];
        int nc = curCell.second + dc[i];
        std::vector<std::pair<int, int>> currentCells = dfs({nr, nc}, endCell, pathCells, visited);
        if(currentCells[currentCells.size()-1].first == 9 && currentCells[currentCells.size()-1].second == 9) return currentCells;
    }
    visited[curCell.first][curCell.second] = false;
    return pathCells;
}

int main(){

    // Create Window and define dimensions
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    const int screenWidth = desktop.width;
    const int screenHeight= desktop.height;
    sf::RenderWindow window(desktop, "Maze", sf::Style::Fullscreen);

    // Initialize maze and define grid rectangles in maze
    std::vector<std::pair<int, int>> gridCells(98);
    const float gridWidth = 150.f;
    const float gridHeight = 90.f;
    const float gridStartPosX = screenWidth / 2.f - 4.5*gridWidth;
    const float gridStartPosY = screenHeight / 2.f - 4.5*gridHeight;
    sf::RectangleShape maze[10][10];
    for(int r=0; r<10; r++){
        for(int c=0; c<10; c++){
            sf::RectangleShape rect({gridWidth, gridHeight});
            rect.setOrigin(rect.getSize().x / 2.f, rect.getSize().y / 2.f);
            rect.setPosition(gridStartPosX + gridWidth*c, gridStartPosY + gridHeight*r);
            rect.setFillColor(sf::Color::Black);
            rect.setOutlineColor(sf::Color::White);
            rect.setOutlineThickness(10.f);
            maze[r][c] = rect;
            if( (r != 0 && c != 0) && (r != 9 && c != 9) ) gridCells[r*10 + c - 1] = {r, c};
        }
    }

    // Define Maze Solution with Inacessible Grid Blocks using depth-first-search (DFS)
    std::vector<std::vector<bool>> visited(10, std::vector<bool>(10, false));

    std::random_device rd;
    std::mt19937 gen(rd());

    // Select random cells to be blocked
    std::shuffle(gridCells.begin(), gridCells.end(), gen);
    for(int i=0; i<8; i++) visited[gridCells[i].first][gridCells[i].second] = true;

    std::vector<std::pair<int, int>> emptyVector;
    std::vector<std::pair<int, int>> pathSolution = dfs({0, 0}, {9, 9}, emptyVector, visited);

    // Print out path solution
    int grid[10][10];
    for(auto &x : pathSolution){
        grid[x.first][x.second] = 1;
    }
    for(auto &x : grid){
        for(auto &y :x) std::cout << y;
        std::cout << std::endl;
    }

    sf::Event event;
    // Run the program as long as the window is open
    while (window.isOpen())
    {
        // Process Events
        while(window.pollEvent(event))
        {
            // Close window event
            if (event.type == sf::Event::Closed) window.close();

            // Close if the escape key is pressed
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) window.close();
        }

        // Clear the window with black color
        window.clear(sf::Color::Blue);
        
        // Draw grid rectangles
        for(auto &row : maze){
            for(auto &grid : row){
                window.draw(grid);
            }
        }

        // Display what was drawn
        window.display();
    }
}