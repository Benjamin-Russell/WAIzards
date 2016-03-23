#ifndef _UNIT_H_
#define _UNIT_H_

#include "Animation.h"

enum DrawType
{
	NO_DRAW = 0,
	BUFFER,
	SPRITE,
	ANIMATION,

	NUM_TYPES
};

struct Unit
{
public:
	Unit();
	Unit( Vector2 pos, Vector2 boundingPos, Vector2 size, GraphicsBuffer* buf, int depth, int id);
	Unit( Vector2 pos, Vector2 boundingPos, Vector2 size, Sprite* spr, int depth, int id);
	Unit( Vector2 pos, Vector2 boundingPos, Vector2 size, Animation* ani, int depth, int id);
	~Unit();

	Vector2 mPosition;
	Vector2 mVelocity;
	Rect mBoundingBox;	// For buffers, boundingBox is repurposed as the bufferSection because they are meant as purely visual.
	
	GraphicsBuffer* mBuffer;
	Sprite* mSprite;
	Animation* mAnimation;
	DrawType mDrawType;

	int mDepth;
	int mID;
};

#endif