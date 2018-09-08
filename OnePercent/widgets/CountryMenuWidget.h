#pragma once

#include "scripting/LuaCountry.h"
#include "widgets/VirtualOverlay.h"

#include <memory>

#include <osgGaming/Injector.h>

namespace onep
{
	class CountryMenuWidget : public VirtualOverlay
	{
	public:
		CountryMenuWidget(osgGaming::Injector& injector);
		~CountryMenuWidget();

		void setCenterPosition(int x, int y);
    void setCountry(LuaCountry::Ptr country);

	private:
		struct Impl;
		std::unique_ptr<Impl> m;
	};
}