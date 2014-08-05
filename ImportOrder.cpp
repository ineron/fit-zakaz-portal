#include "utils/CsvUtil.h"
#include "ImportOrder.h"
#include "Portal.h"


#include <fstream>
#ifndef WIN32
#include <unistd.h>
#endif
#include <boost/lexical_cast.hpp>

#include <iostream>


#include <Wt/WApplication>
#include <Wt/WTemplate>
#include <Wt/WComboBox>
#include <Wt/WFileUpload>
#include <Wt/WPushButton>
#include <Wt/WText>
#include <Wt/WProgressBar>
#include <Wt/WCssDecorationStyle>
#include <Wt/WMessageBox>

#include <Wt/WCheckBox>
#include <Wt/WFileResource>
#include <Wt/WAnchor>
#include <Wt/WBreak>

ImportOrder::UploadInfo::UploadInfo(const Http::UploadedFile& f,
				       WContainerWidget *parent)
  : WContainerWidget(parent),
    info_(f)
{
  /*
   * Include the file ?
   */
  keep_ = new WCheckBox(this);
  keep_->setChecked();

  /*
   * Give information on the file uploaded.
   */
  std::streamsize fsize = 0;
  {
    std::ifstream theFile(info_.spoolFileName().c_str());
    theFile.seekg(0, std::ios_base::end);
    fsize = theFile.tellg();
    theFile.seekg(0);
  }
  std::wstring size;
  if (fsize < 1024)
    size = boost::lexical_cast<std::wstring>(fsize) + L" bytes";
  else
    size = boost::lexical_cast<std::wstring>((int)(fsize / 1024))
      + L"kb";

  std::wstring fn = static_cast<std::wstring>
    (escapeText(WString::fromUTF8(info_.clientFileName())));

  downloadLink_
    = new WAnchor("", fn + L" (<i>" + WString::fromUTF8(info_.contentType())
		  + L"</i>) " + size, this);

  WFileResource *res = new WFileResource(info_.contentType(),
					 info_.spoolFileName(),
					 this);
  res->suggestFileName(info_.clientFileName());
  downloadLink_->setLink(res);
}

ImportOrder::ImportOrder(LoadMode mode,WContainerWidget *parent):WContainerWidget(parent),m_mainTemplate(0),m_importButtonDiv(0),m_LoadMode(mode),m_splitter(0),m_codePage(0)
{
    setWidth(670);
    renderView();
}

ImportOrder::~ImportOrder()
{
    //dtor
}

void ImportOrder::renderView()
{
    m_mainTemplate = new WTemplate(tr("portal.import.order"), this);

    m_splitter = new WComboBox();
    m_splitter->addItem(tr("import.order.splitter.string.1"));
    m_splitter->addItem(tr("import.order.splitter.string.2"));
    m_splitter->addItem(tr("import.order.splitter.string.3"));
    m_splitter->addItem(tr("import.order.splitter.string.4"));
    m_splitter->addItem(tr("import.order.splitter.string.5"));


    m_codePage = new WComboBox();
    m_codePage->addItem(tr("import.order.codepage.string.1"));
    m_codePage->addItem(tr("import.order.codepage.string.2"));


    /// Upload file
    m_upload = new Wt::WFileUpload();
    m_upload->setFileTextSize(40);

    // Provide a button
    m_importButtonDiv = new WContainerWidget();
    WPushButton *uploadButton = new WPushButton(tr("import.order.button.select.file"));
    WPushButton *importButton = 0;
    if(m_LoadMode == Order)
        importButton = new WPushButton(tr("import.order.button.import"),m_importButtonDiv);
    else
        importButton = new WPushButton(tr("import.order.button.import_price"),m_importButtonDiv);

    /*
    * A progress bar
    */
    //WProgressBar *progress = new WProgressBar();
    //progress->setFormat(WString::Empty);
    //progress->setVerticalAlignment(AlignMiddle);
    //m_upload->setProgressBar(progress);

    /*
    * The 'remove' option.
    */
    //m_remove = new Option(tr("msg.remove"), this);
    m_upload->decorationStyle().font().setSize(WFont::Smaller);
    m_upload->setVerticalAlignment(AlignMiddle);
    //m_remove->setMargin(5, Left);
    //m_remove->item()->clicked().connect(this, &WWidget::hide);
    //m_remove->item()->clicked().connect(this, &AttachmentEdit::remove);

    // The error message.
    m_error = new WText("", this);
    m_error->setStyleClass("error");
    m_error->setMargin(WLength(5), Left);

    importButton->clicked().connect(this, &ImportOrder::import);

    // Upload when the button is clicked.
    uploadButton->clicked().connect(m_upload, &WFileUpload::upload);
    uploadButton->clicked().connect(uploadButton, &WPushButton::disable);

    // Upload automatically when the user entered a file.
    m_upload->changed().connect(m_upload, &WFileUpload::upload);
    m_upload->changed().connect(uploadButton, &WPushButton::disable);

    // React to a succesfull upload.
    m_upload->uploaded().connect(this, &ImportOrder::uploaded);

    // React to a fileupload problem.
    m_upload->fileTooLarge().connect(this, &ImportOrder::fileTooLarge);

    m_mainTemplate->bindWidget("import.order.splitter",m_splitter);
    m_mainTemplate->bindWidget("import.order.codepage",m_codePage);
    m_mainTemplate->bindWidget("import.order.file",m_upload);
    m_mainTemplate->bindWidget("import.order.button",m_importButtonDiv);

    std::string fullpath = WApplication::instance()->appRoot()+"/images/ImportFromExcel.pdf";
    //log("info")<<fullpath<<" "<<__LINE__;
    WAnchor* anchor = new WAnchor();
    WFileResource* fileResource = new WFileResource(fullpath.c_str());
    fileResource->suggestFileName("ImportFromExcel.pdf");
    fileResource->setMimeType("application/pdf");
    anchor->setResource(fileResource);
    anchor->setTarget( Wt::TargetNewWindow );
    anchor->setText(tr("import.order.anchor.instuction"));
    m_mainTemplate->bindWidget("import.order.instuction",anchor);
    //addWidget(m_mainTemplate);

}

void ImportOrder::import()
{
    std::vector<Http::UploadedFile> files = m_upload->uploadedFiles();

    if(((Portal *)WApplication::instance())->getSession()->getEditableOrder() && m_LoadMode == Order)
    {
         WMessageBox::show("Confirm", WString("Есть не сохраненый заказ. Закончите, с ним работу и повторите операцию",UTF8), Ok);
    }else{
        //log("info")<<"Files "<<files.size();
        int badRowCount = 0;
        for (unsigned i = 0; i < files.size(); ++i)
        {
            //log("info")<<"File Name " << files[i].spoolFileName().c_str();
            //log("info")<<" index split "<<m_splitter->currentIndex();
            //log("info")<<" index cp "<<m_codePage->currentIndex();
            WStandardItemModel *importModel = readCsvFile(files[i].spoolFileName().c_str(),this,m_splitter->currentIndex(),m_codePage->currentIndex());
            std::vector<std::string> notFoundStrings;
            std::vector<ImportResult> orderLoadResult;
            if(m_LoadMode == Order)
                orderLoadResult = ((Portal *)WApplication::instance())->getSession()->addOrderFromFile(importModel);
            else
                notFoundStrings = ((Portal *)WApplication::instance())->getSession()->loadPriceFromFile(importModel);
            if(notFoundStrings.size())
            {
                WContainerWidget *div = new WContainerWidget(m_importButtonDiv);
                div->setObjectName("error_code");
                //WString str = WString("<br/<br/>>Не найдены коды товаров <br/>",UTF8);
                div->addWidget(new WBreak());
                if(m_LoadMode == Order)
                    div->addWidget(new WTemplate(WString::fromUTF8("Не найдены коды товаров.")));
                else
                    div->addWidget(new WTemplate(WString::fromUTF8("Найдены дубликаты кодов, пропущены.")));
                div->addWidget(new WBreak());
                for(unsigned k = 0; k < notFoundStrings.size(); ++k)
                {
                    badRowCount++;
                    div->addWidget(new WTemplate(WString::fromUTF8(notFoundStrings[k])));
                    div->addWidget(new WBreak());
                }
                div->addWidget(new WBreak());
                div->addWidget(new WTemplate(WString::fromUTF8("Загруженно из файла {1} строк из {2}").arg((int)importModel->rowCount()-(int)notFoundStrings.size()).arg(importModel->rowCount())));
                //new WTemplate(str,m_importButtonDiv);

            }
            if(orderLoadResult.size())
            {
                WContainerWidget *div = new WContainerWidget(m_importButtonDiv);
                div->setObjectName("error_code");
                //WString str = WString("<br/<br/>>Не найдены коды товаров <br/>",UTF8);
                div->addWidget(new WBreak());
                div->addWidget(new WTemplate(WString::fromUTF8("Найдены дубликаты кодов, пропущены.")));
                div->addWidget(new WBreak());
                for(unsigned k = 0; k < orderLoadResult.size(); ++k)
                {
                    badRowCount++;
                    WString res ;
                    if(orderLoadResult[k].errors)
                        res = WString::fromUTF8("дубли {1}").arg(orderLoadResult[k].errors);
                    else
                        res = WString::fromUTF8("Не найден");


                    div->addWidget(new WTemplate(WString::fromUTF8("Код {1} {2}").arg(orderLoadResult[k].code).arg(res)));
                }
                div->addWidget(new WBreak());
                div->addWidget(new WTemplate(WString::fromUTF8("Загруженно из файла {1} строк из {2}").arg(importModel->rowCount()-(int)orderLoadResult.size()).arg(importModel->rowCount())));
            }
            if(!badRowCount)
            {
                WContainerWidget *div = new WContainerWidget(m_importButtonDiv);
                //WString str = WString("<br/<br/>>Не найдены коды товаров <br/>",UTF8);
                div->addWidget(new WBreak());
                div->addWidget(new WTemplate(WString::fromUTF8("Загруженно из файла {1} строк.").arg(importModel->rowCount())));

            }
            if(importModel->rowCount()-badRowCount > 0)
                WMessageBox::show(WString::fromUTF8("Загрузка успешно завершена !"), WString::fromUTF8("Загруженно из файла {1} строк из {2}").arg(importModel->rowCount()-badRowCount).arg(importModel->rowCount()), Ok);

        }
    }
}

 void ImportOrder::fileTooLarge(::int64_t size)
{

    WMessageBox::show("Error", WString(tr("msg.file-too-large"))
          .arg(size / 1024)
          .arg(WApplication::instance()->maximumRequestSize() / 1024), Ok);
    //m_error->setText(tr("msg.file-too-large")
    //      .arg(size / 1024)
    //      .arg(WApplication::instance()->maximumRequestSize() / 1024));
    m_uploadFailed = true;

  /*
   * Signal to the Composer that a new asyncrhonous file upload was processed.
   */
  m_uploadDone.emit();
}

void ImportOrder::uploaded()
{
    std::vector<Http::UploadedFile> files = m_upload->uploadedFiles();
    //log("info")<<"Files - "<<files.size();
    if (!files.empty()) {
    /*
     * Delete this widgets since we have a succesfull upload.
     */
    //delete m_upload;
    //m_upload = 0;
    //delete remove_;
    //remove_ = 0;
    //delete m_error;
    //m_error = 0;

    //for (unsigned i = 0; i < files.size(); ++i)
    //  m_uploadInfo.push_back(new UploadInfo(files[i], this));
  } else {
//    m_error->setText(tr("msg.file-empty"));
    m_uploadFailed = true;
  }

  /*
   * Signal to the Composer that a new asynchronous file upload was processed.
   */
  m_uploadDone.emit();
  if(WWidget *errorDiv = find("error_code"))
    m_importButtonDiv->removeWidget(errorDiv);
}

bool ImportOrder::uploadNow()
{
  /*
   * See if this attachment still needs to be uploaded,
   * and return if a new asynchronous upload is started.
   */
  if (m_upload) {
    if (m_upload->canUpload()) {
      m_upload->upload();
      return true;
    } else
      return false;
  } else
    return false;
}
