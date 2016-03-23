#include "WAIzard.h"
#include "Game.h"

#include <fstream>

WAIzard::WAIzard(Vector2 position, string file)
{
	// My Unit:
	mPosition = position;
	mBoundingBox.mPosition = Vector2(position.x + 16.0f, position.y + 16.0f);
	mBoundingBox.mSize = Vector2(32.0f, 32.0f);
	mSprite = Game::getInstance()->mpWAIzardSprite;
	mDrawType = SPRITE;
	mDepth = 50;
	mID = Game::getInstance()->getUnitID();

	mHealthBar = Unit(position, Vector2(0.0f, 0.0f), Vector2(32.0f, 4.0f), Game::getInstance()->getGraphicsBufferManager()->getGraphicsBuffer("Bars"), 50, Game::getInstance()->getUnitID());
	mManaBar = Unit(position, Vector2(0.0f, 10.0f), Vector2(32.0f, 4.0f), Game::getInstance()->getGraphicsBufferManager()->getGraphicsBuffer("Bars"), 50, Game::getInstance()->getUnitID());

	mMaxHealth = 100;
	mMaxMana = 100.0f;
	mMaxManaRegen = 2.0f;
	mMaxSpeed = 2.0f;
	mFriction = 1.0f;
	mDestination = Vector2();
	mDead = false;
	mLastHitBy = -1;

	mTimelastActed = Game::getInstance()->getCurrentTime() + ((float)Game::getInstance()->getRandomInt(50) / 100.0f); // One second random start
	mAICoolDown = 0.5f;

	mCurrentFacing = DOWN;

	mFocusDirection = DOWN;
	mTimeMark1 = Game::getInstance()->getCurrentTime();
	mTimeMark2 = Game::getInstance()->getCurrentTime();

	mTimeLastHit = -1.0f;
	mTimeLastPoisoned = -1.0f;
	mTimeToBePoisoned = 0.0f;
	mTimeLastBasic = -120.0f;
	mTimeLastUltimate = -120.0f;
	mBasicCoolDown = 1.0f;
	mUltimateCoolDown = 10.0f;

	mMoveFailed = false;

	// CoolDowns: (Of non-standard length)
	switch (mBasicAttack)
	{
		case SNIPE:
			mBasicCoolDown = 3.0f;
			break;
	}

	switch (mUltimateAttack)
	{
		case TITAN_SHIELD:
			mUltimateCoolDown = 16.0f;
			break;
	}

	// Variations via passives
	if (mPassive1 == LIFE_ALIGNMENT || mPassive2 == LIFE_ALIGNMENT)
	{
		mMaxHealth += 20;
	}

	if (mPassive1 == MANA_ALIGNMENT || mPassive2 == MANA_ALIGNMENT)
	{
		mMaxMana += 20;
	}

	if (mPassive1 == ROCK_ALIGNMENT || mPassive2 == ROCK_ALIGNMENT)
	{
		mMaxSpeed -= 0.5f;
		mFriction += 0.5f;
	}

	if (mPassive1 == WIND_ALIGNMENT || mPassive2 == WIND_ALIGNMENT)
	{
		mMaxSpeed += 0.5f;
	}

	if (mPassive1 == BLOOD_RUNNER || mPassive2 == BLOOD_RUNNER)
	{
		mMaxSpeed -= 1.0f;
	}

	if (mPassive1 == MANA_STAR || mPassive2 == MANA_STAR)
	{
		mMaxManaRegen += 0.1f;
	}

	if (mPassive1 == BLOOD_MAGE || mPassive2 == BLOOD_MAGE)
	{
		mMaxManaRegen -= 0.15f;
	}

	mCurrentHealth = mMaxHealth;
	mCurrentMana = mMaxMana;
	mCurrentManaRegen = mMaxManaRegen;
	manaForBasic = 0.0f;
	manaForUltimate = 0.0f;
	mCurrentSpeed = mMaxSpeed;

	mTotalDamageDealt = 0;
	mMeleeDamage = 25;

	loadAI(file);
}

WAIzard::~WAIzard()
{
	mAI.clear();

	for (unsigned int i = 0; i < mActionsToTake.size(); ++i)
	{
		delete mActionsToTake[i];
	}
	mActionsToTake.clear();
}

DirectionsTried::DirectionsTried()
{
	reset();
}

void DirectionsTried::reset()
{
	for (int i = 0; i < 4; ++i)
	{
		mDirections[i] = false;
	}
}

Direction DirectionsTried::getUntriedDirection()
{
	if (!mDirections[0] || !mDirections[1] || !mDirections[2] || !mDirections[3])
	{
		int check;
		bool foundOne = false;

		while (!foundOne)
		{
			check = Game::getInstance()->getRandomInt(4);

			if (!mDirections[check])
			{
				foundOne = true;
			}
		}

		mDirections[check] = true;
		return (Direction)check;

	}
	else
	{
		// If all have been tried, just return a random direction
		return (Direction)Game::getInstance()->getRandomInt(4);
	}

}

void DirectionsTried::setTry(Direction dir)
{
	mDirections[(int)dir] = true;
}

void DirectionsTried::unsetTry(Direction toUnset)
{
	mDirections[(int)toUnset] = false;
}

void WAIzard::updateBars()
{
	mHealthBar.mBoundingBox.mSize = Vector2(32.0f * mCurrentHealth / mMaxHealth, 4.0f);
	mManaBar.mBoundingBox.mSize = Vector2(32.0f * mCurrentMana / mMaxMana, 4.0f);
}

void WAIzard::dealDamage(int damage, Vector2 push, int id)
{ 
	mCurrentHealth -= damage; 

	mLastHitBy = id; 
	mTimeLastHit = Game::getInstance()->getCurrentTime();

	if (push.x != 0 || push.y != 0)
	{
		mVelocity += push;
		mMoveFailed = false;
		mMoveDirection = RECOVERING;
	}
}

void WAIzard::update()
{
	if (!mDead)
	{
		// Fly through the air
		mPosition += mVelocity;

		// Walk
		if (mMoveDirection != NO_DIRECTION && mMoveDirection != RECOVERING)
		{
			switch (mMoveDirection)
			{
				case UP:
					mPosition.y -= mCurrentSpeed;
					break;
				case DOWN:
					mPosition.y += mCurrentSpeed;
					break;
				case LEFT:
					mPosition.x -= mCurrentSpeed;
					break;
				case RIGHT:
					mPosition.x += mCurrentSpeed;
					break;
			}
		}

		// Update bounding box
		mBoundingBox = Rect(Vector2(mPosition.x + 16.0f, mPosition.y + 16.0f), Vector2(32.0f, 32.0f));

		// Moving normally
		if (mMoveDirection != NO_DIRECTION && mMoveDirection != RECOVERING)
		{
			// Snap to destination if past it
			switch (mMoveDirection)
			{
				case UP:
					if (mPosition.y < mDestination.y)
					{
						mPosition.y = mDestination.y;
					}
					break;
				case DOWN:
					if (mPosition.y > mDestination.y)
					{
						mPosition.y = mDestination.y;
					}
					break;
				case LEFT:
					if (mPosition.x < mDestination.x)
					{
						mPosition.x = mDestination.x;
					}
					break;
				case RIGHT:
					if (mPosition.x > mDestination.x)
					{
						mPosition.x = mDestination.x;
					}
					break;
			}

			// Stop once aligned again during normal movement
			if (((int)mPosition.x % 64 == 0 && (int)mPosition.y % 64 == 0))
			{
				mMoveDirection = NO_DIRECTION;

				if (!mMoveFailed)
				{
					mDirectionsTried.reset();
				}

				// Check if walked off the edge
				bool foundFloor = false;
	
				for (unsigned int i = 0; i < Game::getInstance()->getFloorSize() && !foundFloor; ++i)
				{
					foundFloor = mBoundingBox.checkCollision(Game::getInstance()->getFloorBoundingBox(i));
				}

				if (!foundFloor)
				{
					mCurrentHealth = 0;
					mDead = true;
				}
			}

			// Run into other stuff while moving, as long as the current move isn't already a failure
			else if (!mMoveFailed)
			{
				// Run into walls
				for (unsigned int i = 0; i < Game::getInstance()->getWallSize() && !mMoveFailed; ++i)
				{
					mMoveFailed = mBoundingBox.checkCollision(Game::getInstance()->getWallBoundingBox(i));
				}

				// Run into other wAIzards
				for (unsigned int i = 0; i < Game::getInstance()->getWAIzardSize() && !mMoveFailed; ++i)
				{
				
					if (Game::getInstance()->getWAIzardID(i) != mID && !Game::getInstance()->getWAIzardDead(i))
					{
						mMoveFailed = mBoundingBox.checkCollision(Game::getInstance()->getWAIzardBoundingBox(i));
					
						if (mMoveFailed) // Deal melee damage
						{
							Game::getInstance()->getWAIzard(i)->dealDamage(mMeleeDamage, Vector2(), mID);
							mTotalDamageDealt += mMeleeDamage;
						}
					}
				}

				// Reverse Directions
				if (mMoveFailed)
				{
					switch (mMoveDirection)
					{
					case UP:
						mMoveDirection = DOWN;
						mPosition.y += mCurrentSpeed;
						mDestination = Vector2(mPosition.x, mPosition.findTilePos().y + 64.0f);
						break;
					case DOWN:
						mMoveDirection = UP;
						mPosition.y -= mCurrentSpeed;
						mDestination = Vector2(mPosition.x, mPosition.findTilePos().y);
						break;
					case LEFT:
						mMoveDirection = RIGHT;
						mPosition.x += mCurrentSpeed;
						mDestination = Vector2(mPosition.findTilePos().x + 64.0f, mPosition.y);
						break;
					case RIGHT:
						mMoveDirection = LEFT;
						mPosition.x -= mCurrentSpeed;
						mDestination = Vector2(mPosition.findTilePos().x, mPosition.y);
						break;
					}
				}

				mBoundingBox = Rect(Vector2(mPosition.x + 16.0f, mPosition.y + 16.0f), Vector2(32.0f, 32.0f));
			}
		}

		// Friction
		else if (mMoveDirection == RECOVERING)
		{
			if (mVelocity.x != 0.0f)
			{
				if (mVelocity.x > 0.0f)
				{
					mVelocity.x -= mFriction;

					if (mVelocity.x < 0.0f)
					{
						mVelocity.x = 0.0f;
					}
				}
				else
				{
					mVelocity.x += mFriction;

					if (mVelocity.x > 0.0f)
					{
						mVelocity.x = 0.0f;
					}
				}
			}
		
			if (mVelocity.y != 0.0f)
			{
				if (mVelocity.y > 0.0f)
				{
					mVelocity.y -= mFriction;

					if (mVelocity.y < 0.0f)
					{
						mVelocity.y = 0.0f;
					}
				}
				else
				{
					mVelocity.y += mFriction;

					if (mVelocity.y > 0.0f)
					{
						mVelocity.y = 0.0f;
					}
				}
			}

			// Run into walls with velocity
			if (mVelocity.x != 0.0f || mVelocity.y != 0.0f)
			{
				bool hit = false;

				for (unsigned int i = 0; i < Game::getInstance()->getWallSize() && !hit; ++i)
				{
					//////////////////////////////////////
					//if (hits)
					{
						//glance off, else stop completely
						mVelocity = Vector2(0.0f, 0.0f);
					}
				}
			}
		}

		// Recover to alignment from push
		if (mMoveDirection == RECOVERING && mVelocity.x == 0.0f && mVelocity.y == 0.0f && ((int)mPosition.x % 64 != 0 || (int)mPosition.y % 64 != 0))
		{ 
			// First, decide open space to move to	//////////////////////////////////
		
			// Move

			// When recovered
			if ((int)mPosition.x % 64 == 0 && (int)mPosition.y % 64 == 0)
			{
				mMoveDirection = NO_DIRECTION;
				mDirectionsTried.reset();
			}
		}

		// Compute dynamic max mana regen (include all relevant passives)
		if (mPassive1 == BLOOD_MAGE || mPassive2 == BLOOD_MAGE)
		{
			mMaxManaRegen = 0.1f + (0.5f * (1.0f - (float)mCurrentHealth / (float)mMaxHealth));

			if (mPassive1 == MANA_STAR || mPassive2 == MANA_STAR)									//FIXXXXXXX, and add stacking
			{
				mMaxManaRegen += 0.1f;
			}
		}

		// Compute current mana regen (include all relevant debuffs)
		mCurrentManaRegen = mMaxManaRegen;

		// Compute dynamic max speed (include all relevant passives)
		if (mPassive1 == BLOOD_RUNNER || mPassive2 == BLOOD_RUNNER)
		{
			mMaxSpeed = 1.0f + (2.0f * (1.0f - (float)mCurrentHealth / (float)mMaxHealth));
		}

		if (mPassive1 == ROCK_ALIGNMENT || mPassive2 == ROCK_ALIGNMENT)
		{
			mMaxSpeed -= 0.5f;
		}

		if (mPassive1 == WIND_ALIGNMENT || mPassive2 == WIND_ALIGNMENT)
		{
			mMaxSpeed += 0.5f;
		}

		// Compute current speed (include all relevant debuffs)
		mCurrentSpeed = mMaxSpeed;

		// Check that WAIzard is not moving, position is aligned with the grid, and it's been long enough since last action
		if (mMoveDirection == NO_DIRECTION && Game::getInstance()->getCurrentTime() - mTimelastActed >= mAICoolDown)
		{
			// Regen mana
			mCurrentMana += mCurrentManaRegen;
			if (mCurrentMana > mMaxMana)
				mCurrentMana = mMaxMana;

			// Run AI
			executeAI();
		}

		// Update bar positions
		mHealthBar.mPosition = Vector2(mPosition.x + 18.0f, mPosition.y + 51.0f);
		mManaBar.mPosition = Vector2(mPosition.x + 18.0f, mPosition.y + 55.0f);

		if (mCurrentHealth <= 0)
		{
			// Die
			mDepth = 60;
			mSprite = Game::getInstance()->mpDeadSprite;
			mDead = true;
		}
	}
	else // if mDead
	{
		// Nothing currently missing here
	}

	// Add tiles to redraw at four corners of WAIzard
	Game::getInstance()->addCornersToRedraw(mPosition, 60.0f);
}

void WAIzard::executeAI()
{
	bool doAction;
	Vector2 toCheck;
	bool lastOutcome = false;	// For else statements. First condition cannot be "Else"
	Direction UntriedDirToTry = NO_DIRECTION; // When adding new action, if an Untried direction was going to be tried, set it back to Untried

	// Reset actions vector
	for (unsigned int i = 0; i < mActionsToTake.size(); ++i)
	{
		delete mActionsToTake[i];
	}
	mActionsToTake.clear();

	// There must always be a DO_NOTHING action at index 0
	mActionsToTake.push_back(new AIBlock::BlockActionSet());
	mActionsToTake[0]->mAction = DO_NOTHING;

	for (unsigned int i = 0; i < mAI.size(); ++i)
	{
		doAction = false;

		switch (mAI[i].mConditionSet.mCondition)
		{
		case CHECK_TIME_MARK1:
			if (mAI[i].mConditionSet.mMoreOrLessThan)
			{
				if (Game::getInstance()->getCurrentTime() - mTimeMark1 >= (float)mAI[i].mConditionSet.mConditionAmount / 10.0f)	// Condition amount is in tenths of a second
				{
						doAction = true;

						if (mAI[i].mConditionSet.mResetTimer)
							mTimeMark1 = Game::getInstance()->getCurrentTime();
				}
			}
			else
			{
				if (Game::getInstance()->getCurrentTime() - mTimeMark1 < (float)mAI[i].mConditionSet.mConditionAmount / 10.0f)	// Condition amount is in tenths of a second
				{
					doAction = true;

					if (mAI[i].mConditionSet.mResetTimer)
						mTimeMark1 = Game::getInstance()->getCurrentTime();
				}
			}
			break;

		case CHECK_TIME_MARK2:
			if (mAI[i].mConditionSet.mMoreOrLessThan)
			{
				if (Game::getInstance()->getCurrentTime() - mTimeMark2 >= (float)mAI[i].mConditionSet.mConditionAmount / 10.0f)
				{
						doAction = true;

						if (mAI[i].mConditionSet.mResetTimer)
							mTimeMark2 = Game::getInstance()->getCurrentTime();
				}
			}
			else
			{
				if (Game::getInstance()->getCurrentTime() - mTimeMark2 < (float)mAI[i].mConditionSet.mConditionAmount / 10.0f)
				{
					doAction = true;

					if (mAI[i].mConditionSet.mResetTimer)
						mTimeMark2 = Game::getInstance()->getCurrentTime();
				}
			}
			break;

		case CHECK_HEALTH:
			if (mAI[i].mConditionSet.mMoreOrLessThan) // True = More, False = Less
			{
				if ((float)mCurrentHealth / (float)mMaxHealth > (float)mAI[i].mConditionSet.mConditionAmount / 100.0f)
				{
					doAction = true;
				}
			}
			else
			{
				if ((float)mCurrentHealth / (float)mMaxHealth < (float)mAI[i].mConditionSet.mConditionAmount / 100.0f)
				{
					doAction = true;
				}
			}
			break;

		case CHECK_MANA:
			if (mAI[i].mConditionSet.mMoreOrLessThan) // True = More, False = Less
			{
				if ((float)mCurrentMana / (float)mMaxMana >(float)mAI[i].mConditionSet.mConditionAmount / 100.0f)
				{
						doAction = true;
				}
			}
			else
			{
				if ((float)mCurrentMana / (float)mMaxMana < (float)mAI[i].mConditionSet.mConditionAmount / 100.0f)
				{
					doAction = true;
				}
			}
			break;

		case CHECK_POSITION:

			// Relative directions computed during AI execution
			if (mAI[i].mConditionSet.mConditionDirection == FORWARD)
			{
				mAI[i].mConditionSet.mConditionPosition.x = 0;
				mAI[i].mConditionSet.mConditionPosition.y = 0;

				switch (mCurrentFacing)
				{
				case UP:
					mAI[i].mConditionSet.mConditionPosition.y = -1;
					break;
				case DOWN:
					mAI[i].mConditionSet.mConditionPosition.y = 1;
					break;
				case LEFT:
					mAI[i].mConditionSet.mConditionPosition.x = -1;
					break;
				case RIGHT:
					mAI[i].mConditionSet.mConditionPosition.x = 1;
					break;
				}

			}
			else if (mAI[i].mConditionSet.mConditionDirection == BACKWARD)
			{
				mAI[i].mConditionSet.mConditionPosition.x = 0;
				mAI[i].mConditionSet.mConditionPosition.y = 0;

				switch (mCurrentFacing)
				{
				case UP:
					mAI[i].mConditionSet.mConditionPosition.y = 1;
					break;
				case DOWN:
					mAI[i].mConditionSet.mConditionPosition.y = -1;
					break;
				case LEFT:
					mAI[i].mConditionSet.mConditionPosition.x = 1;
					break;
				case RIGHT:
					mAI[i].mConditionSet.mConditionPosition.x = -1;
					break;
				}
			}
			else if (mAI[i].mConditionSet.mConditionDirection == FOCUSED)
			{
				mAI[i].mConditionSet.mConditionPosition.x = 0;
				mAI[i].mConditionSet.mConditionPosition.y = 0;

				switch (mFocusDirection)
				{
				case UP:
					mAI[i].mConditionSet.mConditionPosition.y = -1;
					break;
				case DOWN:
					mAI[i].mConditionSet.mConditionPosition.y = 1;
					break;
				case LEFT:
					mAI[i].mConditionSet.mConditionPosition.x = -1;
					break;
				case RIGHT:
					mAI[i].mConditionSet.mConditionPosition.x = 1;
					break;
				}
			}

			// Set place to check
			toCheck = Vector2(mPosition.x + 32.0f + (mAI[i].mConditionSet.mConditionPosition.x * 64.0f), mPosition.y + 32.0f + (mAI[i].mConditionSet.mConditionPosition.y * 64.0f));

			switch (mAI[i].mConditionSet.mConditionObject)
			{
			case FLOOR:
				for (unsigned int i = 0; i < Game::getInstance()->getFloorSize() && !doAction; ++i)
				{
					doAction = checkPointCollision(Game::getInstance()->getFloorBoundingBox(i), toCheck);
				}
				break;

			case WALL:
				for (unsigned int i = 0; i < Game::getInstance()->getWallSize() && !doAction; ++i)
				{
					doAction = checkPointCollision(Game::getInstance()->getWallBoundingBox(i), toCheck);
				}
				break;

			case WAIZARD:
				for (unsigned int i = 0; i < Game::getInstance()->getWAIzardSize() && !doAction; ++i)
				{
					if (mID != Game::getInstance()->getWAIzardID(i) && !Game::getInstance()->getWAIzardDead(i))
						doAction = checkPointCollision(Game::getInstance()->getWAIzardBoundingBox(i), toCheck);
				}
				break;

			case SPELL:
				for (unsigned int i = 0; i < Game::getInstance()->getSpellSize() && !doAction; ++i)
				{
					doAction = checkPointCollision(Game::getInstance()->getSpellBoundingBox(i), toCheck);
				}
				break;

			case EMPTY:
				// If there is no wall or floor at position
				bool foundObject = false;
				unsigned int i;

				for (i = 0; i < Game::getInstance()->getWallSize() && !foundObject; ++i)
				{
					foundObject = checkPointCollision(Game::getInstance()->getWallBoundingBox(i), toCheck);
				}

				for (i = 0; i < Game::getInstance()->getFloorSize() && !foundObject; ++i)
				{
					foundObject = checkPointCollision(Game::getInstance()->getFloorBoundingBox(i), toCheck);
				}
			
				if (!foundObject)
					doAction = true;
				break;
			}
			break;

		case CHECK_COLLISION_LINE:																											         // 64 / 8, for tile size and reducing conditionAmount to only useful distances
			doAction = checkLineCollision(mAI[i].mConditionSet.mConditionObject, Vector2(mPosition.x + 32.0f, mPosition.y + 32.0f), (mAI[i].mConditionSet.mConditionAmount * 8) + 64, relativizeDirection(mAI[i].mConditionSet.mConditionDirection), mID);
			break;

		case CHECK_RANDOM:
			if (Game::getInstance()->getRandomInt(100) > mAI[i].mConditionSet.mConditionAmount)
			{
				doAction = true;
			}
			break;

		case CHECK_ATTACK:
			if (mActionsToTake[mActionsToTake.size() - 1]->mAction == SHOOT_BASIC || mActionsToTake[mActionsToTake.size() - 1]->mAction == SHOOT_ULT)
			{
				doAction = true;
			}
			break;

		case CHECK_MOVE:
			if (mActionsToTake[mActionsToTake.size() - 1]->mAction == MOVE)
			{
				doAction = true;
			}
			break;

		case DO_ACTION:
			doAction = true;
			break;

		case ELSE:
			if (lastOutcome == false)
			{
				doAction = true;
			}
			break;
		}

		if (mAI[i].mConditionSet.mNegateCondition)
		{
			doAction = !doAction;
		}

		// Save outcome for an else
		lastOutcome = doAction;

		// Add action to top of action vector if condition is true
		if (doAction)
		{
			// If previous action was to use an Untried direction, set it back to Untried
			if (UntriedDirToTry != NO_DIRECTION)
			{
				mDirectionsTried.unsetTry(UntriedDirToTry);
				UntriedDirToTry = NO_DIRECTION;
			}

			// Action vector iterator
			vector<AIBlock::BlockActionSet*>::iterator iter;

			switch (mAI[i].mActionSet.mAction)
			{
			case DO_NOTHING:
				mActionsToTake.push_back(new AIBlock::BlockActionSet());
				break;

			case SHOOT_BASIC:
				if (Game::getInstance()->getCurrentTime() - mTimeLastBasic >= mBasicCoolDown && mCurrentMana >= manaForBasic)
				{
					mActionsToTake.push_back(new AIBlock::BlockActionSet());
					mActionsToTake[mActionsToTake.size() - 1]->mAction = SHOOT_BASIC;
					mActionsToTake[mActionsToTake.size() - 1]->mActionDirection = relativizeDirection(mAI[i].mActionSet.mActionDirection);

					if (mAI[i].mActionSet.mActionDirection == UNTRIED)
						UntriedDirToTry = mActionsToTake[mActionsToTake.size() - 1]->mActionDirection;
				}
				break;

			case SHOOT_ULT:
				if (Game::getInstance()->getCurrentTime() - mTimeLastUltimate >= mUltimateCoolDown && mCurrentMana >= manaForUltimate)
				{
					mActionsToTake.push_back(new AIBlock::BlockActionSet());
					mActionsToTake[mActionsToTake.size() - 1]->mAction = SHOOT_ULT;
					mActionsToTake[mActionsToTake.size() - 1]->mActionDirection = relativizeDirection(mAI[i].mActionSet.mActionDirection);

					if (mAI[i].mActionSet.mActionDirection == UNTRIED)
						UntriedDirToTry = mActionsToTake[mActionsToTake.size() - 1]->mActionDirection;
				}
				break;

			case MOVE:
				mActionsToTake.push_back(new AIBlock::BlockActionSet());
				mActionsToTake[mActionsToTake.size() - 1]->mAction = MOVE;
				mActionsToTake[mActionsToTake.size() - 1]->mActionDirection = relativizeDirection(mAI[i].mActionSet.mActionDirection);

				if (mAI[i].mActionSet.mActionDirection == UNTRIED)
					UntriedDirToTry = mActionsToTake[mActionsToTake.size() - 1]->mActionDirection;
				break;

			case CANCEL_NOTHING:
				if (mActionsToTake.size() > 1 && mActionsToTake[mActionsToTake.size() - 1]->mAction == DO_NOTHING)
				{
					// If current action is a DO_NOTHING (and not index 0), remove it
					delete mActionsToTake[mActionsToTake.size() - 1];
					mActionsToTake.pop_back();
				}
				break;

			case CANCEL_ATTACK:
				if (mActionsToTake[mActionsToTake.size() - 1]->mAction == SHOOT_BASIC || mActionsToTake[mActionsToTake.size() - 1]->mAction == SHOOT_ULT)
				{
					// If current action is an attack, remove it
					delete mActionsToTake[mActionsToTake.size() - 1];
					mActionsToTake.pop_back();
				}
				break;

			case CANCEL_ALL_ATTACKS:
				for (unsigned int j = 0; j < mActionsToTake.size(); ++j)
				{
					if (mActionsToTake[j]->mAction == SHOOT_BASIC || mActionsToTake[j]->mAction == SHOOT_ULT)
					{
						iter = mActionsToTake.begin() + j;

						delete mActionsToTake[j];
						mActionsToTake.erase(iter);

						--j;
					}
				}
				break;

			case CANCEL_MOVE:
				if (mActionsToTake[mActionsToTake.size() - 1]->mAction == MOVE)
				{
					// If current action is a move, remove it
					delete mActionsToTake[mActionsToTake.size() - 1];
					mActionsToTake.pop_back();
				}
				break;

			case CANCEL_ALL_MOVES:
				for (unsigned int j = 0; j < mActionsToTake.size(); ++j)
				{
					if (mActionsToTake[j]->mAction == MOVE)
					{
						iter = mActionsToTake.begin() + j;

						delete mActionsToTake[j];
						mActionsToTake.erase(iter);

						--j;
					}
				}
				break;

			case CANCEL_TWO_ACTIONS:
				if (mActionsToTake.size() > 1)
				{
					// If current action is not index 0, delete it
					delete mActionsToTake[mActionsToTake.size() - 1];
					mActionsToTake.pop_back();
				}
				// No Break
			case CANCEL_ACTION:
				if (mActionsToTake.size() > 1)
				{
					// If current action is not index 0, delete it
					delete mActionsToTake[mActionsToTake.size() - 1];
					mActionsToTake.pop_back();
				}
				break;

			case CHANGE_DIRECTION:
				mActionsToTake[mActionsToTake.size() - 1]->mActionDirection = mAI[i].mActionSet.mActionDirection;
				break;

			case CHANGE_TO_BASIC:
				if (Game::getInstance()->getCurrentTime() - mTimeLastBasic >= mBasicCoolDown && mCurrentMana >= manaForBasic)
				{
					mActionsToTake[mActionsToTake.size() - 1]->mAction = SHOOT_BASIC;
			
					if (mActionsToTake[mActionsToTake.size() - 1]->mActionDirection == NO_DIRECTION)
						mActionsToTake[mActionsToTake.size() - 1]->mActionDirection = mCurrentFacing;
				}
				break;

			case CHANGE_TO_ULT:
				if (Game::getInstance()->getCurrentTime() - mTimeLastUltimate >= mUltimateCoolDown && mCurrentMana >= manaForUltimate)
				{
					mActionsToTake[mActionsToTake.size() - 1]->mAction = SHOOT_ULT;

					if (mActionsToTake[mActionsToTake.size() - 1]->mActionDirection == NO_DIRECTION)
						mActionsToTake[mActionsToTake.size() - 1]->mActionDirection = mCurrentFacing;
				}
				break;

			case CHANGE_TO_MOVE:
				mActionsToTake[mActionsToTake.size() - 1]->mAction = MOVE;

				if (mActionsToTake[mActionsToTake.size() - 1]->mActionDirection == NO_DIRECTION)
					mActionsToTake[mActionsToTake.size() - 1]->mActionDirection = mCurrentFacing;
				break;
				
			case SET_TIME_MARK1:
				mTimeMark1 = Game::getInstance()->getCurrentTime();
				break;

			case SET_TIME_MARK2:
				mTimeMark2 = Game::getInstance()->getCurrentTime();
				break;

			case SET_FOCUS:
				mFocusDirection = mAI[i].mActionSet.mActionDirection;
				break;

			case SET_FOCUS_CURRENT_DIR:
				mFocusDirection = mActionsToTake[mActionsToTake.size() - 1]->mActionDirection;

				// If no direction, default to current facing
				if (mFocusDirection == NO_DIRECTION)
					mFocusDirection = mCurrentFacing;
				break;
			}
		}
	}

	// After all the AI has been run, perform chosen action
	switch (mActionsToTake[mActionsToTake.size() - 1]->mAction)
	{
	case MOVE:
		mCurrentFacing = mActionsToTake[mActionsToTake.size() - 1]->mActionDirection;
		mMoveDirection = mCurrentFacing;

		// Set destination
		switch (mCurrentFacing)
		{
		case UP:
			mDestination = Vector2(mPosition.x, mPosition.y - 64.0f);
			break;
		case DOWN:
			mDestination = Vector2(mPosition.x, mPosition.y + 64.0f);
			break;
		case LEFT:
			mDestination = Vector2(mPosition.x - 64.0f, mPosition.y);
			break;
		case RIGHT:
			mDestination = Vector2(mPosition.x + 64.0f, mPosition.y);
			break;
		}

		mMoveFailed = false;
		break;

	case SHOOT_BASIC:
		mCurrentFacing = mActionsToTake[mActionsToTake.size() - 1]->mActionDirection;
		Game::getInstance()->fireSpell(new Spell(mBasicAttack, mPosition, mCurrentFacing, mID, Game::getInstance()->getUnitID()));

		mCurrentMana -= manaForBasic;
		mTimeLastBasic = Game::getInstance()->getCurrentTime();
		break;

	case SHOOT_ULT:
		mCurrentFacing = mActionsToTake[mActionsToTake.size() - 1]->mActionDirection;
		// fire

		mCurrentMana -= manaForUltimate;
		mTimeLastUltimate = Game::getInstance()->getCurrentTime();
		break;
	}

	mTimelastActed = Game::getInstance()->getCurrentTime();
}

void WAIzard::loadAI(string file)
{
	mAI.clear();

	ifstream fileIn(file, std::ifstream::in);
	string word;
	int count = 0;

	while (!fileIn.eof())
	{
		fileIn >> word;
		count++;

		if (count < 9)
		{
			if  (count % 2 == 0)
			{ 
				if (word == "FIREBALL")										// Basic Attacks
				{
					mBasicAttack = (BasicAttacks)FIREBALL;
					manaForBasic = 10.0f;
				}


				else if (word == "LASER_BEAM")								// Ultimate Attacks
				{
					mUltimateAttack = (UltimateAttacks)LASER_BEAM;
					manaForUltimate = 80.0f;
				}

				else if (word == "MANA_ALIGNMENT")							// Passives
				{
					if (count == 6)
						mPassive1 = (Passives)MANA_ALIGNMENT;
					else
						mPassive2 = (Passives)MANA_ALIGNMENT;
				}
				else if (word == "BLOOD_MAGE")
				{
					if (count == 6)
						mPassive1 = (Passives)BLOOD_MAGE;
					else
						mPassive2 = (Passives)BLOOD_MAGE;
				}
			}
		}

		// On to the AI blocks

		else if (word == "if")
		{
			loadCondition(fileIn, true);
		}
		else if (word == "else")
		{
			mAI.push_back(AIBlock());
			mAI[mAI.size() - 1].mConditionSet.mCondition = ELSE;
			loadAction(fileIn);
		}
	}

	std::cout << "~Loaded file into AI: " << file << std::endl;
	fileIn.close();
}

void WAIzard::loadCondition(ifstream &fileIn, bool newBlock)
{
	string word;

	if (newBlock)
	{
		mAI.push_back(AIBlock());
	}

	fileIn >> word;

	if (word == "!")
	{
		mAI[mAI.size() - 1].mConditionSet.mNegateCondition = true;

		loadCondition(fileIn, false);	// Recursive
	}
	else if (word == "Health")
	{
		mAI[mAI.size() - 1].mConditionSet.mCondition = (AICondition)CHECK_HEALTH;

		fileIn >> word;
		if (word == ">")
			mAI[mAI.size() - 1].mConditionSet.mMoreOrLessThan = true;
		else
			mAI[mAI.size() - 1].mConditionSet.mMoreOrLessThan = false;

		fileIn >> word;
		mAI[mAI.size() - 1].mConditionSet.mConditionAmount = stoi(word);
	}
	else if (word == "Mana")
	{
		mAI[mAI.size() - 1].mConditionSet.mCondition = (AICondition)CHECK_MANA;

		fileIn >> word;
		if (word == ">")
			mAI[mAI.size() - 1].mConditionSet.mMoreOrLessThan = true;
		else
			mAI[mAI.size() - 1].mConditionSet.mMoreOrLessThan = false;

		fileIn >> word;
		mAI[mAI.size() - 1].mConditionSet.mConditionAmount = stoi(word);
	}
	else if (word == "WAIZARD")
	{
		mAI[mAI.size() - 1].mConditionSet.mConditionObject = (Object)WAIZARD;
		loadDetection(fileIn);
	}
	else if (word == "SPELL")
	{
		mAI[mAI.size() - 1].mConditionSet.mConditionObject = (Object)SPELL;
		loadDetection(fileIn);
	}
	else if (word == "FLOOR")
	{
		mAI[mAI.size() - 1].mConditionSet.mConditionObject = (Object)FLOOR;
		loadDetection(fileIn);
	}
	else if (word == "WALL")
	{
		mAI[mAI.size() - 1].mConditionSet.mConditionObject = (Object)WALL;
		loadDetection(fileIn);
	}
	else if (word == "EMPTY")
	{
		mAI[mAI.size() - 1].mConditionSet.mConditionObject = (Object)EMPTY;
		loadDetection(fileIn);
	}
	else if (word == "TimeMark1")
	{
		mAI[mAI.size() - 1].mConditionSet.mCondition = (AICondition)CHECK_TIME_MARK1;

		fileIn >> word;
		if (word == ">")
			mAI[mAI.size() - 1].mConditionSet.mMoreOrLessThan = true;
		else
			mAI[mAI.size() - 1].mConditionSet.mMoreOrLessThan = false;

		fileIn >> word;
		mAI[mAI.size() - 1].mConditionSet.mConditionAmount = stoi(word);
	}	
	else if (word == "TimeMark2")
	{
		mAI[mAI.size() - 1].mConditionSet.mCondition = (AICondition)CHECK_TIME_MARK2;

		fileIn >> word;
		if (word == ">")
			mAI[mAI.size() - 1].mConditionSet.mMoreOrLessThan = true;
		else
			mAI[mAI.size() - 1].mConditionSet.mMoreOrLessThan = false;

		fileIn >> word;
		mAI[mAI.size() - 1].mConditionSet.mConditionAmount = stoi(word);
	}
	else if (word == "Random")
	{
		mAI[mAI.size() - 1].mConditionSet.mCondition = (AICondition)CHECK_RANDOM;

		fileIn >> word;
		if (word == ">")
			mAI[mAI.size() - 1].mConditionSet.mMoreOrLessThan = true;
		else
			mAI[mAI.size() - 1].mConditionSet.mMoreOrLessThan = false;

		fileIn >> word;
		mAI[mAI.size() - 1].mConditionSet.mConditionAmount = stoi(word);
	}
	else if (word == "Attacking")
	{
		mAI[mAI.size() - 1].mConditionSet.mCondition = (AICondition)CHECK_ATTACK;
	}
	else if (word == "Moving")
	{
		mAI[mAI.size() - 1].mConditionSet.mCondition = (AICondition)CHECK_MOVE;
	}
	else if (word == "True")
	{
		mAI[mAI.size() - 1].mConditionSet.mCondition = (AICondition)DO_ACTION;
	}

	// If recursive, don't loadAction twice
	if (word != "!")
	{
		// Load the action to pair the condition
		loadAction(fileIn);
	}
}

void WAIzard::loadAction(ifstream &fileIn)
{
	string word;
	fileIn >> word;

	if (word == "Basic")
	{
		mAI[mAI.size() - 1].mActionSet.mAction = (AIAction)SHOOT_BASIC;
		
		fileIn >> word;
		mAI[mAI.size() - 1].mActionSet.mActionDirection = interpretDirectionString(word);
	}
	else if (word == "Ult")
	{
		mAI[mAI.size() - 1].mActionSet.mAction = (AIAction)SHOOT_ULT;

		fileIn >> word;
		mAI[mAI.size() - 1].mActionSet.mActionDirection = interpretDirectionString(word);
	}
	else if (word == "Move")
	{
		mAI[mAI.size() - 1].mActionSet.mAction = (AIAction)MOVE;

		fileIn >> word;
		mAI[mAI.size() - 1].mActionSet.mActionDirection = interpretDirectionString(word);
	}
	else if (word == "Reset")
	{
		mAI[mAI.size() - 1].mConditionSet.mResetTimer = true;

		fileIn >> word;		 // and

		loadAction(fileIn);	 // Recursive
	}
	else if (word == "SetMark1")
	{
		mAI[mAI.size() - 1].mActionSet.mAction = (AIAction)SET_TIME_MARK1;
	}
	else if (word == "SetMark2")
	{
		mAI[mAI.size() - 1].mActionSet.mAction = (AIAction)SET_TIME_MARK2;
	}
	else if (word == "Focus")
	{
		fileIn >> word;

		if (word == "Current")
		{
			mAI[mAI.size() - 1].mActionSet.mAction = (AIAction)SET_FOCUS_CURRENT_DIR;
		}		
		else
		{
			mAI[mAI.size() - 1].mActionSet.mAction = (AIAction)SET_FOCUS;
			mAI[mAI.size() - 1].mActionSet.mActionDirection = interpretDirectionString(word);
		}
	}
	else if (word == "Do")
	{
		mAI[mAI.size() - 1].mActionSet.mAction = (AIAction)DO_NOTHING;

		fileIn >> word;		 // Nothing
	}
	else if (word == "Cancel")
	{
		fileIn >> word;

		if (word == "Nothing")
		{
			mAI[mAI.size() - 1].mActionSet.mAction = (AIAction)CANCEL_NOTHING;
		}
		else if (word == "Attack")
		{
			mAI[mAI.size() - 1].mActionSet.mAction = (AIAction)CANCEL_ATTACK;
		}
		else if (word == "Attacks")
		{
			mAI[mAI.size() - 1].mActionSet.mAction = (AIAction)CANCEL_ALL_ATTACKS;
		}
		else if (word == "Move")
		{
			mAI[mAI.size() - 1].mActionSet.mAction = (AIAction)CANCEL_MOVE;
		}
		else if (word == "Moves")
		{
			mAI[mAI.size() - 1].mActionSet.mAction = (AIAction)CANCEL_ALL_MOVES;
		}
		else if (word == "Action")
		{
			mAI[mAI.size() - 1].mActionSet.mAction = (AIAction)CANCEL_ACTION;
		}
		else if (word == "Actions")
		{
			mAI[mAI.size() - 1].mActionSet.mAction = (AIAction)CANCEL_TWO_ACTIONS;
		}
	}
	else if (word == "Change")
	{
		fileIn >> word;

		if (word == "Direction")
		{
			mAI[mAI.size() - 1].mActionSet.mAction = (AIAction)CHANGE_DIRECTION;

			fileIn >> word;
			mAI[mAI.size() - 1].mActionSet.mActionDirection = interpretDirectionString(word);
		}
		else if (word == "Basic")
		{
			mAI[mAI.size() - 1].mActionSet.mAction = (AIAction)CHANGE_TO_BASIC;
		}
		else if (word == "Ult")
		{
			mAI[mAI.size() - 1].mActionSet.mAction = (AIAction)CHANGE_TO_ULT;
		}
		else if (word == "Move")
		{
			mAI[mAI.size() - 1].mActionSet.mAction = (AIAction)CHANGE_TO_MOVE;
		}
	}
	else
	{
		mAI[mAI.size() - 1].mActionSet.mAction = (AIAction)DO_NOTHING;
	}
}

void WAIzard::loadDetection(ifstream &fileIn)
{
	string word;

	fileIn >> word;

	if (word == "at")	// Check Point
	{
		mAI[mAI.size() - 1].mConditionSet.mCondition = (AICondition)CHECK_POSITION;

		fileIn >> word;
		mAI[mAI.size() - 1].mConditionSet.mConditionPosition.x = (float)stoi(word);

		fileIn >> word;
		mAI[mAI.size() - 1].mConditionSet.mConditionPosition.y = (float)stoi(word);
	}
	else if (word == "through")	// Check Line
	{
		mAI[mAI.size() - 1].mConditionSet.mCondition = (AICondition)CHECK_COLLISION_LINE;

		fileIn >> word;
		mAI[mAI.size() - 1].mConditionSet.mConditionDirection = interpretDirectionString(word);

		fileIn >> word;
		mAI[mAI.size() - 1].mConditionSet.mConditionAmount = stoi(word);
	}
	else if (word == "Up")	// Check directions
	{
		mAI[mAI.size() - 1].mConditionSet.mCondition = (AICondition)CHECK_POSITION;

		mAI[mAI.size() - 1].mConditionSet.mConditionPosition.x = 0;
		mAI[mAI.size() - 1].mConditionSet.mConditionPosition.y = -1;
	}
	else if (word == "Down")
	{
		mAI[mAI.size() - 1].mConditionSet.mCondition = (AICondition)CHECK_POSITION;

		mAI[mAI.size() - 1].mConditionSet.mConditionPosition.x = 0;
		mAI[mAI.size() - 1].mConditionSet.mConditionPosition.y = 1;
	}
	else if (word == "Left")
	{
		mAI[mAI.size() - 1].mConditionSet.mCondition = (AICondition)CHECK_POSITION;

		mAI[mAI.size() - 1].mConditionSet.mConditionPosition.x = -1;
		mAI[mAI.size() - 1].mConditionSet.mConditionPosition.y = 0;
	}
	else if (word == "Right")
	{
		mAI[mAI.size() - 1].mConditionSet.mCondition = (AICondition)CHECK_POSITION;

		mAI[mAI.size() - 1].mConditionSet.mConditionPosition.x = 1;
		mAI[mAI.size() - 1].mConditionSet.mConditionPosition.y = 0;
	}
	else if (word == "Forward")
	{
		mAI[mAI.size() - 1].mConditionSet.mCondition = (AICondition)CHECK_POSITION;
		mAI[mAI.size() - 1].mConditionSet.mConditionDirection = FORWARD;
	}
	else if (word == "Backward")
	{
		mAI[mAI.size() - 1].mConditionSet.mCondition = (AICondition)CHECK_POSITION;
		mAI[mAI.size() - 1].mConditionSet.mConditionDirection = BACKWARD;
	}
	else if (word == "Focused")
	{
		mAI[mAI.size() - 1].mConditionSet.mCondition = (AICondition)CHECK_POSITION;
		mAI[mAI.size() - 1].mConditionSet.mConditionDirection = FOCUSED;
	}
}

Direction WAIzard::interpretDirectionString(string dir)
{
	 if (dir == "Up")
		return (Direction)UP;
	else if (dir == "Down")
		return (Direction)DOWN;
	else if (dir == "Left")
		return (Direction)LEFT;
	else if (dir == "Right")
		return (Direction)RIGHT;
	else if (dir == "Forward")
		return (Direction)FORWARD;
	else if (dir == "Backward")
		return (Direction)BACKWARD;
	else if (dir == "Untried")
		return (Direction)UNTRIED;
	else if (dir == "Focused")
		return (Direction)FOCUSED;
	else if (dir == "Random")
		return (Direction)RANDOM;
	else
		return (Direction)NO_DIRECTION;
}

Direction WAIzard::relativizeDirection(Direction dir)
{
	// At execution time

	switch (dir)
	{
	case FORWARD:
		dir = mCurrentFacing;
		break;
	case BACKWARD:
		if (mCurrentFacing == UP)
			dir = DOWN;
		else if (mCurrentFacing == DOWN)
			dir = UP;
		else if (mCurrentFacing == LEFT)
			dir = RIGHT;
		else if (mCurrentFacing == RIGHT)
			dir = LEFT;
		break;
	case UNTRIED:
		dir = mDirectionsTried.getUntriedDirection();
		break;
	case FOCUSED:
		dir = mFocusDirection;
		break;
	case RANDOM:
		dir = (Direction)Game::getInstance()->getRandomInt(4);
		break;
	}

	return dir;
}

// AI Block
AIBlock::AIBlock()
{
	mConditionSet = BlockConditionSet();
	mActionSet = BlockActionSet();
}

// AI Block Condition Set
AIBlock::BlockConditionSet::BlockConditionSet()
{
	mConditionDirection = NO_DIRECTION;

	mNegateCondition = false;
	mResetTimer = false;
	mMoreOrLessThan = true;

	mConditionAmount = 0;
	mConditionPosition = Vector2();
	mConditionObject = NO_OBJECT;
}

// AI Block Action Set
AIBlock::BlockActionSet::BlockActionSet()
{
	mAction = DO_NOTHING;
	mActionDirection = NO_DIRECTION;
}

// Spell
Spell::Spell()
{

}

Spell::Spell(BasicAttacks type, Vector2 pos, Direction dir, int WAIzardID, int id)
{
	mBasicType = type;
	isUltimate = false;

	// My Unit:
	mPosition = pos;
	mBoundingBox.mPosition = Vector2(pos.x + 16.0f, pos.y + 16.0f);
	mBoundingBox.mSize = Vector2(32.0f, 32.0f);
	mAnimation = new Animation(0.1f, true);
	mDrawType = ANIMATION;
	mDepth = 25;
	mID = id;

	mWAIzardID = WAIzardID;
	distanceTraveled = 64.0f;
	mDamage = 0;				// Default damage
	mPushMultiplier = 0.0f;
	isTrash = false;

	switch (mBasicType)
	{
	case FIREBALL:
		mAnimation->addSprite(Game::getInstance()->mFireballSprites[0]);
		mAnimation->addSprite(Game::getInstance()->mFireballSprites[1]);
		mAnimation->addSprite(Game::getInstance()->mFireballSprites[2]);
		mAnimation->addSprite(Game::getInstance()->mFireballSprites[3]);

		distanceToTravel = 64.0f * 8.0f;
		setVelocity(dir, 4.0f);
		mDamage = 40;
		break;
	}
}

Spell::Spell(UltimateAttacks type, Vector2 pos, Direction dir, int WAIzardID, int id)
{
	mUltimateType = type;
	isUltimate = true;

	// My Unit:
	mPosition = pos;
	mBoundingBox.mPosition = Vector2(pos.x + 16.0f, pos.y + 16.0f);
	mBoundingBox.mSize = Vector2(32.0f, 32.0f);
	mAnimation = new Animation(0.1f, true);
	mDrawType = ANIMATION;
	mDepth = 10;
	mID = id;

	mWAIzardID = WAIzardID;
	mDamage = 0;
	mPushMultiplier = 0.0f;
	isTrash = false;

	//switch (mUltimateType)
	//{
	//case LASERBEAM:
	//	mAnimation->addSprite(Game::getInstance()->mFireballSprites[0]);
	//	mAnimation->addSprite(Game::getInstance()->mFireballSprites[1]);
	//	mAnimation->addSprite(Game::getInstance()->mFireballSprites[2]);
	//	mAnimation->addSprite(Game::getInstance()->mFireballSprites[3]);

	//	setVelocity(dir, 12.0f);
	//	break;
	//}
}

void Spell::update()
{
	if (!isTrash)
	{
		mPosition += mVelocity;
		mBoundingBox.mPosition = Vector2(mPosition.x + 16.0f, mPosition.y + 16.0f);
		distanceTraveled += abs(mVelocity.x) + abs(mVelocity.y);

		if (distanceTraveled >= distanceToTravel)
			isTrash = true;

		mAnimation->update(Game::getInstance()->getCurrentTime());

		// Check for collisions
		unsigned int i;

		// WAIzards
		for (i = 0; i < Game::getInstance()->getWAIzardSize() && !isTrash; ++i)
		{
			if (Game::getInstance()->getWAIzardID(i) != mWAIzardID && !Game::getInstance()->getWAIzardDead(i))
				isTrash = mBoundingBox.checkCollision(Game::getInstance()->getWAIzardBoundingBox(i));

			if (isTrash)
			{
				// Deal damage to WAIzard victim
				Game::getInstance()->getWAIzard(i)->dealDamage(mDamage, Vector2(mVelocity.x * mPushMultiplier, mVelocity.y * mPushMultiplier), mWAIzardID);

				// Credit damage to WAIzard caster
				Game::getInstance()->getWAIzardWithID(mWAIzardID)->creditDamage(mDamage);
			}
		}

		// Walls
		for (i = 0; i < Game::getInstance()->getWallSize() && !isTrash; ++i)
		{
			isTrash = mBoundingBox.checkCollision(Game::getInstance()->getWallBoundingBox(i));
		}

		Game::getInstance()->addCornersToRedraw(mPosition, 60.0f);
	}	
}

void Spell::setVelocity(Direction dir, float speed)
{
	switch (dir)
	{
		case UP:
			mVelocity = Vector2(0.0f, -speed);
			break;
		case DOWN:
			mVelocity = Vector2(0.0f, speed);
			break;
		case LEFT:
			mVelocity = Vector2(-speed, 0.0f);
			break;
		case RIGHT:
			mVelocity = Vector2(speed, 0.0f);
			break;
	}
}