#ifndef _GRAPHICS_BUFFER_MANAGER_H
#define _GRAPHICS_BUFFER_MANAGER_H

#include <map>

#include "GraphicsBuffer.h"

class GraphicsBufferManager
{
public:
	GraphicsBufferManager();
	~GraphicsBufferManager();

	void addGraphicsBuffer(std::string key, GraphicsBuffer* newBuffer);
	void deleteGraphicsBuffer(std::string key);
	void clearBuffers();
	GraphicsBuffer* getGraphicsBuffer(std::string key);

	inline int getSize() { return mGraphicsBuffers.size(); }

private:
	std::map<std::string, GraphicsBuffer*> mGraphicsBuffers;
};

#endif