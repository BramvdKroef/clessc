
#ifndef __ParseException_h__
#define __ParseException_h__

#include <exception>
#include <string>

#include <less/Token.h>
#include <less/TokenList.h>

using namespace std;

class ParseException: public exception{
public:
  string err;

  string source;
  unsigned int line, column;
  
  ParseException(string found, string& expected,
                 unsigned int line, unsigned int column,
                 string source);
  ParseException(string found, const char* expected,
                 unsigned int line, unsigned int column,
                 string source);
  ParseException(const char* found, const char* expected,
                 unsigned int line, unsigned int column,
                 string source);

  ParseException(Token &found, const char* expected);
  ParseException(TokenList &found, const char* expected);
  
  ~ParseException() throw () {};

  void setLocation(unsigned int line, unsigned int column);
  unsigned int getLineNumber();
  unsigned int getColumn();

  /**
   * URL or file name where the Less code is located.
   */
  void setSource(string source);

  string getSource(); 
  virtual const char* what() const throw(); 

protected:
  string translate(string found); 

};

#endif
