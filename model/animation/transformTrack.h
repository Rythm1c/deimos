#ifndef TRANSFORMTRACK_H
#define TRANSFORMTRACK_H

#include "track.h"

class Transform;
class TransformTrack
{
public:
  TransformTrack();
  ~TransformTrack() {}

  size_t getId();
  void setId(size_t id);
  VectorTrack &getPosTrack();
  QuatTrack &getRotationTrack();
  VectorTrack &getScalingTrack();

  float getStartTime();
  float getEndTime();
  bool isValid();

  Transform sample(const Transform &ref, float time, bool looping);

private:
  VectorTrack position;
  QuatTrack rotation;
  VectorTrack scaling;
  // joint id
  size_t id;
};

#endif
