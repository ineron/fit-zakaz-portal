#ifndef PRICEEDITDIALOG_H
#define PRICEEDITDIALOG_H

#include <Wt/WDialog>
#include <Wt/WModelIndex>

using namespace Wt;

//class WModelIndex;

class PriceEditDialog : public Wt::WDialog
{
public:
    PriceEditDialog(WAbstractItemModel *model, const WModelIndex& item);
    virtual ~PriceEditDialog();
protected:
private:
    enum PriceType {Dynamic = 1,Static = 2};
    void update();
    void handleFinish(DialogCode result);
    void onCalcTypeChanged();

    std::string         m_id;
    WAbstractItemModel  *m_model;
    WModelIndex         m_item;

    WCheckBox           *m_chbPercentEnabled;
    WCheckBox           *m_chbCostEnabled;
    WCheckBox           *m_chbModifyStatic;
    WRadioButton        *m_rbDynamicCalc;
    WRadioButton        *m_rbStaticCalc;
    WButtonGroup        *m_rbGroup;
    WLineEdit           *m_lePercent;
    WLineEdit           *m_leCost;
    WDatePicker         *m_dpModified;
    WComboBox           *m_roundingMethod;
};

#endif // PRICEEDITDIALOG_H
