#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <map>
#include <string>
#include <vector>
#include "../../math/mat4.h"
// animation controller class
// This class is responsible for managing the animation state and transitions

enum AnimationState
{
  PLAYING,
  PAUSED,
  STOPPED,
  RESUMED
};

class Controller
{

private:
  // time since last state change
  float elapsed;

  float speed;

  size_t currentClip;

  class Skeleton *skeleton;

  AnimationState state;

  class Pose *outPose;

  std::vector<class Clip *> clips;

public:
  Controller()
      : elapsed(0.0f),
        speed(1.0f),
        currentClip(0),
        skeleton(nullptr),
        state(STOPPED),
        outPose(nullptr) {}

  // function definations
  void setCurrentAnimation(size_t index);
  void setSkeleton(class Skeleton *skeleton);
  void addClip(class Clip *clip);
  void removeClip(size_t index);
  size_t clipCount() const;
  std::string getCurrentAnimationName() const;
  class Clip *getClip(size_t index) const;

  void update(float deltaTime);
  void pause();
  void play();
  void stop();
  void resume();

  bool isPlaying() const;
  float getSpeed() const;
  void setSpeed(float inSpeed);

  std::vector<Mat4x4> getPose();

  void reset();

  void clean();
};

#endif