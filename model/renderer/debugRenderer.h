#ifndef DEBUG_RENDERER_H
#define DEBUG_RENDERER_H

#include "boundingVolumes.h"
#include "shader.h"
#include "mesh.h"
#include <vector>

class DebugRenderer {
public:
    DebugRenderer();
    ~DebugRenderer();

    void init();
    void renderBoundingBox(const BoundingBox& bbox, const Mat4x4& transform, 
                          const Mat4x4& view, const Mat4x4& projection);
    void clean();

private:
    uint VAO, VBO, EBO;
    Shader* lineShader;
    std::vector<Vector3f> lineVertices;
    std::vector<uint> lineIndices;

    void setupBBoxLines();
};

#endif