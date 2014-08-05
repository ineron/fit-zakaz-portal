#include "FoldersPanel.h"
#include "SearchPanel.h"
#include "Portal.h"


#include <Wt/WMenuItem>
#include <Wt/WSubMenuItem>
#include <Wt/WMenu>
#include <Wt/WText>
#include <Wt/WAnchor>
#include <Wt/WLabel>
//#include "db/DBUtils.h"

FoldersPanel::FoldersPanel(WContainerWidget *parent): Wt::WContainerWidget(parent),m_currentFolder("00000000-0000-0000-0000-000000000000")
{
    update();
}

FoldersPanel::~FoldersPanel()
{
    //dtor
}



WMenuItem *FoldersPanel::findMenuItem(WMenu *menu,std::string id)
{

    WMenuItem *item = dynamic_cast<WMenuItem*>(menu->find(boost::lexical_cast<std::string>(id)));
    //log("info")<<" item "<<item<<" "<<__LINE__;
    return item;

    std::vector< WMenuItem * >  items = menu->items();
    std::vector< WMenuItem * >::iterator i;
    //WMenuItem *item = 0;
    std::string name = WString("{1}").arg(id).toUTF8().c_str();
    for(i = items.begin(); i!= items.end() ; ++i)
    {

        item = *i;
        ///WSubMenuItem * subMenuItem = dynamic_cast<WSubMenuItem*>(item);
        WMenuItem * subMenuItem = dynamic_cast<WMenuItem*>(item);
        if(subMenuItem)
        {
            //log("info")<<"\E[;35m \033[1m  1 name1 "<<subMenuItem->objectName()<<" = "<<name<<" \033[0m";
            //log("info")<<"\E[;35m \033[1m  object1 = "<<item<<" \033[0m";
            if(subMenuItem->objectName() == name)
                return subMenuItem;
            else
                return findMenuItem(subMenuItem->menu(),id);
                //return findMenuItem(subMenuItem,id);
        }
        else
        {
            if(item->objectName() == name)
            {
                //log("info")<<"\E[;35m \033[1m  2 name1 "<<item->objectName()<<" = "<<name<<" \033[0m";
                //log("info")<<"\E[;35m \033[1m  object2 = "<<item<<" \033[0m";
                item->setStyleClass(WString("active"));
                return item;
            };

        };


    };
    return 0;
}

WMenuItem *FoldersPanel::addMenuItem(const std::string &id,const std::string &parent,const int &page,const int &level,const WString &name,WMenu *catalogFolder)
{

#ifdef DEBUG
    log("info")<<"addMenuItem(id "<<id<<",parent "<<parent<<",page"<<page<<",level "<<level<<",name "<<name<<")";
#endif
    WMenuItem *currenItem=0;
    if(!parent.empty())
        currenItem = findMenuItem(catalogFolder,parent);

    //WMenuItem *mi;
    //log("info")<<"**********  finded objetct " <<currenItem;
    //log("info")<<"**********  level "<<level<<" id "<<id<<" parent "<< parent<<" page "<<page<<" name "<<name;

    /// Если currenItem не 0 тогда проверям является ли он элементом подменю
    /// Если нет то создаем подменю и добавляем новый элемент
    /// При этом те данные которые были в элементе нужно сохранить в новом под меню



    if(currenItem) /// Добавляем к имеющейся папке
    {
        //log("info")<<"\E[;31m \033[1m ***** WSubMenuItem "<< dynamic_cast<WSubMenuItem*>(currenItem) <<" ***** \033[0m ";
        //log("info")<<"\E[;31m \033[1m ***** WMenuItem "<< dynamic_cast<WMenuItem*>(currenItem) <<" ***** \033[0m ";
        //log("info")<<"\E[;31m \033[1m ***** WMenu "<< dynamic_cast<WMenu*>(currenItem) <<" ***** \033[0m ";
        WMenu * subMenuItem = dynamic_cast<WMenu*>(currenItem->menu());
        if(subMenuItem){ /// Если элемент уже является подменю тогда мы в него просто добавляем новый элемент.
            //WMenuItem *mi = new WMenuItem(WString(name.c_str(),UTF8),new WText(""));
            //WMenuItem *mi = new WMenuItem(WString().fromUTF8(name.c_str()),new WText(""));
            //WMenuItem *mi = new WMenuItem(name); /// Проблема с UTF8
            WMenuItem *mi = subMenuItem->addItem(name);
            //mi->setLink(WLink(boost::lexical_cast<std::string>(page)));
            mi->setLink(WLink(WLink::InternalPath,"/shop/"+boost::lexical_cast<std::string>(page)));
            WAnchor *anc = dynamic_cast<WAnchor*>(mi->widget(0)) ;
            if(anc)
                if(anc->count())
                {
                    WLabel *lbl = dynamic_cast<WLabel*>(anc->widget(0));
                    if(lbl)
                        lbl->setWordWrap(true);
                }

            mi->setObjectName(boost::lexical_cast<std::string>(id));

            if(id == m_currentFolder)
                mi->setStyleClass(WString("active").arg((int)level));
            /*else
                mi->itemWidget()->setStyleClass(WString("level{1}").arg((int)level));
            */
#ifdef DEBUG
            //log("info")<<"\E[;35m \033[1m  add element to sub menu " << mi->objectName()<< " styleClass ="<< mi->itemWidget()->styleClass()<< "\033[0m  ";//<< ${ACTIVE_EDITOR_LINE};
#endif


        }
        else ///создаем подменю и копируем в него все то что было в элементе меню
        {
            ///*******************************************************************************************
//            WMenu * upMenu = currenItem->menu();
#ifdef DEBUG
            //log("info")<<"\E[;31m \033[1m ***** Copy menu to sub menu ***** \033[0m ";
#endif

            WMenu *subMenuItems = new WMenu(Wt::Vertical);
            //subMenuItems->setStyleClass("nav-pills");
            /// Создаем новое меню и добавлем стандартные настройки
            //WSubMenuItem *subMenu = new WSubMenuItem(currenItem->text(),new WText());
            /// Копируем данные с элемента меню которое переделываем в подменю
            //subMenu->setObjectName(currenItem->objectName());// Идентификатор записи
            //subMenu->setPathComponent(currenItem->pathComponent());// Номер страницы
            //subMenu->itemWidget()->setId(currenItem->id());
            /// Добавляем новое подменю к элементу родительского меню
            //subMenu->setSubMenu(subMenuItems);
            //subMenu->itemWidget()->setInline(true);
            /// Устанавливаем вызов функции при клике наэлемент меню

            subMenuItems->itemSelected().connect(boost::bind(&FoldersPanel::changed, this, subMenuItems));
            /// Меняем тип отресовки с табличного на списочный
            //subMenuItems->setRenderAsList(true);
            //if(dynamic_cast<WAnchor*>(subMenu->itemWidget()))
            //{
            //    ((WAnchor*)subMenu->itemWidget())->setWordWrap(true);
            //}

            if(WContainerWidget *wc = dynamic_cast<WContainerWidget*>(currenItem->itemWidget()))
            {
                wc->setStyleClass(WString("active"));
                /*for (int i=0;i<wc->count();i++)
                {
                    WAnchor *an = dynamic_cast<WAnchor*>(wc->widget(i));
                    if(an)
                    {
                        //an->setWordWrap(true);
                        //an->setStyleClass(WString("active").arg((int)level-1));

                    }
                }*/
            }

            /// Добавляем новый элемент меню
            //WMenuItem *mi = subMenuItems->addItem(WString(name.c_str(),UTF8) ,new WText());
            WMenuItem *mi = subMenuItems->addItem(name);
            //((WAnchor*)mi->itemWidget())->setWordWrap(true);
            //mi->setLink(WLink(boost::lexical_cast<std::string>(page)));
            mi->setLink(WLink(WLink::InternalPath,"/shop/"+boost::lexical_cast<std::string>(page)));
            mi->setObjectName(boost::lexical_cast<std::string>(id));
            //mi->setPathComponent(boost::lexical_cast<std::string>(page));
            //mi->itemWidget()->setId(WString("{1}").arg(idrec).toUTF8());
            //mi->itemWidget()->removeStyleClass("Wt-rr");
            mi->itemWidget()->removeStyleClass("item");


            WAnchor *anc = dynamic_cast<WAnchor*>(mi->widget(0)) ;
            if(anc)
                if(anc->count())
                {
                    WLabel *lbl = dynamic_cast<WLabel*>(anc->widget(0));
                    if(lbl)
                        lbl->setWordWrap(true);
                }


            if(id == m_currentFolder)
                mi->setStyleClass(WString("active").arg((int)level));
            //else
            //    mi->itemWidget()->setStyleClass(WString("level{1}").arg((int)level));
#ifdef DEBUG
            //log("info")<<"\E[;34m \033[1m add new sub menu\033[0m  "<< mi->objectName();
#endif

            ///*******************************************************************************************
            currenItem->setMenu(subMenuItems);
            //if(currenItem)
            //{
            //    currenItem->setMenu(subMenuItems);
                /// Удаляем из ветки найденый элемент (он нам больше не нужен)
                //upMenu->removeItem(currenItem);
                /// Очищаем память удаленого элемента меню
                //delete currenItem;
                //currenItem=0;
            //}
        }


    }else{ /// Создаем новый головной элемент

        //log("info")<<"\E[;31m \033[1m ***** "<<__LINE__<<" ***** \033[0m ";
        //log("info")<<"\E[;31m \033[1m ***** Add zero level element ***** \033[0m ";
        //WMenuItem *mi = new WMenuItem(WString(name.c_str(),UTF8),new WText());
        //WMenuItem *mi =catalogFolder->addItem(WString(name.c_str(),UTF8),new WText());
        WMenuItem *mi =catalogFolder->addItem(name);

        //((WAnchor*)mi->itemWidget())->setWordWrap(true);
        mi->setLink(WLink(WLink::InternalPath,"/shop/"+boost::lexical_cast<std::string>(page)));
        mi->setObjectName(boost::lexical_cast<std::string>(id));
        //mi->setPathComponent("shop/"+boost::lexical_cast<std::string>(page));
        //mi->itemWidget()->setId(WString("{1}").arg(idrec).toUTF8());

        WAnchor *anc = dynamic_cast<WAnchor*>(mi->widget(0)) ;
        if(anc)
            if(anc->count())
            {
                WLabel *lbl = dynamic_cast<WLabel*>(anc->widget(0));
                if(lbl)
                    lbl->setWordWrap(true);
            }



        //mi->itemWidget()->removeStyleClass("Wt-rr");
        //mi->itemWidget()->removeStyleClass("item");
        //mi->itemWidget()->setStyleClass(WString("level{1}").arg((int)level));
#ifdef DEBUG
        //log("info")<<"\E[;36m \033[1m add new element " << mi->objectName()<<"\033[0m "<<__LINE__;
#endif

    };
        //log("info")<<"\E[;36m \033[1m add element "<<currenItem << "\033[0m ";
    return 0;
}


void FoldersPanel::update()
{
    clear();
#ifdef DEBUG
    log("info")<<"updateFoldersPanel()";
    log("info")<<"\E[;31m \033[1m ***** "<<__LINE__<<" ***** \033[0m ";
#endif
    std::stringstream html;
    //WAnchor *menuItem;// = new WAnchor(m_parent);
    WString menuStr;
    //WMenuItem *mi;
    //log("info")<<"\E[;31m \033[1m ***** "<<__LINE__<<" ***** \033[0m ";
//    clearMenu(m_catalogFolder);
    //delete m_catalogFolder;
    //m_catalogFolder = 0;
    //log("info")<<"\E[;31m \033[1m ***** "<<__LINE__<<" ***** \033[0m ";
    //log("info")<<"*** Parent "<<m_parent;
    //if(m_catalogFolder)
    //    delete m_catalogFolder;
    //m_catalogFolder = 0;
    //if(!m_catalogFolder)
    //{
        WMenu *catalogFolder = new WMenu(Vertical,this);
        catalogFolder->itemSelected().connect(boost::bind(&FoldersPanel::changed, this, catalogFolder));
        //catalogFolder->setRenderAsList(true);
        catalogFolder->setObjectName("menuright");
        catalogFolder->setStyleClass("nav-pills nav-stacked");
    //}
#ifdef DEBUG
    log("info")<<"**********  Current folder id "<<m_currentFolder<<" line "<<__LINE__;
#endif

    std::vector<FolderItem> folders = ((Portal *)WApplication::instance())->getSession()->folders(m_currentFolder);
#ifdef DEBUG
    //log("info")<<"\E[;31m \033[1m ***** "<<__LINE__<<" ***** \033[0m ";
#endif
    m_ItemPageMap.clear();
    for (std::vector<FolderItem>::const_iterator i = folders.begin(); i != folders.end(); ++i)
    {
        FolderItem item = *i;
        std::string id = item.get<0>();
        std::string parent = item.get<1>();
        int page = item.get<3>();
        int level = item.get<4>();
        WString strname = WString().fromUTF8(item.get<6>());
        m_ItemPageMap[id] = page;
        addMenuItem(id,parent,page,level,strname,catalogFolder);
    }
#ifdef DEBUG
    //log("info")<<"\E[;31m \033[1m ***** "<<__LINE__<<" ***** \033[0m ";
#endif
//    m_catalogFolder = catalogFolder;



}
void FoldersPanel::setCurrentPage(int page)
{
    log("info")<<"FoldersPanel::setCurrentPage(page "<<page<<") "<<__LINE__;
    m_currentFolder = ((Portal *)WApplication::instance())->getSession()->getIdFromPage(page);
}


void FoldersPanel::changed(WWidget* sender)
{
    if(SearchPanel *search = dynamic_cast<SearchPanel*>(parent()->find("poisk")))
        search->setSearchText("");
    if(WMenu * menu = dynamic_cast<WMenu *>(sender))
    {
        //log("info") << "******** Menu *************";
        std::string folder = menu->currentItem()->objectName().c_str();
        //log("info") << "Current folder = " << folder;
        if(m_currentFolder==folder)
    	    return;
    	m_currentFolder=folder;
        //log("info") << "Current item = " << menu->objectName(); //((WMenu*)sender)->currentItem()->itemWidget()->objectName();
        //m_currentPage = atoi(((WAnchor*)sender)->text().toUTF8().c_str())-1;
        //log("info") << "Current path "<< menu->currentItem()->link().internalPath();
        m_currentPage = m_ItemPageMap[menu->currentItem()->objectName()];
        //log("info") << "Current page " << m_currentPage;
        //m_currentPage = atoi(((WMenu*)sender)->currentItem()->itemWidget()->id().c_str())-1;
        //m_currentPage = atoi(menu->currentItem()->link().internalPath().toUTF8().c_str());

        m_pageChanged.emit(m_currentPage);
        update();
    }

}
