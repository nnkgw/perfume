#ifndef _WAV_H_
#define _WAV_H_

#include <AL/alut.h>

class CWav{
public:
  CWav();
  ~CWav();
  int   Load(const char* fn);
  float Get(float time);
  void  Play();
private:
  struct sPCM{
    double* L;
    double* R;
    short*  raw;
    long    length;
    long    sample_per_sec;
    float   play_time;
  };
  struct sAL{
    ALuint buffer;
    ALuint source;
  };
  sPCM m_PCM;
  sAL  m_AL;
};

#endif // _WAV_H_
