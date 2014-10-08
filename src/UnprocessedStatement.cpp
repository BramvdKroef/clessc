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
#include "ParameterMixin.h"

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

void UnprocessedStatement::getProperty(TokenList* tokens) {
  for(size_t i=0; i < property_i; i++) {
    tokens->push(this->tokens.at(i)->clone());
  }
}

void UnprocessedStatement::getValue(TokenList* tokens) {
  for(size_t i=property_i; i < this->tokens.size(); i++) {
    tokens->push(this->tokens.at(i)->clone());
  }
}

void UnprocessedStatement::setRuleset(LessRuleset* r) {
  DLOG(INFO) << "Set LessRuleset";
  RulesetStatement::setRuleset(r);
  lessRuleset = r;
}

LessRuleset* UnprocessedStatement::getLessRuleset() {
  return lessRuleset;
}

void UnprocessedStatement::insert(Stylesheet* s) {
  ParameterMixin mixin;
  mixin.setStylesheet(getLessRuleset()->getLessStylesheet());
  
  // process mixin
  if (mixin.parse(getTokens()))
    mixin.insert(s, NULL, getLessRuleset());
}

void UnprocessedStatement::process(Ruleset* r) {
  ParameterMixin mixin;
  Declaration declaration;

  DLOG(INFO) << "Statement: " << *getTokens()->toString();

  mixin.setStylesheet(getLessRuleset()->getLessStylesheet());
  
  // process mixin
  if (mixin.parse(getTokens()) &&
      mixin.insert(r->getStylesheet(), r, getLessRuleset())) {

  } else if (processDeclaration(&declaration)) {
    DLOG(INFO) << "Declaration: " <<
      *declaration.getProperty() << ": " << *declaration.getValue()->toString();
    getLessRuleset()->getLessStylesheet()->
      getValueProcessor()->processValue(declaration.getValue());

    r->addStatement(declaration.clone());

    DLOG(INFO) << "Processed declaration: " <<
      *declaration.getProperty() << ": " << *declaration.getValue()->toString();

  } else {
    throw new ParseException(*getTokens()->toString(),
                             "variable, mixin or declaration.",
                             line, column, source);
  }

  DLOG(INFO) << "Statement done";
}

bool UnprocessedStatement::processDeclaration (Declaration* declaration) {
  TokenList property;
  TokenList* value = new TokenList();

  DLOG(INFO) << "Declaration";
  
  getValue(value);
  
  if (value->empty() ||
      value->front()->type != Token::COLON) {
    delete value;
    return NULL;
  }
    
  delete value->shift();
  
  getProperty(&property);
  
  declaration->setProperty(property.toString());
  declaration->setValue(value);
  
  return declaration;
}


UnprocessedStatement* UnprocessedStatement::clone() {
  UnprocessedStatement* statement = new UnprocessedStatement();
  statement->line = line;
  statement->column = column;
  statement->property_i = property_i;
  statement->getTokens()->push(getTokens());
  return statement;
}
