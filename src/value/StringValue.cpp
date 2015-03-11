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
  Token token(str, Token::STRING);
  
  type = Value::STRING;
  tokens.push_back(token);
  this->quotes = quotes;
  setString(token);
}

StringValue::StringValue(const StringValue &s) {
  Token token(s.getString(), Token::STRING);

  type = Value::STRING;
  tokens.push_back(token);
  this->quotes = quotes;
  setString(token);
}

StringValue::StringValue(const Value &val, bool quotes) {
  const StringValue* sval;
  Token token;
  
  if (val.type == STRING) {
    sval = static_cast<const StringValue*>(&val);
    token = Token(sval->getString(), Token::STRING);
  } else {
    token = Token(val.getTokens()->toString(), Token::STRING);
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
  throw new ValueException("Can't substract from strings.");
}
Value* StringValue::multiply(const Value &v) const {
  std::string newstr;
  double i;
  const NumberValue* n;
  
  if (v.type != Value::NUMBER) {
    throw new ValueException("Strings can only be multiplied by a number.");
  }

  n = static_cast<const NumberValue*>(&v);
  
  for (i = 0; i < n->getValue(); i++) {
    newstr.append(strvalue);
  }
  return new StringValue(newstr, getQuotes());
}

Value* StringValue::divide(const Value &v) const {
  (void)v;
  throw new ValueException("Can't divide strings.");
}

BooleanValue* StringValue::equals(const Value &v) const {
  const StringValue* s;
  
  if (v.type == STRING) {
    s = static_cast<const StringValue*>(&v);
    return new BooleanValue(getString() == s->getString());
  } else {
    throw new ValueException("You can only compare a string with a *string*.");
  }
}
BooleanValue* StringValue::lessThan(const Value &v) const {
  const StringValue* s;
  
  if (v.type == STRING) {
    s = static_cast<const StringValue*>(&v);
    return new BooleanValue(getString() < s->getString());
  } else {
    throw new ValueException("You can only compare a string with a *string*.");
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

Value* StringValue::escape(vector<Value*> arguments) {
  string unreservedChars("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_.~,/?@&+'!$");

  string oldstr = ((StringValue*)arguments[0])->getString();

  ((StringValue*)arguments[0])
    ->setString(StringValue::escape(oldstr, ",/?@&+'!$"));

  return arguments[0];
}


Value* StringValue::e(vector<Value*> arguments) {
  ((StringValue*)arguments[0])->setQuotes(false);
  return arguments[0];
}

Value* StringValue::format(vector<Value*> arguments) {
  string escapeChars("adsADS");
  
  string oldstr = ((StringValue*)arguments[0])->getString();
  std::ostringstream newstr;
  unsigned int i, argc = 1;
  string argStr;

  for (i = 0; i < oldstr.size(); i++) {
    if (oldstr[i] == '%') {
      i++;
      
      if (escapeChars.find(oldstr[i]) != string::npos) {
        if (argc == arguments.size())
          throw new ValueException("Format template expects more \
arguments than provided.");

        if ((oldstr[i] == 's' || oldstr[i] == 'S') &&
            arguments[argc]->type == STRING) {
          argStr = ((StringValue*)arguments[argc])->getString();
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
placeholders for all given arguments.");
  }
  
  ((StringValue*)arguments[0])->setString(newstr.str());
  return arguments[0];
}

Value* StringValue::color(vector<Value*> arguments) {
  StringValue* s;
  Token t;
  
  s = static_cast<StringValue*>(arguments[0]);

  t = Token(s->getString(), Token::HASH);
  return new Color(t);
}
Value* StringValue::data_uri(vector<Value*> arguments) {
  return arguments[0];
}
