#pragma once
#include <iostream>
#include <raylib.h>
#include <vector>
#include <thread>
#include <mutex>
#include <string>
#include <cmath>
#include <sstream>
#include <functional>

namespace platformer
{
    // All objects that are collidable must be derived from this parent class
    // Everything is in charge of drawing itself
    // No collidable should be deleted or added UNTIL ALL THREADS are done accessing it
    class collidable
    {
    protected:
        Rectangle inGamePositionDimension{0, 0, 64, 64};
        bool collisionEnabled;
        int groupNumber;
        bool isVisibleToPlayer{0};

    public:
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
            DrawTexturePro(spritesheet, positionOnSpriteSheet, inGamePositionDimension, {0, 0}, 0, WHITE);
        }
        stationaryStaticBlock()
        {
        }
        stationaryStaticBlock(stationaryStaticBlock &whereToInherit, int globalx, int globaly, int wid, int hgt)
        {
            (*this) = whereToInherit;
            inGamePositionDimension.x = globalx;
            inGamePositionDimension.y = globaly;
            inGamePositionDimension.width = wid;
            inGamePositionDimension.height = hgt;
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

    public:
        // Computes the max distance a laser beam will travel
        void computeRay(std::vector<stationaryStaticBlock *> obstecules)
        {
            bool hasCollided{0};
            for (size_t k = 0; k < 4000 && !hasCollided; k++)
            {
                for (size_t i = 0; i < obstecules.size(); i++)
                {
                    Vector2 placeholder = {inGamePositionDimension.x, inGamePositionDimension.y};
                    if (CheckCollisionPointRec({placeholder.x + 64 + k, placeholder.y + 32}, obstecules.at(i)->getRectangle()))
                    {
                        hasCollided = 1;
                        rayLength = k;
                        break;
                    }
                }
                rayLength = k;
            }
        }
        // Returns the distance a laser beam will travel. This should be 0 for anything else
        int getRayLength()
        {
            return rayLength;
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
                DrawTexturePro(spritesheet, {(frameToDisplay * pixelsToOffsetUponUpdate.x) + initialPositionOnSpriteSheet.x, (frameToDisplay * pixelsToOffsetUponUpdate.y) + initialPositionOnSpriteSheet.y, initialPositionOnSpriteSheet.width, initialPositionOnSpriteSheet.height}, inGamePositionDimension, {0, 0}, 0, WHITE);
            }
            else
            {
                std::cerr << "Object is missing iterable" << '\n';
            }
        }
        stationaryAnimatedBlock()
        {
        }
        stationaryAnimatedBlock(stationaryAnimatedBlock &whereToInherit, int globalx, int globaly, int wid, int hgt, size_t *i)
        {
            (*this) = whereToInherit;
            inGamePositionDimension.x = globalx;
            inGamePositionDimension.y = globaly;
            inGamePositionDimension.width = wid;
            inGamePositionDimension.height = hgt;
            iterable = i;
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
        bool canJump{1};
        int isMoving{0};
        Vector2 playerDesiredMovement;

    public:
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
        void doPhysicsStep(std::vector<stationaryStaticBlock *> &staticBlocks, std::vector<stationaryAnimatedBlock *> &animatedBlocks, float frameDelta)
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
                        canJump = 1;
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
                if (animatedBlocks.at(i)->getType() == 4 && animatedBlocks.at(i)->getFrameDisplayed() == 1)
                {
                    Rectangle cache = animatedBlocks.at(i)->getRectangle();
                    deadlyWillCollide = CheckCollisionRecs(getPredictedPosition(frameDelta, 1, 1), {cache.x + 64, cache.y + 19, (float)animatedBlocks.at(i)->getRayLength(), 28});
                    if (deadlyWillCollide)
                    {
                        inGamePositionDimension.x = initialPosition.x;
                        inGamePositionDimension.y = initialPosition.y;
                        break;
                    }
                }
                else if (animatedBlocks.at(i)->getType() == 5)
                {
                    Rectangle cache = animatedBlocks.at(i)->getRectangle();
                    if (CheckCollisionRecs(getPredictedPosition(frameDelta, 1, 1), {cache.x, cache.y, cache.width - 8, cache.height - 8}))
                    {
                        inGamePositionDimension.x = initialPosition.x;
                        inGamePositionDimension.y = initialPosition.y;
                        break;
                    }
                }
            }
            std::abs(velocity.x) > 0.3f ? velocity.x = velocity.x : velocity.x = 0;
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
            }
        }
        void setInitialSpawnPosition(int x, int y)
        {
            initialPosition.x = x;
            initialPosition.y = y;
        }
    };
    class console
    {
    protected:
        Vector2 positionToStartDrawing;
        Vector2 positionToDrawFPS;
        std::string cin;
        std::vector<std::string> arguments;
        bool isInConsole;
        bool fpsIsVisible{0};
        int parseArguments(std::string &filename)
        {
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
                }
                if (arguments.at(0) == "/load")
                {
                    std::string flnm = "levels/" + arguments.at(1);
                    if (FileExists(flnm.c_str()))
                    {
                        filename = arguments.at(1);
                        return -1;
                    }
                }
            }
            catch (const std::exception &e)
            {
                std::cerr << e.what() << '\n';
            }
            return 0;
        }

    public:
        int draw(Vector2 &windowResolution, float &hypotenuse, wchar_t &keypress, std::string &filename)
        {
            int returnVal = 0;
            if (fpsIsVisible)
            {
                DrawFPS(positionToDrawFPS.x * windowResolution.x, positionToDrawFPS.y * windowResolution.y);
            }
            if (isInConsole)
            {
                if (keypress != 0)
                {
                    cin += (keypress % 255);
                }
                if (IsKeyPressed(KEY_BACKSPACE) && cin.size() > 0)
                {
                    cin.pop_back();
                }
                if (IsKeyPressed(KEY_ENTER))
                {
                    returnVal = parseArguments(filename);
                    toggleConsole();
                    cin.clear();
                }
                DrawText(cin.c_str(), (positionToStartDrawing.x * windowResolution.x), (positionToStartDrawing.y * windowResolution.y), hypotenuse * 0.01f, BLUE);
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
}