#include "../../external/glad/glad.h"

#include <fstream>
#include <sstream>
#include <string>

#include "shader.h"

Shader::Shader(const char *vert_path, const char *frag_path) : program(0)
{
  load(vert_path, frag_path);
}

void Shader::use() { glUseProgram(program); }
void Shader::clean() { glDeleteProgram(program); }

void Shader::updateMat4(const char *name, const Mat4x4 &mat)
{
  unsigned int location = glGetUniformLocation(program, name);
  glUniformMatrix4fv(location, 1, true, &mat.rc[0][0]);
}
void Shader::updateVec3(const char *name, const Vector3f &vec)
{
  unsigned int location = glGetUniformLocation(program, name);
  glUniform3f(location, vec.x, vec.y, vec.z);
}
void Shader::updateFloat(const char *name, float value)
{
  unsigned int location = glGetUniformLocation(program, name);
  glUniform1f(location, value);
}
void Shader::updateInt(const char *name, int value)
{
  unsigned int location = glGetUniformLocation(program, name);
  glUniform1i(location, value);
}
void Shader::load(const char *vert_path, const char *frag_path)
{
  std::string vertexcode;
  std::string fragmentcode;
  std::ifstream vShaderFile;
  std::ifstream fShaderFile;

  vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

  try
  {
    vShaderFile.open(vert_path);
    fShaderFile.open(frag_path);

    std::stringstream vshaderstream, fshaderstream;

    vshaderstream << vShaderFile.rdbuf();
    fshaderstream << fShaderFile.rdbuf();

    vShaderFile.close();
    fShaderFile.close();

    vertexcode = vshaderstream.str();
    fragmentcode = fshaderstream.str();
  }
  catch (std::ifstream::failure &e)
  {
    std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ\n";
  }
  const GLchar *vshadercode = vertexcode.c_str();
  const char *fshadercode = fragmentcode.c_str();

  unsigned int vertex, fragment;
  vertex = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex, 1, &vshadercode, NULL);
  glCompileShader(vertex);

  fragment = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment, 1, &fshadercode, NULL);
  glCompileShader(fragment);

  program = glCreateProgram();
  glAttachShader(program, vertex);
  glAttachShader(program, fragment);
  glLinkProgram(program);

  glDeleteShader(vertex);
  glDeleteShader(fragment);
}
