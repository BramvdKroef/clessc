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

#ifndef __TokenList_h__
#define __TokenList_h__

#include "Token.h"
#include <list>
using namespace std;

class TokenListIterator {
private:
  list<Token*>::iterator it, begin, end;
  
public:
  TokenListIterator(list<Token*>::iterator begin,
                    list<Token*>::iterator end);
  void toEnd ();
  Token* previous ();
  Token* next ();
  Token* peek ();
  Token* current ();
  bool hasNext ();
  bool hasPrevious ();
};

class TokenList {
private:
  list<Token*> tokens;
  
public:
  virtual ~TokenList();
  
  void push (Token* token);
  Token* pop ();

  void unshift (Token* token);
  Token* shift ();

  bool empty ();
  size_t size();

  bool equals(TokenList* list);

  Token* back();
  Token* front();
  Token* at(unsigned int i);

  TokenListIterator* iterator();
  TokenListIterator* reverseIterator ();

  void push(TokenList* list);
  void unshift(TokenList* list);

  /**
   * Delete all tokens in the list and remove them from the list.
   */
  void clear();

  virtual TokenList* clone();
  
  string* toString();
};

#endif
