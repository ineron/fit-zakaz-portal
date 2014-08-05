#include "MsSql7.h"

#include <iconv.h>
#include <Wt/WLogger>

MsSql7::MsSql7(WStandardItemModel *model):henv(0),hdbc(0),hstmt(0),hdesc(0),ret(0),row(0),columns(0),m_dataModel(model)
{

    //m_dataModel = new WStandardItemModel(0, 0);
    m_pFile = fopen ("./logs/sqlerror.log","w");

}

MsSql7::~MsSql7()
{
    delete
    m_dataModel;
    disconnect();		/* disconnect from driver */
    fclose (m_pFile);
}

int MsSql7::errorHandle(const char *where)
{
    unsigned char buf[250];
    unsigned char sqlstate[15];

    /*
     *  Get statement errors
     */
    while (SQLError (henv, hdbc, hstmt, sqlstate, NULL, buf, sizeof (buf), NULL) == SQL_SUCCESS)
    {
        fprintf (m_pFile, "STMT: %s || %s, SQLSTATE=%s\n", where, buf, sqlstate);
        log("error")<<"STMT: "<<where<<"  "<<buf<<", SQLSTATE="<<sqlstate<<"\n";
    }

    /*
     *  Get connection errors
     */
    while (SQLError (henv, hdbc, SQL_NULL_HSTMT, sqlstate, NULL, buf, sizeof (buf), NULL) == SQL_SUCCESS)
    {
        fprintf (m_pFile, "CONN:%s || %s, SQLSTATE=%s\n", where, buf, sqlstate);
        log("error")<<"STMT: "<<where<<" || "<<buf<<", SQLSTATE="<<sqlstate<<"\n";
    }

    /*
     *  Get environment errors
     */
    while (SQLError (henv, SQL_NULL_HDBC, SQL_NULL_HSTMT, sqlstate, NULL, buf, sizeof (buf), NULL) == SQL_SUCCESS)
    {
        fprintf (m_pFile, "ENV:%s || %s, SQLSTATE=%s\n", where, buf, sqlstate);
        log("error")<<"STMT: "<<where<<" || "<<buf<<", SQLSTATE="<<sqlstate<<"\n";
    }

    return -1;
}

int MsSql7::bindDate(int column,char **item_data,char *item_type)
{
    SQLSMALLINT type;
    SQLSMALLINT namelen;
    SQLSMALLINT nulable;
    SQLULEN size;
    SQLLEN indptr;
    SQLSMALLINT digits;
    SQLCHAR nameptr[32];
    char *dataptr;
    SQLDescribeCol (hstmt, column, nameptr, sizeof(nameptr), &namelen, &type, &size, &digits, &nulable);
    switch(type)
    {
    case SQL_CHAR:
    case SQL_VARCHAR:
        /// Выделяем строковый бфер и связываем с ним столбец
        item_data[column] = dataptr = new char[size+1];//malloc(size + 1) ;
        item_type[column] = SQL_C_CHAR;
        SQLBindCol(hstmt, column, SQL_C_CHAR, dataptr, size+1, &indptr);
        break;
    case SQL_TYPE_DATE:
    case SQL_TYPE_TIME:
    //case SQL_TYPE_TIME_WITH_TIMEZONE:
    case SQL_TYPE_TIMESTAMP:
    //case SQL_TYPE_TIMESTAMP_WITH_TIMEZONE:
    case SQL_INTERVAL_DAY:
    case SQL_INTERVAL_DAY_TO_HOUR:
    case SQL_INTERVAL_DAY_TO_MINUTE:
    case SQL_INTERVAL_DAY_TO_SECOND:
    case SQL_INTERVAL_HOUR:
    case SQL_INTERVAL_HOUR_TO_MINUTE:
    case SQL_INTERVAL_HOUR_TO_SECOND:
    case SQL_INTERVAL_MINUTE:
    case SQL_INTERVAL_MINUTE_TO_SECOND:
    case SQL_INTERVAL_MONTH:
    case SQL_INTERVAL_SECOND:
    case SQL_INTERVAL_YEAR:
    case SQL_INTERVAL_YEAR_TO_MONTH:
        /// Просим CLI преобразовать значение даты/времени в строку фиксированной длины
        item_data[column] = dataptr = new char[31];//malloc(31);
        item_type[column] = SQL_C_CHAR;
        SQLBindCol(hstmt, column, SQL_C_CHAR, dataptr, 31, &indptr);
        break;
    case SQL_INTEGER:
    case SQL_SMALLINT:
    case SQL_TINYINT:
        /// Преобразовать целое число в формат длинного целого, принятый в языке С
        item_data[column] = dataptr = new char[sizeof(SQL_INTEGER)]; //malloc(sizeof(integer));
        item_type[column] = SQL_C_SLONG;
        SQLBindCol(hstmt, column, SQL_C_SLONG, dataptr, sizeof(SQL_INTEGER), &indptr);
        break;
    case SQL_NUMERIC:
    case SQL_DECIMAL:
    case SQL_REAL:
    case SQL_FLOAT:
    case SQL_DOUBLE:
        /// Значения с плавоющей точкой
        item_data[column] = dataptr = new char[sizeof(SQL_NUMERIC)]; //malloc(sizeof(double));
        item_type[column] = SQL_C_DOUBLE;
        SQLBindCol(hstmt, column, SQL_C_DOUBLE, dataptr, 31, &indptr);
        break;
    }
    return 0;
}
/*
///"DSN=infotex_1;UID=sa;PWD=sa;"
void MsSql7::connect(const std::string& connStr)
{

    //char *scqConn = connStr.c_str();
    std::locale::global(std::locale(""));
// Allocate an environment handle
    SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv);
// We want ODBC 3 support
    SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (void *) SQL_OV_ODBC3, 0);
// Allocate a connection handle
    ret = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);
    if(!SQL_SUCCEEDED(ret))
        errorHandle("SQLAllocHandle");
// Connect to the DSN mydsn
// You will need to change mydsn to one you have created and tested
    ret = SQLDriverConnect(hdbc, NULL, (SQLCHAR*)connStr.c_str(), SQL_NTS,
                           NULL, 0, NULL, SQL_DRIVER_COMPLETE);

    if(!SQL_SUCCEEDED(ret))
        errorHandle("SQLDriverConnect");

    //errorHandle("SQLDriverConnect for dbc", dbc, SQL_HANDLE_ENV);
// Allocate a statement handle
    SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);

}
*/

/*
char* convert_1251_utf8(char* s,char* sout)//
{
    iconv_t ic = iconv_open("utf-8", "cp1251");


    if (ic == (iconv_t)(-1))
    {
        fprintf(stderr, "iconv: cannot convert from %s to %s\n", "cp1251", "utf-8");
        return (char*)NULL;
    }

    //char* out_buf = (char*)calloc(strlen(s)*2+1, 1);
    //char* out = out_buf;
    //char* s = (char*)malloc(strlen(in)+1);
    char *s = new char[strlen(in)+1];
    strcpy(s, in);

    size_t in_ln = strlen(in);
    size_t out_ln = in_ln*2;
    size_t k = iconv(ic, &s, &in_ln, &out, &out_ln);
    if(k == (size_t)-1)
        fprintf(stderr, "iconv: %u of %d converted\n", k, strlen(s));

    //if(errno != 0)
    //	fprintf(stderr, "iconv: %s\n", strerror(errno));

    iconv_close(ic);
    //free(s);
    return out;//out_buf;




    char* out_buf = (char*)calloc(strlen(s)*2+1, 1);
    char* out = out_buf;
    char* in = s;//(char*)malloc(strlen(s)+1);
    //strcpy(in, s);

    size_t in_ln = strlen(s);
    size_t out_ln = in_ln*2;
    size_t k = iconv(ic, &in, &in_ln, &out, &out_ln);
    if(k == (size_t)-1)
        fprintf(stderr, "iconv: %u of %d converted\n", k, strlen(s));
    //if(errno != 0)
    //	fprintf(stderr, "iconv: %s\n", strerror(errno));
    strcpy(sout, out_buf);
    iconv_close(ic);
    free(out_buf);
    //free(in);
    return sout;
}
*/
int MsSql7::connect (const char *dsn, const char *usr,const char *pwd)
{
    SQLRETURN rc;
    std::cout<<" DSN "<<dsn<<std::endl;
    /* Allocate environment handle */
    rc = SQLAllocHandle (SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv);
    if (!SQL_SUCCEEDED (rc))
        goto error;

    /* Set the ODBC version environment attribute */
    rc = SQLSetEnvAttr (henv, SQL_ATTR_ODBC_VERSION, (void *) SQL_OV_ODBC3, 0);
    if (!SQL_SUCCEEDED (rc))
        goto error;

    /* Allocate connection handle */
    rc = SQLAllocHandle (SQL_HANDLE_DBC, henv, &hdbc);
    if (!SQL_SUCCEEDED (rc))
        goto error;

    /* Connect to data source */
    rc = SQLConnect (hdbc, (SQLCHAR *) dsn, SQL_NTS, (SQLCHAR *) usr, SQL_NTS, (SQLCHAR *) pwd, SQL_NTS);
    if (!SQL_SUCCEEDED (rc))
        goto error;

    /* Allocate statement handle */
    rc = SQLAllocHandle (SQL_HANDLE_STMT, hdbc, &hstmt);
    if (!SQL_SUCCEEDED (rc))
        goto error;

    /* Successful connection */
    return 0;

error:
    /* Failed connection */
    errorHandle("ODBC_Connect");

    disconnect();

    return -1;
}

int MsSql7::disconnect (void)
{
    if (hstmt)
        SQLFreeHandle (SQL_HANDLE_STMT, hstmt);
    hstmt = SQL_NULL_HANDLE;

    if (hdesc)
        SQLFreeHandle (SQL_HANDLE_STMT, hdesc);


    if (hdbc)
        SQLDisconnect (hdbc);

    if (hdbc)
        SQLFreeHandle (SQL_HANDLE_DBC, hdbc);
    hdbc = SQL_NULL_HANDLE;

    if (henv)
        SQLFreeHandle (SQL_HANDLE_ENV, henv);
    henv = SQL_NULL_HANDLE;
    return 0;
}

int MsSql7::execute(const WString &query)
{
    return execute(query.toUTF8().c_str());
}

///"select * from Web_getGoodsUpdate()"
int MsSql7::execute(const char *query)
{
    //char *buf = new char[2048];
    //char *utfbuf = new char[4096];
//    char utfbuf[4096];
    char col_name[128+1];
    log("info")<<"query: "<<query;
    m_dataModel->clear();
//    log("info")<<"Column count = "<<m_dataModel->columnCount();
//    log("info")<<"Row count = "<<m_dataModel->rowCount();
    //char *strBuf;
    //strBuf = new char [query.size()+1];
    //strcpy (strBuf, query.c_str());
    fprintf(m_pFile, "Query : %s\n", query);
    ret = SQLExecDirect(hstmt, (SQLCHAR*)query, SQL_NTS);
    if(!SQL_SUCCEEDED(ret))
    {
        errorHandle("SQLExecDirect");
        return -1;
    }
    SQLGetStmtAttr(hstmt,SQL_ATTR_IMP_ROW_DESC,&hdesc,SQL_IS_POINTER,NULL);
    if(!SQL_SUCCEEDED(ret))
    {
        errorHandle("SQLGetStmtAttr");
        return -1;
    }
// How many columns are there
    SQLNumResultCols(hstmt, &columns);
// Loop through the rows in the result-set
    log("info")<<"Column count = "<<columns;
/// Добавляем нужное количество колонок
    if (columns >= m_dataModel->columnCount())
        m_dataModel->insertColumns(m_dataModel->columnCount(), columns + 1 - m_dataModel->columnCount());


    for(int i=0; i<columns; i++)
    {
        SQLSMALLINT dvtype;
        int length;
        ret = SQLGetDescField(hdesc,
                              i+1, /* look at the parameter */
                              SQL_DESC_TYPE,
                              &dvtype,//&dvtype, /* result */
                              SQL_IS_SMALLINT,//SQL_IS_SMALLINT,
                              NULL);
        ret = SQLGetDescField(hdesc,
                              i+1, /* look at the parameter */
                              SQL_DESC_NAME,
                              &col_name,//&dvtype, /* result */
                              sizeof(col_name),//SQL_IS_SMALLINT,
                              (SQLINTEGER*)&length);
//        char name[length*2];
        //std::cout<<"Field "<<i<<" Type is "<< dvtype << " Name is "<<convert_1251_utf8(col_name,name);
        log("info")<<"Field "<<i<<" Type is "<< dvtype << " Name is "<<(col_name);
//        m_dataModel->setHeaderData(i, boost::any(convert_1251_utf8(col_name,name)));
        m_dataModel->setHeaderData(i, boost::any(std::string(col_name)));
    }

    int dataRow=0;
    while (SQL_SUCCEEDED(ret = SQLFetch(hstmt)))
    {
        SQLUSMALLINT i;
        // Loop through the columns
        m_dataModel->insertRows(m_dataModel->rowCount(),1);//dataRow + 1 - m_dataModel->rowCount()
        char buf[4096];

        for (i = 1; i <= columns; i++)
        {
            SQLLEN indicator;
            // retrieve column data as a string
            ret = SQLGetData(hstmt, i, SQL_C_CHAR, buf, sizeof(buf), &indicator);

            if (SQL_SUCCEEDED(ret) )
            {
                // Handle null columns
                if (indicator == SQL_NULL_DATA)
                {
                    strcpy(buf, "\0");
                }
                //convert_1251_utf8(buf,utfbuf);
                //boost::any data;
                //data = boost::any(WString(utfbuf,UTF8));
                //m_dataModel->setData(dataRow++, i-1, boost::any(WString(utfbuf,UTF8)));
                //log("info")<<i<<" ind "<<indicator<<" buf "<<buf;
                WStandardItem *item = new WStandardItem();
                item->setText(WString(buf,UTF8));
                m_dataModel->setItem(dataRow, i-1,item);
                //fprintf(stderr, "field N: %d value %s \n", i, utfbuf);
            }
        }
        dataRow++;
    }
    //delete utfbuf;
    //delete buf;
    SQLCloseCursor(hstmt);
    return 0;
}

WStandardItemModel *MsSql7::getModel()
{
    return m_dataModel;
}





