#include "texture.h"
#include "stb_image.h"

Texture::Texture(const char *path, bool mipmaps, uint format, uint type)
{
  this->load_from_file(path, mipmaps, format, type);
}

Texture::Texture(int w, int h, void *data, bool mipmaps, uint format, uint type, int filter)
{
  this->width = w;
  this->height = h;
  this->create(mipmaps, format, type, filter, GL_TEXTURE_2D, data);
}

void Texture::load_from_file(
    const char *path, bool mipmaps, uint format,
    uint type)
{
  int n_channels = 3;
  unsigned char *data = stbi_load(path, &this->width, &this->height, &n_channels, 0);
  if (data == nullptr)
  {
    std::cout << "failed to load texture" << std::endl;
  }

  if (n_channels != 4)
  {
    format = GL_RGB;
  }

  this->create(mipmaps, format, type, GL_LINEAR, GL_TEXTURE_2D, data);

  stbi_image_free(data);
}
void Texture::load_empty_texture(
    int w, int h, GLenum target, bool mipmaps,
    uint format, uint type, int filter)
{
  this->width = w;
  this->height = h;

  this->create(mipmaps, format, type, filter, GL_TEXTURE_2D, nullptr);
}

void Texture::create_char_bitmap(int w, int h, void *data)
{
  // character rendering settings
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  glGenTextures(1, &this->id);
  glBindTexture(GL_TEXTURE_2D, this->id);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, w, h, 0, GL_RED, GL_UNSIGNED_BYTE,
               data);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  glBindTexture(GL_TEXTURE_2D, 0);
}
void Texture::create(
    bool mipmaps,
    unsigned int format,
    unsigned int type,
    int filter,
    GLenum target,
    void *data)
{
  glGenTextures(1, &this->id);
  glBindTexture(target, this->id);

  glTexImage2D(target, 0, format, this->width, this->height, 0, format, type, data);

  if (mipmaps)
  {
    glGenerateMipmap(target);
    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  }
  else
  {
    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, filter);
  }
  glTexParameteri(target, GL_TEXTURE_MAG_FILTER, filter);
  glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  glBindTexture(target, 0);
}
