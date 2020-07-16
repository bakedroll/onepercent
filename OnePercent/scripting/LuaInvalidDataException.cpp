#include "scripting/LuaInvalidDataException.h"

namespace onep
{

LuaInvalidDataException::LuaInvalidDataException(const QString& message)
  : std::runtime_error(QString("Invalid lua data exception: %1").arg(message).toStdString())
{
}

LuaInvalidDataException::LuaInvalidDataException(const LuaInvalidDataException& rhs)
  : std::runtime_error(rhs.what())
{
}

LuaInvalidDataException::~LuaInvalidDataException() = default;

}
