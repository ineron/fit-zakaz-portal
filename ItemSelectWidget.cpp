#include "ItemSelectWidget.h"
#include "ItemPad.h"
#include "ItemHScroll.h"
#include "ItemVScroll.h"

ItemSelectWidget::ItemSelectWidget(WContainerWidget *parent): Wt::WContainerWidget(parent)
{
    setStyleClass("item-field");
    update();
}

ItemSelectWidget::~ItemSelectWidget()
{
    //dtor
}

void ItemSelectWidget::update()
{
    clear();
    //new ItemVScroll(this);
    //new ItemPad(this);
    //new ItemHScroll(this);
}
