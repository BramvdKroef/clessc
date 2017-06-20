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

