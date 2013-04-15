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

#include "NumberValue.h"

NumberValue::NumberValue(Token* token) {
  tokens.push(token);
  
  switch(token->type) {
  case Token::NUMBER:
    type = NUMBER;
    break;
  case Token::PERCENTAGE:
    type = PERCENTAGE;
    break;
  case Token::DIMENSION:
    type = DIMENSION;
    break;
  default:
    throw new ValueException("Token used for Value that isn't a number, percentage or dimension");
  }
}

NumberValue::~NumberValue() {
}

Value* NumberValue::add(Value* v) {
  Token* t;
  StringValue* s;
  
  if (v->type == Value::NUMBER ||
      v->type == Value::PERCENTAGE ||
      v->type == Value::DIMENSION) {
    
    if (type == NUMBER) 
      setType(v);
    setValue(getValue() + ((NumberValue*)v)->getValue());
    return this;
    
  } else if (v->type == COLOR) {
    return v->add(this);
    
  } else if (v->type == STRING) {
    t = this->tokens.front()->clone();
    t->type = Token::STRING;
    s = new StringValue(t, ((StringValue*)v)->getQuotes());
    s->add(v);
    return s;
  } else {
    throw new ValueException("Unsupported type.");
  }
}
Value* NumberValue::substract(Value* v) {
  if (v->type == Value::NUMBER ||
      v->type == Value::PERCENTAGE ||
      v->type == Value::DIMENSION) {
    if (type == NUMBER) 
      setType(v);
    setValue(getValue() - ((NumberValue*)v)->getValue());
    return this;
  } else
    throw new ValueException("You can only substract a *number* from a number.");
}
Value* NumberValue::multiply(Value* v) {
  if (v->type == Value::NUMBER ||
      v->type == Value::PERCENTAGE ||
      v->type == Value::DIMENSION) {
    if (type == NUMBER) 
      setType(v);
    setValue(getValue() * ((NumberValue*)v)->getValue());
    return this;
  } else if (v->type == COLOR ||
             v->type == STRING) {
    return v->multiply(this);
  } else {
    throw new ValueException("Unsupported type.");
  }
}

Value* NumberValue::divide(Value* v) {
  if (v->type == Value::NUMBER ||
      v->type == Value::PERCENTAGE ||
      v->type == Value::DIMENSION) {
    if (type == NUMBER) 
      setType(v);
    setValue(getValue() / ((NumberValue*)v)->getValue());
    return this;
  } else
    throw new ValueException("You can only divide a number by a *number*.");
}

void NumberValue::setType(Value* v) {
  NumberValue* n;
  
  if (v->type == Value::NUMBER ||
      v->type == Value::PERCENTAGE ||
      v->type == Value::DIMENSION) {

    n = static_cast<NumberValue*>(v);
    
    type = n->type;
    if (n->type == DIMENSION)
      setUnit(n->getUnit());
    else if (n->type == PERCENTAGE) 
      tokens.front()->type = Token::PERCENTAGE;
    else if (n->type == NUMBER) {
      setUnit("");
      tokens.front()->type = Token::NUMBER;
    }
  }
}

double NumberValue::getValue() {
  string number;
  istringstream stm;
  double ret;
  char c;

  for (unsigned int i = 0; i < tokens.front()->str.size(); i++) {
    c = tokens.front()->str[i];
    if (!isdigit(c) && c != '.' && c != '-') {
      number = tokens.front()->str.substr(0, i);
      break;
    }
  }
  if (number == "")
    number = tokens.front()->str;
  stm.str(number);
  stm >>ret;
  return ret;
}
string NumberValue::getUnit () {
  char c;
  unsigned int i;
  
  for (i = 0; i < tokens.front()->str.size(); i++) {
    c = tokens.front()->str[i];
    if (!isdigit(c) && c != '.' && c != '-')
      return tokens.front()->str.substr(i);
  }
  return string("");
}

void NumberValue::setUnit(string unit) {
  ostringstream stm;
  stm << getValue();
  stm << unit;
  type = DIMENSION;
  tokens.front()->type = Token::DIMENSION;
  tokens.front()->str = stm.str();
}

void NumberValue::setValue(double d) {
  ostringstream stm;
  stm << d;
  if (type == DIMENSION)
    stm << getUnit();
  else if (type == PERCENTAGE)
    stm << "%";
  tokens.front()->str = stm.str();
}

void NumberValue::loadFunctions(FunctionLibrary* lib) {
  lib->push("unit", " U", &NumberValue::unit);
}

// DIMENSION unit(DIMENSION, UNIT)
Value* NumberValue::unit(vector<Value*> arguments) {
  if (arguments[0]->type == Value::NUMBER ||
      arguments[0]->type == Value::DIMENSION) {
    ((NumberValue*)arguments[0])
      ->setUnit(((UnitValue*)arguments[1])->getUnit());
    return arguments[0];
  } else 
    throw new ValueException("argument 1 has to be a number or dimension");
}
