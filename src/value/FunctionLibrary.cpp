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

#include "FunctionLibrary.h"

FuncInfo* FunctionLibrary::getFunction(const char* functionName) {
  return map[functionName];
}

void FunctionLibrary::push(string name, const char* parameterTypes,
                           Value* (*func)(vector<Value*> arguments))
{
  FuncInfo* fi = new FuncInfo();
  fi->parameterTypes = parameterTypes;
  fi->func = func;
  map[name] = fi;
}
