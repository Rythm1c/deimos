#ifndef SKELETON_H
#define SKELETON_H

#include "../../math/mat4.h"
#include "pose.h"
#include <vector>
#include <string>

class Skeleton
{
public:
  Skeleton();
  ~Skeleton() {}

  Pose restPose;
  std::vector<Mat4x4> inversePose;
  std::vector<std::string> jointNames;

  // std::vector<Mat4x4> getFinalMat() const;
};

#endif