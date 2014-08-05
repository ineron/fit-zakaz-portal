#ifndef REPORTS_H
#define REPORTS_H

#include <Wt/WContainerWidget>
#include "Session.h"

using namespace Wt;

#ifdef GSOAP
class SoapClient;
#endif

class Reports : public WContainerWidget
{
public:
    Reports(WContainerWidget *parent, int reportType);
    virtual ~Reports();
    enum ReportsType {Mutual,DeptStucture,Orders,GoodsSales,OrderDetail,OrderJournal};
    void update();
protected:
    void changed(WWidget* sender);
    int m_iReportType;
    WDateEdit *m_pBeginDateEdit;
    WDateEdit *m_pEndDateEdit;
    WContainerWidget *m_pReport;
    WAnchor *m_printAnc;

private:
    void createUi();
    void createReport();
    void prepareReport();
    void createFooter();
    void updateHeaders();
    WResource *printReport();
    WContainerWidget *m_pDateSelectForm;
    WContainerWidget *m_wcHeaders;
    WContainerWidget *m_reportContent;
#ifdef GSOAP
    SoapClient *m_soapClient;
#endif
};

#endif // REPORTS_H
