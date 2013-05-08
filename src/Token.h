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

#ifndef __Token_h__
#define __Token_h__

#include <string>
using namespace std;

class Token {
public:
  enum Type{IDENTIFIER, ATKEYWORD, STRING, HASH, NUMBER, PERCENTAGE,
            DIMENSION, URL, UNICODE_RANGE, COLON, DELIMITER, BRACKET_OPEN,
            BRACKET_CLOSED, PAREN_OPEN, PAREN_CLOSED, BRACE_OPEN,
            BRACE_CLOSED, WHITESPACE, COMMENT, INCLUDES,
            DASHMATCH, OTHER, EOS} type; 
  string str;

  Token ();
  Token (string s, Type t);
  
  void add(char c);
  
  void clear ();
  bool equals (Token* t);
  Token* clone ();
};

#endif
