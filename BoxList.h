#ifndef BOXLIST_H
#define BOXLIST_H

#include <Wt/WCompositeWidget>

using namespace Wt;

class BoxList : public WCompositeWidget
{
public:
    BoxList(WContainerWidget *parent=0);
    virtual ~BoxList();
    WWidget *addItem(WWidget *item);
    WContainerWidget *items() const {return m_ul;};
protected:
    WContainerWidget *ul() const {return m_ul;}
private:
    void init();
private:
    WContainerWidget *m_ul;
};

#endif // BOXLIST_H
