#include <iostream>
#include <GL/glew.h>
#include <GL/gl.h>

#include "window.h"

Window::Window() : win(nullptr), context(nullptr), width(800), height(600) {}
void Window::swapBuffer() { SDL_GL_SwapWindow(this->win); }
float Window::ratio() { return (float)this->width / (float)this->height; }
Window::~Window()
{
  SDL_GL_DeleteContext(this->context);
  SDL_DestroyWindow(this->win);
  SDL_Quit();
}
void Window::reSize()
{
  SDL_GetWindowSize(this->win, &this->width, &this->height);
  glViewport(0, 0, this->width, this->height);
}
void Window::clear(float r, float g, float b)
{
  glClearColor(r, g, b, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Window::init()
{

  if (SDL_Init(SDL_INIT_VIDEO) != 0)
  {
    std::runtime_error("failed to create SDL window!");
  }
  // window creation
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

  int flags =
      SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI;
  this->win = SDL_CreateWindow("little-engine", 100, 100, this->width,
                               this->height, flags);
  this->context = SDL_GL_CreateContext(this->win);

  if (SDL_GL_MakeCurrent(this->win, this->context) != 0)
  {
    std::runtime_error("failed to set an opengl context with window!");
  }

  SDL_GL_SetSwapInterval(1);

  glewExperimental = true;
  glewInit();

  glViewport(0, 0, this->width, this->height);
  glEnable(GL_DEPTH_TEST);
  /* glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glFrontFace(GL_CCW); */
}
