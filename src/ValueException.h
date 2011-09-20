#ifndef __ValueException_h__
#define __ValueException_h__

/**
 * 
 */
class ValueException: public exception {
public:
  string err;
  
  ValueException(string message) {
    err = message;
  }
  virtual ~ValueException() throw () {};

  virtual const char* what() const throw(){
    return err.c_str();
  }
};

#endif
