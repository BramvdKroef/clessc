#include "less/value/Value.h"
#include "less/value/BooleanValue.h"

Value::Value() {
}

Value::~Value() {
}

void Value::setLocation(const Token& ref) {
  if (!tokens.empty())
    tokens.front().setLocation(ref);
}

const TokenList* Value::getTokens() const {
  return &tokens;
}

bool Value::operator==(const Value& v) const {
  return this->type == v.type && getTokens() == v.getTokens();
}
bool Value::operator!=(const Value& v) const {
  return !(*this == v);
}


bool Value::operator<(const Value& v) const {
  return this->type < v.type || (this->type == v.type &&
                                 getTokens() < v.getTokens());
}
bool Value::operator>(const Value& v) const {
  return  !(*this == v || *this < v);
}
bool Value::operator<=(const Value& v) const {
  return (*this == v || *this < v);
}
bool Value::operator>=(const Value& v) const {
  return !(*this < v);
}


const char* Value::typeToString(const Type& t) {
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
