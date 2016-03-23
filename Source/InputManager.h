#ifndef _INPUT_MANAGER_H_
#define _INPUT_MANAGER_H_

#include <SDL.h>

#include "Vector2.h"


class InputManager
{
public:
	InputManager();
	~InputManager();

	void update();

private:
	SDL_Event mEvent;

	bool leftWasDown;
	bool rightWasDown;
};

#endif