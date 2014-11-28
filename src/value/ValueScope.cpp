/*
 * Copyright 2012 Bram van der Kroef
 *
 * This file is part of LESS CSS Compiler.
 *
 * LESS CSS Compiler is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * LESS CSS Compiler is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with LESS CSS Compiler.  If not, see <http://www.gnu.org/licenses/>. 
 *
 * Author: Bram van der Kroef <bram@vanderkroef.net>
 */

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
  map<string, TokenList>::const_iterator mit;
  
  mit = variables->find(key);
  
  if (mit != variables->end()) 
    return &mit->second;
  else if (getParent() != NULL)
    return getParent()->getVariable(key);
  else
    return NULL;
}

const ValueScope* ValueScope::getParent() const {
  return parent;
}
