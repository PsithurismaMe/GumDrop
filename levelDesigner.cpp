#include "headers/blocks.hpp"

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
            stationaryStaticBlock playerSpawn;

            std::vector<stationaryStaticBlock *> types;
            void init()
            {
                grass.setPositionOnSpriteSheet({0, 0, 64, 64});
                dirt.setPositionOnSpriteSheet({64, 0, 64, 64});
                brick.setPositionOnSpriteSheet({128, 0, 64, 64});
                laser.setPositionOnSpriteSheet({0, 1792, 64, 64});
                lava.setPositionOnSpriteSheet({0, 1856, 64, 64});
                grass.setGroupNumber(-1);
                dirt.setGroupNumber(-1);
                brick.setGroupNumber(-1);
                laser.setGroupNumber(-1);
                lava.setGroupNumber(-1);
                grass.setType(1);
                dirt.setType(2);
                brick.setType(3);
                laser.setType(4);
                lava.setType(5);
                types.push_back(&grass);
                types.push_back(&dirt);
                types.push_back(&brick);
                types.push_back(&laser);
                types.push_back(&lava);
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
        }
    }
}

int main()
{
    platformer::blocks::editor::init();
    platformer::stationaryStaticBlock selectedBlock = platformer::blocks::editor::brick;
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
    viewPort.target = {0, 0};
    viewPort.zoom = 0.1f;
    viewPort.rotation = 0;
    bool workerStatus{1};
    while (!WindowShouldClose())
    {
        resolution.x = GetRenderWidth();
        resolution.y = GetRenderHeight();
        mousePosition = GetMousePosition();
        {
            Vector2 cache = GetScreenToWorld2D(mousePosition, viewPort);
            snappingMousePosition.x = ((int)(cache.x / 64)) * 64;
            snappingMousePosition.y = ((int)(cache.y / 64)) * 64;
        }
        hypotenuse = std::sqrt((resolution.x * resolution.x) + (resolution.y * resolution.y));
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
        EndDrawing();
        viewPort.zoom += (GetMouseWheelMove() / 100.0f);
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
        {
            if (CheckCollisionPointRec(mousePosition, {(resolution.x * 0.1f), 0, resolution.x * 0.9f, resolution.y}))
            {
                bool duplicateFound{0};
                for (size_t i = 0; i < blocks.size(); i++)
                {
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
                viewPort.target.y -= 2 * cache.y;
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
    }
    workerStatus = 0;
    UnloadTexture(spritesheet);
    CloseWindow();
    return 0;
}