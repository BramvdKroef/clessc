#include "less/css/ParseException.h"

ParseException::ParseException(string found,
                               string& expected,
                               unsigned int line,
                               unsigned int column,
                               string source) :
  LessException(line, column, source) {
  
  err.append("Found \"");
  err.append(translate(found));
  err.append("\" when expecting ");
  err.append(expected);
  setLocation(line, column);
  this->source = source;
}

ParseException::ParseException(string found,
                               const char* expected,
                               unsigned int line,
                               unsigned int column,
                               string source) :
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
                               string source) :
  LessException(line, column, source) {
  err.append("Found \"");
  if (found[0] == -1)
    err.append("end of file");
  else
    err.append(translate(string(found)));
  err.append("\" when expecting ");
  err.append(expected);
}
ParseException::ParseException(Token& found, const char* expected) :
  LessException(found) {
  err.append("Found \"");
  err.append(translate(found));
  err.append("\" when expecting ");
  err.append(expected);
}

ParseException::ParseException(TokenList& found, const char* expected) :
  LessException(found.front()) {
  err.append("Found \"");
  err.append(translate(found.toString()));
  err.append("\" when expecting ");
  err.append(expected);
}


const char* ParseException::what() const throw() {
  return err.c_str();
}

string ParseException::translate(string found) {
  if (found == "\n")
    return "newline";
  return found;
}
