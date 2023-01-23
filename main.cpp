/*
This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or any later version.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

#include <iostream>
#include <raylib.h>
#include <vector>
#include <thread>


class intVector2
{
	public:
		int x {0};
		int y {0};
};

class block
{
	public:
		Rectangle source; // This cooresponds to the rectangle in tilesheet.png
		Vector2 position; // This is world position
		bool ignoreCollision {0};
		void beDrawn(Texture2D sourceImage, Vector2 origin, float rotation) // This function should only be called for blocks
		{
			DrawTexturePro(sourceImage, source, {position.x, position.y, 64, 64}, origin, rotation, WHITE);
		}
};

class player : public block
{
	public:
		int isFacingLeft {0}; // Set to 64 to face right
		int speed {8}; // MUST BE A MULTIPLE OF 64
		int threadTerminator {0};
		Vector2 userForces {0, 0};
		bool canJump {1};
		void beDrawn(Texture2D sourceImage, Vector2 origin, float rotation)
		{
			DrawTexturePro(sourceImage, {source.x + isFacingLeft, source.y, source.width, source.height}, {position.x, position.y, 64, 64}, origin, rotation, WHITE);
		}
		void attemptMovement(Vector2 direction, std::vector<block> & thingsToCheck)
		{
			bool willCollide{0};
			for (size_t i = 0; i < thingsToCheck.size() && !willCollide; i++)
			{
				willCollide = CheckCollisionRecs({position.x + direction.x, position.y + direction.y, 64, 64}, {thingsToCheck.at(i).position.x, thingsToCheck.at(i).position.y, 64, 64});
				if (willCollide)
				{
					willCollide = !thingsToCheck.at(i).ignoreCollision;
				}
			}
			if (!willCollide)
			{
				position.x += direction.x;
			}
		}
		void beginGravity(std::vector<block> * thingsToCheck)
		{
			while (!threadTerminator)
			{
				bool willCollide{0};
				Vector2 gravityAccel {0, 0};
				gravityAccel.y += 1;
				gravityAccel.y -= userForces.y;
				for (size_t i = 0; i < thingsToCheck->size() && !willCollide; i++)
				{
					willCollide = CheckCollisionRecs({position.x + gravityAccel.x, position.y + gravityAccel.y, 64, 64}, {thingsToCheck->at(i).position.x, thingsToCheck->at(i).position.y, 64, 64});
					if (willCollide)
					{
						canJump = 1;
						willCollide = !thingsToCheck->at(i).ignoreCollision;
					}
				}
				if (!willCollide)
				{
					position.y += gravityAccel.y;
				}
				std::this_thread::sleep_for(std::chrono::milliseconds(16));
				userForces.y /= 2;
			}
		}
};

class level
{
	public:
		intVector2 numberOfBlocks;
		std::vector<block> allBlocks;
		Vector2 playerInitalPos {0, -128};
		level(intVector2 levelDimentions)
		{
			numberOfBlocks = levelDimentions;
			allBlocks.reserve(numberOfBlocks.x * numberOfBlocks.y);
			for (int i = 0; i < (numberOfBlocks.x * numberOfBlocks.y); i++)
			{
				allBlocks.push_back(block());

			}
		}
		void createTestLevel()
		{
			for (int i = 0; i < (numberOfBlocks.x * numberOfBlocks.y); i++)
			{
				allBlocks.at(i).ignoreCollision = 0;
				allBlocks.at(i).source = {128, 0, 64, 64};
				allBlocks.at(i).position = {(i % numberOfBlocks.x)*64.0f, (i / numberOfBlocks.x)*64.0f};
			}
		}
		void drawAll(Texture2D sourceImage)
		{
			for (block i : allBlocks)
			{
				if (i.source.width != 0)
				{
					i.beDrawn(sourceImage, {0, 0}, 0);
				}
			}
		}

};


int main()
{
	InitWindow(800, 400, "A Platformer");
	SetWindowState(FLAG_WINDOW_RESIZABLE); // Allow the window to be resized
	Vector2 screenDimentions; 
	Camera2D mainCamera;
	Texture2D sourceImage = LoadTexture("assets/tilesheet.png"); // Tilesheet.png
	mainCamera.offset = {400, 200}; // Default origion
	mainCamera.rotation = 0;
	mainCamera.zoom = 1;
	// std::vector<block> allBlocks;
	
	// block myPurpleBlock;
	//myPurpleBlock.source = {128, 0, 64, 64};
	//myPurpleBlock.position = {0, 512};
	player me;
	me.position = {128, 128};
	me.source = {0, (2048-64), 64, 64};
	level myLevel({11, 4});
	myLevel.createTestLevel();
	//allBlocks.push_back(myPurpleBlock);
	me.position = myLevel.playerInitalPos;
	std::thread physics(&player::beginGravity, &me, &myLevel.allBlocks);
	while (!WindowShouldClose())
	{
		screenDimentions.x = GetRenderWidth();
		screenDimentions.y = GetRenderHeight();
		mainCamera.offset = {(screenDimentions.x / 2), (screenDimentions.y / 2)};
		mainCamera.target = me.position;
		BeginDrawing();
		BeginMode2D(mainCamera);
		ClearBackground(BLACK);
		//myPurpleBlock.beDrawn(sourceImage, {0, 0}, 0);
		myLevel.drawAll(sourceImage);
		me.beDrawn(sourceImage, {0, 0}, 0);
		EndMode2D();
		EndDrawing();
		int response = GetCharPressed();
		
		switch (response)
		{
		case 'w':
			//me.attemptMovement({0, -1.0f*(me.speed)}, myLevel.allBlocks);
			break;
		case 's':
			//me.attemptMovement({0, (float) me.speed}, myLevel.allBlocks);
			//me.userForces.x -= 64;
			break;
		case 'a':
			me.attemptMovement({-1.0f*(me.speed), 0}, myLevel.allBlocks);
			//me.userForces.x -= 64;
			me.isFacingLeft = 64;
			break;
		case 'd':
			me.attemptMovement({(float) me.speed, 0}, myLevel.allBlocks);
			//me.userForces.x += 64;
			me.isFacingLeft = 0;
			break;
		case ' ':
			me.userForces.y += 32 * me.canJump;
			me.canJump = 0;
			break;
		
		default:
			break;
		}
	}
	me.threadTerminator = 1;
	physics.join();
	UnloadTexture(sourceImage);
	CloseWindow();
}