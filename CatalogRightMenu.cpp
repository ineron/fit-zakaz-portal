#include "CatalogRightMenu.h"
#include <Wt/WLineEdit>
#include <Wt/WPushButton>
#include <Wt/WTemplate>
#include <Wt/WCssDecorationStyle>

CatalogRightMenu::CatalogRightMenu(dbo::Session &session,dbo::ptr<User> user,WContainerWidget *parent):
WContainerWidget(parent),
m_session(session),
m_user(user)
{
    setStyleClass("right-column");
    WLineEdit *search = new WLineEdit();
    search->setStyleClass("search");
    addWidget(search);
    WPushButton *button = new WPushButton();
    button->setStyleClass("search-button");
    addWidget(button);
    WAnchor *showOrder = new WAnchor("orders/order_detail/",WString::fromUTF8("Предварительный просмотр заказа"));
    showOrder->setStyleClass("preview");
    addWidget(showOrder);
    init();
}

CatalogRightMenu::~CatalogRightMenu()
{
    //dtor
}

void CatalogRightMenu::init()
{
    WContainerWidget *menu = new WContainerWidget();
    int colorIndex = 0;
    menu->setList(true);
    menu->setStyleClass("filters");
    std::string query = "select ref,parent_ref,name,level,(order_id-1)/120+1 page from goods_simple where is_group = true and not_visible = false and level < 4 and  state = 0 order by order_id";
    dbo::Transaction transaction(m_session);
    RightMenuList folders = m_session.query< RightMenuItem >(query);
    for (RightMenuList::const_iterator i = folders.begin(); i != folders.end(); ++i)
    {
        RightMenuItem item = *i;
        std::string id = item.get<0>();
        std::string parent = item.get<1>();
        std::string name = item.get<2>();
        int level = item.get<3>();
        int page = item.get<4>();
        if(++colorIndex > MAXCOLORINDEX)colorIndex = 0;
        m_folderPages[id] = page;
        switch (level)
        {
            case 1:
            {
                WContainerWidget *item = new WContainerWidget();
                item->setStyleClass("category");
                menu->addWidget(item);
                WAnchor *anchor = new WAnchor("",WString::fromUTF8(name.c_str()));
                anchor->setStyleClass("pad fit");
                anchor->widget(0)->setStyleClass("centered");
                item->addWidget(anchor);
                WContainerWidget *subMenu = new WContainerWidget();
                subMenu->setList(true);
                subMenu->setStyleClass("sub");
                subMenu->setObjectName(id);
                item->addWidget(subMenu);
                //item->clicked().connect(*new JSlot("function(o,e){if($(o).hasClass('selected')) {$(o).removeClass('selected');}else{$(o).addClass('selected');}};"));//$('li').removeClass('selected');
                anchor->clicked().connect(*new JSlot("function(o,e){if($(o).parent().hasClass('selected')) {$(o).parent().removeClass('selected');}else{$(o).parent().addClass('selected');}};"));//$('li').removeClass('selected');
                break;
            }

            case 2:
            {
                if(WContainerWidget *item = dynamic_cast<WContainerWidget*>(menu->find(parent)))
                {
                    WAnchor *anchor = new WAnchor("",WString::fromUTF8(name.c_str()));
                    WContainerWidget *subItem = new WContainerWidget();
                    item->addWidget(subItem);
                    subItem->addWidget(anchor);
                    WContainerWidget *slideMenu = new WContainerWidget();
                    slideMenu->setStyleClass("slide-menu");
                    WContainerWidget *label = new WContainerWidget();
                    label->setStyleClass("green-label");
                    slideMenu->addWidget(label);
                    WTemplate *title = new WTemplate(WString::fromUTF8(name.c_str()));
                    title->setStyleClass("title");
                    slideMenu->addWidget(title);
                    subItem->addWidget(slideMenu);
                    WContainerWidget *subMenu = new WContainerWidget();
                    subMenu->setList(true);
                    subMenu->setObjectName(id);
                    subItem->decorationStyle().setBackgroundColor(WColor(colorList[colorIndex].red,colorList[colorIndex].green,colorList[colorIndex].blue));
                    slideMenu->addWidget(subMenu);
                }
                break;
            }


            case 3:
            {
                if(WContainerWidget *item = dynamic_cast<WContainerWidget*>(menu->find(parent)))
                {
                    WContainerWidget *subMenu = new WContainerWidget();
                    WAnchor *anchor = new WAnchor("",WString::fromUTF8(name.c_str()));
                    anchor->setWordWrap(true);
                    anchor->setObjectName(id);
                    subMenu->addWidget(anchor);
                    item->addWidget(subMenu);
                    anchor->clicked().connect(this,&CatalogRightMenu::itemClicked);
                }
                break;
            }

        }

    }

    transaction.commit();
    addWidget(menu);

}

void CatalogRightMenu::itemClicked()
{
    //log("info")<<sender()->objectName();
    m_event.emit(m_folderPages[sender()->objectName()]);
}
