
#ifndef __SourceMapWriter_h__
#define __SourceMapWriter_h__

#include "../Token.h"

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
