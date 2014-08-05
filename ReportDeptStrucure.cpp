#include "ReportDeptStrucure.h"

#include <Wt/WDatePicker>
#include <Wt/WLabel>
#include <Wt/WTable>
#include <Wt/WText>
#include <Wt/WLineEdit>
#include <Wt/WStandardItemModel>
#include <Wt/WStandardItem>
#include "utils/LineEdit.h"


ReportDeptStrucure::ReportDeptStrucure(WStandardItemModel *model,WContainerWidget *parent):WContainerWidget(parent),m_pModel(model)//,m_pReport(0)
{
    setObjectName("ordersreport");
    m_pReport = new WTable(this);
    m_pReport->setStyleClass("table table-bordered table-striped");
    //createReportHeader();
    updateReport();
}

ReportDeptStrucure::~ReportDeptStrucure()
{
    //dtor
}

void ReportDeptStrucure::createReportHeader()
{

    //report->setCellspacing(0);
    m_pReport->setHeaderCount(1);
    m_pReport->elementAt(0,0)->addWidget(new WText(tr("reports.deptstucture.header.coll1")));
    m_pReport->elementAt(0,1)->setColumnSpan(2);
    m_pReport->elementAt(0,1)->addWidget(new WText(tr("reports.deptstucture.header.coll2")));
    m_pReport->elementAt(0,2)->addWidget(new WText());
    m_pReport->elementAt(0,3)->setColumnSpan(2);
    m_pReport->elementAt(0,3)->addWidget(new WText(tr("reports.deptstucture.header.coll3")));
    m_pReport->elementAt(0,4)->addWidget(new WText());
    m_pReport->elementAt(0,5)->addWidget(new WText(tr("reports.deptstucture.header.coll4")));

}

void ReportDeptStrucure::updateReport()
{

    m_pReport->clear();
    createReportHeader();
    //log("info")<<"Row count "<<m_pModel->rowCount();

    double overdue=0,expected=0,total=0;
#ifndef GSOAP
    for(int i=0; i<m_pModel->rowCount(); i++)
    {
        ///     0       1       2           3       4       5         6     7           8           9           10          11            12
        ///НомерСтроки Фирма КредДокумент Документ ДатаДок ВсегоВал Всего Просрочено Ожидается ДатаОплаты ДнейПросрочки ВсегоЗаписей ДатаОплатыСтр
        // Документ
        if(m_pModel->item(i,3)->text() != "\0")
        {
            if(m_pModel->item(i,12)->text() != "01.01.1900")
            {
                WText *paydate = new WText(tr("paydate")+m_pModel->item(i,12)->text()+"<br/>");
                paydate->setStyleClass("green");
                m_pReport->elementAt(i+2,0)->addWidget(paydate);
            }
            m_pReport->elementAt(i+2,0)->addWidget(new WText(m_pModel->item(i,3)->text()));
        }

        int losdays = atoi(m_pModel->item(i,10)->text().toUTF8().c_str());
        //log("info")<<"losdays "<<losdays;
        // Дней просрочки
        if(losdays > 0)
        {
            m_pReport->elementAt(i+2,1)->addWidget(new WText(WString("{1}").arg(losdays)));
            m_pReport->elementAt(i+2,1)->setStyleClass("red");
        }

        else if(losdays < 0) // Ожидание
            m_pReport->elementAt(i+2,3)->addWidget(new WText(WString("{1}").arg(-losdays)));


        // Сумма просрочки
        if(m_pModel->item(i,7)->text() != "\0" && losdays > 0)
            if(losdays > 0)
            {
                m_pReport->elementAt(i+2,2)->addWidget(new WText(m_pModel->item(i,7)->text()));
                m_pReport->elementAt(i+2,2)->setStyleClass("red");
            }


        // Сумма Ожидается
        if(m_pModel->item(i,8)->text() != "\0" && atoi(m_pModel->item(i,8)->text().toUTF8().c_str()))
            m_pReport->elementAt(i+2,4)->addWidget(new WText(m_pModel->item(i,8)->text()));

        // Сумма всего
        if(m_pModel->item(i,6)->text() != "\0")
            m_pReport->elementAt(i+2,5)->addWidget(new WText(m_pModel->item(i,6)->text()));

        overdue     = overdue   +atof(m_pModel->item(i,7)->text().toUTF8().c_str());
        expected    = expected  +atof(m_pModel->item(i,8)->text().toUTF8().c_str());
        total       = total     +atof(m_pModel->item(i,6)->text().toUTF8().c_str());


        m_pReport->rowAt(i+2)->setStyleClass("col_dark");
    }
#else
    for(int i=0; i<m_pModel->rowCount(); i++)
    {

        m_pReport->elementAt(i+2,0)->addWidget(new WText(m_pModel->item(i,2)->text()));

        int losdays = atoi(m_pModel->item(i,3)->text().toUTF8().c_str());
        //log("info")<<"losdays "<<losdays;
        // Дней просрочки
        m_pReport->elementAt(i+2,1)->addWidget(new WText(m_pModel->item(i,3)->text()));

        m_pReport->elementAt(i+2,3)->addWidget(new WText(m_pModel->item(i,5)->text()));


        // Сумма просрочки
        m_pReport->elementAt(i+2,2)->addWidget(new WText(m_pModel->item(i,4)->text()));


        // Сумма Ожидается
        m_pReport->elementAt(i+2,4)->addWidget(new WText(m_pModel->item(i,6)->text()));

        // Сумма всего
        m_pReport->elementAt(i+2,5)->addWidget(new WText(m_pModel->item(i,7)->text()));

        overdue     = overdue   +atof(m_pModel->item(i,4)->text().toUTF8().c_str());
        expected    = expected  +atof(m_pModel->item(i,6)->text().toUTF8().c_str());
        total       = total     +atof(m_pModel->item(i,7)->text().toUTF8().c_str());


        m_pReport->rowAt(i+2)->setStyleClass("col_dark");
    }
#endif //GSOAP

    //m_pReport->rowAt(1)->setStyleClass("col_dark");
    char number[50];
    sprintf(number,"%.2f",overdue);
    m_pReport->rowAt(1)->setStyleClass("col_black");
    m_pReport->elementAt(1,2)->addWidget(new WText(WString("{1}").arg(number)));
    sprintf(number,"%.2f",expected);
    m_pReport->elementAt(1,4)->addWidget(new WText(WString("{1}").arg(number)));
    sprintf(number,"%.2f",total);
    m_pReport->elementAt(1,5)->addWidget(new WText(WString("{1}").arg(number)));

}

void ReportDeptStrucure::setModel(WStandardItemModel *model)
{
    m_pModel = model;
    updateReport();
}


