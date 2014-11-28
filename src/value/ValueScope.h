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

#ifndef __ValueScope_h__
#define __ValueScope_h__

#include <map>
#include <string>
#include <list>

#include "../TokenList.h"
#include "../Stylesheet.h"

class ValueScope {
private:
  const ValueScope* parent;
  const std::map<std::string, TokenList>* variables;

  //void putVariable(const std::string &key, const TokenList &value);
  
public:
  ValueScope(const ValueScope &p, const std::map<std::string, TokenList> &v);
  ValueScope(const std::map<std::string, TokenList> &v);
  
  const TokenList* getVariable(const std::string &key) const;
  
  const ValueScope* getParent() const;
};

#endif
