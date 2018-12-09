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

#include <LuaBridge/LuaBridge.h>

namespace onep
{
  class LuaObjectMapper
  {
  public:
    using Ptr = std::shared_ptr<LuaObjectMapper>;
    using VisitorFunc = std::function<void(luabridge::LuaRef&)>;

    explicit LuaObjectMapper(const luabridge::LuaRef& object, lua_State* luaState);
    virtual ~LuaObjectMapper();

    luabridge::LuaRef& luaref() const;

    void addVisitorFunc(int type, VisitorFunc func);

    void foreachElementDo(std::function<void(luabridge::LuaRef& key, luabridge::LuaRef& value)> func);
    void traverseElements(int type);

    template <typename KeyType>
    bool containsMappedElement(const KeyType& key)
    {
      return m_elements.count(key) > 0;
    }

    int getNumElements() const;

    template<typename LuaObject>
    void foreachMappedElementDo(std::function<void(std::shared_ptr<LuaObject>&)> func)
    {
      for (auto element : m_elements)
      {
        auto object = std::dynamic_pointer_cast<LuaObject>(element.second);
        func(object);
      }
    }

    template <typename LuaObject, typename KeyType>
    std::shared_ptr<LuaObject> addMappedElement(KeyType key, luabridge::LuaRef& ref)
    {
      (*m_ref)[key] = ref;
      return makeSharedAndAddElement<LuaObject, KeyType>(ref, key);
    }

    template <typename LuaObject, typename KeyType>
    std::shared_ptr<LuaObject> newMappedElement(KeyType key)
    {
      luabridge::LuaRef ref = luabridge::newTable(m_luaState);
      return addMappedElement<LuaObject, KeyType>(key, ref);
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

  private:
    using ElementsMap = std::map<std::string, Ptr>;
    using TraversalMap = std::map<int, VisitorFunc>;

    std::unique_ptr<luabridge::LuaRef> m_ref;
    ElementsMap m_elements;
    TraversalMap m_visitorFuncs;
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