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

#include "CssComment.h"

CssComment::CssComment() {
  comment = "";
}

CssComment::CssComment(const Token &comment) {
  this->comment = comment;
}
void CssComment::setComment(const Token &comment) {
  this->comment = comment;
}
Token& CssComment::getComment() {
  return comment;
}

void CssComment::process(Ruleset &r) {
  CssComment* c = r.createComment();
  c->setComment(comment);
}

void CssComment::process(Stylesheet &s) {
  CssComment* c = s.createComment();
  c->setComment(comment);
}

void CssComment::write(CssWriter &writer) {
  writer.writeComment(comment);
}


