////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// File      :  FModSound.cpp                                                 //
// Author    :  Radek Koncewicz                                               //
// Date      :  March 28, 2004                                                //
//                                                                            //
// Description:                                                               //
// Implementation of the FMod wrapper used to play all the sound effects and  //
// music in our RC game.                                                      //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////


//------------------------------------------------------------------------------
// Header of FModSound.cpp                                                     
//------------------------------------------------------------------------------
#include "FModSound.h"

#define MUSIC_VOLUME 255

using namespace Sound;

//------------------------------------------------------------------------------
// Class definition of FModSound
//------------------------------------------------------------------------------
FModSound::FModSound()
{
   path = "..\\sfx\\";
   
   if(!FSOUND_Init(44100, 64, 0))
   {
      cout << "Error!\n";
      cout << FMOD_ErrorString(FSOUND_GetError());
      FSOUND_Close();
   }

   loadCommon();
}
      
FModSound::~FModSound()
{
   stopSong();
   
   if(track1_sounds.size() > 0)
      unloadTrack1();
   if(track2_sounds.size() > 0)
      unloadTrack2();
   if(common.size() > 0)
      unloadCommon();
   FSOUND_Close();
}
      
bool FModSound::loadCommon()
{
   bool rtn = false;
   string tmp_name, tmp_path = path + "common.txt";
   SoundObj tmp_SoundObj;
   ifstream file_stream(tmp_path.c_str());

   if(file_stream)
   {
      rtn = true;
      song_channel = 1;
      playSong(path + "title.mp3");
      while(file_stream >> tmp_name)
      {
         tmp_SoundObj.name = tmp_name;
         tmp_name = path + tmp_name + ".wav";
         tmp_SoundObj.cur_sound = FSOUND_Sample_Load(FSOUND_UNMANAGED, tmp_name.c_str(), FSOUND_NORMAL, 0, 0);
         if(!tmp_SoundObj.cur_sound)
         {
            rtn = false;
            break;
         }
         common.push_back(tmp_SoundObj);
      }
      file_stream.close();
      group = 0;
      return rtn;
   }
   
   return rtn;
}
      
bool FModSound::unloadCommon()
{
   int size = getVectorSize();
   int i = getVectorSize() - 1;
   
   unloadActives();
   
   if(size >= 0)
   {
      while(!common.empty())
      {
         FSOUND_Sample_Free(common[i].cur_sound);
         common.pop_back();
         i--;
      }
      stopSong();
      return true;
   }
   
   return false;
}
      
bool FModSound::unloadActives()
{
   if(active_sounds.size() > 0)
   {
      while(!active_sounds.empty())
      {
         active_sounds.pop_back();
      }
      return true;
   }
   
   return false;
} 
     
bool FModSound::loadTrack1()
{
   bool rtn = false;
   string tmp_name, tmp_path = path + "track1.txt";
   SoundObj tmp_SoundObj;
   ifstream file_stream(tmp_path.c_str());

   if(file_stream)
   {
      rtn = true;
      stopSong();
      while(file_stream >> tmp_name)
      {
         tmp_SoundObj.name = tmp_name;
         tmp_name = path + tmp_name + ".wav";
         tmp_SoundObj.cur_sound = FSOUND_Sample_Load(FSOUND_UNMANAGED, tmp_name.c_str(), FSOUND_NORMAL, 0, 0);
         if(!tmp_SoundObj.cur_sound)
         {
            rtn = false;
            break;
         }
         track1_sounds.push_back(tmp_SoundObj);
      }
      file_stream.close();
      playSong(path + "track1.mp3");
      group = 1;
      return rtn;
   }
   
   return rtn;
}
      
bool FModSound::unloadTrack1()
{
   int size = getVectorSize();
   int i = getVectorSize() - 1;
   
   unloadActives();
   
   if(size >= 0)
   {
      while(!track1_sounds.empty())
      {
         FSOUND_Sample_Free(track1_sounds[i].cur_sound);
         track1_sounds.pop_back();
         i--;
      }
      stopSong();
      loadCommon();
      return true;
   }
   
   return false;
}

bool FModSound::loadTrack2()
{
   bool rtn = false;
   string tmp_name, tmp_path = path + "track2.txt";
   SoundObj tmp_SoundObj;
   ifstream file_stream(tmp_path.c_str());

   if(file_stream)
   {
      rtn = true;
      stopSong();
      while(file_stream >> tmp_name)
      {
         tmp_SoundObj.name = tmp_name;
         tmp_name = path + tmp_name + ".wav";
         tmp_SoundObj.cur_sound = FSOUND_Sample_Load(FSOUND_UNMANAGED, tmp_name.c_str(), FSOUND_NORMAL, 0, 0);
         if(!tmp_SoundObj.cur_sound)
         {
            rtn = false;
            break;
         }
         track2_sounds.push_back(tmp_SoundObj);
      }
      file_stream.close();
      playSong(path + "track2.mp3");
      group = 2;
      return rtn;
   }
   
   return rtn;
}
      
bool FModSound::unloadTrack2()
{
   int size = getVectorSize();
   int i = getVectorSize() - 1;
   
   unloadActives();
   
   if(size >= 0)
   {
      while(!track2_sounds.empty())
      {
         FSOUND_Sample_Free(track2_sounds[i].cur_sound);
         track2_sounds.pop_back();
         i--;
      }
      stopSong();
      loadCommon();
      return true;
   }
   
   return false;
}

bool FModSound::playSong(string name)
{
   current_song = FSOUND_Stream_Open(name.c_str(), FSOUND_LOOP_NORMAL | FSOUND_MPEGACCURATE, 0, 0);
   FSOUND_SetVolume(song_channel, MUSIC_VOLUME);
   
   if (current_song)
   {
      if(song_channel = FSOUND_Stream_Play(1, current_song) >= 0)
      {
         if(FSOUND_SetVolume(song_channel, MUSIC_VOLUME))
         {
            return true;  
         }
      }
   }

   return false;
}

bool FModSound::stopSong()
{
   if(current_song)
   {
      FSOUND_Stream_Stop(current_song);
      //FSOUND_Stream_Close(current_song);
      return true;
   }
   
   return false;
}
  
int FModSound::getVectorSize() const
{
   if(group == 0)
   {
      return common.size();
   }
   else if(group == 1)
   {
      return track1_sounds.size();
   }
   else if(group == 2)
   {
      return track2_sounds.size();
   }
   
   return -1;
}

int FModSound::searchSounds(string name) const
{
   int i, size = getVectorSize();
   
   if(group == 0)
   {
      for(i = 0; i < size; i++)
      {
         if(name == common[i].name)
            return i;
      }
   }
   else if(group == 1)
   {
      for(i = 0; i < size; i++)
      {
         if(name == track1_sounds[i].name)
            return i;
      }
   }
   else if(group == 2)
   {
      for(i = 0; i < size; i++)
      {
         if(name == track2_sounds[i].name)
            return i;
      }
   }
      
   return -1;
}

FSOUND_SAMPLE * FModSound::getSample(int index) const
{
   int size = getVectorSize();
   
   if(index < size)
   {
      if(group == 0)
      {
         return common[index].cur_sound;
      }
      else if(group == 1)
      {
         return track1_sounds[index].cur_sound;
      }
      else if(group == 2)
      {
         return track2_sounds[index].cur_sound;
      }
   }
   
   return NULL;
}

int FModSound::getGroup() const
{
   return group;
}

int FModSound::searchActives(string name, int id) const
{
   int i, size = active_sounds.size();
   
   for(i = 0; i < size; i++)
   {
      if(active_sounds[i].name == name && active_sounds[i].id == id)
         return i;
   }
   
   return -1;
}

bool FModSound::playSound(string name)
{
   int index = searchSounds(name);
   
   if(index >= 0)
   {
      FSOUND_SAMPLE *tmp = getSample(index);
      if(tmp != NULL && FSOUND_PlaySound(FSOUND_FREE, tmp) >= 0)
         return true;
   }
   
   return false;
}
      
bool FModSound::playLoopingSound(string name, int id)
{
   int index = searchSounds(name);
   
   if(index >= 0)
   {
      FSOUND_SAMPLE * tmp = getSample(index);
      
      if(tmp != NULL && searchActives(name, id) == -1)
      {
         FSOUND_Sample_SetMode(tmp, FSOUND_LOOP_NORMAL);
         SoundInfo tmp_active;
         tmp_active.id = id;
         tmp_active.name = name;
         tmp_active.channel = FSOUND_PlaySound(FSOUND_FREE, tmp);
         active_sounds.push_back(tmp_active);
         return true;
      }
   }
   
   return false;
}
      
bool FModSound::pauseLoopingSound(string name, int id)
{
   int index = searchSounds(name);
   
   if(index >= 0)
   {
      int active_index = searchActives(name, id);
      if(active_index >= 0)
      {
         FSOUND_SetPaused(active_sounds[active_index].channel, !FSOUND_GetPaused(active_sounds[active_index].channel)); 
         return true;
      }
   }
   
   return false;
}

int FModSound::getFrequency(string name, int id)
{
   int index = searchSounds(name);
   
   if(index >= 0)
   {
      int active_index = searchActives(name, id);
      if(active_index >= 0)
      {
         return FSOUND_GetFrequency(active_sounds[active_index].channel);
      }
   }
   
   return 0;
}

bool FModSound::setFrequency(string name, int id, int new_freq)
{
   int index = searchSounds(name);
   
   if(index >= 0)
   {
      int active_index = searchActives(name, id);
      if(active_index >= 0)
      {
         FSOUND_SetFrequency(active_sounds[active_index].channel, new_freq);
         return true;
      }
   }
   
   return false;
}
