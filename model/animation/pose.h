#ifndef POSE_H
#define POSE_H

#include <vector>
#include "../../math/transform.h"

class Pose
{
public:
  Pose() {};
  Pose(size_t nJoints);
  Pose(const Pose &pose);
  ~Pose() {}

  Pose &operator=(const Pose &p);
  bool operator==(const Pose &p);
  bool operator!=(const Pose &p);

  void resize(size_t newSize);
  unsigned int size();

  int getParent(size_t index);
  void setParent(size_t index, int parent);

  Transform getLocalTransform(size_t index);
  void setLocalTransform(size_t index, const Transform &transform);
  Transform getGlobalTransform(size_t index);

  void getMatrixPalette(std::vector<struct Mat4x4> &out);

private:
  std::vector<Transform> joints;
  std::vector<int> parents;
};

#endif
