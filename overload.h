#ifndef OVERLOAD_H__INCLUDED
#define OVERLOAD_H__INCLUDED

#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid.hpp>            // uuid class
#include <boost/uuid/uuid_generators.hpp> // generators
#include <boost/uuid/uuid_io.hpp>         // streaming operators etc.

#include <Wt/Dbo/Types>
#include <Wt/Dbo/Impl>
#include <Wt/Dbo/WtSqlTraits>
//#include "db/DBUtils.h"

using namespace Wt;

class User;
class UserProperty;
class UserPropertyFields;


namespace db{
class Catalog;
class Catalog;
class OrdersHeader;
class OrderRecords;
class Message;
class CalcOptions;
class Comments;
class GoodsSimple;
}



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
/*
template<>
struct sql_value_traits<WDateTime, void>
{
  static const bool specialized = true;

  static const char *type(SqlConnection *conn, int size);
  static void bind(const WDateTime& v, SqlStatement *statement, int column,
                   int size);
  static bool read(WDateTime& v, SqlStatement *statement, int column, int size);
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
    typedef std::string IdType;
    static IdType invalidId() { return std::string(); }
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




#endif // OVERLOAD_H__INCLUDED
