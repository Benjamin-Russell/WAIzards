#include "Animation.h"

Animation::Animation(float aniSpeed, bool looping)
{
	mAnimationSpeed = aniSpeed;
	mCurrentSprite = 0;
	mLooping = looping;
	timeLastUpdated = -1.0f;
	timeUntilNextSprite = mAnimationSpeed;
}

Animation::~Animation()
{

}

void Animation::update(float currentTime)
{
	if ((mSprites.size() > 0) && (mAnimationSpeed > 0.0f) && (mLooping || mCurrentSprite != mSprites.size() - 1))
	{
		if (timeLastUpdated != -1.0f)
		{
			timeUntilNextSprite -= currentTime - timeLastUpdated;

			while (timeUntilNextSprite <= 0)
			{
				++mCurrentSprite;
				if (mCurrentSprite >= mSprites.size())
					mCurrentSprite = 0;

				timeUntilNextSprite += mAnimationSpeed;
			}
		}
	}

	timeLastUpdated = currentTime;
}

void Animation::addSprite(Sprite* newSprite)
{
	mSprites.push_back(newSprite);
}

void Animation::clearSprites()
{
	for (unsigned int i = 0; i < mSprites.size(); ++i)
	{
		delete mSprites[i];
	}

	mSprites.clear();
}

void Animation::setAnimationSpeed(float speed)
{
	mAnimationSpeed = speed;
}

void Animation::setCurrentSprite(unsigned int index)
{
	mCurrentSprite = index;
}

void Animation::setLooping(bool loop)
{
	mLooping = loop;
}