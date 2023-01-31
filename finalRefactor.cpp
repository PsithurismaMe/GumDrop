#include <iostream>
#include <raylib.h>
#include <vector>
#include <thread>
#include <string>
#include <cmath>
#include <fstream>

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
			SetExitKey(-1);
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
		Color inheritableColor;
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
		int constructFromFile(std::string filename)
		{
			std::ifstream source(filename);
			if (!source.is_open())
			{
				std::cerr << "Reading " << filename << " went wrong" << '\n';
				return -1;
			}
			std::string buffer;
			std::string lineBuffer;
			std::getline(source, lineBuffer);
			// Read player starting position
			size_t i {0};
			for (; i < lineBuffer.size() && lineBuffer.at(i) != ','; i++)
			{
				buffer += lineBuffer.at(i);
			}
			playerStartingPosition.x = std::stoi(buffer);
			buffer.clear();
			i++;
			for (; i < lineBuffer.size(); i++)
			{
				buffer += lineBuffer.at(i);
			}
			playerStartingPosition.y = std::stoi(buffer);
			buffer.clear();
			matter.clear();
			// Now add all remaining blocks
			while (std::getline(source, lineBuffer))
			{
				// Read block positionX
				Block newBlock;
				for (i = 0; i < lineBuffer.size() && lineBuffer.at(i) != ','; i++)
				{
					buffer += lineBuffer.at(i);
				}
				newBlock.absolutePos.x = std::stoi(buffer);
				buffer.clear();
				i++;
				// Read block positionY
				for (; i < lineBuffer.size() && lineBuffer.at(i) != ','; i++)
				{
					buffer += lineBuffer.at(i);
				}
				newBlock.absolutePos.y = std::stoi(buffer);
				buffer.clear();
				i++;
				// Now read texture position
				for (; i < lineBuffer.size() && lineBuffer.at(i) != ','; i++)
				{
					buffer += lineBuffer.at(i);
				}
				newBlock.source.x = std::stoi(buffer);
				buffer.clear();
				i++;
				for (; i < lineBuffer.size() && lineBuffer.at(i) != ','; i++)
				{
					buffer += lineBuffer.at(i);
				}
				newBlock.source.y = std::stoi(buffer);
				buffer.clear();
				i++;
				for (; i < lineBuffer.size() && lineBuffer.at(i) != ','; i++)
				{
					buffer += lineBuffer.at(i);
				}
				newBlock.source.width = std::stoi(buffer);
				buffer.clear();
				i++;
				for (; i < lineBuffer.size(); i++)
				{
					buffer += lineBuffer.at(i);
				}
				newBlock.source.height = std::stoi(buffer);
				matter.push_back(newBlock);
				buffer.clear();
			}
			source.close();
			return 0;
		}
	};

	namespace debugging
	{
		// Iterate over debug entries
		struct debugMenu
		{
			int highlighted {0};
			int dummyValue {0};
			std::vector<std::string> variables = {"slime.speed", "Edit Mode", "Slime X", "Slime Y", "Mouse X", "Mouse Y"};
			std::vector<int *> pointers = {&dummyValue, &dummyValue, &dummyValue, &dummyValue, &dummyValue, &dummyValue};
			void drawSelf()
			{
				std::string content;
				for (int i = 0; i < variables.size(); i++)
				{
					std::string prependable;
					i == highlighted ? prependable = "[X] " : prependable = "";
					content += prependable + variables.at(i) + ": " + std::to_string(*(pointers.at(i))) + '\n';
				}
				DrawText(content.c_str(), 20, 20, 20, BLUE);
			}
		};

	}
	class PauseMenu
	{
		public:
			bool isOpen {0};
			bool gameShouldPauseWhenOpened {1};
			int keyToOpen {KEY_ESCAPE};
			void alternateRender(Player & player, Level & level, Screen & screen, debugging::debugMenu & debugs)
			{
				BeginDrawing();
				ClearBackground(screen.background);
				debugs.drawSelf();
				BeginMode2D(screen.camera);
				player.drawSelf(screen);
				level.drawSelf(screen);
				EndMode2D();
				DrawRectangle(0,0,screen.dimentions.x, screen.dimentions.y, {0, 50, 0, 30});
				DrawText("PAUSED", screen.dimentions.x - 100 - MeasureText("PAUSED", 50), screen.dimentions.y - 100 , 50, GREEN);
				EndDrawing();
				// Handle inputs
				if (IsKeyPressed(keyToOpen))
				{
					isOpen = !isOpen;
				}
			}
	};
	class inputHandler
	{
	public:
		bool canJump{1};
		bool debugMenuVisible{0};
		int debugOption{0};
		int editMode {0};
		Vector2 mousePosition;
		Vector2 snappingMousePosition;
		void handleKeypresses(Player &mainPlayer, Screen &mainScreen, float frameDelta, debugging::debugMenu &debugs, Level * level, PauseMenu & pauseMenu)
		{
			int response = GetKeyPressed();
			mousePosition = GetMousePosition();
			snappingMousePosition = GetScreenToWorld2D(mousePosition, mainScreen.camera);
			snappingMousePosition.x = (((int) snappingMousePosition.x / 64)*64)-64;
			snappingMousePosition.y = (((int) snappingMousePosition.y / 64)*64)-64;
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
			if (IsKeyPressed(pauseMenu.keyToOpen))
			{
				pauseMenu.isOpen = 1;
			}
			if (IsKeyPressed(KEY_Z))
			{
				debugMenuVisible = !debugMenuVisible;
			}
			if (IsKeyPressed(KEY_F))
			{
				ToggleFullscreen();
			}
			if (debugMenuVisible)
			{
				if (response == KEY_UP)
				{
					if (debugs.highlighted == 0)
					{
						debugs.highlighted = debugs.variables.size() - 1;
					}
					else
					{
						debugs.highlighted--;
					}
				}
				if (response == KEY_DOWN)
				{
					if (debugs.highlighted == debugs.variables.size() - 1)
					{
						debugs.highlighted = 0;
					}
					else
					{
						debugs.highlighted++;
					}
				}
				if (response == KEY_LEFT)
				{
					(*(debugs.pointers.at(debugs.highlighted)))--;
				}
				if (response == KEY_RIGHT)
				{
					(*(debugs.pointers.at(debugs.highlighted)))++;
				}
				if (response == KEY_M)
				{
					std::ofstream output("LevelDump.raw", std::ios::ate);
					if (!output.is_open())
					{
						std::cerr << "Something went wrong" << '\n';
						return;
					}
					output << level->playerStartingPosition.x << ", " << level->playerStartingPosition.y << std::endl;
					for (size_t i = 0; i < level->matter.size(); i++)
					{
						output << level->matter.at(i).absolutePos.x << ", " << level->matter.at(i).absolutePos.y << ", " << level->matter.at(i).source.x << ", " << level->matter.at(i).source.y << ", " << level->matter.at(i).source.width << ", " << level->matter.at(i).source.height << std::endl;
					}
					output.close();
				}
				if (response == KEY_N)
				{
					if (FileExists("LevelDump.raw"))
					{
						if (level->constructFromFile("LevelDump.raw") == 0)
						{
							mainPlayer.velocity = {0, 8};
							mainPlayer.absolutePos = level->playerStartingPosition;
						}
						else
						{
							std::cerr << "Failed to read " << "LevelDump.raw" << "\n";
						}
						
					}
					

				}
			}
			if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && editMode)
			{
				Block newBlock;
				newBlock.source = BRICKTEXTURE;
				newBlock.absolutePos = snappingMousePosition;
				level->matter.push_back(newBlock);
			}
		}
	};
	
	void printAllShit(Screen &screen, Player &player, inputHandler &inputs, debugging::debugMenu &stuff)
	{
		if (inputs.editMode)
		{
			BeginMode2D(screen.camera);
			DrawTexturePro(screen.spriteSheet, BRICKTEXTURE, {inputs.snappingMousePosition.x, inputs.snappingMousePosition.y, 64, 64}, {0, 0}, 0, RED);
			EndMode2D();
		}
	}
}


int main()
{
	game::Screen mainScreen;
	{
		game::Level earth(1);
		float frameDelta;
		game::Player slime;
		slime.absolutePos = earth.playerStartingPosition;
		slime.velocity = {0, 8};
		slime.playerDesiredMovement = {0, 0};
		game::inputHandler test;
		game::PauseMenu pauseMenu;
		game::debugging::debugMenu debugMenu;
		debugMenu.pointers.at(1) = &test.editMode;
		while (!WindowShouldClose())
		{
			frameDelta = GetFrameTime();
			mainScreen.updateWindow();
			mainScreen.camera.target = slime.absolutePos;
			if (pauseMenu.isOpen)
			{
				pauseMenu.alternateRender(slime, earth, mainScreen, debugMenu);
			}
			else
			{
				BeginDrawing();
				ClearBackground(mainScreen.background);
				game::printAllShit(mainScreen, slime, test, debugMenu);
				debugMenu.drawSelf();
				BeginMode2D(mainScreen.camera);
				slime.drawSelf(mainScreen);
				earth.drawSelf(mainScreen);
				EndMode2D();
				EndDrawing();
				slime.physicsStep(earth.matter, frameDelta);
				test.handleKeypresses(slime, mainScreen, frameDelta, debugMenu, &earth, pauseMenu);
			}
		}
	}
	return 0;
}