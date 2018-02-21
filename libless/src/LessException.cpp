#include "less/LessException.h"

LessException::LessException(unsigned int line,
                             unsigned int column,
                             std::string source) :
  source(source), line(line), column(column) {
}
LessException::LessException(const Token& token) :
  source(token.source), line(token.line), column(token.column) {
}


void LessException::setLocation(unsigned int line, unsigned int column) {
  this->line = line;
  this->column = column;
}

unsigned int LessException::getLineNumber() {
  return line;
}
unsigned int LessException::getColumn() {
  return column;
}

/**
 * URL or file name where the Less code is located.
 */
void LessException::setSource(std::string source) {
  this->source = source;
}

std::string LessException::getSource() {
  return source;
}
