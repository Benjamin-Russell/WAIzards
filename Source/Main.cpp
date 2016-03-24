#include <iostream>

#include "Game.h"

// SDL Tutorials http://lazyfoo.net/SDL_tutorials/lesson02/index.php

// TODO:

// The longer the AI, the longer the wait to execute AI is? make variable

// Make all movement based on time
// - multiply by a ratio, correct?

// Push spells
// - Waizard to die when flung into the emptiness. (Already works if they move into emptyness).

// Move AI stuff (attacks and passives as well) to AI.h file. Spell to Spell.h

// AI:
// - Action: WAIT, 0-99 * hundredths of a sec?
// - Action: CHANGE_DIRECTION
// - Action: CHANGE_TO_BASIC
// - Action: CHANGE_TO_ULT
// - Action: CHANGE_TO_MOVE
// - Multi condition ifs / actions which allow for this behavior
// - Waizards can set a direction like the timestamps   ////////////////////
// - Add condition for checking direction currently about to move in?
// - SURROUNDING direction for conditions?

// - list for actions to take
// -- Cancel last action
// -- Cancel all actions
// - UNTRIED, Add this to attacks as well?

// BUG: Waizards can clip into eachother when crammed.

// Is move speed redundant?
// -- Check if melee outcomes are favored towards lower ID WAIzards or faster movement speed

// Make passives stack

// Fullscreen toggle non-functional

// See if removing transparency of walls and floors increases draw performance

// Implement all the shit

int main(int argc, char** argv)
{
	// Init Game Objects
	Game::initInstance();

	// Run Game Loop
	Game::getInstance()->runGameLoop(60);

	// End
	Game::getInstance()->cleanUp();
	delete Game::getInstance();

	return 0;
}