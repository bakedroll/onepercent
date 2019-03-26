#include "luadoc/Namespace.h"
#include "luadoc/DocsGenerator.h"

namespace luadoc
{
  Namespace::Namespace(luabridge::Namespace const& other)
    : luabridge::Namespace(other)
  {
  }

  Namespace Namespace::getGlobalNamespace(lua_State* L)
  {
    return Namespace(luabridge::getGlobalNamespace(L));
  }

  Namespace Namespace::beginNamespace(const char* name)
  {
    DocsGenerator::instance().beginNamespace(name);
    return Namespace(luabridge::Namespace::beginNamespace(name));
  }

  Namespace Namespace::endNamespace()
  {
    DocsGenerator::instance().endNamespace();
    return Namespace(luabridge::Namespace::endNamespace());
  }
}
