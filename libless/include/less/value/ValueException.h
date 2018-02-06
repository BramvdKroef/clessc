#ifndef __less_value_ValueException_h__
#define __less_value_ValueException_h__

#include <string>
#include "less/TokenList.h"
#include "less/LessException.h"

/**
 *
 */
class ValueException : public LessException {
public:
  std::string err;

  ValueException(std::string message, const TokenList& source) :
    LessException(source.front()) {
    err = message;
  }
  virtual ~ValueException() throw(){};

  virtual const char* what() const throw() {
    return err.c_str();
  }

};

#endif  // __less_value_ValueException_h__
