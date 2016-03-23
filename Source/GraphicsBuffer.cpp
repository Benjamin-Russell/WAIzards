#include "GraphicsBuffer.h"
#include <string>

GraphicsBuffer::GraphicsBuffer()
{
	mpSurface = NULL;
}

GraphicsBuffer::GraphicsBuffer(const char* file)
{
	mpSurface = IMG_Load(file);
}

GraphicsBuffer::GraphicsBuffer(std::string file)
{
	mpSurface = IMG_Load(file.c_str());
}

GraphicsBuffer::GraphicsBuffer(SDL_Surface* surf)
{
	mpSurface = surf;
}

GraphicsBuffer::~GraphicsBuffer()
{
	SDL_FreeSurface(mpSurface);
}