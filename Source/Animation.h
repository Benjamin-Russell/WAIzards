#ifndef _Animation_H_
#define _Animation_H_

#include <vector>

#include "Sprite.h"

class Animation
{
public:
	Animation(){};
	Animation(float aniSpeed, bool looping);
	~Animation();

	void update(float currentTime);

	inline Sprite* getSprite(int index) { return mSprites[index]; }
	inline float getAnimationSpeed() { return mAnimationSpeed; }
	inline int getCurrentSprite() { return mCurrentSprite; }
	inline int getNumSprites() { return mSprites.size(); }
	inline bool getLooping() { return mLooping; }

	void addSprite(Sprite* newSprite);
	void clearSprites();
	void setAnimationSpeed(float speed);
	void setCurrentSprite(unsigned int index);
	void setLooping(bool loop);

private:
	std::vector<Sprite*> mSprites;
	float mAnimationSpeed;
	float timeLastUpdated;
	float timeUntilNextSprite;
	unsigned int mCurrentSprite;
	bool mLooping;
};

#endif