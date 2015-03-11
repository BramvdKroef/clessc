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

#include <config.h>
#ifdef WITH_LIBGLOG
#include <glog/logging.h>
#endif

/*
#include <sstream>
#include <iostream>
template <class T>
inline std::string to_string (const T& t)
{
  std::stringstream ss;
  ss << t;
  return ss.str();
}
*/

ValueProcessor::ValueProcessor() {
  NumberValue::loadFunctions(functionLibrary);
  Color::loadFunctions(functionLibrary);
  StringValue::loadFunctions(functionLibrary);
  UrlValue::loadFunctions(functionLibrary);  
}
ValueProcessor::~ValueProcessor() {
}

void ValueProcessor::processValue(TokenList &value, const ValueScope &scope) {
  TokenList newvalue;
  Value* v;
  const TokenList* var;
  TokenList variable;
  TokenList::iterator i;

  if (!needsProcessing(value)) {
    // interpolate strings
    for(i = value.begin(); i != value.end(); i++) {
      if ((*i).type == Token::STRING)
        interpolate((*i), scope);
    }
    return;
  }

  while (value.size() > 0) {
    v = processStatement(value, scope);

    // add spaces between values
    if (v != NULL || value.size() > 0) {
      if (newvalue.size() == 0 ||
          !needsSpace(newvalue.back(), false) ||
          (v == NULL &&
           !needsSpace(value.front(), true))) {
        
      } else {
        newvalue.push_back(Token(" ", Token::WHITESPACE));
      }
    }
    
    if (v != NULL) {
      newvalue.insert(newvalue.end(),
                      v->getTokens()->begin(),
                      v->getTokens()->end());
      delete v;
    } else if (value.size() > 0) {
      // variable containing a non-value.
      if (value.front().type == Token::ATKEYWORD &&
          (var = scope.getVariable(value.front())) != NULL) {
        variable = *var;
        processValue(variable, scope);
        
        newvalue.insert(newvalue.end(), variable.begin(), variable.end());
        value.pop_front();
        
      } else if ((var = processDeepVariable(value, scope)) != NULL) {
        variable = *var;
        processValue(variable, scope);

        newvalue.insert(newvalue.end(), variable.begin(), variable.end());
        value.pop_front();
        value.pop_front();

      } else if (value.size() > 2 &&
                 value.front().type == Token::IDENTIFIER) {

        newvalue.push_back(value.front());
        value.pop_front();
        
        if (value.front().type == Token::PAREN_OPEN) {
          newvalue.push_back(value.front());
          value.pop_front();
        }
      } else {
        newvalue.push_back(value.front());
        value.pop_front();
      }
    }
  }
  value.swap(newvalue);
  return;
}

bool ValueProcessor::needsProcessing(const TokenList &value) {
  TokenList::const_iterator i;
  const Token* t;
  string operators("+-*/");
  
  for(i = value.begin(); i != value.end(); i++) {

      // variable
    if ((*i).type == Token::ATKEYWORD ||
        // url
        (*i).type == Token::URL ||
        // operator
        operators.find(*i) != string::npos) {
      return true;

    } else if (i != value.end()) {
      // function
      if ((*i).type == Token::IDENTIFIER) {
        t = &(*i);
        if ((*++i).type == Token::PAREN_OPEN &&
            functionExists(*t)) {
          return true;
        } else
          i--;
        
      } else if (*i == "~") {
        if ((*++i).type == Token::STRING)
          return true;
        else
          i--;
      }
    }
  }
  
  return false;
}

bool ValueProcessor::validateCondition(TokenList &value, const ValueScope &scope) {
  bool ret = validateValue(value, scope);

  value.ltrim();
  
  while(ret == true &&
        !value.empty() &&
        value.front() == "and") {
    value.pop_front();
    value.ltrim();
    ret = validateValue(value, scope);
    value.ltrim();
  }
  
  return ret;
}
bool ValueProcessor::validateValue(TokenList &value, const ValueScope &scope) {
  Value* v = processStatement(value, scope);
  const BooleanValue trueVal(true);
  Value* v2;
  bool ret;

  if (v == NULL) {
    throw new ParseException(value.front(),
                             "condition", 0, 0, "");
  }
  
  v2 = v->equals(trueVal);
  ret = ((BooleanValue*)v2)->getValue();

  delete v;
  delete v2;
  
  return ret;
}


Value* ValueProcessor::processStatement(TokenList &value, const ValueScope& scope) {
  Value* op, *v;

  value.ltrim();
  v = processConstant(value, scope);
  
  if (v != NULL) {
    value.ltrim();

    while ((op = processOperator(value, *v, scope)) != NULL) {
      if (v != op) {
        delete v;
        v = op;        
      }
      value.ltrim();
    }
    
    return v;
  } else
    return NULL;
}

Value* ValueProcessor::processOperator(TokenList &value, const Value &operand1,
                                       const ValueScope &scope, Token* lastop) {
  const Value* operand2;
  Value* result;
  Token op;
  std::string operators("+-*/=><");
  size_t pos;
  
  if (value.size() == 0 ||
      (pos = operators.find(value.front())) == string::npos)
    return NULL;

  if (lastop != NULL &&
      operators.find(*lastop) >= pos) {
    return NULL;
  }
  op = value.front();
  value.pop_front();

  // if a minus is not followed by a space we have to consider it
  // part of a negative consonant.
  if (op == "-" && value.front().type != Token::WHITESPACE) {
    value.push_front(op);
    return NULL;
  }

  // Check for 2 char operators ('>=' and '=<')
  if (value.size() > 0 &&
      (pos = operators.find(value.front())) != string::npos) {
    op.append(value.front());
    value.pop_front();
  }

  value.ltrim();
  
  operand2 = processConstant(value, scope);
  if (operand2 == NULL) {
    if (value.size() > 0) 
      throw new ParseException(value.front(),
                               "Constant or @-variable", 0, 0, "");
    else
      throw new ParseException("end of line",
                               "Constant or @-variable", 0, 0, "");
  }

  value.ltrim();
  
  while ((result = processOperator(value, *operand2, scope, &op))) {
    delete operand2;
    operand2 = result;
    
    value.ltrim();
  }

#ifdef WITH_LIBGLOG
  VLOG(3) << "Operation: " << operand1.getTokens()->toString() << 
    "(" << Value::typeToString(operand1.type) <<  ") " << op << " " <<
    operand2->getTokens()->toString() << "(" <<
    Value::typeToString(operand2->type) << ")";
#endif
  
  if (op == "+") 
    result = operand1.add(*operand2);
  else if (op == "-")
    result = operand1.substract(*operand2);
  else if (op == "*")
    result = operand1.multiply(*operand2);
  else if (op == "/")
    result = operand1.divide(*operand2);
  else if (op == "=")
    result = operand1.equals(*operand2);
  else if (op == "<")
    result = operand1.lessThan(*operand2);
  else if (op == ">")
    result = operand1.greaterThan(*operand2);
  else if (op == "=<")
    result = operand1.lessThanEquals(*operand2);
  else if (op == ">=") 
    result = operand1.greaterThanEquals(*operand2);
  
  delete operand2;
  return result;
}
Value* ValueProcessor::processConstant(TokenList &value, const ValueScope &scope) {
  Token token;
  Value* ret;
  const TokenList* var;
  TokenList variable;
  bool hasQuotes;
  std::string str;
  
  if (value.size() == 0)
    return NULL;
  
  token = value.front();
  
#ifdef WITH_LIBGLOG
  VLOG(3) << "Constant: " << token;
#endif
  
  switch(token.type) {
  case Token::HASH:
    value.pop_front();
    // generate color from hex value
    return new Color(token);
    
  case Token::NUMBER:
  case Token::PERCENTAGE:
  case Token::DIMENSION:
    value.pop_front();
    return new NumberValue(token);

  case Token::ATKEYWORD:
    if ((var = scope.getVariable(token)) != NULL) {
      variable = *var;

      ret = processStatement(variable, scope);
      
      if (ret != NULL && variable.empty()) {
        value.pop_front();
        return ret;
      }
    } 
    return NULL;

  case Token::STRING:
    value.pop_front();
    hasQuotes = token.stringHasQuotes();
    interpolate(token, scope);
    token.removeQuotes();
    return new StringValue(token, hasQuotes);

  case Token::URL:
    value.pop_front();
    interpolate(token, scope);
    str = token.getUrlString();
    return new UrlValue(token, str);
        
  case Token::IDENTIFIER:
    value.pop_front();
    
    if (value.size() > 1 &&
        value.front().type == Token::PAREN_OPEN) {

      if (functionExists(token)) {
        value.pop_front();
      
        return processFunction(token, value, scope);
      } else {
        value.push_front(token);
        return NULL;
      }
      
    } else if ((ret = processUnit(token)) != NULL) {
      return ret;  
    } else if (token.compare("true") == 0) {
      return new BooleanValue(true);
    } else  
      return new StringValue(token, false);
    
  case Token::PAREN_OPEN:
    value.pop_front();
    ret = processStatement(value, scope);

    value.ltrim();
    
    if (value.size() == 0)
      throw new ParseException("end of line", ")", 0, 0, "");

    if (ret != NULL) {
      if (value.front().type == Token::PAREN_CLOSED) {
        value.pop_front();
        return ret;
      } else {
        value.insert(value.begin(),
                     ret->getTokens()->begin(),
                     ret->getTokens()->end());
        delete ret;
      }
    }
    value.push_front(token);
    return NULL;
    
  default:
    break;
  }

  if ((var = processDeepVariable(value, scope)) != NULL) {
    variable = *var;
    ret = processStatement(variable, scope);
    if (ret != NULL) {
      value.pop_front();
      value.pop_front();
    }
    return ret;

  } else if(token == "%" &&
            value.size() > 2) {
    value.pop_front();
    if (value.front().type == Token::PAREN_OPEN) {
      value.pop_front();
      return processFunction("%", value, scope);
      
    } else {
      value.push_front(token);
      return NULL;
    }
  } else if ((ret = processEscape(value, scope)) != NULL) {
    return ret;
  } else if ((ret = processNegative(value, scope)) != NULL) {
    return ret;
  }
  return NULL;
}

const TokenList* ValueProcessor::processDeepVariable (TokenList &value,
                                                const ValueScope &scope) {
  TokenList::iterator i = value.begin();
  const TokenList* var;
  TokenList variable;
  string key = "@";
  
  if (value.size() < 2) 
    return NULL;
  
  if ((*i).type != Token::OTHER ||
      (*i) != "@")
    return NULL;

  i++;
  
  if ((*i).type != Token::ATKEYWORD ||
      (var = scope.getVariable((*i))) == NULL) 
    return NULL;

  variable = *var;
  processValue(variable, scope);
  
  if (variable.size() != 1 || variable.front().type != Token::STRING)
    return NULL;
  
  // generate key with '@' + var without quotes
  variable.front().removeQuotes();
  key.append(variable.front());
  
  return scope.getVariable(key);
}

bool ValueProcessor::functionExists(const std::string &function) {
  
  return (functionLibrary.getFunction(function.c_str()) != NULL);
}

Value* ValueProcessor::processFunction(const std::string &function,
                                       TokenList &value,
                                       const ValueScope &scope) {
  string percentage;
  vector<Value*> arguments;
  FuncInfo* fi;
  Value* ret;
  vector<Value*>::iterator it;
  string arg_str;

#ifdef WITH_LIBGLOG
  VLOG(3) << "Function: " << function;
#endif
  
  fi = functionLibrary.getFunction(function.c_str());
  
  if (fi == NULL)
    return NULL;

  arguments = processArguments(value, scope);

  if (functionLibrary.checkArguments(fi, arguments)) {
    ret = fi->func(arguments);
  } else {
    arg_str.append(function);
    arg_str.append("(");
    for (it = arguments.begin(); it != arguments.end(); it++) {
      if (it != arguments.begin())
        arg_str.append(", ");
      arg_str.append((*it)->getTokens()->toString().c_str());
    }
    arg_str.append(")");
    
    throw new ParseException(arg_str,
                             functionLibrary.
                             functionDefToString(function.c_str(),fi),
                             0,0, "");
  }
  
  // delete arguments
  for (it = arguments.begin(); it != arguments.end(); it++) {
    if (*it != ret)
      delete (*it);
  }
  
  return ret;
}

vector<Value*> ValueProcessor::processArguments (TokenList &value,
                                                 const ValueScope &scope) {
  vector<Value*> arguments;
  Value* argument;

  if (value.size() == 0) 
    throw new ParseException("end of value", ")", 0, 0, "");
  
  if (value.front().type != Token::PAREN_CLOSED)  {
    argument = processStatement(value, scope);
    if (argument != NULL)
      arguments.push_back(argument);
    else {
      arguments.push_back(new StringValue(value.front(), false));
      value.pop_front();
    }
  }
  
  while (value.size() > 0 &&
         (value.front() == "," ||
          value.front() == ";")) {
    value.pop_front();

    argument = processStatement(value, scope);

    if (argument != NULL) {
      arguments.push_back(argument);
    } else if (value.front().type != Token::PAREN_CLOSED) {
      arguments.push_back(new StringValue(value.front(), false));      
      value.pop_front();
    }
  }

  if (value.size() == 0) 
    throw new ParseException("end of value", ")", 0, 0, "");
  
  if (value.front().type != Token::PAREN_CLOSED) 
    throw new ParseException(value.front(), ")", 0, 0, "");
    
  value.pop_front();

  return arguments;
}


Value* ValueProcessor::processEscape (TokenList &value, const ValueScope &scope) {
  Token t;
  
  if (value.size() < 2 ||
      value.front() != "~")
    return NULL;

  t = value.front();
  value.pop_front();

  if (value.front().type != Token::STRING) {
    value.push_front(t);
    return NULL;
  }

  t = value.front();
  value.pop_front();
  interpolate(t, scope);
  t.removeQuotes();
  return new StringValue(t, false);
}

UnitValue* ValueProcessor::processUnit(Token &t) {
  // em,ex,px,ch,in,mm,cm,pt,pc,ms
  string units("emexpxchinmmcmptpcms");
  size_t pos;
  if (t.size() == 2 &&
      (pos = units.find(t)) != string::npos &&
      pos % 2 == 0) {
    return new UnitValue(t);
  } else if(t.compare("m") == 0 ||
            t.compare("s") == 0 ||
            t.compare("rad") == 0 ||
            t.compare("deg") == 0 ||
            t.compare("grad") == 0 ||
            t.compare("turn") == 0) {
    return new UnitValue(t);
  } else
    return NULL;
}

bool ValueProcessor::needsSpace(const Token &t, bool before) {
  if (t.type == Token::OTHER &&
      t.size() == 1 &&
      string(":=.").find(t[0]) != string::npos) {
    return false;
  }
  if (before && t.type == Token::COLON)
    return false;
  if (!before && t.type == Token::PAREN_OPEN)
    return false;
  if (before && t.type == Token::PAREN_CLOSED)
    return false;
  if (before && t == ",")
    return false;
  return true;
}

Value* ValueProcessor::processNegative(TokenList &value,
                                             const ValueScope &scope) {
  Token minus;
  Value* constant;
  Value *zero, *ret;
  Token t_zero("0", Token::NUMBER);
    
  if (value.front() != "-")
    return NULL;
  
  minus = value.front();
  value.pop_front();
  
  value.ltrim();
  constant = processConstant(value, scope);
  if (constant == NULL) {
    value.push_front(minus);
    return NULL;
  }
#ifdef WITH_LIBGLOG
  VLOG(3) << "Negate: " << constant->getTokens()->toString();
#endif

  zero = new NumberValue(t_zero);
  ret = zero->substract(*constant);

  delete constant;
  delete zero;
  
  return ret;
}

void ValueProcessor::interpolate(std::string &str, const ValueScope &scope) {
  size_t start, end = 0;
  string key , value;
  const TokenList* var;
  TokenList variable;
  
  while ((start = str.find("@{", end)) != string::npos &&
         (end = str.find("}", start)) != string::npos) {
    key = "@";
    key.append(str.substr(start + 2, end - (start + 2)));

#ifdef WITH_LIBGLOG
    VLOG(3) << "Key: " << key;
#endif
    
    var = scope.getVariable(key);
    
    if (var != NULL) {
      variable = *var;

      processValue(variable, scope);

      // Remove quotes off strings.
      if (variable.size() == 1 &&
          variable.front().type == Token::STRING) {
        variable.front().removeQuotes();
      }

      value = variable.toString();
      
      str.replace(start, (end + 1) - start, value);
      end = start + value.length();
    }
  }
}

void ValueProcessor::interpolate(TokenList &tokens,
                                 const ValueScope &scope) {
  TokenList::iterator i;
  
  for (i = tokens.begin(); i != tokens.end(); i++) {
    interpolate((*i), scope);
  }
}

