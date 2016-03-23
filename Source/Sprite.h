#ifndef _SPRITE_H_
#define _SPRITE_H_

#include "GraphicsBuffer.h"
#include "Vector2.h"
#include <string>

using namespace std;

class Sprite
{
public:
	Sprite(GraphicsBuffer* buffer, int width, int height, Vector2 sourcePosition);
	~Sprite();
	Sprite(string buffername, int w, int h, Vector2 s);
	Sprite();

	void setSource(Vector2 newSource);
	void setWidth(int width);
	void setHeight(int height);

	inline GraphicsBuffer* getBuffer() { return mpGraphicsBuffer; }
	inline int getWidth() { return mWidth; }
	inline int getHeight() { return mHeight; }
	inline Vector2 getSource() { return mSource; }

private:
	string mBufferName;
	GraphicsBuffer* mpGraphicsBuffer;
	Vector2 mSource;
	int mWidth;
	int mHeight;
};

#endif