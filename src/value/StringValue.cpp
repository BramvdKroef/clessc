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

#include "StringValue.h"

#include <config.h>
#ifdef WITH_LIBGLOG
#include <glog/logging.h>
#endif


StringValue::StringValue(Token &token, bool quotes) {
  type = Value::STRING;
  
  tokens.push_back(token);
  this->quotes = quotes;
  setString(token);
}

StringValue::StringValue(std::string &str, bool quotes) {
  Token token(str, Token::STRING, 0,0,"generated");
  
  type = Value::STRING;
  tokens.push_back(token);
  this->quotes = quotes;
  setString(token);
}

StringValue::StringValue(const StringValue &s) {
  Token token(s.getString(), Token::STRING, 0,0,"generated");

  type = Value::STRING;
  tokens.push_back(token);
  this->quotes = quotes;
  setString(token);
}

StringValue::StringValue(const Value &val, bool quotes) {
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
void StringValue::setString(const std::string &newValue) {
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

void StringValue::append(const Value &v) {
  const StringValue* s;

  if (v.type == STRING) {
    s = static_cast<const StringValue*>(&v);
    strvalue.append(s->getString());
  } else {
    strvalue.append(v.getTokens()->toString());
  }

  updateTokens();
}


Value* StringValue::add(const Value &v) const {
  StringValue* sv = new StringValue(*this);
  sv->append(v);
  return sv;
}

Value* StringValue::substract(const Value &v) const {
  (void)v;
  throw new ValueException("Can't substract from strings.", *this->getTokens());
}
Value* StringValue::multiply(const Value &v) const {
  std::string newstr;
  double i;
  const NumberValue* n;
  
  if (v.type != Value::NUMBER) {
    throw new ValueException("Strings can only be multiplied by a number.", *this->getTokens());
  }

  n = static_cast<const NumberValue*>(&v);
  
  for (i = 0; i < n->getValue(); i++) {
    newstr.append(strvalue);
  }
  return new StringValue(newstr, getQuotes());
}

Value* StringValue::divide(const Value &v) const {
  (void)v;
  throw new ValueException("Can't divide strings.", *this->getTokens());
}

BooleanValue* StringValue::equals(const Value &v) const {
  const StringValue* s;
  
  if (v.type == STRING) {
    s = static_cast<const StringValue*>(&v);
    return new BooleanValue(getString() == s->getString());
  } else {
    throw new ValueException("You can only compare a string with a *string*.", *this->getTokens());
  }
}
BooleanValue* StringValue::lessThan(const Value &v) const {
  const StringValue* s;
  
  if (v.type == STRING) {
    s = static_cast<const StringValue*>(&v);
    return new BooleanValue(getString() < s->getString());
  } else {
    throw new ValueException("You can only compare a string with a *string*.", *this->getTokens());
  }
}

string StringValue::escape(string rawstr, string extraUnreserved) {
  string unreservedChars("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_.~");

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

void StringValue::loadFunctions(FunctionLibrary &lib) {
  lib.push("escape", "S", &StringValue::escape);
  lib.push("e", "S", &StringValue::e);
  lib.push("%", "S.+", &StringValue::format);
  lib.push("color", "S", &StringValue::color);
  lib.push("data-uri", "SS?", &StringValue::data_uri);
}

Value* StringValue::escape(const vector<const Value*> &arguments) {
  string unreservedChars("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_.~,/?@&+'!$");

  StringValue* s = new StringValue(*(const StringValue*)arguments[0]);
  s->setString(StringValue::escape(s->getString(), ",/?@&+'!$"));
  return s;
}


Value* StringValue::e(const vector<const Value*> &arguments) {
  StringValue* s = new StringValue(*(const StringValue*)arguments[0]);
  s->setQuotes(false);
  return s;
}

Value* StringValue::format(const vector<const Value*> &arguments) {
  std::string escapeChars("adsADS");

  StringValue* s = new StringValue(*(const StringValue*)arguments[0]);

  std::string oldstr = s->getString();
  std::ostringstream newstr;
  unsigned int i, argc = 1;
  std::string argStr;

  for (i = 0; i < oldstr.size(); i++) {
    if (oldstr[i] == '%') {
      i++;
      
      if (escapeChars.find(oldstr[i]) != string::npos) {
        if (argc == arguments.size())
          throw new ValueException("Format template expects more \
arguments than provided.", *arguments[0]->getTokens());

        if ((oldstr[i] == 's' || oldstr[i] == 'S') &&
            arguments[argc]->type == STRING) {
          argStr = ((const StringValue*)arguments[argc])->getString();
        } else
          argStr = arguments[argc]->getTokens()->toString(); 

        if (oldstr[i] == 'A' || oldstr[i] == 'D' ||
            oldstr[i] == 'S') {
          argStr = StringValue::escape(argStr);
        }
        newstr << argStr;
        argc++;
      } else
        newstr << oldstr[i];
    } else
      newstr << oldstr[i];
  }
  
  if (argc != arguments.size()) {
    throw new ValueException("Format template does not supply \
placeholders for all given arguments.", *arguments[0]->getTokens());
  }
  
  s->setString(newstr.str());
  return s;
}

Value* StringValue::color(const vector<const Value*> &arguments) {
  const StringValue* s;
  Token t;
  
  s = static_cast<const StringValue*>(arguments[0]);

  t = Token(s->getString(), Token::HASH ,0,0,"generated");
  return new Color(t);
}
Value* StringValue::data_uri(const vector<const Value*> &arguments) {
  return new StringValue(*(const StringValue*)arguments[0]);
}
