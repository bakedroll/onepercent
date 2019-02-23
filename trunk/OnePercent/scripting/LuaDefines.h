#pragma once

namespace onep
{
  namespace LuaDefines
  {
    enum class TickUpdateMode
    {
      LUA = 0,
      CPP = 1
    };

    enum class Callback
    {
      ON_INITIALIZE       = 0,
      ON_TICK             = 1,
      ON_SKILL_UPDATE     = 2,
      ON_BRANCH_UPDATE    = 3,
      ON_COUNTRY_CHANGED  = 4,
      ON_OVERLAY_CHANGED  = 5,
      ON_BRANCH_PURCHASED = 6
    };
  }
}