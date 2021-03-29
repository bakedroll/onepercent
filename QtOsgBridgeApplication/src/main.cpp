#include "QtOsgBridgeApplication.h"
#include "InitialState.h"

int main(int argc, char** argv)
{
  QtOsgBridgeApplication app(argc, argv);
  return app.runGame<InitialState>();
}
