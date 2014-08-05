#ifndef DEFAULTTHEME_H
#define DEFAULTTHEME_H

#include <Wt/WTheme>

using namespace Wt;

class DefaultTheme : public WTheme
{
public:
    /*! \brief Constructor.
     *
     * Creates a classic %Wt theme ("polished" or "default").
     */
    DefaultTheme(const std::string& name="", WObject *parent = 0);

    virtual ~DefaultTheme();

    virtual std::string name() const;

    virtual std::vector<WCssStyleSheet> styleSheets() const;

    virtual void apply(WWidget *widget, WWidget *child, int widgetRole) const;
    virtual void apply(WWidget *widget, DomElement& element, int elementRole)
    const;

    virtual std::string disabledClass() const;
    virtual std::string activeClass() const;

    virtual bool canStyleAnchorAsButton() const;

    virtual void applyValidationStyle(WWidget *widget,
                                      const Wt::WValidator::Result& validation,
                                      WFlags<ValidationStyleFlag> styles) const;
    virtual bool canBorderBoxElement(const DomElement& element) const;

private:
    std::string name_;
};

#endif // DEFAULTTHEME_H
