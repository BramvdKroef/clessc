#include "less/css/ParseException.h"

ParseException::ParseException(const std::string found,
                               const std::string& expected,
                               unsigned int line,
                               unsigned int column,
                               const std::string source) :
  LessException(line, column, source) {
  
  err.append("Found \"");
  err.append(translate(found));
  err.append("\" when expecting ");
  err.append(expected);
}

ParseException::ParseException(const std::string found,
                               const char* expected,
                               unsigned int line,
                               unsigned int column,
                               const std::string source) :
  LessException(line, column, source) {
  err.append("Found \"");
  err.append(translate(found));
  err.append("\" when expecting ");
  err.append(expected);
}
ParseException::ParseException(const char* found,
                               const char* expected,
                               unsigned int line,
                               unsigned int column,
                               const std::string source) :
  LessException(line, column, source) {
  err.append("Found \"");
  if (found[0] == -1)
    err.append("end of file");
  else
    err.append(translate(std::string(found)));
  err.append("\" when expecting ");
  err.append(expected);
}
ParseException::ParseException(const Token& found, const char* expected) :
  LessException(found) {
  err.append("Found \"");
  err.append(translate(found));
  err.append("\" when expecting ");
  err.append(expected);
}

ParseException::ParseException(const TokenList& found, const char* expected) :
  LessException(found.front()) {
  err.append("Found \"");
  err.append(translate(found.toString()));
  err.append("\" when expecting ");
  err.append(expected);
}


const char* ParseException::what() const throw() {
  return err.c_str();
}

std::string ParseException::translate(std::string found) {
  if (found == "\n")
    return "newline";
  return found;
}
