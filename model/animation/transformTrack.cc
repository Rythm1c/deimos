#include "transformTrack.h"
#include "../../math/transform.h"

TransformTrack::TransformTrack()
    : position(VectorTrack()), rotation(QuatTrack()), scaling(VectorTrack()),
      id(0) {}

size_t TransformTrack::getId() { return this->id; }

void TransformTrack::setId(size_t id) { this->id = id; }

VectorTrack &TransformTrack::getPosTrack() { return this->position; }
QuatTrack &TransformTrack::getRotationTrack() { return this->rotation; }
VectorTrack &TransformTrack::getScalingTrack() { return this->scaling; }

bool TransformTrack::isValid() {
  return this->position.size() > 1 || this->rotation.size() > 1 ||
         this->scaling.size() > 1;
}

float TransformTrack::getStartTime() {
  float result = 0.0;
  bool isSet = false;

  if (this->position.size() > 1) {
    result = this->position.getStartTime();
    isSet = true;
  }

  if (this->rotation.size() > 1) {
    float rotationStart = this->rotation.getStartTime();
    if (rotationStart < result || !isSet) {
      result = rotationStart;
      isSet = true;
    }
  }

  if (this->scaling.size() > 1) {
    float scalingStart = this->scaling.getStartTime();
    if (scalingStart < result || !isSet) {
      result = scalingStart;
    }
  }

  return result;
}

float TransformTrack::getEndTime() {
  float result = 0.0;
  bool isSet = false;

  if (this->position.size() > 1) {
    result = this->position.getEndTime();
    isSet = true;
  }

  if (this->rotation.size() > 1) {
    float rotationEnd = this->rotation.getEndTime();
    if (rotationEnd > result || !isSet) {
      result = rotationEnd;
      isSet = true;
    }
  }

  if (this->scaling.size() > 1) {
    float scalingEnd = this->scaling.getEndTime();
    if (scalingEnd > result || !isSet) {
      result = scalingEnd;
    }
  }

  return result;
}

Transform TransformTrack::sample(const Transform &ref, float time,
                                 bool looping) {
  Transform result = ref;

  if (this->position.size() > 1) {
    result.translation = this->position.sample(time, looping);
  }
  if (this->rotation.size() > 1) {
    result.orientation = this->rotation.sample(time, looping);
  }
  if (this->scaling.size() > 1) {
    result.scaling = this->scaling.sample(time, looping);
  }

  return result;
}
