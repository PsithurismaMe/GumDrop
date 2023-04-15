#pragma once
#include <iostream>
#include <raylib.h>
#include <vector>
#include <thread>
#include <mutex>
#include <string>
#include <cmath>
#include <sstream>
#include <fstream>
#include <functional>

namespace platformer
{
    // All objects that are collidable must be derived from this parent class
    // No collidable should be deleted or added UNTIL ALL THREADS are done accessing it

    // When accessing a blocks type, use these enum values
    enum valuesOfBlocks
    {
        Grass = 47,
        Dirt,
        Brick,
        LaserNoTimeOffset,
        Lava,
        PlayerSpawn,
        Portal,
    };
    Vector2 rotatePointAroundOtherPoint(Vector2 origionalPoint, Vector2 pointToRotateAround, float degreesToRotate)
    {
        Vector2 transformedPoint;
        transformedPoint.x = ((std::cos(degreesToRotate * PI / 180.0f) * (origionalPoint.x - pointToRotateAround.x)) + ((-1.0f * std::sin(degreesToRotate * PI / 180.0f)) * (origionalPoint.y - pointToRotateAround.y))) + pointToRotateAround.x;
        transformedPoint.y = (std::sin(degreesToRotate * PI / 180.0f) * (origionalPoint.x - pointToRotateAround.x)) + (std::cos(degreesToRotate * PI / 180.0f) * (origionalPoint.y - pointToRotateAround.y)) + pointToRotateAround.y;
        return transformedPoint;
    }
    // This function is bugged. Do not use
    int writeCompressedData(std::stringstream &uncompressedString, const char *filename)
    {
        int returnVal = 0;
        std::fstream output(filename, std::ios::trunc | std::ios::out);
        if (!output.is_open())
        {
            returnVal = -1;
        }
        else
        {
            int outputSize;
            unsigned char *compressedData = CompressData((const unsigned char *)uncompressedString.str().c_str(), sizeof(unsigned char) * (uncompressedString.str().length() - 1), &outputSize);
            output.write((const char *)compressedData, outputSize);
            output.close();
            MemFree(compressedData);
            returnVal = 0;
        }
        return returnVal;
    }
    class animatedText
    {
    protected:
        std::string content;
        float timeToLive;
        double creationTime;
        Vector2 destination;

    public:
        animatedText()
        {
        }
        // Sets the destination coordinates
        void setDestination(float x, float y)
        {
            destination.x = x;
            destination.y = y;
        }
        // Sets the string contents
        void setContent(const char *cnt)
        {
            content = cnt;
        }
        // Resets the timer. Time to live is in seconds
        void revive(double timeOfBirth, float ttl)
        {
            creationTime = timeOfBirth;
            timeToLive = ttl;
        }
        void draw(float &c, double time, float fontsize, Vector2 &resolution)
        {
            if ((creationTime + timeToLive) > time)
            {
                DrawText(content.c_str(), (destination.x * resolution.x), (destination.y * resolution.y), fontsize * c, ORANGE);
            }
        }
    };
    // This function is bugged. Do not use
    std::stringstream readCompressedData(const char *filename)
    {
        std::string buffer;
        std::stringstream thingToReturn;
        int fileSize = GetFileLength(filename);
        char *source = LoadFileText(filename);
        if (source == nullptr)
        {
            return thingToReturn;
        }
        else
        {
            int length;
            int dummy;
            for (size_t i = 0; source[i] != '\0'; i++)
            {
                buffer += source[i];
                length = i;
            }
            unsigned char *uncompressedData = DecompressData((const unsigned char *)source, fileSize, &dummy);
            thingToReturn << uncompressedData;
            MemFree(uncompressedData);
        }
        UnloadFileText(source);
        return thingToReturn;
    }
    class collidable
    {
    protected:
        Rectangle inGamePositionDimension{0, 0, 64, 64};
        bool collisionEnabled;
        int groupNumber;
        bool isVisibleToPlayer{0};
        int rotation{0};

    public:
        int getRotation()
        {
            return rotation;
        }
        void setRotation(int deg)
        {
            rotation = deg;
        }
        void setVisibility(bool val)
        {
            isVisibleToPlayer = val;
        }
        bool getVisibility()
        {
            return isVisibleToPlayer;
        }
        // Yet to be implemented
        void updateGroupNumber(void *ptr)
        {
        }
        // So far this number does nothing
        void setGroupNumber(int gn)
        {
            groupNumber = gn;
        }
        // So far this number does nothing
        int getGroupNumber()
        {
            return groupNumber;
        }
        // This position is in game world space
        void setPosition(float x, float y)
        {
            inGamePositionDimension.x = x;
            inGamePositionDimension.y = y;
        }
        // These dimentions are for world space
        void setDimentions(float width, float height)
        {
            inGamePositionDimension.width = width;
            inGamePositionDimension.height = height;
        }
        // This position is in game world space
        Vector2 getPosition()
        {
            return {inGamePositionDimension.x, inGamePositionDimension.y};
        }
        // These dimentions are for world space
        Vector2 getDimentions()
        {
            return {inGamePositionDimension.width, inGamePositionDimension.height};
        }
        // Returns the in game position plus width and height
        Rectangle getRectangle()
        {
            return inGamePositionDimension;
        }
        void setInGameRectangle(Rectangle rect)
        {
            inGamePositionDimension = rect;
        }
        collidable()
        {
            inGamePositionDimension = {0.0f, 0.0f, 0.0f, 0.0f};
            collisionEnabled = 1;
            groupNumber = rand();
        }
    };
    class stationaryStaticBlock : public collidable
    {
    protected:
        Rectangle positionOnSpriteSheet;
        int type;

    public:
        // Sets the portion of spritesheet.png to use as a texture
        void setPositionOnSpriteSheet(Rectangle rect)
        {
            positionOnSpriteSheet = rect;
        }
        // Returns the portion of spritesheet.png used as a texture
        Rectangle getPositionOnSpriteSheet()
        {
            return positionOnSpriteSheet;
        }
        // Draws this object to the screen
        void draw(Texture2D &spritesheet)
        {
            DrawTexturePro(spritesheet, positionOnSpriteSheet, {inGamePositionDimension.x + inGamePositionDimension.width / 2, inGamePositionDimension.y + inGamePositionDimension.height / 2, inGamePositionDimension.width, inGamePositionDimension.height}, {inGamePositionDimension.width / 2, inGamePositionDimension.height / 2}, rotation, WHITE);
        }
        stationaryStaticBlock()
        {
        }
        stationaryStaticBlock(stationaryStaticBlock &whereToInherit, int globalx, int globaly, int wid, int hgt, int rot)
        {
            (*this) = whereToInherit;
            inGamePositionDimension.x = globalx;
            inGamePositionDimension.y = globaly;
            inGamePositionDimension.width = wid;
            inGamePositionDimension.height = hgt;
            rotation = rot;
        }
        void setType(int t)
        {
            type = t;
        }
        int getType()
        {
            return type;
        }
    };
    class stationaryAnimatedBlock : public collidable
    {
    protected:
        Rectangle initialPositionOnSpriteSheet;
        Vector2 pixelsToOffsetUponUpdate;
        int maximumFrames = 2;
        int frameToDisplay{0};
        size_t *iterable = nullptr;
        int type;
        int rayLength{0};
        Vector2 beginOfRay;
        Vector2 endOfRay;

    public:
        // Computes the max distance a laser beam will travel. Gives up if it exceeds 4096 pixels
        void computeRay(std::vector<stationaryStaticBlock *> obstecules)
        {
            Vector2 origion;
            int lowest{4096};
            int halfSpriteWidth = inGamePositionDimension.width / 2;
            Vector2 p1 {inGamePositionDimension.x + halfSpriteWidth, inGamePositionDimension.y + halfSpriteWidth};
            Vector2 p2 {inGamePositionDimension.x + lowest, inGamePositionDimension.y + halfSpriteWidth};
            origion.x = inGamePositionDimension.x + halfSpriteWidth;
            origion.y = inGamePositionDimension.y + halfSpriteWidth;
            p1 = platformer::rotatePointAroundOtherPoint(p1, origion, rotation);
            p2 = platformer::rotatePointAroundOtherPoint(p2, origion, rotation);
            beginOfRay = p1;
            endOfRay = p2;
            for (size_t i = 0; i < obstecules.size(); i++)
            {
                Vector2 cache = obstecules.at(i)->getPosition();
                cache.x += halfSpriteWidth;
                cache.y += halfSpriteWidth;
                if (CheckCollisionPointLine(cache, p1, p2, halfSpriteWidth + 1))
                {
                    float placeholder = sqrt(pow((cache.x) - p1.x, 2) + pow((cache.y) - p1.y, 2));
                    if (placeholder < lowest)
                    {
                        lowest = placeholder;
                        endOfRay = cache;
                    }
                }
            }
            rayLength = lowest;
        }
        // Returns the distance a laser beam will travel. This should be 0 for anything else
        int getRayLength()
        {
            return rayLength;
        }
        Vector2 getRayBegin()
        {
            return beginOfRay;
        }
        Vector2 getRayEnd()
        {
            return endOfRay;
        }
        // Sets the initial position of spritesheet.png to use as a texture
        void setInitialPositionOnSpriteSheet(Rectangle rect)
        {
            initialPositionOnSpriteSheet = rect;
        }
        // (Position of spritesheet.png used as a texure) = initialPositionOnSpriteSheet + (frameToDisplay * pixelsToOffset)
        void setPixelsToOffset(int dx, int dy)
        {
            pixelsToOffsetUponUpdate.x = dx;
            pixelsToOffsetUponUpdate.y = dy;
        }
        // Maximum value that frameToDisplay will reach
        void setMaxFrames(int f)
        {
            maximumFrames = f;
        }
        // This must be set for animation to work. The dereferenced size_t must be changing constantly.
        void setIterablePointer(size_t *ptr)
        {
            iterable = ptr;
        }
        // Returns the initial position of spritesheet.png to use as a texture
        Rectangle getInitialSpritePosition()
        {
            return initialPositionOnSpriteSheet;
        }
        // Returns the currently displayed frame
        int getFrameDisplayed()
        {
            return frameToDisplay;
        }
        // Draws self to screen
        void draw(Texture2D &spritesheet)
        {
            // Iterable must be assigned to a size_t before this can be drawn
            if (iterable != nullptr)
            {
                frameToDisplay = (*iterable % maximumFrames);
                DrawTexturePro(spritesheet, {(frameToDisplay * pixelsToOffsetUponUpdate.x) + initialPositionOnSpriteSheet.x, (frameToDisplay * pixelsToOffsetUponUpdate.y) + initialPositionOnSpriteSheet.y, initialPositionOnSpriteSheet.width, initialPositionOnSpriteSheet.height}, {inGamePositionDimension.x + inGamePositionDimension.width / 2, inGamePositionDimension.y + inGamePositionDimension.height / 2, inGamePositionDimension.width, inGamePositionDimension.height}, {inGamePositionDimension.width / 2, inGamePositionDimension.height / 2}, rotation, WHITE);
            }
            else
            {
                std::cerr << "Object is missing iterable" << '\n';
            }
        }
        stationaryAnimatedBlock()
        {
        }
        stationaryAnimatedBlock(stationaryAnimatedBlock &whereToInherit, int globalx, int globaly, int wid, int hgt, size_t *i, int rot)
        {
            (*this) = whereToInherit;
            inGamePositionDimension.x = globalx;
            inGamePositionDimension.y = globaly;
            inGamePositionDimension.width = wid;
            inGamePositionDimension.height = hgt;
            iterable = i;
            rotation = rot;
        }
        void setType(int t)
        {
            type = t;
        }
        int getType()
        {
            return type;
        }
    };
    class npc : public stationaryAnimatedBlock
    {
    protected:
    };
    class player : public stationaryAnimatedBlock
    {
    protected:
        int isFacingLeft{0}; // Set to 64 to face right
        int speed{64};
        Vector2 velocity;
        Vector2 initialPosition;
        Vector2 terminalVelocity{256.0f, 512.0f};
        Vector2 dragCoefficent{100.0f, 128.0f};
        Vector2 checkpoint;
        bool canJump{1};
        bool canFastFall{0};
        int isMoving{0};
        Vector2 playerDesiredMovement;
        bool reloadLevel{0};

    public:
        void setReloadStatus(bool s)
        {
            reloadLevel = s;
        }
        bool getReloadStatus()
        {
            return reloadLevel;
        }
        void setDesiredMovement(float x, float y)
        {
            playerDesiredMovement.x = x;
            playerDesiredMovement.y = y;
        }
        void incrementDesiredMovement(float x, float y)
        {
            playerDesiredMovement.x += x;
            playerDesiredMovement.y += y;
        }
        void decrementDesiredMovement(float x, float y)
        {
            playerDesiredMovement.x -= x;
            playerDesiredMovement.y -= y;
        }
        void draw(Texture2D &spritesheet)
        {
            if (iterable != nullptr)
            {
                frameToDisplay = (*iterable % maximumFrames);
                DrawTexturePro(spritesheet, {(frameToDisplay * pixelsToOffsetUponUpdate.x) + initialPositionOnSpriteSheet.x, (frameToDisplay * pixelsToOffsetUponUpdate.y) + initialPositionOnSpriteSheet.y - isFacingLeft, initialPositionOnSpriteSheet.width, initialPositionOnSpriteSheet.height}, inGamePositionDimension, {0, 0}, 0, WHITE);
            }
            else
            {
                std::cerr << "Object is missing iterable" << '\n';
            }
        }
        player()
        {
            velocity = {0.0f, 0.0f};
            playerDesiredMovement = {0.0f, 0.0f};
            inGamePositionDimension.width = 64;
            inGamePositionDimension.height = 64;
        }
        player(int spd, int tmvX, int tmvY, int drgcX, int drgcY)
        {
            speed = spd;
            velocity = {0.0f, 0.0f};
            playerDesiredMovement = {0.0f, 0.0f};
            terminalVelocity.x = tmvX;
            terminalVelocity.y = tmvY;
            dragCoefficent.x = drgcX;
            dragCoefficent.y = drgcY;
        }
        // Set val to be 64 to face right
        void setFaceDirection(char val)
        {
            isFacingLeft = val;
        }
        int getSpeed()
        {
            return speed;
        }
        Rectangle getPredictedPosition(float timeDelta, int xAxisOverride, int yAxisOverride)
        {
            // This is slightly smaller than the actual sprite because floating point approximation limitations
            return {(inGamePositionDimension.x) + (velocity.x * timeDelta * xAxisOverride), (inGamePositionDimension.y + 23) + (velocity.y * timeDelta * yAxisOverride), 63, 41};
        }
        void doPhysicsStep(std::vector<stationaryStaticBlock *> &staticBlocks, std::vector<stationaryAnimatedBlock *> &animatedBlocks, float frameDelta, std::string &file)
        {
            velocity.y += 1 * dragCoefficent.y * frameDelta;
            velocity.x > 0 ? velocity.x -= 1 *dragCoefficent.x *frameDelta : velocity.x += 1 * dragCoefficent.x * frameDelta;
            velocity.x += playerDesiredMovement.x;
            velocity.y += playerDesiredMovement.y;
            if (velocity.x > terminalVelocity.x)
            {
                velocity.x = terminalVelocity.x;
            }
            if (velocity.y > terminalVelocity.y)
            {
                velocity.y = terminalVelocity.y;
            }
            if (velocity.x < terminalVelocity.x * -1)
            {
                velocity.x = terminalVelocity.x * -1;
            }
            if (velocity.y < terminalVelocity.y * -1)
            {
                velocity.y = terminalVelocity.y * -1;
            }
            playerDesiredMovement = {0, 0};
            bool xAxisWillCollide;
            bool yAxisWillCollide;
            bool cWillCollide;
            bool deadlyWillCollide;
            int indexes[3] = {-1, -1, -1};
            for (size_t i = 0; i < staticBlocks.size() && indexes[0] == -1; i++)
            {
                if (staticBlocks.at(i)->getVisibility())
                {
                    xAxisWillCollide = CheckCollisionRecs(getPredictedPosition(frameDelta, 1, 0), staticBlocks.at(i)->getRectangle());
                    if (xAxisWillCollide)
                    {
                        indexes[0] = i;
                        break;
                    }
                }
            }
            for (size_t i = 0; i < staticBlocks.size() && indexes[1] == -1; i++)
            {
                if (staticBlocks.at(i)->getVisibility())
                {
                    yAxisWillCollide = CheckCollisionRecs(getPredictedPosition(frameDelta, 0, 1), staticBlocks.at(i)->getRectangle());
                    if (yAxisWillCollide)
                    {
                        break;
                    }
                }
            }
            for (size_t i = 0; i < staticBlocks.size() && indexes[2] == -1; i++)
            {
                if (staticBlocks.at(i)->getVisibility())
                {
                    cWillCollide = CheckCollisionRecs(getPredictedPosition(frameDelta, 1, 1), staticBlocks.at(i)->getRectangle());
                    if (cWillCollide)
                    {
                        indexes[2] = i;
                        break;
                    }
                }
            }
            for (int i = 0; i < animatedBlocks.size(); i++)
            {
                if (animatedBlocks.at(i)->getType() == valuesOfBlocks::LaserNoTimeOffset && animatedBlocks.at(i)->getFrameDisplayed() == 1)
                {
                    Rectangle cache = animatedBlocks.at(i)->getRectangle();
                    deadlyWillCollide = CheckCollisionPointLine({getPredictedPosition(frameDelta, 1, 1).x + 32, getPredictedPosition(frameDelta, 1, 1).y}, animatedBlocks.at(i)->getRayBegin(), animatedBlocks.at(i)->getRayEnd(), 5);
                    if (deadlyWillCollide)
                    {
                        inGamePositionDimension.x = checkpoint.x;
                        inGamePositionDimension.y = checkpoint.y;
                        break;
                    }
                }
                else if (animatedBlocks.at(i)->getType() == valuesOfBlocks::Lava)
                {
                    Rectangle cache = animatedBlocks.at(i)->getRectangle();
                    if (CheckCollisionRecs(getPredictedPosition(frameDelta, 1, 1), {cache.x + 4, cache.y + 4, cache.width - 8, cache.height - 8}))
                    {
                        inGamePositionDimension.x = checkpoint.x;
                        inGamePositionDimension.y = checkpoint.y;
                        break;
                    }
                }
                else if (animatedBlocks.at(i)->getType() == valuesOfBlocks::Portal)
                {
                    Rectangle cache = animatedBlocks.at(i)->getRectangle();
                    if (CheckCollisionRecs(getPredictedPosition(frameDelta, 1, 1), {cache.x + 4, cache.y + 4, cache.width - 8, cache.height - 8}))
                    {
                        int o = std::stoi(file);
                        o++;
                        file = std::to_string(o);
                        reloadLevel = 1;
                        SaveFileText(".savedata", (char *)file.c_str());
                        break;
                    }
                }
            }
            canJump = yAxisWillCollide;
            std::abs(velocity.x) > 1.7f ? velocity.x = velocity.x : velocity.x = 0;
            inGamePositionDimension.x += (velocity.x * frameDelta * !xAxisWillCollide);
            inGamePositionDimension.y += (velocity.y * frameDelta * !yAxisWillCollide);
            velocity.x = velocity.x * !xAxisWillCollide;
            velocity.y = velocity.y * !yAxisWillCollide;
        }
        void jump()
        {
            if (canJump)
            {
                playerDesiredMovement.y -= 300;
                canJump = 0;
                canFastFall = 1;
            }
        }
        void fastFall(float intensity)
        {
            if (canFastFall)
            {
                canFastFall = 0;
                playerDesiredMovement.y += intensity;
            }
        }
        bool getJumpStatus()
        {
            return canJump;
        }
        bool getFastFallStatus()
        {
            return canFastFall;
        }
        void setInitialSpawnPosition(int x, int y)
        {
            initialPosition.x = x;
            initialPosition.y = y;
        }
        void setCheckpoint(int x, int y)
        {
            checkpoint.x = x;
            checkpoint.y = y;
        }
    };
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
        int parseArguments(std::string &filename)
        {

            return 0;
        }

    public:
        int draw(Vector2 &windowResolution, float &hypotenuse, wchar_t &keypress, std::string &filename, platformer::animatedText & aniText, double & time)
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
                    DrawLine(i, ((frameTimes[i] / mean) * 10) + windowResolution.y / 2, i + 1, ((frameTimes[i + 1] / mean) * 10) + windowResolution.y / 2, YELLOW);
                }
                DrawText(TextFormat("FPS: %d", (int)(1.0f / mean)), positionToDrawFPS.x * windowResolution.x, positionToDrawFPS.y * windowResolution.y, 0.01f * hypotenuse, YELLOW);
                // DrawText(TextFormat("stddvn: %f", stdv), positionToDrawFPS.x * windowResolution.x, (0.1f + positionToDrawFPS.y) * windowResolution.y, 0.01f * hypotenuse, YELLOW);
            }
            if (isInConsole)
            {
                if (keypress != 0 && keypress >= 32 && keypress <= 122)
                {
                    cin += keypress;
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
                                filename = arguments.at(1);
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
                        }
                    }
                    catch (const std::exception &e)
                    {
                        std::cerr << e.what() << '\n';
                        aniText.setContent(e.what());
                        aniText.revive(time, 3);
                    }
                    toggleConsole();
                    cin.clear();
                }
                DrawText(cin.c_str(), (positionToStartDrawing.x * windowResolution.x), (positionToStartDrawing.y * windowResolution.y), hypotenuse * 0.01f, YELLOW);
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
    class setting
    {
    protected:
        std::string humanReadableName;
        int value{-1};

    public:
        setting(const char *name, int val)
        {
            value = val;
            humanReadableName = name;
        }
        setting()
        {
        }
        void setValue(int val)
        {
            value = val;
        }
        int getValue()
        {
            return value;
        }
        std::string getName()
        {
            return humanReadableName;
        }
        void setName(const char *name)
        {
            humanReadableName = name;
        }
        void setAll(const char *name, int val)
        {
            value = val;
            humanReadableName = name;
        }
    };
    struct saveData
    {
        int currentLevel;
    };

}