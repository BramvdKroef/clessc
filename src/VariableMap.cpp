
#include "VariableMap.h"

const TokenList* VariableMap::getVariable(const std::string &key) const {
  VariableMap::const_iterator mit;

  if ((mit = this->find(key)) != this->end()) {
    return &mit->second;
  } else
    return NULL;
}

void VariableMap::merge(const VariableMap &map) {
  this->insert(map.cbegin(), map.cend());

}

void VariableMap::overwrite(const VariableMap &map) {
  VariableMap::const_iterator it;
  
  for (it = map.cbegin(); it != map.cend(); ++it) {
    (*this)[it->first] = it->second;
  }
}
    
std::string VariableMap::toString() const {
  std::string str;
  VariableMap::const_iterator it;
  
  for (it = this->cbegin(); it != this->cend(); ++it) {
    str.append(it->first);
    str.append(": ");
    str.append(it->second.toString());
    str.append("\n");
  }
  return str;
}

