#include "Portal.h"
#include "PriceEditDialog.h"
#include "PriceEditor.h"


#include <Wt/WComboBox>
#include <Wt/WContainerWidget>
#include <Wt/WDatePicker>
#include <Wt/WDateValidator>
#include <Wt/WDialog>
#include <Wt/WEnvironment>
#include <Wt/WIntValidator>
#include <Wt/WItemDelegate>
#include <Wt/WLabel>
#include <Wt/WLineEdit>
#include <Wt/WMessageBox>
#include <Wt/WPushButton>
#include <Wt/WRegExpValidator>
#include <Wt/WGridLayout>
#include <Wt/WPopupMenu>
#include <Wt/WSortFilterProxyModel>
#include <Wt/WStandardItem>
#include <Wt/WStandardItemModel>
#include <Wt/WTableView>
#include <Wt/WTreeView>
#include <Wt/WText>
#include <Wt/WVBoxLayout>
#include <Wt/WButtonGroup>
#include <Wt/WRadioButton>
#include <Wt/WGroupBox>
#include <Wt/WBreak>

#include <boost/lexical_cast.hpp>



PriceEditDialog::PriceEditDialog(WAbstractItemModel *model, const WModelIndex& item)
    : WDialog("Edit..."),
    //m_id(0),
    m_model(model),
    m_item(item)
{
#ifdef DEBUG
    log("info")<<"PriceEditDialog(WAbstractItemModel *model, const WModelIndex& item) "<<__LINE__;
#endif
    update();
}

PriceEditDialog::~PriceEditDialog()
{
    //log("info")<<"~PriceEditDialog() "<<__LINE__;
}

void PriceEditDialog::update()
{


    /// 0 id
    /// 1 group type Группа / Элемент
    /// 2 value
    /// 3 value type Процент / Цена
    /// 4 price type Динамический / Статический
    /// 5 Изменять фиксированные цены
    /// 6 Дата принятия изменений

    //resize(450, WLength::Auto);

    char number[50];
    int modelRow = m_item.row();
    m_id            = boost::any_cast<std::string>(m_model->data(modelRow, 0,UserRole));
    //double price    = boost::any_cast<double>(m_model->data(modelRow, 2,UserRole));
    double value    = boost::any_cast<double>(m_model->data(modelRow, 2,UserRole));
    int valueType   = boost::any_cast<int>(m_model->data(modelRow, 3,UserRole));
    int button      = boost::any_cast<int>(m_model->data(modelRow, 4,UserRole));
    WDate chDate    = boost::any_cast<WDate>(m_model->data(modelRow,6,UserRole));
    int roundIndex  = boost::any_cast<int>(m_model->data(modelRow,7,UserRole));

    log("info")<<asString(m_model->data(modelRow, 5,UserRole))<<"  "<<__LINE__;
    //log("info")<<asString(m_model->data(modelRow, 5,UserRole))<<"  "<<__LINE__;

    std::string parent = boost::any_cast<std::string>(m_model->data(modelRow, 5,UserRole));
    int modifyStatic = 0;

    setWindowTitle(asString(m_model->data(modelRow, 3)));
    WGroupBox *container = new Wt::WGroupBox(WString("Вид расчета цены",UTF8));
    //container->setHeight(65);
    m_chbPercentEnabled = new WCheckBox();
    m_chbCostEnabled  = new WCheckBox();
    m_chbModifyStatic  = new WCheckBox(WString("Изменять статические цены ?:",UTF8));
    m_rbGroup = new WButtonGroup(this);
    m_rbDynamicCalc = new WRadioButton(WString("Динамически:",UTF8),container);
    new WBreak(container);
    m_rbStaticCalc = new WRadioButton(WString("Статически",UTF8),container);

    //boost::any d = m_model->data(modelRow, 2,UserRole);
    //int v = boost::lexical_cast<int>(d);
    m_rbGroup->addButton(m_rbDynamicCalc,Dynamic);
    m_rbGroup->addButton(m_rbStaticCalc,Static);

    if(button == 1)
        m_rbGroup->setCheckedButton(m_rbGroup->button(1));
    else
        m_rbGroup->setCheckedButton(m_rbGroup->button(2));

    m_rbGroup->checkedChanged().connect(this,&PriceEditDialog::onCalcTypeChanged);
    m_chbModifyStatic->setChecked(modifyStatic);

    /// Прицент / Цена

    m_leCost = new WLineEdit("0.00");
    m_lePercent = new WLineEdit("0.00");

    if(valueType == 2)
    {
        sprintf(number,"%.2f",value);
        m_chbPercentEnabled->setChecked();
        m_lePercent->setText(number);
    }
    else if(valueType == 1)
    {
        sprintf(number,"%.2f",value);
        m_chbCostEnabled->setChecked();
        m_leCost->setText(number);

    }

    m_lePercent->setValidator(new WRegExpValidator("[0-9]+\\.[0-9]{2}"));
    m_leCost->setValidator(new WRegExpValidator("[0-9]+\\.[0-9]{2}"));


    // created
    m_dpModified = new WDatePicker();
    m_dpModified->lineEdit()->validator()->setMandatory(true);
    m_dpModified->setFormat(FileModel::dateEditFormat);
    m_dpModified->setDate(chDate);
    m_dpModified->setGlobalPopup (true);
    //WDate d = WDate::fromString(item->text(), FileModel::dateEditFormat);
    //item->setData(boost::any(d), DisplayRole);


    /*
         * Use a grid layout for the labels and fields
         */
    WGridLayout *layout = new WGridLayout();

    WLabel *l;
    int row = 0;

    //layout->setContentsMargins(0,0,0,0);
    //layout->setVerticalSpacing(3);
    layout->addWidget(container, row, 0,3,0);
    layout->setRowStretch(1, 1);

    layout->addWidget(m_chbPercentEnabled, row, 1,AlignRight);
    layout->addWidget(m_lePercent, row, 2);
    layout->addWidget(l = new WLabel(WString("Процент:",UTF8)), row, 3);
    l->setBuddy(m_lePercent);
    ++row;

    layout->addWidget(m_chbCostEnabled, row, 1,AlignRight);
    layout->addWidget(m_leCost, row, 2);
    layout->addWidget(l = new WLabel(WString("Цена:",UTF8)), row, 3);
    l->setBuddy(m_leCost);
    ++row;
    ++row;
    //layout->addWidget(new WBreak(),row,0);
    layout->addWidget(l = new WLabel(WString("Дата принятия изменения:",UTF8)), row, 2,0,4,AlignLeft);
    layout->setColumnStretch(2, 1);
    //++row;
    //layout->addWidget(new WBreak(),row,0);
    ++row;
    layout->addWidget(m_chbModifyStatic, row, 0);
    layout->addWidget(m_dpModified->lineEdit(), row, 2);
    layout->addWidget(m_dpModified, row, 3);
    l->setBuddy(m_dpModified->lineEdit());
    ++row;

    WLabel* roundingMethodLabel = new WLabel(WString::fromUTF8("Метод округления:"));
    m_roundingMethod = ((Portal *)WApplication::instance())->getSession()->getRoundList(new WComboBox());
    roundingMethodLabel->setBuddy(m_roundingMethod) ;
    layout->addWidget(roundingMethodLabel, row, 0);
    m_roundingMethod->setCurrentIndex(roundIndex-1);
    layout->addWidget(m_roundingMethod, row, 2);

    //roundingMethod->setModel(model);


    WPushButton *b;
    //WContainerWidget *buttons = new WContainerWidget();
    footer()->addWidget(b = new WPushButton(tr("save")));
    b->clicked().connect(this, &WDialog::accept);
    b->setDefault(true);
    contents()->enterPressed().connect(this, &WDialog::accept);
    footer()->addWidget(b = new WPushButton(tr("cancel")));
    b->clicked().connect(this, &WDialog::reject);

    //layout->addWidget(buttons, row, 0, 0, 4, AlignCenter);
    layout->setColumnStretch(1, 1);

    contents()->setLayout(layout);

    finished().connect(this, &PriceEditDialog::handleFinish);

    show();

}

void PriceEditDialog::handleFinish(DialogCode result)
{
    if (result == WDialog::Accepted)
    {
        /*
         * Update the model with data from the edit widgets.
         *
         * You will want to do some validation here...
         *
         * Note that we directly update the source model to avoid
         * problems caused by the dynamic sorting of the proxy model,
         * which reorders row numbers, and would cause us to switch to editing
         * the wrong data.
         */

        if(m_chbCostEnabled->isChecked() && m_chbPercentEnabled->isChecked())
        {
            WMessageBox::show("Error", WString(tr("msg.ambiguous.select")), Ok);
            return;
        }


        /*! При выставленном варианте расчета статический Поле цена не доступна
            1) Вариант расчета 0 Динамический  1 Статический
            2) Устанавливается Цена или процент
            3) Процента или Цена

        */
        WAbstractItemModel *m = m_model;
        //int id =
        int modelRow = m_item.row();
        int valueType = 0;
        int modifyStatic = 0;
        //int staticValue = 0;
        double value = 0.00;
        //char number[50];

        WAbstractProxyModel *proxyModel = dynamic_cast<WAbstractProxyModel *>(m);
        if (proxyModel)
        {
            m = proxyModel->sourceModel();
            modelRow = proxyModel->mapToSource(m_item).row();
        }
            //modelRow++;



        if(m_chbPercentEnabled->isChecked())
        {
            valueType = 2;
            //sprintf(number,"%.2f%%",boost::lexical_cast<double>(m_lePercent->text().toUTF8()));
            //m->setData(modelRow, 3, boost::any(std::string("0.00")));
            m->setData(modelRow, 5, boost::any(m_lePercent->text().toUTF8()+"%"));/// Процент
            m->setData(modelRow, 2, boost::any(value = boost::lexical_cast<double>(m_lePercent->text().toUTF8())),UserRole);/// Процент

        }else if(m_chbCostEnabled->isChecked())
        {
            valueType = 1;
            //m->setData(modelRow, 3, boost::any(boost::lexical_cast<double>(m_leCost->text().toUTF8())));/// Цена
            //sprintf(number,"%.2f%%",boost::lexical_cast<double>(m_leCost->text().toUTF8()));
            //m->setData(modelRow, 2, boost::any(std::string("0.00%")));
            m->setData(modelRow, 6, boost::any(m_leCost->text().toUTF8()));/// Цена
            m->setData(modelRow, 2, boost::any(value = boost::lexical_cast<double>(m_leCost->text().toUTF8())),UserRole);/// Цена
        }else{
            if(atof(m_leCost->text().toUTF8().c_str()) || atof(m_lePercent->text().toUTF8().c_str()))
                WMessageBox::show("Error", WString(tr("msg.notselected.valuetype")), Ok);

        }

        if(m_chbModifyStatic->isChecked())
            modifyStatic = 1;
        //log("info")<<" Percetn "<<valueType<<" Dinamic "<<m_rbGroup->checkedId();
        m->setData(modelRow, 3, boost::any(valueType),UserRole); /// Value type Процент / Цена
        m->setData(modelRow, 4, boost::any(m_rbGroup->checkedId()),UserRole); /// PriceType Динамический статический
        m->setData(modelRow, 5, boost::any(modifyStatic),UserRole); /// Изменять фиксированные цены
        m->setData(modelRow, 6, boost::any(m_dpModified->date()),UserRole);
        m->setData(modelRow, 7, boost::any(m_roundingMethod->currentIndex()+1),UserRole);

        int priceType = ((m_rbGroup->checkedId()-1)<<1) | (valueType-1);
        bool changeFixed = false;
        WDate modifiDate = WDate::currentDate();

#ifdef DEBUG
        log("info")<<"updateUserPrice(m_id "<<m_id<<",valueType "<<valueType<<",value "<<value<<",priceType "<<priceType<<",changeFixed "<<changeFixed<<",modifiDate "<<modifiDate.toString()<<" rounding "<<m_roundingMethod->currentIndex()+1<<" ) "<<__LINE__;
#endif

        ((Portal *)WApplication::instance())->getSession()->updateUserPrice(m_id,valueType,value,priceType,changeFixed,modifiDate,m_roundingMethod->currentIndex()+1);
    }

    delete this;
}

void PriceEditDialog::onCalcTypeChanged()
{
    if(m_rbGroup->checkedId() == Dynamic)
    {
        m_chbCostEnabled->setEnabled(false);
        m_chbCostEnabled->setChecked(false);
        m_leCost->setEnabled (false);
        m_leCost->setText("0.00");
    }else{
        m_chbCostEnabled->setEnabled(true);
        m_leCost->setEnabled (true);
    }
}
