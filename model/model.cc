#include "model.h"

#include <GL/glew.h>

#include <GL/gl.h>
#include <SDL2/SDL_opengl.h>

Model::Model()
    : animController(nullptr),
      transform(new Transform()),
      factor(Vector3f(1.0)) {}

void Model::translate(Vector3f pos) { this->transform->translation = pos; }

void Model::scale(Vector3f size) { this->transform->scaling = size * factor; }

void Model::orient(Quat orientation)
{
  this->transform->orientation = orientation;
}

Mat4x4 Model::get_transform() { return this->transform->get(); }

void Model::normalize()
{
  // compute bounding box
  BoundingBox box = BoundingBox();
  for (auto &mesh : meshes)
  {
    BoundingBox meshVolume = mesh.getBoundingBox();
    box.update(meshVolume.minPt);
    box.update(meshVolume.maxPt);
  }
  Vector3f size = box.maxPt - box.minPt;
  float maxSide = std::max(size.x, std::max(size.y, size.z));
  factor = Vector3f(2.0f / maxSide);
}

void Model::render(Shader &shader)
{
  for (auto &mesh : meshes)
  {
    int bIdx = mesh.material.baseTex;
    if (bIdx != -1)
    {
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, this->textures[bIdx].id);
    }

    int mIdx = mesh.material.metallicMap;
    if (mIdx != -1)
    {
      glActiveTexture(GL_TEXTURE1);
      glBindTexture(GL_TEXTURE_2D, this->textures[mIdx].id);
    }
    mesh.render(shader);
  }
}


void Model::clean()
{
  delete transform;

  if (this->animController != nullptr)
  {
    this->animController->clean();
    delete this->animController;
  }

  for (auto &mesh : meshes)
  {
    mesh.clean();
  }
}