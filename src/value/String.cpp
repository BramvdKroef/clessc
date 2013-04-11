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

#include "String.h"


String:String(Token* token) {
  String(token, true);
}

String:String(Token* token, bool quotes) {
  this->tokens.push(token);
  this->quotes = quotes;
  type = Value::STRING;
}

String::~String() {
}


TokenList* String::getTokens() {
  return &tokens;
}

void String::setQuotes(bool quotes) {
  this->quotes = quotes;
}
bool String::getQuotes() {
  return quotes;
}

void add(Value* v) {
  bool v_quotes;
  string* str;
  
  if (v->type == Value::STRING) {
    v_quotes = ((String)v)->getQuotes();
    ((String)v)->setQuotes(false);

    str = v->getTokens()->toString();
    ((String)v)->setQuotes(v_quotes);
  } else
    str = *v->getTokens()->toString();
  
  tokens.front()->str.append(*str);
  delete str;
}

void substract(Value* v) {
  throw new ValueException("Can't substract from strings.");
}
void multiply(Value* v) {
  string str;
  double i;
  
  if (v->type != Value::NUMBER) {
    throw new ValueException("Strings can only be multiplied by a number.");
  }

  str = tokens->front()->str;
  for (i = 0; i < v->getValue(); i++) {
    tokens->front()->str.append(str);
  }
}

void divide(Value* v) {
  throw new ValueException("Can't divide strings.");
}

void escape() {
  
}

void e() {
  quotes = false;
}

void format(vector<Value*> args) {
  
}
  
Color* color() {
  return new Color(tokens->front());
}

void data_uri() {
  
}

