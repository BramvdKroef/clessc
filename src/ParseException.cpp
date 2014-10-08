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

#include "ParseException.h"


ParseException::ParseException(string found, string& expected,
                               unsigned int line, unsigned int column,
                               string source){
  err.append("Found \"");
  err.append(translate(found));
  err.append("\" when expecting ");
  err.append(expected);
  setLocation(line, column);
  this->source = source;
}

ParseException::ParseException(string found, const char* expected,
                               unsigned int line, unsigned int column,
                               string source){
  err.append("Found \"");
  err.append(translate(found));
  err.append("\" when expecting ");
  err.append(expected);
  setLocation(line, column);
  this->source = source;
}
ParseException::ParseException(const char* found, const char* expected,
                               unsigned int line, unsigned int column,
                               string source){
  err.append("Found \"");
  if (found[0] == -1)
    err.append("end of file");
  else
    err.append(translate(string(found)));
  err.append("\" when expecting ");
  err.append(expected);
  setLocation(line, column);
  this->source = source;
}

void ParseException::setLocation(unsigned int line, unsigned int column) {
  this->line = line;
  this->column = column;
}

unsigned int ParseException::getLineNumber(){
  return line;
}
unsigned int ParseException::getColumn(){
  return column;
}

/**
 * URL or file name where the Less code is located.
 */
void ParseException::setSource(string source) {
  this->source = source;
}

string ParseException::getSource() {
  return source;
}
  
const char* ParseException::what() const throw(){
  return err.c_str();
}

string ParseException::translate(string found) {
  if (found == "\n")
    return "newline";
  return found;
}
