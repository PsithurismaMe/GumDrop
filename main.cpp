#include "headers/settings.hpp"

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
    platformer::player player = platformer::blocks::templatePlayer;
    player.setIterablePointer(&globalIterables[1]);
    std::thread everyOneSec(platformer::blocks::incrementEveryMilliseconds, std::ref(globalIterables[0]), std::ref(workerStatus), 1000);
    std::thread every100ms(platformer::blocks::incrementEveryMilliseconds, std::ref(globalIterables[1]), std::ref(workerStatus), 100);
    std::thread every16ms(platformer::blocks::Every16Milliseconds, std::ref(staticBlocks), std::ref(animatedBlocks), std::ref(player), std::ref(workerStatus), std::ref(platformer::settings::activeKeypresses));
    for (auto i : animatedBlocks)
    {
        i->setIterablePointer(&globalIterables[1]);
    }
    while (!WindowShouldClose())
    {
        resolution.x = GetRenderWidth();
        resolution.y = GetRenderHeight();
        hypotenuse = std::sqrt((resolution.x * resolution.x) + (resolution.y * resolution.y));
        platformer::blocks::inGameCamera.offset = {resolution.x / 2, resolution.y / 2};
        platformer::blocks::inGameCamera.target = player.getPosition();
        BeginDrawing();
        ClearBackground(BLACK);
        BeginMode2D(platformer::blocks::inGameCamera);
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
        player.draw(spritesheet);
        EndMode2D();
        console.draw(resolution, hypotenuse, keypress);
        EndDrawing();
        platformer::settings::activeKeypresses[0] = IsKeyDown(KEY_D);
        platformer::settings::activeKeypresses[1] = IsKeyDown(KEY_A);

        
    }
    workerStatus = 0;
    every100ms.join();
    everyOneSec.join();
    every16ms.join();
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