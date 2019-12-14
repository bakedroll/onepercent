#include "core/OnePercentApplication.h"

#ifdef WIN32
#include "core/CrashDumpWriter.h"
#endif

int main(int argc, char** argv)
{
#ifdef WIN32
  SetUnhandledExceptionFilter(onep::unhandled_handler);
#endif

  osg::ref_ptr<onep::OnePercentApplication> app = new onep::OnePercentApplication(argc, argv);
  return app->run();
}
