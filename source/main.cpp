/************************************************/
/************************************************/
/**                                            **/
/**               DANGER! DANGER!              **/
/**               DANGER! DANGER!              **/
/**       MAKE SURE TO INCLUDE: FMOD.DLL       **/
/**       WITH FINAL GAME EXE!!!!!!!!!!!       **/
/**                                            **/
/************************************************/
/************************************************/

#include <string.h>
#include <stdio.h>
#include <math.h>
#include <windows.h>
#include <conio.h>
#include "FModSound.h"

int main(int argc, char *argv[])
{
   // this declares an FModSound object (should be global in the game) and
   // automatically loads the common sound effect group into memory
   FModSound gbsound;
   
   // simple int which corresponds to the key hit in the below loop
   int key = 0;
    
   printf("+-----------------------------------------------------------------------+\n");
   printf("| Press '1'   to load/unload sounds/song for track 1                    |\n");
   printf("| Press '2'   to load/unload sounds/song for track 2                    |\n");
   printf("| Press '3'   (common only) to play a sound effect                      |\n");
   printf("| Press '4'   (track 1 only) to play a sound effect                     |\n");
   printf("| Press '5'   (track 1 only) to play a looped sound effect              |\n");
   printf("| Press '6'   (track 1 only) to pause the above sound effect            |\n");
   printf("| Press '7'   (track 2 only) to play a sound effect                     |\n");
   printf("| Press '8'   (track 1 & 2 only) to play a sound effect                 |\n");
   printf("| Press ESC   to quit                                                   |\n");
   printf("+-----------------------------------------------------------------------+\n");

   // update screen loop
   do
   {
      if(kbhit())
      {
         key = getch();
         
         // load track1 sounds. if track1 sounds are already loaded, unload them (which
         // also causes the common sound effect to be automatically loaded back into
         // memory)
         if (key == '1')
         {
            if(gbsound.getGroup() == 1)
               gbsound.unloadTrack1();
            else
               gbsound.loadTrack1();
         }
         
         // load track2 sounds. if track2 sounds are already loaded, unload them (which
         // also causes the common sound effect to be automatically loaded back into
         // memory)
         if (key == '2')
         {
            if(gbsound.getGroup() == 2)
               gbsound.unloadTrack2();
            else
               gbsound.loadTrack2();
         }
         
         // play simple sound effect--this will only work if the current group of
         // sound effects loaded into memory is the "common" group
         if (key == '3')
         {
            gbsound.playSound("MENUbuttonhover");
         }
         
         // play simple sound effect--this will only work if the current group of
         // sound effects loaded into memory is the "track1" group
         if (key == '4')
         {
            gbsound.playSound("RCturbo");
         }
         
         // play a looped sound effect--this will only work if the current 
         // group of sound effects loaded into memory is the "track1" group
         //
         // WARNING--if you play the same exact sound effect as "looping" two times
         // in a row, you'll only be able to unpause the last one, so if you press
         // the 4 key about seven times, then pause will only pause the last one, so 
         // the other six will continue to play
         if (key == '5')
         {
            gbsound.playLoopingSound("oilslick", 1);
         }
         
         // play a looped sound effect--this will only work if the current 
         // group of sound effects loaded into memory is the "track1" group
         if (key == '6')
         {
            gbsound.pauseLoopingSound("oilslick", 1);
         }
         
         // play simple sound effect--this will only work if the current group of
         // sound effects loaded into memory is the "track2" group
         if (key == '7')
         {
            gbsound.playSound("smokealarm");
         }
         
         // play simple sound effect--this will only work if the current group of
         // sound effects loaded into memory is the "track1" group OR the "track2"
         // group
         if (key == '8')
         {
            gbsound.playSound("RCengine");
         }
         
         
         
         if (key == 'z')
         {
            gbsound.playLoopingSound("RCengine", 1);
         }
         if (key == 'x')
         {
            gbsound.pauseLoopingSound("RCengine", 1);
         }
         if (key == 'c')
         {
            gbsound.playLoopingSound("RCengine", 2);
            gbsound.setFrequency("RCengine", 2, 15000);
         }
         if (key == 'v')
         {
            gbsound.pauseLoopingSound("RCengine", 2);
         }
         if (key == 'b')
         {
            gbsound.playLoopingSound("RCengine", 3);
            gbsound.setFrequency("RCengine", 3, 8000);
         }
         if (key == 'n')
         {
            gbsound.pauseLoopingSound("RCengine", 3);
         }
      }
      
      Sleep(50);
   } while (key != 27);
}
