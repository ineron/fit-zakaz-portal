/*
 * Copyright (C) 2011 Emweb bvba, Kessel-Lo, Belgium.
 *
 * See the LICENSE file for terms of use.
 */

#include "overload.h"
#include "Token.h"
#include "User.h"
#include <Wt/Dbo/Impl>

DBO_INSTANTIATE_TEMPLATES(Token);

Token::Token()
{ }

Token::Token(const std::string& v, const Wt::WDateTime& e)
  : value(v),
    expires(e)
{ }
