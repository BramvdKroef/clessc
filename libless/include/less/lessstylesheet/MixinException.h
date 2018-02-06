#ifndef __less_css_MixinException_h__
#define __less_css_MixinException_h__

#include <exception>
#include <string>
#include "less/lessstylesheet/Mixin.h"
#include "less/LessException.h"

class MixinException : public LessException {
  std::string err;
  
public:
  const Mixin * mixin;
  
  MixinException(const Mixin &mixin);
  ~MixinException() throw(){};


  virtual const char* what() const throw();

};

#endif  // __less_css_MixinException_h__
