#pragma once
#include "blocks.hpp"

namespace platformer
{
    namespace settings
    {
        std::vector<int> activeKeypresses(5, 0);
    }
    namespace music
    {
        struct song
        {
            Music music;
            std::string Title;
        };
        std::vector<song> playlist;
        Music *activeMusic = nullptr;
        int currentlyPlayingIndex{0};
        void init()
        {
            /*

            NOTICE: Some songs have been excluded from this git repository due to incompatible licensing.
            If you wish to use other music, do the following,

            1. Obtain a DRM free copy of the music and transcode it if necessary to a codec supported by the [raudio raylib module](https://github.com/raysan5/raylib/blob/master/src/raudio.c)
            2. Place it in assets/music/
            3. (OPTIONAL) Rename the file to match the pattern, "<Music Title> by <Artist>.something"
            4. Run the game. It will automatically load any music present in the assets/music/ directory
            5. DO NOT COMMIT MUSIC INCOMPATIBLE WITH THE GPL-V3
            */
            FilePathList musicInDirectory = LoadDirectoryFiles("assets/music/");
            for (int i = 0; i < musicInDirectory.count; i++)
            {
                std::string placeholder(musicInDirectory.paths[i]);
                playlist.push_back(song());
                playlist.at(playlist.size() - 1).music = LoadMusicStream(placeholder.c_str());
                playlist.at(playlist.size() - 1).Title = "Now Playing: " + placeholder.substr(14);
            }
            UnloadDirectoryFiles(musicInDirectory);
            for (song &i : playlist)
            {
                i.music.looping = 0;
            }
            currentlyPlayingIndex = (rand() % playlist.size());
            activeMusic = &playlist.at(currentlyPlayingIndex).music;
        }
        void update(platformer::animatedText &aniText, double &time)
        {
            UpdateMusicStream(*activeMusic);
            if (GetMusicTimePlayed(*activeMusic) >= 0.999 * GetMusicTimeLength(*activeMusic))
            {
                StopMusicStream(*activeMusic);
                currentlyPlayingIndex++;
                currentlyPlayingIndex = currentlyPlayingIndex % playlist.size();
                activeMusic = &playlist.at(currentlyPlayingIndex).music;
                PlayMusicStream(*activeMusic);
                aniText.setContent(playlist.at(currentlyPlayingIndex).Title.c_str());
                aniText.revive(time, 10);
            }
        }
        void skip(platformer::animatedText &aniText, double &time)
        {
            StopMusicStream(*activeMusic);
            currentlyPlayingIndex++;
            currentlyPlayingIndex = currentlyPlayingIndex % playlist.size();
            activeMusic = &playlist.at(currentlyPlayingIndex).music;
            PlayMusicStream(*activeMusic);
            aniText.setContent(playlist.at(currentlyPlayingIndex).Title.c_str());
            aniText.revive(time, 10);
        }
        void release()
        {
            for (song &i : playlist)
            {
                UnloadMusicStream(i.music);
            }
        }
    }
    class console
    {
    protected:
        Vector2 positionToStartDrawing;
        Vector2 positionToDrawFPS;
        std::string cin;
        std::vector<std::string> arguments;
        std::array<float, 100> frameTimes;
        bool isInConsole;
        bool fpsIsVisible{0};
        int indexToEdit{0};
    private:
        // These pointers are needed since this class needs access to everything
        Vector2 * windowResolution = nullptr;
        Vector2 * mousePosition = nullptr;
        float * hypotenuse = nullptr;
        wchar_t * activeKeypress = nullptr;
        std::string * levelFilename = nullptr;
        platformer::animatedText * aniText = nullptr;
        double * currentTime = nullptr;
        platformer::player * player = nullptr;
    public:
        void assignPointers(Vector2 * winRes, Vector2 * mousePos, float * hypo, wchar_t * keypress, std::string * filename, platformer::animatedText * animatedText, double * time, platformer::player * play)
        {
            windowResolution = winRes;
            mousePosition = mousePos;
            hypotenuse = hypo;
            activeKeypress = keypress;
            levelFilename = filename;
            aniText = animatedText;
            currentTime = time;
            player = play;
        }
        int draw()
        {
            int returnVal = 0;
            if (fpsIsVisible)
            {
                frameTimes[indexToEdit] = GetFrameTime();
                indexToEdit++;
                indexToEdit = indexToEdit % 100;
                float sum{0};
                for (int i = 0; i < 100; i++)
                {
                    sum += frameTimes[indexToEdit];
                }
                float mean = sum / 100.0f;
                for (int i = 0; i < 50; i++)
                {
                    DrawLine(i, ((frameTimes[i] / mean) * 10) + windowResolution->y / 2, i + 1, ((frameTimes[i + 1] / mean) * 10) + windowResolution->y / 2, YELLOW);
                }
                DrawText(TextFormat("FPS: %d", (int)(1.0f / mean)), positionToDrawFPS.x * windowResolution->x, positionToDrawFPS.y * windowResolution->y, 0.01f * (*hypotenuse), YELLOW);
                // DrawText(TextFormat("stddvn: %f", stdv), positionToDrawFPS.x * windowResolution.x, (0.1f + positionToDrawFPS.y) * windowResolution.y, 0.01f * hypotenuse, YELLOW);
            }
            if (isInConsole)
            {
                if ((*activeKeypress) != 0 && (*activeKeypress) >= 32 && (*activeKeypress) <= 122)
                {
                    cin += (*activeKeypress);
                }
                if (IsKeyPressed(KEY_BACKSPACE) && cin.size() > 0)
                {
                    cin.pop_back();
                }
                if (IsKeyPressed(KEY_ENTER))
                {
                    int returnVal{0};
                    arguments.clear();
                    std::string buffer;
                    std::stringstream otherCin(cin);
                    while (std::getline(otherCin, buffer, ' '))
                    {
                        arguments.push_back(buffer);
                    }
                    try
                    {
                        if (arguments.at(0) == "/showfps")
                        {
                            fpsIsVisible = !fpsIsVisible;
                            throw std::invalid_argument("FPS counter toggled.");
                        }
                        if (arguments.at(0) == "/load")
                        {
                            std::string flnm = "levels/" + arguments.at(1);
                            if (FileExists(flnm.c_str()))
                            {
                                *levelFilename = arguments.at(1);
                                return -1;
                            }
                            else
                            {
                                throw std::invalid_argument("Level " + arguments.at(1) + " does not exist!");
                            }
                        }
                        if (arguments.at(0) == "/fullscreen")
                        {
                            ToggleFullscreen();
                            throw std::invalid_argument("Fullscreen toggled");
                        }
                        if (arguments.at(0) == "/reset")
                        {
                            if (arguments.at(1) == "savedata")
                            {
                                remove(".savedata");
                                throw std::invalid_argument("Deleted save data");
                            }
                            if (arguments.at(1) == "level")
                            {
                                return -1;
                            }
                        }
                        if (arguments.at(0) == "/set")
                        {
                            if (arguments.at(1) == "fps" && arguments.size() > 2)
                            {
                                SetTargetFPS(std::stoi(arguments.at(2)));
                                throw std::invalid_argument("FPS capped to " + arguments.at(2) + "FPS");
                            }
                            if (arguments.at(1) == "volume" && arguments.size() > 2)
                            {
                                SetMasterVolume(std::stof(arguments.at(2)));
                                throw std::invalid_argument("Set Volume to " + arguments.at(2));
                            }
                        }
                        if (arguments.at(0) == "/skip")
                        {
                            if (arguments.at(1) == "music")
                            {
                                platformer::music::skip(*aniText, *currentTime);
                            }
                        }
                        if (arguments.at(0) == "/move")
                        {
                            Vector2 gamePos = GetScreenToWorld2D(*mousePosition, platformer::blocks::inGameCamera);
                            player->setPosition(gamePos.x, gamePos.y);
                            throw std::invalid_argument("Player teleported to " + std::to_string(gamePos.x) + ' ' + std::to_string(gamePos.y));
                        }
                    }
                    catch (const std::exception &e)
                    {
                        std::cerr << e.what() << '\n';
                        aniText->setContent(e.what());
                        aniText->revive(*currentTime, 3);
                    }
                    toggleConsole();
                    cin.clear();
                }
                DrawText(cin.c_str(), (positionToStartDrawing.x * windowResolution->x), (positionToStartDrawing.y * windowResolution->y), (*hypotenuse) * 0.01f, YELLOW);
            }
            return returnVal;
        }
        console()
        {
            positionToStartDrawing = {0.1f, 0.8f};
            positionToDrawFPS = {0.1f, 0.1f};
            isInConsole = 0;
        }
        void toggleConsole()
        {
            isInConsole = !isInConsole;
        }
    };
}