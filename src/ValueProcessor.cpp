#include "ValueProcessor.h"

#include <sstream>

template <class T>
inline std::string to_string (const T& t)
{
  std::stringstream ss;
  ss << t;
  return ss.str();
}

TokenList* ValueProcessor::processValue(TokenList* value) {
  TokenList newvalue;
  Value* v;

  while (value->size() > 0) {
    v = processStatement(value);
    if (v != NULL) {
      newvalue.push(v->getToken()->clone());
      delete v;
    } else if (value->front()->type == Token::ATKEYWORD &&
               variables.count(value->front()->str)) {
      newvalue.push(variables[value->front()->str]);
      delete value->shift();
    } else
      newvalue.push(value->shift());
  }
  value->push(&newvalue);
  return value;
}
void ValueProcessor::putVariable(string key, TokenList* value) {
  variables[key] = value;
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
      
  if (!operators.find(value->front()->str))
    return NULL;
  
  if (lastop != NULL &&
      operators.find(lastop->str) >
      operators.find(value->front()->str)) {
    return v1;
  }
  op = value->shift();
  v2 = processConstant(value);
  while ((tmp = processOperator(value, v2, op))) 
    v2 = tmp;
    
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
  Token* token = value->front();
  Value* ret;
  vector<Value*> arguments;
  
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
    if (value->front()->type == Token::PAREN_CLOSED)
      delete value->shift();
    
    return processFunction(token, arguments);
    
  case Token::ATKEYWORD:
    if (variables.count(token->str)) {
      ret = processConstant(variables[token->str]);
      delete value->shift();
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
    if (value->size() > 1) {
      TokenList* var = processDeepVariable(token, value->at(1));
      if (var != NULL) {
        ret = processStatement(var);
        delete value->shift();
        return ret;
      }
    }
    return NULL;
  }
}

TokenList* ValueProcessor::processDeepVariable (Token* token, Token* nexttoken) {
  TokenList* var;
  string key("@");
  
  if (token->type != Token::OTHER ||
      token->str != "@" ||
      nexttoken->type != Token::ATKEYWORD ||
      !variables.count(nexttoken->str))
    return NULL;
  
  var = variables[nexttoken->str];

  if (var->size() > 1 || var->front()->type != Token::STRING)
    return NULL;

  // generate key with '@' + var without quotes
  key.append(var->front()->
             str.substr(1, var->front()->str.size() - 2));

  if (!variables.count(key))
    return NULL;
  
  return variables[key];
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
      static_cast<Color*>(arguments[0])->darken(arguments[1]->getPercent()());
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
        arguments[1]->type == Value::PERCENTAGE) {
      static_cast<Color*>(arguments[0])->spin(arguments[1]->getPercent());
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
