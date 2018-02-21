#include "less/value/StringValue.h"
#include "less/value/FunctionLibrary.h"

StringValue::StringValue(const Token& token, bool quotes) {
  type = Value::STRING;

  tokens.push_back(token);
  this->quotes = quotes;
  setString(token);
}

StringValue::StringValue(const std::string& str, bool quotes) {
  Token token(str, Token::STRING, 0, 0, "generated");

  type = Value::STRING;
  tokens.push_back(token);
  this->quotes = quotes;
  setString(token);
}

StringValue::StringValue(const StringValue& s) : Value() {
  Token token(s.getString(), Token::STRING, 0, 0, "generated");

  type = Value::STRING;
  tokens.push_back(token);
  this->quotes = s.getQuotes();
  setString(token);
}

StringValue::StringValue(const Value& val, bool quotes) {
  const StringValue* sval;
  Token token = val.getTokens()->front();
  token.type = Token::STRING;

  if (val.type == STRING) {
    sval = static_cast<const StringValue*>(&val);
    token = sval->getString();
  } else {
    token = val.getTokens()->toString();
  }

  type = Value::STRING;
  tokens.push_back(token);
  this->quotes = quotes;
  setString(token);
}

StringValue::~StringValue() {
}

void StringValue::updateTokens() {
  std::string::iterator i;
  std::string newstr;

  if (quotes) {
    // add quotes
    newstr.push_back('"');
    for (i = strvalue.begin(); i != strvalue.end(); i++) {
      if (*i == '"')
        newstr.push_back('\\');
      newstr.push_back(*i);
    }
    newstr.push_back('"');
    tokens.front() = newstr;
  } else
    tokens.front() = strvalue;
}

std::string StringValue::getString() const {
  return strvalue;
}
void StringValue::setString(const std::string& newValue) {
  std::string s = newValue;
  this->strvalue = s;
  updateTokens();
}

void StringValue::setQuotes(bool quotes) {
  this->quotes = quotes;
  updateTokens();
}
bool StringValue::getQuotes() const {
  return quotes;
}

void StringValue::append(const Value& v) {
  const StringValue* s;

  if (v.type == STRING) {
    s = static_cast<const StringValue*>(&v);
    strvalue.append(s->getString());
  } else {
    strvalue.append(v.getTokens()->toString());
  }

  updateTokens();
}

Value* StringValue::operator+(const Value& v) const {
  StringValue* sv = new StringValue(*this);
  sv->append(v);
  return sv;
}

Value* StringValue::operator-(const Value& v) const {
  (void)v;
  throw new ValueException("Can't substract from strings.", *this->getTokens());
}
Value* StringValue::operator*(const Value& v) const {
  std::string newstr;
  double i;
  const NumberValue* n;

  if (v.type != Value::NUMBER) {
    throw new ValueException("Strings can only be multiplied by a number.",
                             *this->getTokens());
  }

  n = static_cast<const NumberValue*>(&v);

  for (i = 0; i < n->getValue(); i++) {
    newstr.append(getString());
  }
  return new StringValue(newstr, getQuotes());
}

Value* StringValue::operator/(const Value& v) const {
  (void)v;
  throw new ValueException("Can't divide strings.", *this->getTokens());
}

bool StringValue::operator==(const Value& v) const {
  const StringValue* s;
  const BooleanValue* b;

  if (v.type == STRING) {
    s = static_cast<const StringValue*>(&v);
    return getString() == s->getString();
    
  } else if (v.type == BOOLEAN) {
    // any string is falsy.
    b = static_cast<const BooleanValue*>(&v);
    return false == b->getValue();
  } else {
    throw new ValueException("You can only compare a string with a *string*.",
                             *this->getTokens());
  }
}
bool StringValue::operator<(const Value& v) const {
  const StringValue* s;
  const BooleanValue* b;

  if (v.type == STRING) {
    s = static_cast<const StringValue*>(&v);
    return getString() < s->getString();
  } else if (v.type == BOOLEAN) {
    b = static_cast<const BooleanValue*>(&v);
    return b->getValue();
  } else {
    throw new ValueException("You can only compare a string with a *string*.",
                             *this->getTokens());
  }
}

string StringValue::escape(string rawstr, string extraUnreserved) {
  string unreservedChars(
      "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_.~");

  std::ostringstream newstr;
  unsigned int i;

  for (i = 0; i < rawstr.size(); i++) {
    if (unreservedChars.find(rawstr[i]) == string::npos &&
        extraUnreserved.find(rawstr[i]) == string::npos) {
      newstr << '%' << std::setfill('0') << std::setw(2) << std::hex
             << (int)rawstr[i];
    } else
      newstr << rawstr[i];
  }
  return newstr.str();
}

