#ifndef MESH_H
#define MESH_H

#include <array>
#include <iostream>
#include <vector>

#include "../../math/mat4.h"
#include "../../math/vec2.h"
#include "../../math/vec3.h"

#include "material.h"

struct Vertex
{
  Point3f pos{0.0};
  Vector3f norm{0.0};
  Vector2f tc{0.0};
  float weights[4] = {0.0, 0.0, 0.0, 0.0};
  int joints[4] = {-1, -1, -1, -1};
};

enum DrawMode
{
  POINTS,
  LINES,
  TRIANGLES
};

struct Mesh
{
  uint VAO{0};
  uint VBO{0};
  uint EBO{0};

  std::vector<Vertex> vertices;
  std::vector<uint> indices;
  DrawMode mode{POINTS};
  Material material{};

  void init();
  void render(class Shader &);
  void clean();

  // get bounding box
  struct BoundingBox getBoundingBox();
};

#endif
