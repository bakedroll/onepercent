#include "core/OnePercentApplication.h"

#ifdef WIN32
#include <QtOsgBridge/CrashDumpWriter.h>
#endif

int main(int argc, char** argv)
{
#ifdef WIN32
  SetUnhandledExceptionFilter(QtOsgBridge::unhandled_handler);
#endif

  onep::OnePercentApplication app(argc, argv);
  return app.run();
}
