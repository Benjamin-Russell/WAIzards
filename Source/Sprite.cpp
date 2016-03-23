#include "Sprite.h"
#include "Game.h"

Sprite::Sprite(GraphicsBuffer* buffer, int width, int height, Vector2 sourcePosition)
{
	mpGraphicsBuffer = buffer;
	mBufferName = mpGraphicsBuffer->getName();
	mWidth = width;
	mHeight = height;
	mSource = sourcePosition;
}

Sprite::Sprite(string buffername, int w, int h, Vector2 s)
{
	mWidth = w;
	mHeight = h;
	mSource = s;
	mpGraphicsBuffer = Game::getInstance()->getGraphicsBufferManager()->getGraphicsBuffer(buffername);
}

Sprite::Sprite()
{
}

Sprite::~Sprite()
{

}

void Sprite::setSource(Vector2 newSource)
{
	mSource = newSource;
}

void Sprite::setWidth(int width)
{
	mWidth = width;
}

void Sprite::setHeight(int height)
{
	mHeight = height;
}