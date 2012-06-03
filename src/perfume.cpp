#if defined(WIN32)
#pragma comment(lib,"glut32.lib")
#pragma comment(lib,"OpenAL32.lib")
#pragma comment(lib,"alut.lib")
#ifndef _DEBUG
#pragma comment(linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"")
#endif // _DEBUG
#endif // WIN32

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "bvh.h"
#include "wav.h"

#if defined(WIN32)
#include <GL/glut.h>
#include <AL/alut.h>
#elif defined(__APPLE__) || defined(MACOSX)
#include <GLUT/glut.h>
#endif // MACOSX

enum eModel{
  eModelAachan,
  eModelKashiyuka,
  eModelNocchi,

  eModelNum,
};

struct sModel{
  CBVH* bvh;
  float time;
};

struct sSound{
  CWav* wav;
};

sModel g_Model[eModelNum];
sSound g_Sound;

const char* BVH_FILE[] = { "aachan.bvh",
                           "kashiyuka.bvh",
                           "nocchi.bvh"     };
const char* WAV_FILE = "Perfume_globalsite_sound.wav";

void load_bvh(sModel* model, const char* fn){
  int ret = model->bvh->Load(fn);
  if (ret != 0) {
    printf( "cant read %s file!\n", fn );
    exit(0);
  }
}

void load_wav(sSound* sound, const char* fn){
  int ret = sound->wav->Load(fn);
  if (ret != 0) {
    printf( "cant read %s file!\n", fn );
    exit(0);
  }
}

void app_init() {
  GLfloat ambient[]  = {0.2f, 0.2f, 0.2f, 1.0f};
  GLfloat diffuse[]  = {0.5f, 0.5f, 0.5f, 1.0f};
  GLfloat specular[] = {1.0f, 1.0f, 1.0f, 1.0f};
  GLfloat position[] = {5.0f, 5.0f, 5.0f, 0.0f};

  glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
  glLightfv(GL_LIGHT0, GL_POSITION, position);

  glFrontFace(GL_CW);
//  glEnable(GL_LIGHTING);
//  glEnable(GL_LIGHT0);
  //glEnable(GL_AUTO_NORMAL);
  //glEnable(GL_NORMALIZE);
  glEnable(GL_DEPTH_TEST);

  glShadeModel(GL_SMOOTH);
  glEnable(GL_LINE_SMOOTH);
  glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);

  for(int i = 0; i < eModelNum; i++){
    g_Model[i].bvh = new CBVH();
    g_Model[i].time = 0.0f;
    load_bvh(&g_Model[i], BVH_FILE[i]);
  }
  g_Sound.wav = new CWav();
  load_wav(&g_Sound, WAV_FILE);
}

void app_exit( void ) { 
  alutExit();
}

void display(void){
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(0.0, 5.0, -10.0, 0.0, 0.0f, 0.0, 0.0, 1.0, 0.0); // eye(x,y,z), tgt(x,y,z), up(x,y,z)

#if 0
  GLfloat mat[] = {1.0, 0.0, 0.0, 1.0};
  glMaterialfv(GL_FRONT, GL_DIFFUSE, mat);
  glutSolidTeapot(1.0);
#else
  CBVH::sMaterial material[3];
  material[0].r = 1.0f;
  material[0].g = 0.0f;
  material[0].b = 0.0f;
  material[1].r = 0.0f;
  material[1].g = 1.0f;
  material[1].b = 0.0f;
  material[2].r = 0.0f;
  material[2].g = 0.0f;
  material[2].b = 1.0f;
  for (int i = 0; i < eModelNum; i++){
    int frame = static_cast<int>(g_Model[ i ].time / g_Model[ i ].bvh->GetFrameTime());
    if (frame > g_Model[i].bvh->GetFrameNum()){
      frame -= g_Model[i].bvh->GetFrameNum();
      g_Model[i].time = frame * g_Model[i].bvh->GetFrameTime();
      g_Sound.wav->Play();
    }
    g_Model[i].bvh->RenderPosture(frame, 0.01f, &material[i]);
  }
#endif

  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  gluOrtho2D( 0.0, 1.0f, 1.0f, 0.0 );
  glMatrixMode( GL_MODELVIEW );
  glPushMatrix();
  glLoadIdentity();
  for(int i = 0; i < 100; i++){
    float wave_height_l = ( g_Sound.wav->Get( g_Model[ 0 ].time + i     * 0.001f ) + 1.0f ) / 2.0f;
    float wave_height_r = ( g_Sound.wav->Get( g_Model[ 0 ].time + (i+1) * 0.001f ) + 1.0f ) / 2.0f;
    float base_height = -3.0f;
    glColor3d( 1.0f, 1.0f, 1.0f);
    glBegin(GL_LINES);
      glVertex2d( 0.1f + 0.008f *     i, 0.9f - wave_height_l * 0.1f );
      glVertex2d( 0.1f + 0.008f * (i+1), 0.9f - wave_height_r * 0.1f );
    glEnd();
  }
  glMatrixMode( GL_PROJECTION );
  glPopMatrix();
  glMatrixMode( GL_MODELVIEW );
  glPopMatrix();

  glutSwapBuffers();
}

void reshape(int w, int h) {
  glViewport(0, 0, (GLsizei)w, (GLsizei)h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(30.0, (double)w / (double)h, 1.0, 1000.0); // fovy, aspect, near, far
}

void idle(){
  static float old_time = 0.0f;
         float now_time = static_cast<float>(glutGet(GLUT_ELAPSED_TIME)) * 0.001; // msec
  for(int i = 0; i < eModelNum; i++){
    g_Model[i].time += (now_time - old_time);
  }
  old_time = now_time;
  glutPostRedisplay();
}

int main(int argc, char* argv[]){
  alutInit(&argc, argv);
  atexit(app_exit);

  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
  glutInitWindowSize(640, 480);
  glutCreateWindow(argv[0]);
  app_init();
  glutReshapeFunc(reshape);
  glutDisplayFunc(display);
  glutIdleFunc(idle);
  glutMainLoop();
  return 0;
}
