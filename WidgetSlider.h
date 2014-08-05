#ifndef WIDGETSLIDER_H
#define WIDGETSLIDER_H

#include <Wt/WContainerWidget>

using namespace Wt;

class WidgetSlider: public WContainerWidget
{
public:
    enum ButtonType{
        leftRight = 0,
        topBottom = 1
    };
    WidgetSlider(WContainerWidget *parent = 0);
    virtual ~WidgetSlider();
    void addSlideWidget(WWidget *widget);
    void startTimer(int second);
    void stopTimer();
protected:

private:
    int m_currenIndex;
    int m_maxIndex;
    WTimer *m_timer;
    WStackedWidget *m_stack;
    WContainerWidget * m_leftBtn;
    WContainerWidget * m_rightBtn;


};

#endif // WIDGETSLIDER_H
