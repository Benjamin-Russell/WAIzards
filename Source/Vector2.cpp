#include "Vector2.h"

#include "Game.h"

Vector2::Vector2()
{
	x = 0.0f;
	y = 0.0f;
}

Vector2::Vector2(float x_, float y_)
{
	x = x_;
	y = y_;
}

Vector2::~Vector2()
{

}

Vector2& Vector2::operator+=(const Vector2 &vector)
{
	x += vector.x;
	y += vector.y;

	return *this;
}

Vector2& Vector2::operator-=(const Vector2 &vector)
{
	x -= vector.x;
	y -= vector.y;

	return *this;
}

bool Vector2::operator==(const Vector2 &vector)
{
	if (x == vector.x && y == vector.y)
		return true;
	else
		return false;
}

int Vector2::getDistanceTo(Vector2 vector2)
{
	return (int)(sqrt( pow( vector2.x - x, 2) + pow( vector2.y - y, 2)));
}

Vector2 Vector2::findTilePos()
{
	Vector2 toReturn(0.0f, 0.0f);

	toReturn.x = (float)(((int)x / 64) * 64);
	toReturn.y = (float)(((int)y / 64) * 64);

	return toReturn;
}

Rect::Rect()
{
	mPosition = Vector2(0.0f, 0.0f);
	mSize = Vector2(0.0f, 0.0f);
}

Rect::Rect(Vector2 pos, Vector2 size)
{
	mPosition = pos;
	mSize = size;
}

Rect::~Rect()
{

}

bool Rect::checkCollision(Rect otherRect)
{
	if (mPosition.x + mSize.x >= otherRect.mPosition.x && mPosition.x <= otherRect.mPosition.x + otherRect.mSize.x
		&& mPosition.y + mSize.y >= otherRect.mPosition.y && mPosition.y <= otherRect.mPosition.y + otherRect.mSize.y)
		return true;
	else
		return false;
}

bool checkPointCollision(Rect rect, Vector2 point)
{
	if (point.x >= rect.mPosition.x && point.x <= rect.mPosition.x + rect.mSize.x && point.y >= rect.mPosition.y && point.y <= rect.mPosition.y + rect.mSize.y)
		return true;
	else
		return false;
}

bool checkLineCollision(Object objectType, Vector2 start, int length, Direction dir)
{
	int accuracy = 8;
	int distanceGone = 0;
	Vector2 toAdd = Vector2();
	Vector2 toCheck = start;
	bool foundCollision = false;

	switch (dir)
	{
	case UP:
		toAdd.y -= accuracy;
		start.y -= 24.0f;
		break;
	case DOWN:
		toAdd.y += accuracy;
		start.y += 24.0f;
		break;
	case LEFT:
		toAdd.x -= accuracy;
		start.x -= 24.0f;
		break;
	case RIGHT:
		toAdd.x += accuracy;
		start.y += 24.0f;
		break;
	}

	while (distanceGone <= length && !foundCollision)
	{
		// Check for instances of an object type at current point
		switch (objectType)
		{
		case FLOOR:
			for (unsigned int i = 0; i < Game::getInstance()->getFloorSize() && !foundCollision; ++i)
			{
				//foundCollision = checkPointCollision(Game::getInstance()->getFloorBoundingBox(i), toCheck);
				foundCollision = checkPointCollisionASM(&Game::getInstance()->getFloorBoundingBox(i), &toCheck);
			}
			break;
		case WALL:
			for (unsigned int i = 0; i < Game::getInstance()->getWallSize() && !foundCollision; ++i)
			{
				//foundCollision = checkPointCollision(Game::getInstance()->getWallBoundingBox(i), toCheck);
				foundCollision = checkPointCollisionASM(&Game::getInstance()->getWallBoundingBox(i), &toCheck);
			}
			break;
		case WAIZARD:
			for (unsigned int i = 0; i < Game::getInstance()->getWAIzardSize() && !foundCollision; ++i)
			{
				//foundCollision = checkPointCollision(Game::getInstance()->getWAIzardBoundingBox(i), toCheck);
				foundCollision = checkPointCollisionASM(&Game::getInstance()->getWAIzardBoundingBox(i), &toCheck);

			}
			break;
		case SPELL:
			for (unsigned int i = 0; i < Game::getInstance()->getSpellSize() && !foundCollision; ++i)
			{
				//foundCollision = checkPointCollision(Game::getInstance()->getSpellBoundingBox(i), toCheck);
				foundCollision = checkPointCollisionASM(&Game::getInstance()->getSpellBoundingBox(i), &toCheck);

			}
			break;
		}

		// Advance to next point in line
		toCheck += toAdd;
		distanceGone += accuracy;
	}

	return foundCollision;
}

bool checkLineCollision(Object objectType, Vector2 start, int length, Direction dir, int notID)
{
	int accuracy = 8;
	int distanceGone = 0;
	Vector2 toAdd = Vector2();
	Vector2 toCheck = start;
	bool foundCollision = false;

	switch (dir)
	{
	case UP:
		toAdd.y -= accuracy;
		start.y -= 24.0f;
		break;
	case DOWN:
		toAdd.y += accuracy;
		start.y += 24.0f;
		break;
	case LEFT:
		toAdd.x -= accuracy;
		start.x -= 24.0f;
		break;
	case RIGHT:
		toAdd.x += accuracy;
		start.y += 24.0f;
		break;
	}

	while (distanceGone <= length && !foundCollision)
	{
		// Check for instances of an object type at current point
		switch (objectType)
		{
		case FLOOR:
			for (unsigned int i = 0; i < Game::getInstance()->getFloorSize() && !foundCollision; ++i)
			{
				foundCollision = checkPointCollision(Game::getInstance()->getFloorBoundingBox(i), toCheck);
			}
			break;
		case WALL:
			for (unsigned int i = 0; i < Game::getInstance()->getWallSize() && !foundCollision; ++i)
			{
				foundCollision = checkPointCollision(Game::getInstance()->getWallBoundingBox(i), toCheck);
			}
			break;
		case WAIZARD:
			for (unsigned int i = 0; i < Game::getInstance()->getWAIzardSize() && !foundCollision; ++i)
			{
				if (notID != Game::getInstance()->getWAIzardID(i) && !Game::getInstance()->getWAIzardDead(i))
					foundCollision = checkPointCollision(Game::getInstance()->getWAIzardBoundingBox(i), toCheck);
			}
			break;
		case SPELL:
			for (unsigned int i = 0; i < Game::getInstance()->getSpellSize() && !foundCollision; ++i)
			{
				foundCollision = checkPointCollision(Game::getInstance()->getSpellBoundingBox(i), toCheck);
			}
			break;
		}

		// Advance to next point in line
		toCheck += toAdd;
		distanceGone += accuracy;
	}

	return foundCollision;
}