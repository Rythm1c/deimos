#include "controller.h"
#include "clip.h"
#include "pose.h"
#include "skeleton.h"
#include "transformTrack.h"
#include <iostream>

void Controller::addClip(Clip *clip)
{
  this->clips.push_back(clip);
}
void Controller::removeClip(size_t index)
{
  if (index < this->clips.size())
  {
    this->clips.erase(this->clips.begin() + index);
  }
  else
  {
    std::cout << "Clip with index (" << index << ") does not exist" << "\n";
  }
}

void Controller::setSkeleton(Skeleton *skeleton)
{
  this->skeleton = skeleton;
  if (this->outPose == nullptr)
  {
    this->outPose = new Pose(skeleton->restPose.size());
  }
}

Clip *Controller::getClip(size_t index) const
{
  if (index < this->clips.size())
  {
    return this->clips[index];
  }
  else
  {
    std::cout << "Clip with index (" << index << ") does not exist" << "\n";
    return nullptr;
  }
}

void Controller::update(float deltaTime)
{

  if (this->state == PLAYING)
  {
    if (this->skeleton == nullptr)
    {
      std::cout << "Skeleton not set for controller\n";
      return;
    }

    *this->outPose = this->skeleton->restPose;
    Clip *clip = this->getClip(this->currentClip);

    if (clip == nullptr)
    {
      std::cout << "Clip " << clip->GetName() << " not found in controller\n";
      return;
    }

    clip->sample(*outPose, this->elapsed);

    this->elapsed += deltaTime * this->speed;
  }
}

void Controller::pause()
{
  if (this->state == PLAYING)
  {
    this->state = PAUSED;
  }
}

void Controller::play()
{
  if (this->state == STOPPED || this->state == PAUSED)
  {
    this->state = PLAYING;
  }
}
void Controller::stop()
{
  this->state = STOPPED;
  this->elapsed = 0.0f;
}

void Controller::resume()
{
  if (this->state == PAUSED)
  {
    this->state = PLAYING;
  }
}

void Controller::setCurrentAnimation(size_t index)
{
  if (index < this->clips.size())
  {
    this->currentClip = index;
  }
  else
  {
    std::cout << "Clip with index (" << index << ") does not exist" << "\n";
  }
  this->reset();
}

void Controller::reset()
{
  this->elapsed = 0.0f;
}

std::vector<Mat4x4> Controller::getPose()
{
  std::vector<Mat4x4> result;

  if ((this->outPose != nullptr) && (this->skeleton != nullptr))
  {
    int len = this->outPose->size();
    result.resize(len, identity());
    for (int i = 0; i < len; ++i)
    {
      Transform world = this->outPose->getGlobalTransform(i);
      Mat4x4 invPose = this->skeleton->inversePose[i];

      result[i] = world.get() * invPose;
    }
  }

  return result;
}
bool Controller::isPlaying() const
{
  return this->state == PLAYING;
}
float Controller::getSpeed() const
{
  return this->speed;
}
void Controller::setSpeed(float inSpeed)
{
  if (inSpeed < 0.0f)
  {
    std::cout << "Speed cannot be negative. Setting speed to 0.0f\n";
    this->speed = 0.0f;
    return;
  }

  if (inSpeed > 3.0f)
  {
    std::cout << "Warning: Setting speed to a high value may cause "
                 "undesirable effects in animation playback.\n";
    this->speed = 3.0f;
    return;
  }
  this->speed = inSpeed;
}
std::string Controller::getCurrentAnimationName() const
{
  if (this->currentClip < this->clips.size())
  {
    return this->clips[this->currentClip]->GetName();
  }
  else
  {
    return "No Clip";
  }
}
size_t Controller::clipCount() const
{
  return this->clips.size();
}

void Controller::clean()
{
  for (auto clip : this->clips)
  {
    delete clip;
  }
  this->clips.clear();

  if (this->outPose != nullptr)
  {
    delete this->outPose;
    this->outPose = nullptr;
  }
}