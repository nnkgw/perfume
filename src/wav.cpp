#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "wav.h"

CWav::CWav() {
  memset(&m_PCM, 0, sizeof(sPCM));
  memset(&m_AL,  0, sizeof(m_AL));
}

CWav::~CWav() {
  if (m_PCM.raw) {
    free(m_PCM.raw);
    m_PCM.raw = 0;
  }
  alDeleteSources(1, &m_AL.source);
  alDeleteBuffers(1, &m_AL.buffer);
  alcMakeContextCurrent(NULL);
  alcDestroyContext(m_AL.context);
  alcCloseDevice(m_AL.device);
}

int CWav::Load(const char* fn) {
  GetHeader(fn, &m_PCM.length, &m_PCM.sample_per_sec, &m_PCM.play_time, &m_PCM.raw_offset);
  FILE* fp = fopen(fn, "rb");
  if (!fp) { return -1; }
  fseek(fp, m_PCM.raw_offset, SEEK_SET);
  m_PCM.raw = (short*)malloc(m_PCM.length * 2 * sizeof(short));
  int idx = 0;
  for(int i = 0; i < m_PCM.length; i++) {
    short data;
    fread(&data, 2, 1, fp);
    m_PCM.raw[idx++] = data;
    fread(&data, 2, 1, fp);
    m_PCM.raw[idx++] = data;
  }
  fclose(fp);
  m_AL.device  = alcOpenDevice(NULL);
  m_AL.context = alcCreateContext(m_AL.device, NULL);
  alcMakeContextCurrent(m_AL.context);
  alGenBuffers(1, &m_AL.buffer);
  alBufferData(m_AL.buffer, AL_FORMAT_STEREO16, m_PCM.raw, m_PCM.length * 2 * sizeof(ALshort), 44100);
  alGenSources(1, &m_AL.source);
  alSourcei(m_AL.source, AL_BUFFER, m_AL.buffer);
//  alSourcei(m_AL.source, AL_LOOPING, AL_TRUE );
  Play();
  return 0;
}

float CWav::Get(float time) {
  int index = static_cast<int>( time * m_PCM.sample_per_sec );
  index = ( index > m_PCM.length ) ? m_PCM.length : index;
  return ((double)m_PCM.raw[index*2] + (double)m_PCM.raw[index*2+1]) / 32768.0f / 2.0f;
}

int CWav::GetHeader(const char* fn, int* length, int* sample_per_sec, float* play_time, int* raw_offset) {
  FILE* fp = fopen(fn, "rb");
  if (!fp) { return -1; }
  char riff_chunk_id[4];
  fread(riff_chunk_id, 1, 4, fp); // RIFF
  int riff_chunk_size;
  fread(&riff_chunk_size, 4, 1, fp); // file size -8
  char riff_form_type[4];
  fread(riff_form_type, 1, 4, fp);  // WAVE
  char fmt_chunk_ID[4];
  fread(&fmt_chunk_ID, 1, 4, fp);   // "fmt "
  unsigned int fmt_chunk_size;
  fread(&fmt_chunk_size, 4, 1, fp);
  short fmt_wave_format_type;
  fread(&fmt_wave_format_type, 2, 1, fp);
  short fmt_channel;
  fread(&fmt_channel, 2, 1, fp);
  int fmt_samples_per_sec;
  fread(&fmt_samples_per_sec, 4, 1, fp);
  m_PCM.sample_per_sec = fmt_samples_per_sec;
  int fmt_bytes_per_sec;
  fread(&fmt_bytes_per_sec, 4, 1, fp);
  short fmt_block_size;
  fread(&fmt_block_size, 2, 1, fp);
  short fmt_bits_per_sample;
  fread(&fmt_bits_per_sample, 2, 1, fp);
  char data_chunk_ID[4];
  fread(data_chunk_ID, 1, 4, fp);
  int data_chunk_size;
  fread(&data_chunk_size, 4, 1, fp);
  *length = data_chunk_size / 4;
  *play_time = *length / fmt_bytes_per_sec;
  *sample_per_sec = fmt_samples_per_sec;
  *raw_offset = ftell(fp);
  fclose(fp);
  return 0;
}

int CWav::GetStream(const char* fn, int offset, short* buffer, int size) {
  FILE* fp = fopen(fn, "rb");
  if (!fp) { return -1; }
  fseek(fp, offset, SEEK_SET);
  int idx = 0;
  for(int i = 0; i < size / 2; i++) {
    short data;
    fread(&data, 2, 1, fp);
    buffer[idx++] = data;
    fread(&data, 2, 1, fp);
    buffer[idx++] = data;
  }
  fclose(fp);
  return 0;
}

float CWav::GetPlayedTime() {
  ALfloat sec = 0.0f;
  if (m_AL.source){
    alGetSourcef(m_AL.source, AL_SEC_OFFSET, &sec);
  }
  return sec;
}

void CWav::Play() {
  if (m_AL.source){
    alSourcePlay(m_AL.source);
  }
}
