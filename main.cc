#include "app/app.h"
#include <iostream>

int main()
{

  try
  {
    App app;
    app.run();
  }
  catch (const std::exception &e)
  {
    std::cout << "Exception: " << e.what() << std::endl;
  }

  return 0;
}
