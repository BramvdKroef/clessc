#include "BooleanValue.h"

BooleanValue::BooleanValue(bool value) {
  tokens.push(new Token("true", Token::IDENTIFIER));
  setValue(value);
  type = Value::BOOLEAN;
}

BooleanValue::~BooleanValue() {
}

bool BooleanValue::getValue() {
  return value;
}
void BooleanValue::setValue(bool value) {
  this->value = value;
  tokens.front()->str = value ? "true" : "false";
}

Value* BooleanValue::add(Value* v) {
  Token* t;
  StringValue* s;

  if (v->type == STRING) {
    t = this->tokens.front()->clone();
    t->type = Token::STRING;
    s = new StringValue(t, ((StringValue*)v)->getQuotes());
    s->add(v);
    return s;
  }
  throw new ValueException("Can't add boolean types.");
}
Value* BooleanValue::substract(Value* v) {
  (void)v;
  throw new ValueException("Can't substract boolean types.");
}
Value* BooleanValue::multiply(Value* v) {
  (void)v;
  throw new ValueException("Can't multiply boolean types.");
}
Value* BooleanValue::divide(Value* v) {
  (void)v;
  throw new ValueException("Can't divide boolean types.");
}
int BooleanValue::compare(Value* v) {
  if (v->type == BOOLEAN) {
    return (getValue() ? 1 : 0) -
      (((BooleanValue*)v)->getValue() ? 1 : 0);
  } else {
    return (getValue() ? 1 : 0);
  }
}
