#ifndef DBUTILS_H
#define DBUTILS_H

#include <boost/uuid/uuid.hpp>            // uuid class
#include <boost/uuid/uuid_generators.hpp> // generators
#include <boost/uuid/uuid_io.hpp>         // streaming operators etc.

#include <Wt/Dbo/Session>
#include <Wt/Dbo/ptr>
#include <Wt/Dbo/Types>
#include <Wt/Dbo/backend/Postgres>
#include <Wt/WDateTime>

#include "../User.h"

namespace dbo = Wt::Dbo;
using namespace Wt;

namespace db
{
class Catalog;
//class Action;
class Folders_;

class OrdersHeader;
class OrderRecords;
class Message;
class CalcOptions;
class Comments;
class GoodsSimple;
class Address;
class AddressProperty;
class CompositeAddressPropertyKey;
class Rounding;
}




namespace Wt
{
namespace Dbo
{
/* Объявленя в User.h
template <class Action>
void field(Action& action, boost::uuids::uuid& uuid, const std::string& name,int size = -1)
{
    std::string id = boost::lexical_cast<std::string>(uuid);
    field(action, id, name);
    boost::uuids::string_generator gen;
    uuid = gen(id);
}
*/

template<>struct dbo_traits<db::Address> : public dbo_default_traits
{
    typedef boost::uuids::uuid IdType;
    static IdType invalidId() {return boost::uuids::uuid();}
    static const char *surrogateIdField() { return 0; }
};

template<>struct dbo_traits<db::AddressProperty> : public dbo_default_traits
{
    typedef boost::uuids::uuid IdType;
    static IdType invalidId() {return boost::uuids::uuid();}
    static const char *surrogateIdField() { return 0; }
};

/*
template <class Action> void field(Action& action, db::CompositeAddressPropertyKey& key, const std::string& name,int size = -1)
{
  field(action, key.addressRef, "address_"+name);
  field(action, key.fieldRef, "field_"+name);
}

template<>struct dbo_traits<db::AddressProperty> : public dbo_default_traits
{
    //typedef boost::uuids::uuid IdType;
    //static IdType invalidId() {return boost::uuids::uuid();}

    typedef ptr<CompositeAddressPropertyKey> IdType;
    static IdType invalidId() {return CompositeAddressPropertyKey();}

    static const char *surrogateIdField() { return 0; }
};
*/

template<>struct dbo_traits<db::Catalog> : public dbo_default_traits
{
    typedef std::string IdType;
    static IdType invalidId() { return std::string(); }
    static const char *surrogateIdField() { return 0; }
    //static const char *surrogateIdField(){return "ref";}
};


template<>struct dbo_traits<db::OrdersHeader> : public dbo_default_traits
{
    //typedef std::string IdType;
    //static IdType invalidId() { return std::string(); }
    typedef boost::uuids::uuid IdType;
    static IdType invalidId() {return boost::uuids::uuid();}
    static const char *surrogateIdField() { return 0; }
    //static const char *surrogateIdField(){return "ref";}
};


template<>struct dbo_traits<db::OrderRecords> : public dbo_default_traits
{
    //typedef std::string IdType;
    typedef boost::uuids::uuid IdType;
    //static IdType invalidId() { return std::string(); }
    static IdType invalidId() {return boost::uuids::uuid();}
    static const char *surrogateIdField(){return 0;}
    //static const char *surrogateIdField(){return "ref";}
};


template<>struct dbo_traits<db::Message> : public dbo_default_traits
{
    //typedef std::string IdType;
    typedef boost::uuids::uuid IdType;
    //static IdType invalidId() { return std::string(); }
    static IdType invalidId() {return boost::uuids::uuid();}
    static const char *surrogateIdField(){return 0;}
    //static const char *surrogateIdField() {return "ref";}
};

template<>struct dbo_traits<db::CalcOptions> : public dbo_default_traits
{
    typedef std::string IdType;
    static IdType invalidId() { return std::string(); }
    static const char *surrogateIdField(){return 0;}
    //static const char *surrogateIdField() {return "ref";}
};

template<>struct dbo_traits<db::Comments> : public dbo_default_traits
{
    //typedef std::string IdType;
    typedef boost::uuids::uuid IdType;
    //static IdType invalidId() { return std::string(); }
    static IdType invalidId() {return boost::uuids::uuid();}
    static const char *surrogateIdField(){return 0;}
    //static const char *surrogateIdField(){return "ref";}
};

template<>struct dbo_traits<db::GoodsSimple> : public dbo_default_traits
{
    typedef std::string IdType;
    static IdType invalidId() { return std::string(); }
    static const char *surrogateIdField(){return 0;}
    //static const char *surrogateIdField(){return "ref";}
};


}
}


typedef dbo::ptr<db::Catalog> CatalogPtr;
typedef dbo::collection< CatalogPtr > CatalogList;

typedef dbo::ptr<db::Comments> CommentsPtr;
typedef dbo::collection< CommentsPtr > CommentsList;


typedef dbo::ptr<db::CalcOptions> CalcOptionsPtr;
typedef dbo::collection< dbo::ptr<db::CalcOptions> > CalcOptionsList;


typedef boost::tuple< CatalogPtr > CatalogItem;
//typedef boost::tuple<Catalog > CatalogItem;
typedef dbo::collection< CatalogItem > CatalogItemList;
//typedef boost::tuple< int,int,int,int,int,std::string,int,bool,std::string,std::string,bool,std::string,std::string,std::string,std::string,std::string,int,int,int,float,int > CatalogTItem;
//typedef dbo::collection< CatalogTItem > CatalogTList;



typedef dbo::ptr<db::OrderRecords> OrderRecordsPtr;
typedef dbo::collection< OrderRecordsPtr > OrderRecordsList;


typedef dbo::ptr<db::OrdersHeader> OrdersHeaderPtr;
typedef dbo::collection< OrdersHeaderPtr > OrdersHeaderList;



typedef boost::tuple< std::string,std::string,double,double,int,int,std::string > FolderItem;
typedef boost::tuple< std::string,std::string,std::string,int,int > RightMenuItem;

//typedef Wt::Dbo::collection<CatalogPtr> CatalogList;


//typedef dbo::collection< dbo::ptr<Folders> > FolderList;
//typedef dbo::collection< dbo::ptr<db::Action> > Actions;


typedef dbo::collection< FolderItem > Folders;
typedef dbo::collection< RightMenuItem > RightMenuList;

typedef dbo::ptr<db::Message> MessagePtr;
typedef dbo::collection< MessagePtr > MessageLst;

typedef dbo::ptr<db::Address> AddressPtr;
typedef dbo::collection< AddressPtr > AddressLst;

typedef dbo::ptr<db::AddressProperty> AddressPropertyPtr;
typedef dbo::collection< AddressPropertyPtr > AddressPropertyLst;

typedef dbo::ptr<db::Rounding> RoundingPtr;
typedef dbo::collection< RoundingPtr > RoundingLst;


//typedef Wt::Dbo::ptr<Account> AccountPtr;
//typedef Wt::Dbo::collection<AccountPtr> Accounts;


//typedef boost::tuple<dbo::ptr<Folders>, int> FolderItem;
/*
dbo::QueryModel<CatalogItem> *model = new dbo::QueryModel<CatalogItem>();
model->setQuery(session.query<CatalogItem>
                ("select Post, count(Comment.id) "
                 "from post Post join comment Comment on Comment.post_id = Post.id")
                .where("Post.state = ?").bind(Post::Published)
                .groupBy("Post"));
model->addColumn("Post.date");
model->addColumn("Post.title");
model->addColumn("count(Comment.id)");
*/

namespace db
{

class Address
{
public:
    boost::uuids::uuid  ref;
    int                 recordId;
    dbo::ptr<User>      user;
    AddressPropertyLst  properties;

    template<class Action>
    void persist(Action& a)
    {
        Wt::Dbo::id(a, ref         ,"ref");
        Wt::Dbo::field(a, recordId         ,"record_id");
        dbo::belongsTo(a, user, "user");
        dbo::hasMany(a, properties, dbo::ManyToOne, "address");

    };
};

class Rounding
{
public:
    std::string         name;
    template<class Action>
    void persist(Action& a)
    {
        Wt::Dbo::field(a, name         ,"name");
    };
};


class CompositeAddressPropertyKey
{
    boost::uuids::uuid  addressRef;
    boost::uuids::uuid  fieldRef;
};

class AddressProperty
{
public:
    boost::uuids::uuid  ref;
    AddressPtr          address;
    boost::uuids::uuid  fieldRef;
    int                 fieldId;
    int                 typeId;
    int                 value;
    std::string         simpleValue;
    template<class Action>
    void persist(Action& a)
    {
        Wt::Dbo::id(a, ref                 ,"ref");
        Wt::Dbo::field(a, fieldRef         ,"field_ref");
        Wt::Dbo::field(a, fieldId          ,"field_id");
        Wt::Dbo::field(a, typeId           ,"type_id");
        Wt::Dbo::field(a, value            ,"value");
        Wt::Dbo::field(a, simpleValue      ,"simple_value");
        dbo::belongsTo(a, address          ,"address");
    };
};

class GoodsSimple
{
public:
    GoodsSimple();
public:
    std::string ref;
    std::string parentRef;
    std::string charParent;
    std::string charId;
    bool        isGroup;
    bool        notVisible;
    std::string name;
    std::string articul;
    std::string description;
    std::string shortDescription;
    std::string fileName;
    std::string code;
    int         orderAddress;
    std::string upakovka;
    int         hash;
    int         owner;
    int         orderId;
    int         level;

    template<class Action>
    void persist(Action& a)
    {
        Wt::Dbo::id(a, ref         ,"ref");
        Wt::Dbo::field(a, parentRef      ,"parent_ref");
        Wt::Dbo::field(a, charId         ,"char_id");
        Wt::Dbo::field(a, charParent     ,"char_parent");
        Wt::Dbo::field(a, isGroup         ,"is_group");
        Wt::Dbo::field(a, name            ,"name");
        Wt::Dbo::field(a, articul         ,"articul");
        Wt::Dbo::field(a, notVisible      ,"not_visible");
        Wt::Dbo::field(a, description     ,"description");
        Wt::Dbo::field(a, shortDescription,"short_description");
        Wt::Dbo::field(a, fileName        ,"file_name");
        Wt::Dbo::field(a, code            ,"code");
        Wt::Dbo::field(a, orderAddress    ,"order_address");
        Wt::Dbo::field(a, upakovka        ,"upakovka");
        Wt::Dbo::field(a, hash            ,"hash");
        Wt::Dbo::field(a, owner           ,"owner");
        Wt::Dbo::field(a, orderId         ,"order_id");
        Wt::Dbo::field(a, level           ,"level");
    };

};


class GoodsHeader
{
    int parent;
    std::string name;
    template<class Action>
    void persist(Action& a)
    {
        dbo::field(a, parent        ,"parent");
        dbo::field(a, name          ,"name");
    };
};

class Catalog
{
public:
    Catalog();
    Catalog(const Catalog& cat);
public:
    std::string ref;
    std::string parentRef;
    std::string code;
    int         division;
    int         isGroup;
    std::string name;
    std::string articul;
    int         notVisible;
    std::string description;
    std::string shortDescription;
    std::string fileName;
    std::string upakovka;
    int         owner;
    int         orderId;
    int         level;
    float       price;
    int         quantity;
    bool        active;

    template<class Action>
    void persist(Action& a)
    {

        Wt::Dbo::id(a, ref            ,"ref");
        Wt::Dbo::field(a, parentRef    ,"parent_ref");
        Wt::Dbo::field(a, code          ,"code");
        Wt::Dbo::field(a, isGroup       ,"is_group");
        Wt::Dbo::field(a, name          ,"name");
        Wt::Dbo::field(a, articul       ,"articul");
        Wt::Dbo::field(a, notVisible    ,"not_visible");
        Wt::Dbo::field(a, description   ,"description");
        Wt::Dbo::field(a, shortDescription,"short_description");
        Wt::Dbo::field(a, fileName      ,"file_name");
        Wt::Dbo::field(a, upakovka      ,"upakovka");
        Wt::Dbo::field(a, orderId     ,"order_id");
        Wt::Dbo::field(a, level         ,"level");
        Wt::Dbo::field(a, owner         ,"owner");
        Wt::Dbo::field(a, price         ,"price");
        Wt::Dbo::field(a, quantity      ,"quantity");
        Wt::Dbo::field(a, active      ,"active");

//        Wt::Dbo::field(a, page          ,"page");
        //Wt::Dbo::field(a, division      ,"division");
        //Wt::Dbo::hasMany(a, authInfos, Wt::Dbo::ManyToOne, "user");
    };
};

class Message
{
public:
    Message();
public:
    enum State
    {
        newed = 0,
        readed = 1,
        delated = 2
    };
    enum Type
    {
        input = 0,
        output = 1
    };

    dbo::ptr<User>      author;
    boost::uuids::uuid  ref;
    WDateTime           messageDate;
    State               messageState;
    Type                messageType;
    std::string         message;
    std::string         messageUid;
    std::string         fileName;
    template<class Action>
    void persist(Action& a)
    {
        Wt::Dbo::id(a, ref     ,"ref");
        Wt::Dbo::field(a, messageDate    ,"date");
        Wt::Dbo::field(a, messageState   ,"state");
        Wt::Dbo::field(a, messageUid    ,"uid");
        Wt::Dbo::field(a, messageType    ,"type");
        Wt::Dbo::field(a, message        ,"message");
        Wt::Dbo::field(a, fileName       ,"file_name");

        dbo::belongsTo(a, author, "author");
    };
};


class GoodsHistory
{
public:
    GoodsHistory();
public:
    dbo::ptr<User>      author;
    dbo::ptr<GoodsSimple>goods;
    template<class Action>
    void persist(Action& a)
    {
        dbo::belongsTo(a, author, "user");
        dbo::belongsTo(a, goods, "goods_simple");
    };
};


class Comments
{
public:
    Comments();
public:
    enum Type
    {
        forOperator = 0,
        forDriver = 1,
        forStorage = 2
    };
    //int id;
    dbo::ptr<OrdersHeader>order;
    boost::uuids::uuid  ref;
    WDateTime          commentDate;
    Type                commentType;
    std::string         comment;

    template<class Action>
    void persist(Action& a)
    {
        Wt::Dbo::id(a, ref              ,"ref");
        Wt::Dbo::field(a, commentDate   ,"comment_date");
        Wt::Dbo::field(a, commentType   ,"comment_type");
        Wt::Dbo::field(a, comment       ,"comment");

        dbo::belongsTo(a, order, "orders_header");
    };
};

class OrderRecords
{
public:
    OrderRecords();
    //Orders(const Orders& ord);
public:

    enum State
    {
        newed    = 0,
        modifed  = 3,
        delated  = 9
    };

    dbo::ptr<OrdersHeader>order;
    dbo::ptr<GoodsSimple>goods;

    State               state;
    boost::uuids::uuid  ref;
    int                 lineNumber;
    int                 quantity;
    float               price;
    float               total;
    template<class Action>
    void persist(Action& a)
    {
        dbo::id(a, ref    ,"ref");
        dbo::field(a, lineNumber    ,"line_number");
        dbo::field(a, quantity      ,"quantity");
        dbo::field(a, price         ,"price");
        dbo::field(a, total         ,"total");
        dbo::field(a, state         ,"state");

        dbo::belongsTo(a, order, "orders_header");
        dbo::belongsTo(a, goods, "goods_simple");
        //Wt::Dbo::hasMany(a, authInfos, Wt::Dbo::ManyToOne, "user");
    };
};


class OrdersHeader
{
public:
    OrdersHeader();
    //std::string newref="00000000-0000-0000-0000-000000000000"
    //OrdersHeader(const OrdersHeader& ord);
public:
    enum State
    {
        modifed    = 0,
        saved    = 1,
        process  = 2,
        held     = 3,
        //modifed  = 4,
        delated  = 9
    };

    State               state;// 0 новый 1 сохранен 2 проведен
    int                 orderId;
    boost::uuids::uuid  ref;
    dbo::ptr<User>      author;
    dbo::ptr<User>      user;
    dbo::ptr<Address>   adres;
    WDateTime           orderDate;
    //WDateTime           createDate;
    std::string         sysNumber;//Номер в 1с
    std::string         userNumber;//Идентификатор сессии
    double              orderSumm;
    int                 propertyes;
    AddressPtr          address;
    OrderRecordsList    orderRecords;
    CommentsList        comments;
    template<class Action>
    void persist(Action& a)
    {


        Wt::Dbo::id(a, ref          ,"ref");
        Wt::Dbo::field(a, orderId   ,"order_id");
        Wt::Dbo::field(a, orderDate ,"order_date");

        Wt::Dbo::field(a, userNumber,"user_number");
        Wt::Dbo::field(a, state     ,"state");
        Wt::Dbo::field(a, sysNumber ,"sys_number");

        Wt::Dbo::field(a, propertyes,"order_propertyes");
        Wt::Dbo::field(a, orderSumm ,"order_summ");

        dbo::belongsTo(a, author    ,"author");
        dbo::belongsTo(a, user      ,"user");
        dbo::hasMany(a, orderRecords, dbo::ManyToOne, "orders_header");
        dbo::hasMany(a, comments, dbo::ManyToOne, "orders_header");
        dbo::belongsTo(a, address, "address");

    };

};


class Price
{
public:
    Price();
public:
    //int id;
    //int version;
    std::string ref;
    std::string code;
    float price;
    template<class Action>
    void persist(Action& a)
    {
        //Wt::Dbo::field(a, id            ,"id");
        Wt::Dbo::field(a, code          ,"code");
        Wt::Dbo::field(a, price         ,"price");
    }

};

class PersonalPrice
{
public:
    PersonalPrice();
public:
//    int id;
//    int version;
    std::string contractor;
    std::string code;
    std::string location;
    float price;
    template<class Action>
    void persist(Action& a)
    {
        //Wt::Dbo::field(a, id            ,"id");
        Wt::Dbo::field(a, code          ,"code");
        Wt::Dbo::field(a, price         ,"price");
        Wt::Dbo::field(a, contractor    ,"contractor");
        Wt::Dbo::field(a, location      ,"location");
    }
};




class CalcOptions
{
public:
    CalcOptions();
    std::string ref;
    std::string parentRef;
    int isGroup;
    double value;
    double base;
    double price;
    bool isStatic;
    bool isPercent;
    std::string name;
    std::string code;
    int rounding;
    template<class Action>
    void persist(Action& a)
    {
        dbo::id(a, ref           ,"ref");
        dbo::field(a, parentRef  ,"parent_ref");
        dbo::field(a, isGroup    ,"is_group");
        dbo::field(a, value      ,"value");
        dbo::field(a, base       ,"base");
        dbo::field(a, price      ,"price");
        dbo::field(a, isStatic   ,"static");
        dbo::field(a, isPercent  ,"percent");
        dbo::field(a, name       ,"name");
        dbo::field(a, code       ,"code");
        dbo::field(a, rounding       ,"round_id");
    }
};

/*
class Folders_
{
public:
    Folders_();

public:
    int id;
    int parent;
    int page;
    int level;
    int owner;
    std::string name;
    template<class Action>
    void persist(Action& a)
    {
        Wt::Dbo::field(a, id            ,"id");
        Wt::Dbo::field(a, parent        ,"parent");
        Wt::Dbo::field(a, page          ,"page");
        Wt::Dbo::field(a, level         ,"level");
        Wt::Dbo::field(a, owner         ,"owner");
        Wt::Dbo::field(a, name          ,"name");

        //Wt::Dbo::hasMany(a, authInfos, Wt::Dbo::ManyToOne, "user");
    };
    //SELECT id,parent,version,page,level,owner,name from get_folder_list(162) as (id integer, parent int,version int,page int,name char(100),id_char char(9),level int,owner int,id_rec int,not_visible boolean);
};
*/
class DBUtils
{
public:
    DBUtils();
    virtual ~DBUtils();
public:
    bool query(const int &user_id,const int &action_key,const std::string &query);
    bool select(const int &user_id,const int &action_key,const std::string &query);
    //protected:
    //private:
};

}


//DBO_EXTERN_TEMPLATES(Catalog);


#endif // DBUTILS_H
