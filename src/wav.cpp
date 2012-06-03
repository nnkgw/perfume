#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "wav.h"

CWav::CWav() {
  memset(&m_PCM, 0, sizeof(sPCM));
  memset(&m_AL,  0, sizeof(m_AL));
}

CWav::~CWav() {
  if (m_PCM.L) {
    free(m_PCM.L);
  }
  if (m_PCM.R) {
    free(m_PCM.R);
  }
}

int CWav::Load(const char* fn) {
  m_AL.buffer = alutCreateBufferFromFile("Perfume_globalsite_sound.wav");
  alGenSources(1, &m_AL.source);
  alSourcei(m_AL.source, AL_BUFFER, m_AL.buffer);

  FILE* fp = fopen(fn, "rb");
  if (!fp) { return -1; }
  char riff_chunk_id[4];
  fread(riff_chunk_id, 1, 4, fp); // RIFF
  long riff_chunk_size;
  fread(&riff_chunk_size, 4, 1, fp); // file size -8
  char riff_form_type[4];
  fread(riff_form_type, 1, 4, fp);  // WAVE
  char fmt_chunk_ID[4];
  fread(&fmt_chunk_ID, 1, 4, fp);   // "fmt "
  unsigned long fmt_chunk_size;
  fread(&fmt_chunk_size, 4, 1, fp);
  short fmt_wave_format_type;
  fread(&fmt_wave_format_type, 2, 1, fp);
  short fmt_channel;
  fread(&fmt_channel, 2, 1, fp);
  long fmt_samples_per_sec;
  fread(&fmt_samples_per_sec, 4, 1, fp);
  m_PCM.sample_per_sec = fmt_samples_per_sec;
  long fmt_bytes_per_sec;
  fread(&fmt_bytes_per_sec, 4, 1, fp);
  short fmt_block_size;
  fread(&fmt_block_size, 2, 1, fp);
  short fmt_bits_per_sample;
  fread(&fmt_bits_per_sample, 2, 1, fp);
  char data_chunk_ID[4];
  fread(data_chunk_ID, 1, 4, fp);
  long data_chunk_size;
  fread(&data_chunk_size, 4, 1, fp);
  m_PCM.length = data_chunk_size/4;
  m_PCM.play_time = m_PCM.length / fmt_bytes_per_sec;
  m_PCM.L = (double*)malloc(m_PCM.length * sizeof(double));
  m_PCM.R = (double*)malloc(m_PCM.length * sizeof(double));
  for(int i = 0; i < m_PCM.length; i++) {
    short data;
    fread(&data, 2, 1, fp);
    m_PCM.L[i] = (double)data / 32768.0;
    fread(&data, 2, 1, fp);
    m_PCM.R[i] = (double)data / 32768.0;
  }
  fclose(fp);
  Play();
  return 0;
}

float CWav::Get(float time){
  int index = static_cast<int>( time * m_PCM.sample_per_sec );
  index = ( index > m_PCM.length ) ? m_PCM.length : index;
  return (m_PCM.L[index] + m_PCM.R[index]) / 2.0f;
}

void CWav::Play() {
  if (m_AL.source){
    alSourcePlay(m_AL.source);
  }
}