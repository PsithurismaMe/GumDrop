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
        Music * activeMusic = nullptr;
        int currentlyPlayingIndex {0};
        void init()
        {
            /*

            NOTICE: Some songs have been excluded from this git repository due to incompatible licensing.
            If you wish to use other music, AT YOUR OWN RISK DO THE FOLLOWING,

            1. Obtain a DRM free copy of the music and transcode it if nesessary to a codec supported by the [raudio raylib module](https://github.com/raysan5/raylib/blob/master/src/raudio.c)
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
            for (song & i : playlist)
            {
                i.music.looping = 0;
            }
            currentlyPlayingIndex = (rand() % playlist.size());
            activeMusic = &playlist.at(currentlyPlayingIndex).music;
        }
        void update(platformer::animatedText & aniText, double & time)
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
        void release()
        {
            for (song & i : playlist)
            {
                UnloadMusicStream(i.music);
            }
        }
    }
}