#include "MainMenu.h"
#include <Wt/WContainerWidget>



MainMenu::MainMenu(WContainerWidget *parent):WMenu(parent)
{
    //ctor
}

MainMenu::~MainMenu()
{
    //dtor
}

MainMenuItem::MainMenuItem(const WString &label, WWidget *contents, LoadPolicy policy):WMenuItem(label,contents,policy)
{
    //ctor
}

MainMenuItem::~MainMenuItem()
{
    //dtor
}

void MainMenuItem::renderSelected(bool selected)
{
    WMenuItem::renderSelected(selected);
    if (menu())
        setStyleClass("has-sub");
}
