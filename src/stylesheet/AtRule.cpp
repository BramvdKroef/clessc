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

#include "Stylesheet.h"

AtRule::AtRule (const Token &keyword) {
  this->keyword = keyword;
}

AtRule::~AtRule() {
}

void AtRule::setKeyword (const Token &keyword) {
  this->keyword = keyword;
}
void AtRule::setRule(const TokenList &rule) {
  this->rule = rule;
}
Token& AtRule::getKeyword() {
  return keyword;
}
TokenList& AtRule::getRule() {
  return rule;
}

void AtRule::process(Stylesheet &s) {
  AtRule* target = s.createAtRule(keyword);
  target->setRule(rule);
      
#ifdef WITH_LIBGLOG
  VLOG(2) << "Processing @rule " << this->getKeyword() << ": " <<
    this->getRule().toString();
#endif
}

void AtRule::write(CssWriter &writer) {
  writer.writeAtRule(keyword, rule);
}
