#include "headers/settings.hpp"

int main(int argc, char **argv)
{
    bool isRunning{1};
    std::string filename = "myLevel.lvl";
    while (isRunning)
    {
        std::vector<platformer::stationaryStaticBlock *> staticBlocks;
        std::vector<platformer::stationaryAnimatedBlock *> animatedBlocks;
        platformer::blocks::init();
        InitWindow(800, 400, "A Window");
        std::string temporaryFileName = "levels/" + filename;
        platformer::blocks::loadFromFile(temporaryFileName.c_str(), staticBlocks, animatedBlocks);
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
        std::thread test([&]
        {
            while (workerStatus)
            {
                for (size_t i = 0; i < staticBlocks.size(); i++)
                {
                    Vector2 cache = GetWorldToScreen2D(staticBlocks.at(i)->getPosition(), platformer::blocks::inGameCamera);
                    if (cache.x < resolution.x && cache.x > 0 && cache.y < resolution.y && cache.y > 0)
                    {
                        staticBlocks.at(i)->setVisibility(1);
                    }
                    else
                    {
                        staticBlocks.at(i)->setVisibility(0);
                    }
                }
            }
            
        });
        std::thread everyOneSec(platformer::blocks::incrementEveryMilliseconds, std::ref(globalIterables[0]), std::ref(workerStatus), 1000);
        std::thread every100ms(platformer::blocks::incrementEveryMilliseconds, std::ref(globalIterables[1]), std::ref(workerStatus), 100);
        std::thread every16ms(platformer::blocks::Every16Milliseconds, std::ref(staticBlocks), std::ref(animatedBlocks), std::ref(player), std::ref(workerStatus), std::ref(platformer::settings::activeKeypresses));
        for (auto i : animatedBlocks)
        {
            i->setIterablePointer(&globalIterables[1]);
        }
        while (isRunning)
        {
            isRunning = !WindowShouldClose();
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
                    DrawRectangle(source.x + 64, source.y + 19, animatedBlocks.at(i)->getRayLength(), 28, {0, 255, 0, 255});
                }
            }
            for (int i = 0; i < staticBlocks.size(); i++)
            {
                if (staticBlocks.at(i)->getVisibility())
                {
                    staticBlocks.at(i)->draw(spritesheet);
                }   
                
            }
            for (int i = 0; i < animatedBlocks.size(); i++)
            {
                animatedBlocks.at(i)->draw(spritesheet);
            }
            player.draw(spritesheet);
            EndMode2D();
            if(console.draw(resolution, hypotenuse, keypress, filename) == -1)
            {
                break;
            }
            EndDrawing();
            keypress = GetCharPressed();
            if (IsKeyPressed(KEY_SLASH))
            {
                console.toggleConsole();
            }
            platformer::settings::activeKeypresses[0] = IsKeyDown(KEY_D);
            platformer::settings::activeKeypresses[1] = IsKeyDown(KEY_A);
            platformer::settings::activeKeypresses[2] = IsKeyDown(KEY_SPACE);
        }
        workerStatus = 0;
        every100ms.join();
        everyOneSec.join();
        every16ms.join();
        test.join();
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
}