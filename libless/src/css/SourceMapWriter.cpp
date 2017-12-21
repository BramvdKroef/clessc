#include "less/css/SourceMapWriter.h"

const char* SourceMapWriter::base64 =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

SourceMapWriter::SourceMapWriter(std::ostream& sourcemap,
                                 std::list<const char*>& sources,
                                 const char* out_filename,
                                 const char* rootpath,
                                 const char* basepath)
    : sourcemap_h(sourcemap), sources(sources) {
  lastDstColumn = 0;
  lastSrcFile = 0;
  lastSrcLine = 0;
  lastSrcColumn = 0;
  writePreamble(out_filename, rootpath, basepath);
}

SourceMapWriter::~SourceMapWriter() {
}

void SourceMapWriter::writePreamble(const char* out_filename,
                                    const char* rootpath,
                                    const char* basepath) {
  std::list<const char*>::iterator it;
  const char* source;
  size_t bp_l = 0;

  if (basepath != NULL)
    bp_l = strlen(basepath);

  sourcemap_h << "{";

  sourcemap_h << "\"version\" : 3,";

  if (rootpath != NULL) {
    sourcemap_h << "\"sourceRoot\": \"" << rootpath << "\",";
  }

  sourcemap_h << "\"file\": ";

  if (basepath != NULL && strncmp(out_filename, basepath, bp_l) == 0) {
    out_filename += bp_l;
  }

  sourcemap_h << "\"" << out_filename << "\",";

  sourcemap_h << "\"sources\": [";

  for (it = sources.begin(); it != sources.end(); it++) {
    if (it != sources.begin())
      sourcemap_h << ",";
    source = *it;

    if (basepath != NULL && strncmp(source, basepath, bp_l) == 0) {
      source += bp_l;
    }
    sourcemap_h << "\"" << source << "\"";
  }

  sourcemap_h << "],";

  sourcemap_h << "\"names\": [],";
  sourcemap_h << "\"mappings\": \"";
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
                                      const Token& source,
                                      char* buffer) {
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
