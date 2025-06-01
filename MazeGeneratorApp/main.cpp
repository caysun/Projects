#include <SFML/Graphics.hpp>
#include <iostream>
#include <random>

const int mazeSize = 100;
const float wallThickness = 100.f/mazeSize;

void dfs(std::pair<int, int> curCell, std::pair<int, int> prevCell, 
    std::vector<std::vector<bool>> &pathCells, std::vector<std::vector<bool>> &visited, 
    std::vector<std::vector<sf::RectangleShape>> &maze){

    if(prevCell.first != -1){
        sf::RectangleShape curRect = maze[curCell.first][curCell.second];
        sf::RectangleShape prevRect = maze[prevCell.first][prevCell.second];

        // Determine the wall that was crossed
        if(curCell.first > prevCell.first) pathCells[curCell.first-1][curCell.second] = true;
        else if(curCell.first < prevCell.first) pathCells[curCell.first+1][curCell.second] = true;
        else if(curCell.second > prevCell.second) pathCells[curCell.first][curCell.second-1] = true;
        else if(curCell.second < prevCell.second) pathCells[curCell.first][curCell.second+1] = true;
    }
    visited[curCell.first][curCell.second] = true;

    std::vector<int> dr = {0, -2, 0, 2};
    std::vector<int> dc = {-2, 0, 2, 0};
    
    // Go in a random direction
    std::vector<int> directions = {0, 1, 2, 3};

    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::shuffle(directions.begin(), directions.end(), gen);

    for(int i=0; i<4; i++){
        int nr = curCell.first + dr[directions[i]];
        int nc = curCell.second + dc[directions[i]];
        if(nr < 0 || nr >= 2*mazeSize+1 || nc < 0 || nc >= 2*mazeSize+1 || visited[nr][nc]) continue;
        dfs({nr, nc}, {curCell.first, curCell.second}, pathCells, visited, maze);
    }
}

std::vector<std::vector<bool>> mazeLayout(std::vector<std::vector<sf::RectangleShape>> &maze){

    // Seed Randomly start cell, wall, and end cell
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, 3);
    int wallSelectionStart = dist(gen);
    std::pair<int, int> startCell;
    std::vector<std::vector<bool>> pathCells(2*mazeSize+1, std::vector<bool>(2*mazeSize+1, false));

    if(wallSelectionStart == 0){
        std::uniform_int_distribution<> dist2(1, mazeSize);
        int randomPos = dist2(gen);
        startCell = {1, 2*randomPos-1};
        pathCells[0][2*randomPos-1] = true;

        int randomPos2 = dist2(gen);
        pathCells[2*mazeSize][2*randomPos2-1] = true;
    }
    else if(wallSelectionStart == 1){
        std::uniform_int_distribution<> dist2(1, mazeSize);
        int randomPos = dist2(gen);
        startCell = {2*randomPos-1, 2*mazeSize-1};
        pathCells[2*randomPos-1][2*mazeSize] = true;

        int randomPos2 = dist2(gen);
        pathCells[2*randomPos2-1][0] = true;
    }
    else if(wallSelectionStart == 2){
        std::uniform_int_distribution<> dist2(1, mazeSize);
        int randomPos = dist2(gen);
        startCell = {2*mazeSize-1, 2*randomPos-1};
        pathCells[2*mazeSize][2*randomPos-1] = true;
        
        int randomPos2 = dist2(gen);
        pathCells[0][2*randomPos2-1] = true;
    }
    else if(wallSelectionStart == 3){
        std::uniform_int_distribution<> dist2(1, mazeSize);
        int randomPos = dist2(gen);
        startCell = {2*randomPos-1, 1};
        pathCells[2*randomPos-1][0] = true;
        
        int randomPos2 = dist2(gen);
        pathCells[2*randomPos2-1][2*mazeSize] = true;
    }
    
    // Define Maze Solution with Inacessible Grid Blocks using depth-first-search (DFS)
    std::vector<std::vector<bool>> visited(2*mazeSize+1, std::vector<bool>(2*mazeSize+1, false));

    // Use recursive backtracking
    dfs(startCell, {-1, -1}, pathCells, visited, maze);
    return pathCells;
}

int main(){

    // Create Window and define dimensions
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    const int screenWidth = desktop.width;
    const int screenHeight= desktop.height;
    sf::RenderWindow window(desktop, "Maze", sf::Style::Fullscreen);
    // sf::RenderWindow window(sf::VideoMode(1200, 800), "Maze", sf::Style::Default);
    
    std::vector<sf::RectangleShape> gridBoundaries;
    std::vector<std::pair<int, int>> gridBoundPos;

    const float gridWidth = 1500.f/mazeSize;
    const float gridHeight = 900.f/mazeSize;
    const float gridStartPosX = screenWidth / 2.f - mazeSize/2.f*gridWidth;
    const float gridStartPosY = screenHeight / 2.f - mazeSize/2.f*gridHeight;
    std::vector<std::vector<sf::RectangleShape>> maze(2*mazeSize+1, std::vector<sf::RectangleShape>(2*mazeSize+1));
    for(int r=0; r<2*mazeSize+1; r++){
        for(int c=0; c<2*mazeSize+1; c++){
            // Generate horizontal walls
            if(r%2==0){
                if(c == 2*mazeSize) break;
                sf::RectangleShape horizontalLine({gridWidth + wallThickness, wallThickness});
                horizontalLine.setPosition({gridStartPosX + gridWidth*c/2.f, gridStartPosY + gridHeight*r/2.f});
                horizontalLine.setFillColor(sf::Color::White);
                c++;
                maze[r][c] = horizontalLine;
                continue;
            }
            // Generate vertical walls
            else if(c%2 == 0){
                sf::RectangleShape verticalLine({wallThickness, gridHeight + wallThickness});
                verticalLine.setPosition({gridStartPosX + gridWidth*c/2.f, gridStartPosY + gridHeight*(r-1)/2.f});
                if(r == 2*mazeSize-1) verticalLine.setSize({wallThickness, gridHeight});
                verticalLine.setFillColor(sf::Color::White);
                maze[r][c] = verticalLine;
            }
            // Generate grid cells
            else {
                sf::RectangleShape rect({gridWidth + wallThickness, gridHeight + wallThickness});
                rect.setPosition({gridStartPosX + gridWidth*(c-1)/2.f, gridStartPosY + gridHeight*(r-1)/2.f});
                rect.setFillColor(sf::Color::Black);
                maze[r][c] = rect;
            }
            
        }
    }
    
    std::vector<std::vector<bool>> grid = mazeLayout(maze);

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

            // Generate new maze layout if the mouse is clicked
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left){
                grid = mazeLayout(maze);
            }
        }

        // Clear the window with solid color
        window.clear(sf::Color::Blue);

        // Draw background cells
        for(int r=0; r<maze.size(); r++){
            for(int c=0; c<maze[r].size(); c++){
                if(r%2 == 0 && c&2 == 1) continue;
                else if(r%2 == 1 && c%2 == 0) continue;
                if(!grid[r][c]) window.draw(maze[r][c]);
            }
        }

        // Draw layout with carved walls
        for(int r=0; r<maze.size(); r++){
            for(int c=0; c<maze[r].size(); c++){
                if(r%2 == 0 && c&2 == 0) continue;
                else if(r%2 == 1 && c%2 == 1) continue;
                if(!grid[r][c]){
                    if(c%2 == 0 && r == 2*mazeSize-1) maze[r][c].setSize({wallThickness, gridHeight + wallThickness});
                    window.draw(maze[r][c]);
                }
            }
        }
        
        // Display what was drawn
        window.display();
    }
}