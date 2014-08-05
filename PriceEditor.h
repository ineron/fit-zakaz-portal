#ifndef PRICEEDITOR_H
#define PRICEEDITOR_H

#include "FolderView.h"

#include <Wt/WContainerWidget>
#include <Wt/WStandardItemModel>


using namespace Wt;

//class Wt::WStandardItemModel;


/*! \class FileModel
 *  \brief A specialized standard item model which report a specific
 *         drag and drop mime type.
 *
 * A specific drag and drop mime type instead of the generic abstract
 * item model is returned by the model.
 */
class FileModel : public WStandardItemModel
{
public:
    /*! \brief Constructor.
     */
    FileModel(WObject *parent):WStandardItemModel(parent) {}

    /*! \brief Return the mime type.
     */
    virtual std::string mimeType() const
    {
        return FolderView::FileSelectionMimeType;
    }

    /// Date display format.
    static WString dateDisplayFormat;

    /// Date edit format.
    static WString dateEditFormat;
};

class PriceEditor : public Wt::WContainerWidget
{
public:
    PriceEditor(WContainerWidget *parent=0);
    virtual ~PriceEditor();
protected:
    void showPopup(const WModelIndex& item, const WMouseEvent& event);
    void folderChanged();
    void onDataChange(const WModelIndex& item);
    void keyUp(const WKeyEvent &e);
private:
    /// The folder model (used by folderView_)
    WStandardItemModel    *folderModel_;

    std::map<std::string, WStandardItem*> folderIdNameMap_;
    /// The file model (used by fileView_)
    WStandardItemModel    *fileModel_;

    /// The sort filter proxy model that adapts fileModel_
    WSortFilterProxyModel *fileFilterModel_;

    /// Maps folder id's to folder descriptions.
    std::map<std::string, WString> folderNameMap_;

    /// The folder view.
    WTreeView *folderView_;

    /// The file view.
    WTableView *fileView_;

    /// Popup menu on the folder view
    WPopupMenu *popup_;

    /// Message box to confirm the poup menu action
    WMessageBox *popupActionBox_;

private:

    WStandardItem *createFolderItem(const WString& location,const std::string& folderId = std::string());
    void populateFolders();
    void convertToDate(WStandardItem *item);
    void populateFiles(std::string parent);
    void createUI();
    WText *createTitle(const WString& title);
    WTreeView *folderView();
    WTableView *fileView();
    void editFile(const WModelIndex& item);
    void treeItemSelect(const WModelIndex& item);
    void popupAction();
    void dialogDone();
};

#endif // PRICEEDITOR_H
