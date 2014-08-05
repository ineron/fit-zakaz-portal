#ifndef MSSQL7_H
#define MSSQL7_H

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sql.h>
#include <sqlext.h>
#include <string.h>
#include <locale>


#include <Wt/WStandardItemModel>
#include <Wt/WStandardItem>
#include <Wt/Dbo/SqlConnection>
#include <Wt/Dbo/SqlStatement>


using namespace Wt;
//namespace dbo = Wt::Dbo;


char* convert_1251_utf8(char* s,char* sout);

class MsSql7 //: public SqlConnection
{
    public:
        MsSql7(WStandardItemModel *model);
        virtual ~MsSql7();
    protected:

    /// Переменные
    private:
        SQLHENV henv;
        SQLHDBC hdbc;
        SQLHSTMT hstmt; /// handle of stmt
        SQLHDESC hdesc; /// handle of IRD
        SQLRETURN ret; /// ODBC API return status
        int row;
        SQLSMALLINT columns; // number of columns in result-set
        WStandardItemModel *m_dataModel;
	FILE *m_pFile;
    /// Функции
    private:
        int errorHandle(const char *where);
        int bindDate(int column,char **item_data,char *item_type);

    public:
        int connect(const char *dsn, const char *usr,const char *pwd);
        int execute(const char *query);
        int execute(const WString &query);
        WStandardItemModel *getModel();
        int disconnect (void);
    /// Операторы
    private:

};

#endif // MSSQL7_H
