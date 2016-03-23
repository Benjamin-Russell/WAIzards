#ifndef _WAIZARD_H_
#define _WAIZARD_H_

#include "Unit.h"
#include "GraphicsSystem.h"
#include <iostream>

#include <vector>

enum BasicAttacks	   // Some attacks may do no damage but hitting a wall with high velocity hurts some and going out-of-arena is instant death.
{
	FIREBALL = 0,      // ranged missile, damage, low mana.
	BOULDER,           // ranged missile, damage, low knockback.
	TOXIC_TRAIL,       // place in area, poison, slow?
	GALE_FORCE,        // short range cone, no damage, high knockback, high mana.
	VENOM_SPIT,        // short range cone, poison, high mana.
	MUD_SLIDE,         // long arc, low damage, consistent knockback, high mana.
	SPAWN_WALL,        // place adjacent, timed, damage 2 spaces in 4 directions, immunity to Spawned Wall explosions.
	SNIPE,             // long range missile, pierces 2 walls, damage, long cooldown.
	LIGHTNING_BOLT,    // 4 tile line attack, damage, long cooldown, high mana.
	// Melee range attacks
	NUM_BASICS
};

enum UltimateAttacks   // long cooldowns on most of these
{
	LASER_BEAM = 0,    // instant total range, total piercing, damage.
	MANA_SPRITE_FORM,  // basic attack doesn't consume mana for 12 seconds. ~
	TITAN_SHIELD,      // take no damage for 8 seconds, excepting the bleed.
	IMPACT,            // fly in the air, charge back down. Centered AOE, damage, knockback.
	MAGIC_MISSLE,      // long range missile turns to avoid walls and the void, damage.
	NUM_ULTIMATES
};

enum Passives
{
	MANA_SHIELD = 0,   // half of all damage taken applied doubled to mana instead. (Doesn't affect fitness ratings)
	MANA_STAR,         // regen mana faster.
	LIFE_ALIGNMENT,    // higher max health.
	MANA_ALIGNMENT,    // higher max mana.
	ROCK_ALIGNMENT,    // move slower, higher friction.
	WIND_ALIGNMENT,    // move faster.
	FIRE_ALIGNMENT,    // enemy WAIzards that come in contact take damage, low knockback. Consumes mana or fails.
	PREY_INSTINCTS,    // detect incoming attacks, dash in a random direction away. On cooldown. May run into walls or the void.
	GUARDIAN_SPIRIT,   // when fallen into void first time, take half remaining life off instead, respawn.
	SLIME_WARD,        // reduced damage from impact with walls.
	SURVIVALIST,       // reduced damage from poison effects.
	LICHDOM,           // after death, continue life for 8 seconds or until falling into the void.
	MAGIC_BARRIER,     // when about to fall into the void, consume 40 mana. If successful, reverse velocity.
	PHASE_SHIFTER,     // when damaged and off cooldown, consume 15 mana. If successful, teleport to random vacant space.
	BLOOD_RUNNER,      // start slower, move faster as health decreases. XXX May be removing speed
	BLOOD_MAGE,        // mana regen starts slower, goes faster as health decreases.
					   // something that speeds up wait for next action XXX may be making the cycle global for Waizards
					   // more melee damage
	NUM_PASSIVES
};

// AI:

enum AICondition
{
	CHECK_HEALTH = 0,
	CHECK_MANA,
	CHECK_TIME_MARK1,
	CHECK_TIME_MARK2,
	CHECK_JUST_FOCUSED,		// Check mJustFocused, True if focus dir has been set this turn     //////////to do
	CHECK_POSITION,
	CHECK_COLLISION_LINE,	// ConditionAmount is divided by 8, checks that many tiles + 1
	CHECK_RANDOM,
	CHECK_ATTACK,
	CHECK_MOVE,
	DO_ACTION,				// if True
	ELSE,
	NUM_CONDITIONS
};

enum AIAction
{
	DO_NOTHING = -1,		// Add action to not move or attack this cycless
	SHOOT_BASIC,			// Add action for activating basic ability if able
	SHOOT_ULT,				// Add action for activating ultimate ability if able
	MOVE,					// Add action for moving in a direction

	SET_TIME_MARK1,			// Set mTimeMark1 to now
	SET_TIME_MARK2,			// Set mTimeMark2 to now
	SET_FOCUS,				// Set mFocusDirection in a direction
	SET_FOCUS_CURRENT_DIR,	// Set mFocusDirection to current action direction

	CANCEL_NOTHING,			// If current action is a DO_NOTHING, delete it
	CANCEL_MOVE,			// If current action is a move, delete it
	CANCEL_ATTACK,			// If current action is an attack, delete it		
	CANCEL_ALL_MOVES,		// Delete all move actions from action vector
	CANCEL_ALL_ATTACKS,		// Delete all attack actions from action vector
	CANCEL_ACTION,			// delete current action
	CANCEL_TWO_ACTIONS,		// delete current and previous action

	CHANGE_DIRECTION,		// Change current action direction
	CHANGE_TO_BASIC,		// Change current action to Basic if able to basic
	CHANGE_TO_ULTIMATE,		// Change current action to Ult if able to Ult
	CHANGE_TO_MOVE,			// Change current action to Move

	WAIT,					// Wait a period of time before running AI again	//////////to do

	NUM_ACTIONS
};

// This structure keeps track of the directions a WAIzard has attempted to move and is to be reset when a move is successful.
struct DirectionsTried
{
	DirectionsTried();
	~DirectionsTried(){};

	bool mDirections[4]; // Up, Down, Left, Right

	void reset();
	void setTry(Direction tried);
	void unsetTry(Direction toUnset);
	Direction getUntriedDirection();
};

struct AIBlock
{
	struct BlockActionSet
	{
		BlockActionSet();
		~BlockActionSet(){};

		AIAction mAction;
		Direction mActionDirection;
	};

	struct BlockConditionSet
	{
		BlockConditionSet();
		~BlockConditionSet(){};

		AICondition mCondition;
		Direction mConditionDirection;

		Vector2 mConditionPosition;
		Object mConditionObject;
		int mConditionAmount;	// May need to convert when mutating a condition, else keep at 0-99
		bool mMoreOrLessThan;
		bool mNegateCondition;
		bool mResetTimer;
	};

	AIBlock();
	~AIBlock(){};

	BlockConditionSet mConditionSet;
	BlockActionSet mActionSet;
};

class WAIzard : public Unit
{
public:
	WAIzard(Vector2 position, string file);
	~WAIzard();

	Unit mHealthBar;
	Unit mManaBar;

	void update();
	void updateBars();

	inline Vector2 getPosition() { return mPosition; }
	inline Vector2 getVelocity() { return mVelocity; }
	inline Rect getBoundingBox() { return mBoundingBox; }
	inline int getID() { return mID; }
	inline Direction getCurrentFacing() { return mCurrentFacing; }
	inline bool getDead() { return mDead; }
	
	inline void creditDamage(int dam) {mTotalDamageDealt += dam;}

	void dealDamage(int damage, Vector2 push, int id);
	void loadAI(string file);

private:

	int mMaxHealth;
	int mCurrentHealth;
	float mMaxMana;
	float mCurrentMana;
	float mMaxManaRegen;
	float mCurrentManaRegen;
	float manaForBasic;
	float manaForUltimate;
	bool mDead;

	float mMaxSpeed;
	float mCurrentSpeed;
	float mFriction;
	Vector2 mDestination;

	float mTimeLastPoisoned;
	float mTimeToBePoisoned;
	float mTimeLastHit;
	float mTimelastActed;
	float mAICoolDown;

	float mBasicCoolDown;
	float mUltimateCoolDown;
	float mTimeLastBasic;
	float mTimeLastUltimate;

	int mTotalDamageDealt;
	int mMeleeDamage;
	int mLastHitBy;

	Direction mMoveDirection;
	Direction mCurrentFacing;
	DirectionsTried mDirectionsTried;
	bool mMoveFailed;

	// AI vars
	float mTimeMark1;
	float mTimeMark2;
	Direction mFocusDirection;

	BasicAttacks mBasicAttack;
	UltimateAttacks mUltimateAttack;
	Passives mPassive1;
	Passives mPassive2;
	
	// AI
	vector<AIBlock> mAI;
	vector<AIBlock::BlockActionSet*> mActionsToTake;

	void executeAI();
	void loadCondition(ifstream &fileIn, bool newBlock);
	void loadAction(ifstream &fileIn);
	void loadDetection(ifstream &fileIn);

	Direction interpretDirectionString(string dir);
	Direction relativizeDirection(Direction dir);
};

class Spell : public Unit
{
public:
	Spell();
	Spell(BasicAttacks type, Vector2 pos, Direction dir, int WAIzardID, int id);
	Spell(UltimateAttacks type, Vector2 pos, Direction dir, int WAIzardID, int id);
	~Spell(){};

	void update();

	inline Rect getBoundingBox() { return mUnit.mBoundingBox; }
	inline Vector2 getPosition() { return mUnit.mPosition; }	// Position is x and y of bounding box
	inline int getID() { return mID; }
	inline int getWAIzardID() { return mWAIzardID; }
	inline bool getIsTrash() { return isTrash; }

private:
	Unit mUnit;

	void setVelocity( Direction dir, float speed);

	bool isUltimate;
	BasicAttacks mBasicType;
	UltimateAttacks mUltimateType;

	int mWAIzardID;
	int mDamage;
	float mPushMultiplier;
	float distanceTraveled;
	float distanceToTravel;
	bool isTrash;
};

#endif