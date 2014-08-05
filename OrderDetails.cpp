#include "OrderDetails.h"
#include "Session.h"
#include "Portal.h"
#include "utils/LineEdit.h"

#include <iostream>
#include <string>

#include <Wt/WPushButton>

#include <Wt/WMessageBox>
#include <Wt/WContainerWidget>
#include <Wt/WAnchor>
#include <Wt/WLink>
#include <Wt/WText>
#include <Wt/WLabel>
#include <Wt/WTable>
#include <Wt/WTableCell>
#include <Wt/WImage>
#include <Wt/WLineEdit>
#include <Wt/WIntValidator>
#include <Wt/WTextEdit>
#include <Wt/WComboBox>
#include <Wt/WTemplate>
#include <Wt/WLengthValidator>

#include <Wt/WStringListModel>
#include <Wt/WLengthValidator>

OrderDetails::OrderDetails(WContainerWidget *parent):WContainerWidget(parent),m_TotalAmount(0),m_Comments(0),m_CommentsDiv(0),m_OrderRecords(0)
{
    m_OrderTables = new WTable(this);
    m_OrderTables->setStyleClass("table table-bordered table-striped");

}

OrderDetails::OrderDetails(std::vector<db::Catalog> orderRecords,WContainerWidget *parent):WContainerWidget(parent),m_TotalAmount(0),m_Comments(0),m_CommentsDiv(0),m_OrderRecords(0)
{
    //log("info")<<"OrderDetails::OrderDetails() "<<__LINE__;
    m_OrderTables = new WTable(this);
    m_OrderTables->setStyleClass("table table-bordered table-striped");
    m_OrderRecords = orderRecords;
    updateReport();
    //log("info")<<"OrderDetails::OrderDetails() "<<__LINE__;
}

OrderDetails::~OrderDetails()
{
    //dtor
}

void OrderDetails::sheduleRender()
{
    if(isRendered())
        sheduleRender();

}

void OrderDetails::createHeader()
{
    //log("info")<<"OrderDetails::createHeader() "<<__LINE__;
    m_OrderTables->clear();
    m_OrderTables->setHeaderCount(1);
    m_OrderTables->elementAt(0,0)->addWidget(new WText(tr("order.details.header.coll1")));
    m_OrderTables->elementAt(0,1)->addWidget(new WText(tr("order.details.header.coll2")));
    m_OrderTables->elementAt(0,2)->addWidget(new WText(tr("order.details.header.coll3")));
    m_OrderTables->elementAt(0,3)->addWidget(new WText(tr("order.details.header.coll4")));
    m_OrderTables->elementAt(0,4)->addWidget(new WText(tr("order.details.header.coll5")));
    m_OrderTables->elementAt(0,5)->addWidget(new WText(tr("order.details.header.coll6")));
    m_OrderTables->elementAt(0,6)->addWidget(new WText(tr("order.details.header.coll7")));
    m_OrderTables->elementAt(0,7)->addWidget(new WText(tr("order.details.header.coll8")));
    //log("info")<<"OrderDetails::createHeader() "<<__LINE__;

}

void OrderDetails::updateReport(std::vector<db::Catalog> orderRecords)
{

    //log("info")<<"OrderDetails::updateReport(std::vector<db::Catalog> "<<&orderRecords<<") "<<__LINE__;


    OrdersHeaderPtr  order = ((Portal *)WApplication::instance())->getSession()->getCurrentOrder();
    //log("info")<<"ORDER "<<order;
    bool canModifed = false;
    if(order)
    {
        //int orderState = order->state;
        canModifed = !((order->state == db::OrdersHeader::held) || (order->state == db::OrdersHeader::process));
    }
    //log("info")<<"MODE "<<canModifed;

    if(m_TotalAmount)
        m_TotalAmount->clear();


    createHeader();
    float summa = 0;
    int quantity = 0;
    unsigned row_count=0;
    for (row_count=0; row_count < orderRecords.size(); row_count++)
    {
        db::Catalog c = orderRecords[row_count];

        std::string folder = (c.owner == 1?"FIT":"KF");
        WString imageName = WString(c.fileName,UTF8).trim();
        /// Выводим первую строку группы товаров с картинкой
        WContainerWidget *wc = new WContainerWidget();
        wc->setStyleClass("corner_kick");
        WContainerWidget *wc2 = new WContainerWidget(wc);
        wc2->setStyleClass("pic");


        WString imagePath = WString(""+WString(folder)+"/"+WString(imageName));
        WTemplate *img = new WTemplate(tr("portal.catalog.cell_image"),wc2);
        img->bindString("imgaename",imagePath);


        m_OrderTables->elementAt(row_count+1, 0)->addWidget(wc);

        WString articul = WString(c.articul,UTF8).trim();
        std::string color = "";
        if(articul=="FIT IT")
            color = "light_blue";
        else if(articul=="FIT HQ")
            color = "orange_yellow";
        else
            color = "light_gray";

        /// Col 1 
        WContainerWidget *div = new WContainerWidget();
        new WText(WString(c.code,UTF8),m_OrderTables->elementAt(row_count+1, 1));
        //m_OrderTables->elementAt(row_count+1, 1)->addWidget(div);
        /// Col 2
        div = new WContainerWidget();
        new WText( WString(articul),div);
        m_OrderTables->elementAt(row_count+1, 2)->addWidget(div);
        /// Col 3
        div = new WContainerWidget();
        new WText( WString(c.name,UTF8),div); //shortDescription
        m_OrderTables->elementAt(row_count+1, 3)->addWidget(div);
        /// Col 4
        div = new WContainerWidget();
        new WText( WString(c.upakovka,UTF8),div);
        m_OrderTables->elementAt(row_count+1, 4)->addWidget(div);
        /// Col 5
        //div = new WContainerWidget();
        char number[80];
        sprintf(number,"%.2f",c.price);
        WText *price = new WText(number);
        if(!c.price)
        {
            WCssDecorationStyle &dc = price->decorationStyle();
            dc.setForegroundColor (WColor("red"));
        }

        m_OrderTables->elementAt(row_count+1, 5)->addWidget(price);
        /// Col 6
        div = new WContainerWidget();
        div->setStyleClass("amount");
        if(canModifed)
        {
            WLineEdit *edit = new WLineEdit("0",div);
            edit->setText(WString("{1}").arg(c.quantity));
            edit->setWidth(25);
            edit->setObjectName( WString("{1}").arg(c.quantity).toUTF8() );
            //edit->setId(c.code);
            edit->setTabIndex(row_count+1);
            if(c.price)
            {
                edit->setValidator(new Wt::WIntValidator(0, 200));
                //edit->blurred().connect(this,&OrderDetails::changed);
                edit->changed().connect(this,&OrderDetails::changed);
                //edit->enterPressed().connect(this,&OrderDetails::changed);
            }
        }
        else
        {
            new WText(WString("{1}").arg(c.quantity),div);
        }

        m_OrderTables->elementAt(row_count+1, 6)->addWidget(div);

        /// Col 7
        sprintf(number,"%.2f",c.price*c.quantity);
        summa += c.price*c.quantity;
        quantity += c.quantity;
        WText *total = new WText( WString("{1}").arg(number),m_OrderTables->elementAt(row_count+1, 7));
        if(!c.price)
        {
            WCssDecorationStyle &dc = total->decorationStyle();
            dc.setForegroundColor (WColor("red"));
        }
        m_OrderTables->elementAt(row_count+1, 2)->setContentAlignment(AlignTop | AlignCenter);
        m_OrderTables->elementAt(row_count+1, 3)->setContentAlignment(AlignTop | AlignCenter);
        m_OrderTables->elementAt(row_count+1, 4)->setContentAlignment(AlignTop | AlignCenter);
        m_OrderTables->elementAt(row_count+1, 5)->setContentAlignment(AlignTop | AlignRight);
        m_OrderTables->elementAt(row_count+1, 7)->setContentAlignment(AlignTop | AlignRight);
        //log("info")<<"*** Row count *** "<<row_count;
    }

    if(!m_TotalAmount)
        m_TotalAmount = new WContainerWidget(this);
    m_TotalAmount->setStyleClass("summary");
    (new WText(tr("order.details.cart.total.amount").arg(row_count),m_TotalAmount))->setStyleClass("amount");
    char number[40];
    sprintf(number,"%.2f",summa);
    (new WText(tr("order.details.cart.total").arg(number),m_TotalAmount))->setStyleClass("price");

    //bool canModifed = false;



    if(!m_CommentsDiv)
    {
        m_CommentsDiv = new WContainerWidget(this);
        //m_CommentsDiv->setId("order_form");
        m_CommentsDiv->setObjectName("order_form");
        //m_CommentsDiv->setStyleClass("btn-group");

    }
    m_CommentsDiv->clear();

    WLengthValidator *validator = new WLengthValidator(2,150);
    validator->setMandatory(true);


    WLabel *addressLbl = new WLabel(WString::fromUTF8("<h3>Адрес доставки: </h3>"),m_CommentsDiv);
    WComboBox *addressCmb = new WComboBox(m_CommentsDiv);
    int currentIndex=0;
    addressCmb->setModel(((Portal *)WApplication::instance())->getSession()->getAddressList(new WStringListModel(),currentIndex));
    addressCmb->setCurrentIndex(currentIndex);
    addressCmb->setStyleClass("span7");
    addressCmb->changed().connect(this,&OrderDetails::changed);
    addressCmb->setObjectName("cmb_address");
    addressCmb->setValidator(validator);
    //addressCmb->setWidth(WLength(40,WLength::FontEm));
    addressLbl->setBuddy(addressCmb);
    addressCmb->setDisabled(true);
    new WBreak(m_CommentsDiv);

    if(canModifed)
    {

        addressCmb->setDisabled(false);

        //(new WContainerWidget(m_CommentsDiv))->setStyleClass("clear");
        WLabel *label = new WLabel(tr("order.details.comments"), m_CommentsDiv );
        //label->setStyleClass("comments_h");
        label->setObjectName("comments_label");
        WTextArea *comments = new WTextArea(m_CommentsDiv);
        comments->setStyleClass("span7");
        comments->setObjectName("comments");
        std::string str = ((Portal *)WApplication::instance())->getSession()->getOrderComments();
        comments->setText(WString(str,UTF8));
        WLengthValidator *validatorText = new WLengthValidator(0, 200);
        validatorText->setMandatory(true);
        comments->setValidator(validatorText);
        comments->changed().connect(this,&OrderDetails::changed);
        WContainerWidget *groupButtons = new WContainerWidget(m_CommentsDiv);
        new WBreak(m_CommentsDiv);
        groupButtons->setStyleClass("btn-group ");

        {
            //WTemplate *button = new WTemplate("<ins class=\"i1\"></ins><ins class=\"i2\"></ins><ins class=\"i3\"></ins><ins class=\"i4\"></ins>${anchor}",m_CommentsDiv);
            //WAnchor *anchor = new WAnchor("#",tr("order.details.held.btn"));

            //anchor->clicked().connect(boost::bind(&OrderDetails::changed, this, anchor));
            WPushButton *button = new WPushButton(tr("order.details.held.btn"),groupButtons);
            button->setStyleClass("btn-success span2 btn-lg");
            button->setObjectName("held_btn");
            button->clicked().connect(this,&OrderDetails::changed);
            //button->bindWidget("anchor",anchor);
            //button->setStyleClass("continue corner_kick");
        }

        {
            //WTemplate *button = new WTemplate("<ins class=\"i1\"></ins><ins class=\"i2\"></ins><ins class=\"i3\"></ins><ins class=\"i4\"></ins>${anchor}",m_CommentsDiv);
            //WAnchor *anchor = new WAnchor(WLink(WLink::InternalPath,"/shop"),tr("order.details.continue.btn"));
            //anchor->setObjectName("continue_btn");
            ////anchor->clicked().connect(boost::bind(&OrderDetails::changed, this, anchor));
            //anchor->clicked().connect(this,&OrderDetails::changed);
            //button->bindWidget("anchor",anchor);
            //button->setStyleClass("continue2 corner_kick");

            WPushButton *button = new WPushButton(tr("order.details.continue.btn"),groupButtons);
            button->setStyleClass("btn-info span2 btn-lg");
            button->setLink(WLink(WLink::InternalPath,"/shop"));
            //button->setInternalPath("shop");
            //button->setRef("/shop");
            button->setObjectName("continue_btn");
            button->clicked().connect(this,&OrderDetails::changed);

        }
        //(new WContainerWidget(m_CommentsDiv))->setStyleClass("clear");
        {
            //WTemplate *button = new WTemplate("<ins class=\"i1\"></ins><ins class=\"i2\"></ins><ins class=\"i3\"></ins><ins class=\"i4\"></ins>${anchor}",m_CommentsDiv);
            //WAnchor *anchor = new WAnchor("#",tr("order.details.clear.btn"));
            //anchor->setObjectName("clear_btn");
            //anchor->clicked().connect(this,&OrderDetails::changed);
            //button->bindWidget("anchor",anchor);
            //button->setStyleClass("reset_option corner_kick");

            WPushButton *button = new WPushButton(tr("order.details.clear.btn"),groupButtons);
            button->setStyleClass("btn-danger span2 btn-lg");
            button->setObjectName("clear_btn");
            button->clicked().connect(this,&OrderDetails::changed);

        }

        {
            //WTemplate *button = new WTemplate("<ins class=\"i1\"></ins><ins class=\"i2\"></ins><ins class=\"i3\"></ins><ins class=\"i4\"></ins>${anchor}",m_CommentsDiv);
            //WAnchor *anchor = new WAnchor(WLink(WLink::InternalPath,"/orders/orders_journal"),tr("order.details.save.btn"));
            //anchor->setObjectName("save_btn");
            //anchor->clicked().connect(this,&OrderDetails::changed);
            //button->bindWidget("anchor",anchor);
            //button->setStyleClass("save_option corner_kick");

            WPushButton *button = new WPushButton(tr("order.details.save.btn"),groupButtons);
            button->setStyleClass("btn-primary span2 btn-lg");
            button->setObjectName("save_btn");
            button->clicked().connect(this,&OrderDetails::changed);

        }


    }
    else
    {

        (new WContainerWidget(m_CommentsDiv))->setStyleClass("clear");
        WLabel *label = new WLabel(tr("order.details.comments"), m_CommentsDiv );
        label->setStyleClass("comments_h");
        label->setObjectName("comments_label");
        WTextArea *comments = new WTextArea(m_CommentsDiv);
        comments->setStyleClass("comments_txt");
        comments->setObjectName("comments");
        std::string str = ((Portal *)WApplication::instance())->getSession()->getOrderComments();
        comments->setText(WString(str,UTF8));
        comments->setReadOnly(true);
        (new WContainerWidget(m_CommentsDiv))->setStyleClass("clear");

    }
    (new WContainerWidget(m_CommentsDiv))->setStyleClass("clear");
    //log("info")<<"OrderDetails::updateReport(std::vector<db::Catalog> "<<&orderRecords<<") "<<__LINE__;

}

void OrderDetails::updateReport()
{
    //log("info")<<"OrderDetails::updateReport() "<<__LINE__;
    updateReport(m_OrderRecords);
    //log("info")<<"OrderDetails::updateReport() "<<__LINE__;
}

void OrderDetails::setModel(const std::vector<db::Catalog> orderRecords)
{
    m_OrderRecords = orderRecords;
}

void OrderDetails::changed()//(WWidget* sender)
{
    WPushButton *edit = dynamic_cast<WPushButton*>(sender());
    WTextArea *text = dynamic_cast<WTextArea*>(sender());
    WPushButton *anch = dynamic_cast<WPushButton*>(sender());
    WLineEdit *quantityEdit = dynamic_cast<WLineEdit*>(sender());
    OrdersHeaderPtr  order = ((Portal *)WApplication::instance())->getSession()->getCurrentOrder();
    int orderNumber = order->orderId;

    //WTableCell *cell = dynamic_cast<WTableCell*>(sender);
    //log("info")<<quantityEdit->id();
    if(WComboBox *address = dynamic_cast<WComboBox*>(sender()))
    {
        int row = address->currentIndex();

        if(WStringListModel *model = dynamic_cast<WStringListModel*>(address->model()))
        {
            AddressPtr addressRef = boost::any_cast<AddressPtr>(model->data(model->index(row,0), Wt::UserRole));
            ((Portal *)WApplication::instance())->getSession()->setOrderAddress(addressRef);
            log("info")<<"Address ref "<<addressRef;
        }
    }
    if(edit)
    {
        if (edit->objectName()=="continue_btn")
        {
            //log("info") <<"go back ";

            if(((Portal *)WApplication::instance())->getSession()->getEditableOrder() )
            {
                WMessageBox::show("Info", WString("Есть не сохраненый заказ. Закончите, с ним работу и повторите операцию",UTF8), Ok);
            }
            else
            {
                ((Portal *)WApplication::instance())->getSession()->setEditableOrder();
            }

        }
        else if(edit->objectName()=="save_btn")
        {
            //log("info") <<"save ";
            ((Portal *)WApplication::instance())->getSession()->saveCurrentOrder();
            ((Portal *)WApplication::instance())->getSession()->setCurrentOrder(0);
            //updateReport();
            WMessageBox  msg;
            msg.titleBar()->addStyleClass("alert alert-info");
            msg.setMaximumSize(140, 70);
            msg.show(WString("Warning",UTF8), tr("order.details.save.message"), Ok );
        }
        else if(edit->objectName()=="held_btn")
        {
            //log("info") <<"held ";alert alert-danger
            WMessageBox  msg;
            msg.titleBar()->addStyleClass("alert alert-warning");
            msg.titleBar()->setId("dialog_header");
            msg.setText(WString::fromUTF8("Отправить документ № {1} на обработку ?").arg(orderNumber));
            msg.setCaption(WString::fromUTF8("Предупреждение !"));
            msg.setStandardButtons(Ok | Cancel);
            //msg.exec();
            if(msg.show(WString::fromUTF8("Предупреждение !"),WString::fromUTF8("Отправить документ № {1} на обработку ?").arg(orderNumber),Ok | Cancel) == Ok )
            {
                if(WComboBox *address = dynamic_cast<WComboBox*>(find("cmb_address")))
                {
                    int row = address->currentIndex();
                    if(WStringListModel *model = dynamic_cast<WStringListModel*>(address->model()))
                    {
                        if(row)
                        {
                            AddressPtr addressRef = boost::any_cast<AddressPtr>(model->data(model->index(row,0), Wt::UserRole));
                            ((Portal *)WApplication::instance())->getSession()->heldCurrentOrder(addressRef);
                            msg.titleBar()->addStyleClass("alert alert-success");
                            msg.show(WString("Предупреждение !",UTF8), tr("order.details.held.message"), Ok );
                            edit->hide();
                        }
                        else
                        {
                            msg.titleBar()->addStyleClass("alert alert-danger");
                            msg.show(WString("Предупреждение !",UTF8), tr("order.details.setaddress.message"), Ok );
                        }
                    }
                }
            }
        }
        else if(edit->objectName()=="clear_btn")
        {
            //log("info") <<"clear ";
            WMessageBox  msg(WString::fromUTF8("Предупреждение !"),WString::fromUTF8("Удалить документ № {1} ?").arg(orderNumber),Critical,Ok | Cancel);
            //msg.titleBar()->addStyleClass("alert alert-danger");
            if( msg.show(WString::fromUTF8("Предупреждение !"),WString::fromUTF8("Удалить документ № {1} ?").arg(orderNumber),Ok | Cancel) == Ok)
            //msg.show();
            //if( msg.buttonResult() == Ok)
            {
                ((Portal *)WApplication::instance())->getSession()->clearCurrentOrder();
                ((Portal *)WApplication::instance())->getSession()->setCurrentOrder(0);
            }
        }
        updateReport(((Portal *)WApplication::instance())->getSession()->getOrderRecords());
    }
    else if(quantityEdit)
    {
        int quantity = atoi(quantityEdit->text().toUTF8().c_str());
        int oldquantity = atoi(quantityEdit->objectName().c_str());
        std::string code;
        if(WTableCell *cell = dynamic_cast<WTableCell*>(sender()->parent()->parent()))
        {
            if(WText *text = dynamic_cast<WText*>(cell->tableRow()->elementAt(1)->widget(0)))
                code = text->text().toUTF8();
        }

        //log("info") <<"goods "<< code <<" quantity "<< quantity<<" old "<<oldquantity;
        if(oldquantity != quantity)
        {
            ((Portal *)WApplication::instance())->getSession()->addOrderRecords(quantity,code);
            updateReport(((Portal *)WApplication::instance())->getSession()->getOrderRecords());
        }

    }
    else if(text)
    {
        if(text->objectName()=="comments")
        {
            //log("info")<<" NEW COMMENTS "<<text->text();
            if(text->text().toUTF8().size() < 200)
                ((Portal *)WApplication::instance())->getSession()->setOrderComments(text->text().toUTF8().c_str());
            else
                WMessageBox::show("Warning", WString("Слишком длинный текст. <br/> Максимум 200 символов",UTF8), Ok);

        }
    }
    else if(anch)
    {
        if (anch->objectName()=="continue_btn1")
        {
            // 1 Проверить что нет заказов в сотстонии редактируемый
            // 2 если есть такой заказ то предложить сохранить не сохранять и продолжить
            // 3 после чего сделать выбранный заказ редактируемым
            if(((Portal *)WApplication::instance())->getSession()->getEditableOrder())
            {
                WMessageBox::show("Confirm", WString("Есть не сохраненый заказ. Закончите с ним работу и повторите операцию",UTF8), Ok);
            }
            else
            {

                ((Portal *)WApplication::instance())->getSession()->setEditableOrder();
            }
            //log("info") <<"go back *********************************";
        }
    }
}
