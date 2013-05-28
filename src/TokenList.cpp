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

TokenListIterator::TokenListIterator(list<Token*>::iterator begin,
                  list<Token*>::iterator end) {
  it = begin;
  this->begin = begin;
  this->end = --end;
  it--;
}
void TokenListIterator::toEnd () {
  it = end;
  it++;
}
Token* TokenListIterator::previous () {
  return *--it;
}
Token* TokenListIterator::next () {
  return *++it;
}
Token* TokenListIterator::peek () {
  Token* ret;
  it++;
  ret = *it;
  it--;
  return ret;
}
Token* TokenListIterator::current () {
  return *it;
}

bool TokenListIterator::hasNext () {
  return (it != end);
}
bool TokenListIterator::hasPrevious () {
  return (it != begin);
}

TokenList::~TokenList() {
  clear();
}
  
void TokenList::push (Token* token) {
  tokens.push_back(token);
}
Token* TokenList::pop () {
  Token* token = tokens.back();
  tokens.pop_back();
  return token;
}

void TokenList::unshift (Token* token) {
  tokens.push_front(token);
}
  
Token* TokenList::shift () {
  Token* token = tokens.front();
  tokens.pop_front();
  return token;
}

bool TokenList::empty () {
  return tokens.empty();
}

size_t TokenList::size() {
  return tokens.size();
}

bool TokenList::equals(TokenList* list) {
  TokenListIterator* it1, * it2;
  if (list->size() != size())
    return false;
    
  it1 = iterator();
  it2 = list->iterator();
    
  while (it1->hasNext() && it2->hasNext()) {
    if (!it1->next()->equals(it2->next())) {
      delete it1;
      delete it2;
      return false;
    }
  }
  return true;
}

Token* TokenList::back() {
  return tokens.back();
}

Token* TokenList::front() {
  return tokens.front();
}
Token* TokenList::at(unsigned int i) {
  list<Token*>::iterator it;
  
  if (i > tokens.size())
    return NULL;

  it = tokens.begin();
  for (unsigned int j = 0; j < i; j++)
    it++;
  return *it;
}

TokenListIterator* TokenList::iterator() {
  return new TokenListIterator(tokens.begin(), tokens.end());
}
TokenListIterator* TokenList::reverseIterator () {
  TokenListIterator* it =
    new TokenListIterator(tokens.begin(), tokens.end());
  it->toEnd();
  return it;
}

void TokenList::push(TokenList* list) {
  TokenListIterator* it = list->iterator();
  while (it->hasNext())
    push(it->next()->clone());
  delete it;
}
void TokenList::unshift(TokenList* list) {
  TokenListIterator* it = list->reverseIterator();
  while (it->hasPrevious())
    unshift(it->previous()->clone());
  delete it;
}

void TokenList::clear() {
  while (!empty()) 
    delete pop();
}

TokenList* TokenList::clone() {
  TokenList* newtokens = new TokenList();
  list<Token*>::iterator it;
    
  for (it = tokens.begin(); it != tokens.end(); it++) {
    newtokens->push((*it)->clone());
  }
  return newtokens;
}
  
string* TokenList::toString() {
  string* str = new string();
  list<Token*>::iterator it;
    
  for (it = tokens.begin(); it != tokens.end(); it++) {
    str->append((*it)->str);
  }
  return str;
}
