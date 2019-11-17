#pragma once

#include <osgGaming/Injector.h>

#include "nodes/GlobeOverviewWorld.h"
#include "states/QtGameState.h"

namespace onep
{
	class GlobeOverviewState : public QtGameState
	{
	public:
		GlobeOverviewState(osgGaming::Injector& injector);
    ~GlobeOverviewState();

		virtual unsigned char getProperties() const override;

		virtual StateEvent* update() override;

		virtual void onKeyPressedEvent(int key) override;

	private:
    struct Impl;
    std::unique_ptr<Impl> m;

	};
}
