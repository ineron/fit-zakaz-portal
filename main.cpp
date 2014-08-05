// --docroot ./ --approot ./ --http-addres 127.0.0.1 --http-port 8080 -c wt-config.xml

/*
-lodbc
-lwt
-lwthttp
-lboost_signals
-lcrypt
#-lwtdbosqlite3
-lwtdbopostgres

xterm -T $TITLE -e
*/
#include "PortalServer.h"
#include "AppPortal.h"
#include "Session.h"


#include <Wt/WApplication>
#include <Wt/WServer>
#include <Wt/WContainerWidget>
#include <Wt/WVBoxLayout>
#include <Wt/WPushButton>
#include <Wt/WText>
#include <Wt/WLogger>


#include <stdio.h>
//#include <sqlfront.h>
#include <sql.h>
#include <sqlext.h>
//#include <QtSql>
//#include <locale>



using namespace Wt;
using namespace std;


#include <Wt/WApplication>
#include <Wt/WText>
#include <Wt/WBreak>
#include <Wt/WPopupMenu>


/*
class my_app: public WApplication
{
public:
    my_app(const WEnvironment& env);
private:
    WText* _text1;
    void item1();
    void item2();
    void subitem1();
};

my_app::my_app(const WEnvironment& env) :
    WApplication(env)
{
    _text1 = new WText("-----", root());

    new WBreak(root());

    WPopupMenu* _popup1 = new WPopupMenu();
    WPopupMenuItem* item1 = _popup1->addItem("Item 1");
    item1->triggered().connect(this, &my_app::item1);

    WPopupMenuItem* item2 = _popup1->addItem("Item 2");
    item2->triggered().connect(this, &my_app::item2);

    WPopupMenu* _popup2 = new WPopupMenu();

    WPopupMenuItem* subitem1 = _popup2->addItem("Sub Item 1");
    subitem1->triggered().connect(this, &my_app::subitem1);

    _popup1->addMenu("xxx", _popup2);

    WText* text = new WText("Clicking here will show a popup menu.", root());
    text->clicked().connect(_popup1, &WPopupMenu::popup);
    useStyleSheet("wt/wt.css");
}

void my_app::item1()
{
    _text1->setText("item1");
}

void my_app::item2()
{
    _text1->setText("item2");
}

void my_app::subitem1()
{
    // code is not reaching here
    _text1->setText("subitem1");
}

WApplication* createApplication(const WEnvironment& env)
{
    return new my_app(env);
}

int main(int argc, char** argv)
{
    return WRun(argc, argv, &createApplication);
}
*/
/*
// Класс приложения
class HelloWorld: public WApplication
{
 public:
   HelloWorld(const WEnvironment &env);
   void OnButtonClick();
 private:
   WText *text;
   WPushButton *button;
};

HelloWorld::HelloWorld(const WEnvironment &env)
 :WApplication(env)
{

 //Wt::WContainerWidget *container1 = new Wt::WContainerWidget();
 //container1->addWidget(new Wt::WText("Some text"));
 //container1->addWidget(new Wt::WImage("images/img.png"));
 //Wt::WContainerWidget *child3 = new Wt::WContainerWidget(container1);

    WContainerWidget *w = new WContainerWidget(root());
    w->resize(WLength::Auto, 600);



    Wt::WVBoxLayout *layout = new Wt::WVBoxLayout();
    layout->addWidget(new WText("One"));
    layout->addWidget(new WText("Two"));
    layout->addWidget(new WText("Three"));
    layout->addWidget(new WText("Four"));

    text = new WText("<h1>Push the button</h1>");
    button = new WPushButton("Button" );

    layout->addWidget(text);
    layout->addWidget(button);

    w->setLayout(layout);
    button->clicked().connect(this, &HelloWorld::OnButtonClick);
}

 void HelloWorld::OnButtonClick()
 {
  if (text != NULL)
    text->setText("<h1>Hello World!</h1>");
 }
*/


// Создание экземпляря и запуск
/*
 WApplication* createApplication(const WEnvironment &env)
 {
    Wt::WApplication *app = new Wt::WApplication(env);

    app->setTitle("Portal");

    app->messageResourceBundle().use(app->appRoot() + "strings");
    app->messageResourceBundle().use(app->appRoot() + "templates");

    app->useStyleSheet("css/hangman.css");

//    setLocale("");
    //new Portal(app->root());

    return app;

 }

*/
//sprintf(tmp, "DRIVER=FreeTDS[4];SERVER=%s;UID=%s;PWD=%s;DATABASE=%s;TDS_Version=5.0;Port=4444;",
//	servername, username, password, dbname);

/*
 * see Retrieving ODBC Diagnostics
 * for a definition of extract_error().
 */
/*
void extract_error(
     char *fn,
     SQLHANDLE handle,
     SQLSMALLINT type)
 {
   SQLINTEGER i = 0;
   SQLINTEGER native;
   SQLCHAR state[ 7 ];
   SQLCHAR text[256];
   SQLSMALLINT len;
   SQLRETURN ret;
   fprintf(stderr,
           "\n"
           "The driver reported the following diagnostics whilst running "
           "%s\n\n",
           fn);

   do
   {
     ret = SQLGetDiagRec(type, handle, ++i, state, &native, text,
     sizeof(text), &len );
     if (SQL_SUCCEEDED(ret))
     printf("%s:%ld:%ld:%s\n", state, i, native, text);
   }
   while( ret == SQL_SUCCESS );
 }

main() {
 SQLHENV env;
 SQLHDBC dbc;
 SQLHSTMT stmt;
 SQLRETURN ret; // ODBC API return status
 SQLCHAR outstr[1024];
 SQLSMALLINT outstrlen;

 // Allocate an environment handle
 SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &env);
 // We want ODBC 3 support
 SQLSetEnvAttr(env, SQL_ATTR_ODBC_VERSION, (void *) SQL_OV_ODBC3, 0);
 // Allocate a connection handle
 SQLAllocHandle(SQL_HANDLE_DBC, env, &dbc);
 // Connect to the DSN mydsn
 ret = SQLDriverConnect(dbc, NULL, (SQLCHAR*)"DSN=infotex;UID=sa;PWD=sa;", SQL_NTS,
		 outstr, sizeof(outstr), &outstrlen,
		 SQL_DRIVER_COMPLETE);
 if (SQL_SUCCEEDED(ret)) {
   printf("Connected\n");
   printf("Returned connection string was:\n\t%s\n", outstr);
   if (ret == SQL_SUCCESS_WITH_INFO) {
     printf("Driver reported the following diagnostics\n");
     extract_error("SQLDriverConnect", dbc, SQL_HANDLE_DBC);
   }
   SQLDisconnect(dbc);		// disconnect from driver
 } else {
   fprintf(stderr, "Failed to connect\n");
   extract_error("SQLDriverConnect", dbc, SQL_HANDLE_DBC);
 }
 // free up allocated handles
 SQLFreeHandle(SQL_HANDLE_DBC, dbc);
 SQLFreeHandle(SQL_HANDLE_ENV, env);
}

main() {
SQLHENV env;
SQLHDBC dbc;
SQLHSTMT stmt;
SQLRETURN ret; // ODBC API return status
SQLSMALLINT columns; // number of columns in result-set
int row = 0;
std::locale::global(std::locale(""));
// Allocate an environment handle
SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &env);
// We want ODBC 3 support
SQLSetEnvAttr(env, SQL_ATTR_ODBC_VERSION, (void *) SQL_OV_ODBC3, 0);
// Allocate a connection handle
SQLAllocHandle(SQL_HANDLE_DBC, env, &dbc);
// Connect to the DSN mydsn
// You will need to change mydsn to one you have created and tested
SQLDriverConnect(dbc, NULL, (SQLCHAR*)"DSN=infotex;UID=sa;PWD=sa;", SQL_NTS,
                NULL, 0, NULL, SQL_DRIVER_COMPLETE);
// Allocate a statement handle
SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
// Retrieve a list of tables
//SQLTables(stmt, (SQLCHAR*)"Torg_Infotex", SQL_NTS, NULL, 0, NULL, 0, (SQLCHAR*)"TABLE", SQL_NTS);
//SQLTables(stmt, NULL, 0, NULL, 0, NULL, 0, (SQLCHAR*)"TABLE", SQL_NTS);
SQLExecDirect(stmt, (SQLCHAR*)"select * from sc14", SQL_NTS);
// How many columns are there
SQLNumResultCols(stmt, &columns);
// Loop through the rows in the result-set
while (SQL_SUCCEEDED(ret = SQLFetch(stmt))) {
   SQLUSMALLINT i;
   printf("Row %d\n", row++);
   // Loop through the columns
   for (i = 1; i <= columns; i++) {
       SQLINTEGER indicator;
       char buf[512];
       // retrieve column data as a string
       ret = SQLGetData(stmt, i, SQL_C_CHAR,
                        buf, sizeof(buf), &indicator);
       if (SQL_SUCCEEDED(ret)) {
           // Handle null columns
           if (indicator == SQL_NULL_DATA) strcpy(buf, "NULL");
    printf("  Column %u : %s\n", i, buf);
       }
   }
 }
}


int main(int argc, char*argv[])
{
   QSqlDatabase db = QSqlDatabase::addDatabase("QODBC3");
   db.setConnectOptions("Driver={FreeTDSDriver};NetworkAddress=10.7.111.149,4101;Database=kplus;UID=kplus;PWD=kplus11;");
   db.setUserName("kplus");
   db.setPassword("kplus11");
   if( !db.open() )
   {
       qDebug() << "Connection errror:"  << db.lastError().text();
   }
   else
   {
       QSqlQuery query("SELECT country FROM artist", db);
       while (query.next())
       {
           QString country = query.value(0).toString();
           qDebug() << country;
       }
   }
}
*/

/*
#ifdef POSTGRES
  connection = new dbo::backend::Postgres
   ("host=127.0.0.1 user=test password=test port=5432 dbname=test");
#endif // POSTGRES
*/

#include <iostream>
#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>



void sendOrders()
{

}

int main(int argc, char **argv)
{

    try
    {
        PortalServer server(argv[0]);

        server.setServerConfiguration(argc, argv, WTHTTP_CONFIGURATION);
        //server.addEntryPoint(Wt::Application, createPortalApplication);
        server.addEntryPoint(Application, createPortalApplication, "", "/favicon.gif");
        //server.addEntryPoint(Wt::WidgetSet, createPortalApplication,      "/portal.js", "/css/wt/favicon.ico");



        Session::configureAuth();
        std::ofstream ofs;
        std::string value;
        server.readConfigurationProperty("logfile",value);
        if (value.begin() != value.end())
        {
            ofs.open(value, std::ofstream::out | std::ofstream::app);
            server.logger().setStream(ofs);
        }
        //server.logger().setFile("/var/log/portal.log");

/*
    Wt::WLogger logger();
    logger.addField("datetime", false);
    logger.addField("session", false);
    logger.addField("type", false);
    logger.addField("message", true);

    //logger.setFile("/var/log/portal.log");

    // Add an entry
    Wt::WLogEntry entry = logger.entry("debug");
    entry << Wt::WLogger::timestamp << Wt::WLogger::sep
    << '[' << wApp->sessionId() << ']' << Wt::WLogger::sep
    << '[' << "notice" << ']' << Wt::WLogger::sep
    << "Succesfully started.";
*/
    //std::ofstream ofs;
    //ofs.open("/var/log/wt/portal.log", std::ofstream::out | std::ofstream::app);
    //logger.setStream(ofs);


        //server.m_timer->start();
        //server.startTimer();

        if (server.start())
        {
            PortalServer::waitForShutdown();
            server.stop();
        }
    }
    catch (PortalServer::Exception& e)
    {
        log("info") << e.what();
    }
    catch (std::exception &e)
    {
        log("info") << "exception: " << e.what();
    }

//  return WRun(argc, argv, &CreateApplication);
}





