#ifndef _GRAPHICS_SYSTEM_H_
#define _GRAPHICS_SYSTEM_H_

#include "Unit.h"
#include "Color.h"
#include "Font.h"

int initSDL();

class GraphicsSystem
{
public:
	GraphicsSystem(int width, int height);
	GraphicsSystem();
	~GraphicsSystem();

	void toggleFullscreen();
	void drawBuffer(GraphicsBuffer* buffer, Vector2 location);
	void drawBufferSection(GraphicsBuffer* buffer, Vector2 location, Rect section);
	void drawSprite(Sprite* sprite, Vector2 location);
	void drawAnimation(Animation* ani, Vector2 location);
	void writeText(Vector2 location, Font font, Color color, string text);
	void cleanUp();
	void clear(Color color);
	void clear(GraphicsBuffer &buffer, Color color);

	inline int getWidth() { return mScreenSurface->w; };
	inline int getHeight() { return mScreenSurface->h; };

	void addUnit(Unit* toAdd);
	void drawQueuedUnits();
	void flipDisplay();

private:
	SDL_Window* mWindow;
	SDL_Surface* mScreenSurface;

	std::vector<Unit*> mUnitsToDraw;

	bool fullScreen;
};

#endif