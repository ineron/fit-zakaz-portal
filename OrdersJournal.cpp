#include "OrdersJournal.h"
#include "Reports.h"

#include "Session.h"
#include "Portal.h"
#include "utils/LineEdit.h"
#include "ComboBoxItemDelegate.h"

#include <Wt/WContainerWidget>
#include <Wt/WStackedWidget>
#include <Wt/WAnchor>
#include <Wt/WLink>
#include <Wt/WText>
#include <Wt/WLabel>
#include <Wt/WTable>
#include <Wt/WImage>
#include <Wt/WLineEdit>
//#include <Wt/WIntValidator>
#include <Wt/WLengthValidator>
#include <Wt/WTextEdit>
#include <Wt/WCheckBox>
#include <Wt/WTemplate>
#include <Wt/WDialog>
#include <Wt/WMessageBox>
#include <Wt/WComboBox>

#include <Wt/WStandardItemModel>
#include <Wt/WStringListModel>
#include <Wt/WStandardItem>
#include <Wt/WTableView>

#include <Wt/WRegExpValidator>
#include <Wt/WPushButton>

OrdersJournal::OrdersJournal(WContainerWidget *parent):WContainerWidget(parent),m_OrdersModel(0),m_groupOperations()
{
    m_OrderTables = new WTable(this);
    m_OrderTables->setStyleClass("table table-bordered table-striped");
    m_groupOperations = new WComboBox(this);
    m_groupOperations->addItem(WString::fromUTF8("Удалить документы"));
    m_groupOperations->addItem(WString::fromUTF8("Отправить документы"));
    m_groupOperations->addItem(WString::fromUTF8("Скопировать документы"));
    m_groupOperations->addItem(WString::fromUTF8("Печать документов"));
    m_groupOperations->addItem(WString::fromUTF8("Выгрузить документы в Excell"));
    m_groupOperations->changed().connect(std::bind([=](){
                            switch(m_groupOperations->currentIndex())
                            {
                                case 3:
                                case 4:WMessageBox::show(WString::fromUTF8("Предупреждение !"),WString::fromUTF8("Эта опция пока не работает."),Ok);break;
                            };
                        }));
}

OrdersJournal::OrdersJournal(WStandardItemModel *orders,WContainerWidget *parent):WContainerWidget(parent),m_OrdersModel(0),m_groupOperations()
{
    m_OrderTables = new WTable(this);
    m_OrderTables->setStyleClass("table table-bordered table-striped");
    m_OrdersModel = orders;
    //log("info")<<"****************"<<__LINE__<<"  "<<__FILE__;
    new WText(WString::fromUTF8("Отмеченные: "),this);
    m_groupOperations = new WComboBox(this);
    m_groupOperations->addItem(WString::fromUTF8("Удалить документы"));
    m_groupOperations->addItem(WString::fromUTF8("Отправить документы"));
    m_groupOperations->addItem(WString::fromUTF8("Скопировать документы"));
    m_groupOperations->addItem(WString::fromUTF8("Печать документов"));
    m_groupOperations->addItem(WString::fromUTF8("Выгрузить документы в Excell"));
    m_groupOperations->changed().connect(std::bind([=](){
                            switch(m_groupOperations->currentIndex())
                            {
                                case 3:
                                case 4:WMessageBox::show(WString::fromUTF8("Предупреждение !"),WString::fromUTF8("Эта опция пока не работает."),Ok);break;
                            };
                        }));

    WPushButton *execBtn = new WPushButton(WString::fromUTF8("Выполнить"),this);
    execBtn->clicked().connect(this,&OrdersJournal::onGroupOeration);
    updateReport();

}


OrdersJournal::~OrdersJournal()
{
    //dtor
}

void OrdersJournal::createHeader()
{
    m_OrderTables->clear();
    m_OrderTables->setHeaderCount(1);

    m_OrderTables->elementAt(0,0)->addWidget(new WText());// Статус
    WCheckBox *selectAll = new WCheckBox();
    selectAll->setObjectName("selectall");
    //selectAll->checked().connect(boost::bind(&OrdersJournal::changed, this, selectAll));
    selectAll->checked().connect(std::bind([=](){
        for(int i=1;i<m_OrderTables->rowCount();i++)
        {
            WCheckBox *chb = dynamic_cast<WCheckBox*>(m_OrderTables->elementAt(i,1)->widget(0));
            if(chb)
                chb->setCheckState(selectAll->checkState());
        }
    }));

    //selectAll->unChecked().connect(boost::bind(&OrdersJournal::changed, this, selectAll));
    selectAll->unChecked().connect(std::bind([=](){
        for(int i=1;i<m_OrderTables->rowCount();i++)
        {
            WCheckBox *chb = dynamic_cast<WCheckBox*>(m_OrderTables->elementAt(i,1)->widget(0));
            if(chb)
                chb->setCheckState(selectAll->checkState());
        }
    }));

    m_OrderTables->columnAt(0)->setWidth(24);
    m_OrderTables->columnAt(1)->setWidth(13);
    m_OrderTables->columnAt(2)->setWidth(56);
    m_OrderTables->columnAt(3)->setWidth(81);
    m_OrderTables->columnAt(4)->setWidth(150);

    m_OrderTables->columnAt(8)->setWidth(24);
    m_OrderTables->columnAt(9)->setWidth(24);
    m_OrderTables->columnAt(10)->setWidth(24);

    m_OrderTables->elementAt(0,1)->addWidget(selectAll);// Пометка всех
    m_OrderTables->elementAt(0,2)->addWidget(new WText(tr("orders.journal.header.coll3")));// Дата
    m_OrderTables->elementAt(0,3)->addWidget(new WText(tr("orders.journal.header.coll4")));// Номер
    m_OrderTables->elementAt(0,4)->addWidget(new WText(tr("orders.journal.header.coll5")));// Комментарий
    m_OrderTables->elementAt(0,5)->addWidget(new WText(tr("orders.journal.header.coll6")));// смма
    m_OrderTables->elementAt(0,6)->addWidget(new WText(tr("orders.journal.header.coll7")));// Адрес
    m_OrderTables->elementAt(0,7)->addWidget(new WText(tr("orders.journal.header.coll8")));// изменить
    m_OrderTables->elementAt(0,8)->addWidget(new WText(tr("orders.journal.header.coll9")));// удалить
    m_OrderTables->elementAt(0,9)->addWidget(new WText(tr("orders.journal.header.coll10")));// Печать


}

void OrdersJournal::updateReport(WStandardItemModel *ordersModel)
{
    //log("info")<<"Update report "<<ordersModel->rowCount();
    createHeader();
    WStringListModel *addressList = new WStringListModel;
    ((Portal *)WApplication::instance())->getSession()->getAddressList(addressList);

    for (int row_count=0; row_count < ordersModel->rowCount(); row_count++)
    {

        std::string ref          = asString(ordersModel->data(row_count,0,UserRole)).toUTF8();
        WDateTime orderDate(boost::any_cast<WDateTime>(ordersModel->data(row_count,1,UserRole)));
        int orderId             = boost::any_cast<int>(ordersModel->data(row_count,2,UserRole));
        std::string comments    = boost::any_cast<std::string>(ordersModel->data(row_count,3,UserRole));
        double orderSumm        = boost::any_cast<double>(ordersModel->data(row_count,4,UserRole));
        int state               = boost::any_cast<int>(ordersModel->data(row_count,5,UserRole));
        std::string userNumber  = boost::any_cast<std::string>(ordersModel->data(row_count,6,UserRole));
        std::string userName  = boost::any_cast<std::string>(ordersModel->data(row_count,7,UserRole));
        std::string currentAdress = boost::any_cast<std::string>(ordersModel->data(row_count,8,UserRole));
        OrdersHeaderPtr order = boost::any_cast< OrdersHeaderPtr >(ordersModel->data(row_count,9,UserRole));
        switch (state)
        {
        case db::OrdersHeader::modifed:
                m_OrderTables->elementAt(row_count+1,0)->addWidget(new WImage("/icons/current.png"));// Статус новый
                m_OrderTables->elementAt(row_count+1,0)->setToolTip(WString::fromUTF8("Новый"));
                break;
        case db::OrdersHeader::saved:
                m_OrderTables->elementAt(row_count+1,0)->addWidget(new WImage("/icons/saved.png"));// Статус сохранен
                m_OrderTables->elementAt(row_count+1,0)->setToolTip(WString::fromUTF8("Сохранен"));
                break;
        case db::OrdersHeader::process:
                m_OrderTables->elementAt(row_count+1,0)->addWidget(new WImage("/icons/inprocess.png"));// Статус проведен
                m_OrderTables->elementAt(row_count+1,0)->setToolTip(WString::fromUTF8("В обработке"));
                break;
        case db::OrdersHeader::held:
                m_OrderTables->elementAt(row_count+1,0)->addWidget(new WImage("/icons/held.png"));// Статус проведен
                m_OrderTables->elementAt(row_count+1,0)->setToolTip(WString::fromUTF8("Обработан"));
                break;


        }
        //log("info")<<" Order id "<<orderId;

        m_OrderTables->elementAt(row_count+1,1)->addWidget(new WCheckBox());// Пометка
        m_OrderTables->elementAt(row_count+1,2)->addWidget(new WText(orderDate.toString("dd.MM.yy")));// Дата
        char number[50];
        sprintf(number,"%010d",orderId);
        WAnchor *anc_number = new WAnchor("#",number);
        anc_number->clicked().connect(this, &OrdersJournal::changeNumebr);
        anc_number->setObjectName(number);

        if(!userNumber.empty())
        {
            sprintf(number,"%s",userNumber.c_str());
            anc_number->setText(WString::fromUTF8(userNumber));
        }

        std::string orderNumber = number;
        if(state == db::OrdersHeader::saved ||state == db::OrdersHeader::modifed){
            m_OrderTables->elementAt(row_count+1,3)->addWidget(anc_number); // Номер
            m_OrderTables->elementAt(row_count+1,3)->setToolTip(WString::fromUTF8("Установить Ваш внутренний номер заказа"));
        }else{
            m_OrderTables->elementAt(row_count+1,3)->addWidget(new WText(number)); // Номер
        }
        //new WAnchor(WLink(WLink::InternalPath,"/orders/orders_journal/"),WString::fromUTF8(comments))
        m_OrderTables->elementAt(row_count+1,4)->addWidget(new WTemplate(WString::fromUTF8(comments))); // Комментарий

        sprintf(number,"%.2f",orderSumm);
        m_OrderTables->elementAt(row_count+1,5)->addWidget(new WText(number)); // Сумма
        m_OrderTables->elementAt(row_count+1,5)->setContentAlignment(Wt::AlignTop | Wt::AlignRight);

        sprintf(number,"%010d",orderId);

        /// Адрес доставки
        if(state == db::OrdersHeader::saved ||state == db::OrdersHeader::modifed)
        {
            WComboBox *address = new WComboBox();
            //int currentIndex = 0;
            //WStringListModel localAddressList = addressList;
            //WStringListModel *listModel = new WStringListModel(&localAddressList);
            //((Portal *)WApplication::instance())->getSession()->getAddressList(new WStringListModel(),currentIndex);
            address->setModel(addressList);
            address->setCurrentIndex(address->findText(WString::fromUTF8(currentAdress)));
            address->activated().connect(std::bind([=](){
                int row = address->currentIndex();
                AddressPtr addressRef = boost::any_cast<AddressPtr>(addressList->data(addressList->index(row,0), Wt::UserRole));
                ((Portal *)WApplication::instance())->getSession()->setOrderAddress(addressRef,order);
            }));
            m_OrderTables->elementAt(row_count+1,6)->addWidget(address); // Адрес
        }else{
            m_OrderTables->elementAt(row_count+1,6)->addWidget(new WText(WString::fromUTF8(currentAdress))); // Адрес
        }

        /// Автор документа
        m_OrderTables->elementAt(row_count+1,7)->addWidget(new WText(WString::fromUTF8(userName))); // Автор
        /// Редактировать
        WAnchor *anc_edit = new WAnchor(WLink(WLink::InternalPath,"/orders/current_order"));
        anc_edit->setImage(new WImage("/icons/edit.png"));

        //anc_edit->clicked().connect(boost::bind(&OrdersJournal::changed, this, anc_edit));
        anc_edit->clicked().connect(std::bind([=](){
                ((Portal *)WApplication::instance())->getSession()->setCurrentOrder(ref);
            }));

        //((Portal *)WApplication::instance())->getSession()->setCurrentOrder(boost::lexical_cast<int>(sender->objectName()));
        m_OrderTables->elementAt(row_count+1,8)->addWidget(anc_edit);
        m_OrderTables->elementAt(row_count+1,8)->setToolTip(WString::fromUTF8("Изменить"));
        //anc_edit->setObjectName();

        /// Удалить
        //anc_delete->clicked().connect(boost::bind(&OrdersJournal::onRemove, this, c.ref));
        if(state == db::OrdersHeader::saved ||state == db::OrdersHeader::modifed)
        {
            WAnchor *anc_delete = new WAnchor("javascript:void(0)", "");
            anc_delete->setObjectName(ref);
            anc_delete->setImage(new WImage("/icons/delete.png"));
            //anc_delete->clicked().connect(this,&OrdersJournal::onRemove);
            anc_delete->clicked().connect(std::bind([=]{
                if( WMessageBox::show(WString::fromUTF8("Предупреждение !"),WString::fromUTF8("Удалить документ № {1} ?").arg(orderNumber),Ok | Cancel) == Ok)
                {
                    ((Portal *)WApplication::instance())->getSession()->setCurrentOrder(ref);
                    ((Portal *)WApplication::instance())->getSession()->clearCurrentOrder();
                    ((Portal *)WApplication::instance())->getSession()->setCurrentOrder(0);
                    m_OrderTables->deleteRow(row_count+1);
                }
            }));



            m_OrderTables->elementAt(row_count+1,9)->addWidget(anc_delete); // Удалить
            m_OrderTables->elementAt(row_count+1,9)->setToolTip(WString::fromUTF8("Удалить"));
        }
        else
        {
            m_OrderTables->elementAt(row_count+1,9)->addWidget(new WImage("/icons/delete_desabled.png")); // Удалить
            m_OrderTables->elementAt(row_count+1,9)->setToolTip(WString::fromUTF8("Удалить нельзя"));

        }

        /// Печать
        WAnchor *anc_print = new WAnchor("javascript:void(0)", "");
        anc_print->setImage(new WImage("/icons/print.png"));
        anc_print->setObjectName(ref);
        //anc_print->clicked().connect(boost::bind(&OrdersJournal::onPrint, this, c.ref));
        anc_print->clicked().connect(this,&OrdersJournal::onPrint);
        m_OrderTables->elementAt(row_count+1,10)->addWidget(anc_print); // Печатать документ
        m_OrderTables->elementAt(row_count+1,10)->setToolTip(WString::fromUTF8("Печать документа"));

        m_OrderTables->elementAt(row_count+1,0)->setWidth(24);
        m_OrderTables->elementAt(row_count+1,8)->setWidth(24);
        m_OrderTables->elementAt(row_count+1,9)->setWidth(24);
        m_OrderTables->elementAt(row_count+1,10)->setWidth(24);

    }
}

void OrdersJournal::updateReport()
{
    updateReport(m_OrdersModel);
}

void OrdersJournal::setModel(WStandardItemModel *ordersModel)
{
    m_OrdersModel = ordersModel;
}

void OrdersJournal::onPrint()
{
    //log("info")<<"Printing "<<sender()->objectName();

}

void OrdersJournal::onRemove()
{
    log("info")<<"Removing "<<sender()->objectName();
    if(WTableCell *cell = dynamic_cast<WTableCell*>(sender()->parent()))
    {
        if(WLabel *label = dynamic_cast<WLabel*>(cell->tableRow()->elementAt(3)->widget(0)))
        {
            if( WMessageBox::show(WString::fromUTF8("Предупреждение !"),WString::fromUTF8("Удалить документ № {1} ?").arg(label->text() ),Ok | Cancel) == Ok)
            {
                log("info")<<"Doc number "<<label->text().toUTF8();
                ((Portal *)WApplication::instance())->getSession()->setCurrentOrder(sender()->objectName());
                ((Portal *)WApplication::instance())->getSession()->clearCurrentOrder();
                ((Portal *)WApplication::instance())->getSession()->setCurrentOrder(0);
                cell->table()->deleteRow(cell->row());
            }
        }
    }


}

void OrdersJournal::onGroupOeration()
{
    /**
    0 "Удалить документы"
    1 "Отправить документы"
    2 "Скопировать документы"
    3 "Печать документов"
    4 "Выгрузить документы в Excell"
    */
    // Собирем выделенные документы
    //std::map<std::string,int> docList;
    std::vector<int> docList;
    for(int i=1;i<m_OrderTables->rowCount();i++)
    {
        if(WCheckBox *chb = dynamic_cast<WCheckBox*>(m_OrderTables->elementAt(i,1)->widget(0)))
        {
            if(chb->checkState() == Checked)
            {
                chb->setCheckState(Unchecked);

                //std::string ref         = asString(m_OrdersModel->data(i-1,0,UserRole)).toUTF8();
                //int state               = boost::any_cast<int>(m_OrdersModel->data(i-1,5,UserRole));
                //docList.insert(std::pair<std::string,int>(ref,state));
                docList.push_back(i-1);
            }
        }
    }

    if(docList.size())
    {
        switch(m_groupOperations->currentIndex())
        {
            case 0:removeGroupDoc(docList);break;
            case 1:sendGroupDoc(docList);break;
            case 2:copyGroupDoc(docList);break;
            case 3:printGroupDoc(docList);break;
            case 4:uploadGroupDoc(docList);break;
        }
    }
}

void OrdersJournal::removeGroupDoc(std::vector<int> docList)
{
    if( WMessageBox::show(WString::fromUTF8("Предупреждение !"),WString::fromUTF8("Удалить {1} документов ?").arg((int)docList.size()),Ok | Cancel) != Ok)
        return;

    int rowRemoved = 1;

    for (std::vector<int>::const_iterator it = docList.begin();it != docList.end();++it)
    {
        std::string ref         = asString(m_OrdersModel->data(*it,0,UserRole)).toUTF8();
        int state               = boost::any_cast<int>(m_OrdersModel->data(*it,5,UserRole));

        if(state == db::OrdersHeader::modifed || state == db::OrdersHeader::saved)
        {
            ((Portal *)WApplication::instance())->getSession()->setCurrentOrder(ref);
            ((Portal *)WApplication::instance())->getSession()->clearCurrentOrder();
            m_OrderTables->deleteRow((*it+rowRemoved));
            rowRemoved++;
        }
    }
    ((Portal *)WApplication::instance())->getSession()->setCurrentOrder(0);
    WMessageBox::show(WString::fromUTF8("Предупреждение !"),WString::fromUTF8("Удалено {1} документов ?").arg(rowRemoved-1),Ok);
}

WDialog *OrdersJournal::setOrderAddresGroupDoc(std::vector<int> docList)
{
    /// Номер документ
    /// Комментарий
    /// Поле выбора адреса
    WDialog *dialog = new WDialog();
    WStandardItemModel *adressModel = new WStandardItemModel(0,3);


    int rowHelded = 0;
    for (std::vector<int>::const_iterator it = docList.begin();it != docList.end();++it)
    {
        std::string ref         = asString(m_OrdersModel->data(*it,0,UserRole)).toUTF8();
        int state               = boost::any_cast<int>(m_OrdersModel->data(*it,5,UserRole));
        std::string number         = asString(m_OrdersModel->data(*it,2,UserRole)).toUTF8();
        std::string comment         = asString(m_OrdersModel->data(*it,3,UserRole)).toUTF8();

        if(state == db::OrdersHeader::modifed || state == db::OrdersHeader::saved)
        {
            //AddressPtr addressRef = boost::any_cast<AddressPtr>(m_OrdersModel->data(m_OrdersModel->index(*it,0), Wt::UserRole));
            //((Portal *)WApplication::instance())->getSession()->heldCurrentOrder(addressRef);
            std::vector<WStandardItem*> row;
            row.push_back(new WStandardItem(WString(number)));
            row.push_back(new WStandardItem(WString(comment)));
            row.push_back(new WStandardItem(WString("")));

            row[2]->setFlags(ItemIsEditable);
            adressModel->appendRow(row);
            rowHelded++;
        }
    }
    WContainerWidget *div = new WContainerWidget(dialog->contents());
    WTableView *setAddressTable = new WTableView(div);
    setAddressTable->setModel(adressModel);
    int currentIndex=0;
    WStringListModel *listModel = ((Portal *)WApplication::instance())->getSession()->getAddressList(new WStringListModel(),currentIndex);
    listModel->setHeaderData(0, WString::fromUTF8("Номер"));
    listModel->setHeaderData(1, WString::fromUTF8("Комментарий"));
    listModel->setHeaderData(2, WString::fromUTF8("Адрес доставки"));
    setAddressTable->setItemDelegateForColumn(2, new ComboBoxItemDelegate(listModel));
    setAddressTable->setStyleClass("table table-bordered table-striped");
    setAddressTable->setColumnWidth(0,100);
    setAddressTable->setColumnWidth(1,220);
    setAddressTable->setColumnWidth(2,300);
    setAddressTable->setEditTriggers(WTableView::SingleClicked);
    WPushButton *ok = new WPushButton(tr("ok"), dialog->footer());
    WPushButton *cancel = new WPushButton(tr("cancel"), dialog->footer());
    ok->clicked().connect(dialog, &Wt::WDialog::accept);
    cancel->clicked().connect(dialog, &Wt::WDialog::reject);
    dialog->setClosable(true);
    dialog->rejectWhenEscapePressed();
    dialog->setCaption(tr("set.address.title"));
    return dialog;

}

void OrdersJournal::sendGroupDoc(std::vector<int> docList)
{
    int rowHelded = 0;
    for (std::vector<int>::const_iterator it = docList.begin();it != docList.end();++it)
    {
        std::string ref         = asString(m_OrdersModel->data(*it,0,UserRole)).toUTF8();
        int state               = boost::any_cast<int>(m_OrdersModel->data(*it,5,UserRole));

        if(state == db::OrdersHeader::modifed || state == db::OrdersHeader::saved)
        {
            AddressPtr addressRef = boost::any_cast<AddressPtr>(m_OrdersModel->data(m_OrdersModel->index(*it,0), Wt::UserRole));
            if(addressRef)
            {
                ((Portal *)WApplication::instance())->getSession()->heldCurrentOrder(addressRef);
                rowHelded++;
            }
        }
    }
    ((Portal *)WApplication::instance())->getSession()->setCurrentOrder(0);
    WMessageBox::show(WString::fromUTF8("Предупреждение !"),WString::fromUTF8("Отправлено {1} документов <br/> Проигнорированно документов без адреса {2}").arg(rowHelded).arg((int)docList.size()-rowHelded),Ok);
}

void OrdersJournal::copyGroupDoc(std::vector<int> docList)
{
    if( WMessageBox::show(WString::fromUTF8("Предупреждение !"),WString::fromUTF8("Скопировать {1} документов ?").arg((int)docList.size()),Ok | Cancel) != Ok)
        return;

    int rowCopyed = 1;
    std::string newRef;
    for (std::vector<int>::const_iterator it = docList.begin();it != docList.end();++it)
    {
        std::string ref         = asString(m_OrdersModel->data(*it,0,UserRole)).toUTF8();
        //int state               = boost::any_cast<int>(m_OrdersModel->data(*it,5,UserRole));
        std::string newRef = ((Portal *)WApplication::instance())->getSession()->copyOrder(ref);
        rowCopyed++;
    }
    log("info")<<newRef;
    ((Portal *)WApplication::instance())->getSession()->setCurrentOrder(0);
    WMessageBox::show(WString::fromUTF8("Предупреждение !"),WString::fromUTF8("Скопировано {1} документов <br/> Нажмите кнопку обновить.").arg(rowCopyed-1),Ok);
}

void OrdersJournal::printGroupDoc(std::vector<int> docList)
{
    WMessageBox::show(WString::fromUTF8("Предупреждение !"),WString::fromUTF8("Пока не работает."),Ok);
}

void OrdersJournal::uploadGroupDoc(std::vector<int> docList)
{
    WMessageBox::show(WString::fromUTF8("Предупреждение !"),WString::fromUTF8("Пока не работает."),Ok);
}

void OrdersJournal::inputOrderNumber(WAnchor *anc)
{
    WDialog *dialog = new WDialog(tr("order.journal.input.number.header"));

    WLabel *label = new WLabel(tr("order.journal.input.number"),
				       dialog->contents());
    WLineEdit *edit = new WLineEdit(dialog->contents());
    label->setBuddy(edit);

    //WRegExpValidator *validator = new Wt::WRegExpValidator("[A-Za-z0-9.-]{2,10}");
    WLengthValidator *validator = new WLengthValidator(0,17);
    validator->setMandatory(true);
    edit->setValidator(validator);

    WPushButton *ok = new WPushButton("OK", dialog->footer());
    ok->setDefault(true);
    //ok->disable();

    WPushButton *cancel = new WPushButton("Cancel", dialog->footer());
    dialog->rejectWhenEscapePressed();

    /*edit->keyWentUp().connect(std::bind([=] () {
	ok->setDisabled(edit->validate() != WValidator::Valid);
    }));*/

    /*
     * Accept the dialog
     */
    ok->clicked().connect(std::bind([=] () {
	if (edit->validate())
    {
	    dialog->accept();
    }else{
        WMessageBox::show(WString::fromUTF8("Предупреждение !"),WString::fromUTF8("Номер документа очень длинный <br/> Максиму 17 символов"),Ok);
    }
    }));

    /*
     * Reject the dialog
     */
    cancel->clicked().connect(dialog, &Wt::WDialog::reject);

    /*
     * Process the dialog result.
     */
    //dialog->finished().connect()

    dialog->finished().connect(
    std::bind([=] () {
        if (dialog->result() == WDialog::Accepted)
        {
            if(edit->text().empty())
            {
                anc->setText(anc->objectName());
                ((Portal *)WApplication::instance())->getSession()->setOrderNumber(anc->objectName(),"");
            }
            else
            {
                anc->setText(edit->text());
                ((Portal *)WApplication::instance())->getSession()->setOrderNumber(anc->objectName(),edit->text());
            }

        }
        delete dialog;
    }));

    dialog->show();
}

void OrdersJournal::changeNumebr()
{
    inputOrderNumber((WAnchor*)sender());
}

void OrdersJournal::changed(WWidget* sender)
{
    //log("info")<<sender->id();
    //WImage *img = dynamic_cast<WImage*>(sender);
    //if(sender->objectName()=="selectall")
    //{
    //    WCheckBox *header_chb = dynamic_cast<WCheckBox*>(sender);
    //    for(int i=1;i<m_OrderTables->rowCount();i++)
    //    {
    //        WCheckBox *chb = dynamic_cast<WCheckBox*>(m_OrderTables->elementAt(i,1)->widget(0));
    //        if(chb)
    //            chb->setCheckState(header_chb->checkState());
    //    }
    //}

    //WAnchor *anc = dynamic_cast<WAnchor*>(sender);
    //if(anc)
    //{

        //log("info")<<sender->objectName()<<" "<<__LINE__;
        //((Portal *)WApplication::instance())->getSession()->setCurrentOrder(boost::lexical_cast<int>(sender->objectName()));
        //log("info")<<" NAME - "<<sender->objectName();

        /*
        Reports *report = dynamic_cast<Reports*>(WApplication::instance()->root()->find("OrderDetail"));
        WStackedWidget *content = ((Portal*)WApplication::instance())->getMainContent();
        if(report)
        {
            log("info")<<"Update widget report";
            //content->currentWidget()->hide();
            report->update();
            content->setCurrentWidget(report);
        }else{
            log("info")<<"Add widget report";
            WWidget *report;
            //content->currentWidget()->hide();
            content->addWidget(report = new Reports(NULL,  Reports::OrderDetail));
            content->setCurrentWidget(report);
        }
*/

    //}
}


WWidget *OrdersJournal::changeNumberDialog()
{
    WDialog *dlg = new WDialog(WString::fromUTF8("Изменить номер заказа"));
    return dlg;
}
