#include <osgGaming/UIMarkupLoader.h>
#include <osgGaming/ResourceManager.h>
#include <osgGaming/Property.h>
#include <osgGaming/Helper.h>
#include <osgGaming/GameException.h>

using namespace osgGaming;
using namespace osg;
using namespace std;
using namespace rapidxml;

template<>
ref_ptr<UIMarkupLoader> Singleton<UIMarkupLoader>::_instance;

ref_ptr<UIElement> UIMarkupLoader::loadMarkupFromXmlResource(string resourceKey)
{
	string xmlText = ResourceManager::getInstance()->loadText(resourceKey);
	char* xmlCstr = new char[xmlText.size() + 1];
	strcpy(xmlCstr, xmlText.c_str());

	xml_document<> doc;
	doc.parse<0>(xmlCstr);

	xml_node<>* rootNode = doc.first_node("root");

	if (rootNode == nullptr)
	{
		throw GameException("Invalid resource format: " + resourceKey);
	}

	ref_ptr<UIGrid> grid = new UIGrid();

	parseXmlElement(rootNode, grid);

	delete[] xmlCstr;
	ResourceManager::getInstance()->clearCacheResource(resourceKey);

	return grid;
}

void UIMarkupLoader::parseXmlElement(xml_node<>* node, ref_ptr<UIElement> uiElement)
{
	XmlUiElementList xmlUiElements = parseXmlChildElements(node);

	for (XmlUiElementList::iterator it = xmlUiElements.begin(); it != xmlUiElements.end(); ++it)
	{
		uiElement->addChild(it->uiElement);
	}
}

void UIMarkupLoader::parseXmlGrid(xml_node<>* node, ref_ptr<UIGrid> uiGrid)
{
	XmlUiElementList xmlUiElements = parseXmlChildElements(node);

	parseXmlCellDefinitions(node, "rows", "rowdefinitions", "row", uiGrid->getRows());
	parseXmlCellDefinitions(node, "columns", "coldefinitions", "column", uiGrid->getColumns());

	xml_attribute<>* attrSpacing = node->first_attribute("spacing");
	if (attrSpacing != nullptr)
	{
		uiGrid->setSpacing(parseXmlAttributeFloat(attrSpacing->value()));
	}

	for (XmlUiElementList::iterator it = xmlUiElements.begin(); it != xmlUiElements.end(); ++it)
	{
		int col = 0, row = 0;

		xml_attribute<>* attrRow = it->xmlNode->first_attribute("row");
		if (attrRow != nullptr)
		{
      row = parseXmlAttributeInt(attrRow->value());
      //row = uiGrid->getRows()->getNumCells() - (parseXmlAttributeInt(attrRow->value()) + 1);
		}

		xml_attribute<>* attrCol = it->xmlNode->first_attribute("col");
		if (attrCol != nullptr)
		{
			col = parseXmlAttributeInt(attrCol->value());
		}

		uiGrid->addChild(it->uiElement, ColRow(col, row));
	}
}

void UIMarkupLoader::parseXmlStackPanel(xml_node<>* node, ref_ptr<UIStackPanel> uiStackPanel)
{
	XmlUiElementList xmlUiElements = parseXmlChildElements(node);

  xml_attribute<>* attrOrientation = node->first_attribute("orientation");
  if (attrOrientation != nullptr)
  {
    std::string orentation = parseXmlAttributeString(attrOrientation->value());

    if (orentation == "vertical")
    {
      uiStackPanel->setOrientation(UIStackPanel::VERTICAL);
    }
    else if (orentation == "horizontal")
    {
      uiStackPanel->setOrientation(UIStackPanel::HORIZONTAL);
    }
    else
    {
      throw GameException("Invalid Orientation attribute");
    }
  }

  xml_attribute<>* attrSizePolicy = node->first_attribute("sizepolicy");
  if (attrSizePolicy != nullptr)
  {
    std::string sizePolizy = parseXmlAttributeString(attrSizePolicy->value());
    
    if (sizePolizy == "content")
    {
      uiStackPanel->getCells()->setDefaultSizePolicy(UICells::CONTENT);
    }
    else if (sizePolizy == "auto")
    {
      uiStackPanel->getCells()->setDefaultSizePolicy(UICells::AUTO);
    }
    else
    {
      throw GameException("Invalid Orientation attribute");
    }
  }

  parseXmlCellDefinitions(node, "cells", "celldefinitions", "cell", uiStackPanel->getCells());

	xml_attribute<>* attrSpacing = node->first_attribute("spacing");
	if (attrSpacing != nullptr)
	{
		uiStackPanel->setSpacing(parseXmlAttributeFloat(attrSpacing->value()));
	}

  int cell = 
    //uiStackPanel->getOrientation() == UIStackPanel::VERTICAL ? 
    //uiStackPanel->getCells()->getNumCells() - 1 : 
    0;

	for (XmlUiElementList::iterator it = xmlUiElements.begin(); it != xmlUiElements.end(); ++it)
	{
		xml_attribute<>* attrCell = it->xmlNode->first_attribute("cell");
		if (attrCell != nullptr)
		{
      cell = parseXmlAttributeInt(attrCell->value()); 
      
      //if (uiStackPanel->getOrientation() == UIStackPanel::VERTICAL)
      //  cell = uiStackPanel->getCells()->getNumCells() - (cell + 1);

      uiStackPanel->addChild(it->uiElement, cell);
		}
    else
    {
      uiStackPanel->addChild(it->uiElement, cell);

      //if (uiStackPanel->getOrientation() == UIStackPanel::VERTICAL)
      //  cell--;
      //else
        cell++;

    }
	}
}

void UIMarkupLoader::parseXmlButton(xml_node<>* node, ref_ptr<UIButton> uiButton)
{
	xml_attribute<>* attrText = node->first_attribute("text");
	if (attrText != nullptr)
	{
		uiButton->setText(parseXmlAttributeString(attrText->value()));
	}
}

void UIMarkupLoader::parseXmlText(xml_node<>* node, ref_ptr<UIText> uiText)
{
	xml_attribute<>* attrContent = node->first_attribute("content");
	if (attrContent != nullptr)
		uiText->setText(parseXmlAttributeString(attrContent->value()));

	xml_attribute<>* attrFontsize = node->first_attribute("fontsize");
	if (attrFontsize != nullptr)
		uiText->setFontSize(parseXmlAttributeInt(attrFontsize->value()));

  xml_attribute<>* attrOutlineColor = node->first_attribute("outlinecolor");
  if (attrOutlineColor != nullptr)
  {
    uiText->setOutlineEnabled(true);
    uiText->setOutlineColor(parseXmlAttributeVec4(attrOutlineColor->value()));
  }
}

UIMarkupLoader::XmlUiElementList UIMarkupLoader::parseXmlChildElements(xml_node<>* node)
{
	XmlUiElementList xmlUiElements;

	xml_node<>* child = node->first_node();
	while (child != nullptr)
	{
		XmlUiElement element;

		if (strcmp(child->name(), "grid") == 0)
		{
			ref_ptr<UIGrid> grid = new UIGrid();

			element.uiElement = grid;
			parseXmlGrid(child, grid);
		}
		else if (strcmp(child->name(), "stackpanel") == 0)
		{
			ref_ptr<UIStackPanel> stackPanel = new UIStackPanel();

			element.uiElement = stackPanel;
			parseXmlStackPanel(child, stackPanel);
		}
		else if (strcmp(child->name(), "button") == 0)
		{
			ref_ptr<UIButton> button = new UIButton();

			element.uiElement = button;
			parseXmlButton(child, button);
		}
		else if (strcmp(child->name(), "text") == 0)
		{
			ref_ptr<UIText> text = new UIText();

			element.uiElement = text;
			parseXmlText(child, text);
		}

		if (element.uiElement.valid())
		{
			// element.uiElement->getVisualGroup();

			parseXmlElementAttributes(child, element.uiElement);

			element.xmlNode = child;
			xmlUiElements.push_back(element);
		}

    xml_attribute<>* attrVertAlign = child->first_attribute("vertical_alignment");
    if (attrVertAlign != nullptr)
    {
      if (strcmp(attrVertAlign->value(), "top") == 0)
        element.uiElement->setVerticalAlignment(UIElement::TOP);
      else if (strcmp(attrVertAlign->value(), "middle") == 0)
        element.uiElement->setVerticalAlignment(UIElement::MIDDLE);
      else if (strcmp(attrVertAlign->value(), "bottom") == 0)
        element.uiElement->setVerticalAlignment(UIElement::BOTTOM);
      else if (strcmp(attrVertAlign->value(), "stretch") == 0)
        element.uiElement->setVerticalAlignment(UIElement::V_STRETCH);
      else
        throw GameException("Invalid VerticalAlignment attribute");
    }

    xml_attribute<>* attrHorAlign = child->first_attribute("horizontal_alignment");
    if (attrHorAlign != nullptr)
    {
      if (strcmp(attrHorAlign->value(), "left") == 0)
        element.uiElement->setHorizontalAlignment(UIElement::LEFT);
      else if (strcmp(attrHorAlign->value(), "center") == 0)
        element.uiElement->setHorizontalAlignment(UIElement::CENTER);
      else if (strcmp(attrHorAlign->value(), "right") == 0)
        element.uiElement->setHorizontalAlignment(UIElement::RIGHT);
      else if (strcmp(attrHorAlign->value(), "stretch") == 0)
        element.uiElement->setHorizontalAlignment(UIElement::H_STRETCH);
      else
        throw GameException("Invalid HorizontalAlignment attribute");
    }

		child = child->next_sibling();
	}

	return xmlUiElements;
}

void UIMarkupLoader::parseXmlElementAttributes(xml_node<>* node, ref_ptr<UIElement> uiElement)
{
	xml_attribute<>* attrName = node->first_attribute("name");
	if (attrName != nullptr)
	{
		uiElement->setUIName(parseXmlAttributeString(attrName->value()));
	}

	xml_attribute<>* attrMargin = node->first_attribute("margin");
	if (attrMargin != nullptr)
	{
		uiElement->setMargin(parseXmlAttributeVec4(attrMargin->value()));
	}

	xml_attribute<>* attrPadding = node->first_attribute("padding");
	if (attrPadding != nullptr)
	{
		uiElement->setPadding(parseXmlAttributeVec4(attrPadding->value()));
	}

	xml_attribute<>* attrWidth = node->first_attribute("width");
	if (attrWidth != nullptr)
	{
		uiElement->setWidth(parseXmlAttributeFloat(attrWidth->value()));
	}

	xml_attribute<>* attrHeight = node->first_attribute("height");
	if (attrHeight != nullptr)
	{
		uiElement->setHeight(parseXmlAttributeFloat(attrHeight->value()));
	}

	xml_attribute<>* attrVisible = node->first_attribute("visible");
	if (attrVisible != nullptr)
	{
		uiElement->setVisible(parseXmlAttributeBool(attrVisible->value()));
	}
}

bool UIMarkupLoader::parseXmlAttribute(string& value)
{
	if (value.length() > 2 && value[0] == '{' && value[value.length() - 1] == '}')
	{
		value = value.substr(1, value.length() - 2);
		return true;
	}

	return false;
}

string UIMarkupLoader::parseXmlAttributeString(string value)
{
	return parseXmlAttribute(value) ? ~Property<string>(value.c_str()) : utf8ToLatin1(value.c_str());
}

int UIMarkupLoader::parseXmlAttributeInt(string value)
{
	return parseXmlAttribute(value) ? ~Property<int>(value.c_str()) : stoi(value);
}

float UIMarkupLoader::parseXmlAttributeFloat(string value)
{
	return parseXmlAttribute(value) ? ~Property<float>(value.c_str()) : stof(value);
}

bool UIMarkupLoader::parseXmlAttributeBool(string value)
{
	return parseXmlAttribute(value) ? ~Property<bool>(value.c_str()) : (value == "true" ? true : false);
}

Vec4f UIMarkupLoader::parseXmlAttributeVec4(string value)
{
	return parseXmlAttribute(value) ? ~Property<Vec4f>(value.c_str()) : parseVector<Vec4f>(value);
}

void UIMarkupLoader::parseXmlCellDefinitions(rapidxml::xml_node<>* node, const char* attrname, const char* defname, const char* tagname, ref_ptr<UICells> cells)
{
	xml_attribute<>* attrCells = node->first_attribute(attrname);
	if (attrCells != nullptr)
	{
		cells->setNumCells(parseXmlAttributeInt(attrCells->value()));
	}
  else
  {
    int numCells = 0;
    xml_node<>* child = node->first_node();

    while (child != nullptr)
    {
      if (strcmp(child->name(), defname) != 0)
        numCells++;

      child = child->next_sibling();
    }

    cells->setNumCells(numCells);
  }

	xml_node<>* defnode = node->first_node(defname);
	if (defnode != nullptr)
	{
		xml_node<>* tagnode = defnode->first_node(tagname);
		while (tagnode != nullptr)
		{
			xml_attribute<>* indexAttr = tagnode->first_attribute("index");

			if (indexAttr == nullptr)
			{
				continue;
			}

			int index = parseXmlAttributeInt(indexAttr->value());

			xml_attribute<>* sizepolicyAttr = tagnode->first_attribute("sizepolicy");
			if (sizepolicyAttr != nullptr)
			{
				std::string sizePolicy = parseXmlAttributeString(sizepolicyAttr->value());

				if (sizePolicy == "content")
				{
					cells->setSizePolicy(index, UICells::CONTENT);
				}
				else if (sizePolicy == "auto")
				{
					cells->setSizePolicy(index, UICells::AUTO);
				}
				else
				{
					throw GameException("Invalid SizePolicy attribute");
				}

				// cells->setSizePolicy(index, strcmp(parseXmlAttributeString(sizepolicyAttr->value()).c_str(), "content") == 0 ? UICells::CONTENT : UICells::AUTO);
			}

			tagnode = tagnode->next_sibling(tagname);
		}
	}
}
