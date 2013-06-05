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

#ifndef __Selector_h__
#define __Selector_h__

#include "TokenList.h"
#include <list>
using namespace std;

/**
 * 
 */
class Selector: public TokenList {
public:
  virtual ~Selector();

  void addPrefix(Selector* prefix);

  /**
   * Trim whitespace from the front and back of the selector.
   */
  void trim();
  
  /**
   * If the selector contains commas then it is split up into multiple
   * selectors.
   * 
   * For example <code>p .class, a:hover</code> is split up into
   * <code>p .class</code> and <code>a:hover</code>.
   */
  list<TokenList*>* split();
  virtual Selector* clone();
};

#endif
