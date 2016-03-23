#ifndef _WALL_H_
#define _WALL_H_

#include "Unit.h"

class Wall : public Unit
{
public:
	Wall(Vector2 pos, Sprite* sprite, int id);
	~Wall(){};

	inline Vector2 getPosition() { return mPosition; }
	inline Rect getBoundingBox() { return mBoundingBox; }

private:

};

#endif