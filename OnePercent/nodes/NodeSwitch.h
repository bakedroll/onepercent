#pragma once

#include <osg/Switch>

namespace onep
{
  template <typename TKey, typename TNode>
  class NodeSwitch : public osg::Switch
  {
  public:
    void addChild(const TKey& key, const osg::ref_ptr<TNode>& node, bool value = true)
	  {
	    m_nodes[key] = node;
	    osg::Switch::addChild(node, value);
	  }

	  void removeChild(const TKey& key)
	  {
	    auto it = m_nodes.find(key);
	    if (it == m_nodes.end())
	    {
	      return;
	    }

	    removeChild(it->second);
	    m_nodes.erase(it);
	  }

    bool hasChild(const TKey& key) const
    {
      return m_nodes.count(key) > 0;
    }

	  TNode* getChild(const TKey& key) const
	  {
	    if (m_nodes.count(key) == 0)
	    {
	      return nullptr;
	    }

	    return m_nodes.find(key)->second;
	  }

	  bool getKeyValue(const TKey& key) const
	  {
	    if (m_nodes.count(key) == 0)
	    {
	      return false;
	    }

	    return getChildValue(m_nodes.find(key)->second);
	  }

	  void setKeyValue(const TKey& key, bool value)
	  {
	    if (m_nodes.count(key) == 0)
	    {
	      return;
	    }

	    setChildValue(m_nodes[key], value);
	  }

    const std::map<TKey, osg::ref_ptr<TNode>>& getNodes() const
    {
      return m_nodes;
    }

  private:
    std::map<TKey, osg::ref_ptr<TNode>> m_nodes;
  };
}  // namespace onep