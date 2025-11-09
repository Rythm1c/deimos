#include "debugRenderer.h"
#include <GL/glew.h>

DebugRenderer::DebugRenderer() : VAO(0), VBO(0), EBO(0), lineShader(nullptr) {}

DebugRenderer::~DebugRenderer()
{
  clean();
  if (lineShader)
  {
    delete lineShader;
  }
}

void DebugRenderer::init()
{
  // Create and compile the line shader
  lineShader = new Shader("shaders/line.vert", "shaders/line.frag");

  // Initialize the vertices and indices for a bounding box
  setupBBoxLines();

  // Create and setup OpenGL buffers
  glCreateVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  glCreateBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vector3f) * lineVertices.size(),
               lineVertices.data(), GL_STATIC_DRAW);

  glCreateBuffers(1, &EBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * lineIndices.size(),
               lineIndices.data(), GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vector3f), (void *)0);

  glBindVertexArray(0);
}

void DebugRenderer::setupBBoxLines()
{
  // Setup vertices for unit cube (will be transformed to actual bbox size)
  this->lineVertices = {
      Vector3f(0, 0, 0), Vector3f(1, 0, 0), Vector3f(1, 1, 0), Vector3f(0, 1, 0),
      Vector3f(0, 0, 1), Vector3f(1, 0, 1), Vector3f(1, 1, 1), Vector3f(0, 1, 1)};

  // Indices for 12 lines forming the bbox
  this->lineIndices = {
      0, 1, 1, 2, 2, 3, 3, 0, // Bottom face
      4, 5, 5, 6, 6, 7, 7, 4, // Top face
      0, 4, 1, 5, 2, 6, 3, 7  // Vertical edges
  };
}

void DebugRenderer::renderBoundingBox(const BoundingBox &bbox, const Mat4x4 &transform,
                                      const Mat4x4 &view, const Mat4x4 &projection)
{
  if (!lineShader)
    return;

  lineShader->use();
  lineShader->updateMat4("projection", projection);
  lineShader->updateMat4("view", view);

  // Create transform that scales and positions the unit cube to match the bbox
  Vector3f size = bbox.maxPt - bbox.minPt;
  Vector3f position = bbox.minPt;

  Mat4x4 bboxTransform = transform * translate(position) * scale(size);
  lineShader->updateMat4("transform", bboxTransform);

  // Draw the bounding box lines
  glBindVertexArray(VAO);
  glDrawElements(GL_LINES, lineIndices.size(), GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}

void DebugRenderer::clean()
{
  if (VAO)
    glDeleteVertexArrays(1, &VAO);
  if (VBO)
    glDeleteBuffers(1, &VBO);
  if (EBO)
    glDeleteBuffers(1, &EBO);
  VAO = VBO = EBO = 0;
}