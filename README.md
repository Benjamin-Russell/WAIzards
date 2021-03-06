# WAIzards
Project by Benjamin Russell, 2016  
A simulation about battling Wizards with AI meant to begin very simply and behave more intelligently through a process of evolution.

This is made with C++ in Visual Studio 2013, and uses SDL 2.

__Note:__
This project is suspended indefinitely.

Currently much of the AI is fleshed out. You can edit the file Resources/AI/Waizard1.txt, which
is where all WAIzards poll their AI. There is a short AI guide below. The plan is to create the evolution system which will breed the most fit WAIzards, save their AI to new files, and start the next round. This part may be done for my Capstone, so in the meantime I plan to work on more ability implementations, graphics, and optimization.

__To Run:__  
-Open the sln file, and build (CONTROL-SHIFT-B) in Debug or Release.  
-Then run the corresponding Batch file, WAIZARDS_DEBUG / WAIZARDS_RELEASE  

This is done in order to link the DLL's in their folder without placing them in the working directory. Feel
free to inspect the Batch files yourself.

__Controls:__ (currently)  
H       -   Toggle health bars  
M       -   Toggle mana bars  
L       -   Reload AI (WAIzard1.txt) in game  
Enter   -   Load level_box  
Escape  -   Quit program  

__AI Guide:__  
AI is run when a WAIzard is aligned with the tiles, and has a cooldown of half a second currently.

The first four lines define the basic attack, ultimate attack, and two passives of a WAIzard. Currently there are no ultimate attacks, and very few working passives.

Next are the AI Blocks, one line for an if condition, and the next line for the consequent action. Do_Nothing, Move, Basic, and Ultimate actions are placed at the top of the WAIzard's action vector. When all the AI has been run, the last action is chosen. The other actions like SET, CANCEL, and CHANGE do not get placed in the action vector.

__OBJECTS:__
-  FLOOR
-  WALL
-  SPELL
-  WAIZARD
-  EMPTY        
(No Floor or Wall)  
  
__DIRECTIONS:__  
-	Up
-	Down
-	Left
-	Right
-	Forward
-	Backward
-	Untried  
(A direction in which moving was not just blocked by a Wall or WAIzard. Reset upon successful move)
-	Focused  
(The direction "Focused on," held in the mFocusDirection variable)  
-	Random   
(Up, Down, Left, or Right)  

__Conditions:__  
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
__if OBJECT at 7 -2__   
(check 7 tiles to the right, 2 up)            *See: OBJECTS  
__if OBJECT DIRECTION__  
(check adjacent tile in DIRECTION for OBJECT)         *See: DIRECTIONS    

-	CHECK_COLLISION_LINE   
__if OBJECT through DIRECTION 55__  
(check through (55 / 10) + 1 tiles in DIRECTION for OBJECT)   

-	CHECK_RANDOM  
__if Random < 50__  
(Random between 0-99)    

-	CHECK_ATTACKING  
__if Attacking__  
(if current action is a basic or ultimate attack)    

-	CHECK_MOVING  
__if Moving__  
(if current action is a move)    

-	DO_ACTION  
__if True__  
(tautology)    

-	ELSE  
__else__  
(if previous condition outcome was false)  

- ALSO  
__also__  
(if previous condition ouctome was true)  

__Actions:__  

-	DO_NOTHING  
__Do Nothing__  
(An action for doing nothing)    

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
