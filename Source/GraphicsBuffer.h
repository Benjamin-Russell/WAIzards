#ifndef _GRAPHICS_BUFFER_H_
#define _GRAPHICS_BUFFER_H_

#include <SDL.h>
#include <SDL_image.h>
#include <string>

class GraphicsBuffer
{
public:
	GraphicsBuffer();
	GraphicsBuffer(const char* file);
	GraphicsBuffer(std::string file);
	GraphicsBuffer(SDL_Surface* surf);
	~GraphicsBuffer();

	inline std::string getName() { return mName; }
	inline void setName(std::string newName) { mName = newName; };
	inline int getWidth() { return mpSurface->w; }
	inline int getHeight() { return mpSurface->h; }
	inline SDL_Surface* getSurface() { return mpSurface; }

private:
	friend class GraphicsSystem;
	SDL_Surface* mpSurface;
	std::string mName;
};

#endif