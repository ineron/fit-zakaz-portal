#include "PriceEditor.h"
#include "Portal.h"
#include "FolderView.h"
#include "PriceEditDialog.h"
#include "./utils/CsvUtil.h"


#include <fstream>

#include <Wt/WApplication>
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
#include <Wt/WVBoxLayout>
#include <Wt/WPopupMenu>
#include <Wt/WSortFilterProxyModel>
#include <Wt/WStandardItem>
#include <Wt/WStandardItemModel>
#include <Wt/WTableView>
#include <Wt/WTreeView>
#include <Wt/WTable>
#include <Wt/WText>




WString FileModel::dateDisplayFormat(WString::fromUTF8("MMM dd yyyy"));
WString FileModel::dateEditFormat(WString::fromUTF8("dd.MM.yyyy"));


PriceEditor::PriceEditor(WContainerWidget *parent):
WContainerWidget(parent),
folderModel_(0),
fileModel_(0),
fileFilterModel_(0),
folderView_(0),
fileView_(0),
popup_(0)
{



    setWidth(900);
    setHeight(700);
    folderModel_ = new WStandardItemModel(0, 1, this);
    populateFolders();


    fileModel_ = new FileModel(this);

    fileModel_->insertColumns(0,9);

    //fileModel_->dataChanged().connect(this,&PriceEditor::onDataChange);
    fileModel_->setHeaderData(1, boost::any(WString::fromUTF8("Ред.").toUTF8()));
    fileModel_->setHeaderData(2, boost::any(WString::fromUTF8("Код").toUTF8()));
    fileModel_->setHeaderData(3, boost::any(WString::fromUTF8("Наименование").toUTF8()));
    fileModel_->setHeaderData(4, boost::any(WString::fromUTF8("Закуп.").toUTF8()));
    fileModel_->setHeaderData(5, boost::any(WString::fromUTF8("Наценка").toUTF8()));
    fileModel_->setHeaderData(6, boost::any(WString::fromUTF8("Розн.").toUTF8()));

    populateFiles("da292230-f06b-6992-e81b-0285c2224d10");

    /*
      The header items are also endered using an ItemDelegate, and thus
      support other data, e.g.:
      fileModel_->setHeaderFlags(0, Horizontal, HeaderIsUserCheckable);
      fileModel_->setHeaderData(0, Horizontal,
                                std::string("icons/file.gif"),
			        Wt::DecorationRole);
    */

    fileFilterModel_ = new WSortFilterProxyModel(this);
    fileFilterModel_->setSourceModel(fileModel_);
    fileFilterModel_->setDynamicSortFilter(true);
    fileFilterModel_->setFilterKeyColumn(0);
    fileFilterModel_->setFilterRole(UserRole);

    createUI();
}

PriceEditor::~PriceEditor()
{
    //dtor
}

void PriceEditor::createUI()
{
    setStyleClass("maindiv");

    /*
     * The main layout is a 3x2 grid layout.
     */


    WGridLayout *layout = new WGridLayout();
    layout->addWidget(createTitle(WString("Каталог",UTF8)), 0, 0);
    layout->addWidget(createTitle(WString("Список элементов",UTF8)), 0, 1);
    layout->addWidget(folderView(), 1, 0);
    layout->setColumnResizable(0);


    // select the first folder
    folderView_->select(folderModel_->index(0, 0, folderModel_->index(0, 0)));

    WVBoxLayout *vbox = new WVBoxLayout();
    vbox->addWidget(fileView(), 1);//fileView()
    //vbox->addWidget(new WText("pieChart"), 1);//pieChart()
    //vbox->setResizable(0);

    layout->addLayout(vbox, 1, 1);


    //layout->addWidget(new WText("About"), 2, 0, 1, 2);//aboutDisplay()

    /*
     * Let row 1 and column 1 take the excess space.
     */
    layout->setRowStretch(1, 1);
    layout->setColumnStretch(1, 1);


    setLayout(layout);
}

/*! \brief Creates a title widget.
 */
WText *PriceEditor::createTitle(const WString& title)
{
    WText *result = new WText(title);
    result->setInline(false);
    result->setStyleClass("title");

    return result;
}

WTableView *PriceEditor::fileView() {
    WTableView *tableView = new WTableView();
    //WTable *tableView = new WTable();

    tableView->setAlternatingRowColors(true);

    tableView->setModel(fileFilterModel_);
    tableView->setSelectionMode(ExtendedSelection);
    tableView->setDragEnabled(false);

    tableView->setColumnWidth(0, 15);
    tableView->setColumnWidth(1, 20);
    tableView->setColumnWidth(2, 60);
    tableView->setColumnWidth(3, 240);
    tableView->setColumnWidth(4, 60);
    tableView->setColumnWidth(5, 60);
    tableView->setColumnWidth(6, 60);
    //tableView->setColumnWidth(7, 100);

    //tableView->setColumnHidden(6,true);
    tableView->setColumnHidden(7,true);
    tableView->setColumnHidden(8,true);
    //tableView->setColumnHidden(6);


    WItemDelegate *delegate = new WItemDelegate(this);
    delegate->setTextFormat(FileModel::dateDisplayFormat);
    tableView->setItemDelegateForColumn(4, delegate);
    //tableView->setItemDelegateForColumn(5, delegate);

    tableView->setColumnAlignment(4, AlignRight);
    tableView->setColumnAlignment(5, AlignRight);
    tableView->setColumnAlignment(6, AlignRight);
    //tableView->setColumnAlignment(5, AlignRight);

    //tableView->sortByColumn(0, AscendingOrder);



    //tableView->mouseWentUp().connect(this, &PriceEditor::showPopup);
    tableView->clicked().connect(this, &PriceEditor::editFile);
    //tableView->keyWentUp().connect(this, &PriceEditor::keyUp);
    tableView->setAttributeValue("oncontextmenu","event.cancelBubble = true; event.returnValue = false; return false;");

    fileView_ = tableView;

    return tableView;
  }

WTreeView *PriceEditor::folderView()
{
    WTreeView *treeView = new FolderView();

    /*
     * To support right-click, we need to disable the built-in browser
     * context menu.
     *
     * Note that disabling the context menu and catching the
     * right-click does not work reliably on all browsers.
     */
    treeView->setAttributeValue("oncontextmenu", "event.cancelBubble = true; event.returnValue = false; return false;");
    treeView->setModel(folderModel_);
    treeView->resize(300, WLength::Auto);
    treeView->setSelectionMode(SingleSelection);
    treeView->expandToDepth(1);
    treeView->selectionChanged().connect(this, &PriceEditor::folderChanged);

    //treeView->mouseWentUp().connect(this, &PriceEditor::showPopup);
    //treeView->clicked().connect(this, &PriceEditor::treeItemSelect);

    folderView_ = treeView;

    return treeView;
}

/*! \brief Populate the folders model.
 */
void PriceEditor::populateFolders()
{
/*
    WStandardItem *level1, *level2,*level3,*level4;


    folderModel_->appendRow(level1 = createFolderItem("San Fransisco"));
    level1->appendRow(level2 = createFolderItem("Investors", "sf-investors"));
    level1->appendRow(level2 = createFolderItem("Fellows", "sf-fellows"));


    folderModel_->appendRow(level1 = createFolderItem("Sophia Antipolis"));
    level1->appendRow(level2 = createFolderItem("R&D", "sa-r_d"));
    level1->appendRow(level2 = createFolderItem("Services", "sa-services"));
    level1->appendRow(level2 = createFolderItem("Support", "sa-support"));
    level1->appendRow(level2 = createFolderItem("Billing", "sa-billing"));


    folderModel_->appendRow(level1 = createFolderItem("New York"));
    level1->appendRow(level2 = createFolderItem("Marketing", "ny-marketing"));
    level1->appendRow(level2 = createFolderItem("Sales", "ny-sales"));
    level1->appendRow(level2 = createFolderItem("Advisors", "ny-advisors"));


    folderModel_->appendRow(level1 = createFolderItem(WString::fromUTF8("Frankfürt")));
    level1->appendRow(level2 = createFolderItem("Sales", "frank-sales"));
*/

    ((Portal *)WApplication::instance())->getSession()->getFoldersModel(folderModel_,folderIdNameMap_);
    //log("info")<<"*** MAP SIZE *****"<<folderIdNameMap_.size();

    folderModel_->setHeaderData(0, Horizontal,boost::any(WString("Группы",UTF8).toUTF8()));


}

/*! \brief Create a folder item.
 *
 * Configures flags for drag and drop support.
 */
WStandardItem *PriceEditor::createFolderItem(const WString& location,
        const std::string& folderId)
{
    WStandardItem *result = new WStandardItem(location);
/*
    if (!folderId.empty())
    {
        result->setData(boost::any(folderId));
        result->setFlags(result->flags() | ItemIsDropEnabled);
        folderNameMap_[folderId] = location;
    }
    else
        result->setFlags(result->flags().clear(ItemIsSelectable));

    result->setIcon("icons/folder.gif");
*/
    return result;
}

/*! \brief Convert a string to a date.
 */
void PriceEditor::convertToDate(WStandardItem *item)
{
    WDate d = WDate::fromString(item->text(), FileModel::dateEditFormat);
    item->setData(boost::any(d), DisplayRole);
}

/*! \brief Populate the files model.
 *
 * Data (and headers) is read from the CSV file data/files.csv. We
 * add icons to the first column, resolve the folder id to the
 * actual folder name, and configure item flags, and parse date
 * values.
 */
void PriceEditor::populateFiles(std::string parent)
{
    fileModel_->invisibleRootItem()->setRowCount(0);

        ((Portal *)WApplication::instance())->getSession()->getGoodsModel( parent,fileModel_);
/*
        for (int i = 0; i < fileModel_->rowCount(); ++i)
        {
            //WStandardItem *item = fileModel_->item(i, 0);
            //item->setFlags(item->flags() | ItemIsDragEnabled);
            //item->setIcon("icons/file.gif");
            //item->setIcon("icons/folder.gif");


            std::string folderId = item->text().toUTF8();

            item->setData(boost::any(folderId), UserRole);
            item->setText(folderNameMap_[folderId]);

            //convertToDate(fileModel_->item(i, 4));
            //convertToDate(fileModel_->item(i, 5));
        }
*/


}

void PriceEditor::dialogDone()
{
    delete popupActionBox_;
    popupActionBox_ = 0;
}

/** \brief Process the result of the popup menu
 */
void PriceEditor::popupAction()
{

    if (popup_->result())
    {

         //You could also bind extra data to an item using setData() and
         //check here for the action asked. For now, we just use the text.

        WString text = popup_->result()->text();
        popup_->hide();

        popupActionBox_ = new WMessageBox("Sorry.","Action '" + text
                                          + "' is not implemented.", NoIcon, Ok);
        popupActionBox_->buttonClicked().connect(this, &PriceEditor::dialogDone);
        popupActionBox_->show();
    }
    else
    {
        popup_->hide();
    }

}

/*! \brief Change the filter on the file view when the selected folder
 *         changes.
 */
void PriceEditor::folderChanged()
{
    if (folderView_->selectedIndexes().empty())
        return;

    WModelIndex selected = *folderView_->selectedIndexes().begin();
    boost::any d = selected.data(UserRole);
    if (!d.empty())
    {
        std::string folder = boost::any_cast<std::string>(d);
        //log("info")<<" folder "<<folder<<" ---"<<__LINE__;
        // For simplicity, we assume here that the folder-id does not
        // contain special regexp characters, otherwise these need to be
        // escaped -- or use the \Q \E qutoing escape regular expression
        // syntax (and escape \E)
        populateFiles(folder);
        //fileFilterModel_->setSourceModel(fileModel_);
        //fileFilterModel_->setFilterRegExp(WString("{1}").arg(folder));
    }
}

/*! \brief Show a popup for a folder item.
 */
void PriceEditor::showPopup(const WModelIndex& item, const WMouseEvent& event)
{
    if (event.button() == WMouseEvent::RightButton)
    {
        // Select the item, it was not yet selected.
        if (!folderView_->isSelected(item))
            folderView_->select(item);

        /*
        if (!popup_)
        {
            popup_ = new WPopupMenu();
            popup_->addItem("icons/folder_new.gif", "Create a New Folder");
            popup_->addItem("Rename this Folder")->setCheckable(true);
            popup_->addItem("Delete this Folder");
            popup_->addSeparator();
            popup_->addItem("Folder Details");
            popup_->addSeparator();
            popup_->addItem("Application Inventory");
            popup_->addItem("Hardware Inventory");
            popup_->addSeparator();

            WPopupMenu *subMenu = new WPopupMenu();
            subMenu->addItem("Sub Item 1");
            subMenu->addItem("Sub Item 2");
            popup_->addMenu("File Deployments", subMenu);

            *
             * This is one method of executing a popup, which does not block a
             * thread for a reentrant event loop, and thus scales.
             *
             * Alternatively you could call WPopupMenu::exec(), which returns
             * the result, but while waiting for it, blocks the thread.
             *
            popup_->aboutToHide().connect(this, &PriceEditor::popupAction);
        }

        if (popup_->isHidden())
            popup_->popup(event);
        else
            popup_->hide();
        */
        //editFile(item);
    }
}

void PriceEditor::treeItemSelect(const WModelIndex& item) {
    //FileEditDialog(fileView_->model(), item);
    int row = item.row();
    //int col = item.column();
    //log("info")<<"Col "<<col<<" Row "<<row<<" row count "<<folderModel_->rowCount();
    row = std::min(row,folderModel_->rowCount());
    boost::any id1 = folderModel_->data(row,0,UserRole);
    //boost::any id2 = folderModel_->data(row,1,UserRole);
    //log("info")<<"-ID-"<<asString(id1);
    std::string id = asString(id1).toUTF8().c_str();//boost::any_cast<int>(id1);
    //std::string parent = asString(folderModel_->data(row,1,UserRole)).toUTF8();
    //int percent = 0;//boost::any_cast<int>(fileView_->model()->data(row,2,UserRole));
    //log("info")<<"--"<<id<<"--"<<parent<<"--"<<folderIdNameMap_.size();
    populateFiles(id);
    //WModelIndexList index = folderModel_->match(folderModel_->index(0,0),UserRole,id1);
    WStandardItem *level = dynamic_cast<WStandardItem*>(folderIdNameMap_[id]);
    if(level)
    {
        folderView_->expand(level->parent()->index());
        folderView_->select(level->index());
        folderView_->expand(level->index());
    }

}


void PriceEditor::editFile(const WModelIndex& item) {
    //FileEditDialog(fileView_->model(), item);
    int row = item.row();
    int col = item.column();
    //log("info")<<"Col "<<col<<" Row "<<row<<" row count "<<fileView_->model()->rowCount();
    row = std::min(row,fileView_->model()->rowCount());
    boost::any id1 = fileView_->model()->data(row,0,UserRole);
    boost::any id2 = fileView_->model()->data(row,1,UserRole);
    boost::any id3 = fileView_->model()->data(row,2,UserRole);
    boost::any id4 = fileView_->model()->data(row,4,UserRole);
    //log("info")<<"--"<<asString(id1)<<"--"<<asString(id2)<<"--"<<asString(id3)<<"--"<<asString(id4);
    std::string id = asString(id1).toUTF8().c_str();//boost::any_cast<int>(id1);
    int folder = boost::any_cast<int>(fileView_->model()->data(row,1,UserRole));//boost::any_cast<int>(id2);
//    std::string parent = boost::any_cast<std:string>(fileView_->model()->data(row,5,UserRole));//boost::any_cast<int>(id4);
    std::string parent = asString(fileView_->model()->data(row,5,UserRole)).toUTF8();
    //int percent = 0;//boost::any_cast<int>(fileView_->model()->data(row,2,UserRole));
    //log("info")<<"--"<<id<<"--"<<folder<<"--"<<parent<<"--"<<folderIdNameMap_.size();
    if(!col)
    {
        if(folder==2)
        {
            populateFiles(parent);
            WStandardItem *level = dynamic_cast<WStandardItem*>(folderIdNameMap_[parent]);
            if(level)
            {
                folderView_->select(level->index());
                folderView_->expand(level->index());
            }
            level = dynamic_cast<WStandardItem*>(folderIdNameMap_[id]);
            if(level)
            {
                folderView_->setExpanded(level->index(),false);
            }
        }
        else if (folder==1)
        {
            populateFiles(id);
            //WModelIndexList index = folderModel_->match(folderModel_->index(0,0),UserRole,id1);
            WStandardItem *level = dynamic_cast<WStandardItem*>(folderIdNameMap_[id]);
            if(level)
            {
                folderView_->expand(level->parent()->index());
                folderView_->select(level->index());
                folderView_->expand(level->index());
            }
        }
    //}else{
        //new PriceEditDialog(fileView_->model(), item);
        //onDataChange(item);
    }else{
        if(col == 1 && folder!=2)
            new PriceEditDialog(fileView_->model(), item);
    }


  }

void PriceEditor::keyUp(const WKeyEvent &e)
{
    //tableView
    log("notice")<<" Key "<<e.key();
}

void PriceEditor::onDataChange(const WModelIndex& item)
{
    //log("info")<<"******* <!!!> "<<firstItem.row()<<" col "<<firstItem.column();

}
