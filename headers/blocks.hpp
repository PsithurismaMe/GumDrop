#pragma once
#include "classes.hpp"

namespace platformer
{
    namespace blocks
    {
        stationaryStaticBlock grass; // 1
        stationaryStaticBlock dirt; // 2
        stationaryStaticBlock brick; // 3
        stationaryAnimatedBlock laser; // 4
        stationaryAnimatedBlock lava; // 5
        player templatePlayer;
        Camera2D inGameCamera;
        void init()
        {
            grass.setPositionOnSpriteSheet({0, 0, 64, 64});
            dirt.setPositionOnSpriteSheet({64, 0, 64, 64});
            brick.setPositionOnSpriteSheet({128, 0, 64, 64});
            laser.setInitialPositionOnSpriteSheet({0, 1792, 64, 64});
            laser.setPixelsToOffset(64, 0);
            laser.setMaxFrames(2);
            lava.setInitialPositionOnSpriteSheet({0, 1856, 64, 64});
            lava.setPixelsToOffset(20, 0);
            lava.setMaxFrames(6);
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
            templatePlayer.setInitialPositionOnSpriteSheet({0, 1984, 64, 64});
            templatePlayer.setPixelsToOffset(64, 0);
            templatePlayer.setMaxFrames(5);
            inGameCamera.offset = {400, 200};
            inGameCamera.rotation = 0;
            inGameCamera.zoom = 1;

        }
        void incrementEveryMilliseconds(size_t &iterator, bool &workerLife, int ms)
        {
            while (workerLife)
            {
                iterator++;
                std::this_thread::sleep_for(std::chrono::milliseconds(ms));
            }
        }
        void Every16Milliseconds(std::vector<platformer::stationaryStaticBlock *> & staticBlocks, std::vector<platformer::stationaryAnimatedBlock *> & animatedBlocks, player & pplayer,bool & workerStatus, std::array<int, 3> & activeKeypresses)
        {
            while (workerStatus)
            {
                std::chrono::_V2::system_clock::time_point estimatedCompletionTime = std::chrono::system_clock::now() + std::chrono::milliseconds(16);
                pplayer.doPhysicsStep(staticBlocks, animatedBlocks, 0.01666f);
                if (activeKeypresses[0])
                {
                    pplayer.incrementDesiredMovement(pplayer.getSpeed(), 0);
                    pplayer.setFaceDirection(64);
                    activeKeypresses[0] = 0;
                }
                if (activeKeypresses[1])
                {
                    pplayer.decrementDesiredMovement(pplayer.getSpeed(), 0);
                    pplayer.setFaceDirection(0);
                    activeKeypresses[1] = 0;
                }
                if (std::chrono::system_clock::now() < estimatedCompletionTime)
                {
                    std::this_thread::sleep_until(estimatedCompletionTime);
                }
                else
                {
                    std::cerr << "Physics thread cannot keep up! Physics will be innaccurate." << '\n';
                }
            }
        }
        namespace blockFunctions
        {
            void laserBeams(std::vector<platformer::stationaryAnimatedBlock *> &lasers, bool &status)
            {
                while (status)
                {
                    
                }
            }
        }
        void loadFromFile(const char *filename, std::vector<platformer::stationaryStaticBlock *> &dest, std::vector<platformer::stationaryAnimatedBlock *> &aDest)
        {
            if (FileExists(filename))
            {
                char *readme = LoadFileText(filename);
                int width;
                // Get the width
                for (width = 0; readme[width] != '\n' && readme[width] != '\0'; width++)
                {
                }
                int x = 0;
                int y = 0;
                dest.clear();
                aDest.clear();
                // To do : serialize more blocks
                for (size_t i = 0; readme[i] != '\0'; i++)
                {
                    switch (readme[i])
                    {
                    case ('.'):
                        x++;
                        break;
                    case ('\n'):
                        y++;
                        x = 0;
                        break;
                    case ('M'):
                        aDest.push_back(new platformer::stationaryAnimatedBlock(lava, 64 * x, 64 * y, 64, 64, nullptr));
                        x++;
                        break;
                    case ('L'):
                        aDest.push_back(new platformer::stationaryAnimatedBlock(laser, 64 * x, 64 * y, 64, 64, nullptr));
                        x++;
                        break;
                    case ('B'):
                        dest.push_back(new platformer::stationaryStaticBlock(brick, 64 * x, 64 * y, 64, 64));
                        x++;
                        break;
                    case ('D'):
                        dest.push_back(new platformer::stationaryStaticBlock(dirt, 64 * x, 64 * y, 64, 64));
                        x++;
                        break;
                    case ('G'):
                        dest.push_back(new platformer::stationaryStaticBlock(grass, 64 * x, 64 * y, 64, 64));
                        x++;
                        break;
                    case ('S'):
                        templatePlayer.setPosition(64 * x, 64 * y);
                        x++;
                        break;
                    default:
                        x++;
                        break;
                    }
                }
                UnloadFileText(readme);
            }
        }
    }
}