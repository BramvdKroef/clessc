
#include "ValueScope.h"

#include <config.h>
#ifdef WITH_LIBGLOG
#include <glog/logging.h>
#endif

ValueScope::ValueScope(const ValueScope &p,
                       const std::map<std::string, TokenList> &v):
  parent(&p), variables(&v){
}

ValueScope::ValueScope(const std::map<std::string, TokenList> &v):
  parent(NULL), variables(&v) {
}

/*
void ValueScope::putVariable(const std::string &key, const TokenList &value) {
  variables->insert(pair<std::string, TokenList>(key, value));
}
*/
const TokenList* ValueScope::getVariable(const std::string &key) const {
  std::map<std::string, TokenList>::const_iterator mit;
  
  mit = variables->find(key);
  
  if (mit != variables->end()) 
    return &mit->second;
  else if (getParent() != NULL)
    return getParent()->getVariable(key);
  else
    return NULL;
}

void ValueScope::copyVariables(std::map<std::string, TokenList> &variables,
                               const ValueScope *start) const {
  if (this == start) 
    return;
  
  variables.insert(this->variables->cbegin(), this->variables->cend());

  if (getParent() != NULL)
    getParent()->copyVariables(variables, start);
}

const ValueScope* ValueScope::getParent() const {
  return parent;
}
