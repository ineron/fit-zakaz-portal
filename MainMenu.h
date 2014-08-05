#ifndef MAINMENU_H
#define MAINMENU_H

#include <Wt/WMenu>


using namespace Wt;

class MainMenu : public Wt::WMenu
{
    public:
        MainMenu(WContainerWidget *parent=0);
        virtual ~MainMenu();
    protected:
    private:
};


class MainMenuItem : public Wt::WMenuItem
{
    public:
        MainMenuItem(const WString &label, WWidget *contents=0, LoadPolicy policy=WMenuItem::LazyLoading);
        virtual ~MainMenuItem();
    protected:
        virtual void renderSelected(bool selected);
    private:
};
#endif // MAINMENU_H
