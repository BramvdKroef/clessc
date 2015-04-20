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

#ifndef __SourceMapWriter_h__
#define __SourceMapWriter_h__

#include "Token.h"

#include <list>
#include <iostream>
#include <stdlib.h>
#include <string.h>

class SourceMapWriter {
private:
  std::ostream &sourcemap_h;
  std::list<const char*> &sources;

  unsigned int lastDstColumn;
  unsigned int lastSrcFile, lastSrcLine, lastSrcColumn;

  size_t sourceFileIndex(const char* file) ;
  size_t encodeMapping(unsigned int column,
                       const Token &source, char* buffer);
  size_t encodeField(int field, char* buffer);

  void writePreamble(const char* out_filename,
                     const char* rootpath = NULL,
                     const char* basepath = NULL);
public:
  static const char* base64;
  
  SourceMapWriter(std::ostream &sourcemap,
                  std::list<const char*> &sources,
                  const char* out_filename,
                  const char* rootpath = NULL,
                  const char* basepath = NULL);
  virtual ~SourceMapWriter();

  void writeMapping(unsigned int column, const Token& source) ;
  void writeNewline();

  void close();
};
  
#endif
