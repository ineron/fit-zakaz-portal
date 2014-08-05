#ifndef _CATALOGVIEWWIDE_H_INCLUDED_
#define _CATALOGVIEWWIDE_H_INCLUDED_

using namespace Wt;

#include <Wt/WTemplate>
#include <Wt/WContainerWidget>
#include <Wt/WText>

#include "Session.h"

using namespace Wt;

class CatalogView : public WContainerWidget
{
public:
    enum Mode {Normal,Search};
    CatalogView(int currentPage,WContainerWidget *parent=0):WContainerWidget(parent),m_currentPage(currentPage),m_table(0)
    {
        m_table = new WTable(this);
        m_table->setStyleClass("table table-bordered table-striped");
        update();
    };
    virtual ~CatalogView(){};
    int currentPage()
    {
        return m_currentPage;
    }
    void setCurrentPage(int val)
    {
        m_currentPage = val;
    }
    void setMode(Mode mode)
    {
        m_mode = mode;
    };
    void update()
    {
        m_table->clear();
        createCatalog();
    }
    void setSearchString(const WString& str,bool &findByCode)
    {
        m_currentPage = 1;
        m_SearchString = str;
        m_findByCode = findByCode;
    };
protected:
    WWidget *renderView(){};

    //WContainerWidget *m_parent;
private:
    int m_currentPage;
    int m_lastRecord;
    int m_mode;
    WTable *m_table;
    //Session session_;
    std::vector<WWidget*> m_editStack;
    WString m_SearchString;

    bool m_findByCode;
private:
    void onChangePage()
    {

    };
    void createCatalog()
    {

        std::vector<db::Catalog> catalogs;
        if(m_SearchString.empty())
        {
            catalogs = ((Portal *)WApplication::instance())->getSession()->catalog(m_currentPage*2-1,0);//812 4497752

        }
        else
        {
            catalogs = ((Portal *)WApplication::instance())->getSession()->findString(m_SearchString,m_currentPage*2);
        }

        createCatalogHeader();
        createCatalogTable(catalogs,1);
    };
    void createCatalogHeader()
    {
        m_table->setHeaderCount(1);
        m_table->elementAt(0, 0)->addWidget(new WText());
        m_table->elementAt(0, 1)->addWidget(new WText(WString("№",UTF8)));
        m_table->elementAt(0, 2)->addWidget(new WText(WString("Наименование",UTF8)));
        m_table->elementAt(0, 3)->addWidget(new WText(WString("Вес",UTF8)));
        m_table->elementAt(0, 4)->addWidget(new WText(WString("Упак-ка<br/>Коробка",UTF8)));
        m_table->elementAt(0, 5)->addWidget(new WText(WString("Цена в руб.",UTF8)));
        m_table->elementAt(0, 6)->addWidget(new WText(WString("Заказ по шт",UTF8)));
    }
    void createCatalogTable(std::vector<db::Catalog> catalogs,int tabIndex)
    {
        WString imageName = "";
        WString description="";
        WString name="";

        int row_count = 1;
        for (size_t i=0; i < catalogs.size(); i++)
        {
            db::Catalog c = catalogs[i];
            if(c.isGroup)
            {

                if(catalogs[i+1].isGroup && catalogs[i+1].isGroup)
                {

                    /// Создаем строку с названием группы товара dynamic_cast
                    m_table->elementAt(row_count, 0)->addWidget(new WText(WString(c.name,UTF8)));
                    m_table->elementAt(row_count, 0)->setColumnSpan(7);
                    m_table->rowAt(row_count)->setStyleClass("tab_name level1");
                }
                else
                {

                    /// Создаем строку с названием под группы товара
                    m_table->elementAt(row_count, 0)->addWidget(new WText(WString(c.name,UTF8)));
                    m_table->elementAt(row_count, 0)->setColumnSpan(7);
                    m_table->rowAt(row_count)->setStyleClass("brush_name");

                }

            }
            else
            {

                //if(firsRow)
                {

                    std::string folder = "";
                    imageName = WString(c.fileName,UTF8).trim();
                    /// Выводим первую строку группы товаров с картинкой
                    //WContainerWidget *wc = new WContainerWidget();
                    //wc->setStyleClass("corner_kick");
                    WContainerWidget *wcPic = new WContainerWidget();
                    wcPic->setStyleClass("pic");//pic
                    WString imagePath = WString(""+WString(folder)+WString(imageName));
                    if(imageName.empty())
                        new WText(wcPic);
                    else
                    {
                        WTemplate *img = new WTemplate(tr("portal.catalog.cell_image"),wcPic);
                        img->bindString("imgaename",imagePath);
                    }
                    m_table->elementAt(row_count, 0)->addWidget(wcPic);
                    m_table->rowAt(row_count)->setStyleClass("row_t");
                }

                WString articul = WString::fromUTF8(c.name).trim();

                //WContainerWidget *div = new WContainerWidget();
                //new WText(WString::fromUTF8(c.code),m_table->elementAt(row_count, 1));
                m_table->elementAt(row_count, 1)->setObjectName(c.code);
                //div = new WContainerWidget();
                //new WText( articul,div);

                m_table->elementAt(row_count, 2)->addWidget(new WText(WString(c.name,UTF8)));
                m_table->elementAt(row_count, 2)->setToolTip(articul);

                WContainerWidget *div = new WContainerWidget();
                new WText( WString::fromUTF8(c.shortDescription),div);
                m_table->elementAt(row_count, 3)->addWidget(div);
                std::string str = c.shortDescription;
                boost::replace_all( str, "<br/>", " " );
                m_table->elementAt(row_count, 3)->setToolTip(WString::fromUTF8(str));

                div = new WContainerWidget();
                new WText( WString::fromUTF8(c.upakovka),div);
                m_table->elementAt(row_count, 4)->addWidget(div);
                m_table->elementAt(row_count, 4)->setToolTip(WString::fromUTF8(c.upakovka));
                div = new WContainerWidget();
                char number[50];
                sprintf(number,"%.2f",c.price);
                m_table->elementAt(row_count, 5)->addWidget(new WText(number));
                m_table->elementAt(row_count, 5)->setToolTip(WString::fromUTF8(number));
                div = new WContainerWidget();
                //div->setStyleClass("amount");
                WLineEdit *edit = new WLineEdit("0",div);
                edit->setStyleClass("span1");
                edit->setTabIndex(tabIndex++);
                //log("info")<<"****** price "<<c.price<<" NAME "<<c.name<<" CODE "<<c.code;
                if(c.price)
                {
                    ///edit->setValidator(new Wt::WIntValidator(0, 200));
                    edit->setText(WString("{1}").arg(c.quantity));
                    if(c.quantity)
                        edit->addStyleClass("blue");
                    edit->blurred().connect(this,&CatalogView::fieldChanged);

                    WString javaScr("$('input[tabindex={1}]').focus();");
                    javaScr.arg(tabIndex);
                    edit->enterPressed().connect(std::bind([=]()
                    {
                        edit->doJavaScript(javaScr.toUTF8());
                    }));

                }
                else
                {
                    edit->setReadOnly(true);
                    edit->hide();
                }

                //edit->enterPressed().connect(SLOT(this, &CatalogMainPage::search));

                m_table->elementAt(row_count, 6)->addWidget(div);

                m_table->elementAt(row_count, 0)->setStyleClass("cell_1");
                m_table->elementAt(row_count, 1)->setStyleClass("cell_2");


                if(!m_SearchString.empty())
                {
                    if(WString(c.code,UTF8).trim() == m_SearchString)
                    {
                        //log("info")<<"************************************************************"<<m_SearchString;
                        m_table->elementAt(row_count, 1)->setStyleClass("cell_2 mark");
                        edit->setFocus();
                        //m_SearchString = "";
                    }
                }



            }
            row_count++;
            //log("info")<<"************** GROUP TYPE = "<<c.groupType << " name ="<<name<<" row count "<<row_count;


        }
    };

    void fieldChanged()
    {
#ifdef DEBUG
        log("info") << "*************** CHANGED *****************"<<__LINE__;
        log("info") << sender()->id();
        log("info") << sender()->objectName();
#endif
        WLineEdit *edit = dynamic_cast<WLineEdit*>(sender());
        if(edit)
        {
            int quantity = atoi(edit->text().toUTF8().c_str());
            float price = 0;
            std::string code;
            if(WTableCell *cell = dynamic_cast<WTableCell*>(sender()->parent()->parent()))
            {
                if(WText *text = dynamic_cast<WText*>(cell->tableRow()->elementAt(5)->widget(0)))
                    price = atof(text->text().toUTF8().c_str());
                code = cell->tableRow()->elementAt(1)->objectName();
            }
#ifdef DEBUG
            log("info") <<"goods "<< code <<" quantity "<< quantity<<" Price "<<price;
#endif
            if (quantity)
            {
                edit->addStyleClass("blue");
                ((Portal *)WApplication::instance())->getSession()->addOrderRecords(quantity,code,price);
            }

        }
    };
};

#endif // _CATALOGVIEWWIDE_H_INCLUDED_

