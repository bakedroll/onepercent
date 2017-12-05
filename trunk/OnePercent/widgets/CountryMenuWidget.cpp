#include "CountryMenuWidget.h"

#include "core/Globals.h"

#include <osgGaming/Helper.h>

#include <QPushButton>

namespace onep
{
	struct CountryMenuWidget::Impl
	{
		Impl() {}
	};

	CountryMenuWidget::CountryMenuWidget()
		: VirtualOverlay()
		, m(new Impl())
	{
    QSize size(400, 400);
    QSize buttonSize(80, 80);

    setFixedSize(size);

    for (int i = 0; i<NUM_SKILLBRANCHES; i++)
    {
      QPushButton* button = new QPushButton(QString::fromStdString(branch_getStringFromType(i)));
      button->setParent(this);
      button->setFixedSize(buttonSize);
      button->setGeometry(
        size.width() / 2 + sin(float(i) * C_2PI / float(NUM_SKILLBRANCHES)) * 110 - buttonSize.width() / 2,
        size.height() / 2 + cos(float(i) * C_2PI / float(NUM_SKILLBRANCHES)) * 110 - buttonSize.height() / 2,
        buttonSize.width(),
        buttonSize.height());
    }
	}

	CountryMenuWidget::~CountryMenuWidget()
	{
	}

	void CountryMenuWidget::setCenterPosition(int x, int y)
	{
    QRect geo = geometry();
    setGeometry(x - geo.width() / 2, y - geo.height() / 2, geo.width(), geo.height());
	}

}