#pragma once

#include "scripting/LuaCountry.h"

#include <memory>

#include <QtOsgBridge/VirtualOverlay.h>

#include <osgHelper/ioc/Injector.h>

#include <QWidget>

namespace onep
{
	class CountryMenuWidget : public QtOsgBridge::VirtualOverlay
	{
	public:
		CountryMenuWidget(osgHelper::ioc::Injector& injector);
		~CountryMenuWidget();

		void setCenterPosition(int x, int y);
    void setCountry(const LuaCountry::Ptr& country);

	private:
		struct Impl;
		std::unique_ptr<Impl> m;
	};
}