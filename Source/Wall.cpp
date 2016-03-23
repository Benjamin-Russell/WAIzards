#include "Wall.h"

Wall::Wall(Vector2 pos, Sprite* sprite, int id)
{
	mPosition = pos;
	mBoundingBox.mPosition = mPosition;
	mBoundingBox.mSize = Vector2(64.0f, 64.0f);
	mDrawType = SPRITE;
	mSprite = sprite;
	mDepth = 100;
	mID = id;
}