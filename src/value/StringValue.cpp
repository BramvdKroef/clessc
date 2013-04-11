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

#include "StringValue.h"


StringValue::StringValue(Token* token): Value() {
  this->tokens.push(token);
  this->quotes = true;
  type = Value::STRING;
}

StringValue::StringValue(Token* token, bool quotes) {
  this->tokens.push(token);
  this->quotes = quotes;
  type = Value::STRING;
}

StringValue::~StringValue() {
}


TokenList* StringValue::getTokens() {
  return &tokens;
}

void StringValue::setQuotes(bool quotes) {
  this->quotes = quotes;
}
bool StringValue::getQuotes() {
  return quotes;
}

void StringValue::add(Value* v) {
  bool v_quotes;
  string* str;
  
  if (v->type == Value::STRING) {
    v_quotes = ((StringValue*)v)->getQuotes();
    ((StringValue*)v)->setQuotes(false);

    str = v->getTokens()->toString();
    ((StringValue*)v)->setQuotes(v_quotes);
  } else
    str = v->getTokens()->toString();
  
  tokens.front()->str.append(*str);
  delete str;
}

void StringValue::substract(Value* v) {
  throw new ValueException("Can't substract from strings.");
}
void StringValue::multiply(Value* v) {
  string str;
  double i;
  
  if (v->type != Value::NUMBER) {
    throw new ValueException("Strings can only be multiplied by a number.");
  }

  str = tokens.front()->str;
  for (i = 0; i < ((NumberValue*)v)->getValue(); i++) {
    tokens.front()->str.append(str);
  }
}

void StringValue::divide(Value* v) {
  throw new ValueException("Can't divide strings.");
}

void StringValue::escape() {
  
}

void StringValue::e() {
  quotes = false;
}

void StringValue::format(vector<Value*> args) {
}
  
Color* StringValue::color() {
  return new Color(tokens.front());
}

void StringValue::data_uri() {
  
}

