#include <SFML/Graphics.hpp>
#include <iostream>
#include <random>

const int mazeSize = 30;
const float wallThickness = 10.f;

struct rectangles{
    sf::RectangleShape horizontalLine;
    sf::RectangleShape verticalLine;
    sf::RectangleShape rect;
    int row;
    int col;
    bool hideVert;
    bool hideHori;
};

void dfs(std::pair<int, int> curCell, std::pair<int, int> prevCell, 
    std::vector<std::pair<rectangles, int>> &pathCells, std::vector<std::vector<bool>> &visited, 
    std::vector<std::vector<rectangles>> &maze){

    if(prevCell.first != -1){
        rectangles curRect = maze[curCell.first][curCell.second];
        rectangles prevRect = maze[prevCell.first][prevCell.second];
        // 0 to hide vertical line, 1 to hide horizontal line, and 2 to hide no lines
        if(prevCell.second < curCell.second) pathCells.emplace_back(curRect, 0), pathCells.emplace_back(prevRect, 2);
        else if(prevCell.first < curCell.first) pathCells.emplace_back(curRect, 1), pathCells.emplace_back(prevRect, 2);
        else if(prevCell.second > curCell.second) pathCells.emplace_back(prevRect, 0), pathCells.emplace_back(curRect, 2);
        else if(prevCell.first > curCell.first) pathCells.emplace_back(prevRect, 1), pathCells.emplace_back(curRect, 2);
    }
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
        if(nr < 0 || nr >= mazeSize || nc < 0 || nc >= mazeSize || visited[nr][nc]) continue;
        dfs({nr, nc}, {curCell.first, curCell.second}, pathCells, visited, maze);
    }
}

std::tuple<std::vector<rectangles>, int, std::pair<int, int>, std::pair<int, int>> mazeLayout(
    std::vector<std::pair<int, int>> &gridCellsN, std::vector<std::pair<int, int>> &gridCellsE,
    std::vector<std::pair<int, int>> &gridCellsS, std::vector<std::pair<int, int>> &gridCellsW,
    std::vector<std::vector<rectangles>> &maze){
    // Seed Randomly
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, 3);
    int wallSelection = dist(gen);
    std::vector<std::pair<rectangles, bool>> pathSolution;

    // Define Maze Solution with Inacessible Grid Blocks using depth-first-search (DFS)
    std::vector<std::vector<bool>> visited(mazeSize, std::vector<bool>(mazeSize, false));

    // Select random start and  end cell
    std::uniform_int_distribution<> dist2(0, mazeSize);
    int startCell = dist2(gen);
    int endCell = dist2(gen);

    std::pair<int, int> startCellPair;
    std::pair<int, int> endCellPair;

    // Use recursive backtracking
    std::vector<std::pair<rectangles, int>> pathCells;

    if(wallSelection == 0) dfs({0, startCell}, {-1, -1}, pathCells, visited, maze), startCellPair = {0, startCell}, endCellPair = {mazeSize-1, endCell};
    else if(wallSelection == 1) dfs({0, startCell}, {-1, -1}, pathCells, visited, maze), startCellPair = {startCell, mazeSize-1}, endCellPair = {endCell, 0};
    else if(wallSelection == 2) dfs({mazeSize-1, startCell}, {-1, -1}, pathCells, visited, maze), startCellPair = {mazeSize-1, startCell}, endCellPair = {0, endCell};
    else if(wallSelection == 3) dfs({0, startCell}, {-1, -1}, pathCells, visited, maze), startCellPair = {startCell, 0}, endCellPair = {endCell, mazeSize-1};

    // Convert from dfs pathCells to readable form
    int wallSize = 0;
    
    std::vector<std::vector<bool>> inPath(mazeSize, std::vector<bool>(mazeSize, false));
    std::vector<rectangles> grid;
    for(int i=0; i<pathCells.size(); i++){
        if(pathCells[i].second == 0) pathCells[i].first.hideVert = true;
        else if(pathCells[i].second == 1) pathCells[i].first.hideHori = true;
        grid.push_back(pathCells[i].first); 
        inPath[pathCells[i].first.row][pathCells[i].first.col] = true;
    }
    for(int i=0; i<maze.size(); i++){
        for(int j=0; j<maze[i].size(); j++){
            if(inPath[i][j]){
                wallSize++;
                continue;
            }
            else grid.push_back(maze[i][j]);
        }
    }
    
    return {grid, wallSize, startCellPair, endCellPair};
}

int main(){

    // Create Window and define dimensions
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    const int screenWidth = desktop.width;
    const int screenHeight= desktop.height;
    sf::RenderWindow window(desktop, "Maze", sf::Style::Fullscreen);
    // sf::RenderWindow window(sf::VideoMode(1200, 800), "Maze", sf::Style::Default);

    // Initialize maze and define grid rectangles in maze
    std::vector<std::pair<int, int>> gridCellsN;
    std::vector<std::pair<int, int>> gridCellsE;
    std::vector<std::pair<int, int>> gridCellsS;
    std::vector<std::pair<int, int>> gridCellsW;
    
    std::vector<sf::RectangleShape> gridBoundaries;
    std::vector<std::pair<int, int>> gridBoundPos;

    const float gridWidth = 1500.f/mazeSize;
    const float gridHeight = 900.f/mazeSize;
    const float gridStartPosX = screenWidth / 2.f - mazeSize/2*gridWidth;
    const float gridStartPosY = screenHeight / 2.f - mazeSize/2*gridHeight;
    std::vector<std::vector<rectangles>> maze(mazeSize);
    for(int r=0; r<mazeSize; r++){
        for(int c=0; c<mazeSize; c++){

            // Set up rectangle and border lines
            sf::RectangleShape rect({gridWidth, gridHeight});
            rect.setPosition(gridStartPosX + gridWidth*c, gridStartPosY + gridHeight*r);
            rect.setFillColor(sf::Color::Black);
            
            sf::RectangleShape horizontalLine({gridWidth, wallThickness});
            horizontalLine.setPosition(rect.getPosition().x, rect.getPosition().y);
            horizontalLine.setFillColor(sf::Color::White);
            
            sf::RectangleShape verticalLine({wallThickness, gridHeight});
            verticalLine.setPosition(rect.getPosition().x, rect.getPosition().y);
            verticalLine.setFillColor(sf::Color::White);

            rectangles currentRectangle = {horizontalLine, verticalLine, rect, r, c, false};

            maze[r].push_back(currentRectangle);

            if(r==0){
                gridCellsN.push_back({r, c});
                sf::RectangleShape gridLine({gridWidth, wallThickness});
                gridLine.setPosition(rect.getPosition().x, rect.getPosition().y);
                gridLine.setFillColor(sf::Color::Green);
                gridBoundaries.push_back(gridLine);
                gridBoundPos.emplace_back(r, c);
            }
            if(c==mazeSize-1){
                gridCellsE.push_back({r, c});
                sf::RectangleShape gridLine({wallThickness, gridHeight});
                gridLine.setPosition(rect.getPosition().x + gridWidth, rect.getPosition().y);
                gridLine.setFillColor(sf::Color::Green);
                gridBoundaries.push_back(gridLine);
                gridBoundPos.emplace_back(r, c);
            }
            if(r==mazeSize-1){
                gridCellsS.push_back({r, c});
                sf::RectangleShape gridLine({gridWidth, wallThickness});
                if(c==mazeSize-1) gridLine.setSize({gridWidth + wallThickness, wallThickness});
                gridLine.setPosition(rect.getPosition().x, rect.getPosition().y + gridHeight);
                gridLine.setFillColor(sf::Color::Green);
                gridBoundaries.push_back(gridLine);
                gridBoundPos.emplace_back(r, c);
            }
            if(c==0){
                gridCellsW.push_back({r, c});
                sf::RectangleShape gridLine({wallThickness, gridHeight});
                gridLine.setPosition(rect.getPosition().x, rect.getPosition().y);
                gridLine.setFillColor(sf::Color::Green);
                gridBoundaries.push_back(gridLine);
                gridBoundPos.emplace_back(r, c);
            }
        }
    }
    
    auto [grid, wallSize, startCell, endCell] = mazeLayout(gridCellsN, gridCellsE, gridCellsS, gridCellsW, maze);

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
                std::tie(grid, wallSize, startCell, endCell) = mazeLayout(gridCellsN, gridCellsE, gridCellsS, gridCellsW, maze);
            }
        }

        // Clear the window with solid color
        window.clear(sf::Color::Blue);

        
        // Draw grid walls
        for(int i=0; i<wallSize; i++){
            window.draw(grid[i].rect);
            if(!grid[i].hideVert) window.draw(grid[i].verticalLine);
            if(!grid[i].hideHori) window.draw(grid[i].horizontalLine);
        }
        for(int i=wallSize; i<grid.size(); i++){
            window.draw(grid[i].rect);
            // window.draw(grid[i].horizontalLine);
            // window.draw(grid[i].verticalLine);
        }
        
        // Draw grid boundary lines
        for(int i=0; i<gridBoundaries.size(); i++){

            // Skip if start or end cell
            if( (gridBoundPos[i] == startCell || gridBoundPos[i] == endCell) && ( (startCell.first == mazeSize-1 && endCell.first == 0) || 
            (startCell.first == 0 && endCell.first == mazeSize-1)) && 
            gridBoundaries[i].getSize().y == wallThickness) gridBoundaries[i].setFillColor(sf::Color::Red);
            else if( (gridBoundPos[i] == startCell || gridBoundPos[i] == endCell) && 
            ( (startCell.second == mazeSize-1 && endCell.second == 0) || (startCell.second == 0 && endCell.second == mazeSize-1)) &&
            gridBoundaries[i].getSize().x == wallThickness) gridBoundaries[i].setFillColor(sf::Color::Red);

            window.draw(gridBoundaries[i]);
            gridBoundaries[i].setFillColor(sf::Color::Green);
        }
        
        // Display what was drawn
        window.display();
    }
}