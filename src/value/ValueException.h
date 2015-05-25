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

#ifndef __ValueException_h__
#define __ValueException_h__

#include "TokenList.h"

using namespace std;
/**
 * 
 */
class ValueException: public exception {
public:
  string err;
  const char* _source;
  unsigned int line, column;
  
  ValueException(string message, const TokenList &source) {
    err = message;
    this->_source = source.front().source;
    line = source.front().line;
    column = source.front().column;
  }
  virtual ~ValueException() throw () {};

  virtual const char* what() const throw(){
    return err.c_str();
  }

  unsigned int getLineNumber(){
    return line;
  }
  unsigned int getColumn(){
    return column;
  }
  const char* getSource() {
    return _source;
  }

};

#endif
