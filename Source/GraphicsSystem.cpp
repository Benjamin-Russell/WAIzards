#include "GraphicsSystem.h"

GraphicsSystem::GraphicsSystem()
{
	initSDL();

	mWindow = SDL_CreateWindow("WAIzards", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1920, 1080, SDL_WINDOW_SHOWN);
	fullScreen = false;

	if (mWindow == NULL)
	{
		std::cout << "Error creating display: SDL_SetVideoMode() failed" << std::endl;
	}

	mScreenSurface = SDL_GetWindowSurface(mWindow);
}

GraphicsSystem::GraphicsSystem(int width, int height)
{
	initSDL();

	mWindow = SDL_CreateWindow("WAIzards", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);
	fullScreen = false;

	if (mWindow == NULL)
	{
		std::cout << "Error creating display: SDL_CreateWindow() failed" << std::endl;
	}

	mScreenSurface = SDL_GetWindowSurface(mWindow);
}

GraphicsSystem::~GraphicsSystem()
{
	cleanUp();
}

void GraphicsSystem::cleanUp()
{
	// SDL_QUIT takes care of mDisplay, no need to free it
	IMG_Quit();
	SDL_Quit();
}

void GraphicsSystem::addUnit(Unit* toAdd)
{
	unsigned int i;
	bool foundPlace = false;

	for (i = 0; !foundPlace && i < mUnitsToDraw.size(); ++i)
	{
		if (toAdd->mDepth > mUnitsToDraw[i]->mDepth)
		{
			foundPlace = true;
		}
	}

	if (!foundPlace)
	{
		// Add to end of vector
		mUnitsToDraw.push_back(toAdd);
	}
	else
	{
		// Insert at proper place in descending-depth vector
		std::vector<Unit*>::iterator it = mUnitsToDraw.begin() + i - 1;
		mUnitsToDraw.insert(it, toAdd);
	}
}

void GraphicsSystem::drawQueuedUnits()
{
	for (unsigned int i = 0; i < mUnitsToDraw.size(); ++i)
	{
		switch (mUnitsToDraw[i]->mDrawType)
		{
			case BUFFER:
				drawBufferSection(mUnitsToDraw[i]->mBuffer, mUnitsToDraw[i]->mPosition, mUnitsToDraw[i]->mBoundingBox);
				break;

			case SPRITE:
				drawSprite(mUnitsToDraw[i]->mSprite, mUnitsToDraw[i]->mPosition);
				break;

			case ANIMATION:
				drawAnimation(mUnitsToDraw[i]->mAnimation, mUnitsToDraw[i]->mPosition);
				break;
		}
	}

	mUnitsToDraw.clear();
}

void GraphicsSystem::flipDisplay()
{
	SDL_UpdateWindowSurface(mWindow);
}

void GraphicsSystem::clear(Color color)
{
	SDL_FillRect(mScreenSurface, NULL, color.getHex());
}

void GraphicsSystem::clear(GraphicsBuffer& buffer, Color color)
{
	SDL_FillRect(buffer.getSurface(), NULL, color.getHex());
}

void GraphicsSystem::drawBuffer(GraphicsBuffer* buffer, Vector2 location)
{
	SDL_Rect dest;
	dest.x = (int)location.x;
	dest.y = (int)location.y;

	SDL_BlitSurface(buffer->getSurface(), NULL, mScreenSurface, &dest);
}

void GraphicsSystem::drawBufferSection(GraphicsBuffer* buffer, Vector2 location, Rect section)
{
	SDL_Rect dest;
	dest.x = (int)location.x;
	dest.y = (int)location.y;

	SDL_Rect sect;
	sect.x = (int)section.mPosition.x;
	sect.y = (int)section.mPosition.y;
	sect.w = (int)section.mSize.x;
	sect.h = (int)section.mSize.y;

	SDL_BlitSurface(buffer->getSurface(), &sect, mScreenSurface, &dest);
}

void GraphicsSystem::drawSprite(Sprite* sprite, Vector2 location)
{
	SDL_Rect dest;
	dest.x = (int)location.x;
	dest.y = (int)location.y;

	SDL_Rect source;
	source.x = (int)sprite->getSource().x;
	source.y = (int)sprite->getSource().y;
	source.w = sprite->getWidth();
	source.h = sprite->getHeight();

	SDL_BlitSurface(sprite->getBuffer()->getSurface(), &source, mScreenSurface, &dest);
}

void GraphicsSystem::drawAnimation(Animation* ani, Vector2 location)
{
	if (ani->getNumSprites() > 0)
	{
		SDL_Rect dest;
		dest.x = (int)location.x;
		dest.y = (int)location.y;

		SDL_Rect source;
		source.x = (int)ani->getSprite(ani->getCurrentSprite())->getSource().x;
		source.y = (int)ani->getSprite(ani->getCurrentSprite())->getSource().y;
		source.w = ani->getSprite(ani->getCurrentSprite())->getWidth();
		source.h = ani->getSprite(ani->getCurrentSprite())->getHeight();

		SDL_BlitSurface(ani->getSprite(ani->getCurrentSprite())->getBuffer()->getSurface(), &source, mScreenSurface, &dest);
	}
}

void GraphicsSystem::toggleFullscreen()
{
	if (fullScreen)
	{
		SDL_SetWindowFullscreen(mWindow, 0);

		fullScreen = false;
	}
	else
	{
		SDL_SetWindowFullscreen(mWindow, SDL_WINDOW_FULLSCREEN);

		fullScreen = true;
	}
}

void GraphicsSystem::writeText(Vector2 location, Font font, Color color, string text)
{
	SDL_Rect dest;
	dest.x = (int)location.x;
	dest.y = (int)location.y;

	SDL_Surface* tmpSurface = NULL;
	tmpSurface = TTF_RenderText_Solid(font.getFont(), text.c_str(), color.getSDLColor());

	SDL_BlitSurface(tmpSurface, NULL, mScreenSurface, &dest);
}

int initSDL()
{
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
		return 1;
	}

	IMG_Init(IMG_INIT_PNG);

	return 0;
}