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

#include "TokenList.h"

TokenList::~TokenList() {
}

void TokenList::ltrim() {
  while (!empty() &&
         front().type == Token::WHITESPACE) {
    pop_front();
  }
}
void TokenList::rtrim() {
  while (!empty() &&
         back().type == Token::WHITESPACE) {
    pop_back();
  }
}
void TokenList::trim() {
  ltrim();
  rtrim();
}

  
std::string TokenList::toString() const {
  std::string str;
  std::list<Token>::const_iterator it;
  
  for (it = begin(); it != end(); it++) {
    str.append(*it);
  }
  return str;
}

bool TokenList::contains(const Token &t) const {
  std::list<Token>::const_iterator it;

  for (it = begin(); it != end(); it++) {
    if (*it == t)
      return true;
  }
  return false;
}

bool TokenList::contains(Token::Type type, const std::string &str)
  const {
  return contains(Token(str, type));
}
