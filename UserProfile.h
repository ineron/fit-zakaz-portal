#ifndef USERPROFILE_H
#define USERPROFILE_H

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

class UserProfile : public WTable
{
public:
    UserProfile(WContainerWidget *parent = 0);
    virtual ~UserProfile();
    WStandardItemModel *getModel();
    void setModel(WStandardItemModel *model);

protected:
    WStandardItemModel *m_pModel;
private:
    void countryChanged();

    /*!\brief Submit the form.
    */
    void submit();
    void cancel();
    void createUI();
    void updateUi();

    WContainerWidget *feedbackMessages_;

    WLineEdit *nameEdit_;
    WLineEdit *fullName_;

    WComboBox *countryEdit_;
    WComboBox *cityEdit_;

    WDatePicker *birthDateEdit_;
    WLineEdit *phoneEdit_;
    WLineEdit *addresEdit_;
    WLineEdit *weightEdit_;

    WTextArea *remarksEdit_;

    /*!\brief Add a validation feedback for a field
    */
    void addValidationStatus(int row, WFormWidget *field);

    /*!\brief Validate the form, and return whether succesfull.
    */
    bool validate();

    /*!\brief Validate a single form field.
    *
    * Checks the given field, and appends the given text to the error
    * messages on problems.
    */
    bool checkValid(WFormWidget *edit, const WString& text);

};

#endif // USERPROFILE_H
