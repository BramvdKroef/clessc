#include "BooleanValue.h"

BooleanValue::BooleanValue(bool value) {
  Token t("true", Token::IDENTIFIER);
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

Value* BooleanValue::add(const Value &v) {
  Token t;
  const StringValue* s;
  StringValue* ret;

  if (v.type == STRING) {
    t = this->tokens.front();
    t.type = Token::STRING;
    s = static_cast<const StringValue*>(&v);
    ret = new StringValue(t, s->getQuotes());
    return ret->add(v);
  }
  throw new ValueException("Can't add boolean types.");
}
Value* BooleanValue::substract(const Value &v) {
  (void)v;
  throw new ValueException("Can't substract boolean types.");
}
Value* BooleanValue::multiply(const Value &v) {
  (void)v;
  throw new ValueException("Can't multiply boolean types.");
}
Value* BooleanValue::divide(const Value &v) {
  (void)v;
  throw new ValueException("Can't divide boolean types.");
}
int BooleanValue::compare(const Value &v) {
  const BooleanValue *b;
  
  if (v.type == BOOLEAN) {
    b = static_cast<const BooleanValue*>(&v);
    return (getValue() ? 1 : 0) -
      (b->getValue() ? 1 : 0);
  } else {
    return (getValue() ? 1 : 0);
  }
}
