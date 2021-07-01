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

    template <typename KeyType>
    bool containsMappedObject(const KeyType& key)
    {
      return m_mappedObjects.count(key) > 0;
    }

    int getNumMappedObjects() const;

    template <typename LuaObject,
              typename = typename std::enable_if<std::is_base_of<LuaTableMappedObject, LuaObject>::value>::type>
    void iterateMappedObjects(std::function<void(std::shared_ptr<LuaObject>&)> func)
    {
      for (auto obj : m_mappedObjects)
      {
        auto object = std::dynamic_pointer_cast<LuaObject>(obj.second);
        func(object);
      }
    }

    template <typename LuaObject,
              typename = typename std::enable_if<std::is_base_of<LuaTableMappedObject, LuaObject>::value>::type,
              typename KeyType>
    std::shared_ptr<LuaObject> addMappedObject(KeyType key, luabridge::LuaRef& ref)
    {
      luaRef()[key] = ref;
      return makeSharedAndAddMappedObject<LuaObject, KeyType>(ref, key);
    }

    template <typename LuaObject,
        typename = typename std::enable_if<std::is_base_of<LuaTableMappedObject, LuaObject>::value>::type>
        std::shared_ptr<LuaObject> appendMappedObject(luabridge::LuaRef& ref)
    {
        luaRef().append(ref);
        const auto size = luaRef().length();
        return makeSharedAndAddMappedObject<LuaObject, int>(ref, size);
    }

    template <typename LuaObject,
              typename = typename std::enable_if<std::is_base_of<LuaTableMappedObject, LuaObject>::value>::type,
              typename KeyType,
              typename... Args>
    std::shared_ptr<LuaObject> addUserDataObject(KeyType key, luabridge::LuaRef& ref, Args&&... args)
    {
      auto obj = makeSharedAndAddMappedObject<LuaObject, KeyType>(ref, key, std::forward<Args>(args)...);
      if (obj)
      {
          luaRef()[key] = obj.get();
      }

      return obj;
    }

    template <typename LuaObject,
              typename = typename std::enable_if<std::is_base_of<LuaTableMappedObject, LuaObject>::value>::type,
              typename KeyType>
    std::shared_ptr<LuaObject> newMappedObject(KeyType key)
    {
      luabridge::LuaRef ref = luabridge::newTable(luaState());
      return addMappedObject<LuaObject, KeyType>(key, ref);
    }

    template <typename LuaObject,
              typename = typename std::enable_if<std::is_base_of<LuaTableMappedObject, LuaObject>::value>::type,
              typename KeyType>
    std::shared_ptr<LuaObject> makeMappedObject(KeyType key)
    {
      luabridge::LuaRef ref = luaRef()[key];
      return makeSharedAndAddMappedObject<LuaObject, KeyType>(ref, key);
    }

    template <typename LuaObject,
              typename = typename std::enable_if<std::is_base_of<LuaTableMappedObject, LuaObject>::value>::type>
    std::shared_ptr<LuaObject> makeMappedObject(luabridge::LuaRef& key)
    {
      if (key.isNumber())
        return makeMappedObject<LuaObject>(static_cast<int>(key));
      if (key.isString())
        return makeMappedObject<LuaObject>(key.tostring());

      assert_return(false, std::shared_ptr<LuaObject>()); // key must be number or string
    }

    template <typename LuaObject,
              typename = typename std::enable_if<std::is_base_of<LuaTableMappedObject, LuaObject>::value>::type>
    void makeAllMappedObjects()
    {
      iterateValues([this](const luabridge::Iterator& it)
      {
        makeMappedObject<LuaObject>(it.key());
      });
    }

    template <typename LuaObject,
              typename = typename std::enable_if<std::is_base_of<LuaTableMappedObject, LuaObject>::value>::type>
    std::shared_ptr<LuaObject> getMappedObject(const std::string& key) const
    {
      assert_return(m_mappedObjects.count(key) > 0, std::shared_ptr<LuaObject>());
      return std::dynamic_pointer_cast<LuaObject>(m_mappedObjects.find(key)->second);
    }

    template <typename LuaObject,
              typename = typename std::enable_if<std::is_base_of<LuaTableMappedObject, LuaObject>::value>::type>
    std::shared_ptr<LuaObject> getMappedObject(int key) const
    {
      return getMappedObject<LuaObject>(std::to_string(key));
    }

    template <typename LuaObject,
              typename = typename std::enable_if<std::is_base_of<LuaTableMappedObject, LuaObject>::value>::type>
    std::shared_ptr<LuaObject> getMappedObject(luabridge::LuaRef& key) const
    {
      if (key.isNumber())
      {
        return getMappedObject<LuaObject>(static_cast<int>(key));
      }
      if (key.isString())
      {
        return getMappedObject<LuaObject>(key.tostring());
      }

      assert_return(false, std::shared_ptr<LuaObject>()); // key must be number or string
    }

  private:
    using LuaMappedObjectsMap = std::map<std::string, Ptr>;

    LuaMappedObjectsMap m_mappedObjects;

    template <typename LuaObject,
              typename = typename std::enable_if<std::is_base_of<LuaTableMappedObject, LuaObject>::value>::type>
    void addToMappedObjectsMap(int key, std::shared_ptr<LuaObject>& obj)
    {
      m_mappedObjects[std::to_string(key)] = obj;
    }

    template <typename LuaObject,
              typename = typename std::enable_if<std::is_base_of<LuaTableMappedObject, LuaObject>::value>::type>
    void addToMappedObjectsMap(const std::string& key, std::shared_ptr<LuaObject>& obj)
    {
      m_mappedObjects[key] = obj;
    }

    template <typename LuaObject,
              typename = typename std::enable_if<std::is_base_of<LuaTableMappedObject, LuaObject>::value>::type,
              typename KeyType,
              typename... Args>
    std::shared_ptr<LuaObject> makeSharedAndAddMappedObject(luabridge::LuaRef& ref, KeyType key, Args&&... args)
    {
      std::shared_ptr<LuaObject> obj = std::make_shared<LuaObject>(ref, luaState(), std::forward<Args>(args)...);

      addToMappedObjectsMap<LuaObject>(key, obj);
      return obj;
    }
  };

}