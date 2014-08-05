#ifndef CATALOGRIGHTMENU_H
#define CATALOGRIGHTMENU_H

#include "db/DBUtils.h"


#include <Wt/WContainerWidget>

typedef union {
unsigned int hex;
struct{
unsigned char alpha;
unsigned char blue;
unsigned char green;
unsigned char red;
};

} colors_;

#define MAXCOLORINDEX 35

static const colors_ colorList[] = {
{0x46c8e8ff}, {0x4ec5c3ff}, {0x8cdce7ff}, {0xb4ecdfff}, {0xb0f0d8ff},
{0x6ee4c4ff}, {0xc7dccdff}, {0xe1e6c8ff}, {0xcede61ff}, {0xfdf590ff},
{0xfcf3baff}, {0xfbf2e3ff}, {0xfac9b8ff}, {0xe7cfcdff}, {0xe0dee3ff},
{0xd3b7dfff}, {0xe2c6eeff}, {0xe3f088ff}, {0xfff46eff}, {0x4eeac6ff},
{0x6dcdcbff}, {0xb0efe2ff}, {0x6fe4d0ff}, {0xc8dcd8ff}, {0xe0e7d0ff},
{0xccff66ff}, {0xffff99ff}, {0xfcf3c3ff}, {0xfbf2edff}, {0xfbc9c4ff},
{0xe6ced8ff}, {0xe0deefff}, {0xd3b7eaff}, {0x4dc4cfff}, {0x8cdcf1ff},
{0xb4eceaff}
};


using namespace Wt;
namespace dbo = Wt::Dbo;

class CatalogRightMenu : public Wt::WContainerWidget
{
public:
    CatalogRightMenu(dbo::Session &session,dbo::ptr<User> user,WContainerWidget *parent=0);
    virtual ~CatalogRightMenu();
    Signal<int>&onPagechanged(){return m_event;};
protected:
private:
    dbo::Session &m_session;
    dbo::ptr<User> m_user;
    std::map<WString,int>m_folderPages;

private:
    void init();
    void itemClicked();

private:
    Signal<int>m_event;
};

#endif // CATALOGRIGHTMENU_H
