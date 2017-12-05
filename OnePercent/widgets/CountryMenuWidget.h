#pragma once

#include "widgets/VirtualOverlay.h"

#include <memory>

namespace onep
{
	class CountryMenuWidget : public VirtualOverlay
	{
	public:
		CountryMenuWidget();
		~CountryMenuWidget();

		void setCenterPosition(int x, int y);

	private:
		struct Impl;
		std::unique_ptr<Impl> m;
	};
}