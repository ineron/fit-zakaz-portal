#include "UserProfile.h"

#include <Wt/WApplication>
#include <Wt/WBreak>
#include <Wt/WDatePicker>
#include <Wt/WSelectionBox>
#include <Wt/WContainerWidget>
#include <Wt/WImage>
#include <Wt/WIntValidator>
#include <Wt/WLabel>
#include <Wt/WLineEdit>
#include <Wt/WPushButton>
#include <Wt/WTableCell>
#include <Wt/WTextArea>
#include <Wt/WText>

#include <Wt/WStandardItemModel>
#include <Wt/WStandardItem>
UserProfile::UserProfile(WContainerWidget *parent): WTable(parent),m_pModel(0)
{
    m_pModel = new WStandardItemModel(0,0);
    createUI();
}

UserProfile::~UserProfile()
{
    //dtor
}

void UserProfile::createUI()
{
  WLabel *label;
  int row = 0;

  // Title
  elementAt(row, 0)->setColumnSpan(3);
  elementAt(row, 0)->setContentAlignment(AlignTop | AlignCenter);
  elementAt(row, 0)->setPadding(10);
  WText *title = new WText(tr("profile.form"),
			   elementAt(row, 0));
  title->decorationStyle().font().setSize(WFont::XLarge);

  // error messages
  ++row;
  elementAt(row, 0)->setColumnSpan(3);
  feedbackMessages_ = elementAt(row, 0);
  feedbackMessages_->setPadding(5);

  WCssDecorationStyle& errorStyle = feedbackMessages_->decorationStyle();
  errorStyle.setForegroundColor(Wt::red);
  errorStyle.font().setSize(WFont::Smaller);
  errorStyle.font().setWeight(WFont::Bold);
  errorStyle.font().setStyle(WFont::Italic);

  // Name
  ++row;
  nameEdit_ = new WLineEdit(elementAt(row, 2));
  label = new WLabel(tr("profile.name"), elementAt(row, 0));
  label->setBuddy(nameEdit_);
  nameEdit_->setValidator(new WValidator(true));
  nameEdit_->enterPressed().connect(this, &UserProfile::submit);
  nameEdit_->setWidth(200);

  // First name
  ++row;
  fullName_ = new WLineEdit(elementAt(row, 2));
  label = new WLabel(tr("profile.fullname"), elementAt(row,0));
  label->setBuddy(fullName_);
  fullName_->setWidth(200);

  // Country
  ++row;
  countryEdit_ = new WComboBox(elementAt(row, 2));
  countryEdit_->addItem("");
  countryEdit_->addItem(tr("russia"));
  countryEdit_->addItem(tr("ukraina"));
  countryEdit_->addItem(tr("kazahstan"));

  label = new WLabel(tr("profile.country"), elementAt(row, 0));
  label->setBuddy(countryEdit_);
  countryEdit_->setValidator(new WValidator(true));
  countryEdit_->changed().connect(this, &UserProfile::countryChanged);

  // City
  ++row;
  cityEdit_ = new WComboBox(elementAt(row, 2));
  cityEdit_->addItem(tr("profile.choosecountry"));
  label = new WLabel(tr("profile.city"), elementAt(row, 0));
  label->setBuddy(cityEdit_);

/*
  // Birth date
  ++row;

  birthDateEdit_ = new WDatePicker(elementAt(row, 2));
  birthDateEdit_->setBottom(WDate(1900, 1, 1));
  birthDateEdit_->setTop(WDate::currentDate());
  label = new WLabel(tr("profile.birthdate"), elementAt(row, 0));
  label->setBuddy(birthDateEdit_->lineEdit());
  birthDateEdit_->setFormat("dd/MM/yyyy");
  birthDateEdit_->lineEdit()->validator()->setMandatory(true);
*/
  // Child count
  ++row;
  phoneEdit_ = new WLineEdit(elementAt(row, 2));
  label = new WLabel(tr("profile.phones"),elementAt(row, 0));
  label->setBuddy(phoneEdit_);
  phoneEdit_->setValidator(new WValidator(true));
  phoneEdit_->validator()->setMandatory(true);
  phoneEdit_->setWidth(200);

  ++row;
  addresEdit_ = new WLineEdit(elementAt(row, 2));
  label = new WLabel(tr("profile.addres"),elementAt(row, 0));
  label->setBuddy(addresEdit_);
  addresEdit_->setValidator(new WValidator(true));
  addresEdit_->validator()->setMandatory(true);
  addresEdit_->setWidth(200);


  ++row;
  remarksEdit_ = new WTextArea(elementAt(row, 2));
  remarksEdit_->setColumns(40);
  remarksEdit_->setRows(5);
  label = new WLabel(tr("profile.remarks"),
		     elementAt(row, 0));
  label->setBuddy(remarksEdit_);

/*
  // Submit
  ++row;
  WPushButton *submit = new WPushButton(tr("submit"),
					elementAt(row, 0));
  submit->clicked().connect(this, &UserProfile::submit);
  submit->setMargin(15, Top);

  WPushButton *cancel = new WPushButton(tr("cancel"),
					elementAt(row, 1));
  cancel->clicked().connect(this, &UserProfile::cancel);
  cancel->setMargin(15, Top);

  elementAt(row, 0)->setColumnSpan(3);
  elementAt(row, 0)->setContentAlignment(AlignTop | AlignCenter);
*/
  // Set column widths for label and validation icon
  elementAt(2, 0)->resize(WLength(30, WLength::FontEx), WLength::Auto);
  elementAt(2, 1)->resize(20, WLength::Auto);
}

void UserProfile::countryChanged()
{
  cityEdit_->clear();
  cityEdit_->addItem("");
  cityEdit_->setCurrentIndex(-1);

  switch (countryEdit_->currentIndex()) {
  case 0:
    break;
  case 1:
    cityEdit_->addItem(tr("moskow"));
    cityEdit_->addItem(tr("spb"));
    cityEdit_->addItem(tr("novosibirsk"));
    cityEdit_->addItem(tr("ekaterinburg"));
    cityEdit_->addItem(tr("khabarovsk"));
    cityEdit_->addItem(tr("stavropol"));
    cityEdit_->addItem(tr("pyatigorsk"));
    break;
  case 2:
    cityEdit_->addItem(tr("kiev"));
    cityEdit_->addItem(tr("harkov"));
    cityEdit_->addItem(tr("odesa"));
    cityEdit_->addItem(tr("dnepropetrovsk"));
    cityEdit_->addItem(tr("doneck"));
    break;
  case 3:
    cityEdit_->addItem(tr("almaata"));
    cityEdit_->addItem(tr("astana"));
    cityEdit_->addItem(tr("shimkent"));
    cityEdit_->addItem(tr("karaganda"));
    break;
  }
}

bool UserProfile::checkValid(WFormWidget *edit, const WString& text)
{
  if (edit->validate() != WValidator::Valid) {
    feedbackMessages_->addWidget(new WText(text));
    feedbackMessages_->addWidget(new WBreak());
    edit->label()->decorationStyle().setForegroundColor(Wt::red);
    edit->setStyleClass("Wt-invalid");

    return false;
  } else {
    edit->label()->decorationStyle().setForegroundColor(WColor());
    edit->setStyleClass("");

    return true;
  }
}

bool UserProfile::validate()
{
  feedbackMessages_->clear();
  bool valid = true;

  if (!checkValid(nameEdit_, tr("error.name")))
    valid = false;
  if (!checkValid(countryEdit_, tr("error.country")))
    valid = false;
  //if (!checkValid(birthDateEdit_->lineEdit(), tr("error.birthdate")))
  //  valid = false;
  if (!checkValid(phoneEdit_, tr("error.childcount")))
    valid = false;

  return valid;
}

void UserProfile::submit()
{
  if (validate()) {
    // do something useful with the data...
    std::wstring name
      = fullName_->text() + L" " + nameEdit_->text();

    std::wstring remarks
      = remarksEdit_->text();

    clear();

    new WText(WString::fromUTF8("<p>Thank you, {1}, "
				"for all this precious data.</p>").arg(name),
	      elementAt(0, 0));

    if (!remarks.empty())
      new WText("<p>You had some remarks. Splendid !</p>", elementAt(0, 0));

    //wApp->quit();
  }
}

WStandardItemModel *UserProfile::getModel()
{
    m_pModel->clear();
    for(int i=0;i<rowCount();i++)
    {
        m_pModel->insertRows(m_pModel->rowCount(),1);
        WStandardItem *item = new WStandardItem();
        if(dynamic_cast<WLineEdit*>(elementAt(i,2))){
            item->setText(((WLineEdit*)elementAt(i,2))->text());
        }else if(dynamic_cast<WComboBox*>(elementAt(i,2))){
            item->setText(WString("{1}").arg(((WComboBox*)elementAt(i,2))->currentIndex()));
        }else if(dynamic_cast<WTextArea*>(elementAt(i,2))){
            item->setText(((WTextArea*)elementAt(i,2))->text());
        }
        m_pModel->setItem(i,0,item);

    }
    return m_pModel;
}

void UserProfile::setModel(WStandardItemModel *model)
{
    m_pModel = model;
    updateUi();
}

void UserProfile::updateUi()
{
    //log("info")<<"*** Befor Load date count "<<m_pModel->rowCount();
    nameEdit_->setText(m_pModel->item(0, 0)->text());
    fullName_->setText(m_pModel->item(1, 0)->text());
    countryEdit_->setCurrentIndex(atoi(m_pModel->item(2, 0)->text().toUTF8().c_str()));
    countryChanged();
    cityEdit_->setCurrentIndex(atoi(m_pModel->item(3, 0)->text().toUTF8().c_str()));
    phoneEdit_->setText(m_pModel->item(4, 0)->text());
    addresEdit_->setText(m_pModel->item(5, 0)->text());
    remarksEdit_->setText(m_pModel->item(6, 0)->text());

    for (int i=0;i<m_pModel->rowCount();i++)
    {
        //log("info")<<"*** Load date item "<<m_pModel->item(i, 0)->text();
        //log("info")<<"*** Load date item size "<<elementAt(i,2)->children().size();
        WWidget *object = elementAt(i,2);
        if(dynamic_cast<WLineEdit*>(object)){
            ((WLineEdit*)object)->setText(m_pModel->item(i, 0)->text());
            //log("info")<<" Load date "<<m_pModel->item(i, 0)->text();
        }else if(dynamic_cast<WComboBox*>(object)){
            ((WComboBox*)object)->setCurrentIndex(atoi(m_pModel->item(i, 0)->text().toUTF8().c_str()));
            //log("info")<<" Load date "<<m_pModel->item(i, 0)->text();
        }else if(dynamic_cast<WTextArea*>(object)){
            ((WTextArea*)object)->setText(m_pModel->item(i, 0)->text());
            //log("info")<<" Load date "<<m_pModel->item(i, 0)->text();
        }

    }
    refresh();
}
