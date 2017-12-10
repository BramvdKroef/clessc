#include "less/value/BooleanValue.h"

BooleanValue::BooleanValue(bool value) {
  Token t("true", Token::IDENTIFIER, 0, 0, "generated");
  tokens.push_back(t);
  setValue(value);
  type = Value::BOOLEAN;
}
BooleanValue::BooleanValue(const Token &t, bool value) {
  tokens.push_back(t);
  setValue(value);
  type = Value::BOOLEAN;
}

BooleanValue::~BooleanValue() {
}

bool BooleanValue::getValue() const {
  return value;
}
void BooleanValue::setValue(bool value) {
  this->value = value;
  tokens.front() = value ? "true" : "false";
}

Value *BooleanValue::add(const Value &v) const {
  Token t;
  const StringValue *str;
  StringValue *ret;

  if (v.type == STRING) {
    str = static_cast<const StringValue *>(&v);
    ret = new StringValue(*this, str->getQuotes());
    ret->append(v);
    return ret;
  }

  throw new ValueException("Can't add boolean types.", *this->getTokens());
}
Value *BooleanValue::substract(const Value &v) const {
  (void)v;
  throw new ValueException("Can't substract boolean types.",
                           *this->getTokens());
}
Value *BooleanValue::multiply(const Value &v) const {
  (void)v;
  throw new ValueException("Can't multiply boolean types.", *this->getTokens());
}
Value *BooleanValue::divide(const Value &v) const {
  (void)v;
  throw new ValueException("Can't divide boolean types.", *this->getTokens());
}
BooleanValue *BooleanValue::equals(const Value &v) const {
  const BooleanValue *b;

  if (v.type == BOOLEAN) {
    b = static_cast<const BooleanValue *>(&v);
    return new BooleanValue(getValue() == b->getValue());
  } else {
    return new BooleanValue(false);
  }
}

BooleanValue *BooleanValue::lessThan(const Value &v) const {
  const BooleanValue *b;

  if (v.type == BOOLEAN) {
    b = static_cast<const BooleanValue *>(&v);
    return new BooleanValue(!getValue() && b->getValue());
  } else {
    return new BooleanValue(!getValue());
  }
}
