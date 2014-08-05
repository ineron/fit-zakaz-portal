// This may look like C code, but it's really -*- C++ -*-
/*
 * Copyright (C) 2009 Emweb bvba, Kessel-Lo, Belgium.
 *
 * See the LICENSE file for terms of use.
 */
#ifndef BLOG_LOGIN_WIDGET_H_
#define BLOG_LOGIN_WIDGET_H_

#include <Wt/Auth/AuthWidget>
#include <Wt/Auth/AuthModel>

class Session;
class AuthModel;


using namespace Wt;
/*
 * Displays login, logout and registration options
 */
class PortalLoginWidget : public Wt::Auth::AuthWidget
{
public:
  PortalLoginWidget(Session& session, const std::string& basePath,
		  Wt::WContainerWidget *parent = 0);

    virtual void createLoginView();
    virtual void createLoggedInView();

protected:
    virtual WFormWidget *createFormWidget(WFormModel::Field field);
    void onChangeState();
private:
    //WCheckBox *m_terminal;
    //bool m_isTerminalMode;
    Session& session_;
};

#endif // BLOG_LOGIN_WIDGET_H_
