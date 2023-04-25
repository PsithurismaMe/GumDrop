#include "headers/blocks.hpp"
#include <algorithm>

namespace platformer
{
    class editorBlock : public stationaryStaticBlock
    {
    protected:
        Vector2 beginOfRay;
        Vector2 endOfRay;
        float rayLength;

    public:
        float getRayLength()
        {
            return rayLength;
        }
        Vector2 getBeginOfRay()
        {
            return beginOfRay;
        }
        Vector2 getEndOfRay()
        {
            return endOfRay;
        }
        editorBlock()
        {
        }
        editorBlock(editorBlock &whereToInherit, int globalx, int globaly, int wid, int hgt, int rot)
        {
            (*this) = whereToInherit;
            inGamePositionDimension.x = globalx;
            inGamePositionDimension.y = globaly;
            inGamePositionDimension.width = wid;
            inGamePositionDimension.height = hgt;
            rotation = rot;
        }
        void computeRay(std::vector<editorBlock> &obstecules)
        {
            Vector2 origion;
            int lowest{4096};
            int halfSpriteWidth = inGamePositionDimension.width / 2;
            Vector2 p1{inGamePositionDimension.x + halfSpriteWidth, inGamePositionDimension.y + halfSpriteWidth};
            Vector2 p2{inGamePositionDimension.x + lowest, inGamePositionDimension.y + halfSpriteWidth};
            origion.x = inGamePositionDimension.x + halfSpriteWidth;
            origion.y = inGamePositionDimension.y + halfSpriteWidth;
            p1 = platformer::rotatePointAroundOtherPoint(p1, origion, rotation);
            p2 = platformer::rotatePointAroundOtherPoint(p2, origion, rotation);
            beginOfRay = p1;
            endOfRay = p2;
            for (size_t i = 0; i < obstecules.size(); i++)
            {
                Vector2 cache = obstecules.at(i).getPosition();
                cache.x += halfSpriteWidth;
                cache.y += halfSpriteWidth;
                if (&obstecules.at(i) != this)
                {
                    if (CheckCollisionPointLine(cache, p1, p2, halfSpriteWidth + 1))
                    {
                        float placeholder = sqrt(pow((cache.x) - p1.x, 2) + pow((cache.y) - p1.y, 2));
                        if (placeholder < lowest)
                        {
                            lowest = placeholder;
                            endOfRay = cache;
                        }
                    }
                }
            }
            rayLength = lowest;
        }
    };
    namespace blocks
    {
        namespace editor
        {
            editorBlock grass;
            editorBlock dirt;
            editorBlock brick;
            editorBlock laser;
            editorBlock lava;
            editorBlock playerSpawn;
            editorBlock portal;
            editorBlock brickR;
            editorBlock brickO;
            editorBlock brickY;
            editorBlock brickG;
            editorBlock brickB;
            editorBlock brickP;
            editorBlock brickW;
            editorBlock accessPoint;
            editorBlock susJuice;
            editorBlock topSneakyNote;
            std::vector<editorBlock *> types;
            void init()
            {
                grass.setPositionOnSpriteSheet({0, 0, 64, 64});
                dirt.setPositionOnSpriteSheet({64, 0, 64, 64});
                brick.setPositionOnSpriteSheet({128, 0, 64, 64});
                laser.setPositionOnSpriteSheet({0, 1792, 64, 64});
                lava.setPositionOnSpriteSheet({0, 1856, 64, 64});
                playerSpawn.setPositionOnSpriteSheet({0, 1984, 64, 64});
                portal.setPositionOnSpriteSheet({0, 192, 64, 64});
                brickR.setPositionOnSpriteSheet({512, 0, 64, 64});
                brickO.setPositionOnSpriteSheet({448, 0, 64, 64});
                brickY.setPositionOnSpriteSheet({384, 0, 64, 64});
                brickG.setPositionOnSpriteSheet({320, 0, 64, 64});
                brickB.setPositionOnSpriteSheet({256, 0, 64, 64});
                brickP.setPositionOnSpriteSheet({256, 64, 64, 64});
                brickW.setPositionOnSpriteSheet({320, 64, 64, 64});
                accessPoint.setPositionOnSpriteSheet({576, 0, 64, 64});
                accessPoint.setDimentions(128, 128);
                susJuice.setPositionOnSpriteSheet({0, 1664, 64, 64});
                topSneakyNote.setPositionOnSpriteSheet({192, 0, 64, 64});
                grass.setType(valuesOfBlocks::Grass);
                dirt.setType(valuesOfBlocks::Dirt);
                brick.setType(valuesOfBlocks::Brick);
                laser.setType(valuesOfBlocks::LaserNoTimeOffset);
                lava.setType(valuesOfBlocks::Lava);
                portal.setType(valuesOfBlocks::Portal);
                brickR.setType(valuesOfBlocks::BrickR);
                brickO.setType(valuesOfBlocks::BrickO);
                brickY.setType(valuesOfBlocks::BrickY);
                brickG.setType(valuesOfBlocks::BrickG);
                brickB.setType(valuesOfBlocks::BrickB);
                brickP.setType(valuesOfBlocks::BrickP);
                brickW.setType(valuesOfBlocks::BrickW);
                accessPoint.setType(valuesOfBlocks::AccessPoint);
                susJuice.setType(valuesOfBlocks::SusJuice);
                topSneakyNote.setType(valuesOfBlocks::TopSneakyNote);
                playerSpawn.setType(valuesOfBlocks::PlayerSpawn);
                types.push_back(&grass);
                types.push_back(&dirt);
                types.push_back(&brick);
                types.push_back(&laser);
                types.push_back(&lava);
                types.push_back(&playerSpawn);
                types.push_back(&portal);
                types.push_back(&brickR);
                types.push_back(&brickO);
                types.push_back(&brickY);
                types.push_back(&brickG);
                types.push_back(&brickB);
                types.push_back(&brickP);
                types.push_back(&brickW);
                types.push_back(&accessPoint);
                types.push_back(&susJuice);
                types.push_back(&topSneakyNote);
            }
            bool clickCheck(Vector2 &mousePos, editorBlock *subject)
            {
                if (CheckCollisionPointRec(mousePos, subject->getRectangle()) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                {
                    return true;
                }
                else
                {
                    return false;
                }
            }
        }
    }
}

int main()
{
    srand(time(nullptr));
    platformer::blocks::editor::init();
    platformer::editorBlock selectedBlock = platformer::blocks::editor::brick;
    platformer::animatedText animatedText;
    std::vector<platformer::editorBlock> blocks;
    Vector2 resolution{1900, 1060};
    Vector2 mousePosition;
    Vector2 snappingMousePosition;
    Color background{0, 0, 0, 255};
    float hypotenuse = std::sqrt((resolution.x * resolution.x) + (resolution.y * resolution.y));
    InitWindow(resolution.x, resolution.y, "Level Editor");
    SetWindowState(FLAG_WINDOW_RESIZABLE);
    int refreshRate = GetMonitorRefreshRate(GetCurrentMonitor());
    SetTargetFPS(refreshRate);
    Texture2D spritesheet = LoadTexture("assets/tilesheet.png");
    Camera2D viewPort;
    viewPort.offset = {resolution.x / 2, resolution.y / 2};
    viewPort.target = {0, 10000};
    viewPort.zoom = 0.1f;
    viewPort.rotation = 0;
    bool workerStatus{1};
    bool isInConsole{0};
    bool drawLaserBeams{1};
    bool showFPS{0};
    int isMesuring{0};
    double time;
    Vector2 rulerBegin;
    Vector2 rulerEnd;
    int defaultRotation{0};
    std::string consoleBuffer;
    while (!WindowShouldClose())
    {
        time = GetTime();
        resolution.x = GetRenderWidth();
        resolution.y = GetRenderHeight();
        mousePosition = GetMousePosition();
        {
            Vector2 cache = GetScreenToWorld2D(mousePosition, viewPort);
            snappingMousePosition.x = ((int)(cache.x / 64)) * 64;
            snappingMousePosition.y = ((int)(cache.y / 64)) * 64;
        }
        hypotenuse = std::sqrt((resolution.x * resolution.x) + (resolution.y * resolution.y));
        viewPort.offset = {resolution.x / 2, resolution.y / 2};
        BeginDrawing();
        ClearBackground(background);
        BeginMode2D(viewPort);
        if (drawLaserBeams)
        {
            for (size_t i = 0; i < blocks.size(); i++)
            {
                if (blocks.at(i).getType() == platformer::valuesOfBlocks::LaserNoTimeOffset)
                {
                    DrawLineEx(blocks.at(i).getBeginOfRay(), blocks.at(i).getEndOfRay(), 28, GREEN);
                }
            }
        }
        for (size_t i = 0; i < blocks.size(); i++)
        {
            {
                blocks.at(i).draw(spritesheet);
            }
        }
        // Draw square over cursor
        DrawRectangle(snappingMousePosition.x, snappingMousePosition.y, 64, 64, BLUE);
        // Draw the coordinates of the mouse cursor
        const char *mousePos = TextFormat("%d, %d", (int)snappingMousePosition.x, (int)snappingMousePosition.y);
        DrawText(mousePos, (snappingMousePosition.x - MeasureText(mousePos, 64) / 2), snappingMousePosition.y + 80, 64, YELLOW);
        switch (isMesuring)
        {
        case (1):
        {
            Vector2 midPoint;
            Vector2 distance;
            midPoint.x = (rulerBegin.x + (int)snappingMousePosition.x) / 2.0f;
            midPoint.y = (rulerBegin.y + (int)snappingMousePosition.y) / 2.0f;
            distance.x = std::abs((int)snappingMousePosition.x - rulerBegin.x) + 64;
            distance.y = std::abs((int)snappingMousePosition.y - rulerBegin.y) + 64;
            const char *distanceText = TextFormat("%d x %d", (int)distance.x / 64, (int)distance.y / 64);
            DrawText(distanceText, (midPoint.x - MeasureText(distanceText, 64) / 2), midPoint.y, 64, YELLOW);
            DrawRectangleV(rulerBegin, {64, 64}, {255, 0, 0, 100});
            DrawRectangleV(snappingMousePosition, {64, 64}, {253, 249, 0, 100});
            break;
        }
        case (2):
        {
            Vector2 midPoint;
            Vector2 distance;
            midPoint.x = (rulerBegin.x + rulerEnd.x) / 2.0f;
            midPoint.y = (rulerBegin.y + rulerEnd.y) / 2.0f;
            distance.x = std::abs(rulerEnd.x - rulerBegin.x) + 64;
            distance.y = std::abs(rulerEnd.y - rulerBegin.y) + 64;
            const char *distanceText = TextFormat("%d x %d", (int)distance.x / 64, (int)distance.y / 64);
            DrawText(distanceText, (midPoint.x - MeasureText(distanceText, 64) / 2), midPoint.y, 64, RED);
            DrawRectangleV(rulerBegin, {64, 64}, {255, 0, 0, 100});
            DrawRectangleV(rulerEnd, {64, 64}, {255, 0, 0, 100});
        }
        break;

        default:
            break;
        }
        EndMode2D();
        // Draw toolbox background
        DrawRectangle(0, 0, (resolution.x * 0.1f), resolution.y, {0, 0, 200, 100});
        // Draw all selectable blocks
        for (int i = 0; i < platformer::blocks::editor::types.size(); i++)
        {
            Vector2 whereToDraw = {(resolution.x * 0.015f), (resolution.y * 0.1f)};
            int k = ((int)i / 5);
            int z = (i % 5);
            whereToDraw.x = whereToDraw.x + (k * 0.015f * hypotenuse);
            whereToDraw.y = whereToDraw.y + (z * 0.015f * hypotenuse);
            platformer::blocks::editor::types.at(i)->setPosition(whereToDraw.x, whereToDraw.y);
            platformer::blocks::editor::types.at(i)->setDimentions(0.01f * hypotenuse, 0.01f * hypotenuse);
            platformer::blocks::editor::types.at(i)->draw(spritesheet);
        }
        // Draw text
        const char *descriptor = TextFormat("Blocks");
        const char *activeBlockStatus = TextFormat("Rotation: %d°", defaultRotation);
        DrawText(descriptor, (0.05f * resolution.x) - MeasureText(descriptor, (0.005f * hypotenuse)), resolution.y * 0.07f, 0.01f * hypotenuse, YELLOW);
        DrawText(activeBlockStatus, (0.05f * resolution.x) - MeasureText(activeBlockStatus, (0.005f * hypotenuse)), resolution.y * 0.8f, 0.01f * hypotenuse, YELLOW);
        if (isInConsole)
        {
            DrawText(consoleBuffer.c_str(), resolution.x * 0.1f, resolution.y * 0.8f, hypotenuse * 0.01f, YELLOW);
        }
        animatedText.draw(hypotenuse, time, 0.01f, resolution);
        if (showFPS)
        {
            DrawFPS(resolution.x * 0.8f, resolution.y * 0.1f);
        }
        EndDrawing();
        viewPort.zoom += (GetMouseWheelMove() / 100.0f);
        // Input handling
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
        {
            if (CheckCollisionPointRec(mousePosition, {(resolution.x * 0.1f), 0, resolution.x * 0.9f, resolution.y}))
            {
                bool duplicateFound{0};
                for (size_t i = 0; i < blocks.size(); i++)
                {
                    if (blocks.at(i).getType() == platformer::valuesOfBlocks::PlayerSpawn && selectedBlock.getType() == platformer::valuesOfBlocks::PlayerSpawn)
                    {
                        duplicateFound = 1;
                        break;
                    }
                    if (snappingMousePosition.y < 0)
                    {
                        duplicateFound = 1;
                        animatedText.setContent("You can only place blocks in the positive y-axis");
                        animatedText.setDestination(0.1f, 0.7f);
                        animatedText.revive(time, 3);
                        break;
                    }
                    if (snappingMousePosition.x == blocks.at(i).getPosition().x && snappingMousePosition.y == blocks.at(i).getPosition().y)
                    {
                        duplicateFound = 1;
                        break;
                    }
                }
                if (!duplicateFound)
                {
                    blocks.push_back(platformer::editorBlock(selectedBlock, snappingMousePosition.x, snappingMousePosition.y, 64, 64, defaultRotation));
                    blocks.at(blocks.size() - 1).setVisibility(1);
                    if (blocks.at(blocks.size() - 1).getType() == platformer::valuesOfBlocks::AccessPoint)
                    {
                        blocks.at(blocks.size() - 1).setDimentions(128, 128);
                    }
                    for (size_t i = 0; i < blocks.size(); i++)
                    {
                        if (blocks.at(i).getType() == platformer::valuesOfBlocks::LaserNoTimeOffset)
                        {
                            blocks.at(i).computeRay(blocks);
                        }
                    }
                }
            }
        }
        if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
        {
            if ((CheckCollisionPointRec(mousePosition, {(resolution.x * 0.1f), 0, resolution.x * 0.9f, resolution.y})))
            {
                for (size_t i = 0; i < blocks.size(); i++)
                {
                    if (snappingMousePosition.x == blocks.at(i).getPosition().x && snappingMousePosition.y == blocks.at(i).getPosition().y)
                    {
                        /*
                                    Since std::vector doesn't have a method for removing elements at an arbitrary index, i have to use this mess.
                                    It swaps the element at the last index in the vector, and the element at the index to remove. From then you can pop off the last element, deleting it.
                        */
                        platformer::editorBlock *cache1 = new platformer::editorBlock;
                        if (cache1 == nullptr)
                        {
                            animatedText.setContent("Failed to allocate memory");
                            animatedText.setDestination(0.1f, 0.7f);
                            animatedText.revive(time, 3);
                            continue;
                        }
                        platformer::editorBlock *cache2 = new platformer::editorBlock;
                        if (cache2 == nullptr)
                        {
                            delete cache1;
                            animatedText.setContent("Failed to allocate memory");
                            animatedText.setDestination(0.1f, 0.7f);
                            animatedText.revive(time, 3);
                            continue;
                        }
                        *cache1 = blocks.at(blocks.size() - 1);
                        *cache2 = blocks.at(i);
                        blocks.at(i) = *cache1;
                        blocks.pop_back();
                        delete cache1;
                        delete cache2;
                        for (size_t i = 0; i < blocks.size(); i++)
                        {
                            if (blocks.at(i).getType() == platformer::valuesOfBlocks::LaserNoTimeOffset)
                            {
                                blocks.at(i).computeRay(blocks);
                            }
                        }
                    }
                }
            }
        }
        if (IsMouseButtonDown(MOUSE_BUTTON_MIDDLE))
        {
            if ((CheckCollisionPointRec(mousePosition, {(resolution.x * 0.1f), 0, resolution.x * 0.9f, resolution.y})))
            {
                Vector2 cache = GetMouseDelta();
                viewPort.target.x -= 2 * cache.x;
                if ((viewPort.target.y - 2 * cache.y) > 0)
                {
                    viewPort.target.y -= 2 * cache.y;
                }
                else
                {
                    animatedText.setContent("You can only place blocks in the positive y-axis");
                    animatedText.setDestination(0.1f, 0.7f);
                    animatedText.revive(time, 3);
                }
            }
        }
        if (IsKeyPressed(KEY_R) && !isInConsole)
        {
            defaultRotation += 90;
            defaultRotation = defaultRotation % 360;
        }
        if (IsKeyPressed(KEY_LEFT_CONTROL))
        {
            switch (isMesuring)
            {
            case (0):
                rulerBegin = snappingMousePosition;
                isMesuring++;
                animatedText.setDestination(0.1f, 0.7f);
                animatedText.setContent("Ruler Position 1 Set");
                animatedText.revive(time, 3);
                break;
            case (1):
                rulerEnd = snappingMousePosition;
                isMesuring++;
                animatedText.setDestination(0.1f, 0.7f);
                animatedText.setContent("Ruler Position 2 Set\nShowing distance");
                animatedText.revive(time, 3);
                break;
            case (2):
                isMesuring = 0;
                animatedText.setDestination(0.1f, 0.7f);
                animatedText.setContent("Reset Ruler");
                animatedText.revive(time, 3);
                break;
            default:
                break;
            }
        }
        for (size_t i = 0; i < platformer::blocks::editor::types.size(); i++)
        {
            if (platformer::blocks::editor::clickCheck(mousePosition, platformer::blocks::editor::types.at(i)))
            {
                selectedBlock = *(platformer::blocks::editor::types.at(i));
                break;
            }
        }
        if (IsKeyPressed(KEY_SLASH))
        {
            isInConsole = !isInConsole;
        }
        if (IsKeyPressed(KEY_BACKSPACE) && consoleBuffer.size() > 0)
        {
            consoleBuffer.pop_back();
        }
        if (isInConsole)
        {
            int response = GetCharPressed();
            if (IsKeyPressed(KEY_ENTER))
            {
                isInConsole = !isInConsole;
                // This is an easy way to parse a delimited string
                std::stringstream buffer(consoleBuffer);
                consoleBuffer.clear();
                std::vector<std::string> realBuffers;
                while (std::getline(buffer, consoleBuffer, ' '))
                {
                    realBuffers.push_back(consoleBuffer);
                }
                consoleBuffer.clear();
                try
                {
                    if (realBuffers.at(0) == "/saveas" && realBuffers.size() > 1)
                    {
                        long int xmin = blocks.at(0).getPosition().x;
                        long int xmax = blocks.at(1).getPosition().x;
                        long int ymin = blocks.at(0).getPosition().y;
                        long int ymax = blocks.at(1).getPosition().y;
                        for (auto &i : blocks)
                        {
                            Vector2 cache = i.getPosition();
                            if (cache.x < xmin)
                            {
                                xmin = cache.x;
                            }
                            if (cache.x > xmax)
                            {
                                xmax = cache.x;
                            }
                            if (cache.y < ymin)
                            {
                                ymin = cache.y;
                            }
                            if (cache.y > ymax)
                            {
                                ymax = cache.y;
                            }
                        }
                        size_t xtotalNeededToAllocate = 1 + (((std::abs(xmin - xmax))) / 64);
                        size_t ytotalNeededToAllocate = 1 + (((std::abs(ymin - ymax))) / 64);
                        size_t total = (xtotalNeededToAllocate * ytotalNeededToAllocate);
                        /*
                        std::sort(blocks.begin(), blocks.end(), [](platformer::editorBlock &lhs, platformer::editorBlock &rhs)
                                  { return (lhs.getPosition().y < rhs.getPosition().y) || ((lhs.getPosition().y == rhs.getPosition().y) && (lhs.getPosition().x < rhs.getPosition().x)); });
                        */
                        std::stringstream buf;
                        buf << (int)background.r << ' ' << (int)background.g << ' ' << (int)background.b << ' ' << (int)background.a << '\n';
                        for (platformer::editorBlock &i : blocks)
                        {
                            Vector2 cache = i.getPosition();
                            buf << cache.x << ' ' << cache.y << ' ' << i.getType() << ' ' << i.getRotation() << '\n';
                        }
                        realBuffers.at(1) = "levels/" + realBuffers.at(1);
                        // std::cout << buf.str() << '\n';
                        std::fstream output(realBuffers.at(1), std::ios::out | std::ios::trunc);
                        output << buf.str();
                        output.close();

                        if (0)
                        {
                            animatedText.setContent("Failed to write file");
                            animatedText.setDestination(0.1f, 0.7f);
                            animatedText.revive(time, 3);
                        }
                        else
                        {
                            animatedText.setContent("Level saved sucessfully");
                            animatedText.setDestination(0.1f, 0.7f);
                            animatedText.revive(time, 3);
                        }
                    }
                    else if (realBuffers.at(0) == "/set")
                    {
                        if (realBuffers.at(1) == "background" && realBuffers.size() == 5)
                        {
                            background.r = (std::stoi(realBuffers.at(2)) % 256);
                            background.g = (std::stoi(realBuffers.at(3)) % 256);
                            background.b = (std::stoi(realBuffers.at(4)) % 256);
                        }
                        else if (realBuffers.at(1) == "fps" && realBuffers.size() == 3)
                        {
                            SetTargetFPS(std::stoi(realBuffers.at(2)));
                        }
                    }
                    else if (realBuffers.at(0) == "/help")
                    {
                        animatedText.setContent("See docs/usage.txt for commands");
                        animatedText.setDestination(0.1f, 0.7f);
                        animatedText.revive(time, 3);
                    }
                    else if (realBuffers.at(0) == "/showfps")
                    {
                        showFPS = !showFPS;
                    }
                    else if (realBuffers.at(0) == "/load")
                    {
                        realBuffers.at(1) = "levels/" + realBuffers.at(1);
                        // std::stringstream source = platformer::readCompressedData(realBuffers.at(1).c_str());
                        //  For whatever reason, there is a bug with how data is compressed. For now every level will be uncompressed until this is fixed
                        std::ifstream source(realBuffers.at(1), std::ios::in);
                        blocks.clear();
                        {
                            std::string placeholder;
                            std::getline(source, placeholder, '\n');
                            std::stringstream iDontKnow(placeholder);
                            std::vector<std::string> colors;
                            while (std::getline(iDontKnow, placeholder, ' '))
                            {
                                colors.push_back(placeholder);
                            }
                            background.r = std::stoi(colors.at(0));
                            background.g = std::stoi(colors.at(1));
                            background.b = std::stoi(colors.at(2));
                            background.a = std::stoi(colors.at(3));
                        }
                        std::string lineBuffer;
                        while (std::getline(source, lineBuffer, '\n'))
                        {
                            std::vector<int> parsableArguments;
                            std::stringstream iDontKnow(lineBuffer);
                            while (std::getline(iDontKnow, lineBuffer, ' '))
                            {
                                parsableArguments.push_back(std::stoi(lineBuffer));
                            }
                            if (parsableArguments.size() == 4)
                            {
                                switch (parsableArguments.at(2))
                                {
                                case (platformer::valuesOfBlocks::Grass):
                                    blocks.push_back(platformer::editorBlock(platformer::blocks::editor::grass, parsableArguments.at(0), (parsableArguments.at(1)), 64, 64, parsableArguments.at(3)));
                                    break;
                                case (platformer::valuesOfBlocks::Dirt):
                                    blocks.push_back(platformer::editorBlock(platformer::blocks::editor::dirt, parsableArguments.at(0), (parsableArguments.at(1)), 64, 64, parsableArguments.at(3)));
                                    break;
                                case (platformer::valuesOfBlocks::Brick):
                                    blocks.push_back(platformer::editorBlock(platformer::blocks::editor::brick, parsableArguments.at(0), (parsableArguments.at(1)), 64, 64, parsableArguments.at(3)));
                                    break;
                                case (platformer::valuesOfBlocks::LaserNoTimeOffset):
                                    blocks.push_back(platformer::editorBlock(platformer::blocks::editor::laser, parsableArguments.at(0), (parsableArguments.at(1)), 64, 64, parsableArguments.at(3)));
                                    break;
                                case (platformer::valuesOfBlocks::Lava):
                                    blocks.push_back(platformer::editorBlock(platformer::blocks::editor::lava, parsableArguments.at(0), (parsableArguments.at(1)), 64, 64, parsableArguments.at(3)));
                                    break;
                                case (platformer::valuesOfBlocks::PlayerSpawn):
                                    blocks.push_back(platformer::editorBlock(platformer::blocks::editor::playerSpawn, parsableArguments.at(0), (parsableArguments.at(1)), 64, 64, parsableArguments.at(3)));
                                    blocks.at(blocks.size() - 1).setPosition(parsableArguments.at(0), (parsableArguments.at(1)));
                                    break;
                                case (platformer::valuesOfBlocks::Portal):
                                    blocks.push_back(platformer::editorBlock(platformer::blocks::editor::portal, parsableArguments.at(0), (parsableArguments.at(1)), 64, 64, parsableArguments.at(3)));
                                    break;
                                case (platformer::valuesOfBlocks::BrickR):
                                    blocks.push_back(platformer::editorBlock(platformer::blocks::editor::brickR, parsableArguments.at(0), parsableArguments.at(1), 64, 64, parsableArguments.at(3)));
                                    break;
                                case (platformer::valuesOfBlocks::BrickO):
                                    blocks.push_back(platformer::editorBlock(platformer::blocks::editor::brickO, parsableArguments.at(0), parsableArguments.at(1), 64, 64, parsableArguments.at(3)));
                                    break;
                                case (platformer::valuesOfBlocks::BrickY):
                                    blocks.push_back(platformer::editorBlock(platformer::blocks::editor::brickY, parsableArguments.at(0), parsableArguments.at(1), 64, 64, parsableArguments.at(3)));
                                    break;
                                case (platformer::valuesOfBlocks::BrickG):
                                    blocks.push_back(platformer::editorBlock(platformer::blocks::editor::brickG, parsableArguments.at(0), parsableArguments.at(1), 64, 64, parsableArguments.at(3)));
                                    break;
                                case (platformer::valuesOfBlocks::BrickB):
                                    blocks.push_back(platformer::editorBlock(platformer::blocks::editor::brickB, parsableArguments.at(0), parsableArguments.at(1), 64, 64, parsableArguments.at(3)));
                                    break;
                                case (platformer::valuesOfBlocks::BrickP):
                                    blocks.push_back(platformer::editorBlock(platformer::blocks::editor::brickP, parsableArguments.at(0), parsableArguments.at(1), 64, 64, parsableArguments.at(3)));
                                    break;
                                case (platformer::valuesOfBlocks::BrickW):
                                    blocks.push_back(platformer::editorBlock(platformer::blocks::editor::brickW, parsableArguments.at(0), parsableArguments.at(1), 64, 64, parsableArguments.at(3)));
                                    break;
                                case (platformer::valuesOfBlocks::AccessPoint):
                                    blocks.push_back(platformer::editorBlock(platformer::blocks::editor::accessPoint, parsableArguments.at(0), (parsableArguments.at(1)), 128, 128, parsableArguments.at(3)));
                                    break;
                                case (platformer::valuesOfBlocks::SusJuice):
                                    blocks.push_back(platformer::editorBlock(platformer::blocks::editor::susJuice, parsableArguments.at(0), (parsableArguments.at(1)), 64, 64, parsableArguments.at(3)));
                                    break;
                                case (platformer::valuesOfBlocks::TopSneakyNote):
                                    blocks.push_back(platformer::editorBlock(platformer::blocks::editor::topSneakyNote, parsableArguments.at(0), parsableArguments.at(1), 64, 64, parsableArguments.at(3)));
                                    break;
                                default:
                                    break;
                                }
                            }
                        }
                        viewPort.target = (blocks.at(rand() % blocks.size())).getPosition();
                        for (size_t i = 0; i < blocks.size(); i++)
                        {
                            if (blocks.at(i).getType() == platformer::valuesOfBlocks::LaserNoTimeOffset)
                            {
                                blocks.at(i).computeRay(blocks);
                            }
                        }
                        source.close();
                    }
                    else if (realBuffers.at(0) == "/showlasers")
                    {
                        drawLaserBeams = !drawLaserBeams;
                    }
                    else
                    {
                        throw std::invalid_argument("Invalid command or syntax");
                    }
                }
                catch (const std::exception &e)
                {
                    animatedText.setContent(e.what());
                    animatedText.setDestination(0.1f, 0.7f);
                    animatedText.revive(time, 3);
                }
            }
            else if (response != 0 && response != '\0' && response >= 32 && response <= 122)
            {
                consoleBuffer += response;
            }
        }
    }
    workerStatus = 0;
    UnloadTexture(spritesheet);
    CloseWindow();
    return 0;
}