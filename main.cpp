#include "headers/menus.hpp"

int main(int argc, char **argv)
{
    srand(time(nullptr));
    bool isRunning{1};
    Color background;
    Vector2 resolution = {800, 400};
    InitWindow(resolution.x, resolution.y, "A Window");
    InitAudioDevice();
    Texture2D spritesheet = LoadTexture("assets/tilesheet.png");
    Image windowIcon = LoadImage("assets/icon.png");
    SetExitKey(-1);
    SetWindowIcon(windowIcon);
    SetWindowState(FLAG_WINDOW_RESIZABLE);
    std::string filename = "1";
    if (FileExists(".savedata"))
    {
        char * data = LoadFileText(".savedata");
        filename = data;
        UnloadFileText(data);
    }
    platformer::blocks::init();
    platformer::ui::init();
    platformer::music::init();
    PlayMusicStream(*platformer::music::activeMusic);
    std::vector<platformer::stationaryStaticBlock> staticBlocks;
    std::vector<platformer::stationaryAnimatedBlock> animatedBlocks;
    while (isRunning)
    {
        // Warn the user that this multithreaded program may not run correctly on old systems.
        {
            unsigned int threads = std::thread::hardware_concurrency();
            if (threads < 5) { std::cerr << "WARN: SYSTEM: Your system supports only " << threads << " concurrent threads. You may experience stuttering or other bugs. Capping your framerate may resolve stuttering\n"; }
        }
        std::string temporaryFileName = "levels/" + filename;
        platformer::blocks::loadFromFile(temporaryFileName.c_str(), staticBlocks, animatedBlocks, background);
        Vector2 mousePosition{0, 0};
        float hypotenuse{1.0f};
        float tickRate{1.0f / 60.0f};
        double time {0};
        // These variables are used for animation
        size_t globalIterables[2] = {0, 0};
        bool workerStatus{1};
        bool isPaused{0};
        wchar_t keypress{0};
        platformer::console console;
        platformer::player player = platformer::blocks::templatePlayer;
        platformer::animatedText animatedText;
        animatedText.setDestination(0.1f, 0.7f);
        animatedText.setContent(platformer::music::playlist.at(platformer::music::currentlyPlayingIndex).Title.c_str());
        animatedText.revive(time, 10);
        // Used for animation
        player.setIterablePointer(&globalIterables[1]);
        // Used to optimize collision checking and drawing
        std::thread optimization([&]
                                {
                                while (workerStatus)
                                {
                                    for (size_t i = 0; i < staticBlocks.size(); i++)
                                    {
                                        Vector2 cache = GetWorldToScreen2D(staticBlocks.at(i).getPosition(), platformer::blocks::inGameCamera);
                                        if (cache.x < resolution.x && cache.x > -64 && cache.y < resolution.y && cache.y > -64)
                                        {
                                            staticBlocks.at(i).setVisibility(1);
                                            //float distance = sqrt((pow((staticBlocks.at(i).getPosition().x - platformer::blocks::inGameCamera.target.x), 2) + pow((staticBlocks.at(i).getPosition().y - platformer::blocks::inGameCamera.target.y), 2)));
                                            //staticBlocks.at(i).setAlpha(((1152.0f - distance) / 1152.0f) * 255);
                                        }
                                        else
                                        {
                                            staticBlocks.at(i).setVisibility(0);
                                        }
                                    }
                                    
                                    //for (size_t i = 0; i < animatedBlocks.size(); i++)
                                    //{
                                    //    float distance = sqrt((pow((animatedBlocks.at(i).getPosition().x - platformer::blocks::inGameCamera.target.x), 2) + pow((animatedBlocks.at(i).getPosition().y - platformer::blocks::inGameCamera.target.y), 2)));
                                    //    animatedBlocks.at(i).setAlpha(((1152.0f - distance) / 1152.0f) * 255);
                                    //}
                                    
                                }           
                                });
        std::thread everyOneSec(platformer::blocks::incrementEveryMilliseconds, std::ref(globalIterables[0]), std::ref(workerStatus), 1000);
        std::thread every100ms(platformer::blocks::incrementEveryMilliseconds, std::ref(globalIterables[1]), std::ref(workerStatus), 100);
        std::thread every16ms(platformer::blocks::Every16Milliseconds, std::ref(staticBlocks), std::ref(animatedBlocks), std::ref(player), std::ref(workerStatus), std::ref(platformer::settings::activeKeypresses), std::ref(tickRate), std::ref(filename), std::ref(animatedText), std::ref(time));
        for (int i = 0; i < animatedBlocks.size(); i++)
        {
            animatedBlocks.at(i).setIterablePointer(&globalIterables[1]);
            if (animatedBlocks.at(i).getType() == platformer::Lava || animatedBlocks.at(i).getType() == platformer::SusJuice)
            {
                animatedBlocks.at(i).setIteratorOffset(i);
            }
        }
        console.assignPointers(&resolution, &mousePosition, &hypotenuse, &keypress, &filename, &animatedText, &time, &player);
        while (isRunning)
        {
            platformer::music::update(animatedText, time);
            time = GetTime();
            isRunning = !WindowShouldClose();
            resolution.x = GetRenderWidth();
            resolution.y = GetRenderHeight();
            mousePosition = GetMousePosition();
            hypotenuse = std::sqrt((resolution.x * resolution.x) + (resolution.y * resolution.y));
            platformer::blocks::inGameCamera.offset = {resolution.x / 2, resolution.y / 2};
            platformer::blocks::inGameCamera.target = player.getPosition();
            BeginDrawing();
            ClearBackground(background);
            if (isPaused)
            {
                platformer::ui::pauseMenu(mousePosition, resolution, isPaused, isRunning, spritesheet, hypotenuse, tickRate);
            }
            else
            {
                BeginMode2D(platformer::blocks::inGameCamera);
                // Draw laser beams
                for (int i = 0; i < animatedBlocks.size(); i++)
                {
                    if (animatedBlocks.at(i).getType() == platformer::valuesOfBlocks::LaserNoTimeOffset && animatedBlocks.at(i).getFrameDisplayed() == 1)
                    {
                        animatedBlocks.at(i).setIterablePointer(&globalIterables[0]);
                        DrawLineEx(animatedBlocks.at(i).getRayBegin(), animatedBlocks.at(i).getRayEnd(), 28, {0, 255, 0, 255});
                    }
                }
                // Draw regular blocks
                for (int i = 0; i < staticBlocks.size(); i++)
                {
                    if (staticBlocks.at(i).getVisibility())
                    {
                        staticBlocks.at(i).draw(spritesheet);
                    }
                }
                // Draw other animated blocks
                for (int i = 0; i < animatedBlocks.size(); i++)
                {
                    animatedBlocks.at(i).draw(spritesheet);
                }
                player.draw(spritesheet);
                EndMode2D();
            }
            animatedText.draw(hypotenuse, time, 0.01f, resolution);
            if (console.draw() == -1)
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
                if (!isPaused)
                {
                    tickRate = 1.0f / 60.0f;
                }
            }
            platformer::settings::activeKeypresses[0] = (IsKeyDown(KEY_D) xor (GetGamepadAxisMovement(0, 0) > 0));
            platformer::settings::activeKeypresses[1] = (IsKeyDown(KEY_A) xor (GetGamepadAxisMovement(0, 0) < 0));
            platformer::settings::activeKeypresses[2] = (IsKeyDown(KEY_SPACE) xor IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN));
            platformer::settings::activeKeypresses[3] = (IsKeyDown(KEY_MINUS) xor IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_DOWN));
            if (player.getReloadStatus())
            {
                break;
            }
        }
        
        workerStatus = 0;
        every100ms.join();
        everyOneSec.join();
        every16ms.join();
        optimization.join();
        staticBlocks.clear();
        animatedBlocks.clear();
    }
    StopMusicStream(*platformer::music::activeMusic);
    platformer::music::release();
    UnloadTexture(spritesheet);
    UnloadImage(windowIcon);
    CloseAudioDevice();
    CloseWindow();
}