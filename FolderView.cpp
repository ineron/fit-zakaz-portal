
/*
 * Copyright (C) 2008 Emweb bvba, Kessel-Lo, Belgium.
 *
 * See the LICENSE file for terms of use.
 */
#include "FolderView.h"


#include <iostream>

#include <Wt/WAbstractItemModel>
#include <Wt/WItemSelectionModel>
#include <Wt/WMessageBox>


using namespace Wt;

const char *FolderView::FileSelectionMimeType
  = "application/x-computers-selection";

FolderView::FolderView(Wt::WContainerWidget *parent)
  : WTreeView(parent)
{
  /*
   * Accept drops for the custom mime type.
   */
  acceptDrops(FileSelectionMimeType);
}

void FolderView::dropEvent(const Wt::WDropEvent& event,
			    const Wt::WModelIndex& target)
{
  /*
   * We reimplement the drop event to handle the dropping of a
   * selection of computers.
   *
   * The test below would always be true in this case, since we only
   * indicated support for that particular mime type.
   */
  if (event.mimeType() == FileSelectionMimeType) {
    /*
     * The source object for a drag of a selection from a WTreeView is
     * a WItemSelectionModel.
     */
    WItemSelectionModel *selection
      = dynamic_cast<WItemSelectionModel *>(event.source());

#ifdef WT_THREADED
    int result = WMessageBox::show
      ("Drop event",
       "Move "
       + boost::lexical_cast<std::string>(selection->selectedIndexes().size())
       + " files to folder '"
       + boost::any_cast<WString>(target.data(DisplayRole)).toUTF8()
       + "' ?",
       Yes | No);
#else
    int result = Yes;
#endif

    if (result == Yes) {
      /*
       * You can access the source model from the selection and
       * manipulate it.
       */
      WAbstractItemModel *sourceModel = selection->model();

      WModelIndexSet toChange = selection->selectedIndexes();

      for (WModelIndexSet::reverse_iterator i = toChange.rbegin();
	   i != toChange.rend(); ++i) {
	WModelIndex index = *i;

	/*
	 * Copy target folder to file. Since we are using a
	 * dynamic WSortFilterProxyModel that filters on folder, this
	 * will also result in the removal of the file from the
	 * current view.
	 */
	std::map<int, boost::any> data = model()->itemData(target);
	data[DecorationRole] = index.data(DecorationRole);
	sourceModel->setItemData(index, data);
      }
    }
  }
}
