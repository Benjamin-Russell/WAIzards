#include "Game.h"
#include <math.h>

// Static Member Initializations
bool Game::msInitialized = false;
Game* Game::mspInstance = NULL;
GraphicsSystem* Game::mspGraphicsSystem = NULL;
GraphicsBufferManager* Game::mspGraphicsBufferManager = NULL;
InputManager* Game::mspInputManager = NULL;
InputListener* Game::mspInputListener = NULL;
GameListener* Game::mspGameListener = NULL;

Game::Game()
{
	mRunningLoop = false;
	mCurrentTime = 0.0f;
	mPreviousTime = 0.0f;
	mElapsedTime = 0.0f;
	mLagTime = 0.0f;
	mFpsUpdateTime = 0.0f;
	mFrameCounter = 0;
	mEndFrameTime = 0.0f;

	mRedrawEverything = true;
	mDrawHealth = true;
	mDrawMana = true;

	srand((unsigned)time(NULL));

	mspGraphicsSystem = new GraphicsSystem(1920, 1080);
	mspGraphicsBufferManager = new GraphicsBufferManager();
	mspInputManager = new InputManager();
	mspInputListener = new InputListener();
	
	TTF_Init();
	mUIFont = new Font("Resources/pixelzim.ttf", 36);

	SDL_Surface* surfacePtr = SDL_CreateRGBSurface(0, 64, 64, 32, 0xff000000, 0x00ff0000, 0x0000ff00, 0);
	SDL_Rect sizeRect;
	sizeRect.x = 0;
	sizeRect.y = 0;
	sizeRect.w = 64;
	sizeRect.h = 10;

	surfacePtr = SDL_CreateRGBSurface(0, 64, 64, 32, 0xff000000, 0x00ff0000, 0x0000ff00, 0);
	SDL_FillRect(surfacePtr, &sizeRect, SDL_MapRGB(surfacePtr->format, 255, 0, 0));	// Red
	sizeRect.y = 10;
	SDL_FillRect(surfacePtr, &sizeRect, SDL_MapRGB(surfacePtr->format, 0, 0, 255));	// Blue
	mspGraphicsBufferManager->addGraphicsBuffer("Bars", new GraphicsBuffer(surfacePtr));

	surfacePtr = NULL; // Finish custom surfaces

	mspGraphicsBufferManager->addGraphicsBuffer("Background", new GraphicsBuffer("Resources/WAIzards_Background.png"));
	mspGraphicsBufferManager->addGraphicsBuffer("Environment", new GraphicsBuffer("Resources/WAIzards_Environment.png"));
	mspGraphicsBufferManager->addGraphicsBuffer("WAIzard", new GraphicsBuffer("Resources/WAIzards.png"));
	mspGraphicsBufferManager->addGraphicsBuffer("WAIzardDead", new GraphicsBuffer("Resources/WAIzard_Dead.png"));
	mspGraphicsBufferManager->addGraphicsBuffer("Fireball", new GraphicsBuffer("Resources/Basic_Fireball.png"));

	mpFloorSprite = new Sprite(mspGraphicsBufferManager->getGraphicsBuffer("Environment"), 64, 64, Vector2(0.0f, 0.0f));
	mpWallSprite = new Sprite(mspGraphicsBufferManager->getGraphicsBuffer("Environment"), 64, 64, Vector2(64.0f, 0.0f));
	mpWAIzardSprite = new Sprite(mspGraphicsBufferManager->getGraphicsBuffer("WAIzard"), 64, 64, Vector2(0.0f, 0.0f));
	mpDeadSprite = new Sprite(mspGraphicsBufferManager->getGraphicsBuffer("WAIzardDead"), 64, 64, Vector2(0.0f, 0.0f));

	// Fireball Sprites
	for (int i = 0; i < 4; ++i)
	{
		mFireballSprites.push_back(new Sprite(mspGraphicsBufferManager->getGraphicsBuffer("Fireball"), 64, 64, Vector2((float)i * 64.0f, 0.0f)));
	}

	mCurrentState = PLAY;
}

Game::~Game()
{

}

void Game::initInstance()
{
	if (!msInitialized)
	{
		gpEventSystem = new EventSystem();
		mspInstance = new Game();

		msInitialized = true;
	}
}

void Game::cleanUp()
{
	if (msInitialized)
	{
		msInitialized = false;

		// All destructors automatically call cleanup
		delete mspGraphicsSystem;
		delete mspGraphicsBufferManager;
		delete mspInputManager;
		delete mspInputListener;
		delete mspGameListener;
		delete gpEventSystem;
		delete mUIFont;

		delete mpFloorSprite;
		delete mpWallSprite;
		delete mpWAIzardSprite;

		unsigned int i;
	
		for (i = 0; i < mFireballSprites.size(); ++i)
		{
			delete mFireballSprites[i];
		}
		mFireballSprites.clear();

		mFloors.clear();
		mWalls.clear();
		mSpawnPoints.clear();
		mWAIzards.clear();

		for (unsigned int i = 0; i < mSpells.size(); ++i)
		{
			delete mSpells[i];
		}
		mSpells.clear();

		TTF_Quit();
		SDL_Quit();
	}
}

int Game::getRandomInt(int max)
{
	return rand() % max;
}

void Game::loadLevel(string file)
{
	// Clean up any objects already in level
	mFloors.clear();
	mWalls.clear();
	mSpawnPoints.clear();
	mWAIzards.clear();
	mSpells.clear();

	// Redraw entire screen next draw
	mRedrawEverything = true;

	ifstream fileIn(file, std::ifstream::in);
	int tile = -1;
	int spawnNum = 0;
	
	// 30 wide, 16 high
	for (int i = 0; i < 16; ++i)
	{
		for (int j = 0; j < 30; ++j)
		{
			fileIn >> tile;

			switch (tile)
			{
			case 1:
				mFloors.push_back(Floor(Vector2((float)j * 64.0f, (float)i * 64.0f), mpFloorSprite, getUnitID()));
				break;
			case 2:
				mWalls.push_back(Wall(Vector2((float)j * 64.0f, (float)i * 64.0f), mpWallSprite, getUnitID()));
				break;
			case 3:
				mSpawnPoints.push_back(Vector2((float)(j * 64), (float)(i * 64)));
				mFloors.push_back(Floor(Vector2((float)j * 64.0f, (float)i * 64.0f), mpFloorSprite, getUnitID()));

				if (spawnNum < 15)
				{
					mWAIzards.push_back(WAIzard((Vector2((float)(j * 64), (float)(i * 64))), "Resources/AI/WAIzard1.txt"));
				}
				break;
			}
		}
	}

	fileIn.close();
}

void Game::drawGame(GraphicsSystem* gs)
{
	if (mRedrawEverything)
	{
		mspGraphicsSystem->drawBuffer(mspGraphicsBufferManager->getGraphicsBuffer("Background"), Vector2(0, 0));
		//mspGraphicsSystem->clear(Color(160, 32, 240)); // Draw solid color

		// Draw Floors
		for (unsigned int i = 0; i < mFloors.size(); ++i)
		{
			mspGraphicsSystem->addUnit(dynamic_cast<Unit*>(&mFloors[i]));
		}

		// Draw Walls
		for (unsigned int i = 0; i < mWalls.size(); ++i)
		{
			mspGraphicsSystem->addUnit(dynamic_cast<Unit*>(&mWalls[i]));
		}

		// Draw dead WAIzards
		for (unsigned int i = 0; i < mWAIzards.size(); ++i)
		{
			if (mWAIzards[i].getDead())
				mspGraphicsSystem->addUnit(dynamic_cast<Unit*>(&mWAIzards[i]));
		}

		// Draw WAIzards
		for (unsigned int i = 0; i < mWAIzards.size(); ++i)
		{
			if (!mWAIzards[i].getDead())
				mspGraphicsSystem->addUnit(dynamic_cast<Unit*>(&mWAIzards[i]));
		}

		// Draw Spells
		for (unsigned int i = 0; i < mSpells.size(); ++i)
		{
			mspGraphicsSystem->addUnit(dynamic_cast<Unit*>(mSpells[i]));
		}

		mRedrawEverything = false;
	}
	else // ! mRedrawEverything
	{
		// for fps /////////////////////////
		addTileToRedraw(Vector2());

		// Objects are drawn in best order for descending depth, for least vector inserts.

		unsigned int j;

		for (j = 0; j < mTilesToRedraw.size(); ++j)
		{
			// Redraw Background Section
			mspGraphicsSystem->drawBufferSection(mspGraphicsBufferManager->getGraphicsBuffer("Background"), mTilesToRedraw[j], Rect(mTilesToRedraw[j], Vector2(64.0f, 64.0f)));

			// Draw Floors
			for (unsigned int i = 0; i < mFloors.size(); ++i)
			{
				if (mFloors[i].getPosition() == mTilesToRedraw[j])
				{ 
					mspGraphicsSystem->addUnit(dynamic_cast<Unit*>(&mFloors[i]));
				}
			}

			// Draw Walls
			for (unsigned int i = 0; i < mWalls.size(); ++i)
			{
				if (mWalls[i].getPosition() == mTilesToRedraw[j])
				{
					mspGraphicsSystem->addUnit(dynamic_cast<Unit*>(&mWalls[i]));
				}
			}
		}

		// Draw dead WAIzards always
		for (unsigned int i = 0; i < mWAIzards.size(); ++i)
		{
			if (mWAIzards[i].getDead())
				mspGraphicsSystem->addUnit(dynamic_cast<Unit*>(&mWAIzards[i]));
		}

		// Draw WAIzards always
		for (unsigned int i = 0; i < mWAIzards.size(); ++i)
		{
			if (!mWAIzards[i].getDead())
				mspGraphicsSystem->addUnit(dynamic_cast<Unit*>(&mWAIzards[i]));
		}

		// Draw WAIzard bars always
		for (unsigned int i = 0; i < mWAIzards.size(); ++i)
		{
			if (!mWAIzards[i].getDead())
			{
				mWAIzards[i].updateBars();

				if (mDrawHealth)
				{
					mspGraphicsSystem->addUnit(&mWAIzards[i].mHealthBar);
				}

				if (mDrawMana)
				{
					mspGraphicsSystem->addUnit(&mWAIzards[i].mManaBar);
				}
			}
		}

		// Draw Spells always
		for (unsigned int i = 0; i < mSpells.size(); ++i)
		{
			mspGraphicsSystem->addUnit(dynamic_cast<Unit*>(mSpells[i]));
		}
	}

	// fps /////////////////////////
	gs->writeText(Vector2(), *mUIFont, Color(255, 255, 255, 255), std::to_string(mCurrentFps));

	mTilesToRedraw.clear();
	mspGraphicsSystem->drawQueuedUnits();
}

void Game::addTileToRedraw(Vector2 pos)
{
	Vector2 toAdd = pos.findTilePos();

	// Don't add duplicates
	bool foundCopy = false;

	for (unsigned int i = 0; i < mTilesToRedraw.size() && !foundCopy; ++i)
	{
		if (toAdd == mTilesToRedraw[i])
			foundCopy = true;
	}

	if (!foundCopy)
	{
		mTilesToRedraw.push_back(toAdd);
	}
}

void Game::addCornersToRedraw(Vector2 topLeftCorner, float length)
{
	addTileToRedraw(topLeftCorner);
	addTileToRedraw(Vector2(topLeftCorner.x + length, topLeftCorner.y));
	addTileToRedraw(Vector2(topLeftCorner.x, topLeftCorner.y + length));
	addTileToRedraw(Vector2(topLeftCorner.x + length, topLeftCorner.y + length));
}

void Game::updateGame(const float frameLength)
{
	// Update as many times as needed to catch up
	while (mLagTime >= frameLength)
	{
		for (unsigned int i = 0; i < mWAIzards.size(); ++i)
		{
			mWAIzards[i].update();
		}

		for (unsigned int i = 0; i < mSpells.size(); ++i)
		{
			mSpells[i]->update();

			// Clean up
			if (mSpells[i]->getIsTrash())
			{
				delete mSpells[i];
				mSpells.erase(mSpells.begin() + i);
				--i;
			}
		}

		mLagTime -= frameLength;
	}
}

void Game::draw(GraphicsSystem* graphics)
{
	switch (mCurrentState)
	{
	case PLAY:
		drawGame(graphics);
		break;
	case QUIT:
		//
		break;
	default:
		cout << "Game attempted to draw to an invalid state" << endl;
		break;
	}

	mspGraphicsSystem->flipDisplay();
}

void Game::update(const float frameLength)
{
	switch (mCurrentState)
	{
	case PLAY:
		updateGame(frameLength);
		break;
	case QUIT:
		mRunningLoop = false;
		break;
	default:
		cout << "Game attempted to update to an invalid state" << endl;
		break;
	}
}

// The 10% of the code that's being run 90% of the time
void Game::runGameLoop(int targetFrameRate)
{
	const float frameLength = 1.0f / (float)targetFrameRate;
	mCurrentFps = targetFrameRate;

	mRunningLoop = true;

	// Load a level
	loadLevel("Resources/Levels/Level_Star.txt");
	//loadLevel("Resources/Levels/Level_Span.txt");
	//loadLevel("Resources/Levels/Level_Box.txt");
	//loadLevel("Resources/Levels/Level_Experiment.txt");

	while (mRunningLoop)
	{
		// Time Management
		mCurrentTime = (float)(SDL_GetTicks()) / 1000.0f;
		mElapsedTime = mCurrentTime - mPreviousTime;

		// Only add time for the game when not in Play
		if (mCurrentState == PLAY)
			mLagTime += mElapsedTime;

		// FrameRate
		mFpsUpdateTime += mElapsedTime;

		if (mFpsUpdateTime >= 1.0f)
		{
			mFpsUpdateTime -= 1.0f;
			mCurrentFps = mFrameCounter;
			mFrameCounter = 0;
		}

		//--------------Get input--------------//
		mspInputManager->update();

		//---------------Update---------------//
		update(frameLength);

		//----------------Draw----------------//
		draw(mspGraphicsSystem);

		// FrameRate
		++mFrameCounter;

		// End cycle
		mPreviousTime = mCurrentTime;

											  //										| Back to milliseconds for computation   
		//-------Sleep Until Next Frame-------//										V 
		if ((1000.0f / (float)targetFrameRate) > ((float)SDL_GetTicks() - (mCurrentTime * 1000.0f)))
		{
			SDL_Delay((int)(1000.0f / (float)targetFrameRate) - (SDL_GetTicks() - (int)(mCurrentTime * 1000.0f))); //Yay stable framerate!
		}
	}
}