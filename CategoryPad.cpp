#include "CategoryPad.h"

#include <Wt/WMenu>
#include <Wt/WMenuItem>
#include <Wt/WAnchor>
#include <Wt/WLabel>
#include <Wt/WStackedWidget>
#include <Wt/WStandardItemModel>
#include <Wt/WMessageBox>
#include <Wt/WText>
#include <Wt/WString>
#include <Wt/WTemplate>

#include <Wt/Dbo/QueryModel>
#include <Wt/Dbo/Session>


CategoryPad::CategoryPad(dbo::Session &session,WContainerWidget *parent):WContainerWidget(parent),session_(session),m_model(0)
{
    setObjectName("fa489a9a-ba8e-d855-0450-8782490e5bf5");
    //setId("fa489a9a-ba8e-d855-0450-8782490e5bf5");
    setStyleClass("filters");
    m_model = new dbo::QueryModel< dbo::ptr<db::GoodsSimple> >();
    CreateButtons();
}

CategoryPad::~CategoryPad()
{
    //dtor
}


void CategoryPad::CreateButtons()
{

    readFolders("00000000-0000-0000-0000-000000000000");

    for(int i=0;i<m_model->rowCount();i++)
    {

        WContainerWidget *m_buttons = new WContainerWidget(this);
        m_buttons->setStyleClass("category");
        WContainerWidget *buttonsPad = new WContainerWidget(m_buttons);
        buttonsPad->setStyleClass("pad fit");
        //WContainerWidget *item = new WContainerWidget(m_buttons);
        std::string tmpString = boost::any_cast<std::string>(m_model->data(m_model->index(i,1)));
        WTemplate * anc = new WTemplate(WString::fromUTF8(tmpString),buttonsPad);

        buttonsPad->clicked().connect(this, &CategoryPad::selectElement);
        //anc->setId(asString(m_model->data(m_model->index(i,0))).toUTF8());
        buttonsPad->setObjectName(boost::lexical_cast<std::string>(i));
        anc->setStyleClass("centered");
        //WLabel *lbl = dynamic_cast<WLabel*>(anc->widget(0));
        //if(anc->widget(0))
        //    anc->widget(0)->setStyleClass("centered");
    }
}

void CategoryPad::readFolders(std::string parent)
{

    //transaction = new dbo::Transaction(((Portal *)WApplication::instance())->getSession()->getSession());
    //Dbo::Session &session = ((Portal *)WApplication::instance())->getSession()->getSession();
    m_model->setQuery(session_.find<db::GoodsSimple>().where("parent_ref = ?").bind(parent));
    //model->addAllFieldsAsColumns();
    m_model->addColumn("ref");
    m_model->addColumn("name");
    m_model->addColumn("file_name");
    m_model->addColumn("order_id");
    m_model->sort(3);
}

void CategoryPad::selectElement()
{
    //int parentRef = sender()->objectName();
    int itemIndex = atoi(sender()->objectName().c_str());
    std::string parentRef = boost::any_cast<std::string>(m_model->data(m_model->index(itemIndex,0)));
    //log("info")<<"parentRef "<<parentRef;
    m_catalogChanged.emit(objectName(),parentRef,0);
}
