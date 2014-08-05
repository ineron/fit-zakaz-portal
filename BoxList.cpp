#include "BoxList.h"

#include <Wt/WContainerWidget>


BoxList::BoxList(WContainerWidget *parent):WCompositeWidget(parent)
{
    init();
}

BoxList::~BoxList()
{
    //dtor
}

void BoxList::init()
{
    setImplementation(m_ul = new WContainerWidget());
    m_ul->setList(true);

}

WWidget *BoxList::addItem(WWidget *item)
{
    ul()->addWidget(item);
    return item;
}
