# RCCarGame.OpenGL
A 3D racing game created for my Seneca college final year team project, written in c++ and OpenGL.
 
RC Car Game is a fast paced one and two player arcade style racing game.
Featuring multiple cars and tracks, high scores, a full soundtrack and realistic physics!!

Developed by:
Jeff Barnard, Pat Crawford and Radek Koncewicz
---------------------------------------------

<img src="./development%20docs/images/MenuSelect.png" height="600">

<img src="./development%20docs/images/Menu.png" height="600">

<img src="./development%20docs/images/SplitScreen.png" height="600">

<img src="./development%20docs/images/Buggy0.png" height="600">

<img src="./development%20docs/images/Buggy2.png" height="600">

<img src="./development%20docs/images/FerrariBox.png" height="600">

<img src="./development%20docs/images/WireFrame.png" height="600">


  1  System Requirements
     
     1.1  Minimum System Requirements
     1.2  Recommended System Requirements

  2  Performance
     
     2.1  Resolution
     2.2  CPU speed
     2.3  Memory
     2.4  Getting better performance.

  3. Installation & Game play Information 

     3.1  Installing and executing the game
     3.2  Playing a single player game
     3.3  Playing a two-player game
     3.4  Player 1 car controls
     3.5  Player 2 car controls

  4  Troubleshooting

     4.1  Sound issues
     4.2  Control issues
     4.3  OpenGL

  5  Project Deviations
      
     5.1  Alterations to original design
     5.2  Extra additions  

  6  Known Bugs

     6.1  A list of known bugs and glitches



1 System Requirements
---------------------------------------------


1.1 Minimum System Requirements
--------------------------------------------

Operating System:    	Microsoft Windows 2000/XP

Processor:           		Pentium III or AMD Athlon

Memory:              		128 Mbytes RAM

Hard Disk Space:     	150 Mbytes free HD space

Video:               		16 MB 3D accelerator with Hardware T&L

Sound:               		Windows compatible sound card

OpenGL:              		OpenGL 1.3 (included with video drivers)
                     
1.2 Recommended System Requirements
--------------------------------------------------

Operating System:    	Microsoft Windows 2000/XP

Processor:          		Pentium 4 or AMD Athlon XP

Memory:              		256 Mbytes RAM

Hard Disk Space:     	500 Mbytes free HD space

Video:               	NVIDIA GeForce3 Ti or ATI Radeon 8500/9500+ (32-128 MB VRAM recommended)

Sound:               		Sound Blaster Audigy series sound card

OpenGL:              		OpenGL 1.5 (included with video drivers)



 
 2 Performance
 ---------------------------------------------


2.1 Resolution
------------------

The default resolution is 1024 x 768 and is unchangeable. 

2.2 CPU Speed
-------------------

The game is very sensitive to CPU speed, memory bandwidth, and cache
performance. Thus, it runs far better on leading-edge processors like
those of AMD and Intel.

2.3 Memory
---------------

RC Car Games performance is highly dependent on the amount
of RAM you have in your machine, and the amount of memory that is
available. Machines with less memory will access their hard disk more
frequently to load data, which causes sporadic pauses in game play. Thus,
if you have a 128 Mbytes (or less) machine, you should make sure that
you don't have other unnecessary programs loaded in memory when playing
RC Car Game

How the game will perform under different RAM conditions:

  * 64 Mbytes or less: swapping

  * 128 Mbytes: might swap in rare cases with highest texture detail

  * 256 Mbytes: shouldn't swap



2.5 Getting better performance
-------------------------------------

It is impossible for the user to tweak any game settings without adversely affecting their computer’s performance, therefore the only way to make the game run better is to run it on better hardware.



 
3 Installation & Game play Information
---------------------------------------------


3.1 Installing and executing the game
---------------------------------------------

Simply extract the RC Car Game.zip file to your desired location, then navigate to the “RC Car Game\system\” folder and double click on the “RC Car Game.exe”.

3.2 Playing a single player game
----------------------------------------

Once the game’s main menu pops up and takes up the whole screen, click on the top button entitled “1 Player”. When the screen changes to a new menu, once again click on the top button, this time entitled “Driver”. The main portion of the screen will change to the driver loading/registration screen. If you have previously played the game and registered a driver, type in that name in the “name” textbox and click the “load profile” button, otherwise type in a new name into that very same textbox and click on “register profile” 

Next you select your vehicle by clicking on the top-left button entitled, appropriately enough, “vehicle”, which will cause the main screen to change to the car selection menu. Use the left and right arrow buttons to cycle through the available vehicles (note: if you do not have enough money to purchase more expensive vehicles, you will not be able to select them—shown in figure A-2). Once your desired car is on the screen (and by that virtue becomes your selected car), click on the “course” button found in the bottom-left part of the screen, and select your desired track by clicking on the left and right arrows in the main screen area.


Once have your desired track displayed in the menu, click on the “race” button found on the bottom portion of the screen—this will change the main menu area to a screen with a solitary “GO” button (shown in figure A-3). Click that, and, you guessed, you’re off to the races.


3.3 Playing a two-player game
--------------------------------------

On the main menu, click on the top-left button entitled “2 Players” (shown in figure B-1), which will change the screen to the 2 players menu. From there, click on the “vehicle 1” button and select player 1’s car using the left and right arrow buttons, and then click on the “vehicle 2” button and select player 2’s car using the left and right arrow buttons. Following that, click on the “track” button found in the bottom-left portion of the screen and select the desired track using, once again, the left and right arrow buttons.


Once all of the above steps are complete, click on the “race” button found on the bottom part of the screen. This will bring up a menu in the main portion of the screen asking you to select the amount of laps for the race (shown in figure B-2)—once you click on the desired amount of laps, all that remains is to hit the big “GO” button and the race will begin.



3.4 Player 1 car controls
------------------------------

Player 1:
ESC				- return to main menu
Up Arrow         	 	- Accelerate
Down Arrow        		- Brake
Left/Right Arrows 		- Turn
Right Ctrl        		- Turbo Boost
END Key          		- Change View
DEL key           		- Horn



3.5 Player 2 car controls
------------------------------

Player 2:
ESC				- return to main menu
'W' Key       			- Accelerate
'S' Key       			- Brake
'A'/'D' Keys  		- Turn
Left Shift    			- Turbo Boost
'V' Key        			- Change View
'F' Key       			- Horn




4 Troubleshooting
---------------------------------------------


4.1 Sound issues
---------------------

The game is set up to play 128 different sound effects at one time, plus one mp3. Throughout most of the game, only a dozen or so sound effects will be played at once, but sound would not be initialized at all if the PC’s sound card could not support 128 different software sound channels.

4.2 Control issues
----------------------

Some PC keyboards can't recognize certain combinations of 3 or more
simultaneous key presses.

4.3 OpenGL
----------------

Please ensure that you are running the latest drivers for your graphics
card as the RC Car Game stresses the hardware and drivers to a
greater extent than most available software and we rely on a number of
bug fixes hardware vendors incorporate into their latest drivers. To
obtain the latest drivers please visit the website of your graphics
card manufacturer.

Recommended Drivers:
www.ati.com - Catalyst 4.x
www.nvidia.com - Detonator 53.x or 56.x





5 Project Deviations
---------------------------------------------


5.1 Alterations to original design
-----------------------------------------     

-changed one track from an indoors course to an outdoors one
-removed objects that were meant to obscure the track
-displayed top 5 scores for each track instead of a general top 10 scores for both tracks 
-changed the price of the second car from $1000 to $500
-changed the starting amount of money from $0 to $100
-changed the original “starting-race” countdown sound effects to a single starting sound effect

5.2 Extra additions  
------------------------

-added extra camera views
-used binary file streams to store data
-implement a mini-map for each track
-implemented more advanced physics
-debug mode




6 Known Bugs
---------------------------------------------

6.1 A list of known bugs and glitches
---------------------------------------------

-in a two player game, if one of the players doesn’t move at all and the other player drives his car around into his motionless opponent, the collision detection will act up in a random fashion.
