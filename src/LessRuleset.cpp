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

#include "LessRuleset.h"

LessRuleset::LessRuleset() : Ruleset() {
  processed = false;
}
LessRuleset::LessRuleset(Selector* selector) : Ruleset(selector) {
  processed = false;
}
LessRuleset::~LessRuleset() {
  while (!nestedRules.empty()) {
    delete nestedRules.back();
    nestedRules.pop_back();
  }
}
  

void LessRuleset::addNestedRule(Ruleset* nestedRule) {
  nestedRules.push_back(nestedRule);
}

list<Ruleset*>* LessRuleset::getNestedRules() {
  return &nestedRules;
}

void LessRuleset::putVariable(string key, TokenList* value) {
  variables.insert(pair<string, TokenList*>(key, value));  
}

map<string, TokenList*>* LessRuleset::getVariables() {
  return &variables;
}

bool LessRuleset::isProcessed() {
  return processed;
}
void LessRuleset::setProcessed(bool b) {
  processed = b;
}
