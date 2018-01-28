#pragma once

#include "simulation/Simulation.h"
#include "nodes/CountryMesh.h"
#include "widgets/VirtualOverlay.h"

#include <memory>

namespace onep
{
	class CountryMenuWidget : public VirtualOverlay
	{
	public:
		CountryMenuWidget(Simulation::Ptr simulation);
		~CountryMenuWidget();

		void setCenterPosition(int x, int y);
    void setCountryMesh(CountryMesh::Ptr countryMesh);

	private:
		struct Impl;
		std::unique_ptr<Impl> m;
	};
}