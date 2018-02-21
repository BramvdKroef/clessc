#include "less/value/NumberValue.h"
#include "less/value/FunctionLibrary.h"

NumberValue::NumberValue(const Token& token) {
  tokens.push_back(token);

  switch (token.type) {
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
      throw new ValueException(
          "Token used for Value that isn't a "
          "number, percentage or dimension",
          *this->getTokens());
  }
}
NumberValue::NumberValue(double value) {
  tokens.push_back(Token("", Token::NUMBER, 0, 0, "generated"));
  type = NUMBER;
  setValue(value);
}
NumberValue::NumberValue(double value,
                         Token::Type type,
                         const std::string* unit) {
  if (type != Token::NUMBER && type != Token::PERCENTAGE &&
      type != Token::DIMENSION) {
    throw new ValueException(
        "Type can only be number, "
        "percentage or dimension.",
        *this->getTokens());
  }
  if (type == Token::DIMENSION && unit == NULL)
    throw new ValueException("Dimension requires a unit.", *this->getTokens());

  tokens.push_back(Token("", type, 0, 0, "generated"));

  switch (type) {
    case Token::NUMBER:
      this->type = NUMBER;
      break;
    case Token::PERCENTAGE:
      this->type = PERCENTAGE;
      break;
    case Token::DIMENSION:
      this->type = DIMENSION;
      break;
    default:
      break;
  }

  ostringstream stm;
  stm << setprecision(10) << value;
  if (type == Token::DIMENSION)
    stm << *unit;
  else if (type == Token::PERCENTAGE)
    stm << "%";
  tokens.front() = stm.str();
}

NumberValue::NumberValue(const NumberValue& n) : Value() {
  tokens.push_back(n.getTokens()->front());
  this->type = n.type;
}

NumberValue::~NumberValue() {
}

void NumberValue::verifyUnits(const NumberValue& n) {
  if (type == Value::DIMENSION && n.type == Value::DIMENSION &&
      getUnit().compare(n.getUnit()) != 0) {
    setValue(convert(n.getUnit()));
    setUnit(n.getUnit());
  }
}

double NumberValue::convert(const std::string& unit) const {
  UnitValue::UnitGroup group = UnitValue::getUnitGroup(unit);
  double value = getValue();

  if (UnitValue::getUnitGroup(getUnit()) == group) {
    switch (group) {
      case UnitValue::LENGTH:
        value = UnitValue::lengthToPx(value, getUnit());
        return UnitValue::pxToLength(value, unit);

      case UnitValue::TIME:
        value = UnitValue::timeToMs(value, getUnit());
        return UnitValue::msToTime(value, unit);

      case UnitValue::ANGLE:
        value = UnitValue::angleToRad(value, getUnit());
        return UnitValue::radToAngle(value, unit);

      default:
        return value;
    }
  } else
    throw new ValueException(
        "Can't do math on dimensions with "
        "different units.",
        *this->getTokens());
}

Value* NumberValue::operator+(const Value& v) const {
  const NumberValue* n;
  const StringValue* s;
  NumberValue* nret;
  StringValue* sret;

  if (isNumber(v)) {
    n = static_cast<const NumberValue*>(&v);
    nret = new NumberValue(getValue());
    if (type == NUMBER)
      nret->setType(*n);
    else {
      nret->setType(*this);
      nret->verifyUnits(*n);
    }
    nret->setValue(nret->getValue() + n->getValue());

    return nret;

  } else if (v.type == COLOR) {
    return (*static_cast<const Color*>(&v)) + (*this);

  } else if (v.type == STRING) {
    s = static_cast<const StringValue*>(&v);
    sret = new StringValue(*this, s->getQuotes());
    sret->append(v);
    return sret;

  } else {
    throw new ValueException("Unsupported type.", *this->getTokens());
  }
}
Value* NumberValue::operator-(const Value& v) const {
  const NumberValue* n;
  NumberValue* ret;

  if (isNumber(v)) {
    n = static_cast<const NumberValue*>(&v);
    ret = new NumberValue(getValue());

    if (type == NUMBER)
      ret->setType(*n);
    else {
      ret->setType(*this);
      ret->verifyUnits(*n);
    }

    ret->setValue(ret->getValue() - n->getValue());

    return ret;
  } else
    throw new ValueException(
        "You can only substract a "
        "*number* from a number.",
        *this->getTokens());
}
Value* NumberValue::operator*(const Value& v) const {
  const NumberValue* n;
  NumberValue* ret;

  if (isNumber(v)) {
    n = static_cast<const NumberValue*>(&v);
    ret = new NumberValue(getValue());

    if (type == NUMBER)
      ret->setType(*n);
    else {
      ret->setType(*this);
      ret->verifyUnits(*n);
    }
    ret->setValue(ret->getValue() * n->getValue());
    return ret;

  } else if (v.type == COLOR) {
    return (*static_cast<const Color*>(&v)) * (*this);

  } else if (v.type == STRING) {
    return (*static_cast<const StringValue*>(&v)) * (*this);

  } else {
    throw new ValueException("Unsupported type.", *this->getTokens());
  }
}

Value* NumberValue::operator/(const Value& v) const {
  const NumberValue* n;
  NumberValue* ret;

  if (isNumber(v)) {
    n = static_cast<const NumberValue*>(&v);
    ret = new NumberValue(getValue());

    if (type == NUMBER)
      ret->setType(*n);
    else {
      ret->setType(*this);
      ret->verifyUnits(*n);
    }
    ret->setValue(ret->getValue() / n->getValue());

    return ret;
  } else
    throw new ValueException(
        "You can only divide a number "
        "by a *number*.",
        *this->getTokens());
}

bool NumberValue::operator==(const Value& v) const {
  const NumberValue* n;
  const BooleanValue* b;

  if (isNumber(v)) {
    n = static_cast<const NumberValue*>(&v);
    return convert(n->getUnit()) == n->getValue();
  } else if (v.type == BOOLEAN) {
    // any number is falsy.
    b = static_cast<const BooleanValue*>(&v);
    return (false == b->getValue());
  } else {
    throw new ValueException(
        "You can only compare a number "
        "with a *number*.",
        *this->getTokens());
  }
}

bool NumberValue::operator<(const Value& v) const {
  const NumberValue* n;
  const BooleanValue* b;

  if (isNumber(v)) {
    n = static_cast<const NumberValue*>(&v);
    return (convert(n->getUnit()) < n->getValue());
  } else if (v.type == BOOLEAN) {
    b = static_cast<const BooleanValue*>(&v);
    return b->getValue();
  } else {
    throw new ValueException(
        "You can only compare a number "
        "with a *number*.",
        *this->getTokens());
  }
}

void NumberValue::setType(const NumberValue& n) {
  type = n.type;
  if (n.type == DIMENSION)
    setUnit(n.getUnit());
  else if (n.type == PERCENTAGE)
    tokens.front().type = Token::PERCENTAGE;
  else if (n.type == NUMBER) {
    setUnit("");
  }
}

double NumberValue::getValue() const {
  string number;
  istringstream stm;
  double ret;
  char c;

  for (unsigned int i = 0; i < tokens.front().size(); i++) {
    c = tokens.front()[i];
    if (!isdigit(c) && c != '.' && c != '-') {
      number = tokens.front().substr(0, i);
      break;
    }
  }
  if (number == "")
    number = tokens.front();
  stm.str(number);
  stm >> ret;
  return ret;
}
string NumberValue::getUnit() const {
  char c;
  unsigned int i;

  for (i = 0; i < tokens.front().size(); i++) {
    c = tokens.front()[i];
    if (!isdigit(c) && c != '.' && c != '-')
      return tokens.front().substr(i);
  }
  return string("");
}

void NumberValue::setUnit(string unit) {
  ostringstream stm;
  stm << setprecision(10) << getValue();
  stm << unit;
  tokens.front() = stm.str();

  if (unit.length() == 0) {
    type = NUMBER;
    tokens.front().type = Token::NUMBER;
  } else {
    type = DIMENSION;
    tokens.front().type = Token::DIMENSION;
  }
}

void NumberValue::setValue(double d) {
  ostringstream stm;
  stm << setprecision(10) << d;
  if (type == DIMENSION)
    stm << getUnit();
  else if (type == PERCENTAGE)
    stm << "%";
  tokens.front() = stm.str();
}

bool NumberValue::isNumber(const Value& val) {
  return (val.type == Value::NUMBER || val.type == Value::DIMENSION ||
          val.type == Value::PERCENTAGE);
}

