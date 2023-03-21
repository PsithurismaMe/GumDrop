#include "headers/menus.hpp"

int main(int argc, char **argv)
{
    // Seed random number generator
    srand(time(nullptr));
    bool isRunning{1};
    bool fullscreen {0};
    Vector2 resolution = {800, 400};
    // Specify the level to load upon startup
    std::string filename = "myLevel.lvl";
    while (isRunning)
    {
        // Create vectors to store blocks on the heap
        std::vector<platformer::stationaryStaticBlock *> staticBlocks;
        std::vector<platformer::stationaryAnimatedBlock *> animatedBlocks;
        // Instansiate any template blocks on the stack
        platformer::blocks::init();
        platformer::ui::init();
        // Initialize raylib
        InitWindow(resolution.x, resolution.y, "A Window");
        // Warn the user that this multithreaded program may not run correctly on old systems.
        {
            unsigned int threads = std::thread::hardware_concurrency();
            if (threads < 5)
            {
                std::cerr << "WARN: SYSTEM: Your system supports only " << threads << " concurrent threads. You may experience stuttering or other bugs. Capping your framerate may resolve stuttering\n";
            }
        }
        // Change raylibs default exit key to something impossible to press
        SetExitKey(-1);
        // Load the level and store any blocks in the vectors above
        std::string temporaryFileName = "levels/" + filename;
        platformer::blocks::loadFromFile(temporaryFileName.c_str(), staticBlocks, animatedBlocks);
        // Allow the window manager to handle resizing
        SetWindowState(FLAG_WINDOW_RESIZABLE);
        if (fullscreen)
        {
            SetWindowState(FLAG_FULLSCREEN_MODE);
        }
        // Load textures
        Texture2D spritesheet = LoadTexture("assets/tilesheet.png");
        
        Vector2 mousePosition;
        float hypotenuse;
        // These variables are used for animation
        size_t globalIterables[2] = {0, 0};
        bool workerStatus{1};
        bool isPaused{0};
        wchar_t keypress{0};
        platformer::console console;
        // Instansiate a player and copy the templatePlayer
        platformer::player player = platformer::blocks::templatePlayer;
        // Used for animation
        player.setIterablePointer(&globalIterables[1]);
        // Used to optimize collision checking and drawing
        std::thread optimization([&]
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
        // Assign any animated blocks a pointer to a size_t
        for (auto i : animatedBlocks)
        {
            i->setIterablePointer(&globalIterables[1]);
        }
        while (isRunning)
        {
            isRunning = !WindowShouldClose();
            resolution.x = GetRenderWidth();
            resolution.y = GetRenderHeight();
            mousePosition = GetMousePosition();
            hypotenuse = std::sqrt((resolution.x * resolution.x) + (resolution.y * resolution.y));
            platformer::blocks::inGameCamera.offset = {resolution.x / 2, resolution.y / 2};
            platformer::blocks::inGameCamera.target = player.getPosition();
            BeginDrawing();
            ClearBackground(BLACK);
            if (isPaused)
            {
                platformer::ui::alternateRenderer(mousePosition, resolution, isPaused, isRunning, spritesheet, hypotenuse);
            }
            else
            {
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
                // Draw regular blocks
                for (int i = 0; i < staticBlocks.size(); i++)
                {
                    if (staticBlocks.at(i)->getVisibility())
                    {
                        staticBlocks.at(i)->draw(spritesheet);
                    }
                }
                // Draw other animated blocks
                for (int i = 0; i < animatedBlocks.size(); i++)
                {
                    animatedBlocks.at(i)->draw(spritesheet);
                }
                player.draw(spritesheet);
                EndMode2D();
            }
            // Since you can load other levels from the console, it must be able to break this while loop without closing the program.
            if (console.draw(resolution, hypotenuse, keypress, filename, fullscreen) == -1)
            {
                break;
            }
            EndDrawing();
            keypress = GetCharPressed();
            if (IsKeyPressed(KEY_SLASH))
            {
                console.toggleConsole();
            }
            if (IsKeyPressed(KEY_ESCAPE))
            {
                isPaused = !isPaused;
            }
            platformer::settings::activeKeypresses[0] = (IsKeyDown(KEY_D) xor (GetGamepadAxisMovement(0, 0) > 0));
            platformer::settings::activeKeypresses[1] = (IsKeyDown(KEY_A) xor (GetGamepadAxisMovement(0, 0) < 0));
            platformer::settings::activeKeypresses[2] = (IsKeyDown(KEY_SPACE) xor IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN));
        }
        workerStatus = 0;
        every100ms.join();
        everyOneSec.join();
        every16ms.join();
        optimization.join();
        // Avoid memory leaks
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