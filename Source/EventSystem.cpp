#include "EventSystem.h"

EventSystem* gpEventSystem = NULL;

EventSystem::EventSystem()
{

}

EventSystem::~EventSystem()
{
	mListenerMap.clear();
}

void EventSystem::addListener(EventType type, EventListener* pListener)
{
	mListenerMap.insert(std::pair<EventType, EventListener*>(type, pListener));
}

void EventSystem::fireEvent(const Event& theEvent)
{
	std::pair<std::multimap<EventType, EventListener*>::iterator, std::multimap<EventType, EventListener*>::iterator> ret;
	ret = mListenerMap.equal_range(theEvent.getType());

	std::multimap<EventType, EventListener*>::iterator iter;
	for (iter = ret.first; iter != ret.second; ++iter)
	{
		iter->second->handleEvent(theEvent);
	}
}