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

#include "ValueProcessor.h"

#include <sstream>
#include <iostream>

template <class T>
inline std::string to_string (const T& t)
{
  std::stringstream ss;
  ss << t;
  return ss.str();
}

ValueProcessor::ValueProcessor() {
  pushScope();
  
  functionLibrary = new FunctionLibrary();
  NumberValue::loadFunctions(functionLibrary);
  Color::loadFunctions(functionLibrary);
  StringValue::loadFunctions(functionLibrary);  
}
ValueProcessor::~ValueProcessor() {
  popScope();
}

TokenList* ValueProcessor::processValue(TokenList* value) {
  TokenList newvalue;
  Value* v;
  TokenList* var;
  TokenList* variable;
  
  while (value->size() > 0) {
    v = processStatement(value);

    if (v != NULL || value->size() > 0) {
      if (newvalue.size() == 0 ||
          !needsSpace(newvalue.back(), true) ||
          (v == NULL &&
           !needsSpace(value->front(), false))) {
        
      } else {
        newvalue.push(new Token(" ", Token::WHITESPACE));
      }
    }
    
    if (v != NULL) {
      newvalue.push(v->getTokens());
      delete v;
    } else if (value->size() > 0) {
      if (value->front()->type == Token::ATKEYWORD &&
          (variable = getVariable(value->front()->str)) != NULL) {
        newvalue.push(variable);
        delete value->shift();
        
      } else if (value->front()->type == Token::URL) {
        processString(value->front());
        newvalue.push(value->shift());
        
      } else if ((var = processDeepVariable(value)) != NULL) {
        newvalue.push(var);
        delete var;
        delete value->shift();
        delete value->shift();
          
      } else
        newvalue.push(value->shift());
    }
  }
  value->push(&newvalue);
  return value;
}
void ValueProcessor::putVariable(string key, TokenList* value) {
  map<string, TokenList*>* scope = scopes.back();
  map<string, TokenList*>::iterator mit;
  
  // check if variable is alread declared
  mit = scope->find(key);
  if (mit != scope->end()) {
    cerr << "Warning: Variable " << key << " defined twice in same scope." << endl;
  }
  scope->insert(pair<string, TokenList*>(key, value));
}
TokenList* ValueProcessor::getVariable(string key) {
  list<map<string, TokenList*>*>::reverse_iterator it;
  map<string, TokenList*>::iterator mit;
  
  for (it = scopes.rbegin(); it != scopes.rend(); it++) {
    mit = (*it)->find(key);
    if (mit != (*it)->end()) 
      return mit->second;
  }
  
  return NULL;
}

void ValueProcessor::pushScope() {
  scopes.push_back(new map<string, TokenList*>());
}
void ValueProcessor::popScope() {
  // delete tokenlists in scope
  delete scopes.back();
  scopes.pop_back();
}


Value* ValueProcessor::processStatement(TokenList* value) {
  Value* op, *v = processConstant(value);
  
  if (v != NULL) {
    while ((op = processOperator(value, v))) 
      v = op;
    return v;
  } else
    return NULL;
}

Value* ValueProcessor::processOperator(TokenList* value, Value* v1,
                                       Token* lastop) {
  Value* v2, *tmp;
  Token* op;
  string operators("+-*/=><>==<");
  unsigned int pos;
  
  while (value->size() > 0 &&
         value->front()->type == Token::WHITESPACE) {
    delete value->shift();
  }
  if (value->size() == 0 ||
      (pos = operators.find(value->front()->str)) == string::npos)
    return NULL;
  
  if (pos < 7 && value->front()->str.size() != 1)
    return NULL;
  
  if (pos >= 7 && (value->front()->str.size() != 2 ||
                   pos % 2 != 1))
    return NULL;
  
  if (lastop != NULL &&
      operators.find(lastop->str) >
      operators.find(value->front()->str)) {
    return NULL;
  }
  op = value->shift();
  v2 = processConstant(value);
  if (v2 == NULL) {
    if (value->size() > 0) 
      throw new ParseException(value->front()->str, "Constant or @-variable");
    else
      throw new ParseException("end of line", "Constant or @-variable");
  }
  while ((tmp = processOperator(value, v2, op))) 
    v2 = tmp;

  if (v1->type == Value::DIMENSION &&
      v2->type == Value::DIMENSION &&
      ((NumberValue*)v1)->getUnit()
      .compare(((NumberValue*)v2)->getUnit()) != 0) {
    if (UnitValue::getUnitGroup(((NumberValue*)v1)->getUnit()) ==
        UnitValue::getUnitGroup(((NumberValue*)v2)->getUnit())) {
      
    }
    throw new ValueException("Can't do math on dimensions with different units.");
  }
  
  if (op->str == "+") 
    tmp = v1->add(v2);
  else if (op->str == "-")
    tmp = v1->substract(v2);
  else if (op->str == "*")
    tmp = v1->multiply(v2);
  else if (op->str == "/")
    tmp = v1->divide(v2);
  else if (op->str == "=")
    tmp = v1->equals(v2);
  else if (op->str == "<")
    tmp = v1->lessThan(v2);
  else if (op->str == ">")
    tmp = v1->greaterThan(v2);
  else if (op->str == "=<")
    tmp = v1->lessThanEquals(v2);
  else if (op->str == ">=")
    tmp = v1->greaterThanEquals(v2);
  
  if (tmp != v1)
    delete v1;
  if (tmp != v2)
    delete v2;
  return tmp;
}
Value* ValueProcessor::processConstant(TokenList* value) {
  Token* token;
  Value* ret;
  TokenList* variable;

  while (value->size() > 0 &&
         value->front()->type == Token::WHITESPACE) {
    delete value->shift();
  }

  if (value->size() == 0)
    return NULL;
  
  token = value->front();
  switch(token->type) {
  case Token::HASH:
    // generate color from hex value
    return new Color(value->shift());
  case Token::NUMBER:
  case Token::PERCENTAGE:
  case Token::DIMENSION:
    return new NumberValue(value->shift());

  case Token::FUNCTION:
    value->shift();
    ret = processFunction(token->
                          str.substr(0, token->str.size() - 1),
                          value);
    delete token;
    return ret;
    
  case Token::ATKEYWORD:
    if ((variable = getVariable(token->str)) != NULL) {
      variable = variable->clone();
      ret = processConstant(variable);
      
      while(!variable->empty() &&
            variable->front()->type == Token::WHITESPACE)
        delete variable->shift();
      
      if (!variable->empty()) {
        delete ret;
        ret = NULL;
      } else 
        delete value->shift();
      
      delete variable;
      return ret;
    } else
      return NULL;

  case Token::STRING:
    processString(value->front());
    value->front()->str = removeQuotes(value->front()->str);
    return new StringValue(value->shift(), true);
    
  case Token::PAREN_OPEN:
    delete value->shift();
    
    ret = processStatement(value);

    if (ret == NULL)
      throw new ValueException("Expecting a valid expression in \
parentheses. Something like '(5 + @x)'. Alternatively, one of the \
variables in the expression may not contain a proper value like 5, \
5%, 5em or #555555."); 

    if (value->size() == 0)
      throw new ParseException("end of line", ")");
    else if (value->front()->type == Token::PAREN_CLOSED)
      delete value->shift();
    else
      throw new ParseException(value->front()->str, ")");

    return ret;
    
  default:
    break;
  }

  if ((variable = processDeepVariable(value)) != NULL) {
    ret = processConstant(variable);
    if (ret != NULL) {
      delete value->shift();
      delete value->shift();
    }
    delete variable;
    return ret;

  } else if(token->str.compare("%") == 0 &&
            value->size() > 2 &&
            value->at(1)->type == Token::PAREN_OPEN) {
    delete value->shift();
    delete value->shift();
    return processFunction("%", value);
      
  } else if ((ret = processEscape(value)) != NULL) {
    return ret;
  } else if ((ret = processUnit(value->front())) != NULL) {
    value->shift();
    return ret;  
  } else if (token->str.compare("true") == 0) {
    delete value->shift();
    return new BooleanValue(true);
  }

  return NULL;
}

TokenList* ValueProcessor::processDeepVariable (TokenList* value) {
  Token* first, *second;
  TokenList* var;
  string key = "@";
  
  if (value->size() < 2) 
    return NULL;
  
  first = value->front();
  second = value->at(1);
  
  if (first->type != Token::OTHER ||
      first->str != "@" ||
      second->type != Token::ATKEYWORD ||
      (var = getVariable(second->str)) == NULL)
    return NULL;

  if (var->size() > 1 || var->front()->type != Token::STRING)
    return NULL;

  // generate key with '@' + var without quotes
  key.append(var->front()->
             str.substr(1, var->front()->str.size() - 2));

  var = getVariable(key);
  if (var == NULL)
    return NULL;

  return var->clone();
}

Value* ValueProcessor::processFunction(string function, TokenList* value) {
  string percentage;
  vector<Value*> arguments;
  FuncInfo* fi;
  Value* ret;
  vector<Value*>::iterator it;
  string arg_str;
  
  fi = functionLibrary->getFunction(function.c_str());
  
  if (fi == NULL)
    return NULL;

  arguments = processArguments(value);

  if (functionLibrary->checkArguments(fi, arguments)) {
    ret = fi->func(arguments);
  } else {
    arg_str.append(function);
    arg_str.append("(");
    for (it = arguments.begin(); it != arguments.end(); it++) {
      if (it != arguments.begin())
        arg_str.append(", ");
      arg_str.append((*it)->getTokens()->toString()->c_str());
    }
    arg_str.append(")");
    
    throw new ParseException(arg_str,
                             functionLibrary->
                             functionDefToString(function.c_str(),
                                                 fi));
  }
  
  // delete arguments
  for (it = arguments.begin(); it != arguments.end(); it++) {
    if (*it != ret)
      delete (*it);
  }
  
  return ret;
}

vector<Value*> ValueProcessor::processArguments (TokenList* value) {
  vector<Value*> arguments;
  
  if (value->front()->type != Token::PAREN_CLOSED) 
    arguments.push_back(processStatement(value));
    
  while (value->front()->str == ",") {
    delete value->shift();
    arguments.push_back(processStatement(value));
  }
  if (value->front()->type != Token::PAREN_CLOSED) 
    throw new ParseException(value->front()->str, ")");
    
  delete value->shift();

  return arguments;
}

void ValueProcessor::processString(Token* token) {
  size_t start, end;
  string key = "@", value;
  TokenList* var;
  
  if ((start = token->str.find("@{")) == string::npos ||
      (end = token->str.find("}", start)) == string::npos)
    return;
  
  key.append(token->str.substr(start + 2, end - (start + 2)));
  var = getVariable(key);
  if (var == NULL)
    return;

  value = *var->toString();

  // Remove quotes off strings.
  if (var->size() == 1 && var->front()->type == Token::STRING) 
    value = value.substr(1, value.size() - 2);
  
  token->str.replace(start, (end + 1) - start, value);
}

Value* ValueProcessor::processEscape (TokenList* value) {
  if (value->size() < 2 ||
      value->front()->str != "~" ||
      value->at(1)->type != Token::STRING) 
    return NULL;

  delete value->shift();
  processString(value->front());
  value->front()->str = removeQuotes(value->front()->str);
  return new StringValue(value->shift(), false);
}

string ValueProcessor::removeQuotes(string str) {
  str = str.substr(1, str.size() - 2);
  string::iterator i;
  string ret;
  
  for (i = str.begin(); i != str.end(); i++) {
    if (*i == '\\') 
      i++;
    ret.push_back(*i);
  }
  return ret;
}

UnitValue* ValueProcessor::processUnit(Token* t) {
  // em,ex,px,ch,in,mm,cm,pt,pc,ms
  string units("emexpxchinmmcmptpcms");
  unsigned int pos;
  if (t->str.size() == 2 &&
      (pos = units.find(t->str)) != string::npos &&
      pos % 2 == 0) {
    return new UnitValue(t);
  } else if(t->str.compare("m") == 0 ||
            t->str.compare("s") == 0 ||
            t->str.compare("rad") == 0 ||
            t->str.compare("deg") == 0 ||
            t->str.compare("grad") == 0 ||
            t->str.compare("turn") == 0) {
    return new UnitValue(t);
  } else
    return NULL;
}

bool ValueProcessor::needsSpace(Token* t, bool suffix) {
  if (t->type == Token::OTHER &&
      t->str.compare(",") == 0) {
    return false;
  }
  if (suffix && t->type == Token::FUNCTION) 
    return false;
  return !(t->type == Token::FUNCTION ||
           t->type == Token::PAREN_OPEN ||
           t->type == Token::PAREN_CLOSED);
}
