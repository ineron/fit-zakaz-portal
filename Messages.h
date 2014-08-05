#ifndef MESSAGES_H
#define MESSAGES_H

#include <Wt/WContainerWidget>

//#include "Attachment.h"
#include "db/DBUtils.h"

using namespace Wt;

class Messages : public Wt::WContainerWidget
{
public:
    Messages(dbo::Session& session,WContainerWidget *parent=0);
    virtual ~Messages();
public:
    /*! \brief Returns whether the upload failed.
    */
    bool uploadFailed() const { return m_uploadFailed; }

    /*! \brief Returns the attachment.
    */
    //std::vector<Attachment> attachments();

    void uploaded();
    void update();


private: /// var
    int currentType;
    WContainerWidget *m_messageBodydiv;
    WFileUpload *m_upload;
    WDatePicker *m_pBeginDateEdit;
    WDatePicker *m_pEndDateEdit;
    WContainerWidget *m_mainDiv;
    WComboBox *m_cmbSendTo;
    WLineEdit *m_leTheme;
    WTextArea *m_taMessage;
    bool m_uploadFailed;
private: /// Signals
    Signal<void> m_uploadDone;
private: /// Funcions
    void fileTooLarge(::int64_t size);
    void changed(WWidget* sender);
    void footer(WContainerWidget *parent);
    void messageType(WContainerWidget *parent);
    void messageTable(WContainerWidget *parent,WStandardItemModel *model=0);
    void periodSelector(WContainerWidget *parent);
    void messageForm(WContainerWidget *parent);
    void sendMessageForm(WContainerWidget *parent);
    void readMessageForm(WString msg,WContainerWidget *parent);
    void sendMessage();
    WTable *loadMessageList(int msgType=0);
    dbo::Session &session_;
};

#endif // MESSAGES_H
