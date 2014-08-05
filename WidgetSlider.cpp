#include "WidgetSlider.h"
#include <Wt/WApplication>
#include <Wt/WStackedWidget>
#include <Wt/WText>
#include <Wt/WImage>
#include <Wt/WAnimation>
#include <Wt/WTimer>
#include <Wt/WTemplate>

WidgetSlider::WidgetSlider(WContainerWidget *parent):WContainerWidget(parent),m_currenIndex(0),m_timer(0)
{
    setStyleClass("central-banner shadow");
    m_timer = new WTimer(this);
    m_timer->timeout().connect(std::bind([=] () {if(++m_currenIndex > m_maxIndex-1)m_currenIndex = 0;m_stack->setCurrentIndex(m_currenIndex);}));
    m_leftBtn = new WContainerWidget(this);
    m_stack = new WStackedWidget(this);
    WAnimation tranformation(WAnimation::SlideInFromRight, WAnimation::CubicBezier, 400);
    m_stack->setTransitionAnimation(tranformation,true);
    m_rightBtn = new WContainerWidget(this);
    m_leftBtn->setStyleClass("back button");
    m_rightBtn->setStyleClass("forward button");
    //(new WImage("images/baner.jbg",this))->setStyleClass("banner-content");
    WContainerWidget *bannerContent = new WContainerWidget(this);
    bannerContent->setStyleClass("banner-content");
    bannerContent->decorationStyle().setBackgroundImage("images/baner.jbg");

    WTemplate *bannerText = new WTemplate("${header}${bannertext}",this);
    bannerText->setStyleClass("banner-text");
    WTemplate *header = new WTemplate(WString("Реклама",UTF8));
    header->setStyleClass("header");
    bannerText->bindWidget("header",header);
    bannerText->bindString("bannertext",WString("Ссылка по рекламе ",UTF8));

    new WText(this);

    m_rightBtn->clicked().connect(std::bind([=] () {if(++m_currenIndex > m_stack->count()-1)m_currenIndex = 0;m_stack->setCurrentIndex(m_currenIndex);}));
    m_leftBtn->clicked().connect(std::bind([=] () {if(--m_currenIndex < 0)m_currenIndex = m_stack->count()-1;m_stack->setCurrentIndex(m_currenIndex);}));
}

WidgetSlider::~WidgetSlider()
{

}

void WidgetSlider::addSlideWidget(WWidget *widget)
{

    WContainerWidget *cont = new WContainerWidget(this);
    //cont->set
    cont->addWidget(widget);
    m_stack->addWidget(cont);

}


void WidgetSlider::startTimer(int second)
{
    m_timer->setInterval(second*1000);
    m_timer->start();

}

void WidgetSlider::stopTimer()
{
    m_timer->stop();
}
