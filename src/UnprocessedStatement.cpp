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

#include "UnprocessedStatement.h"
#include "LessRuleset.h"
#include "LessStylesheet.h"
#include "Mixin.h"

#include <config.h>

#ifdef WITH_LIBGLOG
#include <glog/logging.h>
#endif

UnprocessedStatement::UnprocessedStatement() {
  property_i = 0;
}

Selector* UnprocessedStatement::getTokens(){
  return &tokens;
}

void UnprocessedStatement::getProperty(TokenList &tokens) {
  TokenList::iterator i = this->tokens.begin();
  std::advance(i, property_i);

  tokens.insert(tokens.begin(), this->tokens.begin(), i);
}

void UnprocessedStatement::getValue(TokenList &tokens) {
  TokenList::iterator i = this->tokens.begin();
  std::advance(i, property_i);

  tokens.insert(tokens.begin(), i, this->tokens.end());
}

void UnprocessedStatement::setLessRuleset(LessRuleset &r) {
#ifdef WITH_LIBGLOG
  VLOG(3) << "Set LessRuleset";
#endif
  
  lessRuleset = &r;
}

LessRuleset* UnprocessedStatement::getLessRuleset() {
  return lessRuleset;
}

void UnprocessedStatement::insert(Stylesheet &s) {
  Mixin mixin;
  mixin.setStylesheet(getLessRuleset()->getLessStylesheet());
  
  // process mixin
  if (mixin.parse(*getTokens()))
    mixin.insert(s, *getLessRuleset()->getContext(),
                 NULL, getLessRuleset());
}

void UnprocessedStatement::process(Ruleset &r) {
  Extension extension;
  Mixin mixin;
  Declaration* declaration;

#ifdef WITH_LIBGLOG
  VLOG(2) << "Statement: " << getTokens()->toString();
#endif
  
  // process extends statement
  if (getExtension(extension.getTarget())) {
    extension.setExtension(r.getSelector());
    getLessRuleset()->getContext()->addExtension(extension);
    return;
  }
  
  mixin.setStylesheet(getLessRuleset()->getLessStylesheet());
  
  // process mixin
  if (mixin.parse(*getTokens()) &&
      mixin.insert(*r.getStylesheet(), *getLessRuleset()->getContext(),
                   &r, getLessRuleset())) {

  } else {
    declaration = r.createDeclaration();
    
    if (processDeclaration(declaration)) {
    
#ifdef WITH_LIBGLOG
      VLOG(2) << "Declaration: " <<
        declaration->getProperty() << ": " << declaration->getValue().toString();
#endif
    
      getLessRuleset()->getContext()->processValue(declaration->getValue());

#ifdef WITH_LIBGLOG
      VLOG(2) << "Processed declaration: " <<
        declaration->getProperty() << ": " << declaration->getValue().toString();
#endif
    
    } else {
      r.deleteDeclaration(*declaration);
      throw new ParseException(getTokens()->toString(),
                               "variable, mixin or declaration.",
                               line, column, source);
    }
  }

#ifdef WITH_LIBGLOG
  VLOG(3) << "Statement done";
#endif
}

bool UnprocessedStatement::isExtends() {
  TokenList::iterator i = getTokens()->begin();
  
  return ((*i) == "&" &&
          (*++i).type == Token::COLON &&
          (*++i).type == Token::IDENTIFIER &&
          (*i) == "extend" &&
          (*++i).type == Token::PAREN_OPEN);
}

bool UnprocessedStatement::getExtension(TokenList &extension) {
  TokenList::iterator i;
  int parentheses = 1;
  
  if (!isExtends())
    return false;

  i = getTokens()->begin(); 

  // &:extends(
  std::advance(i, 4);

  for (;i != getTokens()->end() && parentheses > 0; i++) {
    
    switch ((*i).type) {
    case Token::PAREN_OPEN:
      parentheses++;
      break;
    case Token::PAREN_CLOSED:
      parentheses--;
      break;
    default:
      break;
    }
    if (parentheses > 0)
      extension.push_back(*i);
  }

  if (parentheses > 0) {
    throw new ParseException("end of statement", ")", line, column, source);
  }
  return true;
}

bool UnprocessedStatement::processDeclaration (Declaration* declaration) {
  TokenList property;

#ifdef WITH_LIBGLOG
  VLOG(3) << "Declaration";
#endif
  
  getValue(declaration->getValue());
  
  if (declaration->getValue().empty() ||
      declaration->getValue().front().type != Token::COLON) {
    return NULL;
  }
    
  declaration->getValue().pop_front();
  
  getProperty(property);
  
  declaration->setProperty(property.toString());
  
  return true;
}

