#ifndef MODEL_H
#define MODEL_H

#include "../math/mat4.h"
#include "../math/quaternion.h"
#include "../math/vec3.h"

#include "animation/animation.h"
#include "foreign/gltf.h"
#include "renderer/renderer.h"

#include <vector>

enum ModelType
{
  ModelGLTF,
  ModelOBJ,
};

class Model
{
public:
  Model();
  ~Model() {}

  void orient(Quat);
  void scale(Vector3f);
  void translate(Vector3f);

  void normalize();

  void render(Shader &);
  void clean();

  Mat4x4 get_transform();

  std::vector<Mesh> meshes;
  std::vector<Texture> textures;
  Controller *animController;

private:
  class Transform *transform;
  Vector3f factor;
};

#endif
