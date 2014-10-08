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

#ifndef __ParseException_h__
#define __ParseException_h__

#include <exception>
#include <string>

using namespace std;

class ParseException: public exception{
public:
  string err;

  string source;
  unsigned int line, column;
  
  ParseException(string found, string& expected,
                 unsigned int line, unsigned int column,
                 string source);
  ParseException(string found, const char* expected,
                 unsigned int line, unsigned int column,
                 string source);
  ParseException(const char* found, const char* expected,
                 unsigned int line, unsigned int column,
                 string source);

  ~ParseException() throw () {};

  void setLocation(unsigned int line, unsigned int column);
  unsigned int getLineNumber();
  unsigned int getColumn();

  /**
   * URL or file name where the Less code is located.
   */
  void setSource(string source);

  string getSource(); 
  virtual const char* what() const throw(); 

protected:
  string translate(string found); 

};

#endif
