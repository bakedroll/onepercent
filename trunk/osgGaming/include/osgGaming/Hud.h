#pragma once

#include <osgGaming/UIElement.h>
#include <osgGaming/UIGrid.h>
#include <osgGaming/UserInteractionModel.h>

#include <osg/Referenced>
#include <osg/Projection>
#include <osg/Geode>

#include <osgText/Text>

namespace osgGaming
{
	class Hud : public osg::Referenced
	{
	public:
		Hud();

		osg::ref_ptr<osg::Projection> getProjection();

		void loadMarkupFromXmlResource(std::string resourceKey);

		// TODO: remove later
		osg::ref_ptr<osg::Geode> getGeode();

		osg::ref_ptr<UIElement> getRootUIElement();
		UserInteractionModelList getUserInteractionModels();

		osg::ref_ptr<osgGaming::UIElement> getUIElementByName(std::string name);

		void updateResolution(osg::Vec2f resolution);
		void updateUIElements();

		void setFpsEnabled(bool enabled);

		void setRootUIElement(osg::ref_ptr<UIElement> element);

		void registerUserInteractionModel(UserInteractionModel* model);

		bool anyUserInteractionModelHovered();
		void resetUserInteractionModel();

	private:
		osg::ref_ptr<osg::Projection> _projection;
		osg::ref_ptr<osg::Geode> _geode;

		osg::ref_ptr<UIElement> _rootUIElement;
		osg::ref_ptr<osg::MatrixTransform> _matrixTransform;

		osg::ref_ptr<osgText::Text> _fpsText;
		bool _fpsEnabled;

		osg::Vec2f _resolution;

		UserInteractionModelList _uimList;
	};
}