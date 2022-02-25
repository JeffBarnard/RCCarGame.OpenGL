////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// File      :  FModSound.h                                                   //
// Author    :  Radek Koncewicz                                               //
// Date      :  March 28, 2004                                                //
//                                                                            //
// Description:                                                               //
// The declaration/prototype of the FMod wrapper used to play all the sound   //
// effects and music in our RC game. I've documented the variables/functions  //
// and declared them in a specific order which should make it fairly clear on //
// how to implement this wrapper. Just follow the instructions and everything //
// should work without any problems. Hopefully.                               //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////


//------------------------------------------------------------------------------
// Header of FModSound.h                                                       
//------------------------------------------------------------------------------
#ifndef _FModSound_
#define _FModSound_

#include <vector>
#include <string>
#include <iostream>
#include <fstream>

// make sure to package our game with the fmod.dll or else the following two includes 
// will cause a rift in the space-time continuum
#include "fmod/fmod.h"
#include "fmod/fmod_errors.h"

using namespace std;

namespace Sound{

//------------------------------------------------------------------------------
// Struct declaration for a SoundObj object which encapsulates all basic sounds
//------------------------------------------------------------------------------
struct SoundObj
{
   // pointer to an FMod sound
   FSOUND_SAMPLE *cur_sound;
   // name of sound effect as read from file (without the .wav extension)
   string name;
};


//------------------------------------------------------------------------------
// Struct declaration for a SoundInfo object used to keep track of active sounds
//------------------------------------------------------------------------------
struct SoundInfo
{
   // name of sound effect as read from file (without the .wav extension)
   string name;
   // channel which the sound will occupy
   int channel;
   // identification number of the sound effect
   int id;
};


//------------------------------------------------------------------------------
// Class declaration for FModSound
//------------------------------------------------------------------------------
class FModSound 
{
   private:
      
      // this is the default path to all the music and sound effect, i.e. the folder 
      // "sfx". if the game exe is not in the main folder of the game (with sfx being 
      // one of its subfolders) you'll need to change this variable in the FModSound
      // constructor to the appropriate path. also, I set up this wrapper to arrange 
      // various sound effects into groups. these groups are simply text files stored 
      // in the sfx folder which are parsed in order to figure out which sound effects 
      // should currently be loaded into memory. the text files are named common.txt, 
      // track1.txt and track2.txt, and contain the names of the sound effect files 
      // (minus the .wav extension) which are separated by a newline whitespace 
      // (although separating them with any sort of a whitespace will work as well). 
      // this approach allows me to dynamically allocate/de-allocate memory for 
      // specific groups of sound effects and lets you guys add any sound effects to 
      // the text files without worrying about any sort of the memory management.
      string path;
      // vector of common sound effects
      vector<SoundObj> common;
      // vector of sound effects used by the first track
      vector<SoundObj> track1_sounds;
      // vector of sound effects used by the second track
      vector<SoundObj> track2_sounds;
      // vector of active sound effects 
      vector<SoundInfo> active_sounds;
      // the current MP3 being streamed (there will only be one used at a time)
      FSOUND_STREAM *current_song;
      // the channel that the above song will get assigned to
      int song_channel;
      // an integer which represents which current group of sounds (represented by
      // their corresponding text files) is currently taking up space in memory
      int group;

   public:
   
      // --default constructor
      //
      // instructions: create a global FModSound object in the game's main
      // constructor, whatever it might be, so it calls this constructor. it
      // might not seem like the most efficient way to do things, but it's actually
      // not that bad because a). we'll need sound effects everywhere, for all the 
      // menus, tracks, etc. and loading/unloading them at every menu change would be 
      // a horrendous drain on resources b). there will be times when we'll need to 
      // keep track of what sfx are loaded into memory, and cross-referencing them
      // without any global object serving as the buffer would be impossible c). this
      // approach allows me to code some functionality that will actually improve
      // performance based on what specific sounds/songs should be in memory d). with
      // a global FModSound object serving as the buffer, you don't need to worry 
      // about de-allocating ANYTHING, which would've been a huge pain.
      FModSound();
      
      // --destructor
      //
      // instructions: this is C++ so the destructor should be called automatically,
      // unless, of course, the destructor is just a useless gimmick and the compilers 
      // have been lying to me all this time...
      ~FModSound();
      
      // --loads common sound effects into memory, returns false if it fails 
      //
      // instructions: this function is automatically called by the constructor and 
      // loads some sound effects and music into memory such as the menu mouse-overs 
      // and the main-title theme (title.mp3). this will happen automatically so don't 
      // touch this function or I'll break your fingers.
      bool loadCommon();
      
      // --"unloads" common sound effects from memory, returns false if it fails 
      //
      // instructions: gets called by the destructor, so once again, no touchy-touchy
      // or your fingers will pay.
      bool unloadCommon();
      
      // --"unloads" sound effect that are currently active
      //
      // instructions: this function will get called by all other unload functions, so 
      // you'll never need to use it.
      bool unloadActives();
      
      // --loads music and sound effects for the first track, returns false if it
      // fails
      // 
      // instructions: call this along with all the other stuff involved in loading
      // the first track.
      bool loadTrack1();
      
      // --"unloads" all of the first track's sound effects, returns false if it fails
      //
      // instructions: call this function when the user is forced/chooses to go back
      // to the main menu screens (i.e. when it's time for all the other stuff you've 
      // loaded with the track to get wiped from memory).
      bool unloadTrack1();

      // --loads music and sound effects for the second track, returns false if it
      // fails
      // 
      // instructions: call this along with all the other stuff involved in loading
      // the second track.
      bool loadTrack2();
      
      // --"unloads" all the second track's sound effects, returns false if it fails
      //
      // instructions: call this function when the user is forced/chooses to go back
      // to the main menu screens (i.e. when it's time for all the other stuff you've 
      // loaded with the track to get wiped from memory).
      bool unloadTrack2();
      
      // --plays the song passed to it by name
      //
      // instructions: automatically used so don't worry about calling it. 
      bool playSong(string name);
      
      // --stops playing the current streaming song
      //
      // instructions: another automatic one, so seriously, don't touch it.
      bool stopSong();
      
      // --returns the size of the current vector being used to store SoundObj objects
      //
      // instructions: NO TOUCHING! none of you peons have the mental capacity to use 
      // this complex function, so just stay away!!!
      int getVectorSize() const;
      
      // --a common function used by many other ones, searches for a sound effect
      // in memory and returns index of vector with the passed name or -1 if it's not 
      // found
      //
      // instructions: this seems like a fairly simple function, but if anyone but
      // me uses it they WILL get cancer. Do you want to get cancer? What's that?
      // It doesn't sound like much fun? Well, it's not, so don't touch this 
      // or you will end up in the chemo ward.
      int searchSounds(string name) const;
      
      // --returns an FSOUND_SAMPLE pointer indicated by the index, NULL if it 
      // doesn't exist
      //
      // instructions: if you touch it, you will be killed.
      FSOUND_SAMPLE * getSample(int index) const;
      
      // --returns the number corresponding to the current group of sound effects 
      // in memory
      //
      // instructions: running out of clever ways of telling you not to use a 
      // function because it's called automatically (yes, that does mean don't use
      // it, it's only there for me). 
      int getGroup() const;
      
      // --returns the vector index if the passed parameters indicate an
      // active sound effect, -1 if the sound is not currently active
      //
      // instructions: same as above, HANDS OFF!!!
      int searchActives(string name, int id) const;
      
      // --plays a single sound effect, returns false if it fails
      //
      // instructions: call this function with the name of the file you wish to play 
      // (without the .wav). 
      bool playSound(string name); 
      
      // --plays a single sound effect that's looped, returns false if it fails
      //
      // instructions: call this function with the name of the file you wish to 
      // play (without the .wav) and an arbitrary int value that will be its id. 
      // you might keep the id at either 1 or 2, to represent the player number calling
      // the sound, or maybe implement it in range sets (i.e. player one would have 10 to
      // 19, while player two 20 to 29). either way, you can use this approach to, for 
      // example, play the looping car engine sound 50 times at once (simulating 50 car
      // engines) and individually pause/unpause each one of them using their id's. keep 
      // in mind that this function sets a sound effect that's loaded into memory to a 
      // looping mode, and if you play that sound effect afterwards using the playSound 
      // function, it will not go through any of the playLoopingSound functionality and 
      // you will be unable to pause or stop it from playing without unloading the whole 
      // set of sound effects from memory.
      bool playLoopingSound(string name, int id); 
      
      // --pauses/unpauses a looping sound effect, returns false if it fails
      //
      // instructions: call this function using the global FModSound object and pass
      // it the name of the looping sound effect (without the .wav) and its id to pause 
      // it or, if it's already paused, to start playing it again.
      bool pauseLoopingSound(string name, int id);
      
      // --returns the frequency of the indicated sound effect, 0 if sound doesn't
      // exist
      //
      // instructions: all .wav fales are recorded at different frequencies, so you 
      // need to use this function first if you want to properly adjust the rate at
      // which a specific sound effect is being played. also, you can only use this
      // function on looping sound effects as it relies on the channel/id variable.
      int getFrequency(string name, int id);
      
      // --changes the frequency of the specified sound to the passed "new_freq" value
      //
      // instructions: use this function to change the rate at which a sound effect
      // is being played. the rates are limited to all values from 100 to 705600, and 
      // -100 to -705600--negative values make the .wav file play in reverse. also, you
      // should make sure to use the getFrequency function first so you change the
      // frequency to something appropriate. i.e. the following would increase the rate
      // of the car engine sound by 10% (although it is a BIT inefficient to do it that
      // way--it'd be much better to only call getFrequency only once):
      // setFrequency("RCengine", 1, getFrequency("RCengine", 1) + getFrequency("RCengine", 1)/10);
      bool setFrequency(string name, int id, int new_freq);
};
}
// end macro definition
#endif