#include "GraphicsBufferManager.h"

GraphicsBufferManager::GraphicsBufferManager()
{

}
GraphicsBufferManager::~GraphicsBufferManager()
{
	clearBuffers();
}

void GraphicsBufferManager::addGraphicsBuffer(std::string key, GraphicsBuffer* newBuffer)
{
	mGraphicsBuffers[key] = newBuffer;
	mGraphicsBuffers[key]->setName(key);
}

void GraphicsBufferManager::clearBuffers()
{
	std::map<std::string, GraphicsBuffer*>::iterator itr;

	for (itr = mGraphicsBuffers.begin(); itr != mGraphicsBuffers.end(); ++itr)
	{
		delete itr->second;
	}
	
	mGraphicsBuffers.clear();
}

void GraphicsBufferManager::deleteGraphicsBuffer(std::string key)
{
	delete mGraphicsBuffers.find(key)->second;
	mGraphicsBuffers.erase(key);
}

GraphicsBuffer* GraphicsBufferManager::getGraphicsBuffer(std::string key)
{
	return mGraphicsBuffers.find(key)->second;
}