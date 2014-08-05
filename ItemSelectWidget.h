#ifndef ITEMSELECTWIDGET_H
#define ITEMSELECTWIDGET_H

#include <Wt/WContainerWidget>

using namespace Wt;

class ItemSelectWidget : public Wt::WContainerWidget
{
    public:
        ItemSelectWidget(WContainerWidget *parent = 0);
        virtual ~ItemSelectWidget();
    protected:
    private:
    void update();
};

#endif // ITEMSELECTWIDGET_H
