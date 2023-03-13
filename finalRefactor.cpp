#include <iostream>
#include <raylib.h>
#include <vector>
#include <thread>
#include <string>
#include <sstream>
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

#define QUITBUTTONUNHIGHLIGHTED \
	{                           \
		0, 64, 128, 64          \
	}
#define QUITBUTTONHIGHLIGHTED {128, 64, 128, 64};
#define RETURNTOGAMEUNHIGHLIGHTED {0, 128, 128, 64};
#define RETURNTOGAMEHIGHLIGHTED {128, 128, 128, 64};

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
	class intVector4 // Unused for now
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
		int isVisible{0};
		void reset(unsigned int milliseconds) // display animated text for a number of milliseconds
		{
			timeToEnd = std::chrono::_V2::system_clock::now() + std::chrono::milliseconds(milliseconds);
			isVisible = 1;
		}
		animatedText(std::string message)
		{
			Text = message;
		}
	};
	struct Vect2Pos // Unused for now
	{
		Vector2 pos1;
		Vector2 pos2;
	};
	class Screen
	{
	public:
		std::string WindowTitle = "A Platformer";
		intVector2 dimentions{800, 400};
		Camera2D camera;
		float hypotenuse;
		Texture2D spriteSheet;
		Image windowIcon;
		Color background;
		bool isRunning;
		void updateWindow(int &additionalZoom)
		{
			// Let the window manager handle window resolution. This isn't traditonal but fuck tradition.
			dimentions.x = GetRenderWidth();
			dimentions.y = GetRenderHeight();
			camera.offset.x = dimentions.x / 2;
			camera.offset.y = dimentions.y / 2;
			camera.zoom = (hypotenuse / (2202.91f + (10 * additionalZoom)));
			hypotenuse = std::sqrt((dimentions.x * dimentions.x) + (dimentions.y * dimentions.y));
		}
		Screen()
		{
			InitWindow(dimentions.x, dimentions.y, WindowTitle.c_str());
			SetWindowState(FLAG_WINDOW_RESIZABLE);
			spriteSheet = LoadTexture("assets/tilesheet.png");
			windowIcon = LoadImage("assets/icon.png");
			SetWindowIcon(windowIcon);
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
			UnloadImage(windowIcon);
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
	void animation(bool *isRunning, std::uint64_t &animatedEntityClock, int &sleepDuration)
	{
		while (*isRunning)
		{
			animatedEntityClock++;
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
	}
	class Player
	{
	public:
		int isFacingLeft{0}; // Set to 64 to face right
		int speed{1};
		Vector2 velocity;
		Vector2 playerDesiredMovement;
		Vector2 absolutePos;
		intVector2 terminalVelocity{256, 512};
		int dragCoefficentX{100};
		int dragCoefficentY{128};
		Rectangle source;
		bool canJump{1};
		int isMoving{0};
		Player()
		{
			source = {0, (2048 - 64), 64, 64};
			absolutePos = {128, 128};
		}
		Rectangle getPredictedPosition(float &timeDelta, int xAxisOverride, int yAxisOverride)
		{
			// This is slightly smaller than the actual sprite because floating point approximation limitations
			return {(absolutePos.x) + (velocity.x * timeDelta * xAxisOverride), (absolutePos.y + 23) + (velocity.y * timeDelta * yAxisOverride), 63, 41};
		}
		void physicsStep(std::vector<Block> &obstecules, float timeDelta, Camera2D &mainCamera)
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
			std::abs(velocity.x) > 0.2f ? velocity.x = velocity.x : velocity.x = 0;
			absolutePos.x += (velocity.x * timeDelta * !xAxisWillCollide);
			absolutePos.y += (velocity.y * timeDelta * !yAxisWillCollide);
			velocity.x = velocity.x * !xAxisWillCollide;
			velocity.y = velocity.y * !yAxisWillCollide;
		}
		void drawSelf(Screen &mainScreen, std::uint64_t *iterable)
		{
			std::abs(velocity.x) > 1 ? isMoving = 1 : isMoving = 0;
			int sourceX = (64 * (*iterable % 5)) * isMoving;
			DrawTexturePro(mainScreen.spriteSheet, {(float)sourceX, source.y - isFacingLeft, source.width, source.height}, {absolutePos.x, absolutePos.y, 64, 64}, {0, 0}, 0, WHITE);
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
		void generateTestLevel(Rectangle textureToApply)
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
		Level(bool generateATestLevel)
		{
			playerStartingPosition = {0, 0};
			if (generateATestLevel)
			{
				generateTestLevel(BRICKTEXTURE);
			}
		}
		int constructFromFile(std::string filename)
		{
			std::ifstream source("levels/" + filename);
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
	class UIbutton
	{
	public:
		Rectangle sourceUnhighlighted;
		Rectangle sourceHighlighted;
		Vector2 destination;
		bool isHighlighted{0};
		Rectangle source(Vector2 mousePos, Camera2D &camera)
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
		void whenClicked() // I haven't decided whether to let the Pause menu handle this behavior or derived classes
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
		UIbutton returnToGame;
		Vector2 mousePos;
		Camera2D otherCamera;
		/*
			Im using a second camera for the pause menu because i don't want to do the math.
		*/
		void alternateRender(Player &player, Level &level, Screen &screen, std::uint64_t *iterable, float &physicsMultiplyer, std::vector<int> &inputs)
		{
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
			DrawTextureRec(screen.spriteSheet, quit.source({(float)inputs[5], (float)inputs[6]}, otherCamera), quit.destination, WHITE);
			DrawTextureRec(screen.spriteSheet, returnToGame.source({(float)inputs[5], (float)inputs[6]}, otherCamera), returnToGame.destination, WHITE);
			EndMode2D();
			DrawText("PAUSED", (screen.dimentions.x / 2) - (MeasureText("PAUSED", screen.hypotenuse * 0.1f) / 2), screen.dimentions.y * 0.1f, screen.hypotenuse * 0.1f, {170, 255, 255, 255});
			EndDrawing();
			if (quit.isHighlighted && inputs[7])
			{
				screen.isRunning = 0;
			}
			if (returnToGame.isHighlighted && inputs[7])
			{
				isOpen = 0;
				physicsMultiplyer = (1.0f / 60.0f);
			}
		}
		PauseMenu()
		{
			quit.sourceHighlighted = QUITBUTTONHIGHLIGHTED;
			quit.sourceUnhighlighted = QUITBUTTONUNHIGHLIGHTED;
			returnToGame.sourceHighlighted = RETURNTOGAMEHIGHLIGHTED;
			returnToGame.sourceUnhighlighted = RETURNTOGAMEUNHIGHLIGHTED;
			quit.destination = {500, 600};
			returnToGame.destination = {372, 600};
			otherCamera.target = {500, 500};
			otherCamera.rotation = 0;
		}
	};

	void playerCollisionPhysics(Level *level, float *frameTime, Screen *screen, Player *player)
	{
		std::chrono::_V2::system_clock::time_point expectedCompletionTime;
		while (screen->isRunning)
		{
			expectedCompletionTime = std::chrono::system_clock::now() + std::chrono::milliseconds(16);
			player->physicsStep(level->matter, *frameTime, screen->camera);
			if (std::chrono::system_clock::now() > expectedCompletionTime)
			{
				std::cerr << "Physics thread falling behind!\nPhysics may become inaccurate!\n";
			}
			else
			{
				std::this_thread::sleep_until(expectedCompletionTime);
			}
		}
	}

}

int main(int argc, char **argv)
{
	game::Screen mainScreen;
	std::uint64_t animatedEntityClock = 0;
	std::vector<int> inputs(10, 0);
	{
		if (argc > 1)
		{
			// Try to parse it
			std::vector<std::string> commandLineArguments;
			for (int i = 1; i < argc; i++)
			{
				commandLineArguments.push_back(argv[i]);
			}
			if (commandLineArguments.at(0) == "-c")
			{
				int placeholder{60};
				try
				{
					placeholder = std::stoi(commandLineArguments.at(1));
					SetTargetFPS(placeholder);
				}
				catch (...)
				{
					std::cerr << "Invalid argument(s) provided. Exiting\n";
					mainScreen.isRunning = 0;
				}
			}
			if (commandLineArguments.at(0) == "--help")
			{
				std::cout << mainScreen.WindowTitle << "\nCommand Arguments\n./a.out <options> <arguments>\n\nOptions:\nFramerate cap: -c <int>\nDisplay this: --help\n";
				mainScreen.isRunning = 0;
			}
		}
		game::Level earth(1);
		int zoom{0};
		int framerate;
		float frameTime;
		float fakeframeTimeForPhysics{1.0f / 60.0f};
		int animationThreadSleep{100};
		int editMode{0};
		bool inConsole{0};
		bool showFPS{0};
		std::string commandLineBuffer;
		Rectangle blocks[3] = {BRICKTEXTURE, DIRTTEXTURE, GRASSTEXTURE};
		Vector2 snappingMousePosition;
		game::Player slime;
		slime.absolutePos = earth.playerStartingPosition;
		slime.velocity = {0, 8};
		slime.playerDesiredMovement = {0, 0};
		game::PauseMenu pauseMenu;
		std::thread animationWorker(game::animation, &mainScreen.isRunning, std::ref(animatedEntityClock), std::ref(animationThreadSleep));
		std::thread physicsWorker(game::playerCollisionPhysics, &earth, &fakeframeTimeForPhysics, &mainScreen, &slime);
		std::thread inputThread([&]
								  {
			while (mainScreen.isRunning)
			{
				snappingMousePosition = GetScreenToWorld2D({(float) inputs[5], (float) inputs[6]}, mainScreen.camera);
				snappingMousePosition.x = (((int)snappingMousePosition.x / 64) * 64) - 64;
				snappingMousePosition.y = (((int)snappingMousePosition.y / 64) * 64) - 64;
				if (inputs[0])
				{
					slime.playerDesiredMovement.x += slime.speed;
					slime.isFacingLeft = 0;

				}
				if (inputs[1])
				{
					slime.playerDesiredMovement.x -= slime.speed;
					slime.isFacingLeft = 64;
				}
				if (inputs[2])
				{
					if (slime.canJump)
					{
						slime.canJump = 0;
						slime.playerDesiredMovement.y -= (slime.speed * 256);
					}
				}
				if (inputs[3])
				{
					pauseMenu.isOpen = 1;
				}
				if (inputs[7] && editMode && !pauseMenu.isOpen)
				{
					bool matchFound {0};
					for (size_t i = 0; i < earth.matter.size(); i++)
					{
						if (snappingMousePosition.x == earth.matter.at(i).absolutePos.x && snappingMousePosition.y == earth.matter.at(i).absolutePos.y)
						{
							matchFound = 1;
							break;
						}
					}
					if (!matchFound)
					{
						game::Block newBlock;
						newBlock.source = blocks[editMode % 3];
						newBlock.absolutePos = snappingMousePosition;
						earth.matter.push_back(newBlock);
					}
				}
				if (inputs[8] && editMode && !pauseMenu.isOpen)
				{
					
					for (size_t i = 0; i < earth.matter.size(); i++)
					{
						if (snappingMousePosition.x == earth.matter.at(i).absolutePos.x && snappingMousePosition.y == earth.matter.at(i).absolutePos.y)
						{
							/*
								Since std::vector doesn't have a method for removing elements at an arbitrary index, i have to use this mess.
								It swaps the element at the last index in the vector, and the element at the index to remove. From then you can pop off the last element, deleting it.
							*/
							game::Block *cache1 = new game::Block;
							game::Block *cache2 = new game::Block;
							*cache1 = earth.matter.at(earth.matter.size() - 1);
							*cache2 = earth.matter.at(i);
							earth.matter.at(i) = *cache1;
							earth.matter.pop_back();
							delete cache1;
							delete cache2;
							break;
						}
					}
				}
			} });
		while (!WindowShouldClose() && mainScreen.isRunning)
		{
			frameTime = GetFrameTime();
			framerate = (1 / (frameTime));
			mainScreen.updateWindow(zoom);
			mainScreen.camera.target = slime.absolutePos;
			if (pauseMenu.isOpen)
			{
				fakeframeTimeForPhysics = 0;
				pauseMenu.alternateRender(slime, earth, mainScreen, &animatedEntityClock, fakeframeTimeForPhysics, inputs);
			}
			else
			{
				BeginDrawing();
				ClearBackground(mainScreen.background);
				BeginMode2D(mainScreen.camera);
				slime.drawSelf(mainScreen, &animatedEntityClock);
				earth.drawSelf(mainScreen);
				EndMode2D();
				if (showFPS)
				{
					DrawText(TextFormat("FPS: %d", framerate), 50, (mainScreen.dimentions.y * 0.1f), 0.01f * mainScreen.hypotenuse, BLUE);
				}
				if (inConsole)
				{
					DrawText(commandLineBuffer.c_str(), 50, (mainScreen.dimentions.y * 0.8f), 0.01f * mainScreen.hypotenuse, RED);
					int keypress = GetKeyPressed();
					if (keypress != 0 && keypress != '/' && keypress != KEY_BACKSPACE)
					{
						if (keypress == KEY_ENTER)
						{
							inConsole = 0;
							std::vector<std::string> arguments;
							std::string buff;
							std::istringstream source(commandLineBuffer);
							while (std::getline(source, buff, ' '))
							{
								arguments.push_back(buff);
							}
							try
							{
								if (arguments.at(0) == "/SET")
								{
									if (arguments.at(1) == "EDITMODE")
									{
										editMode = std::stoi(arguments.at(2));
									}
									if (arguments.at(1) == "ZOOM")
									{
										zoom = std::stof(arguments.at(2));
									}
								}
								if (arguments.at(0) == "/FULLSCREEN")
								{
									ToggleFullscreen();
								}
								if (arguments.at(0) == "/LOAD")
								{
									if (earth.constructFromFile(arguments.at(1)) == 0)
									{
										slime.absolutePos = earth.playerStartingPosition;
									}
								}
								if (arguments.at(0) == "/DUMP" && arguments.size() > 1)
								{
									// I am too lasy to compress levels before writing them to disk
									std::string filename = "levels/" + arguments.at(1);
									std::ofstream output(filename, std::ios::trunc);
									if (!output.is_open())
									{
										std::cerr << "Something went wrong" << '\n';
										continue;
									}
									output << earth.playerStartingPosition.x << ", " << earth.playerStartingPosition.y << std::endl;
									for (size_t i = 0; i < earth.matter.size(); i++)
									{
										output << earth.matter.at(i).absolutePos.x << ", " << earth.matter.at(i).absolutePos.y << ", " << earth.matter.at(i).source.x << ", " << earth.matter.at(i).source.y << ", " << earth.matter.at(i).source.width << ", " << earth.matter.at(i).source.height << std::endl;
									}
									output.close();
								}
								if (arguments.at(0) == "/SHOWFPS")
								{
									showFPS = !showFPS;
								}
								if (arguments.at(0) == "/RESETLEVEL")
								{
									earth.generateTestLevel(BRICKTEXTURE);
									slime.absolutePos = earth.playerStartingPosition;
								}
							}
							catch (const std::exception &e)
							{
								std::cerr << e.what() << '\n';
							}
							commandLineBuffer.clear();
						}
						else
						{

							commandLineBuffer += (keypress % 255);
						}
					}
				}
				EndDrawing();
			}
			if (IsKeyPressed(KEY_BACKSPACE) && inConsole && commandLineBuffer.size() > 1)
			{
				commandLineBuffer.pop_back();
			}
			if (IsKeyPressed(KEY_SLASH) && !inConsole)
			{
				inConsole = 1;
				commandLineBuffer += '/';
			}
			inputs[0] = IsKeyDown(KEY_D);
			inputs[1] = IsKeyDown(KEY_A);
			inputs[2] = IsKeyDown(KEY_SPACE);
			inputs[3] = IsKeyDown(KEY_ESCAPE);
			inputs[4] = IsKeyDown(KEY_F);
			inputs[5] = GetMouseX();
			inputs[6] = GetMouseY();
			inputs[7] = IsMouseButtonDown(MOUSE_BUTTON_LEFT);
			inputs[8] = IsMouseButtonDown(MOUSE_BUTTON_RIGHT);
		}
		mainScreen.isRunning = 0;
		inputThread.join();
		physicsWorker.join();
		animationWorker.join();
	}
	return 0;
}