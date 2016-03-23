#include "InputManager.h"

#include "Game.h"

InputManager::InputManager()
{
	leftWasDown = false;
	rightWasDown = false;
}

InputManager::~InputManager()
{

}

void InputManager::update()
{
	const Uint8* keystate = SDL_GetKeyboardState(NULL);
	SDL_Event event;

	// Keys held
	if (keystate[SDL_SCANCODE_LEFT])
	{
		gpEventSystem->fireEvent(LEFT_HELD);
		leftWasDown = true;
	}
	else
	{
		if (leftWasDown)
		{
			gpEventSystem->fireEvent(LEFT_RELEASED);
		}

		leftWasDown = false;
	}

	if (keystate[SDL_SCANCODE_RIGHT])
	{
		gpEventSystem->fireEvent(RIGHT_HELD);
		rightWasDown = true;
	}
	else
	{
		if (rightWasDown)
		{
			gpEventSystem->fireEvent(RIGHT_RELEASED);
		}

		rightWasDown = false;
	}

	if (keystate[SDL_SCANCODE_UP])
	{
		gpEventSystem->fireEvent(UP_HELD);
	}

	if (keystate[SDL_SCANCODE_DOWN])
	{
		gpEventSystem->fireEvent(DOWN_HELD);
	}

	if (keystate[SDL_SCANCODE_SPACE])
	{
		gpEventSystem->fireEvent(SPACE_HELD);
	}

	// Keys pressed
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
			Game::getInstance()->setState(QUIT);
		case SDL_KEYDOWN:
			switch (event.key.keysym.sym)
			{
				case SDLK_LEFT:
					gpEventSystem->fireEvent(LEFT_PRESSED);
					break;
				case SDLK_RIGHT:
					gpEventSystem->fireEvent(RIGHT_PRESSED);
					break;
				case SDLK_UP:
					gpEventSystem->fireEvent(UP_PRESSED);
					break;
				case SDLK_DOWN:
					gpEventSystem->fireEvent(DOWN_PRESSED);
					break;
				case SDLK_ESCAPE:
					gpEventSystem->fireEvent(ESCAPE_PRESSED);
					break;
				case SDLK_RETURN:
					gpEventSystem->fireEvent(ENTER_PRESSED);
					break;
				case SDLK_b:
					gpEventSystem->fireEvent(B_PRESSED);
					break;
				case SDLK_h:
					gpEventSystem->fireEvent(H_PRESSED);
					break;
				case SDLK_f:
					gpEventSystem->fireEvent(F_PRESSED);
					break;
				case SDLK_l:
					gpEventSystem->fireEvent(L_PRESSED);
					break;
				case SDLK_m:
					gpEventSystem->fireEvent(M_PRESSED);
					break;
				case SDLK_1:
					gpEventSystem->fireEvent(NUM_1_PRESSED);
					break;
				case SDLK_2:
					gpEventSystem->fireEvent(NUM_2_PRESSED);
					break;
				case SDLK_3:
					gpEventSystem->fireEvent(NUM_3_PRESSED);
					break;
				case SDLK_4:
					gpEventSystem->fireEvent(NUM_4_PRESSED);
					break;
			}
		}
	}
}