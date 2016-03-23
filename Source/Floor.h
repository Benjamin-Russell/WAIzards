#ifndef _FLOOR_H_
#define _FLOOR_H_

#include "Unit.h"

class Floor : public Unit
{
public:
	Floor(Vector2 pos, Sprite* sprite, int id);
	~Floor(){};

	inline Vector2 getPosition() { return mPosition; }

private:

};

#endif