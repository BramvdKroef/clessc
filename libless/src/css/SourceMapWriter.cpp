#include "less/css/SourceMapWriter.h"

const char* SourceMapWriter::base64 =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

SourceMapWriter::SourceMapWriter(std::ostream& sourcemap,
                                 std::list<const char*>& sources,
                                 std::list<const char*>& relative_sources,
                                 const char* out_filename,
                                 const char* rootpath)
    : sourcemap_h(sourcemap), sources(sources) {
  lastDstColumn = 0;
  lastSrcFile = 0;
  lastSrcLine = 0;
  lastSrcColumn = 0;
  firstSegment = true;
  writePreamble(out_filename, relative_sources, rootpath);
}

SourceMapWriter::~SourceMapWriter() {
}

void SourceMapWriter::writePreamble(const char* out_filename,
                                    std::list<const char*>& sources,
                                    const char* rootpath) {
  std::list<const char*>::iterator it;
  const char* source;

  sourcemap_h << "{";

  sourcemap_h << "\"version\" : 3,";

  sourcemap_h << "\"file\": ";

  sourcemap_h << "\"" << out_filename << "\",";

  sourcemap_h << "\"sources\": [";

  for (it = sources.begin(); it != sources.end(); it++) {
    if (it != sources.begin())
      sourcemap_h << ",";
    source = *it;

    sourcemap_h << "\"";
    if (rootpath != NULL) {
      sourcemap_h << rootpath;
    }
    sourcemap_h << source << "\"";
  }

  sourcemap_h << "],";

  sourcemap_h << "\"names\": [],";
  sourcemap_h << "\"mappings\": \"";
}

void SourceMapWriter::close() {
  sourcemap_h << "\"}" << std::endl;
}

bool SourceMapWriter::writeMapping(unsigned int column, const Token& source) {
  char buffer[10];
  size_t len = encodeMapping(column, source, buffer);
  if (len > 0) {
    if (firstSegment)
      firstSegment = false;
    else 
      sourcemap_h.write(",", 1);
    sourcemap_h.write(buffer, len);

    return true;
  } else 
    return false;
}

void SourceMapWriter::writeNewline() {
  sourcemap_h.write(";", 1);
  lastDstColumn = 0;
  firstSegment = true;
}

size_t SourceMapWriter::sourceFileIndex(const char* file) {
  std::list<const char*>::iterator i;
  size_t pos = 0;

  for (i = sources.begin(); i != sources.end(); i++, pos++) {
    if (*i == file)
      return pos;
  }
  return sources.size();
}

size_t SourceMapWriter::encodeMapping(unsigned int column,
                                      const Token& source,
                                      char* buffer) {
  unsigned int srcFileIndex = sourceFileIndex(source.source);
  char* start = buffer;

  if (srcFileIndex == sources.size())
    return 0;

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
