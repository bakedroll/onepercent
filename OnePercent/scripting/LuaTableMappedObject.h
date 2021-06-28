#pragma once

#include "scripting/LuaTable.h"

#include <memory>
#include <map>

#include <functional>

#include <osgHelper/Macros.h>

#include <QtOsgBridge/Macros.h>

extern "C"
{
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

#include <LuaBridge/LuaBridge.h>

namespace onep
{
  class LuaTableMappedObject : public LuaTable
  {
  public:
    using Ptr = std::shared_ptr<LuaTableMappedObject>;

    explicit LuaTableMappedObject(const luabridge::LuaRef& object, lua_State* luaState);
    ~LuaTableMappedObject() override;

    void foreachElementDo(std::function<void(luabridge::LuaRef& key, luabridge::LuaRef& value)> func);

    template <typename KeyType>
    bool containsMappedObject(const KeyType& key)
    {
      return m_elements.count(key) > 0;
    }

    int getNumMappedObjects() const;

    template <typename LuaObject,
              typename = typename std::enable_if<std::is_base_of<LuaTableMappedObject, LuaObject>::value>::type>
    void iterateMappedObjects(std::function<void(std::shared_ptr<LuaObject>&)> func)
    {
      for (auto element : m_elements)
      {
        auto object = std::dynamic_pointer_cast<LuaObject>(element.second);
        func(object);
      }
    }

    template <typename LuaObject,
              typename = typename std::enable_if<std::is_base_of<LuaTableMappedObject, LuaObject>::value>::type,
              typename KeyType>
    std::shared_ptr<LuaObject> addMappedElement(KeyType key, luabridge::LuaRef& ref)
    {
      luaRef()[key] = ref;
      return makeSharedAndAddElement<LuaObject, KeyType>(ref, key);
    }

    template <typename LuaObject,
        typename = typename std::enable_if<std::is_base_of<LuaTableMappedObject, LuaObject>::value>::type>
        std::shared_ptr<LuaObject> appendMappedElement(luabridge::LuaRef& ref)
    {
        luaRef().append(ref);
        auto size = luaRef().length();
        return makeSharedAndAddElement<LuaObject, int>(ref, size);
    }

    template <typename LuaObject,
              typename = typename std::enable_if<std::is_base_of<LuaTableMappedObject, LuaObject>::value>::type,
              typename KeyType,
              typename... Args>
    std::shared_ptr<LuaObject> addUserDataElement(KeyType key, luabridge::LuaRef& ref, Args&&... args)
    {
      auto element = makeSharedAndAddElement<LuaObject, KeyType>(ref, key, std::forward<Args>(args)...);
      if (element)
      {
          luaRef()[key] = element.get();
      }

      return element;
    }

    template <typename LuaObject,
              typename = typename std::enable_if<std::is_base_of<LuaTableMappedObject, LuaObject>::value>::type,
              typename KeyType>
    std::shared_ptr<LuaObject> newMappedElement(KeyType key)
    {
      luabridge::LuaRef ref = luabridge::newTable(luaState());
      return addMappedElement<LuaObject, KeyType>(key, ref);
    }

    template <typename LuaObject,
              typename = typename std::enable_if<std::is_base_of<LuaTableMappedObject, LuaObject>::value>::type,
              typename KeyType>
    std::shared_ptr<LuaObject> makeMappedElement(KeyType key)
    {
      luabridge::LuaRef ref = luaRef()[key];
      return makeSharedAndAddElement<LuaObject, KeyType>(ref, key);
    }

    template <typename LuaObject,
              typename = typename std::enable_if<std::is_base_of<LuaTableMappedObject, LuaObject>::value>::type>
    std::shared_ptr<LuaObject> makeMappedElement(luabridge::LuaRef& key)
    {
      if (key.isNumber())
        return makeMappedElement<LuaObject>(int(key));
      if (key.isString())
        return makeMappedElement<LuaObject>(key.tostring());

      assert_return(false, std::shared_ptr<LuaObject>()); // key must be number or string
    }

    template <typename LuaObject,
              typename = typename std::enable_if<std::is_base_of<LuaTableMappedObject, LuaObject>::value>::type>
    void makeAllMappedElements()
    {
      foreachElementDo([this](luabridge::LuaRef& key, luabridge::LuaRef& value)
      {
        makeMappedElement<LuaObject>(key);
      });
    }

    template <typename LuaObject,
              typename = typename std::enable_if<std::is_base_of<LuaTableMappedObject, LuaObject>::value>::type>
    std::shared_ptr<LuaObject> getMappedElement(const std::string& key) const
    {
      assert_return(m_elements.count(key) > 0, std::shared_ptr<LuaObject>());
      return std::dynamic_pointer_cast<LuaObject>(m_elements.find(key)->second);
    }

    template <typename LuaObject,
              typename = typename std::enable_if<std::is_base_of<LuaTableMappedObject, LuaObject>::value>::type>
    std::shared_ptr<LuaObject> getMappedElement(int key) const
    {
      return getMappedElement<LuaObject>(std::to_string(key));
    }

    template <typename LuaObject,
              typename = typename std::enable_if<std::is_base_of<LuaTableMappedObject, LuaObject>::value>::type>
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

    enum class InconsistencyType
    {
      ElementMissing,
      WrongType
    };

    struct Inconsistency
    {
      std::string       key;
      InconsistencyType type;
    };

    using InconsistencyList = std::vector<Inconsistency>;

    ElementsMap m_elements;

    static std::string strKey(const std::string& key) { return key; }
    static std::string strKey(int key) { return std::to_string(key); }

    template <typename LuaObject,
              typename = typename std::enable_if<std::is_base_of<LuaTableMappedObject, LuaObject>::value>::type>
    void addToElementsMap(int key, std::shared_ptr<LuaObject>& elem)
    {
      m_elements[std::to_string(key)] = elem;
    }

    template <typename LuaObject,
              typename = typename std::enable_if<std::is_base_of<LuaTableMappedObject, LuaObject>::value>::type>
    void addToElementsMap(const std::string& key, std::shared_ptr<LuaObject>& elem)
    {
      m_elements[key] = elem;
    }

    template <typename LuaObject,
              typename = typename std::enable_if<std::is_base_of<LuaTableMappedObject, LuaObject>::value>::type,
              typename KeyType,
              typename... Args>
    std::shared_ptr<LuaObject> makeSharedAndAddElement(luabridge::LuaRef& ref, KeyType key, Args&&... args)
    {
      std::shared_ptr<LuaObject> elem = std::make_shared<LuaObject>(ref, luaState(), std::forward<Args>(args)...);

      addToElementsMap<LuaObject>(key, elem);
      return elem;
    }
  };

}