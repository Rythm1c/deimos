#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

/* #include "../renderer/mesh.h"
#include "obj.h"

OBJFile::OBJFile(std::string path)
{
  tinyobj::attrib_t attrib;
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;

  std::string warn, err;

  if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err,
                        path.c_str()))
    throw std::runtime_error(warn + err);

  Vertex vertex = {};
  Mesh tmpmesh = {};
  tmpmesh.mode = TRIANGLES;

  for (const auto &shape : shapes)
  {
    for (const auto &index : shape.mesh.indices)
    {

      vertex.pos = {attrib.vertices[3 * index.vertex_index + 0],
                    attrib.vertices[3 * index.vertex_index + 1],
                    attrib.vertices[3 * index.vertex_index + 2]};

      vertex.norm = {attrib.normals[3 * index.normal_index + 0],
                     attrib.normals[3 * index.normal_index + 1],
                     attrib.normals[3 * index.normal_index + 2]};

      tmpmesh.vertices.push_back(vertex);
    }
  }
  tmpmesh.init();
  meshes.push_back(tmpmesh);
}
 */