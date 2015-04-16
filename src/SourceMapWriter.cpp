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

#include "SourceMapWriter.h"

const char* SourceMapWriter::base64 = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

SourceMapWriter::SourceMapWriter(std::ostream &sourcemap,
                                 std::list<const char*> &sources,
                                 const char* out_filename):
  sourcemap_h(sourcemap), sources(sources) {
  lastDstColumn = 0;
  lastSrcFile = 0;
  lastSrcLine = 0;
  lastSrcColumn = 0;
  writePreamble(out_filename);
}

SourceMapWriter::~SourceMapWriter() {
  
}


void SourceMapWriter::writePreamble(const char* out_filename) {
  std::list<const char*>::iterator it;
  
  sourcemap_h << "{";

  sourcemap_h << "version : 3,";
  sourcemap_h << "file: ";

  sourcemap_h << "\"" << out_filename << "\",";
  
  sourcemap_h << "sources: [";

  for (it = sources.begin(); it != sources.end(); it++) {
    if (it != sources.begin())
      sourcemap_h << ",";
    sourcemap_h << "\"" << *it << "\"";
  }

  sourcemap_h << "],";

  sourcemap_h << "names: [bla],";
  sourcemap_h << "mappings: \"";
}

void SourceMapWriter::close() {
  sourcemap_h << "\"}" << std::endl;
}


void SourceMapWriter::writeMapping(unsigned int column, const Token& source) {
  char buffer[10];
  size_t len = encodeMapping(column, source, buffer);

  sourcemap_h.write(buffer, len);
  sourcemap_h.write(",", 1);
}

void SourceMapWriter::writeNewline() {
  sourcemap_h.write(";", 1);
  lastDstColumn = 0;
}

size_t SourceMapWriter::sourceFileIndex(const char* file) {
  std::list<const char*>::iterator i;
  size_t pos = 0;
  
  for (i = sources.begin(); i != sources.end(); i++, pos++) {
    if (*i == file)
      return pos;
  }
  return 0;
}

size_t SourceMapWriter::encodeMapping(unsigned int column,
                                      const Token &source, char* buffer) {
  unsigned int srcFileIndex = sourceFileIndex(source.source);
  char* start = buffer;
  
  buffer += encodeField(column - lastDstColumn, buffer);
  buffer += encodeField(srcFileIndex - lastSrcFile, buffer);
  buffer += encodeField(source.line - lastSrcLine, buffer);
  buffer += encodeField(source.column - lastSrcColumn, buffer);

  lastDstColumn = column;
  lastSrcFile = srcFileIndex;
  lastSrcLine = source.line;
  lastSrcColumn = source.column;

  return buffer - start;
}

size_t SourceMapWriter::encodeField(int field, char* buffer) {
  // convert to unsigned to get the binary right
  unsigned int value = abs(field);
  unsigned int current = (value & 0xF) << 1;
  size_t pos = 0;

  if (field < 0) {
    current |= 1;
  }
  
  value = value >> 4;
  
  while (value != 0) {
    buffer[pos] = base64[current | (1 << 5)];
    current = value & 0x1F;

    pos++;
    value = value >> 5;
  }
  
  buffer[pos] = base64[current];
  return pos + 1;
}
