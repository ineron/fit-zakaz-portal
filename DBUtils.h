#ifndef DBUTILS_H
#define DBUTILS_H


class DBUtils
{
    public:
        DBUtils();
        virtual ~DBUtils();
    public:
        bool query(const int &user_id,const int &action_key,const std::string &query);
    protected:
    private:
};

#endif // DBUTILS_H
