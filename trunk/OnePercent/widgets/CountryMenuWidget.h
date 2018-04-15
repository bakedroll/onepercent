#pragma once

#include "simulation/Country.h"
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
    void setCountry(Country::Ptr country);

	private:
		struct Impl;
		std::unique_ptr<Impl> m;
	};
}