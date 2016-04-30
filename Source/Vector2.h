#ifndef _VECTOR2_H_
#define _VECTOR2_H_

enum Direction
{
	NO_DIRECTION = -1,
	UP,
	DOWN,
	LEFT,
	RIGHT,
	FORWARD,
	BACKWARD,
	UNTRIED,
	FOCUSED,
	RANDOM,

	RECOVERING
};

enum Object
{
	NO_OBJECT = -1,
	FLOOR,
	WALL,
	WAIZARD,
	SPELL,
	EMPTY
};

struct Vector2
{
public:
	Vector2();
	Vector2(float x_, float y_);
	~Vector2();

	float x,
		  y;

	Vector2& Vector2::operator+=(const Vector2 &vector);
	Vector2& Vector2::operator-=(const Vector2 &vector);
	bool Vector2::operator==(const Vector2 &vector);

	int getDistanceTo(Vector2 vector2);

	Vector2 findTilePos(); // assumes 64 pixel tiles
private:
};

struct Rect
{
public:
	Rect();
	Rect(Vector2 pos, Vector2 size);
	~Rect();

	Vector2 mPosition;
	Vector2 mSize; // Width and Height

	bool checkCollision(Rect otherRect);

private:
};

// Generic collision queries
bool checkPointCollision(Rect rect, Vector2 point);
bool checkLineCollision(Object objectType, Vector2 start, int length, Direction dir);
bool checkLineCollision(Object objectType, Vector2 start, int length, Direction dir, int notID);

// Assembly functions defined externally in object file for VectorNew.asm
extern "C" Vector2* findTilePosASM(Vector2* v);
extern "C" bool checkPointCollisionASM(Rect* rect, Vector2* point);
extern "C" bool checkRectCollisionASM(Rect* r1, Rect* r2);

#endif