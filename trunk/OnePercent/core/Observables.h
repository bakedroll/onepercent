#include <osgGaming/Observable.h>

#define DEF_INT_OBSERVABLE(name) \
  class name : public InitializedObservable<int, 0> \
  { \
  public: \
    name(osgGaming::Injector& injector) \
      : InitializedObservable<int, 0>(injector) \
    {} \
  };

namespace onep
{
  template <typename T, T init = T()>
  class InitializedObservable : public osgGaming::Observable<T>
  {
  public:
    InitializedObservable(osgGaming::Injector& injector)
      : osgGaming::Observable<T>(init)
    {
      
    }
  };

  DEF_INT_OBSERVABLE(ONumSkillPoints);
  DEF_INT_OBSERVABLE(ODay);
}