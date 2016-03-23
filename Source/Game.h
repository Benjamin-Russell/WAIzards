#ifndef _GAME_H_
#define _GAME_H_

#include <sstream>
#include <fstream>
#include <vector>
#include <random>
#include <ctime>

#include "Animation.h"
#include "GraphicsSystem.h"
#include "GraphicsBufferManager.h"
#include "InputManager.h"
#include "InputListener.h"
#include "GameListener.h"

#include "Floor.h"
#include "Wall.h"
#include "WAIzard.h"

enum GameState 
{
	SPLASH = 0,
	MAIN_MENU,
	OPTIONS_MENU,
	PLAY,
	PAUSE_MENU,
	CREDITS_MENU,
	QUIT,
	NUM_GAMESTATES
};

class Game
{

public:
	~Game();

	static void initInstance();
	void runGameLoop(int targetFrameRate);
	void resetGame();
	void cleanUp();

	void loadLevel(string file);

	// Mutators
	inline void setState(GameState state) { mCurrentState = state; }
	inline void setLagTime(float time) { mLagTime = time; }
	inline void pushStateToStack(GameState state) { mStateStack.push_back(state); }
	inline GameState popStateFromStack(){ GameState toReturn = mStateStack.back(); mStateStack.pop_back(); return toReturn; }

	// Accessors
	inline static Game* getInstance() { return mspInstance; }
	inline GraphicsSystem* getGraphicsSystem() { return mspGraphicsSystem; }
	inline GraphicsBufferManager* getGraphicsBufferManager() { return mspGraphicsBufferManager; }
	inline InputManager* getInputManager() { return mspInputManager; }
	inline InputListener* getInputListener() { return mspInputListener; }
	inline float getCurrentTime() { return mCurrentTime; }
	inline float getPreviousTime() { return mPreviousTime; }
	inline float getElapsedTime() { return mElapsedTime; }
	inline float getLagTime() { return mLagTime; }
	inline float getEndFrameTime() { return mEndFrameTime; }
	inline float getFpsUpdateTime() { return mFpsUpdateTime; }
	inline int getCurrentFps() { return mCurrentFps; }
	inline int getFrameCounter() { return mFrameCounter; }
	inline bool getInitialized() { return msInitialized; }
	inline bool getRunningLoop() { return mRunningLoop; }
	inline GameState getState() { return mCurrentState; }

	int getRandomInt(int max);

	// Object vector Accessors and Mutators
	inline Rect getFloorBoundingBox(int index) { return Rect(Vector2(mFloors[index].getPosition().x, mFloors[index].getPosition().y), Vector2(64.0f, 64.0f)); }
	inline Rect getWallBoundingBox(int index) { return mWalls[index].getBoundingBox(); }
	inline Rect getWAIzardBoundingBox(int index) { return mWAIzards[index].getBoundingBox(); }
	inline WAIzard* getWAIzard(int index) { return &mWAIzards[index]; }
	inline bool getWAIzardDead(int index) { return mWAIzards[index].getDead(); }
	inline Rect getSpellBoundingBox(int index) { return mSpells[index]->getBoundingBox(); }
	inline unsigned int getFloorSize() { return mFloors.size(); }
	inline unsigned int getWallSize() { return mWalls.size(); }
	inline unsigned int getWAIzardSize() { return mWAIzards.size(); }
	inline int getWAIzardID(int num) { return mWAIzards[num].getID(); }
	inline WAIzard* getWAIzardWithID(int id) { for (unsigned int i = 0; i < mWAIzards.size(); ++i) {if (mWAIzards[i].getID() == id) return &mWAIzards[i];} return NULL; }

	inline unsigned int getSpellSize() { return mSpells.size(); }
	inline void fireSpell(Spell* newSpell) { mSpells.push_back(newSpell); }
	inline int getUnitID() { return mUnitIDCounter++; }

	// Efficent drawing
	void addTileToRedraw(Vector2 pos);
	void addCornersToRedraw(Vector2 topLeftCorner, float length);
	bool mDrawHealth;
	bool mDrawMana;

	// Sprites
	std::vector<Sprite*> mFireballSprites;

	Sprite* mpDeadSprite;
	Sprite* mpFloorSprite;
	Sprite* mpWallSprite;
	//temp /////////////////////////
	Sprite* mpWAIzardSprite;

private:
	Game();

	static bool msInitialized;
	bool mRunningLoop;
	std::vector<GameState> mStateStack;	// A stack of game states for handling menus and such
	GameState mCurrentState;

	void update(const float frameLength);
	void draw(GraphicsSystem* graphics);

	void updateGame(const float frameLength);
	void drawGame(GraphicsSystem* graphics);

	// Time vars in seconds (*1000)
	float mCurrentTime;
	float mPreviousTime;
	float mElapsedTime;
	float mLagTime;
	float mEndFrameTime;
	float mFpsUpdateTime;
	
	Font *mUIFont;

	int mCurrentFps;
	int	mFrameCounter;

	std::vector<Floor> mFloors;
	std::vector<Wall> mWalls;
	std::vector<Vector2> mSpawnPoints;
	std::vector<WAIzard> mWAIzards;
	std::vector<Spell*> mSpells;

	bool mRedrawEverything;
	std::vector<Vector2> mTilesToRedraw;

	int mUnitIDCounter;

	static Game* mspInstance;
	static GraphicsSystem* mspGraphicsSystem;
	static GraphicsBufferManager* mspGraphicsBufferManager;
	static InputManager* mspInputManager;
	static InputListener* mspInputListener;
	static GameListener* mspGameListener;
};

#endif