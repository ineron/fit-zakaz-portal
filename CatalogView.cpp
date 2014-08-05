#include "CatalogView.h"
#include "Session.h"
#include "Portal.h"

#include <fstream>
#include <iostream>

#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>

//#include <Wt/WComboBox>
#include <Wt/WContainerWidget>
#include <Wt/WAnchor>
#include <Wt/WLink>
#include <Wt/WText>
#include <Wt/WTable>
#include <Wt/WTemplate>
#include <Wt/WFormWidget>
#include <Wt/WImage>
#include <Wt/WLineEdit>
#include <Wt/WIntValidator>
#include <Wt/WStandardItemModel>
#include <Wt/WRectArea>
#include <Wt/WDialog>
#include <Wt/WLength>


using namespace Wt;

CatalogView::CatalogView(int currentPage,WContainerWidget *parent):WStackedWidget(parent),m_currentPage(currentPage),m_lastRecord(0),m_findByCode(false)
{
    //m_currentPage =1;
    //m_lastRecord = 0;
    //m_parent = parent;
    //WAnimation fade(WAnimation::Fade, WAnimation::Linear, 150);
    //setTransitionAnimation(fade);
    //addWidget(renderView());
    //setWidth()
    setStyleClass("cat_bg clear_inside");
#ifdef DEBUG
    log("info")<<" CREATE CATALOG VIEW  ";
#endif
    renderView();
    //ctor
}

CatalogView::~CatalogView()
{
    //dtor
}

WWidget *CatalogView::renderView()
{
#ifdef DEBUG
    log("info")<<"WWidget *CatalogView::renderView( page "<<m_currentPage<<")"<<__LINE__;
#endif

    setWidth(670);
    std::vector<db::Catalog> catalogs;

    WContainerWidget *cw = new WContainerWidget(this);

    cw->setStyleClass("cat_bg_inner");
    WContainerWidget *wf = new WContainerWidget(cw);
    wf->setId("catalog_f");

    log("info")<<" m_SearchString "<<m_SearchString<<"<************************> "<<m_currentPage;



    if(m_SearchString.empty() || m_findByCode )
    {
        catalogs = ((Portal *)WApplication::instance())->getSession()->catalog(m_currentPage*2-1,0);//812 4497752

    }
    else
    {
        catalogs = ((Portal *)WApplication::instance())->getSession()->findString(m_SearchString,m_currentPage*2);
    }
    int tabIndex = 1;
    wf->addWidget(createCatalogTable(catalogs,tabIndex));

    if(m_SearchString.empty() || m_findByCode)
    {
        catalogs = ((Portal *)WApplication::instance())->getSession()->catalog(m_currentPage*2,1);
    }
    else
    {
        catalogs = ((Portal *)WApplication::instance())->getSession()->findString(m_SearchString,m_currentPage*2+1,1);
    }

    //log("info")<<m_SearchString<<"<>";

    wf->addWidget(createCatalogTable(catalogs,tabIndex));

    m_lastRecord = 0;
    //show();
#ifdef DEBUG
    log("info")<<"WWidget *CatalogView::renderView() "<<__LINE__;
#endif
    return cw;
}

//WTableRow *CatalogView::addRow(WTable *table,std::vector<Catalog> catalogs,int side)
//{
//
//}

// Создает левую и правую части таблицы каталога при выводе по странично
// при выводе по поиску используется функция searchCatalogTable
WWidget *CatalogView::createCatalogTable(std::vector<db::Catalog> catalogs,int &tabIndex)
{
#ifdef DEBUG
    log("info")<<"WWidget *CatalogView::createCatalogTable( "<<catalogs.size()<<","<< tabIndex<<")"<<__LINE__;
#endif

    if(!catalogs.size())
    return new WTable();

    //WString *str = new WString();
    //std::string imageName="";
    WString imageName = "";
    WString description="";
    WString name="";
    bool firsRow = false;
    //log("info")<<"Katalog offset= "<<m_lastRecord;
    //int imageCount = 0;
    //m_lastRecord = catalogs[0].id;

    int i=0;//m_lastRecord;
    int row_count = 1;

    if(catalogs[0].owner == 2)
    {
        //WApplication::instance()->root()->setStyleClass("catalog_2");
    }else{
        //WApplication::instance()->root()->setStyleClass("catalog");
    }


    //if(catalogs[m_lastRecord].id == 1)
    //    str = new WString(*str+ WString(tr("catalog.table.title.1")).arg("FIT",UTF8));
    WTable *table = new WTable();

    table->setHeaderCount(1);
    table->elementAt(0, 0)->addWidget(new WText());
    table->elementAt(0, 1)->addWidget(new WText(WString("Код по каталогу",UTF8)));
    table->elementAt(0, 2)->addWidget(new WText(WString("Доп. инф-я",UTF8)));
    table->elementAt(0, 3)->addWidget(new WText(WString("Описание<br/>товара",UTF8)));
    table->elementAt(0, 4)->addWidget(new WText(WString("Упак-ка<br/>Коробка",UTF8)));
    table->elementAt(0, 5)->addWidget(new WText(WString("Цена в руб.",UTF8)));
    table->elementAt(0, 6)->addWidget(new WText(WString("Заказ по шт",UTF8)));
    table->rowAt(0)->setStyleClass("col_title");

    table->elementAt(0, 0)->setStyleClass("cell_1");
    table->elementAt(0, 1)->setStyleClass("cell_2");
    table->elementAt(0, 2)->setStyleClass("cell_3");
    table->elementAt(0, 3)->setStyleClass("cell_4");
    table->elementAt(0, 4)->setStyleClass("cell_5");
    table->elementAt(0, 5)->setStyleClass("cell_6");
    table->elementAt(0, 6)->setStyleClass("cell_7");


    int groupCount = 0;
    for (; i < (int)catalogs.size(); i++)
    {
        db::Catalog c = catalogs[i];
        //log("info")<<c.name;
        if(c.isGroup)
        {
           /* if(c.division != division)
            {
                break;
                division = c.division;
            }
            */
            groupCount++;
            /// Выводим описание товара перед выводом названия группы
            if( row_count > 1 && !catalogs[i-1].isGroup)
            {
                /// Строка таблицы с описанием товара
                table->elementAt(row_count, 0)->addWidget(new WText());
                table->elementAt(row_count, 1)->addWidget(new WText(WString("<b>"+name+"</b><br/>"+description)));
                table->elementAt(row_count, 1)->setStyleClass("txt");
                table->elementAt(row_count, 1)->setColumnSpan(6);
                //table->elementAt(i, 0)
                //str = new WString(*str+ WString(tr("portal.catalog.description")).arg(name,UTF8).arg(description,UTF8));
                name = "";
                description = "";
                if(!c.notVisible)
                {
                    //row_count = table->rowCount();
                    //continue;
                }
                row_count = table->rowCount();

            }

            if(c.notVisible)
            {
                //imageName = c.fileName;
                imageName = WString(c.fileName,UTF8).trim();
                name = WString(c.name,UTF8).trim();
                description = WString(c.description,UTF8).trim();
                firsRow = true;
            }
            else if(catalogs[i+1].isGroup && !catalogs[i+1].notVisible)
                //if(!c.notVisible)
                //{
                //if(catalogs[i+1].isGroup && !catalogs[i+1].notVisible)
                //if(c.groupType == 1)
            {

                /// Создаем строку с названием группы товара dynamic_cast
                table->elementAt(row_count, 0)->addWidget(new WText(WString(c.name,UTF8)));
                table->elementAt(row_count, 0)->setColumnSpan(7);
                table->rowAt(row_count)->setStyleClass("tab_name level1");
                //str = new WString(*str+ WString(tr("catalog.table.title.1")).arg(c.name,UTF8));
            }
            //if(c.groupType != 1 and !c.notVisible)
            else
            {

                /// Создаем строку с названием под группы товара
                table->elementAt(row_count, 0)->addWidget(new WText(WString(c.name,UTF8)));
                table->elementAt(row_count, 0)->setColumnSpan(7);
                table->rowAt(row_count)->setStyleClass("brush_name");
                //str = new WString(*str+ WString(tr("catalog.table.title.brush_name").arg(c.name,UTF8)));
                //imageName = c.fileName;

                imageName = WString(c.fileName,UTF8).trim();
                name = WString(c.name,UTF8).trim();
                description = WString(c.description,UTF8).trim();
                firsRow = true;
            }
            //log("info")<<"************** GROUP TYPE = "<<c.groupType << " name ="<<name<<" row count "<<row_count;

            //}
/*
            if(c.division != division)
            {
                break;
                division = c.division;
            }
*/
            table->elementAt(row_count, 0)->setId(c.code);
            row_count = table->rowCount();
        }
        else
        {
            groupCount = 0;
            if(firsRow)
            {

                std::string folder = "SALE";
                switch(c.owner)
                {
                    case 1: folder = "FIT";break;
                    case 2: folder = "KF";break;
                    case 3: folder = "SALE";break;
                }

                /// Выводим первую строку группы товаров с картинкой
                WContainerWidget *wc = new WContainerWidget();
                wc->setStyleClass("corner_kick");
                WContainerWidget *wc2 = new WContainerWidget(wc);
                wc2->setStyleClass("pic");//pic
                WString imagePath = WString(""+WString(folder)+"/"+WString(imageName));
                //Wt::WImage *image = new Wt::WImage("images/family.jpg");
                if(imageName.empty())
                    new WText(wc2);
                else
                {
                    //WImage *img = new WImage(imagePath.toUTF8().c_str());
                    //img->setObjectName(c.code);
                    //img->clicked().connect(boost::bind(&CatalogView::changed, this, img));
                    //WAnchor *anc_px = new WAnchor("javascript:void(0)", "",wc2);
                    //anc_px->setStyleClass("highslide");
                    //anc_px->setImage(img);
                    //WString templateStr = WString("<img src=\"/images/catalog/${imgaename}\" onclick=\"try{hs.expand(this,{src:'http://nextgen.fit-zakaz.ru/images/${imgaename}'});}catch(e){}\" class=\"highslide\" height=\"255\" border=\"0\" width=\"360\"/>").arg(imagePath);
                    WTemplate *img = new WTemplate(tr("portal.catalog.cell_image"),wc2);
                    img->bindString("imgaename",imagePath);


                }
                //img->clicked().connect(*new JSlot(WString("function(o,e){try{hs.expand(o,{src:'{1}'});}catch(e){}};").arg(WString("/images/"+WString(folder)+"/"+WString(imageName))).toUTF8()));
                //img->setStyleClass("resize_thumb");
                //WRectArea *face = new Wt::WRectArea(100, 120, 200, 130);
                //face->setToolTip("Uncle Frank");
                //img->addArea(face);



                //img = new WImage("/images/action/4231400_2.jpg",actionBorder);
                //img->setObjectName("fit_elektro_akcia4kvartal1.jpg");
                //anc_px->clicked().connect(boost::bind(&CatalogView::changed, this, anc_px));

                //wc->addWidget(img);
                table->elementAt(row_count, 0)->addWidget(wc);
                table->rowAt(row_count)->setStyleClass("row_t");

                //str = new WString(*str+ WString(tr("catalog.table.row.first").arg(WString("{1}/{2}",UTF8).arg(folder).arg(imageName)).arg(c.code).arg(articul).arg(c.shortDescription,UTF8).arg(c.upakovka,UTF8).arg("5.24").arg(5.236).arg(color)));
                firsRow = false;
            }
            else
            {
                /// Вывод обычной строки с товаром
                //table->elementAt(row_count, 0)->addWidget(new WText());

                //str = new WString(*str+ WString(tr("catalog.table.row").arg(0).arg(c.code).arg(articul).arg(c.shortDescription,UTF8).arg(c.upakovka,UTF8).arg("7.08").arg(5.236).arg(color)));
            }

            WString articul = WString::fromUTF8(c.articul).trim();
            std::string color = "";
            if(articul=="FIT IT")
                color = "light_blue";
            else if(articul=="FIT HQ")
                color = "orange_yellow";
            else if(articul=="SW HQ")
                color = "orange_yellow";
            else if(articul=="WP HQ")
                color = "orange_yellow";
            else if(articul=="WH HQ")
                color = "orange_yellow";
            else if(articul==WString("РОС",UTF8))
                color = "light_magenta";
            else
                color = "light_gray";

            WContainerWidget *div = new WContainerWidget();
            new WText(WString::fromUTF8(c.code),table->elementAt(row_count, 1));
            //table->elementAt(row_count, 1)->addWidget(div);
            div = new WContainerWidget();
            new WText( articul,div);

            table->elementAt(row_count, 2)->addWidget(div);
            table->elementAt(row_count, 2)->setToolTip(articul);

            div = new WContainerWidget();
            new WText( WString::fromUTF8(c.shortDescription),div);
            table->elementAt(row_count, 3)->addWidget(div);
            std::string str = c.shortDescription;
            boost::replace_all( str, "<br/>", " " );
            table->elementAt(row_count, 3)->setToolTip(WString::fromUTF8(str));

            div = new WContainerWidget();
            new WText( WString::fromUTF8(c.upakovka),div);
            table->elementAt(row_count, 4)->addWidget(div);
            table->elementAt(row_count, 4)->setToolTip(WString::fromUTF8(c.upakovka));
            div = new WContainerWidget();
            char number[50];
            sprintf(number,"%.2f",c.price);
            table->elementAt(row_count, 5)->addWidget(new WText(number));
            table->elementAt(row_count, 5)->setToolTip(WString::fromUTF8(number));
            div = new WContainerWidget();
            //div->setStyleClass("amount");
            WLineEdit *edit = new WLineEdit("0",div);
            edit->setStyleClass("span1");
            edit->setTabIndex(tabIndex++);
            //edit->setId(c.code);
            //log("info")<<"****** price "<<c.price<<" NAME "<<c.name<<" CODE "<<c.code;
            if(c.price)
            {
                ///edit->setValidator(new Wt::WIntValidator(0, 200));
                edit->setText(WString("{1}").arg(c.quantity));
                if(c.quantity)
                    edit->setStyleClass("blue");
                //edit->clicked().connect(boost::bind(&CatalogView::changed, this, edit));
                //edit->changed().connect(boost::bind(&CatalogView::changed, this, edit));
                //edit->enterPressed().connect(boost::bind(&CatalogView::changed, this, edit));
                edit->blurred().connect(boost::bind(&CatalogView::changed, this, edit));

                WString javaScr("$('input[tabindex={1}]').focus();");
                javaScr.arg(tabIndex);
                edit->enterPressed().connect(std::bind([=](){edit->doJavaScript(javaScr.toUTF8());}));

                //m_editStack.push_back(edit);
            }
            else
            {
                edit->setReadOnly(true);
                edit->hide();
            }

            //edit->enterPressed().connect(SLOT(this, &CatalogMainPage::search));

            table->elementAt(row_count, 6)->addWidget(div);

            table->elementAt(row_count, 0)->setStyleClass("cell_1");
            table->elementAt(row_count, 1)->setStyleClass("cell_2");


            if(!m_SearchString.empty())
            {
                if(WString(c.code,UTF8).trim() == m_SearchString)
                {
                    //log("info")<<"************************************************************"<<m_SearchString;
                    table->elementAt(row_count, 1)->setStyleClass("cell_2 mark");
                    edit->setFocus();
                    //m_SearchString = "";
                }
            }


            table->elementAt(row_count, 2)->setStyleClass("cell_3 "+color);
            table->elementAt(row_count, 3)->setStyleClass("cell_4");
            table->elementAt(row_count, 4)->setStyleClass("cell_5");
            table->elementAt(row_count, 5)->setStyleClass("cell_6");
            table->elementAt(row_count, 6)->setStyleClass("cell_7");
            row_count++;
        }


    };


    row_count = table->rowCount();
    //if(i> m_lastRecord && !catalogs[i-1].isGroup)
    if(catalogs[catalogs.size()-1].isGroup && catalogs[catalogs.size()-1].notVisible)
    {
        description = "";
    }


    if(name != "" && !groupCount)
    {
        //if(description.empty())
        //    description = " </br>";
        table->elementAt(row_count, 0)->addWidget(new WText());
        table->elementAt(row_count, 1)->addWidget(new WText(WString("<b>"+name+"</b><br/>"+description)));
        table->elementAt(row_count, 1)->setColumnSpan(6);
        table->elementAt(row_count, 1)->setStyleClass("txt");

        //str = new WString(*str+ WString(tr("portal.catalog.description")).arg(name,UTF8).arg(description,UTF8));
        name = "";
        description = "";
    }else

    //if(catalogs[catalogs.size()-1].isGroup && !catalogs[catalogs.size()-1].notVisible)
    if(groupCount)
    {

        for(int i=groupCount;i>0;i--)
            table->deleteRow(table->rowCount()-1);

    }

    m_lastRecord = i;
    //str = new WString(*str+ WString(tr("portal.catalog.description")).arg(name,UTF8).arg(description,UTF8));
    //WString *headers = new WString(tr("catalog.table.header"));
    //headers->arg(*str);
    //WText *table = new WText(*headers);
    //table->setStyleClass("cat_tab");
    table->setStyleClass("table");

    //WText *headers =  new WText(WString("catalog.table.title.1").arg("FIT"),m_content);
    //headers += new WText("catalog.table.title.2").arg(new WString().fromUTF8("МАЛЯРНЫЙ И ШТУКАТУРНО-ОТДЕЛОЧНЫЙ ИНСТРУМЕНТ"));
    //result->bindWidget("body", headers);

    return table;
} // createCatalogTable()

void CatalogView::setSearchString(const WString& str,bool &findByCode)
{
#ifdef DEBUG
    log("info") << "CatalogView::setSearchString ("<<str <<")";
#endif
    m_currentPage = 1;
    m_SearchString = str;
    m_findByCode = findByCode;
    //update();
}

void CatalogView::update()
{
#ifdef DEBUG
    log("info") << "***************** UPDATE ***************"<<m_SearchString;
#endif
    //m_editStack.clear();
    clear();
    //addWidget(renderView());
    renderView();

}

//WWidget *CatalogView::indexOf(WWidget widget)
//{
//    return 0;
//}

void CatalogView::changed(WWidget* sender)
{
#ifdef DEBUG
    log("info") << "*************** CHANGED *****************";
    log("info") << sender->id();
    log("info") << sender->objectName();
#endif
    WLineEdit *edit = dynamic_cast<WLineEdit*>(sender);
    if(edit)
    {
        int quantity = atoi(edit->text().toUTF8().c_str());
        float price = 0;
        std::string code;
        if(WTableCell *cell = dynamic_cast<WTableCell*>(sender->parent()->parent()))
        {
            if(WText *text = dynamic_cast<WText*>(cell->tableRow()->elementAt(5)->widget(0)))
                price = atof(text->text().toUTF8().c_str());
            if(WText *text = dynamic_cast<WText*>(cell->tableRow()->elementAt(1)->widget(0)))
                code = text->text().toUTF8();
            /*
            int maxRow = cell->table()->rowCount();
            int currentRow = std::min(maxRow,cell->tableRow()->rowNum()+1);
                    log("info")<<__LINE__;
            if(WContainerWidget *div = dynamic_cast<WContainerWidget*>(cell->table()->elementAt(currentRow,6)->widget(0)))
            {
                    log("info")<<__LINE__;
                if(WLineEdit *nedit = dynamic_cast<WLineEdit*>(div->widget(0)))
                {
                    log("info")<<__LINE__;
                    nedit->setFocus();
                }

            }
            */
        }
#ifdef DEBUG
        log("info") <<"goods "<< code <<" quantity "<< quantity<<" Price "<<price;
#endif
        if (quantity)
        {
            edit->setStyleClass("blue");
            //dbo::ptr<GoodsSimple> goods= session_.goods(edit->id());//find<GoodsSimple>().where("code = ?").bind
            //OrderRecords *record = new OrderRecords();
            //record->goods = goods;
            //log("info")<<"Goods is "<<record->goods;
            //record->quantity = quantity;
            ((Portal *)WApplication::instance())->getSession()->addOrderRecords(quantity,code,price);
        }

    }
    WImage *image = dynamic_cast<WImage*>(sender);
    if(image)
    {
        if(image->objectName() == "imageView")
        {
#ifdef DEBUG
            log("info") << "*** IMAGE VIEW***";
#endif
            image->parent()->parent()->parent()->parent()->setHidden(true);
        }
        else
        {
#ifdef DEBUG
            log("info") << "*** IMAGE ***";
#endif
            WDialog *imgDia = new WDialog(image->objectName());
            imgDia->setClosable(true);
            imgDia->setModal(false);
            //WImage *img = new WImage(imgDia->conent());
            WContainerWidget *wc = new WContainerWidget();
            WImage *img = new WImage(image->imageLink());
            wc->addWidget(img);
            img->setObjectName("imageView");
            img->clicked().connect(boost::bind(&CatalogView::changed, this, img));
            imgDia->contents()->addWidget(wc);
            //imgDia->
            imgDia->show();
        }

    }

    //m_currentPage++;
    //if(sender->id()=="")
    //log("info")<<"Current page "<<m_currentPage;
    //update();
}
