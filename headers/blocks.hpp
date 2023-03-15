#pragma once
#include "classes.hpp"

namespace platformer
{
    namespace blocks
    {
        stationaryStaticBlock grass;
        stationaryStaticBlock dirt;
        stationaryStaticBlock brick;
        stationaryAnimatedBlock laser;
        stationaryAnimatedBlock lava;
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
        }
        void incrementEveryMilliseconds(size_t & iterator, bool & workerLife, int ms)
        {
            while (workerLife)
            {
                iterator++;
                std::this_thread::sleep_for(std::chrono::milliseconds(ms));
            }
        }
    }
}