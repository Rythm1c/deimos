#include "track.h"
#include <cstring>

template class Track<float, 1>;
template class Track<Vector3f, 3>;
template class Track<Quat, 4>;

namespace TrackHelpers
{
  inline float interpolate(float a, float b, float c)
  {
    return (1.0 - c) * a + c * b;
  }
  inline Vector3f interpolate(const Vector3f &a, const Vector3f &b, float c)
  {
    return lerp(a, b, c);
  }
  inline Quat interpolate(Quat &a, Quat &b, float c)
  {
    Quat result = mix(a, b, c);

    if (dot(a, b) < 0.0)
    {
      result = mix(a, -1.0 * b, c);
    }

    return result.unit();
  }

  inline float AdjustHermiteResult(float f) { return f; }
  inline Vector3f AdjustHermiteResult(const Vector3f &v) { return v; }
  inline Quat AdjustHermiteResult(Quat &q) { return q.unit(); }

  inline void Neighborhood(const float &a, float &b) {}
  inline void Neighborhood(const Vector3f &a, Vector3f &b) {}
  inline void Neighborhood(const Quat &a, Quat &b)
  {
    if (dot(a, b) < 0)
    {
      b = -1.0 * b;
    }
  }
}; // namespace TrackHelpers

template <typename T, size_t N>
unsigned int Track<T, N>::size()
{
  return frames.size();
}

template <typename T, size_t N>
float Track<T, N>::getStartTime()
{
  return frames[0].time;
}

template <typename T, size_t N>
float Track<T, N>::getEndTime()
{
  return frames[frames.size() - 1].time;
}

template <typename T, size_t N>
T Track<T, N>::sample(float time, bool looping)
{
  if (interpolation == Interpolation::Constant)
  {
    return sampleConst(time, looping);
  }
  else if (interpolation == Interpolation::Linear)
  {
    return sampleLinear(time, looping);
  }
  else
  {
    return sampleCubic(time, looping);
  }
}

template <typename T, size_t N>
T Track<T, N>::hermite(float t, const T &p1, const T &s1, const T &_p2,
                       const T &s2)
{
  float tt = t * t;

  float ttt = tt * t;
  T p2 = _p2;
  TrackHelpers::Neighborhood(p1, p2);
  float h1 = 2.0f * ttt - 3.0f * tt + 1.0f;
  float h2 = -2.0f * ttt + 3.0f * tt;
  float h3 = ttt - 2.0f * tt + t;
  float h4 = ttt - tt;
  T result = p1 * h1 + p2 * h2 + s1 * h3 + s2 * h4;
  return TrackHelpers::AdjustHermiteResult(result);
}

template <typename T, size_t N>
size_t Track<T, N>::frameIndex(float time, bool looping)
{
  if (this->frames.size() < 1)
  {
    return -1;
  }

  if (looping)
  {
    float startTime = this->getStartTime();
    float endTime = this->getEndTime();
    float difference = endTime - startTime;
    time = fmodf((time - startTime), difference);
    if (time < 0.0)
    {
      time += difference;
    }
    time += startTime;
  }
  else
  {
    if (time <= this->getStartTime())
    {
      return 0;
    }
    if (time >= this->frames[this->frames.size() - 2].time)
    {
      return this->frames.size() - 2;
    }
  }

  for (size_t i = this->frames.size() - 1; i >= 0; --i)
  {
    if (time >= this->frames[i].time)
    {
      return i;
    }
  }

  return -1;
}

template <typename T, size_t N>
float Track<T, N>::adjustToFitTrack(float time, bool looping)
{

  size_t size = this->frames.size();

  if (size < 1)
  {
    return 0.0;
  }

  float startTime = this->getStartTime();
  float endTime = this->getEndTime();
  float difference = endTime - startTime;

  if (difference <= 0.0)
  {
    return 0.0;
  }

  if (looping)
  {
    time = fmodf(time - startTime, difference);
    if (time < 0.0)
    {
      time += difference;
    }
    time += startTime;
  }
  else
  {
    if (time <= startTime)
    {
      time = startTime;
    }
    if (time >= endTime)
    {
      return endTime;
    }
  }

  return time;
}

template <>
float Track<float, 1>::cast(float *value) { return value[0]; }
template <>
Vector3f Track<Vector3f, 3>::cast(float *value)
{
  return Vector3f(value[0], value[1], value[2]);
}
template <>
Quat Track<Quat, 4>::cast(float *value)
{
  Quat r = Quat(value[0], value[1], value[2], value[3]);
  return r.unit();
}

template <typename T, size_t N>
T Track<T, N>::sampleConst(float time, bool looping)
{
  size_t index = this->frameIndex(time, looping);

  if ((int)index < 0 || (int)index >= (int)this->frames.size())
  {
    return T();
  }

  return this->cast(&this->frames[index].m_value[0]);
}

template <typename T, size_t N>
T Track<T, N>::sampleLinear(float time, bool looping)
{
  size_t index = this->frameIndex(time, looping);
  if ((int)index < 0 || (int)index >= (int)this->frames.size() - 1)
  {
    return T();
  }

  int nextFrame = index + 1;
  float trackTime = this->adjustToFitTrack(time, looping);
  float thisTime = this->frames[index].time;
  float frameDelta = this->frames[nextFrame].time - thisTime;

  if (frameDelta <= 0.0)
  {
    return T();
  }

  float t = (trackTime - thisTime) / frameDelta;
  T start = cast(&this->frames[index].m_value[0]);
  T end = cast(&this->frames[nextFrame].m_value[0]);

  return TrackHelpers::interpolate(start, end, t);
}
template <typename T, size_t N>
T Track<T, N>::sampleCubic(float time, bool looping)
{
  int thisFrame = this->frameIndex(time, looping);
  if (thisFrame < 0 || thisFrame >= this->frames.size() - 1)
  {
    return T();
  }

  int nextFrame = thisFrame + 1;
  float trackTime = this->adjustToFitTrack(time, looping);
  float thisTime = this->frames[thisFrame].time;
  float frameDelta = this->frames[nextFrame].time - thisTime;
  if (frameDelta <= 0.0f)
  {
    return T();
  }

  float t = (trackTime - thisTime) / frameDelta;
  size_t fltSize = sizeof(float);
  T point1 = cast(&this->frames[thisFrame].m_value[0]);
  T slope1; // = mFrames[thisFrame].mOut * frameDelta;
  memcpy(&slope1, this->frames[thisFrame].m_out, N * fltSize);
  slope1 = slope1 * frameDelta;
  T point2 = cast(&this->frames[nextFrame].m_value[0]);
  T slope2; // = mFrames[nextFrame].mIn[0] * frameDelta;
  memcpy(&slope2, this->frames[nextFrame].m_in, N * fltSize);
  slope2 = slope2 * frameDelta;

  return hermite(t, point1, slope1, point2, slope2);
}
