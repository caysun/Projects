#include <SFML/Graphics.hpp>
#include <iostream>
#include <random>

const int mazeSize = 30;
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

std::tuple<std::vector<std::vector<bool>>, std::pair<int, int>, std::pair<int, int>> mazeLayout(
    std::vector<std::vector<sf::RectangleShape>> &maze){

    // Seed Randomly start cell, wall, and end cell
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, 3);
    int wallSelectionStart = dist(gen);
    std::pair<int, int> startCell;
    std::pair<int, int> endCell;
    std::vector<std::vector<bool>> pathCells(2*mazeSize+1, std::vector<bool>(2*mazeSize+1, false));

    if(wallSelectionStart == 0){
        std::uniform_int_distribution<> dist2(1, mazeSize);
        int randomPos = dist2(gen);
        startCell = {1, 1};
        pathCells[0][1] = true;

        int randomPos2 = dist2(gen);
        endCell = {2*mazeSize-1, 2*mazeSize-1};
        pathCells[2*mazeSize][2*mazeSize-1] = true;
    }
    else if(wallSelectionStart == 1){
        std::uniform_int_distribution<> dist2(1, mazeSize);
        int randomPos = dist2(gen);
        startCell = {1, 1};
        pathCells[0][1] = true;

        int randomPos2 = dist2(gen);
        endCell = {2*mazeSize-1, 2*mazeSize-1};
        pathCells[2*mazeSize][2*mazeSize-1] = true;
    }
    else if(wallSelectionStart == 2){
        std::uniform_int_distribution<> dist2(1, mazeSize);
        int randomPos = dist2(gen);
        startCell = {1, 1};
        pathCells[0][1] = true;
        
        int randomPos2 = dist2(gen);
        endCell = {2*mazeSize-1, 2*mazeSize-1};
        pathCells[2*mazeSize][2*mazeSize-1] = true;
    }
    else if(wallSelectionStart == 3){
        std::uniform_int_distribution<> dist2(1, mazeSize);
        int randomPos = dist2(gen);
        startCell = {1, 1};
        pathCells[0][1] = true;
        
        int randomPos2 = dist2(gen);
        endCell = {2*mazeSize-1, 2*mazeSize-1};
        pathCells[2*mazeSize][2*mazeSize-1] = true;
    }
    
    // Define Maze Solution with Inacessible Grid Blocks using depth-first-search (DFS)
    std::vector<std::vector<bool>> visited(2*mazeSize+1, std::vector<bool>(2*mazeSize+1, false));

    // Use recursive backtracking
    dfs(startCell, {-1, -1}, pathCells, visited, maze);
    return {pathCells, startCell, endCell};
}

void addExtraWalls(std::vector<std::vector<bool>> &grid, std::vector<std::vector<sf::RectangleShape>> &maze, float scale){
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, 1);
    std::uniform_int_distribution<> dist2(1, mazeSize-1);
    std::uniform_int_distribution<> dist3(1, 2*mazeSize-1);
    std::uniform_int_distribution<> dist4(1, mazeSize);
    for(int i=0; i<mazeSize*mazeSize/scale; i++){
        int type = dist(gen), randomRow, randomCol;
        if(type == 0){
            randomRow = dist3(gen);
            randomCol = 2*dist2(gen);
        }
        else{
            randomRow = 2*dist2(gen);
            randomCol = 2*dist4(gen)-1;
        }
        grid[randomRow][randomCol] = true;
    }
}

std::vector<sf::Vector2f> getCorners(const sf::RectangleShape &rect) {
    sf::Transform t = rect.getTransform();
    sf::FloatRect local = rect.getLocalBounds();

    std::vector<sf::Vector2f> corners(4);
    corners[0] = t.transformPoint(local.left, local.top);                               // top-left
    corners[1] = t.transformPoint(local.left + local.width, local.top);                 // top-right
    corners[2] = t.transformPoint(local.left + local.width, local.top + local.height);  // bottom-right
    corners[3] = t.transformPoint(local.left, local.top + local.height);                // bottom-left

    return corners;
}

std::vector<sf::Vector2f> getAxes(std::vector<sf::Vector2f> &corners){
    std::vector<sf::Vector2f> axes;
    for(int i=0; i<2; i++){
        sf::Vector2f corner1 = corners[i];
        sf::Vector2f corner2 = corners[i+1];
        
        sf::Vector2f edge = corner2 - corner1;
        sf::Vector2f normal(-edge.y, edge.x);
        float length = sqrt(normal.x * normal.x + normal.y * normal.y);
        if(length != 0.0f){
            normal /= length;
        }
        axes.push_back(normal);
    }
    return axes;
}

void mapPointsOntoAxis(sf::Vector2f &axis, std::vector<sf::Vector2f> &corners, float &min, float &max){
    min = max = corners[0].x * axis.x + corners[0].y * axis.y; // Dot product to map points onto normal vectors
    for(int i=1; i<corners.size(); i++){
        float curProj = corners[i].x * axis.x + corners[i].y * axis.y; // Project next point onto axis
        if(curProj < min) min = curProj;
        if(curProj > max) max = curProj;
    }
}

bool wallCollision(sf::RectangleShape &car, sf::RectangleShape &wall){
    std::vector<sf::Vector2f> carCorners = getCorners(car);
    std::vector<sf::Vector2f> wallCorners = getCorners(wall);
    std::vector<sf::Vector2f> carAxes = getAxes(carCorners);
    std::vector<sf::Vector2f> wallAxes = getAxes(wallCorners);
    
    for(int i=0; i<carAxes.size(); i++){
        float carMin, carMax, wallMin, wallMax;
        mapPointsOntoAxis(carAxes[i], carCorners, carMin, carMax);
        mapPointsOntoAxis(carAxes[i], wallCorners, wallMin, wallMax);
        if(carMax < wallMin || wallMax < carMin) return false;
    }
    
    for(int i=0; i<wallAxes.size(); i++){
        float carMin, carMax, wallMin, wallMax;
        mapPointsOntoAxis(wallAxes[i], carCorners, carMin, carMax);
        mapPointsOntoAxis(wallAxes[i], wallCorners, wallMin, wallMax);
        if(carMax < wallMin || wallMax < carMin) return false;
    }

    return true;
}


int main(){

    // Create Window and define dimensions
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    const int screenWidth = desktop.width;
    const int screenHeight= desktop.height;
    sf::RenderWindow window(desktop, "Maze", sf::Style::Fullscreen);
    // sf::RenderWindow window(sf::VideoMode(1200, 800), "Maze", sf::Style::Default);
    
    // Load Background
    sf::Texture textureBackground;
    if(!textureBackground.loadFromFile("assets/gameBackgroundHard.jpeg")){
        return -1;
    }
    sf::Sprite gameBackground(textureBackground);
    gameBackground.setPosition(0.f, 0.f);
    gameBackground.setScale((float)(window.getSize().x/(float)(textureBackground.getSize().x)), 
    (float)(window.getSize().y/(float)(textureBackground.getSize().y)));

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
    
    auto [grid, startCell, endCell] = mazeLayout(maze);
    addExtraWalls(grid, maze, 10.f);

    sf::Texture texture;
    if (!texture.loadFromFile("assets/sprite.png")) {
        // Handle the error
        return -1;
    }
    
    sf::Texture finishTexture;
    if(!finishTexture.loadFromFile("assets/finishLine.png")){
        return -1;
    }

    sf::Sprite finishLine(finishTexture);
    finishLine.setScale(2.5f/mazeSize, 2.5f/mazeSize);
    sf::FloatRect boundsFinish = finishLine.getLocalBounds();
    finishLine.setOrigin({boundsFinish.width/2.f, boundsFinish.height/2.f});

    sf::Sprite player;
    float carPosX = maze[startCell.first][startCell.second].getPosition().x + gridWidth/2.f;
    float carPosY = maze[startCell.first][startCell.second].getPosition().y;
    float carAngle = 0.0;
    float carSpeed = 100.f;
    if(mazeSize == 10) carSpeed = 250.f;
    if(mazeSize == 20) carSpeed = 150.f;
    float carSizeScale = 0.225f * 10.f/mazeSize;
    player.setScale({carSizeScale, carSizeScale});
    player.setTexture(texture);
    sf::FloatRect bounds = player.getLocalBounds();
    player.setOrigin(bounds.width / 2.f, bounds.height / 2.f);

    if(startCell.first == 1 && endCell.first == 2*mazeSize-1){ // North
        player.setRotation(90.f);

        finishLine.setRotation(180.f);
        float finishPosX = maze[endCell.first][endCell.second].getPosition().x + gridWidth/2.f;
        float finishPosY = maze[endCell.first][endCell.second].getPosition().y + gridHeight + 10.f;
        finishLine.setPosition(finishPosX, finishPosY);
    }
    else if(startCell.first == 2*mazeSize-1 && endCell.first == 1){ // South
        player.setRotation(-90.f);
        carPosX = maze[startCell.first][startCell.second].getPosition().x + gridWidth/2.f;
        carPosY = maze[startCell.first][startCell.second].getPosition().y + gridHeight;

        finishLine.setRotation(0.f);
        float finishPosX = maze[endCell.first][endCell.second].getPosition().x + gridWidth/2.f;
        float finishPosY = maze[endCell.first][endCell.second].getPosition().y - 10.f;
        finishLine.setPosition(finishPosX, finishPosY);
    }
    else if(startCell.second == 2*mazeSize-1 && endCell.second == 1){ // East
        player.setRotation(180.f);
        carPosX = maze[startCell.first][startCell.second].getPosition().x + gridWidth;
        carPosY = maze[startCell.first][startCell.second].getPosition().y + gridHeight/2.f;

        finishLine.setRotation(-90.f);
        float finishPosX = maze[endCell.first][endCell.second].getPosition().x - 10.f;
        float finishPosY = maze[endCell.first][endCell.second].getPosition().y + gridHeight/2.f;
        finishLine.setPosition(finishPosX, finishPosY);
    }
    else{ // West
        carPosX = maze[startCell.first][startCell.second].getPosition().x;
        carPosY = maze[startCell.first][startCell.second].getPosition().y + gridHeight/2.f;

        finishLine.setRotation(90.f);
        float finishPosX = maze[endCell.first][endCell.second].getPosition().x + gridWidth + 10.f;
        float finishPosY = maze[endCell.first][endCell.second].getPosition().y + gridHeight/2.f;
        finishLine.setPosition(finishPosX, finishPosY);
    }

    player.setPosition(carPosX, carPosY);

    sf::RectangleShape carBoundingBox;
    std::vector<sf::RectangleShape> wallBoundingBoxes;

    sf::Event event;
    sf::Clock clock;
    // Run the program as long as the window is open
    while (window.isOpen())
    {
        float deltaTime = clock.restart().asSeconds();
        // Process Events
        while(window.pollEvent(event))
        {
            // Close window event
            if (event.type == sf::Event::Closed) window.close();

            // Close if the escape key is pressed
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) window.close();
            
            // Generate new maze layout if the mouse is clicked
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left){
                // tie(grid, startCell, endCell) = mazeLayout(maze);
            }
        }
        
        float dirX = 0.0f;
        float dirY = 0.0f;
        
        // Move car through maze with arrow keys
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)){
            dirY -= 1.f;
        } // Up
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)){
            dirY += 1.f;
        } // Down
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)){
            dirX += 1.f;
        } // Right
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)){
            dirX -= 1.f;
        } // Left
        
        if(dirX != 0.0f || dirY != 0.0f){
            float length = std::sqrt(dirX*dirX + dirY*dirY);
            dirX /= length;
            dirY /= length;
            float carProjPosX = carPosX + dirX * carSpeed * deltaTime;
            float carProjPosY = carPosY + dirY * carSpeed * deltaTime;
            
            int mazeColumn = 2*(static_cast<int>((carProjPosX-gridStartPosX)/(gridWidth))) + 1;
            int mazeRow = 2*(static_cast<int>((carProjPosY-gridStartPosY)/(gridHeight))) + 1;
            if(player.getPosition().y >= maze[endCell.first][endCell.second].getPosition().y + gridHeight){
                std::cout << "Nice job man!" << std::endl;
                window.close();
            }
            
            bool intersectsWall = false;
            player.setPosition(carProjPosX, carProjPosY);
            float orignialCarAngle = player.getRotation();
            player.setRotation((std::atan2(dirY, dirX) * 180.f / M_PI));

            // Make Player Bonding Box
            sf::FloatRect carBounds = player.getLocalBounds();
            sf::Vector2f carSize(carBounds.width, carBounds.height);
            sf::RectangleShape playerRect(carSize);
            playerRect.setPosition(player.getPosition());
            playerRect.setRotation(player.getRotation());
            playerRect.setOrigin(player.getOrigin());
            playerRect.setScale(player.getScale());
            playerRect.setOutlineThickness(5.f);
            playerRect.setOutlineColor(sf::Color::Green);
            playerRect.setFillColor(sf::Color::Transparent);

            for(int dr = -1; dr <= 1; dr++){
                if(intersectsWall) break;
                for(int dc = -1; dc <= 1; dc++){
                    if(dr == 0 && dc == 0) continue;
                    if(mazeRow + dr < 0 || mazeRow + dr > 2*mazeSize || mazeColumn + dc < 0 || mazeColumn + dc > 2*mazeSize) continue;
                    if(!grid[mazeRow+dr][mazeColumn+dc]){

                        // Make Wall Bounding Box
                        sf::RectangleShape wallIntersection = maze[mazeRow+dr][mazeColumn+dc];
                        wallIntersection.setFillColor(sf::Color::Transparent);
                        wallIntersection.setOutlineColor({0, 255, 255, 50});
                        wallIntersection.setOutlineThickness(5.f);
                        wallIntersection.setFillColor({128, 0, 128, 50});
                        wallBoundingBoxes.push_back(wallIntersection);


                        carBoundingBox = playerRect;
                        if(wallCollision(playerRect, maze[mazeRow+dr][mazeColumn+dc])){
                            intersectsWall = true;
                            player.setPosition(carPosX, carPosY);
                            player.setRotation(orignialCarAngle);
                            break;
                        }
                    }
                }
            }
            // Check For Special Case outside walls
            std::vector<int> extraDr = {-2, -2, -1, -1, 1, 1, 2, 2};
            std::vector<int> extraDc = {-1, 1, -2, -2, -2, 2, -1, 1};
            for(int i=0; i<8; i++){
                int r = mazeRow + extraDr[i];
                int c = mazeColumn + extraDc[i];
                if(r < 0 || r > 2*mazeSize || c < 0 || c > 2*mazeSize) continue;
                if(!grid[r][c]){
                    if(wallCollision(playerRect, maze[r][c])){
                        intersectsWall = true;
                        player.setPosition(carPosX, carPosY);
                        player.setRotation(orignialCarAngle);
                        break;
                    }
                }
            }
            if(!intersectsWall && carProjPosY >= gridStartPosY){
                player.setRotation((std::atan2(dirY, dirX) * 180.f / M_PI));
                carPosX = carProjPosX;
                carPosY = carProjPosY;
                player.setPosition(carPosX, carPosY);
            }
        }
        
        // Draw background image and finish line
        window.draw(gameBackground);
        window.draw(finishLine);

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
        window.draw(player);
        // window.draw(carBoundingBox);
        for(auto &w : wallBoundingBoxes) window.draw(w);
        // Display what was drawn
        window.display();
    }
}