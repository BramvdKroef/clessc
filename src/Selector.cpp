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

#include "Selector.h"
#include <iostream>

Selector::~Selector() {
  clear();
}

void Selector::addPrefix(Selector* prefix) {
  list<TokenList*>* prefixParts = prefix->split();
  list<TokenList*>* sepParts = split();
  list<TokenList*>::iterator prefixIt;
  list<TokenList*>::iterator sepIt;
  TokenList* tmp;
  clear();

  for (sepIt = sepParts->begin(); sepIt !=
         sepParts->end(); sepIt++) {

    (*sepIt)->ltrim();

    if (!(*sepIt)->empty()) {
      if ((*sepIt)->front()->str == "&") 
        delete (*sepIt)->shift();
      else 
        (*sepIt)->unshift(new Token(" ", Token::WHITESPACE));
    }
  }

  for (prefixIt = prefixParts->begin(); prefixIt !=
         prefixParts->end(); prefixIt++) {
    
    for (sepIt = sepParts->begin(); sepIt !=
           sepParts->end(); sepIt++) {
      push(*prefixIt);
      push(*sepIt);
      push(new Token(",", Token::OTHER));
    }
  }
  delete pop();
  
  while (prefixParts->size() > 0) {
    tmp = prefixParts->back();
    prefixParts->pop_back();
    delete tmp;
  }
  while (sepParts->size() > 0) {
    tmp = sepParts->back();
    sepParts->pop_back();
    delete tmp;
  }
}

list<TokenList*>* Selector::split() {
  list<TokenList*>* l = new list<TokenList*>();
  TokenListIterator* it = iterator();
  TokenList* current = new TokenList();
  Token* t;
  unsigned int parentheses = 0;
  
  l->push_back(current);

  while (it->hasNext()) {
    t = it->next();
    
    if (parentheses == 0 &&
        t->type == Token::OTHER && t->str == ",") {
      current = new TokenList();
      l->push_back(current);
    } else {
      if (t->str == "(")
        parentheses++;
      else if (t->str == ")")
        parentheses--;
      current->push(t->clone());
    }
  }
  
  return l;
}
Selector* Selector::clone() {
  Selector* newtokens = new Selector();
  TokenListIterator* it = iterator();
    
  while (it->hasNext())
    newtokens->push(it->next()->clone());
  
  return newtokens;
}


bool Selector::equals(TokenList* list) {
  TokenListIterator* it1, * it2;
  bool ret;
  
  it1 = iterator();
  it2 = list->iterator();

  while (it1->hasNext() && it2->hasNext()) {
    if (!it1->next()->equals(it2->next())) {
      delete it1;
      delete it2;
      return false;
    }
    if (it1->hasNext() && it1->peek()->str == ">") {
      it1->next();
      if (it1->hasNext() && it1->peek()->type == Token::WHITESPACE) 
        it1->next();
    }
    if (it2->hasNext() && it2->peek()->str == ">") {
      it2->next();
      if (it2->hasNext() && it2->peek()->type == Token::WHITESPACE) 
        it2->next();
    }
  }
  ret = !(it1->hasNext() || it2->hasNext());
  delete it1;
  delete it2;
  return ret;
}

size_t Selector::walk(TokenList* list, size_t offset) {
  TokenListIterator* it = list->iterator();
  
  while (offset < size() && it->hasNext()) {
    if (!at(offset)->equals(it->next())) {
      delete it;
      return 0;
    }
    offset++;
    
    if (offset < size() && at(offset)->str == ">") {
      offset++;
      if (offset < size() && at(offset)->type == Token::WHITESPACE)
        offset++;
    }
    if (it->hasNext() && it->peek()->str == ">") {
      it->next();
      if (it->hasNext() && it->peek()->type == Token::WHITESPACE) 
        it->next();
    }
  }
  
  if (it->hasNext())
    offset = 0;
  delete it;
  return offset;
}
