#include "NavigationPanel.h"

#include <Wt/WComboBox>
#include <Wt/WMenu>
#include <Wt/WAnchor>
#include <Wt/WStandardItemModel>
#include <Wt/WText>

namespace {
  void addEntry(WAbstractItemModel *model, std::string value) {
    model->insertRows(model->rowCount(), 1);
    model->setData(model->rowCount()-1, 0, boost::any(std::string(value)));
  }
  void addEntry(WAbstractItemModel *model, WString value) {
    model->insertRows(model->rowCount(), 1);
    model->setData(model->rowCount()-1, 0, value);
  }
  void addEntry(WAbstractItemModel *model, int value ,int limit = 0 ,int offset = 0) {
    model->insertRows(model->rowCount(), 1);
    model->setData(model->rowCount()-1, 0, value);
    model->setData(model->rowCount()-1, 1, limit);
    model->setData(model->rowCount()-1, 1, limit);
    model->setData(model->rowCount()-1, 2, offset);
  }

}


NavigationPanel::NavigationPanel(int maxPages,int currentPage,WContainerWidget *parent)
: Wt::WContainerWidget(parent),
       m_pageChanged(this),m_maxPages(maxPages),m_currentPage(currentPage)
{
#ifdef DEBUG
    log("info")<<"********************************";
    log("info")<<"******* NavigationPanel *********"<<maxPages<<" CURRENT "<<currentPage;
#endif
    update();
}

NavigationPanel::~NavigationPanel()
{
    //dtor
}

void NavigationPanel::changed(WWidget* sender)
{
#ifdef DEBUG
    log("info") << "*************************************";
    log("info") << "id = "<<sender->id();
    log("info") << "name = "<<sender->objectName();
#endif
    unsigned m_currentFolder = 0;
    if(dynamic_cast<WComboBox *>(sender))
    {
        m_currentPage = ((WComboBox*)sender)->currentIndex()+1;
        //WApplication::instance()->setInternalPath((WString("/shop/{1}").arg(m_currentPage+1)).toUTF8().c_str());
    }
    else if(dynamic_cast<WMenu *>(sender))
    {
#ifdef DEBUG
        log("info") << "******** Menu *************";
#endif
        unsigned folder = atoi(((WMenu*)sender)->currentItem()->objectName().c_str());
#ifdef DEBUG
        log("info") << "Current folder = " << folder;
#endif
        if(m_currentFolder==folder)
    	    return;
    	m_currentFolder=folder;
#ifdef DEBUG
        log("info") << "Current item = " << ((WMenu*)sender)->objectName(); //((WMenu*)sender)->currentItem()->itemWidget()->objectName();
        //m_currentPage = atoi(((WAnchor*)sender)->text().toUTF8().c_str())-1;
        log("info") << "Current path "<< ((WMenu*)sender)->currentItem()->pathComponent();
#endif
        //m_currentPage = atoi(((WMenu*)sender)->currentItem()->itemWidget()->id().c_str())-1;
        m_currentPage = atoi(((WMenu*)sender)->currentItem()->pathComponent().c_str());
    }
    else{
        //if (sender->id()==)
        if(((WAnchor*)sender)->text() == WString("←",UTF8)){
            m_currentPage--;
        }else if (((WAnchor*)sender)->text() == WString("→",UTF8)){
            m_currentPage++;
        }else
            m_currentPage = atoi(((WAnchor*)sender)->text().toUTF8().c_str());
    }
#ifdef DEBUG
    log("info") << "Current page = "<<m_currentPage;
#endif
    //if (m_currentPage >= 105 )
    //    setStyleClass("catalog_2");
    //else
    //    removeStyleClass("catalog_2");


    m_pageChanged.emit(m_currentPage);
    update();
}

///Создает панель навигации по страницам в верху и внизу
void NavigationPanel::update()
{

    clear();
    setStyleClass("pagination pagination-centered");
    //WContainerWidget *cb_wc = new WContainerWidget();

    WMenuItem *page;
    WMenu *navPages = new WMenu(this);


    WStandardItemModel *item = new WStandardItemModel(0, 1, this);
    for(int i =1;i < m_maxPages + 1;i++)
        addEntry(item, i);

    WComboBox *cbox = new WComboBox();
    cbox->setWidth(50);
    //cbox->setHeight(20);
    //cbox->decorationStyle ()
    //cbox->setMargin(1,Top);
    cbox->setModel(item);
    cbox->setCurrentIndex(m_currentPage-1);
    cbox->changed().connect(boost::bind(&NavigationPanel::changed, this, cbox));
    cbox->setStyleClass("navselect");

    //cb_wc->setWidth(46);
    //cb_wc->setHeight(12);
    //cb_wc->setStyleClass("styled_select");
    //cb_wc->setMargin(5,Left);
    //cb_wc->setMargin(5,Right);




    if(m_currentPage)
    {
        //page = new WAnchor("/shop/next", WString().fromUTF8("←"),this);
        //page->setLink(WLink(WLink::InternalPath,WString("/shop/{1}").arg((int)m_currentPage).toUTF8().c_str()));
        ////page->clicked().connect(boost::bind(&NavigationPanel::changed, this, page));
        //page->setStyleClass("larr");
        //page->show();
        if(m_currentPage>1){
            page = navPages->addItem(WString("/shop/{1}").arg((int)m_currentPage-1).toUTF8(),WString().fromUTF8("←"));
            page->setLink(WLink(WLink::InternalPath,WString("/shop/{1}").arg((int)m_currentPage-1).toUTF8().c_str()));
        }else{
            page = navPages->addItem(WString("/shop/{1}").arg((int)m_currentPage).toUTF8(),WString().fromUTF8("←"));
            page->setLink(WLink(WLink::InternalPath,WString("/shop/{1}").arg((int)m_currentPage).toUTF8().c_str()));
        }
    }

    int i =0;
    if(m_currentPage > 4)
        i = m_currentPage - 5;
    if(m_currentPage > 5)
    {
        //page = new WAnchor("/shop/1", "1",this);
        //page->setLink(WLink(WLink::InternalPath,"/shop/1"));
        ////page->clicked().connect(boost::bind(&NavigationPanel::changed, this, page));
        //page->show();

        //WText *dot_txt = new WText(this);
        //dot_txt->setText("...");
        //dot_txt->setStyleClass("dot");
        //dot_txt->show();
        page = navPages->addItem("/shop/1","1");
        page->setLink(WLink(WLink::InternalPath,"/shop/1"));
        page = navPages->addItem("#","...");
    }

    for(; i< (m_currentPage + 5 > m_maxPages?m_maxPages:m_currentPage + 5);i++)
    {
        if(m_currentPage-1 == i)
        {
            //addWidget(cbox);

            page = navPages->addItem(WString("/shop/{1}").arg((int)i+1).toUTF8().c_str(),WString("{1}").arg((int)i+1).toUTF8());
            if(WContainerWidget *div = dynamic_cast<WContainerWidget*>(page))
            {
                div->addWidget(cbox);
                div->widget(0)->hide();
            }

        }

        else
        {
            //page = new WAnchor(this);
            //page->setText(WString("{1}").arg((int)i+1));
            //page->setLink(WLink(WLink::InternalPath,WString("/shop/{1}").arg((int)i+1).toUTF8().c_str()));
            ////page->clicked().connect(boost::bind(&NavigationPanel::changed, this, page));
            //page->show();
            page = navPages->addItem(WString("/shop/{1}").arg((int)i+1).toUTF8().c_str(),WString("{1}").arg((int)i+1).toUTF8());
            page->setLink(WLink(WLink::InternalPath,WString("/shop/{1}").arg((int)i+1).toUTF8().c_str()));
        }

    }

    if(m_currentPage < m_maxPages-6 && m_maxPages > 6)
    {
        //WText *dot_txt = new WText(this);
        //dot_txt->setText("...");
        //dot_txt->setStyleClass("dot");
        //dot_txt->show();
        page = navPages->addItem("#","...");
    }

    if(m_currentPage < m_maxPages-5 && m_maxPages > 5)
    {
        //page = new WAnchor(WString("/shop/{1}").arg(m_maxPages).toUTF8(), WString("{1}").arg(m_maxPages).toUTF8(),this);
        //page->setLink(WLink(WLink::InternalPath,WString("/shop/{1}").arg(m_maxPages).toUTF8()));
        ////page->clicked().connect(boost::bind(&NavigationPanel::changed, this, page));
        //page->show();
        page = navPages->addItem(WString("/shop/{1}").arg(m_maxPages).toUTF8(),WString("{1}").arg(m_maxPages).toUTF8());
        page->setLink(WLink(WLink::InternalPath,WString("/shop/{1}").arg(m_maxPages).toUTF8()));
    }

    if(m_currentPage < m_maxPages-1 && m_maxPages > 0)
    {
        //page = new WAnchor("/shop/next", WString().fromUTF8("→"),this);
        //page->setLink(WLink(WLink::InternalPath,WString("/shop/{1}").arg((int)m_currentPage+1).toUTF8().c_str()));
        ////page->clicked().connect(boost::bind(&NavigationPanel::changed, this, page));
        //page->setStyleClass("rarr");
        //page->show();
        page = navPages->addItem(WString("/shop/{1}").arg((int)m_currentPage+1).toUTF8(),WString().fromUTF8("→"));
        page->setLink(WLink(WLink::InternalPath,WString("/shop/{1}").arg((int)m_currentPage+1).toUTF8().c_str()));

    }
    //return panel;
}


