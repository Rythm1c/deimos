#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include "../../external/glad/glad.h"

#include <iostream>

class Texture
{
public:
  Texture() : width(0), height(0), id(0) {}

  /// @brief loads a texture from the specified path(URL)
  /// @param path source/URL
  /// @param mipmaps enable mipmaps
  /// @param format RGB, RGBA, etc...
  /// @param type ignore
  Texture(const char *path, bool mipmaps = false, uint format = GL_RGBA, uint type = GL_UNSIGNED_BYTE);

  /// @brief load texture from raw data
  /// @param w width
  /// @param h height
  /// @param mipmaps enable mipmaps
  /// @param format RGB, RGBA, etc...
  /// @param type ignore
  /// @param data pointer to the textures raw data
  Texture(int w, int h, void *data, bool mipmaps = false, uint format = GL_RGBA, uint type = GL_UNSIGNED_BYTE, int filter = GL_LINEAR);

  ~Texture() {}

  int width, height;

  unsigned int id;

  // load a texture from specified path
  void load_from_file(const char *path, bool mipmaps = false, uint format = GL_RGBA, uint type = GL_UNSIGNED_BYTE);
  // load an empty texture for later use eg post proccesssing effects with
  // framebuffers
  void load_empty_texture(int w, int h, GLenum target, bool mipmaps = false, uint format = GL_RGBA, uint type = GL_UNSIGNED_BYTE, int filter = GL_LINEAR);
  // create bit map
  void create_char_bitmap(int w, int h, void *data);

  void clean() { glDeleteTextures(1, &id); }

private:
  // create actual texture
  void create(
      bool _mipmaps,
      unsigned int _format,
      unsigned int _type,
      int _filter,
      GLenum _target,
      void *_data);
};

#endif
