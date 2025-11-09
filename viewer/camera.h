#ifndef CAMERA
#define CAMERA

#include "../math/math.h"
#include <iostream>

class Camera
{

public:
  Camera();
  ~Camera() {}

  float fov;
  Vector3f up;
  Vector3f pos;
  Vector3f front;
  float velocity;
  float sensitivity;

  Mat4x4 view();
  Mat4x4 projection(float ratio);
  void rotation(int x, int y);

  void moveForwards(float);
  void moveBackwards(float);
  void moveLeft(float);
  void moveRight(float);

private:
  float pitch;
  float yaw;
};

#endif
