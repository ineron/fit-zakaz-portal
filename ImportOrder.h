#ifndef IMPORTORDER_H
#define IMPORTORDER_H

#include <Wt/WContainerWidget>

//#include "Attachment.h"


using namespace Wt;

class ImportOrder : public WContainerWidget
{
public:
    typedef enum {Order,Price} LoadMode;
    ImportOrder(LoadMode mode,WContainerWidget *parent=0);
    virtual ~ImportOrder();
    bool uploadNow();

    /*! \brief Returns whether the upload failed.
    */
    bool uploadFailed() const { return m_uploadFailed; }

    /*! \brief Returns the attachment.
    */
    //std::vector<Attachment> attachments();

    void uploaded();



    Signal<void>& uploadDone() { return m_uploadDone; }
protected:
    WTemplate *m_mainTemplate;
private:

    WFileUpload *m_upload;
    bool m_uploadFailed;
    WContainerWidget *m_importButtonDiv;
    LoadMode m_LoadMode;

    void renderView();
    void fileTooLarge(::int64_t size);

    class UploadInfo : public WContainerWidget
    {
    public:
    UploadInfo(const Http::UploadedFile& f, WContainerWidget *parent = 0);

    Http::UploadedFile info_;

    //! Anchor referencing the file.
    WAnchor   *downloadLink_;

    //! The check box to keep or discard the uploaded file.
    WCheckBox *keep_;
    };

    //! The text box to display an error (empty or too big file)
    WText *m_error;

    //! The option to cancel the file upload
    //Option *m_remove;
    WComboBox *m_splitter;
    WComboBox *m_codePage;
    void import();

    std::vector<UploadInfo *> m_uploadInfo;
    Signal<void> m_uploadDone;
};

#endif // IMPORTORDER_H
