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

void NumberValue::verifyUnits(NumberValue* v) {
  if (type == Value::DIMENSION &&
      v->type == Value::DIMENSION &&
      getUnit().compare(v->getUnit()) != 0) {
    
    if (v->convert(getUnit())) {
      return;
    } else
      throw new ValueException("Can't do math on dimensions with different units.");
  }
}

bool NumberValue::convert(string unit) {
  UnitValue::UnitGroup group = UnitValue::getUnitGroup(unit);
  double value = getValue();
  
  if (UnitValue::getUnitGroup(getUnit()) == group) {
    
    switch(group) {
    case UnitValue::LENGTH:
      value = UnitValue::lengthToPx(value, getUnit());
      setValue(UnitValue::pxToLength(value, unit));
      setUnit(unit);
      return true;

    case UnitValue::TIME:
      value = UnitValue::timeToMs(value, getUnit());
      setValue(UnitValue::msToTime(value, unit));
      setUnit(unit);
      return true;

    case UnitValue::ANGLE:
      value = UnitValue::angleToRad(value, getUnit());
      setValue(UnitValue::radToAngle(value, unit));
      setUnit(unit);
      return true;
      
    default:
      break;
    }
  }
  return false;
}

Value* NumberValue::add(Value* v) {
  Token* t;
  StringValue* s;
  
  if (isNumber(v)) {
    if (type == NUMBER) 
      setType(v);
    else
      verifyUnits((NumberValue*)v);
    
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
  if (isNumber(v)) {
    if (type == NUMBER) 
      setType(v);
    else
      verifyUnits((NumberValue*)v);
    
    setValue(getValue() - ((NumberValue*)v)->getValue());
    return this;
  } else
    throw new ValueException("You can only substract a *number* from a number.");
}
Value* NumberValue::multiply(Value* v) {
  if (isNumber(v)) {
    if (type == NUMBER) 
      setType(v);
    else
      verifyUnits((NumberValue*)v);
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
  if (isNumber(v)) {
    if (type == NUMBER) 
      setType(v);
    else
      verifyUnits((NumberValue*)v);
    setValue(getValue() / ((NumberValue*)v)->getValue());
    return this;
  } else
    throw new ValueException("You can only divide a number by a *number*.");
}
int NumberValue::compare(Value* v) {
  if (isNumber(v)) {
    verifyUnits((NumberValue*)v);
    return getValue() - ((NumberValue*)v)->getValue();
  } else {
    throw new ValueException("You can only compare a number with a *number*.");
  }
}

void NumberValue::setType(Value* v) {
  NumberValue* n;
  
  if (isNumber(v)) {

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

bool NumberValue::isNumber(Value* val) {
  return (val->type == Value::NUMBER ||
          val->type == Value::DIMENSION ||
          val->type == Value::PERCENTAGE);
}

void NumberValue::loadFunctions(FunctionLibrary* lib) {
  lib->push("unit", ".U?", &NumberValue::unit);
  lib->push("ceil", ".", &NumberValue::ceil);
  lib->push("floor", ".", &NumberValue::floor);
  lib->push("percentage", "N", &NumberValue::percentage);
  lib->push("round", ".", &NumberValue::round);
  lib->push("sqrt", ".", &NumberValue::sqrt);
  lib->push("abs", ".", &NumberValue::abs);
  lib->push("sin", ".", &NumberValue::sin);
  lib->push("asin", "N", &NumberValue::asin);
  lib->push("cos", ".", &NumberValue::cos);
  lib->push("acos", "N", &NumberValue::acos);
  lib->push("tan", ".", &NumberValue::tan);
  lib->push("atan", "N", &NumberValue::atan);
  lib->push("pi", "", &NumberValue::pi);
  lib->push("pow", ".N", &NumberValue::pow);
  lib->push("mod", "..", &NumberValue::mod);
  lib->push("convert", "..", &NumberValue::convert);
}

// DIMENSION unit(DIMENSION, UNIT)
Value* NumberValue::unit(vector<Value*> arguments) {
  if (arguments[0]->type == Value::NUMBER ||
      arguments[0]->type == Value::DIMENSION) {
    if (arguments.size() > 1) {
      ((NumberValue*)arguments[0])
        ->setUnit(((UnitValue*)arguments[1])->getUnit());
    } else
      ((NumberValue*)arguments[0])->setUnit("");
    return arguments[0];
  } else 
    throw new ValueException("argument 1 has to be a number or dimension");
}
Value* NumberValue::ceil(vector<Value*> args) {
  if (!NumberValue::isNumber(args[0]))
    throw new ValueException("ceil() only works on numeric values");
  
  double val = ((NumberValue*)args[0])->getValue();
  ((NumberValue*)args[0])->setValue(std::ceil(val));
  return args[0];
}   
Value* NumberValue::floor(vector<Value*> args) {
  if (!NumberValue::isNumber(args[0]))
    throw new ValueException("floor() only works on numeric values");

  double val = ((NumberValue*)args[0])->getValue();
  ((NumberValue*)args[0])->setValue(std::floor(val));
  return args[0];
}  
Value* NumberValue::percentage(vector<Value*> args) {
  double val = ((NumberValue*)args[0])->getValue();
  ((NumberValue*)args[0])->setValue(val * 100);
  ((NumberValue*)args[0])->type = Value::PERCENTAGE;
  return args[0];
}
Value* NumberValue::round(vector<Value*> args) {
  if (!NumberValue::isNumber(args[0]))
    throw new ValueException("round() only works on numeric values");

  double val = ((NumberValue*)args[0])->getValue();
  double decimalplaces = 0;
  
  if (args.size() > 1)
    decimalplaces = ((NumberValue*)args[0])->getValue();

  val = val * std::pow(10, decimalplaces);
  val = std::floor(val + 0.5);
  val = val / std::pow(10, decimalplaces);
  ((NumberValue*)args[0])->setValue(val);
  return args[0];
}  
Value* NumberValue::sqrt(vector<Value*> args) {
  if (!NumberValue::isNumber(args[0]))
    throw new ValueException("sqrt() only works on numeric values");

  double val = ((NumberValue*)args[0])->getValue();
  ((NumberValue*)args[0])->setValue(std::sqrt(val));
  return args[0];
}   
Value* NumberValue::abs(vector<Value*> args) {
  if (!NumberValue::isNumber(args[0]))
    throw new ValueException("abs() only works on numeric values");

  double val = ((NumberValue*)args[0])->getValue();
  ((NumberValue*)args[0])->setValue(fabs(val));
  return args[0];
}

Value* NumberValue::sin(vector<Value*> args) {
  if (args[0]->type != Value::NUMBER &&
      args[0]->type != Value::DIMENSION) {
    throw new ValueException("sin() only works on numbers or dimensions");
  }
  
  NumberValue* arg = ((NumberValue*)args[0]);
  double val = arg->getValue();

  if (arg->type == Value::DIMENSION) {
    if(arg->getUnit().compare("rad") != 0 &&
       arg->getUnit().compare("deg") != 0 &&
       arg->getUnit().compare("grad") != 0 &&
       arg->getUnit().compare("turn") != 0) {
      throw new ValueException("sin() requires rad, deg, grad or turn units.");
    }
    val = UnitValue::angleToRad(val, arg->getUnit());
  }
    
  arg->setValue(std::sin(val));
  arg->type = Value::NUMBER;
  arg->setUnit("");
  return arg;
}    
Value* NumberValue::asin(vector<Value*> args) {
  NumberValue* arg = ((NumberValue*)args[0]);

  arg->setValue(std::asin(arg->getValue()));
  arg->setUnit("rad");
  arg->type = Value::DIMENSION;
  return arg;
}   
Value* NumberValue::cos(vector<Value*> args) {
  if (args[0]->type != Value::NUMBER &&
      args[0]->type != Value::DIMENSION) {
    throw new ValueException("cos() only works on numbers or dimensions");
  }
  NumberValue* arg = ((NumberValue*)args[0]);
  double val = arg->getValue();
    
  if (arg->type == Value::DIMENSION) {
    if(arg->getUnit().compare("rad") != 0 &&
       arg->getUnit().compare("deg") != 0 &&
       arg->getUnit().compare("grad") != 0 &&
       arg->getUnit().compare("turn") != 0) {
      throw new ValueException("cos() requires rad, deg, grad or turn units.");
    }
    val = UnitValue::angleToRad(val, arg->getUnit());
  }

  arg->setValue(std::cos(val));
  arg->type = Value::NUMBER;
  arg->setUnit("");
  return arg;
}    
Value* NumberValue::acos(vector<Value*> args) {
  NumberValue* arg = ((NumberValue*)args[0]);

  arg->setValue(std::acos(arg->getValue()));
  arg->setUnit("rad");
  arg->type = Value::DIMENSION;
  return arg;
}   
Value* NumberValue::tan(vector<Value*> args) {
  if (args[0]->type != Value::NUMBER &&
      args[0]->type != Value::DIMENSION) {
    throw new ValueException("tan() only works on numbers or dimensions");
  }
  NumberValue* arg = ((NumberValue*)args[0]);
  double val = arg->getValue();
    
  if (arg->type == Value::DIMENSION) {
    if(arg->getUnit().compare("rad") != 0 &&
       arg->getUnit().compare("deg") != 0 &&
       arg->getUnit().compare("grad") != 0 &&
       arg->getUnit().compare("turn") != 0) {
      throw new ValueException("tan() requires rad, deg, grad or turn units.");
    }
    val = UnitValue::angleToRad(val, arg->getUnit());
  }

  arg->setValue(std::tan(val));
  arg->type = Value::NUMBER;
  arg->setUnit("");
  return arg;
}    
Value* NumberValue::atan(vector<Value*> args) {
  NumberValue* arg = ((NumberValue*)args[0]);

  arg->setValue(std::atan(arg->getValue()));
  arg->setUnit("rad");
  arg->type = Value::DIMENSION;
  return arg;
}   
Value* NumberValue::pi(vector<Value*> args) {
  (void)args;
  return new NumberValue(new Token("3.141592653589793", Token::NUMBER));
}
Value* NumberValue::pow(vector<Value*> args) {
  if (!NumberValue::isNumber(args[0]))
    throw new ValueException("pow() only works on numeric values");

  double val = ((NumberValue*)args[0])->getValue();
  double exp = ((NumberValue*)args[1])->getValue();
  ((NumberValue*)args[0])->setValue(std::pow(val, exp));
  return args[0];
}    
Value* NumberValue::mod(vector<Value*> args) {
  if (!NumberValue::isNumber(args[0]) ||
      !NumberValue::isNumber(args[1]))
    throw new ValueException("mod() only works on numeric values");

  double val1 = ((NumberValue*)args[0])->getValue();
  double val2 = ((NumberValue*)args[1])->getValue();
  ((NumberValue*)args[0])->setValue(std::fmod(val1, val2));
  return args[0];
}    
Value* NumberValue::convert(vector<Value*> args) {
  if (!NumberValue::isNumber(args[0]))
    throw new ValueException("convert() only works on numeric values");  
  if (args[1]->type != Value::STRING &&
      args[1]->type != Value::UNIT) {
    throw new ValueException("convert() requires a unit \
(or unit as a string)");
  }
  
  NumberValue* val = (NumberValue*)args[0];
  string unit;
    
  if (args[1]->type == Value::STRING)
    unit = ((StringValue*)args[1])->getString();
  else
    unit.append(((UnitValue*)args[1])->getUnit());

  val->convert(unit);
  return val;
}
