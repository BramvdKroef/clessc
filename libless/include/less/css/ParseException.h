#ifndef __less_css_ParseException_h__
#define __less_css_ParseException_h__

#include <exception>
#include <string>

#include "less/Token.h"
#include "less/TokenList.h"
#include "less/LessException.h"

class ParseException : public LessException {
public:
  std::string err;

  ParseException(const std::string found,
                 const std::string& expected,
                 unsigned int line,
                 unsigned int column,
                 const std::string source);
  ParseException(const std::string found,
                 const char* expected,
                 unsigned int line,
                 unsigned int column,
                 const std::string source);
  ParseException(const char* found,
                 const char* expected,
                 unsigned int line,
                 unsigned int column,
                 const std::string source);

  ParseException(const Token& found, const char* expected);
  ParseException(const TokenList& found, const char* expected);

  ~ParseException() throw(){};

  virtual const char* what() const throw();

protected:
  std::string translate(std::string found);
};

#endif  // __less_css_ParseException_h__
