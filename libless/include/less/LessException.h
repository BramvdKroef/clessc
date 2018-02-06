#ifndef __less_LessException_h__
#define __less_LessException_h__

#include <exception>
#include <string>
#include "less/Token.h"

class LessException : public std::exception {
public:
  std::string source;
  unsigned int line, column;

  LessException(unsigned int line,
                unsigned int column,
                std::string source);
  LessException(const Token& token);

  ~LessException() throw(){};

  void setLocation(unsigned int line, unsigned int column);
  unsigned int getLineNumber();
  unsigned int getColumn();

  /**
   * URL or file name where the Less code is located.
   */
  void setSource(std::string source);

  std::string getSource();
  virtual const char* what() const throw() =0;

};

#endif  // __less_css_LessException_h__
