#pragma once

#include <QFrame>
#include <memory>

namespace onep
{
	class CountryMenuWidget : public QFrame
	{
	public:
		CountryMenuWidget(QWidget* parent = nullptr);
		~CountryMenuWidget();

		void setCenterPosition(int x, int y);

	private:
		struct Impl;
		std::unique_ptr<Impl> m;
	};
}