#include "less/value/NumberFunctions.h"
#include "less/value/FunctionLibrary.h"
#include "less/value/Value.h"
#include "less/value/NumberValue.h"
#include "less/value/UnitValue.h"


void NumberFunctions::loadFunctions(FunctionLibrary& lib) {
  lib.push("unit", ".U?", &NumberFunctions::unit);
  lib.push("get-unit", ".", &NumberFunctions::get_unit);
  lib.push("isunit", "..", &NumberFunctions::is_unit);
  lib.push("ceil", ".", &NumberFunctions::ceil);
  lib.push("floor", ".", &NumberFunctions::floor);
  lib.push("percentage", "N", &NumberFunctions::percentage);
  lib.push("round", ".", &NumberFunctions::round);
  lib.push("sqrt", ".", &NumberFunctions::sqrt);
  lib.push("abs", ".", &NumberFunctions::abs);
  lib.push("sin", ".", &NumberFunctions::sin);
  lib.push("asin", "N", &NumberFunctions::asin);
  lib.push("cos", ".", &NumberFunctions::cos);
  lib.push("acos", "N", &NumberFunctions::acos);
  lib.push("tan", ".", &NumberFunctions::tan);
  lib.push("atan", "N", &NumberFunctions::atan);
  lib.push("pi", "", &NumberFunctions::pi);
  lib.push("pow", ".N", &NumberFunctions::pow);
  lib.push("mod", "..", &NumberFunctions::mod);
  lib.push("convert", "..", &NumberFunctions::convert);
  lib.push("min", "..+", &NumberFunctions::min);
  lib.push("max", "..+", &NumberFunctions::max);
  lib.push("isnumber", ".", &NumberFunctions::is_number);
  lib.push("isstring", ".", &NumberFunctions::is_string);
  lib.push("iscolor", ".", &NumberFunctions::is_color);
  lib.push("iskeyword", ".", &NumberFunctions::is_keyword);
  lib.push("isurl", ".", &NumberFunctions::is_url);
  lib.push("ispixel", ".", &NumberFunctions::is_pixel);
  lib.push("isem", ".", &NumberFunctions::is_em);
  lib.push("ispercentage", ".", &NumberFunctions::is_percentage);
}

// DIMENSION unit(DIMENSION, UNIT)
Value* NumberFunctions::unit(const vector<const Value*>& arguments) {
  NumberValue* ret;

  if (arguments[0]->type == Value::NUMBER ||
      arguments[0]->type == Value::DIMENSION) {
    ret = new NumberValue(((const NumberValue*)arguments[0])->getValue());

    if (arguments.size() > 1) {
      ret->setUnit(((const UnitValue*)arguments[1])->getUnit());
    } else
      ret->setUnit("");
    return ret;
  } else
    throw new ValueException(
                             "argument 1 has to be a number "
                             "or dimension",
                             *arguments[0]->getTokens());
}

Value* NumberFunctions::get_unit(const vector<const Value*>& arguments) {
  Token t("", Token::IDENTIFIER, 0, 0, NULL);
  const NumberValue* val;
  if (arguments[0]->type == Value::NUMBER ||
      arguments[0]->type == Value::DIMENSION) {
    val = (const NumberValue*)arguments[0];
    t = val->getUnit();
    t.setLocation(val->getTokens()->front());
  }
  
  return new UnitValue(t);
}

Value* NumberFunctions::is_unit(const vector<const Value*>& arguments) {
  bool ret = false;
  
  if ((arguments[0]->type == Value::NUMBER ||
       arguments[0]->type == Value::DIMENSION) &&
      arguments[1]->type == Value::UNIT) {
    
    ret = (((const NumberValue*)arguments[0])->getUnit() ==
           ((const UnitValue*)arguments[1])->getUnit());
    
  } else if (arguments[0]->type == Value::PERCENTAGE &&
             arguments[1]->type == Value::STRING) {
    ret = (((const StringValue*)arguments[1])->getString() == "%");
  }

  return new BooleanValue(ret);
}


Value* NumberFunctions::ceil(const vector<const Value*>& args) {
  NumberValue* n;

  if (!NumberValue::isNumber(*args[0]))
    throw new ValueException(
                             "ceil() only works on numeric "
                             "values",
                             *args[0]->getTokens());

  n = new NumberValue(*static_cast<const NumberValue*>(args[0]));

  double val = n->getValue();
  n->setValue(std::ceil(val));

  return n;
}
Value* NumberFunctions::floor(const vector<const Value*>& args) {
  NumberValue* n;

  if (!NumberValue::isNumber(*args[0]))
    throw new ValueException(
                             "floor() only works on numeric "
                             "values",
                             *args[0]->getTokens());

  n = new NumberValue(*static_cast<const NumberValue*>(args[0]));

  double val = n->getValue();
  n->setValue(std::floor(val));
  return n;
}
Value* NumberFunctions::percentage(const vector<const Value*>& args) {
  const NumberValue* val = (const NumberValue*)args[0];
  return new NumberValue(val->getValue() * 100, Token::PERCENTAGE, NULL);
}

Value* NumberFunctions::round(const vector<const Value*>& args) {
  if (!NumberValue::isNumber(*args[0]))
    throw new ValueException(
                             "round() only works on numeric "
                             "values",
                             *args[0]->getTokens());

  NumberValue* n = new NumberValue(*(const NumberValue*)args[0]);
  double val = n->getValue();
  double decimalplaces = 0;

  if (args.size() > 1)
    decimalplaces = ((const NumberValue*)args[1])->getValue();

  val = val * std::pow(10, decimalplaces);
  val = std::floor(val + 0.5);
  val = val / std::pow(10, decimalplaces);
  n->setValue(val);
  return n;
}
Value* NumberFunctions::sqrt(const vector<const Value*>& args) {
  if (!NumberValue::isNumber(*args[0]))
    throw new ValueException(
                             "sqrt() only works on numeric "
                             "values",
                             *args[0]->getTokens());

  NumberValue* n = new NumberValue(*(const NumberValue*)args[0]);
  n->setValue(std::sqrt(n->getValue()));
  return n;
}
Value* NumberFunctions::abs(const vector<const Value*>& args) {
  if (!NumberValue::isNumber(*args[0]))
    throw new ValueException(
                             "abs() only works on numeric "
                             "values",
                             *args[0]->getTokens());

  NumberValue* n = new NumberValue(*(const NumberValue*)args[0]);
  n->setValue(fabs(n->getValue()));
  return n;
}

Value* NumberFunctions::sin(const vector<const Value*>& args) {
  if (args[0]->type != Value::NUMBER && args[0]->type != Value::DIMENSION) {
    throw new ValueException(
                             "sin() only works on numbers "
                             "or dimensions",
                             *args[0]->getTokens());
  }

  NumberValue* n = new NumberValue(*(const NumberValue*)args[0]);
  double val = n->getValue();
  std::string unit;

  if (n->type == Value::DIMENSION) {
    unit = n->getUnit();
    if (unit.compare("rad") != 0 && unit.compare("deg") != 0 &&
        unit.compare("grad") != 0 && unit.compare("turn") != 0) {
      throw new ValueException(
                               "sin() requires rad, deg, "
                               "grad or turn units.",
                               *args[0]->getTokens());
    }
    val = UnitValue::angleToRad(val, unit);
  }

  n->setValue(std::sin(val));
  n->type = Value::NUMBER;
  n->setUnit("");
  return n;
}
Value* NumberFunctions::asin(const vector<const Value*>& args) {
  NumberValue* n = new NumberValue(*(const NumberValue*)args[0]);

  n->setValue(std::asin(n->getValue()));
  n->setUnit("rad");
  n->type = Value::DIMENSION;
  return n;
}
Value* NumberFunctions::cos(const vector<const Value*>& args) {
  if (args[0]->type != Value::NUMBER && args[0]->type != Value::DIMENSION) {
    throw new ValueException(
                             "cos() only works on numbers "
                             "or dimensions",
                             *args[0]->getTokens());
  }
  NumberValue* n = new NumberValue(*(const NumberValue*)args[0]);
  double val = n->getValue();
  std::string unit;

  if (n->type == Value::DIMENSION) {
    unit = n->getUnit();
    if (unit.compare("rad") != 0 && unit.compare("deg") != 0 &&
        unit.compare("grad") != 0 && unit.compare("turn") != 0) {
      throw new ValueException(
                               "cos() requires rad, deg, "
                               "grad or turn units.",
                               *args[0]->getTokens());
    }
    val = UnitValue::angleToRad(val, unit);
  }

  n->setValue(std::cos(val));
  n->type = Value::NUMBER;
  n->setUnit("");
  return n;
}
Value* NumberFunctions::acos(const vector<const Value*>& args) {
  NumberValue* n = new NumberValue(*(const NumberValue*)args[0]);

  n->setValue(std::acos(n->getValue()));
  n->setUnit("rad");
  n->type = Value::DIMENSION;
  return n;
}
Value* NumberFunctions::tan(const vector<const Value*>& args) {
  if (args[0]->type != Value::NUMBER && args[0]->type != Value::DIMENSION) {
    throw new ValueException(
                             "tan() only works on numbers "
                             "or dimensions",
                             *args[0]->getTokens());
  }
  NumberValue* n = new NumberValue(*(const NumberValue*)args[0]);
  double val = n->getValue();
  std::string unit;

  if (n->type == Value::DIMENSION) {
    unit = n->getUnit();
    if (unit.compare("rad") != 0 && unit.compare("deg") != 0 &&
        unit.compare("grad") != 0 && unit.compare("turn") != 0) {
      throw new ValueException(
                               "ta() requires rad, deg, "
                               "grad or turn units.",
                               *args[0]->getTokens());
    }
    val = UnitValue::angleToRad(val, unit);
  }

  n->setValue(std::tan(val));
  n->type = Value::NUMBER;
  n->setUnit("");
  return n;
}
Value* NumberFunctions::atan(const vector<const Value*>& args) {
  NumberValue* n = new NumberValue(*(const NumberValue*)args[0]);

  n->setValue(std::atan(n->getValue()));
  n->setUnit("rad");
  n->type = Value::DIMENSION;
  return n;
}
Value* NumberFunctions::pi(const vector<const Value*>& args) {
  (void)args;
  return new NumberValue(3.141592653589793);
}
Value* NumberFunctions::pow(const vector<const Value*>& args) {
  if (!NumberValue::isNumber(*args[0]))
    throw new ValueException("pow() only works on numeric values",
                             *args[0]->getTokens());

  NumberValue* n = new NumberValue(*(const NumberValue*)args[0]);
  double exp = ((const NumberValue*)args[1])->getValue();

  n->setValue(std::pow(n->getValue(), exp));
  return n;
}
Value* NumberFunctions::mod(const vector<const Value*>& args) {
  if (!NumberValue::isNumber(*args[0]) || !NumberValue::isNumber(*args[1]))
    throw new ValueException("mod() only works on numeric values",
                             *args[0]->getTokens());

  NumberValue* n = new NumberValue(*(const NumberValue*)args[0]);
  double val2 = ((NumberValue*)args[1])->getValue();

  n->setValue(std::fmod(n->getValue(), val2));
  return n;
}
Value* NumberFunctions::convert(const vector<const Value*>& args) {
  if (!NumberValue::isNumber(*args[0]))
    throw new ValueException("convert() only works on numeric values",
                             *args[0]->getTokens());
  if (args[1]->type != Value::STRING && args[1]->type != Value::UNIT) {
    throw new ValueException(
                             "convert() requires a unit \
(or unit as a string)",
                             *args[1]->getTokens());
  }

  NumberValue* n = new NumberValue(*(const NumberValue*)args[0]);
  std::string unit;

  if (args[1]->type == Value::STRING)
    unit = ((const StringValue*)args[1])->getString();
  else
    unit.append(((const UnitValue*)args[1])->getUnit());

  n->setValue(n->convert(unit));
  n->setUnit(unit);
  return n;
}

Value* NumberFunctions::min(const vector<const Value*>& arguments) {
  const NumberValue* min = NULL;
  vector<const Value *>::const_iterator it;
  
  for (it = arguments.begin(); it != arguments.end(); it++) {
    if (!NumberValue::isNumber(**it)) {
      throw new ValueException("arguments should be numbers",
                               *(*it)->getTokens());
    } else if (min == NULL) {
      min = (const NumberValue*)*it;
    } else {
      if (((const NumberValue*)*it)->convert(min->getUnit()) <
          min->getValue()) {
        min = (const NumberValue*)*it;
      }
    }
  }

  return new NumberValue(*min);
}

Value* NumberFunctions::max(const vector<const Value*>& arguments) {
  const NumberValue* max = NULL;
  vector<const Value *>::const_iterator it;
  
  for (it = arguments.begin(); it != arguments.end(); it++) {
    if (!NumberValue::isNumber(**it)) {
      throw new ValueException("arguments should be numbers",
                               *(*it)->getTokens());
    } else if (max == NULL) {
      max = (const NumberValue*)*it;
    } else {
      if (((const NumberValue*)*it)->convert(max->getUnit()) >
          max->getValue()) {
        max = (const NumberValue*)*it;
      }
    }
  }

  return new NumberValue(*max);
}

Value* NumberFunctions::is_number(const vector<const Value*>& arguments) {
  return new BooleanValue(NumberValue::isNumber(*arguments[0]));
}

Value* NumberFunctions::is_string(const vector<const Value*>& arguments) {
  return new BooleanValue(arguments[0]->type == Value::STRING &&
                          ((const StringValue*)arguments[0])
                          ->getQuotes() == true);
}

Value* NumberFunctions::is_color(const vector<const Value*>& arguments) {
  return new BooleanValue(arguments[0]->type == Value::COLOR);
}

Value* NumberFunctions::is_keyword(const vector<const Value*>& arguments) {
  return new BooleanValue(arguments[0]->type == Value::STRING &&
                          ((const StringValue*)arguments[0])
                          ->getQuotes() == false);
}

Value* NumberFunctions::is_url(const vector<const Value*>& arguments) {
  return new BooleanValue(arguments[0]->type == Value::URL);
}

Value* NumberFunctions::is_pixel(const vector<const Value*>& arguments) {
  return new BooleanValue(arguments[0]->type == Value::DIMENSION &&
                          ((const NumberValue*)arguments[0])
                          ->getUnit() == "px");
}

Value* NumberFunctions::is_em(const vector<const Value*>& arguments) {
  return new BooleanValue(arguments[0]->type == Value::DIMENSION &&
                          ((const NumberValue*)arguments[0])
                          ->getUnit() == "em");
}

Value* NumberFunctions::is_percentage(const vector<const Value*>& arguments) {
  return new BooleanValue(arguments[0]->type == Value::PERCENTAGE);
}
