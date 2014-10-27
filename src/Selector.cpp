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
  list<Selector*>* prefixParts = prefix->split();
  list<Selector*>* sepParts = split();
  list<Selector*>::iterator prefixIt;
  list<Selector*>::iterator sepIt;

  Selector* tmp, *prefixPart;
  TokenListIterator* i;
  bool containsAmp;
  
  clear();

  for (sepIt = sepParts->begin(); sepIt !=
         sepParts->end(); sepIt++) {

    tmp = *sepIt;
    tmp->ltrim();
    containsAmp = tmp->contains(Token::OTHER, "&");

    for (prefixIt = prefixParts->begin(); prefixIt !=
           prefixParts->end(); prefixIt++) {

      prefixPart = *prefixIt;
      
      if (!tmp->empty() && containsAmp) {
        
        for (i = tmp->iterator();
             i->hasNext();) {
          
          if (i->next()->str == "&") {
            push(prefixPart);
          } else
            push(i->current()->clone());
        }
        delete i;
      } else {
        push(prefixPart);
        push(new Token(" ", Token::WHITESPACE));
        push(tmp);
      }
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

list<Selector*>* Selector::split() {
  list<Selector*>* l = new list<Selector*>();
  TokenListIterator* it = iterator();
  Selector* current = new Selector();
  Token* t;
  unsigned int parentheses = 0;
  
  l->push_back(current);

  while (it->hasNext()) {
    t = it->next();
    
    if (parentheses == 0 &&
        t->type == Token::OTHER && t->str == ",") {
      current = new Selector();
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
  return (walk(list, 0) == size());
}

size_t Selector::walk(TokenList* list, size_t offset = 0) {
  size_t i = 0;
  
  while (offset < size() && i < list->size()) {
    if (!at(offset)->equals(list->at(i))) 
      return 0;
    
    offset++;
    
    if (offset < size() && at(offset)->str == ">") {
      offset++;
      if (offset < size() && at(offset)->type == Token::WHITESPACE)
        offset++;
    }
    if (i < list->size() && it->peek()->str == ">") {
      it->next();
      if (it->hasNext() && it->peek()->type == Token::WHITESPACE) 
        it->next();
    }
  }
  
  if (it->hasNext())
    offset = Selector::npos;
  delete it;
  return offset;
}

size_t Selector::find(TokenList* list, size_t offset = 0) {
  for (; offset < size(); offset++) {
    if (walk(list, offset) != Selector::npos)
      return offset;
  }
  return Selector::npos;
}
