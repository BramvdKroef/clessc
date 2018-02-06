#ifndef __less_css_ParseException_h__
#define __less_css_ParseException_h__

#include <exception>
#include <string>

#include "less/Token.h"
#include "less/TokenList.h"
#include "less/LessException.h"

using namespace std;

class ParseException : public LessException {
public:
  string err;

  ParseException(string found,
                 string& expected,
                 unsigned int line,
                 unsigned int column,
                 string source);
  ParseException(string found,
                 const char* expected,
                 unsigned int line,
                 unsigned int column,
                 string source);
  ParseException(const char* found,
                 const char* expected,
                 unsigned int line,
                 unsigned int column,
                 string source);

  ParseException(Token& found, const char* expected);
  ParseException(TokenList& found, const char* expected);

  ~ParseException() throw(){};

  virtual const char* what() const throw();

protected:
  string translate(string found);
};

#endif  // __less_css_ParseException_h__
