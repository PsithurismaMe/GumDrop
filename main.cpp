#include "headers/blocks.hpp"

int main(int argc, char **argv)
{
    std::vector<platformer::stationaryStaticBlock *> staticBlocks;
    std::vector<platformer::stationaryAnimatedBlock *> animatedBlocks;
    platformer::blocks::init();
    InitWindow(800, 400, "A Window");
    platformer::blocks::loadFromFile("levels/test.lvl", staticBlocks, animatedBlocks);
    SetWindowState(FLAG_WINDOW_RESIZABLE);
    Texture2D spritesheet = LoadTexture("assets/tilesheet.png");
    Vector2 resolution;
    float hypotenuse;
    size_t globalIterables[2] = {0, 0};
    bool workerStatus{1};
    wchar_t keypress{0};
    platformer::console console;
    /*
    staticBlocks.push_back(new platformer::stationaryStaticBlock);
    staticBlocks.push_back(new platformer::stationaryStaticBlock);
    staticBlocks.push_back(new platformer::stationaryStaticBlock);
    animatedBlocks.push_back(new platformer::stationaryAnimatedBlock(platformer::blocks::laser, 0, 0, 64, 64, &globalIterables[0]));
    animatedBlocks.push_back(new platformer::stationaryAnimatedBlock(platformer::blocks::lava, 64, 0, 64, 64, &globalIterables[1]));
    (*staticBlocks.at(0)) = platformer::blocks::brick;
    (*staticBlocks.at(1)) = platformer::blocks::dirt;
    (*staticBlocks.at(2)) = platformer::blocks::grass;
    for (int i = 0 ; i < 3; i++)
    {
        staticBlocks.at(i)->setPosition(64 * i, 0);
        staticBlocks.at(i)->setDimentions(64, 64);
    */
    std::thread everyOneSec(platformer::blocks::incrementEveryMilliseconds, std::ref(globalIterables[0]), std::ref(workerStatus), 1000);
    std::thread every100ms(platformer::blocks::incrementEveryMilliseconds, std::ref(globalIterables[1]), std::ref(workerStatus), 100);
    for (auto i : animatedBlocks)
    {
        i->setIterablePointer(&globalIterables[1]);
    }
    while (!WindowShouldClose())
    {
        resolution.x = GetRenderWidth();
        resolution.y = GetRenderHeight();
        hypotenuse = std::sqrt((resolution.x * resolution.x) + (resolution.y * resolution.y));
        BeginDrawing();
        ClearBackground(BLACK);
        // Draw laser beams
        for (int i = 0; i < animatedBlocks.size(); i++)
        {
            if (animatedBlocks.at(i)->getType() == 4 && animatedBlocks.at(i)->getFrameDisplayed() == 1)
            {
                animatedBlocks.at(i)->setIterablePointer(&globalIterables[0]);
                Vector2 source = animatedBlocks.at(i)->getPosition();
                DrawRectangle(source.x + 64, source.y + 19, 2000, 28, {0, 255, 0, 255});
            }
        }
        for (int i = 0; i < staticBlocks.size(); i++)
        {
            staticBlocks.at(i)->draw(spritesheet);
        }
        for (int i = 0; i < animatedBlocks.size(); i++)
        {
            animatedBlocks.at(i)->draw(spritesheet);
        }

        console.draw(resolution, hypotenuse, keypress);
        EndDrawing();
        keypress = GetCharPressed();
    }
    workerStatus = 0;
    every100ms.join();
    everyOneSec.join();
    for (int i = 0; i < staticBlocks.size(); i++)
    {
        delete staticBlocks.at(i);
    }
    for (int i = 0; i < animatedBlocks.size(); i++)
    {
        delete animatedBlocks.at(i);
    }
    staticBlocks.clear();
    animatedBlocks.clear();
    UnloadTexture(spritesheet);
    CloseWindow();
}