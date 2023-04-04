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
            If you wish to use Rush E by Sheet Music Boss, AT YOUR OWN RISK DO THE FOLLOWING,

            1. Obtain Rush E in a .mp3 format
            2. Rename it to "Rush E (Hard).mp3" and place it in assets/music/
            3. Uncomment the PLAYCOPYRIGHTEDMUSIC directive in classes.hpp
            4. Compile the game AND DO NOT COMMIT ANY MUSIC INCOMPATIBLE WITH GPL-V3


            */
            playlist.push_back(song());
            playlist.at(0).music = LoadMusicStream("assets/music/Arpent.mp3");
            playlist.at(0).Title = "Now Playing: Arpent by Kevin MacLeod.";
            playlist.push_back(song());
            playlist.at(1).music = LoadMusicStream("assets/music/Bit Bit Loop.mp3");
            playlist.at(1).Title = "Now Playing: Bit Bit Loop by Kevin MacLeod.";
            #ifdef PLAYCOPYRIGHTEDMUSIC
            playlist.push_back(song());
            playlist.at(2).music = LoadMusicStream("assets/music/Rush E (Hard).mp3");
            playlist.at(2).Title = "Now Playing: Rush E by Sheet Music Boss.";
            #endif
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