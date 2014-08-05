#include "DocumentDetails.h"

#include <Wt/WText>

#include <Wt/WStandardItemModel>
#include <Wt/WStandardItem>
#include <Wt/WLabel>

//WScrollArea scroll = new WScrollArea(dialog.contents())

DocumentDetails::DocumentDetails(WContainerWidget *parent): WTable(parent),m_pModel(0)
{
    setStyleClass("table table-bordered table-striped");
    m_pModel = new WStandardItemModel(0,0);
    createUI();
}

DocumentDetails::~DocumentDetails()
{
    //dtor
}

void DocumentDetails::createUI()
{

}


WStandardItemModel *DocumentDetails::getModel()
{
    return m_pModel;
}

void DocumentDetails::setModel(WStandardItemModel *model)
{
    m_pModel = model;
    updateUi();
}

void DocumentDetails::updateUi()
{
    clear();
    setHeaderCount(1);
    elementAt(0,0)->addWidget(new WText(tr("docdetail.realization.header.coll1")));
    elementAt(0,1)->addWidget(new WText(tr("docdetail.realization.header.coll2")));
    elementAt(0,2)->addWidget(new WText(tr("docdetail.realization.header.coll3")));
    elementAt(0,3)->addWidget(new WText(tr("docdetail.realization.header.coll4")));
    elementAt(0,4)->addWidget(new WText(tr("docdetail.realization.header.coll5")));
    elementAt(0,5)->addWidget(new WText(tr("docdetail.realization.header.coll6")));

    for (int i=0;i<m_pModel->rowCount();i++)
    {
        elementAt(i+1,0)->addWidget(new WText(m_pModel->item(i, 0)->text()));
        elementAt(i+1,1)->addWidget(new WText(m_pModel->item(i, 1)->text()));
        elementAt(i+1,2)->addWidget(new WText(m_pModel->item(i, 2)->text()));
        elementAt(i+1,3)->addWidget(new WText(m_pModel->item(i, 3)->text()));

        char number[50];
        sprintf(number,"%.2f",atof(m_pModel->item(i, 4)->text().toUTF8().c_str()));
        elementAt(i+1,4)->addWidget(new WText(number));
        sprintf(number,"%.2f",atof(m_pModel->item(i, 5)->text().toUTF8().c_str()));
        elementAt(i+1,5)->addWidget(new WText(number));
        elementAt(i+1,0)->setContentAlignment(AlignTop | AlignCenter);
        elementAt(i+1,1)->setContentAlignment(AlignTop | AlignCenter);
        elementAt(i+1,2)->setContentAlignment(AlignTop | AlignLeft);
        elementAt(i+1,3)->setContentAlignment(AlignTop | AlignRight);
        elementAt(i+1,4)->setContentAlignment(AlignTop | AlignRight);
        elementAt(i+1,5)->setContentAlignment(AlignTop | AlignRight);


    }
    refresh();
}
