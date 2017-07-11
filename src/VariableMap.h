
#ifndef __VariableMap_h__
#define __VariableMap_h__

#include "TokenList.h"
#include <map>

class VariableMap: public std::map<std::string, TokenList> {
  
public:
  const TokenList* getVariable(const std::string &key) const ;
  void merge(const VariableMap &map);

  void overwrite(const VariableMap &map);
    
  std::string toString() const;
};

#endif
