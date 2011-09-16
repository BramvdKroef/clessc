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
}
ValueProcessor::~ValueProcessor() {
  popScope();
}

TokenList* ValueProcessor::processValue(TokenList* value) {
  TokenList newvalue;
  Value* v;
  TokenList* var;
  Token* token;
  TokenList* variable;
  
  while (value->size() > 0) {
    v = processStatement(value);

    if (v != NULL) {
      if (newvalue.size() > 0)
        newvalue.push(new Token(" ", Token::WHITESPACE));

      newvalue.push(v->getToken()->clone());
      delete v;
    } else if (value->size() > 0) {
      if (newvalue.size() > 0)
        newvalue.push(new Token(" ", Token::WHITESPACE));

      if (value->front()->type == Token::ATKEYWORD &&
          (variable = getVariable(value->front()->str)) != NULL) {
        newvalue.push(variable);
        delete value->shift();
      } else if (value->front()->type == Token::STRING ||
                 value->front()->type == Token::URL) {
        processString(value->front());
        newvalue.push(value->shift());
      } else {
        if ((var = processDeepVariable(value)) != NULL) {
          newvalue.push(var);
          delete var;
          delete value->shift();
          delete value->shift();
        } else if ((token = processEscape(value)) != NULL)
            newvalue.push(token);
        else
          newvalue.push(value->shift());
      }
    }
  }
  value->push(&newvalue);
  return value;
}
void ValueProcessor::putVariable(string key, TokenList* value) {
  scopes.back()->insert(pair<string, TokenList*>(key, value));
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
  string operators("+-*/");

  while (value->size() > 0 &&
         value->front()->type == Token::WHITESPACE) {
    delete value->shift();
  }
  if (value->size() == 0 ||
      operators.find(value->front()->str) == string::npos)
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

  if (v2->type == Value::COLOR && v1->type != Value::COLOR) {
    tmp = v1;
    v1 = v2;
    v2 = tmp;
  }
  
  if (op->str == "+") 
    v1->add(v2);
  else if (op->str == "-")
    v1->substract(v2);
  else if (op->str == "*")
    v1->multiply(v2);
  else if (op->str == "/")
    v1->divide(v2);
  delete v2;
  return v1;
}
Value* ValueProcessor::processConstant(TokenList* value) {
  Token* token;
  Value* ret;
  vector<Value*> arguments;
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
    return new Value(value->shift());

  case Token::FUNCTION:
    value->shift();
    if (value->front()->type != Token::PAREN_CLOSED) 
      arguments.push_back(processConstant(value));
    
    while (value->front()->str == ",") {
      delete value->shift();
      arguments.push_back(processConstant(value));
    }
    if (value->front()->type != Token::PAREN_CLOSED) 
      throw new ParseException(value->front()->str, ")");
    
    delete value->shift();
    
    return processFunction(token, arguments);
    
  case Token::ATKEYWORD:
    if ((variable = getVariable(token->str)) != NULL) {
      TokenList* var = variable->clone();
      ret = processConstant(var);
      while(!var->empty() && var->front()->type == Token::WHITESPACE)
        delete var->shift();
      
      if (!var->empty()) {
        delete ret;
        ret = NULL;
      } else
        delete value->shift();
      
      delete var;
      return ret;
    } else
      return NULL;

  case Token::PAREN_OPEN:
    delete value->shift();
    
    ret = processStatement(value);
    
    if (value->front()->type == Token::PAREN_CLOSED)
      delete value->shift();
    return ret;

  default:
    TokenList* var = processDeepVariable(value);

    if (var != NULL) {
      ret = processConstant(var);
      if (ret != NULL) {
        delete value->shift();
        delete value->shift();
      }
      delete var;
      return ret;
    } else
      return NULL;
  }
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

Value* ValueProcessor::processFunction(Token* function,
                                   vector<Value*> arguments) {
  Color* color;
  string percentage;
  
  if(function->str == "rgb(") {
    if (arguments.size() == 3 &&
        arguments[0]->type == Value::NUMBER &&
        arguments[1]->type == Value::NUMBER &&
        arguments[2]->type == Value::NUMBER) {
      return new Color(0,0,0);
    }      
    // Color rgb(@red: NUMBER, @green: NUMBER, @blue: NUMBER)
  } else if (function->str == "lighten(") {
    // Color lighten(Color, PERCENTAGE)
    if (arguments.size() == 2 &&
        arguments[0]->type == Value::COLOR &&
        arguments[1]->type == Value::PERCENTAGE) {
      static_cast<Color*>(arguments[0])->lighten(arguments[1]->getPercent());
      return arguments[0];
    }
    
  } else if (function->str == "darken(") {
    // Color darken(Color, PERCENTAGE)
    if (arguments.size() == 2 &&
        arguments[0]->type == Value::COLOR &&
        arguments[1]->type == Value::PERCENTAGE) {
      static_cast<Color*>(arguments[0])->darken(arguments[1]->getPercent());
      return arguments[0];
    }

  } else if (function->str == "saturate(") {
    // Color saturate(Color, PERCENTAGE)
    if (arguments.size() == 2 &&
        arguments[0]->type == Value::COLOR &&
        arguments[1]->type == Value::PERCENTAGE) {
      static_cast<Color*>(arguments[0])->saturate(arguments[1]->getPercent());
      return arguments[0];
    }

  } else if (function->str == "desaturate(") {
    // Color desaturate(Color, PERCENTAGE)
    if (arguments.size() == 2 &&
        arguments[0]->type == Value::COLOR &&
        arguments[1]->type == Value::PERCENTAGE) {
      static_cast<Color*>(arguments[0])->desaturate(arguments[1]->getPercent());
      return arguments[0];
    }

  } else if (function->str == "fadein(") {
    // Color fadein(Color, PERCENTAGE)
    if (arguments.size() == 2 &&
        arguments[0]->type == Value::COLOR &&
        arguments[1]->type == Value::PERCENTAGE) {
      static_cast<Color*>(arguments[0])->fadein(arguments[1]->getPercent());
      return arguments[0];
    }

  } else if (function->str == "fadeout(") {
    // Color fadeout(Color, PERCENTAGE)
    if (arguments.size() == 2 &&
        arguments[0]->type == Value::COLOR &&
        arguments[1]->type == Value::PERCENTAGE) {
      static_cast<Color*>(arguments[0])->fadeout(arguments[1]->getPercent());
      return arguments[0];
    }

  } else if (function->str == "spin(") {
    // Color fadein(Color, PERCENTAGE)
    if (arguments.size() == 2 &&
        arguments[0]->type == Value::COLOR &&
        arguments[1]->type == Value::NUMBER) {
      static_cast<Color*>(arguments[0])->spin(arguments[1]->getValue());
      return arguments[0];
    }

  } else if (function->str == "hsl(") {
    // Color hsl(PERCENTAGE, PERCENTAGE, PERCENTAGE)
    if (arguments.size() == 3 &&
        arguments[0]->type == Value::PERCENTAGE &&
        arguments[1]->type == Value::PERCENTAGE &&
        arguments[2]->type == Value::PERCENTAGE) {
      color = new Color(0,0,0);
      color->setHSL(arguments[0]->getPercent(),
                    arguments[1]->getPercent(),
                    arguments[2]->getPercent());
      return color;
    }      
  } else if (function->str == "hue(") {
    // PERCENTAGE hue(Color)
    if (arguments.size() == 1 &&
        arguments[0]->type == Value::COLOR) {
      percentage.append(to_string(static_cast<Color*>(arguments[0])->getHue()));
      percentage.append("%");
      return new Value(new Token(percentage, Token::PERCENTAGE));
    }
    
  } else if (function->str == "saturation(") {
    // PERCENTAGE saturation(Color)
    if (arguments.size() == 1 &&
        arguments[0]->type == Value::COLOR) {
      percentage.append(to_string(static_cast<Color*>(arguments[0])->getSaturation())); 
      percentage.append("%");
      return new Value(new Token(percentage, Token::PERCENTAGE));
    }

  } else if (function->str == "lightness(") {
    // PERCENTAGE lightness(Color)
    if (arguments.size() == 1 &&
        arguments[0]->type == Value::COLOR) {
      percentage.append(to_string(static_cast<Color*>(arguments[0])->getLightness()));
      percentage.append("%");
      return new Value(new Token(percentage, Token::PERCENTAGE));
    }
  } else 
    return NULL;
  return NULL;
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

  // Remove quotes of strings.
  if (var->size() == 1 && var->front()->type == Token::STRING) 
    value = value.substr(1, value.size() - 2);
  
  token->str.replace(start, (end + 1) - start, value);
}

Token* ValueProcessor::processEscape (TokenList* value) {
  Token* first, *second;
  
  if (value->size() < 2) 
    return NULL;
  
  first = value->front();
  second = value->at(1);
  
  if (first->str != "~" ||
      second->type != Token::STRING) 
    return NULL;

  delete value->shift();
  second->str = second->str.substr(1, second->str.size() - 2);
  return value->shift();
}
