#include "PrintResource.h"

#include <Wt/WCssDecorationStyle>
#include <Wt/WBorder>

#include <Wt/Http/Response>
#include <Wt/WContainerWidget>


PrintResource::PrintResource(WContainerWidget *content,Wt::WObject *parent)
    : Wt::WResource(parent),m_content(content)
{
    //log("info")<<"PrintResource::PrintResource()";
}

PrintResource::~PrintResource()
{
    beingDeleted(); // see "Concurrency issues" below.
}

void PrintResource::handleRequest(const Wt::Http::Request& request,
                                  Wt::Http::Response& response)
{
    //log("info")<<"PrintResource::handleRequest() "<<__LINE__;
    response.addHeader("Cache-Control", "max-age=315360000");
    response.setMimeType("text/html; charset=utf-8");
    std::ostringstream htmlStream;// = std::cout;
    htmlStream << "";

    //log("info")<<m_content->find("order_form");
    if(WContainerWidget *form = dynamic_cast<WContainerWidget*>(m_content->find("order_form")))
    {
        for(int i=0;i < form->count()-1;i++)
        {
            WWidget *item = form->widget(i);
            if(item->id() == "comments")
            {
                item->decorationStyle().setBorder(WBorder());
            }else
                item->setHidden(true);
        }
    }

    response.out() <<"<link href=\"http://netdna.bootstrapcdn.com/bootstrap/3.0.3/css/bootstrap.min.css\" type=\"text/css\" rel=\"stylesheet\">"<<std::endl;
    response.out() <<"<link href=\"http://netdna.bootstrapcdn.com/bootstrap/3.0.3/css/bootstrap-theme.min.css\" type=\"text/css\" rel=\"stylesheet\">"<<std::endl;
    response.out() << "<script src=\"/css/ie_console.js\"></script>";

    response.out() << ""
    "<script type=\"text/javascript\">\n"
    "        window.setTimeout(function () {\n"
    "            window.print();\n"
    "        }, 1000);\n"
    "</script>\n"<<std::endl;

    m_content->htmlText(response.out());
    //log("info")<<"PrintResource::handleRequest() "<<__LINE__;

/*
    response.out() << ""
    "<script type=\"text/javascript\">\n"
    "    (function() { \n"
    "        window.setTimeout(function () {\n"
    "            window.print();\n"
    "        }, 1500);\n"
    "    }); \n"
    "</script>\n"<<std::endl;

*/
    //m_content->htmlText(htmlStream);
    //std::string htmlStr= "";
    //htmlStr << htmlStream;
    //std::cout <<htmlStream<<std::endl;
    //renderer.render(WString().fromUTF8(htmlStream.str()));

    response.out() << WString().fromUTF8(htmlStream.str()) << std::endl;
}
