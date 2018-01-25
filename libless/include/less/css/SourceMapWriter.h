#ifndef __less_css_SourceMapWriter_h__
#define __less_css_SourceMapWriter_h__

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <list>

#include "less/Token.h"

class SourceMapWriter {
private:
  std::ostream& sourcemap_h;
  std::list<const char*>& sources;

  unsigned int lastDstColumn;
  unsigned int lastSrcFile, lastSrcLine, lastSrcColumn;
  bool firstSegment;
  
  size_t sourceFileIndex(const char* file);
  size_t encodeMapping(unsigned int column, const Token& source, char* buffer);
  size_t encodeField(int field, char* buffer);

  void writePreamble(const char* out_filename,
                     std::list<const char*>& sources,
                     const char* rootpath = NULL);

public:
  static const char* base64;

  SourceMapWriter(std::ostream& sourcemap,
                  std::list<const char*>& sources,
                  std::list<const char*>& relative_sources,
                  const char* out_filename,
                  const char* rootpath = NULL);
  virtual ~SourceMapWriter();

  bool writeMapping(unsigned int column, const Token& source);
  void writeNewline();

  void close();
};

#endif  // __less_css_SourceMapWriter_h__
