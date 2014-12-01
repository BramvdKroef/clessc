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

void Selector::addPrefix(const Selector &prefix) {
  list<Selector> prefixParts;
  list<Selector> sepParts;
  list<Selector>::iterator prefixIt;
  list<Selector>::iterator sepIt;

  Selector* tmp, *prefixPart;
  TokenList::iterator i;
  bool containsAmp;

  Token space(" ", Token::WHITESPACE),
    comma(",", Token::OTHER);

  split(sepParts);
  prefix.split(prefixParts);
  
  clear();

  for (sepIt = sepParts.begin();
       sepIt != sepParts.end(); sepIt++) {

    tmp = &(*sepIt);
    tmp->ltrim();
    containsAmp = tmp->contains(Token::OTHER, "&");

    for (prefixIt = prefixParts.begin();
         prefixIt != prefixParts.end(); prefixIt++) {

      prefixPart = &(*prefixIt);
      
      if (containsAmp) {
        
        for (i = tmp->begin(); i != tmp->end(); i++) {
          if (*i == "&")
            insert(end(), prefixPart->begin(), prefixPart->end());
          else
            push_back(*i);
        }

      } else {
        insert(end(), prefixPart->begin(), prefixPart->end());
        push_back(space);
        insert(end(), tmp->begin(), tmp->end());
      }
      push_back(comma);
    }
  }
  pop_back();
}

void Selector::split(std::list<Selector> &l) const {
  TokenList::const_iterator it = begin();
  Selector current;
  const Token* t;
  unsigned int parentheses = 0;
  
  for (it = begin(); it != end(); it++) {
    t = &(*it);

    if (parentheses == 0 &&
        t->type == Token::OTHER &&
        *t == ",") {
      l.push_back(current);
      current.clear();
      
    } else {
      if (*t == "(")
        parentheses++;
      else if (*t == ")")
        parentheses--;
      current.push_back(*t);
    }
  }
  
  l.push_back(current);
}

bool Selector::match(const TokenList &list) const {
  return (walk(list, begin()) == end());
}

TokenList::const_iterator Selector::walk(const TokenList &list,
                                         const_iterator offset) const {
  TokenList::const_iterator li;
  li = list.begin();

  while (offset != end() && li != list.end()) {
    if (*offset != *li)
      return begin();
    
    offset++;
    li++;
    
    if (offset != end() && *offset == ">") {
      offset++;
      if (offset != end() && (*offset).type == Token::WHITESPACE)
        offset++;
    }
    if (li != list.end() && *li == ">") {
      li++;
      if (li != list.end() && (*li).type == Token::WHITESPACE) 
        li++;
    }
  }
  
  if (li != list.end())
    offset = begin();
  
  return offset;
}

TokenList::const_iterator Selector::find(const TokenList &list,
                                         const_iterator offset) const {
  for (; offset != end(); offset++) {
    if (walk(list, offset) != begin())
      return offset;
  }
  return begin();
}
