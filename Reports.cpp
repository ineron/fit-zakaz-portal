#include "Reports.h"
#include "Portal.h"
#include "utils/LineEdit.h"
#include "db/MsSql7.h"
#include "ReportMutual.h"
#include "ReportOrders.h"
#include "ReportDeptStrucure.h"
#include "OrderDetails.h"
#include "OrdersJournal.h"
#include "PrintResource.h"
#ifdef GSOAP
    #include "soapClient.h"
#endif

#include <Wt/WText>
#include <Wt/WLabel>
#include <Wt/WLineEdit>
#include <Wt/WDateEdit>
#include <Wt/WStandardItemModel>
#include <Wt/WStandardItem>
#include <Wt/WTable>
#include <Wt/WAnchor>
#include <Wt/WImage>
#include <Wt/WTemplate>
#include <Wt/WPushButton>

#include <hpdf_types.h>
#include <hpdf.h>
#include <Wt/Render/WPdfRenderer>

 //extern "C" {
 //  HPDF_STATUS HPDF_UseUTFEncodings(HPDF_Doc pdf);
// }

  void
  error_handler (HPDF_STATUS   error_no,
                 HPDF_STATUS   detail_no,
                 void         *user_data)
  {
      /* invoke longjmp() when an error has occurred */
      log("info") <<"ERROR: error_no="<<error_no<<", detail_no="<<detail_no<<"\n";
      //, (unsigned int)error_no,               (int)detail_no);
//      longjmp(env, 1);
  }

Reports::Reports(WContainerWidget *parent, int reportType):WContainerWidget(parent),m_iReportType(reportType),m_pReport(0),m_wcHeaders(0),m_reportContent(0)
{
#ifdef GSOAP
    std::string location = boost::any_cast<std::string>(((Portal *)WApplication::instance())->getSession()->userProperty("Регион"));
    m_soapClient = new SoapClient(location);
#endif
    setWidth(950);
    createReport();
}

Reports::~Reports()
{
#ifdef GSOAP
    delete m_soapClient;
#endif
}

void Reports::updateHeaders()
{
    //log("info")<<"Reports::updateHeaders() "<<__LINE__;
    m_wcHeaders->clear();
    //log("info")<<"Reports::updateHeaders() "<<__LINE__;
    switch(m_iReportType)
    {
    case Mutual:
        new WText(tr("reports.mutual.header"),m_wcHeaders);
        break;
    case DeptStucture:
        new WText(tr("reports.deptstructure.header"),m_wcHeaders);
        //new WLabel("____________________________________________________________________________________", m_wcHeaders );
        //printDiv = new WContainerWidget(this);
        break;
    case Orders:
        new WText(tr("reports.orders.header"),m_wcHeaders);
        //printDiv = new WContainerWidget(this);
        break;
    case GoodsSales:
        new WText(tr("reports.goodssales.header"),m_wcHeaders);
        //printDiv = new WContainerWidget(this);
        break;
    case OrderDetail:
    {
        char number[50];
        if(dbo::ptr<db::OrdersHeader> ord = ((Portal *)WApplication::instance())->getSession()->getCurrentOrder())
        {
            sprintf(number,"%010d",ord->orderId);
        }else if(dbo::ptr<db::OrdersHeader> ord = ((Portal *)WApplication::instance())->getSession()->setCurrentOrder())
        {
            sprintf(number,"%010d",ord->orderId);
        }else{
            sprintf(number,"%010d",0);
        }

        new WText(WString(tr("reports.orderdetails.header")).arg(number),m_wcHeaders);
        break;
    }
    case OrderJournal:
        new WText(tr("reports.orderjournal.header"),m_wcHeaders);
        //printDiv = new WContainerWidget(this);
        break;

    }
    //log("info")<<"Reports::updateHeaders() "<<__LINE__;
    m_printAnc = new WAnchor(new PrintResource(m_pReport), tr("print"),m_wcHeaders);
    m_printAnc->setStyleClass("print");
    //m_printAnc->setId("print");
    m_printAnc->setImage(new WImage("/css/printer.jpg"));
    m_printAnc->setTarget(TargetNewWindow);
    //m_printAnc->setLink(new PrintResource(m_pReport));

}

void Reports::createReport()
{

    m_wcHeaders = new WContainerWidget(this);
    switch(m_iReportType)
    {
    case Mutual:
        new WText(tr("reports.mutual.header"),m_wcHeaders);
        createUi();
        break;
    case DeptStucture:
        new WText(tr("reports.deptstructure.header"),m_wcHeaders);
        //new WLabel("____________________________________________________________________________________", m_wcHeaders );
        //printDiv = new WContainerWidget(this);
        break;
    case Orders:
        new WText(tr("reports.orders.header"),m_wcHeaders);
        //printDiv = new WContainerWidget(this);
        createUi();
        break;
    case GoodsSales:
        new WText(tr("reports.goodssales.header"),m_wcHeaders);
        //printDiv = new WContainerWidget(this);
        createUi();
        break;
    case OrderDetail:
    {
        char number[50];
        if(dbo::ptr<db::OrdersHeader> ord = ((Portal *)WApplication::instance())->getSession()->getCurrentOrder())
        {
            sprintf(number,"%010d",ord->orderId);
        }else if(dbo::ptr<db::OrdersHeader> ord = ((Portal *)WApplication::instance())->getSession()->setCurrentOrder())
        {
            sprintf(number,"%010d",ord->orderId);
        }else{
            sprintf(number,"%010d",0);
        }

        new WText(WString(tr("reports.orderdetails.header")).arg(number),m_wcHeaders);
        //printDiv = new WContainerWidget(this);
        break;
    }
    case OrderJournal:
        new WText(tr("reports.orderjournal.header"),m_wcHeaders);
        //printDiv = new WContainerWidget(this);
        createUi();
        break;

    }

    //WContainerWidget *printDiv = 0;
    //printDiv = new WContainerWidget(m_wcHeaders);
    //printDiv->setMargin(-20,Top);

    //WResource *r = new PdfResource(this); // serializes to a PDF file.
    //a = new WAnchor(r, "PDF version", this);
    //a->setTarget(TargetNewWindow);

    //printDiv->setStyleClass("print");

    m_printAnc = new WAnchor("#", tr("print"),m_wcHeaders);
    m_printAnc->setStyleClass("print");
    //m_printAnc->setId("print");
    m_printAnc->setImage(new WImage("/css/printer.jpg"));
    m_printAnc->setTarget(TargetNewWindow);
    m_reportContent = new WContainerWidget(this);
    prepareReport();
    createFooter();

}

void Reports::createFooter()
{
    WContainerWidget *update_div = new WContainerWidget(this);
    update_div->setId("update_info");
    update_div->setStyleClass("update_info clear_inside");
    WContainerWidget *wc = new WContainerWidget(update_div);
    wc->setStyleClass("update");
    WContainerWidget *wcp = new WContainerWidget(update_div);

    WAnchor *update_kick = new WAnchor("javascript:void(0)", tr("update"),wc);
    //update_kick->setLink(WLink(WLink::InternalPath,"/update"));
    update_kick->show();
    update_kick->setId("update_report");
    update_kick->setStyleClass("debt");
    update_kick->clicked().connect(boost::bind(&Reports::changed, this, update_kick));
    new WText(tr("reports.updatebtn.footer"),wcp);
/*
    <div id="update_info" class="update_info clear_inside">
		<p class="update"><a class="debt" href="/user?mode=debt&amp;force_updates=1">Обновить данные</a></p>
		<p>*Чтоб посмотреть свежие данные на текущий момент, <br> рекомендуется нажать кнопку "Обновить данные"</p>
	</div>
*/
}

void Reports::createUi()
{

    WContainerWidget *dateSelectForm = new WContainerWidget(this);
    dateSelectForm ->setStyleClass("form-inline");
    dateSelectForm ->setId("dateselectform");

    WLabel *label;

    label = new WLabel(tr("reports.begindate"), dateSelectForm );
    m_pBeginDateEdit = new WDateEdit(dateSelectForm );
    //label->setBuddy(m_pBeginDateEdit->lineEdit());
    label->setBuddy(m_pBeginDateEdit);


    //m_pBeginDateEdit->setBottom(WDate(WDate::currentDate().year(), WDate::currentDate().month(), 1));
    m_pBeginDateEdit->setFormat("dd.MM.yyyy");
    m_pBeginDateEdit->setDate(WDate(WDate::currentDate().year(), WDate::currentDate().month(), 1));
    m_pBeginDateEdit->setTop(WDate::currentDate());
    //m_pBeginDateEdit->lineEdit()->validator()->setMandatory(true);
    m_pBeginDateEdit->validator()->setMandatory(true);
    m_pBeginDateEdit->setStyleClass("input-medium");
    //m_pBeginDateEdit->setWidth(120);


    label = new WLabel(tr("reports.enddate"), dateSelectForm );
    m_pEndDateEdit = new WDateEdit(dateSelectForm );
    //label->setBuddy(m_pEndDateEdit->lineEdit());
    label->setBuddy(m_pEndDateEdit);
    //m_pEndDateEdit->setBottom(WDate::currentDate());
    m_pEndDateEdit->setFormat("dd.MM.yyyy");
    m_pEndDateEdit->setDate(WDate::currentDate());
    m_pEndDateEdit->setTop(WDate::currentDate());
    //m_pEndDateEdit->lineEdit()->validator()->setMandatory(true);
    m_pEndDateEdit->validator()->setMandatory(true);
    m_pEndDateEdit->setStyleClass("input-medium");

    /*
    WContainerWidget *corner_kick = new WContainerWidget(dateSelectForm );
    corner_kick->setStyleClass("corner_kick get_filter");
    WTemplate *button = new WTemplate("<ins class=\"i1\"></ins><ins class=\"i2\"></ins><ins class=\"i3\"></ins><ins class=\"i4\"></ins>${anchor}",corner_kick);
    LineEdit *submitBtn = new LineEdit(0,LineEdit::Submit);
    submitBtn->setValueText(tr("reports.filtered"));
    submitBtn->clicked().connect(boost::bind(&Reports::changed, this, submitBtn));
    button->bindWidget("anchor",submitBtn);
    */

    //WTemplate *button = new WTemplate("<ins class=\"i1\"></ins><ins class=\"i2\"></ins><ins class=\"i3\"></ins><ins class=\"i4\"></ins>${anchor}",dateSelectForm);
    WPushButton *anchor = new WPushButton(tr("reports.filtered"),dateSelectForm);
    anchor->clicked().connect(boost::bind(&Reports::changed, this, dateSelectForm));
    //button->bindWidget("anchor",anchor);
    //button->setStyleClass("corner_kick get_filter");


    //(new WLabel(tr("reports.filtered"), corner_kick))->setBuddy(new LineEdit(corner_kick,LineEdit::Submit));

}

void Reports::prepareReport()
{

    //log("info")<<"Reports::prepareReport() "<<__LINE__;
    m_reportContent->clear();
    m_pReport = 0;
    WStandardItemModel *model = 0;
    //WString userId = ((Portal *)WApplication::instance())->getSession()->userCharId();
    std::string userId = boost::any_cast<std::string>(((Portal *)WApplication::instance())->getSession()->userProperty("Идентификатор 1С"));
    std::string userCode = boost::any_cast<std::string>(((Portal *)WApplication::instance())->getSession()->userProperty("Код 1С"));

    MsSql7 *sql = new MsSql7(new WStandardItemModel(0, 0,this));

    std::string location = boost::any_cast<std::string>(((Portal *)WApplication::instance())->getSession()->userProperty("Регион"));

#ifndef GSOAP
#endif
    //updateHeaders();
    //log("info")<<"Reports::prepareReport() "<<__LINE__;
    switch(m_iReportType)
    {
    case Mutual:
    {
        //SELECT * FROM [Torg_Infotex].[dbo].[Web_getUserTransactions]('   2PV', '110101', '121010',100, 0)
#ifndef GSOAP
        sql->connect(std::string("infotex_"+location).c_str(),"sa","sa");
        sql->execute(WString("SELECT * FROM Web_getUserTransactions('{1}', '{2}', '{3}', {4}, {5})").arg(userId).arg(m_pBeginDateEdit->date().toString("yyMMdd")).arg(m_pEndDateEdit->date().toString("yyMMdd")).arg(3000).arg(0));
        model = sql->getModel();
        //log("info")<<"Create report User ID "<<userId<<" Row count "<<model->rowCount();
#else
        model = new WStandardItemModel(0, 0,this);
        std::vector<std::vector<WString> > result;
        //result = m_soapClient->getMutualItog(m_pBeginDateEdit->date(),m_pEndDateEdit->date(),userCode);
        //model = m_soapClient->vectorToModel(result,model);
        result = m_soapClient->getMutual(m_pBeginDateEdit->date(),m_pEndDateEdit->date(),userCode);
        model = m_soapClient->vectorToModel(result,model);

#endif
        if(!m_pReport)
            m_pReport = new ReportMutual(model,m_reportContent);
        else
            ((ReportMutual*)m_pReport)->setModel(model);

        ((ReportMutual*)m_pReport)->updateReport();
        break;
    }
    case DeptStucture:
    {
#ifndef GSOAP
        //SELECT * FROM [Torg_Infotex].[dbo].[Web_getDeptStructure]('   2PV')
        sql->connect(std::string("infotex_"+location).c_str(),"sa","sa");
        sql->execute(WString("SELECT * FROM Web_getDeptStructure('{1}')").arg(userId));
        model = sql->getModel();
        //log("info")<<"Row count "<<model->rowCount();
#else
        model = new WStandardItemModel(0, 0,this);
        std::vector<std::vector<WString> > result;
        result = m_soapClient->getDeptStucture(userCode);
        model = m_soapClient->vectorToModel(result,model);
#endif

        if(!m_pReport)
            m_pReport = new ReportDeptStrucure(model,m_reportContent);
        else
        {
            ((ReportDeptStrucure*)m_pReport)->setModel(model);
            ((ReportDeptStrucure*)m_pReport)->updateReport();
        }
        break;
    }
    case Orders:
    {
        //SELECT * FROM [Torg_Infotex].[dbo].[Web_getUserShipping]('   6KT', '110101', '121010',100, 0)
#ifndef GSOAP
        sql->connect(std::string("infotex_"+location).c_str(),"sa","sa");
        sql->execute(WString("SELECT * FROM Web_getOrderShipping('{1}', '{2}', '{3}', {4}, {5})").arg(userId).arg(m_pBeginDateEdit->date().toString("yyMMdd")).arg(m_pEndDateEdit->date().toString("yyMMdd")).arg(3000).arg(0));
        model = sql->getModel();
#else
        model = new WStandardItemModel(0, 0,this);
        std::vector<std::vector<WString> > result;
        result = m_soapClient->getOrdersShipping(m_pBeginDateEdit->date(),m_pEndDateEdit->date(),userCode);
        model = m_soapClient->vectorToModel(result,model);
#endif
        if(!m_pReport)
            m_pReport = new ReportOrders(model,m_reportContent);
        else
            ((ReportOrders*)m_pReport)->setModel(model);

        ((ReportOrders*)m_pReport)->updateReport();
        break;
    }
    case GoodsSales:
    {
        sql->connect(std::string("infotex_"+location).c_str(),"sa","sa");
        sql->execute(WString("SELECT * FROM [Torg_Infotex].[dbo].[Web_getUserTransactions]('{1}', '{2}', '{3}', {4}, {5})").arg(userId).arg(m_pBeginDateEdit->date().toString("yyMMdd")).arg(m_pEndDateEdit->date().toString("yyMMdd")).arg(20).arg(0));
        model = sql->getModel();
        break;
    }
    case OrderDetail:
    {
        bool canOpen = true;
        /*
        if(((Portal *)WApplication::instance())->isTerminalMode())
        {
            canOpen = false;
            if(((Portal *)WApplication::instance())->getAccesStatus())
            {
                canOpen = true;
            }

        }
        */
        if(canOpen)
        {

            std::vector<db::Catalog> orderRecords = ((Portal *)WApplication::instance())->getSession()->getOrderRecords();
            if(!m_pReport)
                m_pReport = new OrderDetails(orderRecords,m_reportContent);
            else
                ((OrderDetails*)m_pReport)->setModel(orderRecords);
            ((OrderDetails*)m_pReport)->updateReport();
        }
        setObjectName("OrderDetail");
        break;
    }
    case OrderJournal:
    {
        WStandardItemModel *model = new WStandardItemModel(0,10,this);
        ((Portal *)WApplication::instance())->getSession()->getOrdersJournal(model,m_pBeginDateEdit->date(),m_pEndDateEdit->date());

        m_pReport = new OrdersJournal(model,m_reportContent);
        if(!m_pReport)
            m_pReport = new OrdersJournal(model,m_reportContent);
        else
        {
            ((OrdersJournal*)m_pReport)->setModel(model);
            ((OrdersJournal*)m_pReport)->updateReport();
        }
        break;
        setObjectName("OrderJournal");
    }
    }
    //log("info")<<"Reports::prepareReport() "<<__LINE__;

    if(m_pReport)
    {
        try
        {
            //log("info")<<"1 "<<__LINE__;
            m_printAnc->setLink(new PrintResource(m_pReport));
            //log("info")<<"2 "<<__LINE__;
        }
        catch(std::exception &e)
        {
            log("info") << "exception: " << e.what();
        }
    }


        //m_printAnc->setLink(WLink(new PrintResource(m_pReport)));
    //log("info")<<"Reports::prepareReport() "<<__LINE__;

    delete sql;
}

WResource *Reports::printReport()
{
    //HPDF_Doc pdf = HPDF_New(error_handler, 0);
    //HPDF_UseUTFEncodings(pdf); // enables UTF-8 encoding with true type fonts
    //HPDF_Page page = HPDF_AddPage(pdf);
    //HPDF_Page_SetSize(page, HPDF_PAGE_SIZE_A4, HPDF_PAGE_PORTRAIT);

    //Render::WPdfRenderer renderer(pdf, page);
    //renderer.setMargin(2.54);
    //renderer.setDpi(96);

    //renderer.render("<p style=\"background-color: #c11\">Hello, world !</p>");
    //std::ostringstream htmlStream;// = std::cout;
    //htmlStream << "";
    //m_pReport->htmlText(htmlStream);
    //mi->contents()->htmlText(htmlStream);
    //std::string htmlStr= "";
    //htmlStr << htmlStream;
    //std::cout <<htmlStream<<std::endl;
    //renderer.render(WString().fromUTF8(htmlStream.str()));


    //return new WPdfImage(pdf,page,0,0,renderer.pageWidth(0),renderer.pageHeight(0));
    //HPDF_SaveToFile(pdf, "hello.pdf");
    //HPDF_Free(pdf);
    return new PrintResource(m_pReport);

}

void Reports::changed(WWidget* sender)
{
    //log("info")<<"Reports::changed(WWidget* sender) "<<__LINE__;
    //if(sender->id() == "print")
    //{
        //log("info")<<"Print page ";
        //new PrintResource(m_pReport);
    //}else{
        prepareReport();
        updateHeaders();
    //}
    //log("info")<<"Reports::changed(WWidget* sender) "<<__LINE__;

}

void Reports::update()
{
    prepareReport();
    updateHeaders();
}
