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


StringValue::StringValue(Token* token, bool quotes) {
  this->tokens.push(token);
  this->quotes = quotes;
  stringvalue = tokens.front()->str;
  type = Value::STRING;
}

StringValue::~StringValue() {
}


TokenList* StringValue::getTokens() {
  string::iterator i;
  string newstr;

  if (quotes) {
    // add quotes
    newstr.push_back('"');
    for (i = stringvalue.begin(); i != stringvalue.end(); i++) {
      if (*i == '"' || *i == '\\') 
        newstr.push_back('\\');
      newstr.push_back(*i);
    }
    newstr.push_back('"');
    tokens.front()->str = newstr;
  } else
    tokens.front()->str = stringvalue;
  return &tokens;
}

void StringValue::setQuotes(bool quotes) {
  this->quotes = quotes;
}
bool StringValue::getQuotes() {
  return quotes;
}

Value* StringValue::add(Value* v) {
  bool v_quotes;
  string* newstr;
  
  if (v->type == Value::STRING) {
    v_quotes = ((StringValue*)v)->getQuotes();
    ((StringValue*)v)->setQuotes(false);

    newstr = v->getTokens()->toString();
    ((StringValue*)v)->setQuotes(v_quotes);
  } else
    newstr = v->getTokens()->toString();
  
  stringvalue.append(*newstr);
  delete newstr;
  return this;
}

Value* StringValue::substract(Value* v) {
  throw new ValueException("Can't substract from strings.");
}
Value* StringValue::multiply(Value* v) {
  string newstr;
  double i;
  
  if (v->type != Value::NUMBER) {
    throw new ValueException("Strings can only be multiplied by a number.");
  }

  for (i = 0; i < ((NumberValue*)v)->getValue(); i++) {
    newstr.append(stringvalue);
  }
  stringvalue = newstr;
  return this;
}

Value* StringValue::divide(Value* v) {
  throw new ValueException("Can't divide strings.");
}

void StringValue::escape() {
  
}

void StringValue::e() {
  quotes = false;
}

void StringValue::format(vector<Value*> args) {
}
  
Value* StringValue::color() {
  return new Color(tokens.front());
}

void StringValue::data_uri() {
  
}

