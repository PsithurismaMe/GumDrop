#pragma once
#include "classes.hpp"

namespace platformer
{
    namespace blocks
    {
        stationaryStaticBlock grass;    // 1
        stationaryStaticBlock dirt;     // 2
        stationaryStaticBlock brick;    // 3
        stationaryAnimatedBlock laser;  // 4
        stationaryAnimatedBlock lava;   // 5
        player templatePlayer;          // 6
        stationaryAnimatedBlock portal; // 7
        Camera2D inGameCamera;
        void init()
        {
            grass.setPositionOnSpriteSheet({0, 0, 64, 64});
            dirt.setPositionOnSpriteSheet({64, 0, 64, 64});
            brick.setPositionOnSpriteSheet({128, 0, 64, 64});
            laser.setInitialPositionOnSpriteSheet({0, 1792, 64, 64});
            portal.setInitialPositionOnSpriteSheet({0, 192, 64, 64});
            portal.setPixelsToOffset(64, 0);
            portal.setMaxFrames(5);
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
            grass.setType(valuesOfBlocks::Grass);
            dirt.setType(valuesOfBlocks::Dirt);
            brick.setType(valuesOfBlocks::Brick);
            laser.setType(valuesOfBlocks::LaserFacingRightNoTimeOffset);
            lava.setType(valuesOfBlocks::Lava);
            portal.setType(valuesOfBlocks::Portal);
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
        void Every16Milliseconds(std::vector<platformer::stationaryStaticBlock *> &staticBlocks, std::vector<platformer::stationaryAnimatedBlock *> &animatedBlocks, player &pplayer, bool &workerStatus, std::vector<int> &activeKeypresses, float &tickRate, std::string &file, platformer::animatedText & aniText, double & time)
        {
            while (workerStatus)
            {
                std::chrono::_V2::system_clock::time_point estimatedCompletionTime = std::chrono::system_clock::now() + std::chrono::milliseconds(16);
                pplayer.doPhysicsStep(staticBlocks, animatedBlocks, tickRate, file);
                if (activeKeypresses[0])
                {
                    pplayer.incrementDesiredMovement(pplayer.getSpeed(), 0);
                    pplayer.setFaceDirection(0);
                    activeKeypresses[0] = 0;
                }
                if (activeKeypresses[1])
                {
                    pplayer.decrementDesiredMovement(pplayer.getSpeed(), 0);
                    pplayer.setFaceDirection(64);
                    activeKeypresses[1] = 0;
                }
                if (activeKeypresses[2])
                {
                    pplayer.jump();
                }
                if (activeKeypresses[3])
                {
                    activeKeypresses[3] = 0;
                    pplayer.setCheckpoint(pplayer.getPosition().x, pplayer.getPosition().y);
                }
                if (std::chrono::system_clock::now() < estimatedCompletionTime)
                {
                    std::this_thread::sleep_until(estimatedCompletionTime);
                }
                else
                {
                    std::cerr << "WARN: SYSTEM: Physics thread cannot keep up! Physics will be innaccurate" << '\n';
                    aniText.setContent("WARN: SYSTEM: Physics thread cannot keep up! Physics will be innaccurate");
                    aniText.revive(time, 3);
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
        void loadFromFile(const char *filename, std::vector<platformer::stationaryStaticBlock *> &dest, std::vector<platformer::stationaryAnimatedBlock *> &aDest, Color &backgroundColor)
        {
            if (FileExists(filename))
            {
                std::stringstream source = platformer::readCompressedData(filename);
                dest.clear();
                aDest.clear();
                {
                    std::string placeholder;
                    std::getline(source, placeholder, '\n');
                    std::stringstream iDontKnow(placeholder);
                    std::vector<std::string> colors;
                    while (std::getline(iDontKnow, placeholder, ' '))
                    {
                        colors.push_back(placeholder);
                    }
                    backgroundColor.r = std::stoi(colors.at(0));
                    backgroundColor.g = std::stoi(colors.at(1));
                    backgroundColor.b = std::stoi(colors.at(2));
                    backgroundColor.a = std::stoi(colors.at(3));
                }
                std::string lineBuffer;
                while (std::getline(source, lineBuffer, '\n'))
                {
                    std::vector<int> parsableArguments;
                    std::stringstream iDontKnow(lineBuffer);
                    while (std::getline(iDontKnow, lineBuffer, ' '))
                    {
                        parsableArguments.push_back(std::stoi(lineBuffer));
                    }
                    if (parsableArguments.size() == 4)
                    {
                        switch (parsableArguments.at(2))
                        {
                        case (platformer::valuesOfBlocks::Grass):
                            dest.push_back(new platformer::stationaryStaticBlock(platformer::blocks::grass, parsableArguments.at(0), (parsableArguments.at(1)), 64, 64));
                            break;
                        case (platformer::valuesOfBlocks::Dirt):
                            dest.push_back(new platformer::stationaryStaticBlock(platformer::blocks::dirt, parsableArguments.at(0), (parsableArguments.at(1)), 64, 64));
                            break;
                        case (platformer::valuesOfBlocks::Brick):
                            dest.push_back(new platformer::stationaryStaticBlock(platformer::blocks::brick, parsableArguments.at(0), (parsableArguments.at(1)), 64, 64));
                            break;
                        case (platformer::valuesOfBlocks::LaserFacingRightNoTimeOffset):
                            aDest.push_back(new platformer::stationaryAnimatedBlock(platformer::blocks::laser, parsableArguments.at(0), (parsableArguments.at(1)), 64, 64, nullptr));
                            break;
                        case (platformer::valuesOfBlocks::Lava):
                            aDest.push_back(new platformer::stationaryAnimatedBlock(platformer::blocks::lava, parsableArguments.at(0), (parsableArguments.at(1)), 64, 64, nullptr));
                            break;
                        case (platformer::valuesOfBlocks::PlayerSpawn):
                            templatePlayer.setPosition(parsableArguments.at(0), parsableArguments.at(1));
                            templatePlayer.setPosition(parsableArguments.at(0), parsableArguments.at(1));
                            templatePlayer.setCheckpoint(parsableArguments.at(0), parsableArguments.at(1));
                            break;
                        case (platformer::valuesOfBlocks::Portal):
                            aDest.push_back(new platformer::stationaryAnimatedBlock(platformer::blocks::portal, parsableArguments.at(0), (parsableArguments.at(1)), 64, 64, nullptr));
                            break;
                        default:
                            break;
                        }
                    }
                }
            }
            for (int i = 0; i < aDest.size(); i++)
            {
                aDest.at(i)->computeRay(dest);
            }
        }
    }
}
