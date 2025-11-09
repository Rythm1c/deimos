#ifndef MATERIAL_H
#define MATERIAL_H

#include "../../math/vec3.h"

class Texture;

struct Material
{
  float ao{0.01};
  float roughness{0.5};
  float metallicness{0.5};
  Color3f baseCol{1.0};
  int baseTex{-1};
  int metallicMap{-1};

  void configShader(class Shader &);
};

#endif