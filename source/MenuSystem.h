//////////////////////////////////////////////////////////////////////////////
////
////  GL Initialization, Main Render Loop, and User Input				
////
////  Patrick Crawford  2003
////																			
//////////////////////////////////////////////////////////////////////////////

#include "texture.h"
#include "SystemSettings.h"
#include "gldraw.h"
#include <vector>

using namespace std;

#define MAX_PLAYER_CHARS 15

//////////////////////////////////////////
////  function declarations
//////////////////////////////////////////

struct player 
{
	float money;
	int cars;  
	string name;
};

struct scores
{
	float time;
	int track;
	string player_name, car;
};

class MenuSystem
{
	private:

	int nameCharCtr;
	enum MenuStates currentState;
	
	
	vector<unsigned int> textures;
	
	public:

	SystemSettings gameSettings;
	enum ErrorStates currentErrorState;
	bool hasProfile;
	
	MenuSystem(){nameCharCtr=0;};
	int initGLMenu(void);
	int whichMouseObject(int,int);
	
	// screen state methods
	void drawTitleScreen(int,int);
	void drawTitleScreenSelect();
	void drawOnePlayerTitle(int,int);
	void drawOnePlayerTitleSelect();
	void drawTwoPlayerTitle(int,int);
	void drawTwoPlayerTitleSelect();
	void drawOnePlayerCourse(int,int);
	void drawOnePlayerCourseSelect();
	void drawOnePlayerDriver(int,int);
	void drawOnePlayerDriverSelect();
	void drawOnePlayerRace(int,int);
	void drawOnePlayerRaceSelect();
	void drawOnePlayerVehicle(int,int);
	void drawOnePlayerVehicleSelect();
	void drawTwoPlayer1Vehicle(int,int);
	void drawTwoPlayer1VehicleSelect();
	void drawTwoPlayer2Vehicle(int,int);
	void drawTwoPlayer2VehicleSelect();
	void drawTwoPlayerCourse(int,int);
	void drawTwoPlayerCourseSelect();
	void drawTwoPlayerRace(int,int);
	void drawTwoPlayerRaceSelect();
	void drawHighScores(int,int);
	void drawHighScoresSelect();
	void drawCredits();

	void drawLoadingScreen();

	void processKey(int);
	void processClicked(int);
	void printKeys(void);
	void setState(enum MenuStates);
	void deleteObjects(void);
	enum MenuStates getState() { return currentState; }
	void executeRenderLoop(void){ 
		DrawGLScene(gameSettings.numPlayers, &gameSettings); 
	} // will accept a System Settings object
	void executeCheckForMovement(void){
		CheckForMovement(&gameSettings);
	}

	// save player data 
	bool save_player(player &plr);

	// load player data 
	bool load_player(void);

	// save high scores 
	void save_scores(scores &scr);

	// load player data
	void load_scores(void);
	
	// check if the save file exists
	bool MenuSystem::check_save_file_exists(player &plr);
};

enum MenuStates 
{ 
	CREDITS_SCREEN,
	TITLE_SCREEN,	
	INIT_GAMEPLAY, 
	GAMEPLAY,
	QUIT,
	ONE_PLAYER_TITLE,
	ONE_PLAYER_COURSE,
	ONE_PLAYER_DRIVER,
	ONE_PLAYER_RACE,
	ONE_PLAYER_VEHICLE,
	TWO_PLAYER_TITLE,
	TWO_PLAYER_1_VEHICLE,
	TWO_PLAYER_2_VEHICLE,
	TWO_PLAYER_COURSE,
	TWO_PLAYER_RACE,
	HIGH_SCORES,
};

enum ErrorStates 
{ 
	ALL_OK,
	DRIVER_NAME_TAKEN,
	PROFILE_REGISTERED,
	PROFILE_LOADED,
	DRIVER_DOES_NOT_EXIST,
	NO_PROFILE_SELECTED
};






