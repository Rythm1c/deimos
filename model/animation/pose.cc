#include "pose.h"
#include "../../math/mat4.h"
#include "../../math/transform.h"
#include <cstring>

Pose::Pose(size_t nJoints) { this->resize(nJoints); }

Pose::Pose(const Pose &p) { *this = p; }

Pose &Pose::operator=(const Pose &p)
{
  if (this == &p)
  {
    return *this;
  }

  if (this->parents.size() != p.parents.size())
  {
    this->parents.resize(p.parents.size());
  }
  if (this->joints.size() != p.joints.size())
  {
    this->joints.resize(p.joints.size());
  }

  if (this->parents.size() != 0)
  {
    memcpy(&this->parents[0], &p.parents[0],
           sizeof(int) * this->parents.size());
  }

  if (this->joints.size() != 0)
  {
    memcpy(&this->joints[0], &p.joints[0],
           sizeof(Transform) * this->joints.size());
  }

  return *this;
}

void Pose::resize(size_t newSize)
{
  this->joints.resize(newSize, Transform());
  this->parents.resize(newSize, -1);
}

unsigned int Pose::size() { return (uint)this->joints.size(); }

Transform Pose::getLocalTransform(size_t index) { return this->joints[index]; }
void Pose::setLocalTransform(size_t index, const Transform &transform)
{
  this->joints[index] = transform;
}

Transform Pose::getGlobalTransform(size_t index)
{
  Transform result = this->joints[index];
  int p = this->parents[index];

  while (p != -1)
  {
    result = combine(this->joints[p], result);

    p = this->parents[p];
  }

  return result;
}

void Pose::getMatrixPalette(std::vector<Mat4x4> &out)
{
  unsigned int size = this->size();
  if (out.size() != size)
  {
    out.resize(size);
  }
  for (unsigned int i = 0; i < size; ++i)
  {
    Transform t = this->getGlobalTransform(i);
    out[i] = t.get();
  }
}

int Pose::getParent(size_t index) { return this->parents[index]; }
void Pose::setParent(size_t index, int parent)
{
  this->parents[index] = parent;
}

bool Pose::operator==(const Pose &other)
{
  if (this->joints.size() != other.joints.size())
  {
    return false;
  }
  if (this->parents.size() != other.parents.size())
  {
    return false;
  }
  unsigned int size = (unsigned int)this->joints.size();
  for (unsigned int i = 0; i < size; ++i)
  {
    Transform thisLocal = this->joints[i];
    Transform otherLocal = other.joints[i];
    int thisParent = this->parents[i];
    int otherParent = other.parents[i];
    if (thisParent != otherParent)
    {
      return false;
    }
    if (thisLocal.translation != otherLocal.translation)
    {
      return false;
    }
    if (thisLocal.orientation != otherLocal.orientation)
    {
      return false;
    }
    if (thisLocal.scaling != otherLocal.scaling)
    {
      return false;
    }
  }
  return true;
}
bool Pose::operator!=(const Pose &other) { return !(*this == other); }
