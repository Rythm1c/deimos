#ifndef OBJ_H
#define OBJ_H

#include <iostream>
#include <vector>

struct Mesh;

class OBJFile {
public:
  OBJFile(std::string path);
  ~OBJFile() {}

  std::vector<Mesh> meshes;
};

#endif
