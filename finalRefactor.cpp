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

#define QUITBUTTONUNHIGHLIGHTED     \
	{                  \
		0, 64, 128, 64 \
	}
#define QUITBUTTONHIGHLIGHTED {128, 64, 128, 64};

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
	struct animatedText
	{
		std::chrono::_V2::system_clock::time_point timeToEnd;
		std::string Text;
		int isVisible {0};
		void reset(unsigned int milliseconds)
		{
			timeToEnd = std::chrono::_V2::system_clock::now() + std::chrono::milliseconds(milliseconds);
			isVisible = 1;
		}
		animatedText(std::string message)
		{
			Text = message;
		}
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
		bool isRunning;
		void updateWindow(int & additionalZoom)
		{
			dimentions.x = GetRenderWidth();
			dimentions.y = GetRenderHeight();
			camera.offset.x = dimentions.x / 2;
			camera.offset.y = dimentions.y / 2;
			camera.zoom = (hypotenuse / (2202.91 + (10*additionalZoom)));
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
			isRunning = 1;
		}
		~Screen()
		{
			isRunning = 0;
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
	void animation(bool * isRunning, std::uint64_t & animatedEntityClock, int & sleepDuration, std::vector<animatedText> * texts)
	{
		while (*isRunning)
		{
			animatedEntityClock++;
			for (size_t i = 0; i < texts->size(); i++)
			{
				if (std::chrono::system_clock::now() > texts->at(i).timeToEnd)
				{
					texts->at(i).isVisible = 0;
				}
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
	}
	class Player
	{
	public:
		int isFacingLeft{0}; // Set to 64 to face right
		int speed{1};		 // MUST BE A MULTIPLE OF 64
		Vector2 velocity;
		Vector2 playerDesiredMovement;
		Vector2 absolutePos;
		intVector2 terminalVelocity{256, 512};
		int dragCoefficentX{4};
		int dragCoefficentY{128};
		Rectangle source;
		bool canJump{1};
		int isMoving {0};
		Player()
		{
			source = {0, (2048 - 64), 64, 64};
			absolutePos = {128, 128};
		}
		Rectangle getPredictedPosition(float &timeDelta, int xAxisOverride, int yAxisOverride)
		{
			// This is slightly smaller than the actual sprite because floating point approximation limitations
			return {absolutePos.x + (velocity.x * timeDelta * xAxisOverride), absolutePos.y + (velocity.y * timeDelta * yAxisOverride), 63, 63};
		}
		void physicsStep(std::vector<Block> &obstecules, float timeDelta)
		{
			velocity.y += speed * (float)dragCoefficentY * timeDelta;
			velocity.x > 0 ? velocity.x -= speed * (float)dragCoefficentX *timeDelta : velocity.x += speed * (float)dragCoefficentX * timeDelta;
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
		void drawSelf(Screen &mainScreen, std::uint64_t * iterable)
		{
			std::abs(velocity.x) > 1 ? isMoving = 1 : isMoving = 0;
			int sourceX = (64 * (*iterable % 5)) * isMoving;
			DrawTexturePro(mainScreen.spriteSheet, {(float) sourceX, source.y - isFacingLeft, source.width, source.height}, {absolutePos.x, absolutePos.y, 64, 64}, {0, 0}, 0, WHITE);
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
			size_t i{0};
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
		public:
			std::vector<int *> dynamicPointers = {&dummyValue, &dummyValue, &dummyValue, &dummyValue, &dummyValue, &dummyValue};

		public:
			int highlighted{0};
			int dummyValue{2000};
			std::vector<std::string> variables;
			std::vector<int *> pointers;
			void drawSelf()
			{
				std::string content;
				for (int i = 0; i < variables.size(); i++)
				{
					std::string prependable;
					i == highlighted ? prependable = "[X] " : prependable = "";
					content += prependable + variables.at(i) + ": " + std::to_string(*(pointers.at(i))) + '\n';
				}
				DrawText(content.c_str(), 20, 20, 20, {170, 255, 255, 255});
			}
		};
	}
	class UIbutton
	{
		public:
		Rectangle sourceUnhighlighted;
		Rectangle sourceHighlighted;
		Vector2 destination;
		bool isHighlighted {0};
		Rectangle source(Vector2 mousePos, Camera2D & camera)
		{
			isHighlighted = CheckCollisionPointRec(GetScreenToWorld2D(mousePos, camera), {destination.x, destination.y, sourceHighlighted.width, sourceHighlighted.height});
			if (isHighlighted)
			{
				return (sourceHighlighted);
			}
			else
			{
				return (sourceUnhighlighted);
			}
		}
		void whenClicked()
		{

		}
		
	};
	class PauseMenu
	{
	public:
		bool isOpen{0};
		bool gameShouldPauseWhenOpened{1};
		int keyToOpen{KEY_ESCAPE};
		UIbutton quit;
		Vector2 mousePos;
		Camera2D otherCamera;
		void alternateRender(Player &player, Level &level, Screen &screen, debugging::debugMenu &debugs, std::uint64_t * iterable)
		{
			mousePos = GetMousePosition();
			otherCamera.offset = screen.camera.offset;
			otherCamera.zoom = screen.camera.zoom * 2;
			BeginDrawing();
			ClearBackground(screen.background);
			BeginMode2D(screen.camera);
			player.drawSelf(screen, iterable);
			level.drawSelf(screen);
			EndMode2D();
			DrawRectangle(0, 0, screen.dimentions.x, screen.dimentions.y, {0, 50, 0, 30});
			BeginMode2D(otherCamera);
			DrawTextureRec(screen.spriteSheet, quit.source(mousePos, otherCamera), quit.destination, WHITE);
			EndMode2D();
			debugs.drawSelf();
			DrawText("PAUSED", (screen.dimentions.x / 2 ) - (MeasureText("PAUSED", 50) / 2), screen.dimentions.y * 0.1f, 50, {170, 255, 255, 255});
			EndDrawing();
			// Handle inputs
			if (IsKeyPressed(keyToOpen))
			{
				isOpen = !isOpen;
			}
			if (IsKeyPressed(KEY_F))
			{
				ToggleFullscreen();
			}
			if (quit.isHighlighted && IsMouseButtonDown(MOUSE_BUTTON_LEFT))
			{
				screen.isRunning = 0;
			}

		}
		PauseMenu()
		{
			quit.sourceHighlighted = QUITBUTTONHIGHLIGHTED;
			quit.sourceUnhighlighted = QUITBUTTONUNHIGHLIGHTED;
			quit.destination = {436, 600};
			otherCamera.target = {500, 500};
		}
	};
	
	class inputHandler
	{
	public:
		bool canJump{1};
		bool debugMenuVisible{0};
		int debugOption{0};
		int editMode{0};
		int debugOptionMultiple{4};
		int saveName{0};
		int loadNumber{0};
		Vector2 mousePosition;
		Vector2 snappingMousePosition;
		Rectangle blocks[3] = {BRICKTEXTURE, DIRTTEXTURE, GRASSTEXTURE};
		void handleKeypresses(Player &mainPlayer, Screen &mainScreen, float frameDelta, debugging::debugMenu &debugs, Level *level, PauseMenu &pauseMenu, std::vector<animatedText> &animations)
		{
			int response = GetKeyPressed();
			mousePosition = GetMousePosition();
			snappingMousePosition = GetScreenToWorld2D(mousePosition, mainScreen.camera);
			snappingMousePosition.x = (((int)snappingMousePosition.x / 64) * 64) - 64;
			snappingMousePosition.y = (((int)snappingMousePosition.y / 64) * 64) - 64;
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
				switch (response)
				{
				case KEY_UP:
					if (debugs.highlighted == 0)
					{
						debugs.highlighted = debugs.variables.size() - 1;
					}
					else
					{
						debugs.highlighted--;
					}
					break;

				case KEY_DOWN:
					if (debugs.highlighted == debugs.variables.size() - 1)
					{
						debugs.highlighted = 0;
					}
					else
					{
						debugs.highlighted++;
					}
					break;

				case KEY_LEFT:
					(*(debugs.pointers.at(debugs.highlighted))) -= (debugOptionMultiple * IsKeyDown(KEY_LEFT_SHIFT)) + 1;
					break;

				case KEY_RIGHT:
					(*(debugs.pointers.at(debugs.highlighted))) += (debugOptionMultiple * IsKeyDown(KEY_LEFT_SHIFT)) + 1;
					break;

				case KEY_M:
				{
					std::string filename = "levels/" + std::to_string(saveName) + ".level";
					std::ofstream output(filename, std::ios::trunc);
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
					animations.at(0).reset(3000);
				}
				break;

				case KEY_N:
				{
					std::string filename = "levels/" + std::to_string(loadNumber) + ".level";
					if (FileExists(filename.c_str()))
					{
						if (level->constructFromFile(filename.c_str()) == 0)
						{
							mainPlayer.velocity = {0, 8};
							mainPlayer.absolutePos = level->playerStartingPosition;
							animations.at(1).reset(3000);
						}
						else
						{
							std::string error = "Failed to read " + filename;
							std::cerr << error << std::endl;
							animations.at(2).Text = error;
							animations.at(2).reset(3000);
						}
						
					}
					else
					{
						std::string error = "Failed to read " + filename;
						std::cerr << error << std::endl;
						animations.at(2).Text = error;
						animations.at(2).reset(3000);
					}
					break;
				}

				default:
					break;
				}
			}
			if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT) && editMode)
			{
				for (size_t i = 0; i < level->matter.size(); i++)
				{
					if (snappingMousePosition.x == level->matter.at(i).absolutePos.x && snappingMousePosition.y == level->matter.at(i).absolutePos.y)
					{
						break;
					}
				}
				Block newBlock;
				newBlock.source = blocks[editMode % 3];
				newBlock.absolutePos = snappingMousePosition;
				level->matter.push_back(newBlock);
			}
			if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && editMode)
			{
				size_t i = 0;
				for (; i < level->matter.size(); i++)
				{
					if (snappingMousePosition.x == level->matter.at(i).absolutePos.x && snappingMousePosition.y == level->matter.at(i).absolutePos.y)
					{
						Block *cache1 = new Block;
						Block *cache2 = new Block;
						*cache1 = level->matter.at(level->matter.size() - 1);
						*cache2 = level->matter.at(i);
						level->matter.at(i) = *cache1;
						level->matter.pop_back();
						delete cache1;
						delete cache2;
						break;
					}
				}
			}
		}
	};

	void printAllShit(Screen &screen, Player &player, inputHandler &inputs, debugging::debugMenu &stuff)
	{
		if (inputs.editMode)
		{
			BeginMode2D(screen.camera);
			DrawTexturePro(screen.spriteSheet, inputs.blocks[inputs.editMode % 3], {inputs.snappingMousePosition.x, inputs.snappingMousePosition.y, 64, 64}, {0, 0}, 0, RED);
			EndMode2D();
		}
	}
	
}

int main()
{
	game::Screen mainScreen;
	std::uint64_t animatedEntityClock = 0;
	{
		game::Level earth(1);
		int zoom {0};
		int framerate;
		float frameDelta;
		int AnimationThreadSleep {100};
		game::Player slime;
		slime.absolutePos = earth.playerStartingPosition;
		slime.velocity = {0, 8};
		slime.playerDesiredMovement = {0, 0};
		game::inputHandler test;
		game::PauseMenu pauseMenu;
		game::debugging::debugMenu debugMenu;
		std::vector<game::animatedText> animatedText;
		animatedText.push_back(game::animatedText("Level dumped successfully"));
		animatedText.push_back(game::animatedText("Level loaded successfully"));
		animatedText.push_back(game::animatedText("Failed to load"));
		debugMenu.variables = {"Drag Divider", "Edit Mode", "Slime.dragCoefficentX", "Slime.dragCoefficentY", "Save As", "Load", "Zoom change", "Framerate"};
		debugMenu.pointers = {&slime.speed, &test.editMode, &slime.dragCoefficentX, &slime.dragCoefficentY, &test.saveName, &test.loadNumber, &zoom, &framerate};
		std::thread worker(game::animation, &mainScreen.isRunning, std::ref(animatedEntityClock), std::ref(AnimationThreadSleep), &animatedText);
		while (!WindowShouldClose() && mainScreen.isRunning)
		{
			frameDelta = GetFrameTime();
			framerate = (1/(frameDelta));
			mainScreen.updateWindow(zoom);
			mainScreen.camera.target = slime.absolutePos;
			if (pauseMenu.isOpen)
			{
				pauseMenu.alternateRender(slime, earth, mainScreen, debugMenu, &animatedEntityClock);
			}
			else
			{
				BeginDrawing();
				ClearBackground(mainScreen.background);
				BeginMode2D(mainScreen.camera);
				slime.drawSelf(mainScreen, &animatedEntityClock);
				earth.drawSelf(mainScreen);
				EndMode2D();
				for (int i = 0; i < animatedText.size(); i++)
				{
					if (animatedText.at(i).isVisible > 0)
					{
						DrawText(animatedText.at(i).Text.c_str(), 100, mainScreen.dimentions.y - 100 - 50, 50, {170, 255, 255, 255});
					}
				}
				game::printAllShit(mainScreen, slime, test, debugMenu);
				debugMenu.drawSelf();
				EndDrawing();
				slime.physicsStep(earth.matter, frameDelta);
				test.handleKeypresses(slime, mainScreen, frameDelta, debugMenu, &earth, pauseMenu, animatedText);
			}
		}
		mainScreen.isRunning = 0;
		worker.join();
	}
	return 0;
}