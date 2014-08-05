#ifndef APPPORTAL_H
#define APPPORTAL_H

#include <Wt/WApplication>

#include "Portal.h"

using namespace Wt;

class AppPortal : public Portal
{
public:
  AppPortal(const WEnvironment& env);

protected:
  //virtual WWidget *examples();
  virtual WWidget *createQuoteForm();
  virtual std::string filePrefix() const { return "WT-"; }

private:
  std::string wtExamplePath_;


  WWidget *wrapView(WWidget *(AppPortal::*createFunction)());
};

WApplication *createPortalApplication(const WEnvironment& env);


#endif // APPPORTAL_H
