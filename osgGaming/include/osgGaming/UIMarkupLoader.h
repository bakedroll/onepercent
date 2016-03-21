#pragma once

#include <string>
#include <vector>

#include <rapidxml.hpp>

#include <osgGaming/Singleton.h>
#include <osgGaming/UIElement.h>
#include <osgGaming/UIGrid.h>
#include <osgGaming/UIStackPanel.h>
#include <osgGaming/UIButton.h>
#include <osgGaming/UIText.h>

namespace osgGaming
{
	class UIMarkupLoader : public Singleton<UIMarkupLoader>
	{
	public:
		osg::ref_ptr<UIElement> loadMarkupFromXmlResource(std::string resourceKey);

	private:
		typedef struct _xmlUiElement
		{
			osg::ref_ptr<UIElement> uiElement;
			rapidxml::xml_node<>* xmlNode;
		} XmlUiElement;

		typedef std::vector<XmlUiElement> XmlUiElementList;

		void parseXmlElement(rapidxml::xml_node<>* node, osg::ref_ptr<UIElement> uiElement);
		void parseXmlGrid(rapidxml::xml_node<>* node, osg::ref_ptr<UIGrid> uiGrid);
		void parseXmlStackPanel(rapidxml::xml_node<>* node, osg::ref_ptr<UIStackPanel> uiStackPanel);
		void parseXmlButton(rapidxml::xml_node<>* node, osg::ref_ptr<UIButton> uiButton);
		void parseXmlText(rapidxml::xml_node<>* node, osg::ref_ptr<UIText> uiText);
		XmlUiElementList parseXmlChildElements(rapidxml::xml_node<>* node);

		void parseXmlElementAttributes(rapidxml::xml_node<>* node, osg::ref_ptr<UIElement> uiElement);

		bool parseXmlAttribute(std::string& value);
		std::string parseXmlAttributeString(std::string value);
		int parseXmlAttributeInt(std::string value);
		float parseXmlAttributeFloat(std::string value);
		bool parseXmlAttributeBool(std::string value);
		osg::Vec4f parseXmlAttributeVec4(std::string value);

		void parseXmlCellDefinitions(rapidxml::xml_node<>* node, const char* attrname, const char* defname, const char* tagname, osg::ref_ptr<UICells> cells, bool reverseIdx);
	};
}