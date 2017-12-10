#ifndef __less_css_IOException_h__
#define __less_css_IOException_h__

#include <exception>

using namespace std;

class IOException : public exception {
public:
  const char* err;

  IOException(string& err) {
    this->err = err.c_str();
  }

  IOException(const char* err) {
    this->err = err;
  }

  virtual const char* what() const throw() {
    return err;
  }
};

#endif  // __less_css_IOException_h__
