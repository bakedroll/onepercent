#pragma once

#include <osgGaming/UserInteractionModel.h>

#include <osg/Referenced>
#include <osg/Projection>
#include <osg/MatrixTransform>

#include <memory>

namespace osgGaming
{
  class Injector;
  class UIElement;

	class Hud : public osg::Referenced
	{
	public:
    typedef osg::ref_ptr<Hud> Ptr;

		Hud(Injector& injector);
		~Hud();

		osg::ref_ptr<osg::Projection> getProjection();
		osg::ref_ptr<osg::MatrixTransform> getModelViewTransform();

		osg::ref_ptr<UIElement> getRootUIElement();
		UserInteractionModel::List& getUserInteractionModels();

		osg::ref_ptr<osgGaming::UIElement> getUIElementByName(std::string name);

		void updateResolution(osg::Vec2f resolution);
		void updateUIElements();

		void setFpsEnabled(bool enabled);

		void setRootUIElement(osg::ref_ptr<UIElement> element);

		void registerUserInteractionModel(UserInteractionModel* model);

		bool anyUserInteractionModelHovered();
		void resetUserInteractionModel();

	private:
		struct Impl;
		std::unique_ptr<Impl> m;

	};
}
