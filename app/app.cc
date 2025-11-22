#include "app.h"
#include "../model/model.h"
#include "../viewer/viewer.h"
#include "window.h"
#include "../external/imgui/imgui.h"
#include "../external/imgui/imgui_impl_sdl2.h"
#include "../external/imgui/imgui_impl_opengl3.h"

#include <chrono>

static auto lastFrameDuration = std::chrono::high_resolution_clock::now();

App::App()
    : viewer(nullptr), window(nullptr), running(true), delta(0.0), fps(0.0),
      elapsed(0.0), keyboardState(nullptr), io(nullptr),
      animSpeedEditMode(false), animSpeedFocusRequested(false), animSpeedEditValue(0.0f) {}

App::~App()
{
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();
}

void App::init()
{

  this->window = new Window();
  this->window->init();

  this->viewer = new Viewer();
  this->viewer->init();

  this->viewer->addModel("zombie", "models/zombie/scene.gltf");
  this->viewer->addModel("mira", "models/mira/scene.gltf");
  // this->viewer->addModel("alien", "models/alien/Alien.gltf");

  this->viewer->currModel = "mira";

  int nkeys;
  this->keyboardState = SDL_GetKeyboardState(&nkeys);

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  std::cout << "pass\n";
  //*io = ImGui::GetIO();

  ImGui::StyleColorsDark();

  ImGui_ImplSDL2_InitForOpenGL(this->window->win, this->window->context);
  ImGui_ImplOpenGL3_Init();
}

void App::run()
{
  this->init();
  while (this->running)
  {
    this->calcFps();
    this->handelInput();

    this->window->clear(0.7, 0.7, 0.7);
    this->viewer->update(this->window->ratio(), this->delta);
    this->viewer->renderCurrModel();
    this->renderGui();
    this->window->swapBuffer();
  }
}
void App::calcFps()
{
  auto now = std::chrono::high_resolution_clock::now();
  this->delta = std::chrono::duration<float, std::chrono::seconds::period>(now - lastFrameDuration).count();
  lastFrameDuration = now;
  this->fps = 1.0 / this->delta;
  this->elapsed += this->delta;
}

void App::renderGui()
{

  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplSDL2_NewFrame();
  ImGui::NewFrame();

  ImGui::Begin("info");
  ImGui::Text("FPS: %.1f", this->fps);

  ImGui::SeparatorText("Model Selection");

  // Create a combo box for model selection
  const char *current_item = this->viewer->currModel.c_str();
  if (ImGui::BeginCombo("##ModelSelector", current_item))
  {
    for (const auto &modelName : this->viewer->getModelNames())
    {
      bool is_selected = (this->viewer->currModel == modelName);
      if (ImGui::Selectable(modelName.c_str(), is_selected))
      {
        this->viewer->currModel = modelName;
      }
      if (is_selected)
      {
        ImGui::SetItemDefaultFocus();
      }
    }
    ImGui::EndCombo();
  }

  ImGui::Spacing();

  ImGui::SeparatorText("Animation Controls");
  if (this->viewer->getCurrModel()->animController)
  {
    // clip selector
    ImGui::Text("Clip");
    const char *currentClipName = this->viewer->getCurrModel()->animController->getCurrentAnimationName().c_str();
    if (ImGui::BeginCombo("##ClipSelector", currentClipName))
    {
      for (size_t i = 0; i < this->viewer->getCurrModel()->animController->clipCount(); i++)
      {
        std::string clipName = this->viewer->getCurrModel()->animController->getClip(i)->GetName();
        bool is_selected = (this->viewer->getCurrModel()->animController->getCurrentAnimationName() == clipName);
        if (ImGui::Selectable(clipName.c_str(), is_selected))
        {
          this->viewer->getCurrModel()->animController->setCurrentAnimation(i);
        }
        if (is_selected)
        {
          ImGui::SetItemDefaultFocus();
        }
      }
      ImGui::EndCombo();
    }

    ImGui::Text("speed");
    float speed = this->viewer->getCurrModel()->animController->getSpeed();

    // If not in typed-edit mode show slider. Double-clicking the slider will enable typed edit.
    if (!this->animSpeedEditMode)
    {
      if (ImGui::SliderFloat("##SpeedSlider", &speed, 0.0f, 3.0f))
      {
        this->viewer->getCurrModel()->animController->setSpeed(speed);
      }

      // Enter typed edit on double-click
      if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
      {
        this->animSpeedEditMode = true;
        this->animSpeedEditValue = speed;
        this->animSpeedFocusRequested = true;
      }
    }
    else
    {
      // Render a compact InputFloat in-place for precise typing
      ImGui::PushItemWidth(100);
      if (this->animSpeedFocusRequested)
      {
        ImGui::SetKeyboardFocusHere(-1); // focus the next widget (the InputFloat)
        this->animSpeedFocusRequested = false;
      }

      if (ImGui::InputFloat("##SpeedEdit", &this->animSpeedEditValue, 0.0f, 0.0f, "%.3f"))
      {
        if (this->animSpeedEditValue < 0.0f)
          this->animSpeedEditValue = 0.0f;
        this->viewer->getCurrModel()->animController->setSpeed(this->animSpeedEditValue);
      }

      // Exit edit mode when user finishes editing or the item is deactivated
      if (ImGui::IsItemDeactivatedAfterEdit() || ImGui::IsItemDeactivated())
      {
        this->animSpeedEditMode = false;
      }
      ImGui::PopItemWidth();
    }
    bool isPlaying = this->viewer->getCurrModel()->animController->isPlaying();
    if (isPlaying)
    {
      if (ImGui::Button("Pause"))
      {
        this->viewer->getCurrModel()->animController->pause();
      }
    }
    else
    {
      if (ImGui::Button("Play "))
      {
        this->viewer->getCurrModel()->animController->play();
      }
    }
    ImGui::SameLine();
    // stop button
    if (ImGui::Button("Stop "))
    {
      this->viewer->getCurrModel()->animController->stop();
    }
  }
  ImGui::End();

  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
void App::handelInput()
{
  SDL_Event event;
  while (SDL_PollEvent(&event))
  {
    ImGui_ImplSDL2_ProcessEvent(&event);

    // Handle general window events first
    if (event.type == SDL_QUIT)
    {
      this->running = false;
      continue;
    }

    if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
    {
      this->window->reSize();
      continue;
    }

    // Skip viewer input handling if ImGui is using the input
    if (ImGui::GetIO().WantCaptureMouse || ImGui::GetIO().WantCaptureKeyboard)
    {
      continue;
    }

    // Handle viewer input only if ImGui is not using it
    switch (event.type)
    {
    case SDL_MOUSEMOTION:
      if (event.button.button == SDL_BUTTON_LEFT)
      {
        this->viewer->camera->rotation(event.motion.xrel, event.motion.yrel);
      }
      break;

    default:
      break;
    }
  }

  // Handle keyboard input for viewer only if ImGui is not capturing keyboard
  if (!ImGui::GetIO().WantCaptureKeyboard)
  {
    if (this->keyboardState[SDL_SCANCODE_W])
    {
      this->viewer->camera->moveForwards(this->delta);
    }
    if (this->keyboardState[SDL_SCANCODE_S])
    {
      this->viewer->camera->moveBackwards(this->delta);
    }
    if (this->keyboardState[SDL_SCANCODE_D])
    {
      this->viewer->camera->moveRight(this->delta);
    }
    if (this->keyboardState[SDL_SCANCODE_A])
    {
      this->viewer->camera->moveLeft(this->delta);
    }
    // for zooming in and out
    if (this->keyboardState[SDL_SCANCODE_Z])
    {
      this->viewer->camera->fov++;
    }
    else if (this->keyboardState[SDL_SCANCODE_X])
    {
      this->viewer->camera->fov--;
    }
  }
}
