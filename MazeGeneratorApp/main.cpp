#include <SFML/Graphics.hpp>
#include <iostream>
#include <random>

std::vector<std::pair<int, int>> dfs(std::pair<int, int> curCell, std::pair<int, int> endCell, 
    std::vector<std::pair<int, int>> &pathCells, std::vector<std::vector<bool>> &visited){
    
    if (curCell == endCell) {
        pathCells.push_back(curCell);
        return pathCells;
    }
    pathCells.push_back(curCell);
    visited[curCell.first][curCell.second] = true;
    std::vector<int> dr = {0, -1, 0, 1};
    std::vector<int> dc = {-1, 0, 1, 0};

    // Go in a random direction
    std::vector<int> directions = {0, 1, 2, 3};

    static std::random_device rd;
    static std::mt19937 gen(rd());

    std::shuffle(directions.begin(), directions.end(), gen);
    for(int i=0; i<4; i++){
        int nr = curCell.first + dr[directions[i]];
        int nc = curCell.second + dc[directions[i]];
        if(nr < 0 || nr > 9 || nc < 0 || nc > 9 || visited[nr][nc]) continue;
        std::vector<std::pair<int, int>> currentCells = dfs({nr, nc}, endCell, pathCells, visited);
        if(currentCells[currentCells.size()-1] == endCell) return currentCells;
    }
    visited[curCell.first][curCell.second] = false;
    pathCells.pop_back();
    return pathCells;
}

int main(){

    // Initialize constant blocked cells
    const int blocked = 15;

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
            maze[r][c] = rect;
            if( (r != 0 && c != 0) || (r != 9 && c != 9) ) gridCells[(r*10 + c - 1)] = std::make_pair(r, c);
        }
    }

    
    // Seed Randomly
    std::random_device rd;
    std::mt19937 gen(rd());

    std::vector<std::pair<int, int>> pathSolution;
    do{
        // Define Maze Solution with Inacessible Grid Blocks using depth-first-search (DFS)
        std::vector<std::vector<bool>> visited(10, std::vector<bool>(10, false));

        // Select random cells to be blocked
        std::shuffle(gridCells.begin(), gridCells.end(), gen);
        for(int i=0; i<blocked; i++) visited[gridCells[i].first][gridCells[i].second] = true;

        // Attempt to find path solution
        std::vector<std::pair<int, int>> emptyVector;
        pathSolution = dfs({0, 0}, {9, 9}, emptyVector, visited);
        
    } while(pathSolution.back() != std::make_pair(9, 9));

    // Print out path solution
    std::vector<std::vector<int>> grid(10, std::vector<int>(10, 0));
    for(auto &x : pathSolution){
        grid[x.first][x.second] = 1;
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

        // Clear the window with solid color
        window.clear(sf::Color::Blue);

        // Color rectangles in Path Green
        for(int r=0; r<grid.size(); r++){
            for(int c=0; c<grid[r].size(); c++){
                if(grid[r][c] == 0){
                    maze[r][c].setFillColor(sf::Color::Black);
                    maze[r][c].setOutlineColor(sf::Color::White);
                    maze[r][c].setOutlineThickness(10.f);
                    window.draw(maze[r][c]);
                    continue;
                }
                maze[r][c].setFillColor({0, 100, 0});
                maze[r][c].setOutlineColor(sf::Color::White);
                maze[r][c].setOutlineThickness(10.f);
                window.draw(maze[r][c]);
            }
        }

        // Display what was drawn
        window.display();
    }
}