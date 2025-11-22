#include "mesh.h"
#include "shader.h"
#include "boundingVolumes.h"

#include "../../external/glad/glad.h"

void Mesh::init()
{

  glCreateVertexArrays(1, &VAO);

  glBindVertexArray(VAO);

  glCreateBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(),
               vertices.data(), GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
  glEnableVertexAttribArray(0);

  if (indices.size() != 0)
  {
    glCreateBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * indices.size(),
                 indices.data(), GL_STATIC_DRAW);
  }

  if (mode == TRIANGLES)
  {
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void *)offsetof(Vertex, norm));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void *)offsetof(Vertex, tc));
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void *)offsetof(Vertex, weights));
    glEnableVertexAttribArray(3);

    glVertexAttribIPointer(4, 4, GL_INT, sizeof(Vertex),
                           (void *)offsetof(Vertex, joints));
    glEnableVertexAttribArray(4);
  }

  glBindVertexArray(0);
}
void Mesh::render(Shader &shader)
{

  this->material.configShader(shader);

  switch (mode)
  {
  case POINTS:

    glBindVertexArray(VAO);
    glDrawArrays(GL_POINTS, 0, vertices.size());
    glBindVertexArray(0);
    break;
  case LINES:

    if (indices.size() != 0)
    {
      glBindVertexArray(VAO);
      glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, 0);
      glBindVertexArray(0);
    }
    else
    {
      glBindVertexArray(VAO);
      glDrawArrays(GL_LINES, 0, vertices.size());
      glBindVertexArray(0);
    }

    break;
  case TRIANGLES:

    if (indices.size() != 0)
    {
      glBindVertexArray(VAO);
      glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
      glBindVertexArray(0);
    }
    else
    {
      glBindVertexArray(VAO);
      glDrawArrays(GL_TRIANGLES, 0, vertices.size());
      glBindVertexArray(0);
    }
    break;
  default:
    break;
  }
}

BoundingBox Mesh::getBoundingBox()
{
  BoundingBox bbox;

  for (const auto &vertex : vertices)
  {
    bbox.update(vertex.pos);
  }

  return bbox;
}

void Mesh::clean()
{
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);
}
