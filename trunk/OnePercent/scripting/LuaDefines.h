#pragma once

namespace onep
{
  namespace LuaDefines
  {
    enum class TickUpdateMode
    {
      LUA,
      CPP
    };

    enum class Callback
    {
      ON_INITIALIZE,
      ON_TICK,
      ON_SKILL_UPDATE,
      ON_BRANCH_UPDATE,
      ON_COUNTRY_CHANGED,
      ON_OVERLAY_CHANGED
    };
  }
}