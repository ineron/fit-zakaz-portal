#include "AppPortal.h"

/*
 * Copyright (C) 2008 Emweb bvba, Kessel-Lo, Belgium.
 *
 * See the LICENSE file for terms of use.
 */

#include "AppPortal.h"

#ifdef WT_EMWEB_BUILD
#include "QuoteForm.h"
#endif // WT_EMWEB_BUILD

#include <Wt/WAnchor>
#include <Wt/WEnvironment>
#include <Wt/WLogger>
#include <Wt/WMenuItem>
#include <Wt/WStackedWidget>
#include <Wt/WTable>
#include <Wt/WTabWidget>
#include <Wt/WText>
#include <Wt/WTreeNode>
#include <Wt/WViewWidget>
#include <Wt/WWidget>

//#include "ExampleSourceViewer.h"

AppPortal::AppPortal(const WEnvironment& env)
    : Portal(env, "Web Portal ", "templates", "css/wt")
{
    addLanguage(Lang("ru", "/", "ру", "RU"));
    addLanguage(Lang("ua", "/ua/", "ua", "UA"));
    addLanguage(Lang("kz", "/kz/", "kz", "KZ"));



/*
    char* wtExamplePath  = getenv("WT_EXAMPLE_PATH");
    if (wtExamplePath)
        wtExamplePath_ = wtExamplePath;
    else
        wtExamplePath_ = "../";
*/
    init();
}


WWidget *AppPortal::createQuoteForm()
{
#ifdef WT_EMWEB_BUILD
    return new QuoteForm(QuoteForm::Wt);
#else
    return 0;
#endif
}

WWidget *AppPortal::wrapView(WWidget *(AppPortal::*createWidget)())
{
    return makeStaticModel(boost::bind(createWidget, this));
}

WApplication *createPortalApplication(const WEnvironment& env)
{
    // support for old (< Wt-2.2) homepage URLS: redirect from "states"
    // to "internal paths"
    // this contains the initial "history state" in old Wt versions
    const std::string *historyKey = env.getParameter("historyKey");

    if (historyKey)
    {
        const char *mainStr[]
        = { "main:0", "/",
            "main:1", "/news",
            "main:2", "/operator",
            "main:4", "/reports",
            "main:3", "/information",
            "main:5", "/news"
          };

        const char *exampleStr[]
        = { "example:0", "/examples",
            "example:1", "/examples/charts",
            "example:2", "/examples/wt-homepage",
            "example:3", "/examples/treelist",
            "example:4", "/examples/hangman",
            "example:5", "/examples/chat",
            "example:6", "/examples/mail-composer",
            "example:7", "/examples/drag-and-drop",
            "example:8", "/examples/file-explorer",
            "example:9", "/examples/calendar"
          };

        if (historyKey->find("main:4") != std::string::npos)
        {
            for (unsigned i = 0; i < 10; ++i)
                if (historyKey->find(exampleStr[i*2]) != std::string::npos)
                {
                    WApplication *app = new WApplication(env);
                    app->log("notice") << "redirecting old style URL '"
                    << *historyKey << "' to internal path: '"
                    << exampleStr[i*2+1] << "'";
                    app->redirect(app->bookmarkUrl(exampleStr[i*2+1]));
                    app->quit();
                    return app;
                }
        }
        else
            for (unsigned i = 0; i < 6; ++i)
                if (historyKey->find(mainStr[i*2]) != std::string::npos)
                {
                    WApplication *app = new WApplication(env);

                    app->log("notice") << "redirecting old style URL '"
                    << *historyKey << "' to internal path: '"
                    << mainStr[i*2+1] << "'";
                    app->redirect(app->bookmarkUrl(mainStr[i*2+1]));
                    app->quit();
                    return app;
                }

        // unknown history key, just continue
    }

    return new AppPortal(env);
}
