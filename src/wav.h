#ifndef _WAV_H_
#define _WAV_H_

class CWav{
public:
  CWav();
  ~CWav();
  int   Load(const char* fn);
  float Get(float time);
private:
  struct sPCM{
    double* L;
    double* R;
    long    length;
    long    sample_per_sec;
    float   play_time;
  }; 
  sPCM m_PCM;
};

#endif // _WAV_H_
