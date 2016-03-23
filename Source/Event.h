#ifndef _EVENT_H_
#define _EVENT_H_

// KEY_UP events are wonky and trigger on KEY_DOWN, so let's avoid using them
enum EventType
{
	INVALID = -1,

	// Input events
	LEFT_PRESSED,
	LEFT_HELD,
	LEFT_RELEASED,
	RIGHT_PRESSED,
	RIGHT_HELD,
	RIGHT_RELEASED,
	UP_PRESSED,
	UP_HELD,
	DOWN_PRESSED,
	DOWN_HELD,
	ESCAPE_PRESSED,
	ENTER_PRESSED,
	SPACE_HELD,
	B_PRESSED,
	F_PRESSED,
	H_PRESSED,
	L_PRESSED,
	M_PRESSED,
	NUM_1_PRESSED,
	NUM_2_PRESSED,
	NUM_3_PRESSED,
	NUM_4_PRESSED,

	NUM_EVENT_TYPES
};

class Event
{
public:
	Event(EventType type);
	virtual ~Event();

	inline EventType getType() const { return mType; };

private:
	EventType mType;
};

#endif