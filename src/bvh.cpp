#include <string.h>
#include <stdio.h>
#include <vector>

#include "BVH.h"

#if defined(WIN32)
#include <GL/glut.h>
#elif defined(__APPLE__) || defined(MACOSX)
#include <GLUT/glut.h>
#endif

CBVH::CBVH()
: m_ChannelNum(0)
, m_FrameNum(0)
, m_FrameTime(0.0)
, m_Motion(0) {
  Clear();
}

CBVH::~CBVH() {
  Clear();
}

void CBVH::Clear() {
  for (unsigned int i = 0; i < m_Joints.size(); i++) {
    delete m_Joints[ i ];
  }
  for (unsigned int i = 0; i < m_Channels.size(); i++) {
    delete m_Channels[ i ];
  }
  if ( m_Motion ) {
    delete m_Motion;
    m_Motion = 0;
  }
  m_Joints.clear();
  m_JointIndex.clear();
  m_Channels.clear();
  m_ChannelNum = 0;
  m_FrameNum   = 0;
  memset(&m_Material, 0, sizeof(sMaterial));
}

int CBVH::Load(const char* fn) {
  FILE* fp = fopen(fn, "rt");
  if (!fp) { return -1; }
  sJoint* joint     = 0;
  sJoint* new_joint = 0;
  std::vector<sJoint*> joint_stack;
  const char SEPARATER[] = " :,\t\n";
  const int  BUFFER_SIZE = 4096;
  bool is_site = false;
  char line[ 4096 ];
  while(!feof(fp)) {
    fgets(line, BUFFER_SIZE, fp);
    char* token = strtok(line, SEPARATER);
    if (token == 0) continue;
    if ((strcmp(token, "ROOT" ) == 0) ||
        (strcmp(token, "JOINT") == 0)) {
      new_joint = new sJoint();
      new_joint->index = m_Joints.size();
      new_joint->parent = joint;
      new_joint->has_site = false;
      new_joint->offset[0] = 0.0;
      new_joint->offset[1] = 0.0;
      new_joint->offset[2] = 0.0;
      new_joint->site[0] = 0.0;
      new_joint->site[1] = 0.0;
      new_joint->site[2] = 0.0;
      token = strtok(0, "");
      while (*token == ' ') { token++; }
      new_joint->name = token;
      m_Joints.push_back(new_joint);
      if (joint) {
        joint->children.push_back(new_joint);
      }
      m_JointIndex[new_joint->name] = new_joint;
      continue;
    }
    if (strcmp(token, "{") == 0) {
      joint_stack.push_back(joint);
      joint = new_joint;
      continue;
    }
    if (strcmp(token, "}") == 0) {
      joint = joint_stack.back();
      joint_stack.pop_back();
      is_site = false;
      continue;
    }
    if (strcmp(token, "OFFSET") == 0) {
      token = strtok(0, SEPARATER);
      double x = token ? atof(token) : 0.0;
      token = strtok(0, SEPARATER);
      double y = token ? atof(token) : 0.0;
      token = strtok(0, SEPARATER);
      double z = token ? atof(token) : 0.0;
      if (is_site) {
        joint->has_site = true;
        joint->site[0] = x;
        joint->site[1] = y;
        joint->site[2] = z;
      } else {
        joint->offset[0] = x;
        joint->offset[1] = y;
        joint->offset[2] = z;
      }
      continue;
    }
    if (strcmp(token, "CHANNELS") == 0) {
      token = strtok(0, SEPARATER);
      joint->channels.resize(token ? atoi(token) : 0);
      for (unsigned int i = 0; i < joint->channels.size(); i++) {
        sChannel* channel = new sChannel();
        channel->joint = joint;
        channel->index = m_Channels.size();
        token = strtok(NULL, SEPARATER);
        if (       strcmp(token, "Xrotation") == 0) {
          channel->type = X_ROTATION;
        } else if (strcmp(token, "Yrotation") == 0) {
          channel->type = Y_ROTATION;
        } else if (strcmp(token, "Zrotation") == 0) {
          channel->type = Z_ROTATION;
        } else if (strcmp(token, "Xposition") == 0) {
          channel->type = X_POSITION;
        } else if (strcmp(token, "Yposition") == 0) {
          channel->type = Y_POSITION;
        } else if (strcmp(token, "Zposition") == 0) {
          channel->type = Z_POSITION;
        }
        m_Channels.push_back(channel);
        joint->channels[i] = channel;
      }
    }
    if ((strcmp(token, "End") == 0)) {
      new_joint = joint;
      is_site = true;
      continue;
    }
    if (strcmp(token, "MOTION") == 0) {
      break;
    }
  }
  fgets(line, BUFFER_SIZE, fp);
  int scan_num = sscanf(line, "Frames: %d", &m_FrameNum);
  if (scan_num == 1) {
    fgets(line, BUFFER_SIZE, fp);
    scan_num = sscanf(line, "Frame Time: %lf", &m_FrameTime);
    if (scan_num == 1) {
      m_ChannelNum = m_Channels.size();
      m_Motion = new double[ m_FrameNum * m_ChannelNum ];
      for (int i = 0; i < m_FrameNum; i++) {
        fgets(line, BUFFER_SIZE, fp);
        char* token = strtok(line, SEPARATER);
        for (int j = 0; j < m_ChannelNum; j++) {
          if (token != 0) {
            m_Motion[ i * m_ChannelNum + j ] = atof(token);
            token = strtok(0, SEPARATER);
          }
        }
      }
    }
  }
  fclose(fp);
  return 0;
}

void CBVH::RenderPosture(int frame_no, float scale, sMaterial* material) {
  RenderJoint(m_Joints[ 0 ], m_Motion + frame_no * m_ChannelNum, scale, material);
}

void CBVH::RenderJoint(const sJoint* joint, const double* data, float scale, sMaterial* material) {
  glPushMatrix();
  if (joint->parent == 0) {
    glTranslatef(data[0] * scale, data[1] * scale, data[2] * scale);
  } else {
    glTranslatef(joint->offset[0] * scale, joint->offset[1] * scale, joint->offset[2] * scale);
  }
  for (unsigned int i = 0; i < joint->channels.size(); i++) {
    sChannel* channel = joint->channels[i];
    if (       channel->type == X_ROTATION) {
      glRotatef(data[ channel->index ], 1.0f, 0.0f, 0.0f);
    } else if (channel->type == Y_ROTATION) {
      glRotatef(data[ channel->index ], 0.0f, 1.0f, 0.0f);
    } else if (channel->type == Z_ROTATION) {
      glRotatef(data[ channel->index ], 0.0f, 0.0f, 1.0f);
    }
  }
  if (joint->children.size() == 0) {
    RenderBone(0.0f, 0.0f, 0.0f, joint->site[ 0 ] * scale, joint->site[ 1 ] * scale, joint->site[ 2 ] * scale, material);
  }
  if (joint->children.size() == 1) {
    sJoint*  child = joint->children[0];
    RenderBone(0.0f, 0.0f, 0.0f, child->offset[ 0 ] * scale, child->offset[ 1 ] * scale, child->offset[ 2 ] * scale, material);
  }
  if (joint->children.size() > 1) {
    float center[ 3 ] = { 0.0f, 0.0f, 0.0f };
    for (unsigned int i = 0; i < joint->children.size(); i++) {
      sJoint* child = joint->children[ i ];
      center[ 0 ] += child->offset[ 0 ];
      center[ 1 ] += child->offset[ 1 ];
      center[ 2 ] += child->offset[ 2 ];
    }
    center[ 0 ] /= joint->children.size() + 1;
    center[ 1 ] /= joint->children.size() + 1;
    center[ 2 ] /= joint->children.size() + 1;
    RenderBone(0.0f, 0.0f, 0.0f, center[ 0 ] * scale, center[ 1 ] * scale, center[ 2 ] * scale, material);
    for (unsigned int i = 0; i < joint->children.size(); i++) {
      sJoint*  child = joint->children[ i ];
      RenderBone(       center[0] * scale,        center[1] * scale,        center[2] * scale,
                 child->offset[0] * scale, child->offset[1] * scale, child->offset[2] * scale, material);
    }
  }
  for (unsigned int i = 0; i < joint->children.size(); i++) {
    RenderJoint(joint->children[ i ], data, scale, material);
  }
  glPopMatrix();
}

void CBVH::RenderBone(float x0, float y0, float z0, float x1, float y1, float z1, sMaterial* material) {
  glColor3d(material->r, material->g, material->b);
  glBegin(GL_LINES);
    glVertex3f(x0, y0, z0);
    glVertex3f(x1, y1, z1);
  glEnd();
}
