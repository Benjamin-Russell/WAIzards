#ifndef _EVENT_LISTENER_H_
#define _EVENT_LISTENER_H_

#include "Event.h"

class EventListener
{
public:
	EventListener(){};
	virtual ~EventListener(){};

	virtual void handleEvent(const Event& theEvent) = 0;

private:

};

#endif