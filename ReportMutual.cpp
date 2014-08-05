#include "ReportMutual.h"

#include "Portal.h"
#include "db/MsSql7.h"
//#include "utils/LineEdit.h"
#include "DocumentDetails.h"
#ifdef GSOAP
    #include "soapClient.h"
#endif



#include <Wt/WDatePicker>
#include <Wt/WLabel>
#include <Wt/WTable>
#include <Wt/WText>
#include <Wt/WLineEdit>
#include <Wt/WStandardItemModel>
#include <Wt/WStandardItem>
#include <Wt/WAnchor>
#include <Wt/WDialog>
#include <Wt/WPushButton>




ReportMutual::ReportMutual(WStandardItemModel *model,WContainerWidget *parent):WContainerWidget(parent),m_pModel(model)//,m_pReport(0)
{
    setId("mutualreport");
    m_pReport = new WTable(this);
    m_pReport->setStyleClass("table table-bordered");
    createReportHeader();
}

ReportMutual::~ReportMutual()
{
    //dtor
}

void ReportMutual::createReportHeader()
{

    m_pReport->clear();
    //report->setCellspacing(0);
    m_pReport->setHeaderCount(1);
    m_pReport->elementAt(0,0)->addWidget(new WText(tr("reports.mutual.header.coll1")));
    m_pReport->elementAt(0,1)->addWidget(new WText(tr("reports.mutual.header.coll2")));
    m_pReport->elementAt(0,2)->addWidget(new WText(tr("reports.mutual.header.coll3")));
    m_pReport->elementAt(0,3)->addWidget(new WText(tr("reports.mutual.header.coll4")));
    m_pReport->elementAt(0,4)->addWidget(new WText(tr("reports.mutual.header.coll5")));

}


void ReportMutual::updateReport()
{


    createReportHeader();
    //log("info")<<"Row count "<<m_pModel->rowCount();
    if(m_pModel->rowCount()<=2)
    {
        m_pReport->elementAt(1,0)->addWidget(new WText(""));
        m_pReport->elementAt(2,0)->addWidget(new WText(""));
        m_pReport->elementAt(3,0)->addWidget(new WText(""));
        //m_pReport->elementAt(4,0)->addWidget(new WText(""));
        //m_pReport->elementAt(5,0)->addWidget(new WText(""));
    }

    //double initial=0,final=0,increase=0,decrease=0;
    WString preDoc;
    for(int i=0; i<m_pModel->rowCount(); i++)
    {
        //m_pReport->rowAt(i+1)->setStyleClass("col_green");
        //log("info")<<"row nuber "<<m_pModel->item(i,1)->text()<<" level "<<m_pModel->item(i,0)->text();

        if(m_pModel->item(i,0)->text().trim() != "1")
        //if(preDoc == m_pModel->item(i,4)->text() ||  i == 0)
        {
            m_pReport->rowAt(i+1)->setStyleClass("col_dark");
            if(m_pModel->item(i,5)->text().empty() && i>1)
                m_pReport->elementAt(i+1,0)->addWidget(new WText(m_pModel->item(i,4)->text()));
            else
                m_pReport->elementAt(i+1,0)->addWidget(new WText(m_pModel->item(i,5)->text()));
        }
        else
        {
            preDoc = m_pModel->item(i,4)->text();
            m_pReport->rowAt(i+1)->setStyleClass("col_green success");
            //log("info")<<asString(m_pModel->item(i,4)->data())<<" "<<__LINE__;
            WAnchor *linc =  new WAnchor("javascript:void(0)",m_pModel->item(i,4)->text());
            linc->setObjectName(m_pModel->item(i,3)->text().toUTF8());
            linc->clicked().connect(boost::bind(&ReportMutual::onClick, this, linc));
            m_pReport->elementAt(i+1,0)->addWidget(linc);

            //initial     = initial   +atof(m_pModel->item(i,6)->text().toUTF8().c_str());
            //increase    = increase  +atof(m_pModel->item(i,7)->text().toUTF8().c_str());
            //decrease    = decrease  +atof(m_pModel->item(i,8)->text().toUTF8().c_str());
            //final       = final     +atof(m_pModel->item(i,9)->text().toUTF8().c_str());

        }

        //m_pReport->elementAt(i+1,0)->addWidget(new WText(m_pModel->item(i,4)->text()));
        //log("info")<<" *6* "<<m_pModel->item(i,6)->text()<<" *7* "<<m_pModel->item(i,7)->text()<<" *8* "<<m_pModel->item(i,8)->text()<<" *9* "<<m_pModel->item(i,9)->text();
        m_pReport->elementAt(i+1,1)->addWidget(new WText(m_pModel->item(i,6)->text()));
        if(atof(m_pModel->item(i,6)->text().toUTF8().c_str()) < 0)
            m_pReport->elementAt(i+1,1)->setStyleClass("text-error");

        m_pReport->elementAt(i+1,2)->addWidget(new WText(m_pModel->item(i,7)->text()));
        if(atof(m_pModel->item(i,7)->text().toUTF8().c_str()) < 0)
            m_pReport->elementAt(i+1,2)->setStyleClass("text-error");


        m_pReport->elementAt(i+1,3)->addWidget(new WText(m_pModel->item(i,8)->text()));
        if(atof(m_pModel->item(i,8)->text().toUTF8().c_str()) < 0)
            m_pReport->elementAt(i+1,3)->setStyleClass("text-error");


        m_pReport->elementAt(i+1,4)->addWidget(new WText(m_pModel->item(i,9)->text()));
        if(atof(m_pModel->item(i,9)->text().toUTF8().c_str()) < 0)
            m_pReport->elementAt(i+1,4)->setStyleClass("text-error");



    }
    //m_pReport->elementAt(1,0)->addWidget(new WText(session_.getUser().));
    //std::cout << "*** "<<initial*0.01<<" *** "<<increase*0.01<<" *** "<<decrease*0.01<<" *** "<<final*0.01<<std::endl;
#ifdef GSOAP
    //MsSql7 *sql = new MsSql7(new WStandardItemModel(0, 0,this));
    //std::string location = boost::any_cast<std::string>(((Portal *)WApplication::instance())->getSession()->userProperty("Регион"));

    //sql->connect(std::string("infotex_"+location).c_str(),"sa","sa");
    //WString userId = ((Portal *)WApplication::instance())->getSession()->userCharId();
    //std::string userId = boost::any_cast<std::string>(((Portal *)WApplication::instance())->getSession()->userProperty("Идентификатор 1С"));
    //if(!sql->execute(WString("SELECT top 6 * FROM Web_getUserProperty('{1}')").arg(userId)))// Сумма просроченная
    //{
    //    WStandardItemModel *model = sql->getModel();
    //    m_pReport->elementAt(1,0)->addWidget(new WText(model->item(1, 0)->text()));
    //}

    WString userName = WString::fromUTF8(boost::any_cast<std::string>(((Portal *)WApplication::instance())->getSession()->userProperty("Наименование")));
    m_pReport->elementAt(1,0)->addWidget(new WText(userName));
    /*
    char number[50];
    sprintf(number,"%.2f",initial);
    m_pReport->rowAt(1)->setStyleClass("col_black");
    m_pReport->elementAt(1,1)->addWidget(new WText(WString("{1}").arg(number)));
    sprintf(number,"%.2f",increase);
    m_pReport->elementAt(1,2)->addWidget(new WText(WString("{1}").arg(number)));
    sprintf(number,"%.2f",decrease);
    m_pReport->elementAt(1,3)->addWidget(new WText(WString("{1}").arg(number)));
    sprintf(number,"%.2f",final);
    m_pReport->elementAt(1,4)->addWidget(new WText(WString("{1}").arg(number)));
    */
    //delete sql;
#endif

}

void ReportMutual::setModel(WStandardItemModel *model)
{
    m_pModel = model;
    updateReport();
}

void ReportMutual::onClick(WWidget *sender)
{
    //log("info")<<" ID "<<sender->id();
    openDocTable(sender->objectName());
}

void ReportMutual::openDocTable(std::string docId)
{
    //SELECT * FROM [Torg_Infotex].[dbo].[Web_getDocTable](docId)
    WDialog dialog(WString("Просмотр данных отгрузки",UTF8));
    dialog.setClosable(true);
    dialog.setMaximumSize(	WLength(50,WLength::Percentage ),	WLength(70,WLength::Percentage));

    std::string location = boost::any_cast<std::string>(((Portal *)WApplication::instance())->getSession()->userProperty("Регион"));
#ifndef GSOAP
    MsSql7 *sql = 0;
    sql = new MsSql7(new WStandardItemModel(0, 0,this));
    sql->connect(std::string("infotex_"+location).c_str(),"sa","sa");
    sql->execute(WString("SELECT * FROM Web_getDocTable('{1}') ORDER BY LineNumber").arg(docId));
    WStandardItemModel *model = sql->getModel();
#else
    SoapClient *client = new SoapClient(location);
    auto result = client->getDocTablel(docId);
    //WStandardItemModel *model = new WStandardItemModel(0, 0,this);
    WStandardItemModel *model = client->vectorToModel(result,new WStandardItemModel(0, 0,this));
#endif
    WCssDecorationStyle &dc = dialog.contents()->decorationStyle();
    dc.setBackgroundColor(WColor("white"));
    DocumentDetails *docdetail = new DocumentDetails(dialog.contents());
    docdetail->setModel(model);
    WContainerWidget *wc = new WContainerWidget(dialog.contents());
    wc->setId("form_div");
    wc->addWidget(docdetail);
    //wc->setOverflow(OverflowAuto);
    //wc->setMaximumSize( WLength::Auto , 400 );
    docdetail->setId("doc_detail");
    dialog.setResizable(false);

    //WPushButton cancell(tr("cancel"), dialog.contents());
    //cancell.clicked().connect(&dialog, &WDialog::reject);
    if (dialog.exec() == WDialog::Accepted)
    {

    }
#ifndef GSOAP
    delete sql;
#else
    delete client;
#endif

}
