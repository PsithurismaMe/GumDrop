#include "headers/blocks.hpp"
#include <algorithm>

namespace platformer
{
    namespace blocks
    {
        namespace editor
        {
            stationaryStaticBlock grass; // 1
            stationaryStaticBlock dirt;  // 2
            stationaryStaticBlock brick; // 3
            stationaryStaticBlock laser; // 4
            stationaryStaticBlock lava;  // 5
            stationaryStaticBlock playerSpawn; // 6
            // Returns the char which corresponds to a type
            char typeToChar(int type)
            {
                char types[] = {'G', 'D', 'B', 'L', 'M', 'S'};
                return types[type - 1];
            }
            std::vector<stationaryStaticBlock *> types;
            void init()
            {
                grass.setPositionOnSpriteSheet({0, 0, 64, 64});
                dirt.setPositionOnSpriteSheet({64, 0, 64, 64});
                brick.setPositionOnSpriteSheet({128, 0, 64, 64});
                laser.setPositionOnSpriteSheet({0, 1792, 64, 64});
                lava.setPositionOnSpriteSheet({0, 1856, 64, 64});
                playerSpawn.setPositionOnSpriteSheet({0, 1984, 64, 64});
                grass.setGroupNumber(-1);
                dirt.setGroupNumber(-1);
                brick.setGroupNumber(-1);
                laser.setGroupNumber(-1);
                lava.setGroupNumber(-1);
                playerSpawn.setGroupNumber(-1);
                grass.setType(1);
                dirt.setType(2);
                brick.setType(3);
                laser.setType(4);
                lava.setType(5);
                playerSpawn.setType(6);
                types.push_back(&grass);
                types.push_back(&dirt);
                types.push_back(&brick);
                types.push_back(&laser);
                types.push_back(&lava);
                types.push_back(&playerSpawn);
            }
            bool clickCheck(Vector2 & mousePos, stationaryStaticBlock * subject)
            {
                if (CheckCollisionPointRec(mousePos, subject->getRectangle()) && IsMouseButtonDown(MOUSE_BUTTON_LEFT))
                {
                    return true;
                }
                else
                {
                    return false;
                }
            }
            class animatedText
            {
                protected:
                std::string content;
                float timeToLive;
                double creationTime;
                Vector2 destination;
                public:
                animatedText()
                {

                }
                // Sets the destination coordinates
                void setDestination(float x, float y)
                {
                    destination.x = x;
                    destination.y = y;
                }
                // Sets the string contents
                void setContent(const char * cnt)
                {
                    content = cnt;
                }
                // Resets the timer
                void revive(double timeOfBirth, float ttl)
                {
                    creationTime = timeOfBirth;
                    timeToLive = ttl;
                }
                void draw(float & c, double time, float fontsize, Vector2 & resolution)
                {
                    if ((creationTime + timeToLive) > time)
                    {
                        DrawText(content.c_str(), (destination.x * resolution.x), (destination.y * resolution.y), fontsize * c, YELLOW);
                    }
                }
            };
            
        }
    }
}

int main()
{
    platformer::blocks::editor::init();
    platformer::stationaryStaticBlock selectedBlock = platformer::blocks::editor::brick;
    platformer::blocks::editor::animatedText animatedText;
    std::vector<platformer::stationaryStaticBlock> blocks;
    Vector2 resolution = {800, 400};
    Vector2 mousePosition;
    Vector2 snappingMousePosition;
    float hypotenuse = std::sqrt((resolution.x * resolution.x) + (resolution.y * resolution.y));
    InitWindow(resolution.x, resolution.y, "Level Editor");
    SetWindowState(FLAG_WINDOW_RESIZABLE);
    Texture2D spritesheet = LoadTexture("assets/tilesheet.png");
    Camera2D viewPort;
    viewPort.offset = {resolution.x / 2, resolution.y / 2};
    viewPort.target = {0, 10000};
    viewPort.zoom = 0.1f;
    viewPort.rotation = 0;
    bool workerStatus{1};
    bool isInConsole {0};
    double time;
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
        ClearBackground(BLACK);
        BeginMode2D(viewPort);
        for (size_t i = 0; i < blocks.size(); i++)
        {
            {
                blocks.at(i).draw(spritesheet);
            }
        }
        // Draw square over cursor
        DrawRectangle(snappingMousePosition.x, snappingMousePosition.y, 64, 64, BLUE);
        // Draw the coordinates of the mouse cursor
        const char * mousePos = TextFormat("%d, %d", (int) snappingMousePosition.x, (int) snappingMousePosition.y);
        DrawText(mousePos, (snappingMousePosition.x - MeasureText(mousePos, 64) / 2), snappingMousePosition.y + 80, 64, YELLOW);

        EndMode2D();
        // Draw toolbox background
        DrawRectangle(0, 0, (resolution.x * 0.1f), resolution.y, {0, 0, 200, 100});
        // Draw all selectable blocks
        for (int i = 0; i < platformer::blocks::editor::types.size(); i++)
        {
            int k = ((int) i / 5);
            platformer::blocks::editor::types.at(i)->setPosition((resolution.x * 0.015f) + ((i % 5) * resolution.x * 0.015f), (resolution.y * 0.1f * (k + 1)));
            platformer::blocks::editor::types.at(i)->setDimentions(0.01f * hypotenuse, 0.01f * hypotenuse);
            platformer::blocks::editor::types.at(i)->draw(spritesheet);
        }
        // Draw text
        const char * descriptor = TextFormat("Choose a block");
        DrawText(descriptor, 0.01f * resolution.x, resolution.y * 0.07f, 0.01f * hypotenuse, YELLOW);
        if (isInConsole)
        {
            DrawText(consoleBuffer.c_str(), resolution.x * 0.1f, resolution.y * 0.8f, hypotenuse * 0.01f, YELLOW);
        }
        animatedText.draw(hypotenuse, time, 0.01f, resolution);
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
                    if (blocks.at(i).getType() == 6 && selectedBlock.getType() == 6)
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
                    blocks.push_back(platformer::stationaryStaticBlock(selectedBlock, snappingMousePosition.x, snappingMousePosition.y, 64, 64));
                    blocks.at(blocks.size() - 1).setVisibility(1);
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
                        platformer::stationaryStaticBlock *cache1 = new platformer::stationaryStaticBlock;
                        platformer::stationaryStaticBlock *cache2 = new platformer::stationaryStaticBlock;
                        *cache1 = blocks.at(blocks.size() - 1);
                        *cache2 = blocks.at(i);
                        blocks.at(i) = *cache1;
                        blocks.pop_back();
                        delete cache1;
                        delete cache2;
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
        for (size_t i = 0; i < platformer::blocks::editor::types.size() ; i++)
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
                        std::sort(blocks.begin(), blocks.end(), [](platformer::stationaryStaticBlock& lhs, platformer::stationaryStaticBlock& rhs) {
                            return (lhs.getPosition().y < rhs.getPosition().y) || ((lhs.getPosition().y  == rhs.getPosition().y) && (lhs.getPosition().x < rhs.getPosition().x));
                        });
                        std::string buf;
                        for (long int y = 0; y < ytotalNeededToAllocate; y++)
                        {
                            for (long int z = 0; z < xtotalNeededToAllocate; z++)
                            {
                                char blockFound {'.'};
                                for (long int k = 0; k < blocks.size(); k++)
                                {
                                    Vector2 cache = blocks.at(k).getPosition();
                                    
                                    if (cache.x == (xmin + (z * 64)) && cache.y == (ymin + (y * 64)))
                                    {
                                        blockFound = platformer::blocks::editor::typeToChar(blocks.at(k).getType());
                                        break;
                                    }
                                }
                                buf += blockFound;
                            }
                            buf += '\n';
                        }
                        realBuffers.at(1) = "levels/" + realBuffers.at(1);
                        if (SaveFileText(realBuffers.at(1).c_str(), (char *)buf.c_str()) != 1)
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
                    else if (realBuffers.at(0) == "/help")
                    {
                        animatedText.setContent("See docs/usage.txt for commands");
                        animatedText.setDestination(0.1f, 0.7f);
                        animatedText.revive(time, 3);
                    }
                    else
                    {
                        throw std::invalid_argument("Invalid command");
                    }

                }
                catch(const std::exception& e)
                {
                    animatedText.setContent(e.what());
                    animatedText.setDestination(0.1f, 0.7f);
                    animatedText.revive(time, 3);
                }
                
            }
            else if (response != 0 && response != '\0')
            {
                consoleBuffer += (response % 255);
            }
            
        }
    }
    workerStatus = 0;
    UnloadTexture(spritesheet);
    CloseWindow();
    return 0;
}