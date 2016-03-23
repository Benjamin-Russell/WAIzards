# WAIzards
Project by Benjamin Russell, 2016
A simulation about battling Wizards with AI meant to begin very simply and behave more intelligently through a process of evolution.

This is made in Visual Studio 2013, and uses SDL 2.

The project is in development, currently most of the AI is fleshed out. You can edit the file Resources/AI/Waizard1.txt, which
is currently where all WAIzards poll their AI. I'll add a short AI guide below. The plan is to next end a level after a minute or a winner has been found. Then create the evolution system which will breed the most fit WAIzards, save their AI to new files, and start the next round. After, I plan to add more attacks, an interface with tons of customizability, and to generate reports of data on the simulation.

To Run:  
-Open the sln file, and build (CONTROL-SHIFT-B) in Debug or Release.
-Then run the corresponding Batch file, WAIZARDS_DEBUG / WAIZARDS_RELEASE

This is done in order to link the DLL's in their folder without placing them in the working directory. Feel
free to inspect the Batch files yourself.

Controls: (currently)  
H       -   Toggle health bars  
M       -   Toggle mana bars  
L       -   Reload AI (WAIzard1.txt) in game  
Enter   -   Load level_box  
Escape  -   Quit program  

AI Guide:
AI is run when a WAIzard is aligned with the tiles, and has a cooldown of half a second currently.

The first four lines define the basic attack, ultimate attack, and two passives of a WAIzard. Currently there are no ultimate attacks, and very few working passives. The evolution system takes precedent before those.

Next are the AI Blocks, one line for an if condition, and the next line for the consequent action. Do_Nothing, Move, Basic, and Ultimate actions are placed at the top of the WAIzard's action vector. When all the AI has been run, the last action is chosen. There are other actions like SET, CANCEL, and CHANGE, which do not get placed in the action vector.

OBJECTS:
-  FLOOR
-  WALL
-  SPELL
-  WAIZARD
-  EMPTY      (No Floor or Wall)
  
DIRECTIONS:
-	UP
-	DOWN
-	LEFT
-	RIGHT
-	FORWARD
-	BACKWARD
-	UNTRIED  
(A direction in which moving was not just blocked by a Wall or WAIzard)
-	FOCUSED  
(The direction "Focused on," held in the mFocusDirection variable)
-	RANDOM   
(Up, Down, Left, or Right)

Conditions:
-  !                      
__if ! Health > 90__   
(Negate any condition check)
-	CHECK_HEALTH            
__if Health < 20__ 
-	CHECK_MANA              
__if Mana > 60__       
(condition amounts are integers 0 to 99)
-	CHECK_TIME_MARK1        
__if TimeMark1 > 20__  
(tenths of a second since TimeMark1 was set)
-	CHECK_TIME_MARK2        
__if TimeMark2 < 30__  
(if "Reset and" begins the consequent action, set the condition TimeMark)  
-	CHECK_POSITION          
__if OBJECT* at 7 2__   
(check 7 tiles to the right, 2 down)            *See: OBJECTS
-	CHECK_COLLISION_LINE   
__if OBJECT through DIRECTION* 55__  
(check through (55 / 10) + 1 tiles in DIRECTION for OBJECT)       *See: DIRECTIONS  
-	CHECK_RANDOM  
__if Random < 50__  
(50% chance)  
-	CHECK_ATTACK  
__if Attacking__  
(if current action is a basic or ultimate attack)  
-	CHECK_MOVE  
__if Moving__  
(if current action is a move)  
-	DO_ACTION  
__if True__  
(tautology)  
-	ELSE  
__else__  
(if previous condition outcome was false)  

Actions:  

-	DO_NOTHING  
__Do Nothing__  
(Don't act this AI cycle)  
-	SHOOT_BASIC  
__Basic DIRECTION__  
(Use Basic spell if has the mana and off cooldown in DIRECTION)  
-	SHOOT_ULT  
__Ultimate DIRECTION__  
(Use Ultimate spell if has the mana and off cooldown in DIRECTION)  
-	MOVE  
__Move DIRECTION__  
(Move in DIRECTION)  

-	SET_TIME_MARK1  
__SetMark1__  
(Set TimeMark1 to current time)  
-	SET_TIME_MARK2  
__SetMark2__  
(Set TimeMark2 to current time)  
-	SET_FOCUS  
__Focus DIRECTION__  
(Set mFocusDirection in DIRECTION)  
-	SET_FOCUS_CURRENT_DIR  
__Focus Current__  
(Set mFocusDirection to the direction of the current action to take)  
-	CANCEL_NOTHING  
__Cancel Nothing__  
(If current action is a Do_Nothing, remove it from action vector)  
-	CANCEL_MOVE  
__Cancel Move__  
(If current action is a Move, remove it from action vector)  
-	CANCEL_ATTACK  
__Cancel Attack__  
(If current action is an attack, remove it from action vector)  
-	CANCEL_ALL_MOVES  
__Cancel Moves__  
(Remove all move actions from the action vector)  
-	CANCEL_ALL_ATTACKS  
__Cancel Attacks__  
(Remove all attack actions from the action vector)  
-	CANCEL_ACTION  
__Cancel Action__  
(Remove the current action from the top of the action vector)  
-	CANCEL_TWO_ACTIONS  
__Cancel Actions__  
(Remove the current and previous actions from the action vector)  

-	CHANGE_DIRECTION  
__Change Direction DIRECTION__  
(Change the direction of the current action to DIRECTION)  
-	CHANGE_TO_BASIC  
__Change Basic__  
(Change current action to Basic spell if able)  
-	CHANGE_TO_ULTIMATE  
__Change Ultimate__  
(Change current action to Ultimate spell if able)  
-	CHANGE_TO_MOVE  
__Change Move__  
(Change current action to Move)  
