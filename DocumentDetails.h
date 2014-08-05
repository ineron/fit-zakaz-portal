#ifndef DOCUMENTDETAILS_H
#define DOCUMENTDETAILS_H

#include <Wt/WContainerWidget>
#include <Wt/WTable>

using namespace Wt;

namespace Wt
{
class WContainerWidget;
class WText;
class WTextArea;
class WLineEdit;
class WComboBox;
class WFormWidget;
class WDatePicker;
}


class DocumentDetails : public WTable
{
public:
    DocumentDetails(WContainerWidget *parent = 0);
    virtual ~DocumentDetails();
    WStandardItemModel *getModel();
    void setModel(WStandardItemModel *model);
protected:
    WStandardItemModel *m_pModel;
private:

    /*!\brief Submit the form.
    */
    void cancel();
    void createUI();
    void updateUi();

    WContainerWidget *feedbackMessages_;

};

#endif // DOCUMENTDETAILS_H
