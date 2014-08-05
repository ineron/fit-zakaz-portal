#include "Portal.h"
#include "ReportOrders.h"
#include "utils/LineEdit.h"
#include "DocumentDetails.h"
#include "db/MsSql7.h"
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
#include <Wt/WScrollArea>
#include <Wt/WVBoxLayout>
#include <Wt/WCssDecorationStyle>



ReportOrders::ReportOrders(WStandardItemModel *model,WContainerWidget *parent):WContainerWidget(parent),m_pModel(model)//,m_pReport(0)
{
    //setId("ordersreport");
    m_pReport = new WTable(this);
    m_pReport->setStyleClass("table table-bordered table-striped");
    createReportHeader();
}

ReportOrders::~ReportOrders()
{
    //dtor
}

void ReportOrders::createReportHeader()
{

    //report->setCellspacing(0);
    m_pReport->setHeaderCount(1);
    m_pReport->elementAt(0,0)->addWidget(new WText(tr("reports.orders.header.coll1")));
    m_pReport->elementAt(0,1)->addWidget(new WText(tr("reports.orders.header.coll2")));
    m_pReport->elementAt(0,2)->addWidget(new WText(tr("reports.orders.header.coll3")));
    m_pReport->elementAt(0,3)->addWidget(new WText(tr("reports.orders.header.coll4")));
    m_pReport->elementAt(0,4)->addWidget(new WText(tr("reports.orders.header.coll5")));
    m_pReport->elementAt(0,5)->addWidget(new WText(tr("reports.orders.header.coll6")));
    m_pReport->elementAt(0,6)->addWidget(new WText(tr("reports.orders.header.coll7")));

}

void ReportOrders::updateReport()
{

    //log("info")<<"updateReport() "<<__LINE__;
    m_pReport->clear();
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

    for(int i=0;i<m_pModel->rowCount();i++)
    {
#ifndef GSOAP
        if(m_pModel->item(i,2)->text() == "1")
        {
            m_pReport->elementAt(i+1,0)->addWidget(new WText(WString("Проведен",UTF8)));
            m_pReport->elementAt(i+1,0)->widget(0)->setStyleClass("held");
            // Дата документа
            if(m_pModel->item(i,8)->text() != "\0")
                m_pReport->elementAt(i+1,4)->addWidget(new WText(m_pModel->item(i,8)->text()));

            // Номер документа
            if(m_pModel->item(i,5)->text() != "\0")
            {
                //m_pReport->elementAt(i+1,5)->addWidget(new WText(m_pModel->item(i,4)->text()));
                WAnchor *linc =  new WAnchor("javascript: void(0)",m_pModel->item(i,5)->text());
                linc->setObjectName(m_pModel->item(i,1)->text().toUTF8());
                linc->clicked().connect(boost::bind(&ReportOrders::onClick, this, linc));
                m_pReport->elementAt(i+1,1)->addWidget(linc);
            }

            // Сумма документа
            if(m_pModel->item(i,3)->text() != "\0")
                m_pReport->elementAt(i+1,2)->addWidget(new WText(m_pModel->item(i,3)->text()));

            // Комментарий
            if(m_pModel->item(i,6)->text() != "\0")
                m_pReport->elementAt(i+1,3)->addWidget(new WText(m_pModel->item(i,6)->text()));



        }else{
            // Дата документа
            if(m_pModel->item(i,8)->text() != "\0")
                m_pReport->elementAt(i+1,4)->addWidget(new WText(m_pModel->item(i,8)->text()));

            // Номер документа
            if(m_pModel->item(i,5)->text() != "\0")
            {
                //m_pReport->elementAt(i+1,5)->addWidget(new WText(m_pModel->item(i,4)->text()));
                WAnchor *linc =  new WAnchor("javascript: void(0)",m_pModel->item(i,5)->text());
                linc->setObjectName(m_pModel->item(i,1)->text().toUTF8());
                linc->clicked().connect(boost::bind(&ReportOrders::onClick, this, linc));
                m_pReport->elementAt(i+1,5)->addWidget(linc);
            }

            // Сумма документа
            if(m_pModel->item(i,3)->text() != "\0")
                m_pReport->elementAt(i+1,6)->addWidget(new WText(m_pModel->item(i,3)->text()));
        }
#else
        if(m_pModel->item(i,1)->text() == "1")
        {
            m_pReport->elementAt(i+1,0)->addWidget(new WText(WString::fromUTF8("Проведен")));
            m_pReport->elementAt(i+1,0)->widget(0)->setStyleClass("held");
            // Дата документа
            if(m_pModel->item(i,5)->text() != "\0")
                m_pReport->elementAt(i+1,4)->addWidget(new WText(m_pModel->item(i,5)->text()));
            m_pReport->elementAt(i+1,4)->setWidth(143);

            // Номер документа
            if(m_pModel->item(i,2)->text() != "\0")
            {
                //m_pReport->elementAt(i+1,5)->addWidget(new WText(m_pModel->item(i,4)->text()));
                WAnchor *linc =  new WAnchor("javascript: void(0)",m_pModel->item(i,2)->text());
                linc->setObjectName(m_pModel->item(i,0)->text().toUTF8());
                linc->clicked().connect(boost::bind(&ReportOrders::onClick, this, linc));
                m_pReport->elementAt(i+1,1)->addWidget(linc);
            }

            // Сумма документа
            if(m_pModel->item(i,3)->text() != "\0")
                m_pReport->elementAt(i+1,2)->addWidget(new WText(m_pModel->item(i,3)->text()));
            m_pReport->elementAt(i+1,2)->setContentAlignment(AlignRight);

            // Комментарий
            if(m_pModel->item(i,4)->text() != "\0")
                m_pReport->elementAt(i+1,3)->addWidget(new WText(m_pModel->item(i,4)->text()));



        }else{
            // Дата документа
            if(m_pModel->item(i,5)->text() != "\0")
                m_pReport->elementAt(i+1,4)->addWidget(new WText(m_pModel->item(i,5)->text()));
            m_pReport->elementAt(i+1,4)->setWidth(143);

            // Номер документа
            if(m_pModel->item(i,7)->text() != "\0")
            {
                //m_pReport->elementAt(i+1,5)->addWidget(new WText(m_pModel->item(i,4)->text()));
                WAnchor *linc =  new WAnchor("javascript: void(0)",m_pModel->item(i,7)->text());
                linc->setObjectName(m_pModel->item(i,6)->text().toUTF8());
                linc->clicked().connect(boost::bind(&ReportOrders::onClick, this, linc));
                m_pReport->elementAt(i+1,5)->addWidget(linc);
            }

            // Сумма документа
            if(m_pModel->item(i,8)->text() != "\0")
                m_pReport->elementAt(i+1,6)->addWidget(new WText(m_pModel->item(i,8)->text()));
            m_pReport->elementAt(i+1,6)->setContentAlignment(AlignRight);
        }

#endif

    }
    //log("info")<<"updateReport() "<<__LINE__;
}

void ReportOrders::setModel(WStandardItemModel *model)
{
    m_pModel = model;
    updateReport();
}

void ReportOrders::onClick(WWidget *sender)
{
    //log("info")<<" ID "<<sender->id();
    openDocTable(sender->objectName());
}

void ReportOrders::openDocTable(std::string docId)
{
    //SELECT * FROM [Torg_Infotex].[dbo].[Web_getDocTable](docId)
    WDialog dialog(WString("Просмотр данных отгрузки",UTF8));
    dialog.setClosable(true);
    dialog.setMaximumSize(	WLength(50,WLength::Percentage ),	WLength(400));
    //dialog.resize( WLength::Auto , WLength::Auto );

    std::string location = boost::any_cast<std::string>(((Portal *)WApplication::instance())->getSession()->userProperty("Регион"));

#ifndef GSOAP
    MsSql7 *sql = 0;
    sql = new MsSql7(new WStandardItemModel(0, 0,this));


    sql->connect(std::string("infotex_"+location).c_str(),"sa","sa");
    sql->execute(WString("SELECT * FROM Web_getDocTable('{1}') order by LineNumber").arg(docId));
    WStandardItemModel *model = sql->getModel();
#else
    SoapClient *client = new SoapClient(location);
    auto result = client->getDocTablel(docId);
    WStandardItemModel *model = client->vectorToModel(result,new WStandardItemModel(0, 0,this));

#endif

    DocumentDetails *docdetail = new DocumentDetails();
    docdetail->setModel(model);
    WContainerWidget *wc = new WContainerWidget(dialog.contents());
    //wc->decorationStyle().setBackgroundColor(WColor(238,238,238));
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
