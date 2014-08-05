#include "SearchPanel.h"
#include "utils/LineEdit.h"
#include "Portal.h"

#include <Wt/WLabel>
#include <Wt/WText>
#include <Wt/WPushButton>
#include <Wt/WSuggestionPopup>
#include <Wt/WStringListModel>
#include <Wt/WTemplate>

#define INLINE_SQLQUERY(...) #__VA_ARGS__
/*
std::string getJavaScript()
{
    return INLINE_SQLQUERY
    (
    var flyBasket = {
     init: function() {
      var bsBox = $('#poisk.s_f'),
       bsPosition = bsBox.position(),
       bsOffset = bsBox.offset(),
       w = $(window);

      //if isset bsBox
      if (bsBox.length) {

       function flyBasketResize() {
        var scrollBasket = false;

        if (bsBox.hasClass('bs-scrollBasket')) {
         scrollBasket = true;
         bsBox.removeClass('bs-scrollBasket').css('left', 'auto').offset().left;
        }

        bsOffset = bsBox.offset();
        bsPosition = bsBox.position();

        if (scrollBasket)
         bsBox.addClass('bs-scrollBasket');
        w.trigger('scroll');
       };

       //events
       window.onresize = flyBasketResize;

       w.scroll(function () {
        if (w.scrollTop() >= 200) {
         bsBox.addClass('bs-scrollBasket');
         bsBox.css('left', bsOffset.left-13);
        }
        else {
         bsBox.removeClass('bs-scrollBasket');
         bsBox.css('left', 'auto');
        }
       });
      }
     }

    };
    $(function() {
     flyBasket.init();
    })
    );
}
*/
SearchPanel::SearchPanel(WContainerWidget *parent): Wt::WContainerWidget(parent),m_searchModel(0)
{
    setStyleClass("s_f");
    setObjectName("poisk_1");
    //m_searchModel = new Wt::WStringListModel(this);//new dbo::QueryModel< StringPtr >();

    update();

}

SearchPanel::~SearchPanel()
{
    //dtor
}
/*
 WSuggestionPopup *SearchPanel::createAliasesMatchingPopup(Wt::WContainerWidget *parent)
 {
    *//*
     * This matcher JavaScript function matches the input against the
     * name of a product, or one or more aliases.
     *
     * A match is indicated by product name and optionally matching aliases
     * between brackets.
     */

    /*
     * Note!
     *
     * INLINE_JAVASCRIPT is a macro which allows entry of JavaScript
     * directly in a C++ file.
     *//*
    std::string matcherJS = INLINE_JAVASCRIPT
      (
       function (edit) {
	 var value = edit.value;

	 return function(suggestion) {
	   if (!suggestion)
	     return value;

	   var i, il,
	     names = suggestion.split(';'),
	     val = value.toUpperCase(),
	     matchedAliases = [],
	     matched = null;

	   if (val.length) {
	     for (i = 0, il = names.length; i < il; ++i) {
	       var name = names[i];
	       if (name.length >= val.length
		   && name.toUpperCase().substr(0, val.length) == val) {
		 // This name matches
		 name = '<b>' + name.substr(0, val.length) + '</b>'
		   + name.substr(val.length);

		 if (i == 0) // it's the product name
		   matched = name;
		 else // it's an alias
		   matchedAliases.push(name);
	       }
	     }
	   }

	   // Let '...' always match
	   if (names[0] == '...')
	     matched = names[0];

	   if (matched || matchedAliases.length) {
	     if (!matched)
	       matched = names[0];

	     if (matchedAliases.length)
	       matched += " (" + matchedAliases.join(", ") + ")";

	     return { match : true,
		      suggestion : matched };
	   } else {
	     return { match : false,
		      suggestion : names[0] };
	   }
	 }
       }
       );

    std::string replacerJS = INLINE_JAVASCRIPT
      (
       function (edit, suggestionText, suggestionValue) {
	 edit.value = suggestionValue;

	 if (edit.selectionStart)
	   edit.selectionStart = edit.selectionEnd = suggestionValue.length;
       }
       );

    return new Wt::WSuggestionPopup(matcherJS, replacerJS, parent);
}
*/
void SearchPanel::readStringModel(WString searchStr)
{
    /*
     * We implement a virtual model contains all items that start with
     * any arbitrary 3 characters, followed by "a-z"
     */
    //log("info")<<"Search string "<<searchStr;
    dbo::Session &m_session = ((Portal *)WApplication::instance())->getSession()->getSession();
    dbo::Transaction transaction(m_session);
    StringList result = m_session.query<StringPtr>("SELECT name::varchar(70) from goods_simple where LOWER(name) LIKE LOWER(?) and not is_group and  state = 0"
                            "UNION ALL "
                            "SELECT code from goods_simple where code LIKE ? and state = 0 order by name "
                            ).bind(searchStr+"%").bind(searchStr+"%");

    for (StringList::const_iterator i = result.begin(); i != result.end(); ++i)
    {
        StringPtr item = *i;
        m_searchModel->addString(WString::fromUTF8(item.get<0>()));
    }
    transaction.commit();


}

void SearchPanel::update()
{
    //setStyleClass("s_f");

    //(new WLabel(tr("search.form.label"), this))->setBuddy(new LineEdit(this,LineEdit::Hidden));
    WContainerWidget *corner_kick = new WContainerWidget(this);
    corner_kick->setStyleClass("corner_kick");
    //new WTemplate("<ins class=\"i1\"></ins><ins class=\"i2\"></ins><ins class=\"i3\"></ins><ins class=\"i4\"></ins>",corner_kick);
    WContainerWidget *search_bg = new WContainerWidget(corner_kick);
    search_bg->setStyleClass("search_bg");
    m_searchStr = new WLineEdit(search_bg);
    //m_searchStr->setId("search_val");
    m_searchStr->enterPressed().connect(this,&SearchPanel::search);
    m_searchStr->changed().connect(this,&SearchPanel::search);
    //(new LineEdit(search_bg,LineEdit::Hidden))->setId("search_t");
    (new WPushButton(search_bg))->clicked().connect(this,&SearchPanel::search);


    /*WSuggestionPopup *popup = createAliasesMatchingPopup(corner_kick);

    popup->setModel(m_searchModel);
    popup->filterModel().connect(this,&SearchPanel::readStringModel);
    popup->forEdit(m_searchStr,WSuggestionPopup::Editing);
    popup->setFilterLength(3);
    popup->setJavaScriptMember("wtNoReparent", "true");
    popup->setMinimumSize(150, Wt::WLength::Auto);
    popup->setMaximumSize(300, 250);*/

    doJavaScript("flyBasket.init();");
}


void SearchPanel::search()
{
#ifdef DEBUG
        log("info") << "**** SEARCH *******"<<m_searchStr->text();
#endif
        m_searchChanged.emit(m_searchStr->text());

    /*
    m_currentPage = ((Portal *)WApplication::instance())->getSession()->findCode(m_searchStr->text());
    log("info")<<" PAGE "<<m_currentPage;
    if(m_currentPage)
    {
        m_currentPage--;
        WApplication::instance()->setInternalPath((WString("/shop/{1}").arg(m_currentPage+1)).toUTF8().c_str());
    }else{
        WApplication::instance()->setInternalPath("/shop/1");
    }
    */
    //m_searchStr->setText("");

}
