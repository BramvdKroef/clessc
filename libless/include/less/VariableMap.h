#ifndef __less_VariableMap_h__
#define __less_VariableMap_h__

#include <map>
#include "less/TokenList.h"

class VariableMap : public std::map<std::string, TokenList> {
public:
  const TokenList *getVariable(const std::string &key) const;
  void merge(const VariableMap &map);

  void overwrite(const VariableMap &map);

  std::string toString() const;
};

#endif  // __less_VariableMap_h__
