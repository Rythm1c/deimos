#include "clip.h"
#include "../../math/transform.h"
#include "pose.h"
#include "transformTrack.h"

Clip::Clip() : name("none"), startTime(0.0), endTime(0.0), looping(true) {}

float Clip::sample(Pose &outPose, float inTime)
{
  if (this->GetDuration() == 0.0)
  {
    return 0.0;
  }

  float time = inTime;
  time = this->adjustTimeToFitRange(time);

  uint size = this->tracks.size();
  for (uint i = 0; i < size; ++i)
  {
    uint j = (uint)this->tracks[i].getId();
    Transform local = outPose.getLocalTransform((size_t)j);
    Transform animated = this->tracks[i].sample(local, time, this->looping);
    outPose.setLocalTransform((size_t)j, animated);
  }
  return time;
}

float Clip::adjustTimeToFitRange(float time)
{
  if (this->looping)
  {
    float duration = this->GetDuration();
    if (duration <= 0.0)
    {
      return 0.0;
    }
    time = fmodf(time - this->startTime, duration);

    if (time <= 0.0)
    {
      time += duration;
    }
    time += this->startTime;
  }
  else
  {
    if (time < this->startTime)
    {
      time = this->startTime;
    }
    if (time > this->endTime)
    {
      time = this->endTime;
    }
  }
  return time;
}

void Clip::ReCalculateDuartion()
{
  this->startTime = 0.0;
  this->endTime = 0.0;

  bool startSet = false;
  bool endSet = false;
  uint trackSize = this->tracks.size();
  for (uint i = 0; i < trackSize; ++i)
  {
    float mStartTime = this->tracks[i].getStartTime();
    float mEndTime = this->tracks[i].getEndTime();

    if (startTime < this->startTime || !startSet)
    {
      this->startTime = mStartTime;
      startSet = true;
    }

    if (endTime > this->endTime || !endSet)
    {
      this->endTime = mEndTime;
      endSet = true;
    }
  }
}

TransformTrack &Clip::getTrack(size_t index)
{
  return this->tracks[index];
}

std::vector<TransformTrack> &Clip::getTracks()
{
  return this->tracks;
}

std::string &Clip::GetName() { return this->name; }
uint Clip::getIdAtIndex(uint index)
{
  return (uint)this->tracks[index].getId();
}

void Clip::resize(size_t newSize)
{
  this->tracks.resize(newSize, TransformTrack());
}

uint Clip::size() { return (uint)this->tracks.size(); }
float Clip::GetDuration() { return this->endTime - this->startTime; }
float Clip::GetStartTime() { return this->startTime; }
float Clip::GetEndTime() { return this->endTime; }
bool Clip::GetLooping() { return this->looping; }

void Clip::SetName(const std::string &inNewName) { this->name = inNewName; }
void Clip::setIdAtIndex(uint index, uint id)
{
  return this->tracks[index].setId((size_t)id);
}
void Clip::SetLooping(bool inLooping) { this->looping = inLooping; }
