#pragma once

#include <stdexcept>

#include <QString>

namespace onep
{

  class LuaInvalidDataException : public std::runtime_error
  {
  public:
    explicit LuaInvalidDataException(const QString& message);
    LuaInvalidDataException(const LuaInvalidDataException& rhs);
    ~LuaInvalidDataException() override;

  };

}
