#include "GameListener.h"
#include "Game.h"

int GameListener::msID = 0;

GameListener::GameListener()
{
	mID = msID;
	msID++;

	// Add this listener to each game event in the Event System
	//gpEventSystem->addListener(INVADER_HIT_WALL, this);
}

GameListener::~GameListener()
{

}

void GameListener::handleEvent(const Event &theEvent)
{
	//switch (theEvent.getType())
	//{
	//default:
	//	break;
	//}
}