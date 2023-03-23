#include "settings.hpp"

namespace platformer
{
    namespace ui
    {
        stationaryAnimatedBlock quitButton;
        stationaryAnimatedBlock returnToGameButton;
        size_t quitButtonIsHighlighted;
        size_t returnButtonIsHighlighted;
        void init()
        {
            quitButton.setInitialPositionOnSpriteSheet({0, 64, 128, 64});
            quitButton.setPixelsToOffset(128, 0);
            quitButton.setMaxFrames(2);
            returnToGameButton.setInitialPositionOnSpriteSheet({0, 128, 128, 64});
            returnToGameButton.setPixelsToOffset(128, 0);
            returnToGameButton.setMaxFrames(2);
            quitButtonIsHighlighted = 0;
            returnButtonIsHighlighted = 0;
            quitButton.setIterablePointer(&quitButtonIsHighlighted);
            returnToGameButton.setIterablePointer(&returnButtonIsHighlighted);
        }
        void alternateRenderer(Vector2 & mousePos, Vector2 & screenDimentions, bool & isPaused, bool & isRunning, Texture2D & spritesheet, float c, float & tickRate)
        {
            tickRate = 0;
            Rectangle qtb = {(screenDimentions.x / 2), (screenDimentions.y * 0.8f), 128, 64};
            Rectangle rtg = {(screenDimentions.x / 2) - 128, (screenDimentions.y * 0.8f), 128, 64};
            platformer::ui::quitButton.setInGameRectangle(qtb);
            platformer::ui::returnToGameButton.setInGameRectangle(rtg);
            if (CheckCollisionPointRec(mousePos, qtb))
            {
                platformer::ui::quitButtonIsHighlighted = 1;
                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                {
                    isRunning = 0;
                }
            }
            else
            {
                platformer::ui::quitButtonIsHighlighted = 0;
            }
            if (CheckCollisionPointRec(mousePos, rtg))
            {
                platformer::ui::returnButtonIsHighlighted = 1;
                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                {
                    isPaused = 0;   
                    tickRate = 1.0f / 60.0f;             
                }
            }
            else
            {
                platformer::ui::returnButtonIsHighlighted = 0;
            }
            platformer::ui::quitButton.draw(spritesheet);
            platformer::ui::returnToGameButton.draw(spritesheet);
            char myText[] = "PAUSED";
            DrawText(myText, (screenDimentions.x / 2) - (MeasureText(myText, 0.1f * c) / 2), screenDimentions.y * 0.15f, 0.1f * c, GREEN); 
        }
    }
}