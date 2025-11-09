
class App
{
public:
  App();
  ~App();

  void run();

private:
  class Viewer *viewer;
  class Window *window;
  bool running;

  float delta;
  float fps;
  float elapsed;

  const unsigned char *keyboardState;

  struct ImGuiIO *io;

  // Animation speed typed-edit state
  bool animSpeedEditMode;
  bool animSpeedFocusRequested;
  float animSpeedEditValue;

  void init();
  void handelInput();
  void renderGui();
  void calcFps();
};