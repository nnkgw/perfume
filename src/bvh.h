#ifndef _BVH_H_
#define _BVH_H_

#include <string>
#include <vector>
#include <map>

class CBVH{
public:

  struct sMaterial{
    float r;
    float g;
    float b;
  };

  enum eChannel{
    X_ROTATION,
    Y_ROTATION,
    Z_ROTATION,

    X_POSITION,
    Y_POSITION,
    Z_POSITION
  };

  struct sJoint;

  struct sChannel{
    sJoint*  joint;
    eChannel type;
    int      index;
  };

  struct sJoint{
    std::string            name;
    int                    index;
    sJoint*                parent;
    std::vector<sJoint*>   children;
    double                 offset[3];
    bool                   has_site;
    double                 site[3];
    std::vector<sChannel*> channels;
  };

  CBVH();
  ~CBVH();

  int    Load(const char* fn);
  void   RenderPosture(int frame_no, float scale, sMaterial* material);
  int    GetFrameNum(){  return m_FrameNum;  }
  double GetFrameTime(){ return m_FrameTime; }

private:
  std::vector<sJoint*>           m_Joints;
  std::map<std::string, sJoint*> m_JointIndex;
  std::vector<sChannel*>         m_Channels;
  int                            m_ChannelNum;
  int                            m_FrameNum;
  double                         m_FrameTime;
  double*                        m_Motion; // [frame][channel]
  sMaterial                      m_Material;

  void Clear();

  void RenderJoint(const sJoint* joint, const double* data, float scale, sMaterial* material);
  void RenderBone(float x0, float y0, float z0, float x1, float y1, float z1, sMaterial* material);
};

#endif // _BVH_H_