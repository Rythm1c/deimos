#ifndef VIEWER_H
#define VIEWER_H

#include "../math/math.h"
#include "camera.h"
#include "../model/renderer/debugRenderer.h"
#include <map>
#include <string>
#include <vector>

class Shader;

struct Light
{
  Color3f color;
  Point3f position;
};

class Viewer
{
public:
  Viewer();
  ~Viewer();

  void init();

  void addModel(std::string name, std::string path);

  void update(float ratio, float elapsed);
  void renderCurrModel();

  class Model *getCurrModel();

  std::vector<std::string> getModelNames();

  Camera *camera;

  std::string currModel;

  Vector3f lightDir;

  std::vector<Light> lights;
  bool showBoundingBoxes{true}; // Toggle for bounding box visualization

private:
  Shader *phongStatic;
  Shader *phongAnimated;

  Shader *pbrStatic;
  Shader *pbrAnimated;

  DebugRenderer debugRenderer;
  std::map<std::string, class Model *> models;
};

#endif
