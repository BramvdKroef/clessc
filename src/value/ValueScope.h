
#ifndef __ValueScope_h__
#define __ValueScope_h__

#include <map>
#include <string>
#include <list>

#include "../TokenList.h"

class ValueScope {
public:
  virtual const TokenList* getVariable(const std::string &key) const =0;
  
};

#endif
