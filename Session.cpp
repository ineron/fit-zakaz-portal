/*
 * Copyright (C) 2011 Emweb bvba, Heverlee, Belgium.
 * pgsql LnK_23od
 * postgres TrO_15na
 * See the LICENSE file for terms of use.
 */
//#include "Portal.h"
#include "Session.h"
#include "Messages.h"
#include "db/MsSql7.h"
//#include "ButtonPad.h"
//#include "ext/blowfish.h"
// crypto++

/*
#include <cryptopp/blowfish.h>
#include <cryptopp/base64.h>
#include <cryptopp/files.h>
#include <cryptopp/filters.h>
#include <cryptopp/modes.h>
#include <cryptopp/gzip.h>
#include <cryptopp/osrng.h>
#include <cryptopp/hex.h>
*/
// crypt BlowFish
//#include <libcrypt/Blowfish.h>

#include "Wt/Auth/AuthService"
#include "Wt/Auth/HashFunction"
#include "Wt/Auth/PasswordService"
#include "Wt/Auth/PasswordStrengthValidator"
#include "Wt/Auth/PasswordVerifier"

#include "Wt/Auth/PasswordHash"

#include "Wt/Auth/GoogleService"
#include "Wt/Auth/Dbo/AuthInfo"
#include "Wt/Auth/Dbo/UserDatabase"
#include <Wt/WAnchor>
#include <Wt/WTemplate>

#include <Wt/WMessageBox>
//#include "xml/tinyxml2.h"
//#include <boost/filesystem/operations.hpp>
//#include <boost/filesystem/exception.hpp>
//#include <libhml++.h>
#include <htmlcxx/html/ParserDom.h>
#include <htmlcxx/html/CharsetConverter.h>
#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>

//#include <boost/html/document.hpp>
#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <fcntl.h>

//#include "rapidxml/rapidxml.hpp"
//#include "rapidxml/rapidxml_print.hpp"

#include <Wt/WServer>
#include <Wt/WAbstractItemModel>
#include <Wt/WStandardItemModel>
#include <Wt/WStringListModel>
#include <Wt/WStandardItem>
#include <Wt/WMenuItem>
#include <Wt/WPopupMenu>
#include <Wt/WTable>
#include <Wt/WText>
#include <Wt/WAnchor>
#include <Wt/Utils>
#include <Wt/WComboBox>

#include <Wt/WApplication>
#include <Wt/WLogger>

#include <exception>

#ifndef WIN32
#include <unistd.h>
#endif

#if !defined(WIN32) && !defined(__CYGWIN__) && !defined(ANDROID)
#define HAVE_CRYPT
#endif

using namespace Wt;
namespace dbo = Wt::Dbo;
//using namespace rapidxml;
//using namespace tinyxml2;
using namespace htmlcxx;

namespace
{


#ifdef HAVE_CRYPT
class UnixCryptHashFunction : public Auth::HashFunction
{
public:
    virtual std::string compute(const std::string& msg,
                                const std::string& salt) const
    {
        std::string md5Salt = "$1$" + salt;
        /// new
        /*
        byte key[CryptoPP::Blowfish::MAX_KEYLENGTH], iv[CryptoPP::Blowfish::BLOCKSIZE];
        std::string plainText = "admin";

        //CryptoPP::AutoSeededRandomPool rng (true);
        //rng.GenerateBlock(key, sizeof (key));
        //rng.GenerateBlock(iv, sizeof (iv));
        memcpy(key,md5Salt.c_str(),sizeof(key));
        memcpy(iv,md5Salt.c_str()+sizeof(key),sizeof(iv));

        std::string cipher;
        {
            CryptoPP::StringSink* sink = new CryptoPP::StringSink(cipher);
            CryptoPP::Base64Encoder* base64_enc = new CryptoPP::Base64Encoder(sink);
            CryptoPP::CBC_Mode<CryptoPP::Blowfish>::Encryption twofish(key, CryptoPP::Blowfish::MAX_KEYLENGTH,iv);
            CryptoPP::StreamTransformationFilter* enc = new CryptoPP::StreamTransformationFilter(twofish, base64_enc);
            CryptoPP::Gzip *zip = new CryptoPP::Gzip (enc);
            CryptoPP::StringSource source(plainText, true, zip);
        }


        //std::cout << "Key length: " << (std::countof(key)*8) << std::endl;
        //std::cout << "IV length: " << (std::countof(iv)*8) << std::endl;
        std::cout << "Encrypted text:\n" << cipher << std::endl;
        return cipher;
        */
        /// old
        return crypt(msg.c_str(), md5Salt.c_str());
    }

    virtual bool verify(const std::string& msg,
                        const std::string& salt,
                        const std::string& hash) const
    {
        return crypt(msg.c_str(), hash.c_str()) == hash;
    }

    virtual std::string name () const
    {
        return "crypt";
    }
};
#endif // HAVE_CRYPT

class MyOAuth : public std::vector<const Auth::OAuthService *>
{
public:
    ~MyOAuth()
    {
        for (unsigned i = 0; i < size(); ++i)
            delete (*this)[i];
    }
};

Auth::AuthService myAuthService;
Auth::PasswordService myPasswordService(myAuthService);
//MyOAuth myOAuthServices;
}

pthread_mutex_t Session::cs_mutex =  PTHREAD_MUTEX_INITIALIZER;//PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;


void Session::configureAuth()
{
    myAuthService.setAuthTokensEnabled(true, "portalcookie");
    //myAuthService.setAuthTokensEnabled(false, "remember-me");
    myAuthService.setEmailVerificationEnabled(true);

    Auth::PasswordVerifier *verifier = new Auth::PasswordVerifier();
    verifier->addHashFunction(new Auth::BCryptHashFunction(7));

#ifdef HAVE_CRYPT
    // We want to still support users registered in the pre - Wt::Auth
    // version of the hangman example
    verifier->addHashFunction(new UnixCryptHashFunction());
#endif

    myPasswordService.setVerifier(verifier);
    myPasswordService.setStrengthValidator(new Auth::PasswordStrengthValidator());
    myPasswordService.setAttemptThrottlingEnabled(true);

  //  if (Auth::GoogleService::configured())
  //      myOAuthServices.push_back(new Auth::GoogleService(myAuthService));
}

Session::Session()
#ifndef FIT
    //: postgres_("host='127.0.0.1' port='54032' dbname='portal' user='postgres' password='TrO_15na'")
    : postgres_("host='192.168.0.9' port='5432' dbname='portal' user='postgres' password='TrO_15na'")
#else
    : postgres_("host='192.168.10.57' dbname='portal' user='postgres' password='TrO_15na'")
#endif
{
    session_.setConnection(postgres_);
    postgres_.setProperty("show-queries", "true");

#ifdef DEBUG
    std::cout<<"******** Session() *********"<<__LINE__<<std::endl;
#endif
    session_.mapClass<User>("user");

    session_.mapClass<UserProperty>("user_property");
    session_.mapClass<UserPropertyFields>("user_property_fields");
    session_.mapClass<AuthInfo>("auth_info");
    session_.mapClass<AuthInfo::AuthIdentityType>("auth_identity");
    session_.mapClass<AuthInfo::AuthTokenType>("auth_token");
    session_.mapClass<db::GoodsSimple>("goods_simple");
    session_.mapClass<db::GoodsHistory>("goods_history");
    session_.mapClass<db::Price>("price");
    session_.mapClass<db::Catalog>("goods_catalog");
    session_.mapClass<db::PersonalPrice>("personal_price");
    session_.mapClass<db::OrdersHeader>("orders_header");
    session_.mapClass<db::OrderRecords>("order_records");
    session_.mapClass<db::Comments>("comments");
    session_.mapClass<db::Message>("messages");
    session_.mapClass<db::CalcOptions>("calculation_options");
    session_.mapClass<db::Address>("address");
    session_.mapClass<db::AddressProperty>("address_property");
    session_.mapClass<db::Rounding>("price_rounding");


    std::string value;
    //WApplication::readConfigurationProperty("soapUser",value);
    //soapUserName = static_cast<std::string>(value);

    //WApplication::readConfigurationProperty("soapPassword",value);
    //soapUserPasswd = static_cast<std::string>(value);



    users_ = new UserDatabase(session_);



    //dbo::Transaction transaction(session_);

        //Auth::HashFunction* hashFunc = ((Auth::PasswordVerifier*)myPasswordService.verifier())->hashFunctions()[1];
        //log("info")<<hashFunc->compute("admin","cjdI8oWYx5yHBtYB");//"013CV6BhxoPSc1AhzmlfFoe"

    try
    {
        //session_.createTables();

        /*
         * Add a default guest/guest account
         */
        //Auth::User guestUser = users_->registerNew();
        //guestUser.addIdentity(Auth::Identity::LoginName, "admin");
        //myPasswordService.updatePassword(guestUser, "admin");
        //((Portal *)WApplication::instance())->getSession()->
        //addUser("0","0","admin","admin","spb",0,0);
	//Auth::PasswordVerifier *verifer = myPasswordService.verifier();
        //crypt();
        //log("info")<<Utils::hexEncode(Utils::md5("   740   "));
        //log("info")<<(new Auth::MD5HashFunction)->compute("   740   ","");
        //Auth::PasswordHash hash = myPasswordService.verifier()->hashPassword("admin");
        //log("info")<<hash.value();
        //log("info")<<hash.salt();
        Wt::log("info") << "Database created";
    }
    catch (...)
    {
        Wt::log("info") << "Using existing database";

    }
    //setCurrentOrder();
    //transaction.commit();


}

Session::~Session()
{
    delete users_;

}

dbo::ptr<User> Session::user() const
{

    if (login_.loggedIn())
    {
        dbo::ptr<AuthInfo> authInfo = users_->find(login_.user());
        dbo::ptr<User> user = authInfo->user();

        if (!user)
        {
            user = session_.add(new User());
            authInfo.modify()->setUser(user);
        }
        return user;
    }
    else
        return dbo::ptr<User>();
}

void Session::removeOldToken()
{
    dbo::Transaction transaction(session_);
    dbo::ptr<AuthInfo> authInfo = users_->find(login_.user());
    log("info")<<authInfo<<" "<<__LINE__;
    session_.execute("delete from auth_token where auth_info_id = ? and id not in (select id from auth_token where auth_info_id = ? order by expires desc limit 1)").bind(authInfo.id()).bind(authInfo.id());
}

/*
dbo::ptr<User>Session::getUser()
{
#ifdef DEBUG
    std::cout<<"******** getUser() *********"<<__LINE__<<std::endl;
#endif

    dbo::Transaction transaction(session_);
    dbo::ptr<User> usr = user();
    User *result = new User(*usr);
    result->ref = usr.id();
    transaction.commit();
    return usr;
}
*/
User *Session::getUser()
{
#ifdef DEBUG
    std::cout<<"******** getUser() *********"<<__LINE__<<std::endl;
#endif

    dbo::Transaction transaction(session_);
    dbo::ptr<User> usr = user();
    User *result = new User(*usr);
    result->ref = usr.id();
    transaction.commit();
    return result;
}


std::string Session::userName() const
{
    if (login_.loggedIn())
        return login_.user().identity(Auth::Identity::LoginName).toUTF8();
    else
        return std::string();
}


std::string Session::userId() const
{

    std::string result= "";
    if (login_.loggedIn())
    {
        //dbo::ptr<User> u = user();
        result = login_.user().id();
    }
    return result;
}

std::string Session::userCharId() const
{

    std::string result= "";
    dbo::Transaction transaction(session_);
    if (login_.loggedIn())
    {
        //dbo::ptr<User> u = user();
        //result = login_.user().id();
        //UserPropertyList upl = session_.find<UserProperty>().where("id = ?").bind(login_.user().id());
        result = session_.query<std::string>("select b.char_id from public.user b  where b.ref = ?").bind(boost::lexical_cast<std::string>(user().id()));

    }
    transaction.commit();
    return result;
}

boost::any Session::userProperty(const std::string &propertyName,const std::string &userRef)
{
#ifdef DEBUG
    std::cout<<"******** userProperty("<< propertyName<<") *********"<<__LINE__<<std::endl;
#endif

    //WStandardItemModel * model= new WStandardItemModel(0,0);
    boost::any result(std::string("0"));
    boost::uuids::uuid uuid;
    if (login_.loggedIn())
    {
        //dbo::ptr<User> u = user();
        //result = login_.user().id();
    dbo::Transaction transaction(session_);
    try{
        dbo::ptr<UserPropertyFields> field = session_.find<UserPropertyFields>().where("field_name = ?").bind(propertyName).limit(1);

        if(!field)
        {
            std::cout << "Field name "<< propertyName <<"not found"<<std::endl;
            throw std::exception();
        }


        switch (field->type)
        {
            case UserProperty::Boolean: result = boost::any(false);break;
            case UserProperty::Numeric: result = boost::any((double)0);break;
            case UserProperty::String: result = boost::any(std::string());break;
            case UserProperty::Date: result = boost::any(WDate());break;
            default: result = boost::any(std::string());break;
        }

        uuid = field.id();
        UserPropertyList upl;
        if(userRef.begin() == userRef.end())
            upl = session_.find<UserProperty>().where("user_ref = ? AND field_ref = ?").bind(user()).bind(field);
        else
            upl = session_.find<UserProperty>().where("user_ref = ? AND field_ref = ?").bind(userRef).bind(field);

        for (UserPropertyList::const_iterator i = upl.begin(); i != upl.end(); ++i)
        {
            dbo::ptr<UserProperty> property = *i;
            if(property->typeId == UserProperty::Numeric)
                result = boost::any(property->value);
            else if(property->typeId == UserProperty::String)
                result = boost::any(property->simpleValue);
            else if(property->typeId == UserProperty::Boolean)
                result = boost::any((bool)property->value != 0);
            else
                result = boost::any(property->simpleValue);
        }

    transaction.commit();
    }
    catch(...)
    {
        log("EXCEPTION")<<"addOrderFromFile "<<__LINE__;
        WMessageBox::show("Exception !", WString("Произошла исключительная ситуация",UTF8), Ok);
        transaction.rollback();
    }
    }
#ifdef DEBUG
    std::cout<<"******** userProperty("<<asString(result)<<") *********"<<__LINE__<<std::endl;
#endif

    return result;
}

void Session::initUserProperty() const
{
    dbo::Transaction transaction(session_);
    UserPropertyFieldList prtList = session_.find<UserPropertyFields>();
    boost::any value;
    for(UserPropertyFieldList::const_iterator i = prtList.begin();i != prtList.end();++i)
    {

        switch ((*i)->type)
        {
            case UserProperty::Boolean: value = boost::any(false);break;
            case UserProperty::Numeric: value = boost::any((double)0);break;
            case UserProperty::String: value = boost::any(std::string());break;
            case UserProperty::Date: value = boost::any(WDate());break;
            default: value = boost::any(std::string());break;
        }
        m_userProperty.insert(std::pair<std::string,boost::any>( (*i)->name,value));

    }


    UserPropertyList uprt = session_.find<UserProperty>().where("user_ref = ?").bind(user());
    for(UserPropertyList::const_iterator i = uprt.begin();i != uprt.end();++i)
    {
        log("SESSION")<<" property name "<<(*i)->propertyField->name<<" value "<<(*i)->simpleValue;

        dbo::ptr<UserProperty> property = *i;
        if(property->typeId == UserProperty::Numeric)
            value = boost::any(property->value);
        else if(property->typeId == UserProperty::String)
            value = boost::any(property->simpleValue);
        else if(property->typeId == UserProperty::Boolean)
            value = boost::any((bool)property->value != 0);
        else
            value = boost::any(property->simpleValue);

        m_userProperty[(*i)->propertyField->name] = value;
    }

}

const boost::any Session::userProperty(const std::string &propertyName) const
{
    pthread_mutex_lock( &cs_mutex );
    if (m_userProperty.begin() == m_userProperty.end())
    {
        log("SESSION")<<"Init property "<<propertyName;
        initUserProperty();
    }
    pthread_mutex_unlock( &cs_mutex );
    log("SESSION")<<"Get property "<<propertyName<<"  "<<asString(m_userProperty[propertyName]);
    //return const_cast<boost::any>(m_userProperty[propertyName]);
    return m_userProperty[propertyName];
}

bool Session::setUserProperty(const std::string &propertyName,const boost::any &value)
{
#ifdef DEBUG
    std::cout<<"******** setUserProperty() *********"<<__LINE__<<std::endl;
#endif

    //WStandardItemModel * model= new WStandardItemModel(0,0);
    boost::any result;
    dbo::Transaction transaction(session_);
    try{
    if (login_.loggedIn())
    {
        //dbo::ptr<User> u = user();
        //result = login_.user().id();
        dbo::ptr<UserPropertyFields> field = session_.find<UserPropertyFields>().where("field_name = ?").bind(propertyName);

        if(!field)
        {
            std::cout << "Field name "<< propertyName <<"not found"<<std::endl;
            throw std::exception();
        }

        dbo::ptr<UserProperty> property = session_.find<UserProperty>().where("user_id = ? AND field_id = ?").bind(user()).bind(field);

        if(property)
        {
            if(field->type == UserProperty::Numeric)
                property.modify()->value = boost::any_cast<double>(value);
            else if(field->type == UserProperty::String)
                property.modify()->simpleValue = boost::any_cast<std::string>(value);
        }else{
            /// Создаем объект свойств
            UserProperty *newProperty   = new UserProperty;
            /// Устанавливаем значения свойств
            newProperty->propertyField  = field;
            newProperty->typeId         = field->type;
            newProperty->user           = user();
            /// Проверяем тип устанавливаемого значения
            if(field->type == UserProperty::Numeric)
                newProperty->value = boost::any_cast<double>(value);
            else if(field->type == UserProperty::String)
                newProperty->simpleValue = boost::any_cast<std::string>(value);
            /// Записываем новый элемент
            session_.add(newProperty);
        }
    }
    transaction.commit();
}
    catch(...)
    {
        log("EXCEPTION")<<"addOrderFromFile "<<__LINE__;
        WMessageBox::show("Exception !", WString("Произошла исключительная ситуация",UTF8), Ok);
        transaction.rollback();
    }
    return true;
}


int Session::loadMessageList()
{

#ifdef DEBUG
        log("info")<< "loadMessageList "<<__LINE__;
#endif

    dbo::Transaction transaction(session_);

    std::string location = boost::any_cast<std::string>(userProperty("Регион"));
    std::string code = boost::any_cast<std::string>(userProperty("Код 1С"));


    std::string path = WApplication::instance()->appRoot()+"msg/"+location+"/"+code;
    //std::string path = "list.htm";//"list.htm";//"strings
    std::string r, html;

#ifdef DEBUG
    log("info") <<" PATH *** "<<path+"/list.htm"<<" File-> "<<__FILE__<<" Function-> "<< __FUNCTION__<<" Line ->"<< __LINE__;
#endif
    std::fstream ifile(std::string(path+"/list.htm").c_str(), std::ios_base::in);

    /*
        XMLDocument doc;
        doc.LoadFile(path.c_str());
        log("info")<<" 1 *** ";
        doc.PrintError();
        XMLElement *element = doc.FirstChildElement( "HTML" )->FirstChildElement( "HEAD" );
        log("info")<<" 2 *** ";
        log("info")<<" ECHO *** "<<element->FirstChildElement("BODY")->GetText();
    */



    if (!ifile)
    {
#ifdef DEBUG
        log("info")<< "File "<<path+"/list.htm"<<" no opened";
#endif
        return(0);
    }
    //string html = "<html><body>hey</body></html>";
    // Читаем в строку
    CharsetConverter cc("WINDOWS-1251","UTF-8");
    while (getline(ifile, r))
    {

        //int length = r.length();
        //char line[length*2];
        //char str[length];
        //std::strcpy(str,r.c_str());
        //std::cout << convert_1251_utf8(str,line) << std::endl;
        html.append(r);

    }

    ifile.close();
    boost::replace_all( html, "\r", "" );
    html = cc.convert(html);

    /*
        char* a = new char[s.size()+1];
    	a[s.size()] = 0;
    	memcpy(a,s.c_str(),s.size());
    */

    HTML::ParserDom parser;
    tree<HTML::Node> dom = parser.parseTree(html);

    //Print whole DOM tree
    //std::cout << dom << std::endl;

    //Dump all links in the tree
    tree<HTML::Node>::iterator it = dom.begin();
    tree<HTML::Node>::iterator end = dom.end();
    //WTable *table = new WTable();
    int row=0,col=0;
    std::string str="",message="";
    std::string filelink="";
    WDateTime fdate;
    for (; it != end; ++it)
    {
        if(it->tagName() == "TR")
        {
            row++;
            col = 0;
            str="";
            filelink = "";
        };
        if(it->tagName() == "TD")col++;

        if ((!it->isTag()) && (!it->isComment()) && (it->text() != "&nbsp;"))
        {
            //xstd::cout << it->text()<<" col "<<col;
            switch (col)
            {
            case 1:
            {
                //struct stat64 fileInfo;
                struct stat fileInfo;

                std::string filename = std::string(path+"/"+it->text());//WApplication::instance()->docRoot()+

                std::string date = it->text().substr(14,8);
                date = date.substr(6,2)+"."+date.substr(4,2)+"."+date.substr(0,4);
                std::fstream msgfile(filename.c_str(), std::ios_base::in);

                int fd =  open(filename.c_str(), O_RDONLY);
                if(!fd)
                {
                    std::cerr << "Error: " << strerror(errno) << " File: "<<__FILE__<<" Line: "<<__LINE__<<std::endl;
                }
                //if (fstat64(fd, &fileInfo) != 0)    // Use stat( ) to get the info
                if (fstat(fd, &fileInfo) != 0)    // Use stat( ) to get the info
                {

                    std::cerr << "Error: " << strerror(errno) << " File: "<<__FILE__<<" Line: "<<__LINE__ <<std::endl;
                    std::cerr << "Error: " <<" Path "<<filename<<std::endl;
                    return(EXIT_FAILURE);
                }
                close(fd);
                fdate = WDateTime(WDateTime::fromTime_t(fileInfo.st_mtime));
                //std::cout << "Created       : " << fdate.toString("dd.MM.yyyy hh:mm");
                //WString filedate = WString::tr("portal.messages.filedate").arg(fdate.toString("dd.MM.yyyy hh:mm"));
                //table->elementAt(row,col-1)->addWidget(new WText(filedate));

                while (getline(msgfile, r))
                    str.append(r);
                message = cc.convert(str);

                //std::cout<<"PATH "<<path+"/"+it->text();
                //std::cout<<str;
                //std::cout<<msgfile.name();

                break;
            }
            case 2:
            {
                //WContainerWidget *div = new WContainerWidget();
                //div->setStyleClass("in");
                //div->setToolTip(WString(str,UTF8));
                //table->elementAt(row,col-1)->addWidget(div);
                //if(str.size()>24)
                //    new WAnchor("#",WString(str,UTF8).value().substr(0,12)+WString("..."),div);
                //else
                //    new WAnchor("#",WString(str,UTF8).value(),div);
                filelink = it->text();
                break;
            }
            case 3:
            {
                //WContainerWidget *div = new WContainerWidget();
                //div->setStyleClass("in");
                //table->elementAt(row,col-1)->addWidget(div);
                //new WAnchor(WLink(WLink::InternalPath,"/private/messages/"+filelink),WString(it->text(),UTF8),div);
                addInputMessage(filelink,fdate,message,it->text());
                break;
            }
            }

        }
        //std::cout << std::endl;
    }
    /*
    	xml_document<> doc;
        doc.parse<rapidxml::parse_no_data_nodes | rapidxml::parse_trim_whitespace>(a);
        log("info")<<"  *** 3";
        std::cout << "Name of my first node is: " << doc.first_node()->name() << "\n";
    */
    /*
    std::string x;
    while (ifile >> x)
    {
        int length = x.length();
        char line[length*2];
        char str[length];
        std::strcpy(str,x.c_str());
        std::cout << convert_1251_utf8(str,line) << std::endl;

        xml_document<> doc;
        doc.parse<0>(str);
        std::cout << "Name of my first node is: " << doc.first_node()->name() << "\n";
    }
    */



    /*
        boost::html::document   htmlDOM;

        htmlDOM.read(ifile);

        htmlDOM.write(cout);
    */
    /*
        try
        {
            if(boost::filesystem::is_directory(path))
            {
                std::set<boost::filesystem::path> paths;
                boost::filesystem::directory_iterator end_itr;
                for (boost::filesystem::directory_iterator i(path); i != end_itr; ++i)
                    paths.insert(*i);

                //int dataRow = 0;
                for (std::set<boost::filesystem::path>::iterator i = paths.begin(); i != paths.end(); ++i)
                {
                    std::cerr << i->stem();
                    //if(model->)
                    //model->insertRows(model->rowCount(),dataRow + 1 - model->rowCount());
                }

            }
        }
        catch (boost::filesystem::filesystem_error& e)
        {
            std::cerr << e.what() << std::endl;
        }
        */
    transaction.commit();
    return 0;
}

std::vector<db::Message>Session::getMessageList(const bool msgType)
{
#ifdef DEBUG
    std::cout<<"******** getMessageList() *********"<<__LINE__<<std::endl;
#endif

    dbo::Transaction transaction(session_);
    std::vector<db::Message> result;
    MessageLst msgList = session_.find<db::Message>().where("type = ? and author_ref = ?").bind(msgType).bind(user());
    for (MessageLst::const_iterator i = msgList.begin(); i != msgList.end(); ++i)
    {
        MessagePtr msg = *i;
        result.push_back(*msg);
    }
    transaction.commit();
    return result;
}

void Session::addInputMessage(const std::string &uid,const WDateTime &date,const std::string &message,const std::string &fileName)
{
    dbo::Transaction transaction(session_);
    dbo::ptr<db::Message> msg = session_.find<db::Message>().where("uid = ?").bind(uid);
    if(!msg)
    {
        db::Message *newMsg = new db::Message();
        newMsg->author = user();
        boost::uuids::uuid uuid = boost::uuids::random_generator()();
        newMsg->ref = uuid;
        newMsg->messageDate = date;
        newMsg->messageState = db::Message::newed;
        newMsg->messageType = db::Message::input;
        newMsg->message = message;
        newMsg->messageUid = uid;
        newMsg->fileName = fileName;
        //std::cout<<"*** File name "<<fileName<<" Message "<<message;
        session_.add(newMsg);
    }

    transaction.commit();
}
void Session::addToScore(int s)
{
    /*
     dbo::Transaction transaction(session_);

     dbo::ptr<User> u = user();
     if (u) {
       u.modify()->score += s;
       ++u.modify()->gamesPlayed;
       u.modify()->lastGame = WDateTime::currentDateTime();
     }

     transaction.commit();
     */
}

std::vector<User> Session::topUsers(int limit)
{
    dbo::Transaction transaction(session_);

    Users top = session_.find<User>().orderBy("score desc").limit(20);

    std::vector<User> result;
    for (Users::const_iterator i = top.begin(); i != top.end(); ++i)
    {
        dbo::ptr<User> user = *i;
        result.push_back(*user);

        dbo::ptr<AuthInfo> auth = *user->authInfos.begin();
        std::string name = auth->identity(Auth::Identity::LoginName).toUTF8();

        result.back().name = name;
    }

    transaction.commit();

    return result;
}


std::vector<db::Catalog> Session::catalog(const int &page,const int &part)
{
#ifdef DEBUG
    log("info")<<"std::vector<db::Catalog> Session::catalog(int "<<page<<")  "<<__LINE__;
#endif
    dbo::Transaction transaction(session_);

    //CatalogList catalog = session_.find<Catalog>().where("page = ?").bind(page);
#ifdef DEBUG
    log("info")<<" USER "<<user()<<" PAGE "<<page<<" part "<<part;
#endif

    std::string query ="";
    CatalogList catalogList;
    /*
    if(licenseID != "")
        catalogList = session_.query<CatalogPtr>("SELECT a from get_user_catalog_list(?,?::uuid,?,?) "
                                            "as a(id integer,parent integer,version integer,code text,division integer,is_group boolean,name text,articul text,not_visible boolean,description text,short_description text,file_name text,upakovka text,char_id varchar(9),level integer,owner integer,price numeric,quantity integer,ref uuid);")
        .bind(page-1).bind(user().id()).bind(part).bind(licenseID);
    else
    */
    WString queryStr("SELECT a from get_catalog_list(?::integer,?::uuid,?::integer) "
        "as a(ref uuid,parent_ref uuid,version integer,code text,division integer,is_group boolean,name text,articul text,"
        "not_visible boolean,description text,short_description text,file_name text,upakovka text,char_id varchar(9),level integer,"
        "owner integer,price numeric,quantity integer,order_id integer,active boolean)");
    //queryStr.arg(page-1).arg(user().id()).arg(part);
        /*
        catalogList = session_.query<CatalogPtr>("SELECT a from get_catalog_list(?::integer,?::uuid,?::integer) "
        "as a(ref uuid,parent_ref uuid,version integer,code text,division integer,is_group boolean,name text,articul text,"
        "not_visible boolean,description text,short_description text,file_name text,upakovka text,char_id varchar(9),level integer,"
        "owner integer,price numeric,quantity integer,order_id integer,active boolean)").bind(page-1).bind(user().id()).bind(part);
        */
        catalogList = session_.query<CatalogPtr>(queryStr.toUTF8()).bind(page-1).bind(boost::lexical_cast<std::string>(user().id())).bind(part);

    std::vector<db::Catalog> result;
    for (CatalogList::const_iterator i = catalogList.begin(); i != catalogList.end(); ++i)
    {
        dbo::ptr<db::Catalog> catalogList = *i;
        //log("info")<<"****** quantity "<<catalog->quantity<<" code "<<catalog->code;
        result.push_back(*catalogList);

        //dbo::ptr<AuthInfo> auth = *catalog->authInfos.begin();
        //std::string name = auth->identity(Auth::Identity::LoginName).toUTF8();

        //result.back().name = name;
    }

    transaction.commit();
#ifdef DEBUG
    log("info")<<"std::vector<db::Catalog> Session::catalog(int "<<page<<")  "<<__LINE__;
#endif
    return result;

}


std::vector<db::Catalog> Session::userCatalog(int page,int part)
{
#ifdef DEBUG
    log("info")<<"std::vector<db::Catalog> Session::catalog(int "<<page<<")  "<<__LINE__;
#endif
    dbo::Transaction transaction(session_);

    //CatalogList catalog = session_.find<Catalog>().where("page = ?").bind(page);
#ifdef DEBUG
    log("info")<<" USER "<<user()<<" PAGE "<<page;
#endif
    CatalogList catalog = session_.query<CatalogPtr>("SELECT a from get_user_catalog_list(?,?,?) a;").bind(page-1).bind(boost::lexical_cast<std::string>(user().id())).bind(part);
    std::vector<db::Catalog> result;
    for (CatalogList::const_iterator i = catalog.begin(); i != catalog.end(); ++i)
    {
        dbo::ptr<db::Catalog> catalog = *i;
        //log("info")<<"****** quantity "<<catalog->quantity<<" code "<<catalog->code;
        result.push_back(*catalog);

        //dbo::ptr<AuthInfo> auth = *catalog->authInfos.begin();
        //std::string name = auth->identity(Auth::Identity::LoginName).toUTF8();

        //result.back().name = name;
    }

    transaction.commit();
#ifdef DEBUG
    log("info")<<"std::vector<db::Catalog> Session::catalog(int "<<page<<")  "<<__LINE__;
#endif
    return result;

}



WStandardItem *Session::createFolderItem(
        const std::string& folderName,
        const std::string& folderId)
{
    WStandardItem *result = new WStandardItem(WString::fromUTF8(folderName.c_str()));

    if (!folderName.empty())
    {
        //log("info")<<" id "<<folderId;
        result->setData(boost::any(folderId),UserRole);
        result->setFlags(result->flags() | ItemIsDropEnabled);
        //foldersIdMap[folderId] = location;
    }
    else
        result->setFlags(result->flags().clear(ItemIsSelectable));

    result->setIcon("icons/folder.gif");

    return result;
}

void Session::getFoldersModel(WStandardItemModel *foldersModel,std::map<std::string, WStandardItem*> &folderIdNameMap)
{
    //WStandardItemModel *result = new WStandardItemModel();
    WStandardItem *level;
    //std::map<int, WStandardItem*> folderIdNameMap;
    dbo::Transaction transaction(session_);
    try{
    /// id,parent,version,page,level,owner
    //WString query = "select ref,parent_ref,version,0 as page, level, owner,name from goods_simple where is_group  and state = 0 order by order_id";//and not not_visible
    WString query = "select ref,parent_ref,version,0 as page, level, owner,name from goods_simple goods where is_group and exists(select count(*) from goods_simple where ref = goods.parent_ref and state=0) and state = 0 order by order_id ";
    Folders folders = session_.query< FolderItem >(query.toUTF8().c_str());
    for (Folders::const_iterator i = folders.begin(); i != folders.end(); ++i)
    {
        FolderItem item = *i;
        std::string name = item.get<6>();
        std::string id = item.get<0>();
        std::string parent = item.get<1>();
        level = dynamic_cast<WStandardItem*>(folderIdNameMap[parent]);
        if(level)
        {
            //log("info")<<" id "<<id<<" parent "<<parent<<" name "<<name<<" found level "<< level;
            WStandardItem *level1;
            level->appendRow(level1 = createFolderItem(name,id));
            folderIdNameMap[id] = level1;
        }else{
            //log("info")<<" id "<<id<<" not found parent "<<parent<<" name "<<name;
            if(parent == "00000000-0000-0000-0000-000000000000")
            {
                level = createFolderItem(name,id);
                foldersModel->appendRow(level);
                folderIdNameMap[id] = level;
            }
        }

    }

    transaction.commit();
}
    catch(...)
    {
        log("EXCEPTION")<<"addOrderFromFile "<<__LINE__;
        WMessageBox::show("Exception !", WString("Произошла исключительная ситуация",UTF8), Ok);
        transaction.rollback();
    }
    //log("info")<<" MAP SIZE "<<folderIdNameMap.size();
    //return result;

}


void Session::getFoldersModel(WPopupMenu *popup,std::map<std::string, WPopupMenu*> &folderIdNameMap)
{
    //WStandardItemModel *result = new WStandardItemModel();
    WPopupMenu *subLevel;
    //std::map<int, WStandardItem*> folderIdNameMap;
    dbo::Transaction transaction(session_);
    try{
    /// id,parent,version,page,level,owner
    //WString query = "select ref,parent_ref,version,0 as page, level, owner,name from goods_simple where is_group  and state = 0 order by order_id";//and not not_visible
    WString query = "select ref,parent_ref,version,order_id / 120 as page, level, owner,name from goods_simple goods where is_group and exists(select count(*) from goods_simple where ref = goods.parent_ref and state=0) and state = 0 and level < 3 order by order_id ";
    Folders folders = session_.query< FolderItem >(query.toUTF8().c_str());
    for (Folders::const_iterator i = folders.begin(); i != folders.end(); ++i)
    {
        FolderItem item = *i;
        std::string name = item.get<6>();
        std::string id = item.get<0>();
        std::string parent = item.get<1>();
        int page = item.get<3>();
        subLevel = dynamic_cast<WPopupMenu*>(folderIdNameMap[parent]);
        if(subLevel)
        {
            //log("info")<<" id "<<id<<" parent "<<parent<<" name "<<name<<" found level "<< subLevel;
            //WMenuItem *level1 =
            WMenuItem *mi = subLevel->addItem(boost::lexical_cast<std::string>(page),Wt::WString::fromUTF8(name));
            mi->setPathComponent(boost::lexical_cast<std::string>(page+1));
            //folderIdNameMap[id] = level1;
        }else{
            //log("info")<<" id "<<id<<" not found parent "<<parent<<" name "<<name;
            if(parent == "00000000-0000-0000-0000-000000000000")
            {
                //level = createFolderItem(name,id);
                WMenuItem *level = new Wt::WMenuItem(boost::lexical_cast<std::string>(page),Wt::WString::fromUTF8(name));
                WPopupMenu *popupSubMenu = new Wt::WPopupMenu(popup->contentsStack());
                level->setMenu(popupSubMenu);
                popup->addItem(level);
                popupSubMenu->addStyleClass("dropdown-menu submenu");
                //popupSubMenu->removeStyleClass("nav",true);
                //popupSubMenu->scheduleRender("");
                popupSubMenu->setInternalPathEnabled("shop");
                //level->widget(0)->removeStyleClass("nav",true);
                folderIdNameMap[id] = popupSubMenu;//8036
            }
        }

    }

/*
    WPopupMenu *popup = new Wt::WPopupMenu(m_content);
    popup->addItem(Wt::WString::fromUTF8("Малярные изделия"));
    popup->addItem(Wt::WString::fromUTF8("Кисти"));

    WMenuItem *item = new Wt::WMenuItem("FIT");
    item->setMenu(popup);
    //foldersModel = new Wt::WPopupMenu(m_content);
    foldersModel->addItem(item);


    foldersModel->addItem("KF");
    foldersModel->addItem("SALE");
 */
    transaction.commit();
}
    catch(...)
    {
        log("EXCEPTION")<<"addOrderFromFile "<<__LINE__;
        WMessageBox::show("Exception !", WString("Произошла исключительная ситуация",UTF8), Ok);
        transaction.rollback();
    }
    //log("info")<<" MAP SIZE "<<folderIdNameMap.size();
    //return result;

}


//std::vector<std::string,std::string>Session::getFolders(std::string parent)

boost::any Session::convertData(const WString &s)
{

    boost::any data;
    char *endptr;

    if (s.empty())
    {
        data = boost::any(Wt::WString(""));
        //log("info")<<" !!! -- "<<__LINE__;
    }

    else
    {
        double d = strtod(s.toUTF8().c_str(), &endptr);
        if (*endptr == 0)
            data = boost::any(d);
        else
            data = boost::any(s);

    }
    return data;
}

WStandardItemModel *Session::getGoodsModel(const std::string &parent,WStandardItemModel *model)
{
    //WStandardItemModel *result = new WStandardItemModel();
    //model->clear();
    dbo::Transaction transaction(session_);
    /// id,parent,price_type,price_type_id,price,name
    //std::string query = "select id, CASE WHEN is_group AND id <> ? THEN 1 WHEN id = ? THEN 2 ELSE 0 END is_group, 0.00 as price, 0::integer price_type, parent, 0, name from goods_simple where parent <> 0 AND (parent  = ? or id = ?) order by order_id ";//not is_group  and
    //std::string query = "SELECT a get_user_price_list( ?, ? ) a;";
    ///CatalogList catalog = session_.query<CatalogPtr>("SELECT a from get_catalog_list(?,?,?) a;").bind(page-1).bind(user().id()).bind(part);
    log("log")<<" parent "<<parent<<" user "<<user().id()<<" ******************** ";
    CalcOptionsList optionsList = session_.query<CalcOptionsPtr>("SELECT a from get_user_price_list(?,?) as a(ref uuid ,parent_ref uuid,version integer,is_group integer, value numeric, base numeric, price numeric, static bool, percent bool, name text,code text,round_id integer);").bind(parent).bind(boost::lexical_cast<std::string>(user().id()));
    for(int r = 0;r < model->rowCount()-1;r++)
    {
        model->removeRow(r);
    }
        //model->insertColumns(0,7);
    int dataRow = 0;
    //int columnCount = 6;
    for (CalcOptionsList::const_iterator i = optionsList.begin(); i != optionsList.end(); ++i)
    {
        CalcOptionsPtr item = *i;
        //std::string name = item.get<6>();
        std::string id = item.id();
        std::string parent = item->parentRef;
        std::string code = item->code;
        int isGroup = item->isGroup;
        int rounding = item->rounding;
        //log("info")<<" price "<<item->price<<" value "<<item->value<<" isGroup "<<isGroup<<" ******** ****** ";
        double price = item->price;
        double value = item->value;
        double base = item->base;

        int valueType = 1;
        if(item->isPercent)
            valueType = 2;
        int isStatic = 1;
        if(item->isStatic)
            isStatic = 2;


        if (dataRow >= model->rowCount())
            model->insertRows(model->rowCount(),
                              dataRow + 1 - model->rowCount());

        WString s;
        char number[50];
        model->setData(dataRow, 0, boost::any(id),UserRole);/// id
        model->setData(dataRow, 1, boost::any(isGroup),UserRole);/// group type Группа / Элемент
        model->setData(dataRow, 2, boost::any(value),UserRole);/// value
        model->setData(dataRow, 3, boost::any(valueType),UserRole);/// value type Процент / Цена
        model->setData(dataRow, 4, boost::any(isStatic),UserRole);/// price type Динамический / Статический
        model->setData(dataRow, 5, boost::any(parent),UserRole);/// Парент
        model->setData(dataRow, 6, boost::any(WDate::currentDate()),UserRole);/// Дата принятия изменений
        model->setData(dataRow, 7, boost::any(rounding),UserRole);/// Дата принятия изменений

        model->item(dataRow, 0)->setText(s = WString("{1}").arg( id));
        model->item(dataRow, 3)->setText(s = WString::fromUTF8(item->name));
        //model->item(dataRow, 2)->setData(boost::any(value));
        //model->item(dataRow, 3)->setData(boost::any(price));

        //log("info")<<model->data(dataRow, 3,UserRole);

        /// 0 не выбран не один вид наценки
        //model->item(dataRow, 2)->setText("0.00%");
        //model->item(dataRow, 3)->setText("0.00");
        model->item(dataRow, 2)->setText(code);
        model->item(dataRow, 4)->setText("0.00");
        model->item(dataRow, 5)->setText("0.00%");
        model->item(dataRow, 6)->setText("0.00");

        sprintf(number,"%.2f",base);
        model->item(dataRow, 4)->setText(number);

        //if(valueType == 2)
        if(value && !price)
        {
            sprintf(number,"%.2f%%",value);
            model->item(dataRow, 5)->setText(number);
            sprintf(number,"%.2f",base+base/100*value);
            model->item(dataRow, 6)->setText(number);
        }else{
            sprintf(number,"%.2f%%",(price/base-1)*100);
            model->item(dataRow, 5)->setText(number);
            sprintf(number,"%.2f",price);
            model->item(dataRow, 6)->setText(number);
        }


        WStandardItem *fitem = model->item(dataRow, 0);
        fitem->setFlags(fitem->flags() | ItemIsDragEnabled);
        if(isGroup==1)
            fitem->setIcon("icons/folder.gif");
        else if (isGroup==2)
            fitem->setIcon("icons/folder-open.png");
        else
            fitem->setIcon("icons/file.gif");

        WStandardItem *pitem = model->item(dataRow, 1);
        pitem->setFlags(pitem->flags() | ItemIsDragEnabled);
        if(isGroup!=2)
            pitem->setIcon("icons/gtk-edit.png");

        dataRow++;
    }

    transaction.commit();
    return model;
}

void Session::updateUserPrice(const std::string& id,const int& valueType,const double& value,const int& priceType,const bool& changeFixed,const WDate& modifiDate,int rounding)
{
    dbo::Transaction transaction(session_);
#ifdef DEBUG
    log("info")<<" SELECT set_user_price_options("<<user().id()<<","<<priceType<<","<<modifiDate.toString("yyyy.MM.dd")<<","<<value<<","<<id<<")";
#endif
    session_.execute("select set_user_price_options(?,?,?,?,?,?)").bind(boost::lexical_cast<std::string>(user().id())).bind(priceType).bind(modifiDate).bind(value).bind(id).bind(rounding);
#ifdef DEBUG
    log("info")<<" SELECT set_user_price_value("<<user().id()<<","<<modifiDate.toString("yyyy.MM.dd")<<","<<id<<")";
#endif
    session_.execute("select set_user_price_value(?,?,?)").bind(boost::lexical_cast<std::string>(user().id())).bind(modifiDate).bind(id);
    transaction.commit();

}

std::vector<FolderItem> Session::folders(std::string id)
{
    dbo::Transaction transaction(session_);
    /*
        std::vector<int>::iterator the_iterator;

        //where is_group and (level = 1 or parent  = 1 or  parent  = 3118 or  parent  = 3239 or parent  = 3262 or parent  = 14937)

        WString whereStr = "is_group and (level = 1 or parent  = 1";
        the_iterator = parents.begin();
        while (the_iterator != parents.end()) {
            whereStr += WString(" or parent {1}").arg(*the_iterator); // Обратите внимание, что доступ к элементу можно получить посредством разыменования итератора
            ++the_iterator;
        }
        whereStr += ")";
    */
    WString query = WString("SELECT ref,parent_ref,version,page,level,owner,name from get_folder_list('{1}') as "
                            "(ref uuid, parent_ref uuid,version int,page int,name text,id_char varchar(9),level int,owner int,id_rec int,not_visible boolean);").arg(id);


    //typedef boost::tuple< int,int,int,int,int,std::string > Row;
    //typedef dbo::collection< Row > Folders;

    Folders folders = session_.query< FolderItem >(query.toUTF8().c_str());
    //FolderList folders = session_.query< FolderItem  >(query.toUTF8().c_str());
    //boost::tuple<int, int>
    std::vector<FolderItem> result;
    for (Folders::const_iterator i = folders.begin(); i != folders.end(); ++i)
    {
        //FolderItem item = *i;
        FolderItem item = *i;
        result.push_back(item);
    }

    transaction.commit();
    return result;

}

int Session::findCode(const WString& code)
{
#ifdef DEBUG
    log("info")<<" FIND CODE "<<code;
#endif
    int result= 0;
    dbo::Transaction transaction(session_);
    result = session_.query<int>("select ceiling((goods_simple.order_id-1)/120::float) from goods_simple where goods_simple.code = ? and state = 0;").bind(code);//+(goods_simple.order_id/60)%2 order by order_id
    transaction.commit();
    return result;

}

std::vector<db::Catalog> Session::findString(const WString &findstr,const int &page,const int &part)
{
#ifdef DEBUG
    log("info")<<"std::vector<db::Catalog> Session::findString( str "<<findstr<<", page "<<page<<" part "<<part<< ")";//<<__LINE__
#endif
    std::vector<db::Catalog> result;
    dbo::Transaction transaction(session_);

    //CatalogTList catalog = session_.query<CatalogTItem>("select id,parent,owner,name,page,groupType,division,code,articul,upakovka,notVisible,isGroup,fileName,shortDescription,description,price,quantity from get_goods_list(?,?)").bind(findstr).bind(page);
    //CatalogItemList catalog = session_.query<CatalogItem>("select a from get_goods_list(?,?) a").bind(findstr).bind(page);
    //CatalogList catalog = session_.query<CatalogPtr >(WString("select a from get_goods_list('%{1}%',0) a").arg(findstr).toUTF8());//.bind(findstr).bind(page);
    //CatalogList  catalog = request.resultList();
    //sUid=boost::lexical_cast<string>((*i)->id);

#ifdef DEBUG
    log("info")<<"select a from get_goods_list( '"<<findstr<<"','"<<user()<<"',"<<page<<")";//<<__LINE__
#endif

    CatalogList catalog = session_.query<CatalogPtr >("select a from get_goods_list(?,?,?) "
        "as a(ref uuid,parent_ref uuid,version integer,code text,division integer,is_group boolean,name text,articul text,"
        "not_visible boolean,description text,short_description text,file_name text,upakovka text,char_id varchar(9),level integer,"
        "owner integer,price numeric,quantity integer,order_id integer,active boolean)"
         ).bind(findstr).bind(user()).bind(page);
    //CatalogList catalog = session_.query<CatalogPtr >(WString("select a from get_goods_list({1},{2}) a").arg(findstr).arg(page).toUTF8());
    int limitCount = 0;
    if (catalog.size())
        for (CatalogList::const_iterator i = catalog.begin(); i != catalog.end(); ++i)
        {
            //dbo::ptr<Catalog> catalog = (*i).get<0>();
            //CatalogItem catalog = (*i).catalog();//get<0>();

            CatalogPtr catalog = *i;

            db::Catalog cat(*catalog);
            //if(++limitCount > 60)
            //    cat.division = part;
            //else
                cat.division = part;

            //size_t found = cat.description.find(findstr.toUTF8());
            //cat.description = cat.description.find
            //<span class="mark">молоток</span>
            result.push_back(cat);

        }

    transaction.commit();
#ifdef DEBUG
    log("info")<<"std::vector<db::Catalog> Session::findString("<<catalog.size()<<","<<limitCount+")"<<__LINE__;
#endif
    return result;

}


int Session::maxPage(const WString &findstr)
{
    int result = 0;
    dbo::Transaction transaction(session_);
    std::string divider = "120";
#ifndef FIT
    divider = "60";
#else
    divider = "120";
#endif
    //CatalogList catalog = session_.find<Catalog>().where("page in (select max(id) from goods_pages)");
    if (findstr.empty())
        result = session_.query<int>("select max(order_id)/"+divider+"+1 from goods_simple");
    else
        result = session_.query<int>("select count(1)/"+divider+"+1 as pages from goods_simple,prepareTsquery(?) q WHERE fts_goods @@ q and state = 0")
                 .bind(findstr).limit(1);
    transaction.commit();
#ifdef DEBUG
    log("info")<<" MAX PAGE IS "<<result;
#endif
    return result;

}

int Session::getCatalogFirstPage(const int owner)
{
    int result = 0;
    dbo::Transaction transaction(session_);

    //CatalogList catalog = session_.find<Catalog>().where("page in (select max(id) from goods_pages)");
    result = session_.query<int>("select count(*)/120+1 from goods_simple").where("owner < ? and state = 0").bind(owner);
    //result = session_.query<int>("select count(goods_simple.char_id)/40 as pages from goods_simple where (lower(goods_simple.description) LIKE lower('%'||?||'%') or lower(goods_simple.name) LIKE lower('%'||?||'%') or lower(goods_simple.articul) LIKE lower('%'||?||'%')) and not is_group")
    //             .bind(findstr).bind(findstr).bind(findstr).limit(1);
    transaction.commit();
#ifdef DEBUG
    log("info")<<" CATALOG "<<owner<<" PAGE IS "<<result;
#endif
    return result;

}


std::vector<std::string> Session::loadPriceFromFile(WStandardItemModel *loadModel)
{
    std::vector<std::string> result;
    std::string ref = "";
    std::string code = "";
    std::string price="";
    int i=0;
    std::string masterQuery = "CREATE TEMPORARY TABLE  tmpprice  "
        "(code text, "
        "price numeric(15,2)) "
        "ON COMMIT DROP; \n";
        //('0001',25),('0002',26);

    std::string query = "insert into tmpprice values \n";
    int rowCount = loadModel->rowCount();
    for( ;i<rowCount; i++)
    {
        code = "";
        price = "";
        //log("info")<<"0 "<<loadModel->item(i,0)->text();
        //log("info")<<"1 "<<loadModel->item(i,1)->text();
        if(loadModel->item(i,0))
            code = loadModel->item(i,0)->text().toUTF8();
        boost::replace_all( code, "\"", "" );
        boost::replace_all( code, "'", "" );
        boost::replace_all( code, """", "" );
        if(code.empty())
            continue;
        if(loadModel->item(i,1))
            price = loadModel->item(i,1)->text().toUTF8();
        boost::replace_all( price, ",", "." );
        boost::replace_all( price, " ", "" );
        boost::replace_all( price, " ", "" );
        if(!price.find("#"))
            continue;
        //log("info")<<"code = "<<code;
        if(price.empty())
            price = "0";
        //std::size_t found = price.find("#");
        query += "('"+code+"',"+price+")";

        if(i == rowCount-1)
            query += ";\n";
        else
            query += ",";
    }

    //std::string user_ref = user().id();
    dbo::Transaction transaction(session_);
try{
        //log("info")<<query;
        session_.execute(masterQuery);
        session_.execute(query);
        masterQuery+=query;

        dbo::collection<boost::tuple<std::string> >   codeList  = session_.query < boost::tuple<std::string> > (" select code from tmpprice group by code having count(code) > 1 ;\n");
        for (dbo::collection<boost::tuple<std::string> >::const_iterator i = codeList.begin(); i != codeList.end(); ++i)
        {

            code = (*i).get<0>();
            result.push_back(code);
        }

        session_.execute("delete from tmpprice where code in(select code from tmpprice group by code having count(code)>1);\n");

    query = "insert into calculation_options(user_ref,goods_simple_ref,price_type_id,value_date,value,price_type_ref) \n"
        "select distinct ?::uuid user_ref,goods.ref,2 price_type_id,current_date,tmp.price,price_type.ref from tmpprice tmp inner join goods_simple goods on goods.code = tmp.code inner join price_type on price_type.id = 2 \n"
        "where  not exists(SELECT goods_simple_ref FROM calculation_options c WHERE c.value_date = current_date and c.goods_simple_ref = goods.ref and c.user_ref = ?); \n";
        //log("info")<<query;
        session_.execute(query).bind(boost::lexical_cast<std::string>(user().id())).bind(boost::lexical_cast<std::string>(user().id()));
        masterQuery+=query;
    query = "update calculation_options \n"
        "SET price_type_id = 2,value = tmp.price,version = calculation_options.version+1 \n"
        "from goods_simple goods,tmpprice tmp \n"
        "where calculation_options.value_date = current_date and calculation_options.goods_simple_ref = goods.ref and goods.code = tmp.code and calculation_options.user_ref = ? and goods.state = 0; \n";
        //log("info")<<query;
        session_.execute(query).bind(boost::lexical_cast<std::string>(user().id()));
        masterQuery+=query;
    query = "insert into price_value(price_type_id,user_ref,goods_simple_ref,price_value)\n"
        "select 2,?::uuid,calc.goods_simple_ref,calc.value from calculation_options calc "
        "where calc.value_date = current_date and calc.user_ref = ? and not exists (select 1 from price_value where price_value.goods_simple_ref = calc.goods_simple_ref \n"
        "and price_value.value_date = current_date and price_value.user_ref = ?);\n";
        //log("info")<<query;
        session_.execute(query).bind(boost::lexical_cast<std::string>(user().id())).bind(boost::lexical_cast<std::string>(user().id())).bind(boost::lexical_cast<std::string>(user().id()));
        masterQuery+=query;
    query = "update price_value set price_value = result, version = price_value.version+1 from (\n"
        "select calc.value result from calculation_options calc,price_value \n"
        "where calc.goods_simple_ref = price_value.goods_simple_ref and price_value.value_date = current_date and calc.value_date = current_date and calc.user_ref = ? and price_value.user_ref = ?) a;\n";

    query = "update price_value set price_value = calc.value, version = price_value.version+1 from \n"
        "calculation_options calc \n"
        "where calc.goods_simple_ref = price_value.goods_simple_ref \n"
        "and calc.price_type_id = 2 \n"
        "and price_value.value_date = current_date \n"
        "and calc.value_date = current_date \n"
        "and calc.user_ref = ? \n"
        "and price_value.user_ref = ?; \n";

        //log("info")<<query;
        session_.execute(query).bind(boost::lexical_cast<std::string>(user().id())).bind(boost::lexical_cast<std::string>(user().id()));//.bind(user_ref).bind(user_ref);
        masterQuery+=query;
    transaction.commit();
}
        //log("info")<<masterQuery;
/*
        ref = session_.query<std::string>("select ref finds from goods_simple where code = ?").bind(code);
        if(!ref.empty()){
#ifdef DEBUG
            log("info")<<" Fselect set_user_price_options("<<user().id()<<","<<price<<","<<ref<<")";
#endif
            session_.execute("select set_user_price_options(?,?,?,?,?)").bind(user().id()).bind(2).bind(WDate::currentDate()).bind(price).bind(ref);
            session_.execute("select set_user_price_value(?,?,?)").bind(user().id()).bind(WDate::currentDate()).bind(ref);

            //updateUserPrice(ref,0,price,2,true,WDate::currentDate());
        }else{
            result.push_back(code);
        }

*/
    catch(...)
    {
        log("EXCEPTION")<<"addOrderFromFile "<<__LINE__;
        WMessageBox::show("Exception !", WString("Произошла исключительная ситуация",UTF8), Ok);
        transaction.rollback();
    }
    return  result;
}

std::vector<ImportResult> Session::addOrderFromFile(WStandardItemModel *orderGoods)
{
    std::vector<ImportResult> result;
    dbo::Transaction transaction(session_);

    try{
    std::string ref = "";
    std::string code ;
    std::string count="";

    std::string location = boost::any_cast<std::string>(userProperty("Регион"));
    std::string usrCode = boost::any_cast<std::string>(userProperty("Код 1С"));

    std::string codeList="insert into tmppdoc_"+usrCode+" values \n";
    int i=0;
    std::string masterQuery = "CREATE TEMPORARY TABLE  tmppdoc_"+usrCode+"  "
        "(code text, "
        "quantity numeric(15)) "
        "ON COMMIT DROP; \n";
        //('0001',25),('0002',26);

    std::string query = "";
    int rowCount = orderGoods->rowCount();
    for( ;i<rowCount; i++)
    {
        code = "";
        count = "";
        //log("info")<<"0 "<<loadModel->item(i,0)->text();
        //log("info")<<"1 "<<loadModel->item(i,1)->text();
        if(orderGoods->item(i,0))
            code = orderGoods->item(i,0)->text().toUTF8();
        boost::replace_all( code, "\"", "" );
        boost::replace_all( code, "'", "" );
        boost::replace_all( code, """", "" );
        if(code.empty())
            continue;
        if(orderGoods->item(i,1))
            count = orderGoods->item(i,1)->text().toUTF8();
        boost::replace_all( count, ",", "." );
        boost::replace_all( count, " ", "" );
        boost::replace_all( count, " ", "" );
        if(!count.find("#"))
            continue;
        //log("info")<<"code = "<<code;
        if(count.empty())
            count = "0";
        //std::size_t found = count.find("#");
        query += "[E'"+code+"''::text,'"+count+"'::text],";
        codeList += "('"+code+"',"+count+"),";


    }
    query.erase(query.end()-1,query.end());
    codeList.erase(codeList.end()-1,codeList.end());


    session_.execute(masterQuery);
    session_.execute(codeList);

    //masterQuery = "select code,errors from add_order_records(?,ARRAY["+query+"],?) as (code text ,errors int)";
    masterQuery = "select code,errors from add_order_records(?) as (code text ,errors int)";
    log("info")<<masterQuery;
    dbo::collection<boost::tuple<std::string,int> > badCodeList = session_.query< boost::tuple<std::string,int > >(masterQuery).bind(user());//.bind(rowCount);
    for (dbo::collection<boost::tuple<std::string,int> >::const_iterator i = badCodeList.begin(); i != badCodeList.end(); ++i)
    {
        ImportResult res;
        res.code = (*i).get<0>();
        res.errors = (*i).get<1>();
        result.push_back(res);
    }

/*
    if(find != rowCount)
    {
        dbo::collection<boost::tuple<std::string> >   badCodeList  = session_.query < boost::tuple<std::string> > ("select s from unnest((SELECT ARRAY ["+codeList+"]))s where s not in (select code from goods_simple);\n");
        //dbo::collection<boost::tuple<std::string> >   badCodeList  = session_.query < boost::tuple<std::string> > (" select code from goods_simple where code not in("+codeList+") ;\n");
        for (dbo::collection<boost::tuple<std::string> >::const_iterator i = badCodeList.begin(); i != badCodeList.end(); ++i)
        {
            code = (*i).get<0>();
            result.push_back(code);
        }

    }
    /// Ищем и удаляем дубли строк
    dbo::collection<boost::tuple<std::string> >   badCodeList  = session_.query < boost::tuple<std::string> > ("select s from unnest((SELECT ARRAY ["+codeList+"]))s where s not in (select code from goods_simple);\n");
    for (dbo::collection<boost::tuple<std::string> >::const_iterator i = badCodeList.begin(); i != badCodeList.end(); ++i)
    {
        code = (*i).get<0>();
        result.push_back(code);
    }
*/
    transaction.commit();
    }
    catch(...)
    {
        log("EXCEPTION")<<"addOrderFromFile "<<__LINE__;
        WMessageBox::show("Exception !", WString("Произошла исключительная ситуация<br/>Файл загрузить не удалось.",UTF8), Ok);
        transaction.rollback();
        for(unsigned i=0 ;i<orderGoods->rowCount(); i++)
        {
            ImportResult res;
            res.code = orderGoods->item(i,0)->text().toUTF8();
            res.errors = 1;
            result.push_back(res);
        }

    }


    return result;
}

/// Процедуры работы с заказами
/*
std::vector<std::string> Session::addOrderFromFile_(WStandardItemModel *orderGoods)
{
#ifdef DEBUG
    log("info")<<" addOrderFromFile(WStandardItemModel ) "<<__LINE__;
#endif

    std::vector<std::string> result;
    std::string code = "";
    int find = 0;
    int quantity = 0;

    dbo::Transaction transaction(session_);

    db::OrdersHeader *order = new db::OrdersHeader();
    int orderId = session_.query<int>("select nextval('orders_header_id_seq'::regclass)");
    boost::uuids::uuid uuid = boost::uuids::random_generator()();
    order->ref = uuid;
    order->author = user();
    order->orderId = orderId;
    order->orderDate = WDateTime::currentDateTime();
    //order->sessionId = WApplication::instance()->sessionId();
    m_CurrentOrder = session_.add(order);
#ifdef DEBUG
    log("info")<<"Add order header "<<m_CurrentOrder;
#endif
    int i=0;
    for( ;i<orderGoods->rowCount(); i++)
    {
        find = 0;
        code = "";
        quantity = 0;
        if(orderGoods->item(i,0))
        {
            //code = boost::any_cast<std::string>(orderGoods->item(i,0)->data());
            code = orderGoods->item(i,0)->text().trim().toUTF8();
            //log("info")<<"Code "<<code;
        }

        if(code.empty())
            continue;
        if(orderGoods->item(i,1))
        {
            //int is = boost::any_cast<int>(orderGoods->item(i,1)->data());
            //std::string ss = boost::any_cast<std::string>(orderGoods->item(i,1)->data());
            //if(is)
            //    quantity = is;
            //else
            //    quantity = atoi(ss.c_str());
            quantity = atoi(orderGoods->item(i,1)->text().toUTF8().c_str());
            //log("info")<<"quantity "<<quantity;

        }
        if(quantity)
        {
            find = session_.query<int>("select count(1) finds from goods_simple where code = ? and state = 0").bind(code);
#ifdef DEBUG
            log("info")<<"* row " <<i<<" code "<<code<<" quantity "<<quantity;
#endif

            if(find)
            {
                float price = session_.query<float>("select get_goods_price(?::text,?::uuid)").bind(code).bind(user());
                dbo::ptr<db::OrderRecords> currentRecord = session_.find<db::OrderRecords>().where("goods_simple_ref = ? and orders_header_ref = ?").bind(goods(code)).bind(m_CurrentOrder);
                if(currentRecord)
                {
                    currentRecord.modify()->quantity = quantity;
                    currentRecord.modify()->price = price;
                    currentRecord.modify()->total = price*quantity;
#ifdef DEBUG
                    log("info")<<"Update order record id "<<currentRecord.id();
#endif
                }
                else
                {
                    db::OrderRecords *record = new db::OrderRecords();
                    boost::uuids::uuid uuid = boost::uuids::random_generator()();
                    record->ref = uuid;
                    record->order = m_CurrentOrder;
                    record->goods = goods(code);
                    record->quantity = quantity;
                    record->price = price;
                    record->total = price*quantity;
                    currentRecord = session_.add(record);
#ifdef DEBUG
                    //log("info")<<"ref"<< m_CurrentOrder <<"line_number"<< 0 <<"quantity"<<quantity <<"price"<< price<<"total"<< price*quantity<<"state"<<0 <<"orders_header_ref"<< <<"goods_simple_id"
                    log("info")<<"Add order record id *"<<currentRecord.id()<<"* "<<__LINE__;
#endif
                }
                //float orderSumm = session_.query<float>("select sum(total) total from order_records where orders_header_ref = ?").bind(m_CurrentOrder);
            }
            else
            {
                result.push_back(code);
            }
        }else{

        }
    }
    float orderSumm = session_.query<float>("select sum(total) total from order_records where orders_header_ref = ?").bind(m_CurrentOrder);
    m_CurrentOrder.modify()->orderSumm = orderSumm;
    if(i)
    {
        transaction.commit();
        log("notice")<<"Transaction commited";
    }
    else
    {
        transaction.rollback();
        log("notice")<<"Transaction rollbacked";
    }
    return result;
}
*/
bool Session::addOrderRecords(const int &quantity,const std::string &goodsId,float price)
{

    //dbo::ptr<OrdersHeader> currentOrder = session_.find<OrdersHeader>().where("id = ?").bind(0);
    if(!m_CurrentOrder)
        setCurrentOrder();

    dbo::ptr<db::OrderRecords> currentRecord;
#ifdef DEBUG
    log("info")<<"m_CurrentOrder "<< m_CurrentOrder;
#endif
    dbo::Transaction transaction(session_);
    try{

    if(m_CurrentOrder)
    {
#ifdef DEBUG
        log("info")<<"Update "<<m_CurrentOrder;
#endif
        /// Если у нас текущий заказ уже проведен то создаем новый заказ
        if((m_CurrentOrder->state == db::OrdersHeader::held) || (m_CurrentOrder->state == db::OrdersHeader::process))
        {
            db::OrdersHeader *order = new db::OrdersHeader();
            boost::uuids::uuid uuid = boost::uuids::random_generator()();
            int orderId = session_.query<int>("select nextval('orders_header_id_seq'::regclass)");
            order->ref = uuid;
            order->propertyes = 0;
            order->orderSumm = 0.0;
            order->orderId = orderId;
            order->author = user();
            std::string parentUserRef = boost::any_cast<std::string>(userProperty("Владелец документов"));
            dbo::ptr<User> parentUser;
            if(parentUserRef.end() != parentUserRef.begin())
                parentUser = session_.find<User>().where("ref = ?").bind(parentUserRef);
            if(!parentUser)
                parentUser = user();
            order->user = parentUser;
            m_CurrentOrder = session_.add(order);
        }
    }

    else
 //   {
    {
        db::OrdersHeader *order = new db::OrdersHeader();
        //dbo::Transaction transaction(session_);
        //std::string ref = session_.query<std::string>("select md5(gen_salt('bf',7))::uuid ");
        boost::uuids::uuid uuid = boost::uuids::random_generator()();
        //std::string ref = boost::lexical_cast<std::string>(uuid);

        int orderId = session_.query<int>("select nextval('orders_header_id_seq'::regclass)");

        //log("info")<<"New order ref "<<uuid;
        //db::OrdersHeader *order = new db::OrdersHeader();
        order->ref = uuid;
        order->orderId = orderId;
        order->propertyes = 0;
        order->author = user();
        order->orderSumm = 0.0;
        WStringListModel *addressList = new WStringListModel();
        getAddressList(addressList);
        if(addressList->rowCount() == 2)
        {
            AddressPtr address = boost::any_cast<AddressPtr>(addressList->data(addressList->index(1,0),UserRole));
            order->address = address;
        }

        std::string parentUserRef = boost::any_cast<std::string>(userProperty("Владелец документов"));
        dbo::ptr<User> parentUser;
        //log("info") <<"Parent user "<<parentUserRef;
        if(parentUserRef.end() != parentUserRef.begin())
            parentUser = session_.find<User>().where("ref = ?").bind(parentUserRef);
        if(!parentUser)
            parentUser = user();
        //log("info") <<"Parent user "<<parentUser;
        order->user = parentUser;

        //order->orderDate = WDateTime::currentDateTime();

        //order->sessionId = licenseId;
        //order->orderRef = md5(user().id()+);
        m_CurrentOrder = session_.add(order);
        //transaction.commit();
    }
        //log("info")<<"Add order header id "<<m_CurrentOrder;
    //};
    //log("info")<<"goods_simple_id "<< goodsId<<" orders_header_ref "<<m_CurrentOrder;

    //{
        //dbo::Transaction transaction(session_);
        dbo::ptr<db::GoodsSimple> currentGoods = goods(goodsId);
        log("info")<<currentGoods<<" "<<__LINE__;
        log("info")<<user()<<" "<<__LINE__;
        std::string userRef = boost::lexical_cast<std::string>(user().id());
        if(!price)
            price = session_.query<float>(WString("select get_goods_price('{1}'::text, '{2}'::uuid)").arg(goodsId).arg(userRef).toUTF8());//.bind(goodsId)//.bind(user());
        log("info")<<user()<<" "<<__LINE__;
        currentRecord = session_.find<db::OrderRecords>().where("goods_simple_ref = ? and orders_header_ref = ?").bind(currentGoods).bind(m_CurrentOrder);
        log("info")<<currentRecord<<" "<<__LINE__;
        if(currentRecord)
        {
            currentRecord.modify()->quantity = quantity;
            currentRecord.modify()->price = price;
            currentRecord.modify()->total = price*quantity;
            //log("info")<<"Update order record id "<<currentRecord;
        }
        else
        {
            //std::string ref = session_.query<std::string>("select md5(gen_salt('bf',7))::uuid");
            boost::uuids::uuid uuid = boost::uuids::random_generator()();
            //std::string ref = boost::lexical_cast<std::string>(uuid);
            //log("info")<<"New record ref "<<uuid;
            //db::OrderRecords *record = new db::OrderRecords();
            currentRecord = session_.add(new db::OrderRecords());
            currentRecord.modify()->ref = uuid;
            currentRecord.modify()->order = m_CurrentOrder;
            currentRecord.modify()->goods = currentGoods;
            currentRecord.modify()->quantity = quantity;
            currentRecord.modify()->price = price;
            currentRecord.modify()->total = price*quantity;
            //m_CurrentOrder.modify()->orderRecords.insert(currentRecord);

            //log("info")<<"Add order record id *"<<currentRecord<<"* "<<__LINE__;
        }
        //float orderSumm = session_.query<float>("select sum(total) total from order_records where orders_header_ref = ?").bind(m_CurrentOrder);
        float orderSumm = session_.query<float>("select sum(total) total from order_records where orders_header_ref = ?").bind(m_CurrentOrder);
        m_CurrentOrder.modify()->orderSumm = orderSumm;
        //transaction.commit();
    //}
    transaction.commit();
    }
    catch(...)
    {
        log("EXCEPTION")<<"addOrderRecords "<<__LINE__;
        WMessageBox::show("Exception !", WString("Произошла исключительная ситуация",UTF8), Ok);
        transaction.rollback();
    }

        //log("info")<<"Order "<<m_CurrentOrder<<" Record "<<currentRecord;
    return true;
}

std::vector<db::Catalog> Session::getOrderRecords()
{


#ifdef DEBUG
    log("info")<<"Session::getOrderRecords("<<m_CurrentOrder<<") "<<__LINE__;
#endif

    std::vector<db::Catalog> result;

    if(m_CurrentOrder)
    {
        dbo::Transaction transaction(session_);
        //CatalogList catalog = session_.find<Catalog>().limit(limit).offset(offset);
        //CatalogList catalog = session_.find<Catalog>().where("orders_header_ref = ?").bind(m_CurrentOrder);
        //CatalogList catalog = session_.query<CatalogPtr>("select a from goods_catalog a inner join  order_records rec on a.id = rec.goods_simple_id where orders_header_ref = ?").bind(m_CurrentOrder);
        CatalogList catalog = session_.query<CatalogPtr>("select a from get_order_list(?)"
                                    "as a(ref uuid,parent_ref uuid,version integer,code text,is_group boolean,name text,articul text,not_visible "
                                    "boolean,description text,short_description text,file_name text,upakovka text,order_id int,level integer,owner integer,price numeric(15,2),quantity integer,active boolean)")
                                    .bind(m_CurrentOrder).orderBy("order_id");

        //limit(limit).offset(offset);
        for (CatalogList::const_iterator i = catalog.begin(); i != catalog.end(); ++i)
        {
            CatalogPtr catalog = *i;
            result.push_back(*catalog);

            //dbo::ptr<AuthInfo> auth = *catalog->authInfos.begin();
            //std::string name = auth->identity(Auth::Identity::LoginName).toUTF8();

            //result.back().name = name;
        }
        transaction.commit();

    }
#ifdef DEBUG
    log("info")<<"Session::getOrderRecords() "<<__LINE__;
#endif
    return result;
}

OrdersHeaderPtr Session::getCurrentOrder()
{
    return m_CurrentOrder;
}

void Session::setCurrentOrder(const int &orderId)
{
#ifdef DEBUG
    log("info")<<" setCurrentOrder( orderId "<<orderId<<")"<<__LINE__;
#endif
    //12345
    dbo::Transaction transaction(session_);

    m_CurrentOrder = session_.find<db::OrdersHeader>().where("order_id = ?").bind(orderId);

    transaction.commit();
}

void Session::setCurrentOrder(const std::string &orderUUID)
{
#ifdef DEBUG
    log("info")<<" setCurrentOrder( orderUUID "<<orderUUID<<")"<<__LINE__;
#endif
    //12345
    dbo::Transaction transaction(session_);
    m_CurrentOrder = session_.find<db::OrdersHeader>().where("ref = ?").bind(orderUUID);
    transaction.commit();
}

dbo::ptr<db::OrdersHeader> Session::setCurrentOrder()
{
#ifdef DEBUG
    log("info")<<" setCurrentOrder( user "<<user()<<"')"<<__LINE__;
#endif

    dbo::Transaction transaction(session_);
    if(session_.query<int>("select count(1) from orders_header where state = 0 and author_ref = ?").bind(user()) > 1)
    {
        session_.execute("update orders_header set state = 1 where state = 0 and author_ref = ?").bind(boost::lexical_cast<std::string>(user().id()));
    }
    m_CurrentOrder = session_.find<db::OrdersHeader>().where("state = ? and author_ref = ? ").bind(0).bind(user());//.bind(sessionId) ;
    transaction.commit();
#ifdef DEBUG
    log("info")<<" setCurrentOrder("<<m_CurrentOrder<<" ) "<<__LINE__;
#endif
    return m_CurrentOrder;
}


std::vector<db::OrdersHeader> Session::getOrdersJournal(WStandardItemModel *model,const WDate &beginDate,const WDate &endDate)
{
    std::vector<db::OrdersHeader> result;
    dbo::Transaction transaction(session_);
    try{
    endDate.addDays(1);
#ifdef DEBUG
    //log("info")<<" select * from orders_header where author_ref = '"<<user().id()<<"' and state <> "<<db::OrdersHeader::delated<<" and create_date BETWEEN "<<beginDate.toString("ddMMyy")<<" and "<<endDate.toString("ddMMyy")<<"; "<<__LINE__;
#endif


    std::string parentUserRef = boost::any_cast<std::string>(userProperty("Владелец документов"));
    if(parentUserRef.begin() == parentUserRef.end())
        parentUserRef = boost::lexical_cast<std::string>(user().id());

    log("info")<<"Parent user "<<parentUserRef;
    OrdersHeaderList orders = session_.find<db::OrdersHeader>().where("user_ref = ? and state <> ? and create_date BETWEEN ? and ?").bind(parentUserRef).bind(db::OrdersHeader::delated).bind(beginDate).bind(endDate.addDays(1)).orderBy("order_id desc");
    int dataRow = 0;
    for (OrdersHeaderList::const_iterator i = orders.begin(); i != orders.end(); ++i)
    {
        dbo::ptr<db::OrdersHeader> orders = *i;
        //db::OrdersHeader ord(*orders);
        //ord.id = orders.id();
         //log("info")<<" Order id "<<orders->orderId<<" id "<<orders.id();
        //result.push_back(*orders);
        if (dataRow >= model->rowCount())
            model->insertRows(model->rowCount(), dataRow + 1 - model->rowCount());

        std::string orderRef = boost::lexical_cast<std::string>(orders->ref);
        model->setData(dataRow, 0, boost::any(orderRef),UserRole);/// ref
        model->setData(dataRow, 1, boost::any(orders->orderDate),UserRole);/// Дата
        model->setData(dataRow, 2, boost::any(orders->orderId),UserRole);/// Номер
        std::string comments;
        for (CommentsList::const_iterator i = orders->comments.begin(); i != orders->comments.end(); ++i)
        {
            CommentsPtr record = *i;
            comments = record->comment;
        }

        model->setData(dataRow, 3, boost::any(comments),UserRole);/// Комментарий
        model->setData(dataRow, 4, boost::any(orders->orderSumm),UserRole);/// Сумма
        int state = orders->state;
        model->setData(dataRow, 5, boost::any(state),UserRole);/// Состояние
        model->setData(dataRow, 6, boost::any(orders->userNumber),UserRole);/// номер документа пользователя
        if(orders->author)
        {

            model->setData(dataRow, 7, userProperty("Наименование",boost::lexical_cast<std::string>(orders->author.id())),UserRole);/// Автор
        }

        else
            model->setData(dataRow, 7,std::string(),UserRole);

        if(orders->address)
        {
            for(auto p = orders->address->properties.begin(); p != orders->address->properties.end();++p)
            {
                AddressPropertyPtr properties = *p;
                if(properties->fieldId == 2)
                    model->setData(dataRow, 8, boost::any(properties->simpleValue),UserRole);/// Адрес доставки
            }
        }else{
            model->setData(dataRow, 8, boost::any(std::string()),UserRole);/// Адрес доставки
        }
        model->setData(dataRow, 9, boost::any(orders),UserRole);




        dataRow++;
    }
    transaction.commit();
    }

    catch(...)
    {
        log("EXCEPTION")<<"addOrderFromFile "<<__LINE__;
        WMessageBox::show("Ошибка !", WString("Произошла исключительная ситуация",UTF8), Ok);
        transaction.rollback();
    }
    return result;

}

bool Session::setEditableOrder()
{
    dbo::Transaction transaction(session_);
    m_CurrentOrder.modify()->state = db::OrdersHeader::modifed;
    transaction.commit();
    return true;
}

OrdersHeaderPtr Session::getEditableOrder()
{

#ifdef DEBUG
    log("info")<<" getEditableOrder() "<<__LINE__;
#endif


    dbo::Transaction transaction(session_);
    //("select count(1) from orders_header where author_ref = ? and state = 0")
    OrdersHeaderPtr result = session_.find<db::OrdersHeader>().where("author_ref = ? and state = 0").bind(user()).limit(1);
#ifdef DEBUG
    log("info")<<" RESULT "<<result;
#endif
    transaction.commit();
    return result;
}

std::string Session::copyOrder(std::string &ref)
{
    dbo::Transaction transaction(session_);
    //boost::uuids::uuid uuid = session_.query<std::string>("select copy_order(?)").bind(ref);
    return session_.query<std::string>("select * from copy_order(?)").bind(ref);
}

bool Session::clearCurrentOrder()
{
#ifdef DEBUG
    log("info")<<" RESET ORDER RECORDS "<<user().id();
#endif
    dbo::Transaction transaction(session_);
    m_CurrentOrder.modify()->state = db::OrdersHeader::delated;
    transaction.commit();
    return true;
}

bool Session::heldCurrentOrder(const AddressPtr &address)
{
    dbo::Transaction transaction(session_);
    m_CurrentOrder.modify()->address = session_.find<db::Address>().where("ref = ?").bind(address).limit(1);
    m_CurrentOrder.modify()->state = db::OrdersHeader::process;

    transaction.commit();
    return true;
}

bool Session::saveCurrentOrder()
{
#ifdef DEBUG
    log("info")<<" SAVE ORDER BY USER "<<user().id();
#endif
    dbo::Transaction transaction(session_);
    //m_CurrentOrder.modify()->state = OrdersHeader::saved;
    session_.execute("update orders_header set state = 1 where author_ref = ? and state = 0 ").bind(boost::lexical_cast<std::string>(user().id()));
    transaction.commit();

    return true;
}


std::string Session::getOrderComments()
{
    dbo::Transaction transaction(session_);
    dbo::ptr<db::Comments> comments = session_.find<db::Comments>().where("orders_header_ref = ?").bind(m_CurrentOrder);
    std::string result ="";
    transaction.commit();
    if (comments)
    {
        result = comments->comment;
    }
#ifdef DEBUG
    log("info")<<" COMMENT "<<result;
#endif
    return result;

}

void Session::setOrderComments(std::string comm)
{
    if(m_CurrentOrder)
    {
        dbo::Transaction transaction(session_);
        dbo::ptr<db::Comments> comments = session_.find<db::Comments>().where("orders_header_ref = ?").bind(m_CurrentOrder);
        if (comments)
            comments.modify()->comment = comm;
        else
        {
            boost::uuids::uuid uuid = boost::uuids::random_generator()();
            db::Comments *newComment = new db::Comments();
            newComment->ref = uuid;
            newComment->order = m_CurrentOrder;
            newComment->comment = comm;
            newComment->commentType = db::Comments::forOperator;
            session_.add(newComment);
        }
    #ifdef DEBUG
        log("info")<<" NEW COMMENT "<<comm;
    #endif
        transaction.commit();
    }
}


int Session::findRanking()
{
    dbo::Transaction transaction(session_);

    dbo::ptr<User> u = user();
    int ranking = -1;

    if (u)
        ranking = session_.query<int>("select distinct count(score) from user");
    //.where("score > ?").bind(u->score);

    transaction.commit();

    return ranking + 1;
}

bool Session::addUser(const std::string &id,const std::string &code,const std::string &name,const std::string &passwd,const std::string &location,const int &hash,const int &discount)
{

    dbo::Transaction transaction(session_);

    boost::uuids::string_generator gen;

    dbo::ptr<User> finduser = session_.find<User>().where("ref = ?").bind(gen(Utils::hexEncode(Utils::md5(id+location))));

    if(finduser)

    {

        //finduser.modify()->name              = name;
        //finduser.modify()->hash              = hash;
        //finduser.modify()->location          = location;
        //finduser.modify()->discount          = discount;
        //finduser.modify()->char_id           = id;
        //Auth::User newUser;
        //newUser.findWithId(finduser.id());

        //User *result = new User(*finduser);

        //myPasswordService.updatePassword(Auth::User(*finduser), passwd);
        //log("info")<<name;

    }else{
        Auth::User newUser = users_->registerNew();
        newUser.addIdentity(Auth::Identity::LoginName, name);
        newUser.setEmail(name);
        newUser.setUnverifiedEmail(name);
        myPasswordService.updatePassword(newUser, passwd);

        dbo::ptr<AuthInfo> authInfo = users_->find(newUser);

        boost::uuids::uuid uuid = gen(Utils::hexEncode(Utils::md5(id+location)));

        User *user = new User();
        user->ref = uuid;
        //user->location = location;
        //user->hash = hash;
        user->currency = 643;
        user->role = 3;
        //user->discount = discount;

        //log("info")<<name;
        //log("info")<<location;
        //log("info")<<hash;
        //log("info")<<code;
        //log("info")<<id;

        dbo::ptr<User> userPtr = session_.add(user);
        /// Добавляем идентификатор пользователя таблицы user в таблицу authInfo поле user_id
        //log("info")<<userPtr<<__LINE__;
        authInfo.modify()->setUser(userPtr);
    }
    transaction.commit();
    return true;
}

bool Session::restartSeq(std::string tableName)
{
    //dbo::Transaction transaction(session_);
    session_.execute("select pg_catalog.setval(pg_get_serial_sequence(?,'id') ,1,false); ").bind(tableName);
    //transaction.commit();
    return true;
}


bool Session::addGoods(db::GoodsSimple *goods)
{
    //dbo::Transaction transaction(session_);
    std::string char_id = goods->charId;
    log("info")<<"finding char_id'"<<char_id<<"' hash "<<goods->hash;
    Wt::Dbo::ptr<db::GoodsSimple> f = session_.find<db::GoodsSimple>().where("char_id = ? and hash = ?").bind(char_id).bind(goods->hash);
    if(f)
        return false;

    //log("info")<<"finding char_id'"<<char_id;

    Wt::Dbo::ptr<db::GoodsSimple> findelement = session_.find<db::GoodsSimple>().where("char_id = ?").bind(char_id);
    if(findelement)
    {
        findelement.modify()->charId            = goods->charId;
        findelement.modify()->charParent        = goods->charParent;
        findelement.modify()->name              = goods->name;
        findelement.modify()->articul           = goods->articul;
        findelement.modify()->notVisible        = goods->notVisible;
        findelement.modify()->description       = goods->description;
        findelement.modify()->shortDescription  = goods->shortDescription;
        findelement.modify()->fileName          = goods->fileName;
        findelement.modify()->orderAddress      = goods->orderAddress;
        findelement.modify()->upakovka          = goods->upakovka;
        findelement.modify()->hash              = goods->hash;
#ifdef DEBUG
        log("info")<<"Update record id "<<goods->charId;
#endif
        //transaction.commit();
        return true;
    };
#ifdef DEBUG
    log("info")<<"Add record id "<<char_id;
#endif
    session_.add(goods);
    /*
    try
    {
        //transaction.commit();
    }
    catch (Wt::WServer::Exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
    catch (std::exception &e)
    {
        std::cerr << "exception: " << e.what() << std::endl;
    }
    */
    //log("info")<<"Add record id ************"<<char_id;
    return true;
}

dbo::ptr<db::GoodsSimple> Session::goods(std::string code) const
{
    log("info")<<"'"<<code<<"' "<<__LINE__;
    dbo::ptr<db::GoodsSimple> res = session_.find<db::GoodsSimple>().where("code = ?").bind(code).limit(1);
    log("info")<<code<<" "<<__LINE__;
    return res;
}

bool Session::addPrice(db::Price *price)
{
    //dbo::Transaction transaction(session_);
    //dbo::ptr<Price> pricePtr =
    std::string code = price->code;
    session_.add(price);
    //transaction.commit();
    return true;
}

bool Session::addPersonalPrice(db::PersonalPrice *price)
{
    //dbo::Transaction transaction(session_);
    //dbo::ptr<PersonalPrice> pricePtr =
    session_.add(price);
    //transaction.commit();
    return true;
}

bool Session::erasePrices()
{
    //dbo::Transaction transaction(session_);
    session_.execute("delete from price");
    session_.execute("delete from personal_price");
    return true;
}


bool Session::rebuildGoodsOrder()
{
    //dbo::Transaction transaction(session_);
    session_.execute("select structured_catalog();");
    //transaction.commit();
    return true;
}

WString Session::getPageHeader(int page)
{
#ifdef DEBUG
    log("info")<<"getPageHeader( page "<<page<<")"<<__LINE__;
#endif

    WString result;
    dbo::Transaction transaction(session_);
    try{
    std::string ref = session_.query<std::string>("SELECT goods_simple.ref from goods_simple where (order_id-1) = ?*120 and state = 0").bind(page-1);
    log("info")<<"getPageHeader( id rec "<<ref<<")"<<__LINE__;
    result = session_.query<WString>("select * from get_page_header(?)").bind(ref);
    transaction.commit();
#ifdef DEBUG
    log("info")<<"getPageHeader( result "<<result<<")"<<__LINE__;
#endif
    }
    catch(...)
    {
        log("EXCEPTION")<<"addOrderFromFile "<<__LINE__;
        WMessageBox::show("Exception !", WString("Произошла исключительная ситуация",UTF8), Ok);
        transaction.rollback();
    }
    return result;
}

std::string Session::getIdFromPage(int page)
{
#ifdef DEBUG
    log("info")<<"getIdFromPage( page "<<page<<")"<<__LINE__;
#endif


    std::string result;
    dbo::Transaction transaction(session_);
    result = session_.query<std::string>("SELECT goods_simple.ref from goods_simple where (order_id-1) = ?*120 and state = 0").bind(page-1);
#ifdef DEBUG
    log("info")<<"getIdFromPage( result "<<result<<")"<<__LINE__;
#endif
    transaction.commit();
    return result;
}


std::string Session::getTerminalLicense()
{
    dbo::Transaction transaction(session_);
    std::string result = session_.query<std::string>("SELECT get_terminal_license(?)").bind(boost::lexical_cast<std::string>(user().id()));
    transaction.commit();
    return result;
}

int Session::getTerminalLicenseCount()
{
    dbo::Transaction transaction(session_);
    int result = session_.query<int>("select count(*) from user_propery where user_ref = ?").bind(boost::lexical_cast<std::string>(user().id()));
    transaction.commit();
    return result;
}


void Session::setFreeTerminalLicense(std::string mareketId)
{
    //dbo::Transaction transaction(session_);
    //session_.execute("update user_markets set is_active = false where market_id = ?").bind(mareketId);
    //transaction.commit();
}

WComboBox *Session::getRoundList(WComboBox *roundList)
{
    WStringListModel *model = new WStringListModel(roundList);
    dbo::Transaction transaction(session_);
    RoundingLst rounding = session_.find<db::Rounding>().orderBy("id");
    for (RoundingLst::const_iterator i = rounding.begin(); i != rounding.end(); ++i)
    {
        RoundingPtr roundItem = *i;
        model->addString(WString::fromUTF8(roundItem->name));
        model->setData(model->rowCount()-1,0,roundItem.id(),UserRole);
    }
    roundList->setModel(model);
    return roundList;
}

WStringListModel *Session::getAddressList(WStringListModel *model,int &currentIndex)
{
    dbo::Transaction transaction(session_);
    try{

    //WStringListModel *model = new WStringListModel(addresList);

    std::string documentOwner = asString(userProperty("Владелец документов")).toUTF8();
    if(documentOwner.begin() == documentOwner.end())
        documentOwner = boost::lexical_cast<std::string>(user().id());
    AddressLst addres = session_.find<db::Address>().where("user_ref = ? and state = 0").bind(documentOwner).orderBy("record_id");

    //int colCount = 0;
    //int currentAddres = 0;
    int index = 0;
    if(addres.size())
    {
        model->addString("");//WString::fromUTF8("Выберете адрес доставки...")
        model->setData(model->rowCount()-1,0,AddressPtr(),UserRole);
        for (AddressLst::const_iterator i = addres.begin(); i != addres.end(); ++i)
        {
            index++;
            AddressPtr address = *i;
            for(AddressPropertyLst::const_iterator p = address->properties.begin(); p != address->properties.end();++p)
            {

                AddressPropertyPtr properties = *p;
                if(properties->fieldId == 2)
                {

                    model->addString(WString::fromUTF8(properties->simpleValue));
                    //log("info")<<properties->simpleValue<<"  "<<__LINE__;
                    model->setData(model->rowCount()-1,0,address,UserRole);
                    if(m_CurrentOrder)
                        if(m_CurrentOrder->address == address)
                            currentIndex = index;

                }

            }

        }
    }
    }
    catch(...)
    {
        log("EXCEPTION")<<"addOrderFromFile "<<__LINE__;
        WMessageBox::show("Exception !", WString("Произошла исключительная ситуация",UTF8), Ok);
        transaction.rollback();
    }
    transaction.commit();
    return model;
}


void Session::getAddressList(WStringListModel *model)
{
    dbo::Transaction transaction(session_);
try{
    std::string documentOwner = asString(userProperty("Владелец документов")).toUTF8();
    if(documentOwner.begin() == documentOwner.end())
        documentOwner = boost::lexical_cast<std::string>(user().id());
    AddressLst addres = session_.find<db::Address>().where("user_ref = ? and state = 0").bind(documentOwner).orderBy("record_id");

    int index = 0;
    if(addres.size())
    {
        //addresses.push_back(WString(""));
        model->addString("");//WString::fromUTF8("Выберете адрес доставки...")
        model->setData(model->rowCount()-1,0,AddressPtr(),UserRole);
        for (AddressLst::const_iterator i = addres.begin(); i != addres.end(); ++i)
        {
            index++;
            AddressPtr address = *i;
            for(AddressPropertyLst::const_iterator p = address->properties.begin(); p != address->properties.end();++p)
            {

                AddressPropertyPtr properties = *p;
                if(properties->fieldId == 2)
                {

                    //addresses.push_back(WString::fromUTF8(properties->simpleValue));
                    model->addString(WString::fromUTF8(properties->simpleValue));
                    //log("info")<<properties->simpleValue<<"  "<<__LINE__;
                    model->setData(model->rowCount()-1,0,address,UserRole);
                    //if(m_CurrentOrder)
                    //    if(m_CurrentOrder->address == address)
                    //        currentIndex = index;

                }

            }

        }
    }
}
    catch(...)
    {
        log("EXCEPTION")<<"addOrderFromFile "<<__LINE__;
        Wt::WMessageBox::show("Exception !", WString("Произошла исключительная ситуация",UTF8), Ok);
        transaction.rollback();
    }
    transaction.commit();
}



void Session::setOrderAddress(const AddressPtr &address)
{
    dbo::Transaction transaction(session_);
    m_CurrentOrder.modify()->address = address;//session_.find<db::Address>().where("ref = ?").bind(address).limit(1);

}

void Session::setOrderAddress(const AddressPtr &address,const OrdersHeaderPtr &order)
{
    dbo::Transaction transaction(session_);
    order.modify()->address = address;//session_.find<db::Address>().where("ref = ?").bind(address).limit(1);

}


void Session::setOrderNumber(const WString &orderId,const WString &number)
{
    dbo::Transaction transaction(session_);
    dbo::ptr<db::OrdersHeader> order;
    order = session_.find<db::OrdersHeader>().where("order_id = ?").bind(orderId);
    if(order)
        order.modify()->userNumber = number.toUTF8().c_str();
    transaction.commit();

}
///********************************************************************

Auth::AbstractUserDatabase& Session::users()
{
    return *users_;
}


const Auth::AuthService& Session::auth()
{
    return myAuthService;
}

const Auth::AbstractPasswordService& Session::passwordAuth()
{
    return myPasswordService;
}

//const std::vector<const Auth::OAuthService *>& Session::oAuth()
//{
//    return myOAuthServices;
//}


