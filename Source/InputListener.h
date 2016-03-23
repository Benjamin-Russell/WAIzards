#ifndef _INPUT_LISTENER_H_
#define _INPUT_LISTENER_H_

#include "EventSystem.h"

class InputListener : public EventListener
{
public:
	InputListener();
	~InputListener();

	void handleEvent(const Event& theEvent);

private:
	static int msID;
	int mID;


};

#endif