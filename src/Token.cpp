/*
 * Copyright 2012 Bram van der Kroef
 *
 * This file is part of LESS CSS Compiler.
 *
 * LESS CSS Compiler is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * LESS CSS Compiler is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with LESS CSS Compiler.  If not, see <http://www.gnu.org/licenses/>. 
 *
 * Author: Bram van der Kroef <bram@vanderkroef.net>
 */

#include "Token.h"

Token::Token () {
  type = OTHER;
}
Token::Token (const std::string &s, Type t) {
  type = t;
  append(s);
}
  
void Token::clear () {
  std::string::clear();
  type = OTHER;
}

bool Token::stringHasQuotes() const {
  return (at(0) == '"' ||
          at(0) == '\'');
}

void Token::removeQuotes() {
  removeQuotes(*this);
}

void Token::removeQuotes(std::string &str) const {
  char quote;
  std::string::iterator i;
  
  if (str.size() == 0 || !(str[0] == '"' ||
                           str[0] == '\''))
    return;

  quote = str[0];
  str.erase(str.begin());
  str.erase(str.end() - 1);

  for (i = str.begin(); i != str.end(); i++) {
    if (*i == '\\' && *(i + 1) == quote) 
      str.erase(i);
  }
}


std::string Token::getUrlString() const {
  std::string ret;
  if (type == URL) {
    ret = substr(4, length() - 5);
    removeQuotes(ret);
    return ret;
  } else
    return "";
}

