#include "Unit.h"

Unit::Unit()
{
	mPosition = Vector2();
	mVelocity = Vector2();
	mBoundingBox.mPosition = Vector2();
	mBoundingBox.mSize = Vector2();
	mDrawType = NO_DRAW;
	mBuffer = NULL;
	mSprite = NULL;
	mAnimation = NULL;
	mDepth = 0;
	mID = -1;
}

Unit::Unit(Vector2 pos, Vector2 boundingPos, Vector2 size, GraphicsBuffer* buf, int depth, int id)
{
	mPosition = pos;
	mVelocity = Vector2();
	mBoundingBox.mPosition = boundingPos;
	mBoundingBox.mSize = size;

	mDrawType = BUFFER;
	mBuffer = buf;
	mSprite = NULL;;
	mAnimation = NULL;

	mDepth = depth;
	mID = id;
}

Unit::Unit(Vector2 pos, Vector2 boundingPos, Vector2 size, Sprite* spr, int depth, int id)
{
	mPosition = pos;
	mVelocity = Vector2();
	mBoundingBox.mPosition = boundingPos;
	mBoundingBox.mSize = size;

	mDrawType = SPRITE;
	mBuffer = NULL;
	mSprite = spr;
	mAnimation = NULL;

	mDepth = depth;
	mID = id;
}

Unit::Unit(Vector2 pos, Vector2 boundingPos, Vector2 size, Animation* ani, int depth, int id)
{
	mPosition = pos;
	mVelocity = Vector2();
	mBoundingBox.mPosition = boundingPos;
	mBoundingBox.mSize = size;

	mDrawType = ANIMATION;
	mBuffer = NULL;
	mSprite = NULL;
	mAnimation = ani;

	mDepth = depth;
	mID = id;
}

Unit::~Unit()
{
	mBuffer = NULL;
	mSprite = NULL;
	mAnimation = NULL;
}