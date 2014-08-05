// This may look like C code, but it's really -*- C++ -*-
/*
 * Copyright (C) 2011 Emweb bvba, Heverlee, Belgium
 *
 * See the LICENSE file for terms of use.
 */

#ifndef USER_H_
#define USER_H_

#include <string>
#include <boost/uuid/uuid.hpp>            // uuid class
#include <boost/uuid/uuid_generators.hpp> // generators
#include <boost/uuid/uuid_io.hpp>         // streaming operators etc.

#include <Wt/WDateTime>
#include <Wt/Dbo/Types>
#include <Wt/Dbo/WtSqlTraits>
#include <Wt/Auth/Dbo/AuthInfo>
#include <Wt/WApplication>

namespace dbo = Wt::Dbo;


class User;
class UserProperty;
class UserPropertyFields;

//boost::uuids::uuid

namespace Wt
{
namespace Dbo
{

template <class Action>
void field(Action& action, boost::uuids::uuid& uuid, const std::string& name,int size = -1)
{
    std::string id = boost::lexical_cast<std::string>(uuid);
    field(action, id, name);
    boost::uuids::string_generator gen;
    if(id.empty())
        id = "00000000-0000-0000-0000-000000000000";
    //Wt::WApplication::instance()->log("db::Actions ")<<""<<id;
    uuid = gen(id);
};
/*
template<>struct sql_value_traits<bool,void>
{
  static const bool specialized = true;

  static const char *type(SqlConnection *conn, int size){return "boolean not null";};
  static void bind(bool v, SqlStatement *statement, int column, int size){statement->bind(column, static_cast<int>(v ? 1 : 0));};
  static bool read(bool& v, SqlStatement *statement, int column, int size)
  {
  int intValue;
  bool result = statement->getResult(column, &intValue);
  log("info")<<"BOOL "<<intValue;
  if (result)
    v = intValue ? true : false;

  return result;

  };

};

*/
template<>struct dbo_traits<User> : public dbo_default_traits
{
    //typedef std::string IdType;
    //static IdType invalidId() { return std::string(); }
    typedef boost::uuids::uuid IdType;
    static IdType invalidId() {return boost::uuids::uuid();}
    static const char *surrogateIdField() { return 0; }
    //static const char *surrogateIdField(){return "ref";}
};

template<>struct dbo_traits<UserProperty> : public dbo_default_traits
{
    //typedef std::string IdType;
    //static IdType invalidId() { return std::string(); }
    typedef boost::uuids::uuid IdType;
    static IdType invalidId() {return boost::uuids::uuid();}
    static const char *surrogateIdField() { return 0; }
    //static const char *surrogateIdField(){return "ref";}
};

template<>struct dbo_traits<UserPropertyFields> : public dbo_default_traits
{
    //typedef std::string IdType;
    //static IdType invalidId() { return std::string(); }
    typedef boost::uuids::uuid IdType;
    static IdType invalidId() {return boost::uuids::uuid();}
    static const char *surrogateIdField() { return 0; }
    //static const char *surrogateIdField(){return "ref";}
};

}
}



typedef Wt::Auth::Dbo::AuthInfo<User> AuthInfo;
typedef Wt::Dbo::collection< Wt::Dbo::ptr<User> > Users;

typedef Wt::Dbo::collection< Wt::Dbo::ptr<UserProperty> > UserPropertyList;

typedef dbo::ptr<UserPropertyFields> UserPropertyFieldPtr;
typedef dbo::collection<UserPropertyFieldPtr > UserPropertyFieldList;

class User //: public Wt::Dbo::Dbo<User>
{
public:
    User();

    //std::string ref;
    boost::uuids::uuid  ref;
//    std::string char_id;
    std::string name; /* a copy of auth info's user name */
//    std::string code;
//    std::string location;
    int         role;
    int	      currency;

    Wt::Dbo::collection< Wt::Dbo::ptr<AuthInfo> > authInfos;
    Wt::Dbo::collection< Wt::Dbo::ptr<UserProperty> > userProperty;

    Wt::WDateTime lastLogin;

    template<class Action>
    void persist(Action& a)
    {
        //Wt::Dbo::field(a, email, "email");
        Wt::Dbo::id(a,ref,"ref");
        Wt::Dbo::field(a, role, "role");
        Wt::Dbo::field(a, currency, "currency_id");
        //Wt::Dbo::field(a, discount, "discount");
        //Wt::Dbo::field(a, char_id, "char_id");
        //Wt::Dbo::field(a, location, "location");
        Wt::Dbo::field(a, lastLogin, "last_login_attempt");

        Wt::Dbo::hasMany(a, authInfos, Wt::Dbo::ManyToOne, "user");
        Wt::Dbo::hasMany(a, userProperty, Wt::Dbo::ManyToOne, "user");


    }
};


class UserProperty
{
public:
    UserProperty();
    enum PropertyType
    {
        String = 1,
        Numeric = 2,
        Boolean = 3,
        Date = 4,
        Dictionary = 5, // Ссылка на элемент справочника
        DictionaryList = 6 // По этой ссылки есть несколько записей в справчнике
    };
public:
    //int fieldId;// номер поля
    //std::string ref;
    boost::uuids::uuid  ref;
    int typeId; // тип поля
    double value; // значение числа
    std::string simpleValue; // значение строки
    Wt::Dbo::ptr<User>      user; // ссылка на пользователя
    Wt::Dbo::ptr<UserPropertyFields>      propertyField; // ссылка на пользователя
    template<class Action>
    void persist(Action& a)
    {
        //Wt::Dbo::field(a, fieldId      ,"field_id");
        Wt::Dbo::id(a,ref,"ref");
        Wt::Dbo::field(a, typeId        ,"type_id");
        Wt::Dbo::field(a, value         ,"value");
        Wt::Dbo::field(a, simpleValue   ,"simple_value");
        Wt::Dbo::belongsTo(a, user      ,"user");
        Wt::Dbo::belongsTo(a, propertyField      ,"field");/// имя поля field должно использоваться здесь без _id
    }
};

class UserPropertyFields
{
public:
    UserPropertyFields();
public:
    //std::string ref;
    boost::uuids::uuid  ref;
    int                         type; // тип поля
    int                         fieldId;
    std::string                 name; // Наименование поля
    Wt::Dbo::collection< Wt::Dbo::ptr<UserProperty> > userProperty;// ссылка на свойства пользователя
    template<class Action>
    void persist(Action& a)
    {
        Wt::Dbo::id(a,ref,"ref");
        Wt::Dbo::field(a, type      ,"field_type");
        Wt::Dbo::field(a, fieldId   ,"id");
        Wt::Dbo::field(a, name      ,"field_name");
        Wt::Dbo::hasMany(a, userProperty, Wt::Dbo::ManyToOne, "field");
    }
};






DBO_EXTERN_TEMPLATES(User);

#endif // USER_H_
