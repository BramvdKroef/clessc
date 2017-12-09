
#ifndef __ValueException_h__
#define __ValueException_h__

#include "TokenList.h"

using namespace std;
/**
 * 
 */
class ValueException: public exception {
public:
  string err;
  const char* _source;
  unsigned int line, column;
  
  ValueException(string message, const TokenList &source) {
    err = message;
    this->_source = source.front().source;
    line = source.front().line;
    column = source.front().column;
  }
  virtual ~ValueException() throw () {};

  virtual const char* what() const throw(){
    return err.c_str();
  }

  unsigned int getLineNumber(){
    return line;
  }
  unsigned int getColumn(){
    return column;
  }
  const char* getSource() {
    return _source;
  }

};

#endif
