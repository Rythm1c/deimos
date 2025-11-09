#ifndef BOUNDINGVOLUMES_H
#define BOUNDINGVOLUMES_H
#include "../../math/vec3.h"
#include <array>

struct BoundingBox
{
  Point3f minPt;
  Point3f maxPt;

  BoundingBox()
      : minPt(1e30f), maxPt(-1e30f) {}

  void update(const Point3f &pt)
  {
    minPt.x = std::min(minPt.x, pt.x);
    minPt.y = std::min(minPt.y, pt.y);
    minPt.z = std::min(minPt.z, pt.z);

    maxPt.x = std::max(maxPt.x, pt.x);
    maxPt.y = std::max(maxPt.y, pt.y);
    maxPt.z = std::max(maxPt.z, pt.z);
  }
};
#endif