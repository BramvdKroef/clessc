#ifndef __less_css_SourceMapWriter_h__
#define __less_css_SourceMapWriter_h__

#include <list>
#include <iostream>
#include <cstdlib>
#include <cstring>

#include <less/Token.h>

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
  
#endif // __less_css_SourceMapWriter_h__
