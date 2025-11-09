#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "mat4.h"
#include "quaternion.h"
#include "vec3.h"

class Transform
{
public:
  Transform()
      : translation(Vector3f()),
        orientation(Quat()),
        scaling(Vector3f(1.0)) {};
  ~Transform() {};

  Vector3f translation;
  Quat orientation;
  Vector3f scaling;

  /// @brief combines the translation, rotation and scaling members to produce a
  /// transformation matrix
  /// @return finall transform matrix
  Mat4x4 get() const;
  Transform inverse() const;
};

Transform combine(const Transform &t1, const Transform &t2);
Transform transformFromMat(const Mat4x4 &mat);

#endif
