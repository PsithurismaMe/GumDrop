#include <iostream>
#include <raylib.h>
#include <vector>
#include <thread>
#include <string>
#include <cmath>
#define GRASSTEXTURE \
	{                \
		0, 0, 64, 64 \
	}
#define DIRTTEXTURE   \
	{                 \
		64, 0, 64, 64 \
	}
#define BRICKTEXTURE   \
	{                  \
		128, 0, 64, 64 \
	}

namespace game
{
	class intVector2
	{
	public:
		int x{0};
		int y{0};
		void operator=(Vector2 &source)
		{
			x = source.x;
			y = source.y;
		}
	};
	class intVector4
	{
	public:
		int r{0};
		int g{0};
		int b{0};
		int a{0};
	};
	class Screen
	{
	public:
		std::string WindowTitle = "A Platformer";
		intVector2 dimentions{800, 400};
		Camera2D camera;
		float hypotenuse;
		Texture2D spriteSheet;
		Color background;
		// int framerateCap {144};
		void updateWindow()
		{
			dimentions.x = GetRenderWidth();
			dimentions.y = GetRenderHeight();
			camera.offset.x = dimentions.x / 2;
			camera.offset.y = dimentions.y / 2;
			camera.zoom = (hypotenuse / 2202.91);
			hypotenuse = std::sqrt((dimentions.x * dimentions.x) + (dimentions.y * dimentions.y));
		}
		Screen()
		{
			InitWindow(dimentions.x, dimentions.y, WindowTitle.c_str());
			SetWindowState(FLAG_WINDOW_RESIZABLE);
			spriteSheet = LoadTexture("assets/tilesheet.png");
			camera.offset = {(float)dimentions.x / 2, (float)dimentions.y / 2};
			camera.rotation = 0;
			hypotenuse = std::sqrt((dimentions.x * dimentions.x) + (dimentions.y * dimentions.y));
			background.r = 0;
			background.g = 0;
			background.b = 30;
			background.a = 255;
			camera.target = {0, 0};
		}
		~Screen()
		{
			UnloadTexture(spriteSheet);
			CloseWindow();
		}
	};
	class Block
	{
	public:
		intVector2 absolutePos;
		Rectangle source; // This cooresponds to the rectangle in tilesheet.png
		Rectangle getRectangle()
		{
			return {((float)absolutePos.x), (float)absolutePos.y, 64, 64};
		}
		void drawSelf(Screen *screen)
		{
			DrawTexturePro(screen->spriteSheet, source, getRectangle(), {0, 0}, 0, WHITE);
		}
	};

	class Player
	{
	public:
		int isFacingLeft{0}; // Set to 64 to face right
		int speed{1};		 // MUST BE A MULTIPLE OF 64
		Vector2 velocity;
		Vector2 playerDesiredMovement;
		Vector2 absolutePos;
		intVector2 terminalVelocity{256, 512};
		Rectangle source;
		bool canJump{1};
		Player()
		{
			source = {0, (2048 - 64), 64, 64};
			absolutePos = {128, 128};
		}
		Rectangle getPredictedPosition(float &timeDelta, int xAxisOverride, int yAxisOverride)
		{
			return {absolutePos.x + (velocity.x * timeDelta * xAxisOverride), absolutePos.y + (velocity.y * timeDelta * yAxisOverride), 64, 64};
		}
		void physicsStep(std::vector<Block> &obstecules, float timeDelta)
		{
			velocity.y += speed * 128.0f * timeDelta;
			velocity.x > 0 ? velocity.x -= speed * 4.0f *timeDelta : velocity.x += speed * 4.0f * timeDelta;
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
			int indexes[3] = {-1, -1, -1};
			for (size_t i = 0; i < obstecules.size() && indexes[0] == -1; i++)
			{

				xAxisWillCollide = CheckCollisionRecs(getPredictedPosition(timeDelta, 1, 0), obstecules.at(i).getRectangle());
				if (xAxisWillCollide)
				{
					indexes[0] = i;
					break;
				}
			}
			for (size_t i = 0; i < obstecules.size() && indexes[1] == -1; i++)
			{

				yAxisWillCollide = CheckCollisionRecs(getPredictedPosition(timeDelta, 0, 1), obstecules.at(i).getRectangle());
				if (yAxisWillCollide)
				{
					indexes[1] = i;
					canJump = 1;
					break;
				}
			}
			for (size_t i = 0; i < obstecules.size() && indexes[2] == -1; i++)
			{

				cWillCollide = CheckCollisionRecs(getPredictedPosition(timeDelta, 1, 1), obstecules.at(i).getRectangle());
				if (cWillCollide)
				{
					indexes[2] = i;
					break;
				}
			}
			absolutePos.x += (velocity.x * timeDelta * !xAxisWillCollide);
			absolutePos.y += (velocity.y * timeDelta * !yAxisWillCollide);
			velocity.x = velocity.x * !xAxisWillCollide;
			velocity.y = velocity.y * !yAxisWillCollide;
		}
		void drawSelf(Screen &mainScreen)
		{
			DrawTexturePro(mainScreen.spriteSheet, {source.x + isFacingLeft, source.y, source.width, source.height}, {absolutePos.x, absolutePos.y, 64, 64}, {0, 0}, 0, WHITE);
		}
	};
	class Level
	{
	public:
		Vector2 playerStartingPosition;
		std::vector<Block> matter;
		void clearAll() // Clears everything in level and sets playerStartingPosition to 0, 0
		{
			matter.clear();
			playerStartingPosition = {0, 0};
		}
		void generateTestLevel(Rectangle textureToApply) // Creates a 10x10, hollow box
		{
			matter.clear();
			for (int i = 0; i < 10; i++)
			{
				matter.push_back(Block());
				matter.at(i).absolutePos = {64 * i, 0};
				matter.at(i).source = textureToApply;
			}
			playerStartingPosition = {0, -128};
		}
		void drawSelf(Screen &mainscreen)
		{
			for (size_t i = 0; i < matter.size(); i++)
			{
				matter.at(i).drawSelf(&mainscreen);
			}
		}
		Level(bool generateTest)
		{
			playerStartingPosition = {0, 0};
			if (generateTest)
			{
				generateTestLevel(BRICKTEXTURE);
			}
		}
	};

	namespace debugging
	{
		// Iterate over debug entries
		struct debugEntry
		{
			std::string descriptorOne;
			std::string descriptorTwo;
			std::string descriptorThree;
			int *entryOne = nullptr;
			float *entryTwo = nullptr;
			float *entryThree = nullptr;
			const char *content()
			{
				return TextFormat("%s: %d\n%s: %4.2f\n%s: %4.2f", descriptorOne.c_str(), *entryOne, descriptorTwo.c_str(), *entryTwo, descriptorThree.c_str(), *entryThree);
			}
			debugEntry(std::string a, int *x, std::string b, float *y, std::string c, float *z)
			{
				descriptorOne = a;
				descriptorTwo = b;
				descriptorThree = c;
				entryOne = x;
				entryTwo = y;
				entryThree = z;
			}
		};

	}
	class inputHandler
	{
	public:
		bool canJump{1};
		bool debugMenuVisible{0};
		int debugOption{0};
		void handleKeypresses(Player &mainPlayer, Screen &mainScreen, float frameDelta, std::vector<game::debugging::debugEntry> &debugs)
		{
			int response = GetCharPressed();
			if (IsKeyDown(KEY_D))
			{
				mainPlayer.playerDesiredMovement.x += mainPlayer.speed;
				mainPlayer.isFacingLeft = 0;
			}
			if (IsKeyDown(KEY_A))
			{
				mainPlayer.playerDesiredMovement.x -= mainPlayer.speed;
				mainPlayer.isFacingLeft = 64;
			}
			if (IsKeyDown(KEY_SPACE))
			{
				if (mainPlayer.canJump)
				{
					mainPlayer.canJump = 0;
					mainPlayer.playerDesiredMovement.y -= mainPlayer.speed * 256;
				}
			}
			if (IsKeyDown(KEY_Z))
			{
				debugMenuVisible = !debugMenuVisible;
			}
			if (IsKeyPressed(KEY_F))
			{
				ToggleFullscreen();
			}
			if (debugMenuVisible)
			{
				if (response == 'r')
				{
					mainPlayer.absolutePos = {0, -100};
				}
				if (response == '=')
				{
					(*(debugs.at(debugOption).entryOne))++;
				}
				if (response == '-')
				{
					(*(debugs.at(debugOption).entryOne))--;
				}
				if (response == '`')
				{
					debugOption + 1 < debugs.size() ? debugOption++ : debugOption = 0;
				}
			}
		}
	};
	void printAllShit(Screen &screen, Player &player, inputHandler &inputs, std::vector<game::debugging::debugEntry> &stuff)
	{
		if (inputs.debugMenuVisible)
		{
			DrawText(stuff.at(inputs.debugOption).content(), 20, 20, 12, BLUE);
		}
	}
}

int main()
{
	game::Screen mainScreen;
	game::Level earth(1);
	float frameDelta;
	game::Player slime;
	slime.absolutePos = earth.playerStartingPosition;
	slime.velocity = {0, 8};
	slime.playerDesiredMovement = {0, 0};
	game::inputHandler test;
	std::vector<game::debugging::debugEntry> debugEntries;
	debugEntries.push_back(game::debugging::debugEntry("Player.speed", &slime.speed, "VelocityX", &slime.velocity.x, "VelocityY", &slime.velocity.y));
	while (!WindowShouldClose())
	{
		frameDelta = GetFrameTime();
		mainScreen.updateWindow();
		mainScreen.camera.target = slime.absolutePos;
		BeginDrawing();
		ClearBackground(mainScreen.background);
		game::printAllShit(mainScreen, slime, test, debugEntries);
		BeginMode2D(mainScreen.camera);
		slime.drawSelf(mainScreen);
		earth.drawSelf(mainScreen);
		EndMode2D();
		EndDrawing();
		slime.physicsStep(earth.matter, frameDelta);
		test.handleKeypresses(slime, mainScreen, frameDelta, debugEntries);
	}
	return 0;
}