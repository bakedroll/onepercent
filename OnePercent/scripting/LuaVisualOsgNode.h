#pragma once

#include <osg/Node>

#include <osgHelper/Macros.h>

#include <luaHelper/LuaBridgeDefinition.h>

#include <QtOsgBridge/Macros.h>

#include <QString>

namespace onep
{
  using OsgUniformPtr = osg::ref_ptr<osg::Uniform>;

  template<
    typename NodeType,
    typename = typename std::enable_if<std::is_base_of<osg::Node, NodeType>::value>::type>
  class LuaVisualOsgNode : public NodeType
  {
  public:
    class Definition : public luaHelper::LuaBridgeDefinition
    {
    public:
      void registerDefinition(lua_State* state) override
      {
        osg::ref_ptr<NodeType> node = new NodeType();

        std::string osgClassName = node->className();
        std::string luaClassName = "VisualOsg" + osgClassName;

        getGlobalNamespace(state)
          .deriveClass<LuaVisualOsgNode<NodeType>, osg::Node>(luaClassName.c_str())
          .addFunction("set_uniform_float",  &LuaVisualOsgNode<NodeType>::template luaSetUniform<float>)
          .addFunction("set_uniform_double", &LuaVisualOsgNode<NodeType>::template luaSetUniform<double>)
          .addFunction("set_uniform_vec2f",  &LuaVisualOsgNode<NodeType>::template luaSetUniform<const osg::Vec2f&>)
          .addFunction("set_uniform_vec3f",  &LuaVisualOsgNode<NodeType>::template luaSetUniform<const osg::Vec3f&>)
          .addFunction("set_uniform_vec4f",  &LuaVisualOsgNode<NodeType>::template luaSetUniform<const osg::Vec4f&>)
          .addFunction("set_uniform_vec2d",  &LuaVisualOsgNode<NodeType>::template luaSetUniform<const osg::Vec2d&>)
          .addFunction("set_uniform_vec3d",  &LuaVisualOsgNode<NodeType>::template luaSetUniform<const osg::Vec3d&>)
          .addFunction("set_uniform_vec4d",  &LuaVisualOsgNode<NodeType>::template luaSetUniform<const osg::Vec4d&>)
          .endClass();
      }
    };

    LuaVisualOsgNode()
      : NodeType()
    {}

    void addStateSetUniform(const OsgUniformPtr& uniform, const osg::ref_ptr<osg::Node>& alternateNode = nullptr)
    {
      auto stateSet = alternateNode ? alternateNode->getOrCreateStateSet() : this->getOrCreateStateSet();
      stateSet->addUniform(uniform);
      m_uniforms[uniform->getName()] = uniform;
    }

    OsgUniformPtr getStateSetUniform(const std::string& name) const
    {
      const auto& it = m_uniforms.find(name);
      if (it == m_uniforms.end())
      {
        assert_return(false, nullptr);
      }

      return it->second;
    }

    template <typename ValueType>
    void luaSetUniform(const std::string& name, ValueType value)
    {
      auto uniform = getStateSetUniform(name);
      assert_return(uniform);

      if (!uniform)
      {
        OSGH_QLOG_WARN(QString("Uniform %1 not found.").arg(name.c_str()));
      }

      uniform->set(value);
    }

  private:
    using UniformMap = std::map<std::string, OsgUniformPtr>;
    
    UniformMap m_uniforms;

  };
}
