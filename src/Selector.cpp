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

#ifdef WITH_LIBGLOG
#include <glog/logging.h>
#endif

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
  TokenList::const_iterator first, last;
  Selector current;
    
  for (first = begin(); first != end(); ) {
    last = findComma(first);

    current.assign(first, last);
#ifdef WITH_LIBGLOG
    VLOG(3) << "Split: " << current.toString(); 
#endif
    l.push_back(current);
    
    first = last;
    if (first != end())
      first++;
  }
}

TokenList::const_iterator Selector::findComma(const_iterator offset)
  const {
  return findComma(offset, end());
}
TokenList::const_iterator Selector::findComma(const_iterator offset,
                                              const_iterator limit) const {
  unsigned int parentheses = 0;

  for (; offset != limit; offset++) {
    if (parentheses == 0 &&
        (*offset).type == Token::OTHER &&
        *offset == ",") {
      return offset;
  
    } else {
      if (*offset == "(")
        parentheses++;
      else if (*offset == ")")
        parentheses--;
    }
  }
  return offset;
}

bool Selector::match(const Selector &list) const {
  TokenList::const_iterator first, last;
  TokenList::const_iterator l_first, l_last;

  for (first = begin(); first != end(); ) {
    last = findComma(first);

    for (l_first = list.begin(); l_first != list.end(); ) {
      l_last = list.findComma(l_first);
      
      if (walk(l_first, l_last, first) == last)
        return true;

      l_first = l_last;
      if (l_first != list.end()) {
        l_first++;
        while (l_first != list.end() && (*l_first).type == Token::WHITESPACE)
          l_first++;
      }
    }

    first = last;
    if (first != end()) {
      first++;
      while (first != end() && (*first).type == Token::WHITESPACE)
        first++;
    }
  }
  return false;
}

TokenList::const_iterator Selector::walk(const Selector &list,
                                         const_iterator offset) const {
  TokenList::const_iterator first, last, pos;

  for (first = list.begin(); first != list.end(); ) {
    last = list.findComma(first);

    pos = walk(first, last, offset);

    if (pos != begin())
      return pos;
      
    first = last;
    if (first != list.end()) {
      first++;
      while (first != list.end() && (*first).type == Token::WHITESPACE)
        first++;
    }
  }
  return begin();
}

TokenList::const_iterator Selector::walk(const const_iterator &list_begin,
                                         const const_iterator &list_end,
                                         const_iterator offset) const {
  TokenList::const_iterator li = list_begin;

  while (offset != end() && li != list_end) {
    if (*offset != *li)
      return begin();
    
    offset++;
    li++;
    
    if (offset != end() && *offset == ">") {
      offset++;
      if (offset != end() && (*offset).type == Token::WHITESPACE)
        offset++;
    }
    if (li != list_end && *li == ">") {
      li++;
      if (li != list_end && (*li).type == Token::WHITESPACE) 
        li++;
    }
  }
  
  if (li != list_end)
    offset = begin();
  
  return offset;
}

TokenList::const_iterator Selector::find(const TokenList &list,
                                         TokenList::const_iterator offset,
                                         TokenList::const_iterator limit) const {
  for (; offset != limit; offset++) {
    if (walk(list.begin(), list.end(), offset) != begin())
      return offset;
  }
  return limit;
}
