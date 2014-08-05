// This may look like C code, but it's really -*- C++ -*-
/*
 * Copyright (C) 2011 Emweb bvba, Heverlee, Belgium.
 *
 * See the LICENSE file for terms of use.
 */

#ifndef SESSION_H_
#define SESSION_H_

#include "db/DBUtils.h"
#include "User.h"

#include <pthread.h>
// std
#include <vector>

#include <Wt/Auth/Login>

#include <Wt/Dbo/Session>
#include <Wt/Dbo/ptr>
#include <Wt/Dbo/backend/Postgres>


//#include <Wt/WString>
#include <Wt/WStandardItemModel>

#define PAGEROWS 120

#define INLINE_WQUERY(...)  L ## #__VA_ARGS__

class InfoBaseSoapBindingProxy;

using namespace Wt;

typedef Wt::Auth::Dbo::UserDatabase<AuthInfo> UserDatabase;

typedef struct
{
    std::string code;
    int errors;
} ImportResult;

class Session
{
public:
    static void configureAuth();

    Session();
    ~Session();

    Wt::Auth::AbstractUserDatabase& users();
    Wt::Auth::Login& login()
    {
        return login_;
    }
    Wt::Dbo::Session& getSession()
    {
        return session_;
    };
    void removeOldToken();

    std::vector<User> topUsers(int limit);
    std::vector<db::Catalog> catalog(const int &page,const int &part);
    std::vector<db::Catalog> userCatalog(int page,int part=0);
    std::vector<FolderItem> folders(std::string id);
    WStandardItem *createFolderItem(const std::string& folderName,const std::string& folderId);
    WStandardItemModel *getGoodsModel(const std::string &parent,WStandardItemModel *model);
    void getFoldersModel(WStandardItemModel *foldersModel,std::map<std::string, WStandardItem*>& folderIdNameMap);
    void getFoldersModel(WPopupMenu *popup,std::map<std::string, WPopupMenu*>& folderIdNameMap);

    void updateUserPrice(const std::string& id,const int& valueType,const double& value,const int& priceType,const bool& changeFixed,const WDate& modifiDate,int rounding);
    boost::any convertData(const WString &s);
    std::vector<db::Catalog> findString(const WString &findstr,const int &page,const int &part=0);
    WString getPageHeader(int page);
    std::string getIdFromPage(int page);
    std::vector<db::Catalog> getOrderRecords();
    std::vector<db::OrdersHeader> getOrdersJournal(WStandardItemModel *model,const WDate &beginDate,const WDate &endDate);
    int findCode(const WString& code);
    int maxPage(const WString &findstr);
    int getCatalogFirstPage(const int owner);
    //dbo::ptr<User>getUser();
    User *getUser();
    dbo::ptr<User> user() const;

    std::string getTerminalLicense();
    int getTerminalLicenseCount();
    void setFreeTerminalLicense(std::string mareketId);
    /*
     * These methods deal with the currently logged in user
     */
    std::string userName() const;
    std::string userId() const;
    std::string userCharId() const;
    int findRanking();
    bool restartSeq(std::string tableName);
    void addToScore(int s);
    bool addUser(const std::string &id,const std::string &code,const std::string &name,const std::string &passwd,const std::string &location,const int &hash, const int &discount);
    bool addGoods(db::GoodsSimple *goods);
    bool rebuildGoodsOrder();
    bool addPrice(db::Price *price);
    bool addPersonalPrice(db::PersonalPrice *price);
    bool addOrderRecords(const int &quantity,const std::string &goodsId,float price=0);
    bool erasePrices();
    bool clearCurrentOrder();
    std::string copyOrder(std::string &ref);
    bool heldCurrentOrder(const AddressPtr &address);
    bool saveCurrentOrder();
    void setCurrentOrder(const int &orderId);
    void setCurrentOrder(const std::string &orderUUID);
    dbo::ptr<db::OrdersHeader> setCurrentOrder();
    OrdersHeaderPtr getCurrentOrder();
    bool setEditableOrder();
    OrdersHeaderPtr getEditableOrder();
    std::string getOrderComments();
    void setOrderComments(std::string comm);
    void setOrderAddress(const AddressPtr &address);
    void setOrderAddress(const AddressPtr &address,const OrdersHeaderPtr &order);
    std::vector<ImportResult> addOrderFromFile(WStandardItemModel *orderGoods);
    std::vector<std::string> loadPriceFromFile(WStandardItemModel *loadModel);
    int loadMessageList();
    std::vector<db::Message>getMessageList(const bool msgType=0);
    void addInputMessage(const std::string &uid,const WDateTime &date,const std::string &message,const std::string &fileName);


    bool setUserProperty(const std::string &propertyName,const boost::any &balue);
    void setOrderNumber(const WString &orderId,const WString &number);

    static const Wt::Auth::AuthService& auth();
    static const Wt::Auth::AbstractPasswordService& passwordAuth();
//    static const std::vector<const Wt::Auth::OAuthService *>& oAuth();

    dbo::ptr<db::GoodsSimple> goods(std::string code) const;
    WStringListModel *getAddressList(WStringListModel *model,int &currentIndex);
    void getAddressList(WStringListModel *model);
    WComboBox *getRoundList(WComboBox *roundList);

    /// User property
    typedef std::map<std::string, boost::any> UserPropertyMap;
    mutable UserPropertyMap m_userProperty;
    void initUserProperty() const;
    boost::any userProperty(const std::string &propertyName,const std::string &userRef);
    const boost::any userProperty(const std::string &propertyName) const;
    static pthread_mutex_t cs_mutex;// = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;

private:
    Wt::Dbo::backend::Postgres postgres_;
    mutable Wt::Dbo::Session session_;
    UserDatabase *users_;
    Wt::Auth::Login login_;
    dbo::ptr<db::OrdersHeader> m_CurrentOrder;


};

#endif //SESSION_H_
