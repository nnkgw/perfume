#ifndef _WAV_H_
#define _WAV_H_

#if defined(WIN32)
#include <al.h>
#include <alc.h>
#elif defined(__APPLE__) || defined(MACOSX)
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#else
#include <AL/al.h>
#include <AL/alc.h>
#endif

class CWav{
public:
  CWav();
  ~CWav();
  int   Load(const char* fn);
  float Get(float time);
  int   GetHeader(const char* fn, int* length, int* sample_per_sec, float* play_time, int* raw_offset);
  int   GetStream(const char* fn, int offset, short* buffer, int size);
  float GetPlayedTime();
  void  Play();
private:
  struct sPCM{
    short*  raw;
    int     length;
    int     sample_per_sec;
    float   play_time;
    int     raw_offset;
  };
  struct sAL{
    ALCdevice*  device;
    ALCcontext* context;
    ALuint      buffer;
    ALuint      source;
  };
  sPCM m_PCM;
  sAL  m_AL;
};

#endif // _WAV_H_
