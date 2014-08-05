#ifndef _COMBOBOX_ITEMDELEGATE_H_INCLUDED
#define _COMBOBOX_ITEMDELEGATE_H_INCLUDED

#include <boost/cast.hpp>

#include <Wt/WItemDelegate>
#include <Wt/WComboBox>

#include <vector>


class ComboBoxItemDelegate : public WItemDelegate
{
    void OnCloseEditor (Wt::WWidget* editor, bool save) const
    {
        closeEditor().emit(editor, save);
    }
    WAbstractItemModel *itemsModel;
public:
    ComboBoxItemDelegate (WAbstractItemModel *model,WObject* parent = nullptr)
        :WItemDelegate(parent),itemsModel(model)
    {
    }

    WWidget* createEditor (const WModelIndex& index, WFlags<ViewItemRenderFlag>) const override
    {
        auto container = new Wt::WContainerWidget;
        container->setOverflow(WContainerWidget::OverflowVisible);
        container->setPopup(true);

        auto cb = new WComboBox(container);
        cb->setModel(itemsModel);
        cb->setCurrentIndex(cb->findText(asString(index.data(EditRole))));

        cb->activated().connect(std::bind(&ComboBoxItemDelegate::OnCloseEditor, this, container, true));
        cb->setFocus();

        return container;
    }

    void setEditState (WWidget* editor, const boost::any& value) const override
    {
        auto container = boost::polymorphic_cast<WContainerWidget*>(editor);
        auto cb = boost::polymorphic_cast<WComboBox*>(container->widget(0));

        cb->setCurrentIndex(cb->findText(asString(value)));
    }

    boost::any editState (WWidget* editor) const override
    {
        auto container = boost::polymorphic_cast<WContainerWidget*>(editor);
        auto cb = boost::polymorphic_cast<WComboBox*>(container->widget(0));

        return cb->currentText();
    }
};


#endif // _COMBOBOX_ITEMDELEGATE_H_INCLUDED
