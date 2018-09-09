#pragma once

#include "core/Macros.h"

#include <memory>
#include <map>

#include <functional>

#include <osgGaming/Macros.h>

extern "C"
{
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

#include <LuaBridge.h>

namespace onep
{
  class LuaObjectMapper
  {
  public:
    using Ptr = std::shared_ptr<LuaObjectMapper>;

    explicit LuaObjectMapper(const luabridge::LuaRef& object, lua_State* luaState);
    virtual ~LuaObjectMapper();

    luabridge::LuaRef& luaref() const;

    void foreachElementDo(std::function<void(luabridge::LuaRef& key, luabridge::LuaRef& value)> func);
    void traverseElements(int type);

    int getNumElements() const;

    template <typename LuaObject, typename KeyType>
    std::shared_ptr<LuaObject> newMappedElement(KeyType key)
    {
      luabridge::LuaRef ref = luabridge::newTable(m_luaState);
      (*m_ref)[key] = ref;
      return makeSharedAndAddElement<LuaObject, KeyType>(ref, key);
    }

    template <typename LuaObject, typename KeyType>
    std::shared_ptr<LuaObject> makeMappedElement(KeyType key)
    {
      luabridge::LuaRef ref = (*m_ref)[key];
      return makeSharedAndAddElement<LuaObject, KeyType>(ref, key);
    }

    template <typename LuaObject>
    std::shared_ptr<LuaObject> makeMappedElement(luabridge::LuaRef& key)
    {
      if (key.isNumber())
        return makeMappedElement<LuaObject>(int(key));
      if (key.isString())
        return makeMappedElement<LuaObject>(key.tostring());

      assert_return(false, std::shared_ptr<LuaObject>()); // key must be number or string
    }

    template <typename LuaObject>
    void makeAllMappedElements()
    {
      foreachElementDo([this](luabridge::LuaRef& key, luabridge::LuaRef& value)
      {
        makeMappedElement<LuaObject>(key);
      });
    }

    template <typename LuaObject>
    std::shared_ptr<LuaObject> getMappedElement(const std::string& key) const
    {
      assert_return(m_elements.count(key) > 0, std::shared_ptr<LuaObject>());
      return std::dynamic_pointer_cast<LuaObject>(m_elements.find(key)->second);
    }

    template <typename LuaObject>
    std::shared_ptr<LuaObject> getMappedElement(int key) const
    {
      return getMappedElement<LuaObject>(std::to_string(key));
    }

    template <typename LuaObject>
    std::shared_ptr<LuaObject> getMappedElement(luabridge::LuaRef& key) const
    {
      if (key.isNumber())
        return getMappedElement<LuaObject>(int(key));
      if (key.isString())
        return getMappedElement<LuaObject>(key.tostring());

      assert_return(false, std::shared_ptr<LuaObject>()); // key must be number or string
    }

  protected:
    virtual void onTraverse(int type, luabridge::LuaRef& object);

  private:
    using ElementsMap = std::map<std::string, Ptr>;

    std::unique_ptr<luabridge::LuaRef> m_ref;
    ElementsMap m_elements;
    lua_State* m_luaState;

    template <typename LuaObject>
    void addToElementsMap(int key, std::shared_ptr<LuaObject>& elem)
    {
      m_elements[std::to_string(key)] = elem;
    }

    template <typename LuaObject>
    void addToElementsMap(const std::string& key, std::shared_ptr<LuaObject>& elem)
    {
      m_elements[key] = elem;
    }

    template <typename LuaObject, typename KeyType>
    std::shared_ptr<LuaObject> makeSharedAndAddElement(luabridge::LuaRef& ref, KeyType key)
    {
      std::shared_ptr<LuaObject> elem = std::make_shared<LuaObject>(ref, m_luaState);
      addToElementsMap<LuaObject>(key, elem);
      return elem;
    }
  };

}