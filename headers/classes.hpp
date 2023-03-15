#pragma once
#include <iostream>
#include <raylib.h>
#include <vector>
#include <thread>
#include <mutex>
#include <string>
#include <cmath>
#include <sstream>

namespace platformer
{
    // All objects that are collidable must be derived from this parent class
    // Everything is in charge of drawing itself
    // No collidable should be deleted or added UNTIL ALL THREADS are done accessing it
    class collidable
    {
    protected:
        Rectangle inGamePositionDimension {0, 0, 64, 64};
        bool collisionEnabled;
        int groupNumber;
        

    public:
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
        Rectangle getRectangle()
        {
            return inGamePositionDimension;
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
    public:
        void setPositionOnSpriteSheet(Rectangle rect)
        {
            positionOnSpriteSheet = rect;
        }
        Rectangle getPositionOnSpriteSheet()
        {
            return positionOnSpriteSheet;
        }
        void draw(Texture2D & spritesheet)
        {
            DrawTexturePro(spritesheet, positionOnSpriteSheet, inGamePositionDimension, {0, 0}, 0, WHITE);
        }
    };
    class stationaryAnimatedBlock : public collidable
    {
    protected:
        Rectangle initialPositionOnSpriteSheet;
        Vector2 pixelsToOffsetUponUpdate;
        int maximumFrames = 2;
        int frameToDisplay {0};
        size_t * iterable = nullptr;
    public:
        void setInitialPositionOnSpriteSheet(Rectangle rect)
        {
            initialPositionOnSpriteSheet = rect;
        }
        void setPixelsToOffset(int dx, int dy)
        {
            pixelsToOffsetUponUpdate.x = dx;
            pixelsToOffsetUponUpdate.y = dy;
        }
        void setMaxFrames(int f)
        {
            maximumFrames = f;
        }
        void setIterablePointer(size_t * ptr)
        {
            iterable = ptr;
        }
        void draw(Texture2D & spritesheet)
        {
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
        stationaryAnimatedBlock(stationaryAnimatedBlock & whereToInherit, int globalx, int globaly, int wid, int hgt, size_t * i)
        {
            (*this) = whereToInherit;
            inGamePositionDimension.x = globalx;
            inGamePositionDimension.y = globaly;
            inGamePositionDimension.width = wid;
            inGamePositionDimension.height = hgt;
            iterable = i;
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
        int speed{1};
        Vector2 velocity;
        Vector2 terminalVelocity{256.0f, 512.0f};
        Vector2 dragCoefficent{100.0f, 128.0f};
        bool canJump{1};
        int isMoving{0};
        Vector2 playerDesiredMovement;
    public:
        player()
        {
            velocity = {0.0f, 0.0f};
            playerDesiredMovement = {0.0f, 0.0f};
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

    };
    class console
    {
    protected:
        Vector2 positionToStartDrawing;
        std::string cin;
        std::vector<std::string> arguments;
        bool isInConsole;
        void parseArguments()
        {
            arguments.clear();
            std::string buffer;
            std::stringstream otherCin(cin);
            while (std::getline(otherCin, buffer, ' '))
            {
                arguments.push_back(buffer);
            }
        }

    public:
        void draw(Vector2 &windowResolution, float &hypotenuse, wchar_t &keypress)
        {
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
                }
            }
            if (isInConsole)
            {
                DrawText(cin.c_str(), (positionToStartDrawing.x * windowResolution.x), (positionToStartDrawing.y * windowResolution.y), hypotenuse * 0.01f, BLUE);
            }
        }
        console()
        {
            positionToStartDrawing = {0.1f, 0.8f};
            isInConsole = 0;
        }
        void toggleConsole()
        {
            isInConsole = !isInConsole;
        }
    };
}