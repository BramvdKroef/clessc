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
	
  ParseException(string found, string& expected){
    err.append("Found \"");
    err.append(translate(found));
    err.append("\" when expecting ");
    err.append(expected);
  }
  ParseException(string found, const char* expected){
    err.append("Found \"");
    err.append(translate(found));
    err.append("\" when expecting ");
    err.append(expected);
  }
  ParseException(const char* found, const char* expected){
    err.append("Found \"");
    if (found[0] == -1)
      err.append("end of file");
    else
      err.append(translate(string(found)));
    err.append("\" when expecting ");
    err.append(expected);
  }

  ~ParseException() throw () {};
  
  virtual const char* what() const throw(){
    return err.c_str();
  }

protected:
  string translate(string found) {
    if (found == "\n")
      return "newline";
    return found;
  }

};

#endif
