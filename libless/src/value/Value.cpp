#include "less/value/Value.h"
#include "less/value/BooleanValue.h"

Value::Value() {
}

Value::~Value() {
}

void Value::setLocation(const Token& ref) {
  tokens.front().setLocation(ref);
}
  
const TokenList* Value::getTokens() const{
  return &tokens;
}

BooleanValue* Value::lessThan(const Value &v) const {
  return new BooleanValue(this->type < v.type ||
                          (this->type == v.type &&
                           getTokens() < v.getTokens()));
}


BooleanValue* Value::equals(const Value &v) const {
  return new BooleanValue(this->type == v.type &&
                          getTokens() == v.getTokens());
}

BooleanValue* Value::greaterThan(const Value &v) const {
  BooleanValue* ret = this->equals(v);

  if (ret->getValue()) {
    ret->setValue(false);
    return ret;
  }
  delete ret;
  
  ret = this->lessThan(v);
  ret->setValue(!ret->getValue());

  return ret;
}
BooleanValue* Value::lessThanEquals(const Value &v) const {
  BooleanValue* ret = this->equals(v);

  if (ret->getValue())
    return ret;

  delete ret;

  ret = this->lessThan(v);
  return ret;
}
BooleanValue* Value::greaterThanEquals(const Value &v) const {
  BooleanValue* ret = this->equals(v);

  if (ret->getValue())
    return ret;

  delete ret;

  ret = this->lessThan(v);
  ret->setValue(!ret->getValue());
  return ret;
}

const char* Value::typeToString(const Type &t) {
  switch (t) {
  case NUMBER:
    return "Number";
  case PERCENTAGE:
    return "Percentage";
  case DIMENSION:
    return "Dimension";
  case COLOR:
    return "Color";
  case STRING:
    return "String";
  case UNIT:
    return "Unit";
  case BOOLEAN:
    return "Boolean";
  case URL:
    return "URL";
  }
  return "Undefined";
}
Value::Type Value::codeToType(const char code) {
  switch (code) {
  case 'N':
    return NUMBER;
  case 'P':
    return PERCENTAGE;
  case 'D':
    return DIMENSION;
  case 'C':
    return COLOR;
  case 'U':
    return UNIT;
  case 'S':
    return STRING;
  case 'B':
    return BOOLEAN;
  case 'R':
    return URL;
  default:
    return NUMBER;
  }
}

