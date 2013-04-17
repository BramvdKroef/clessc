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


StringValue::StringValue(Token* token, bool quotes) {
  this->tokens.push(token);
  this->quotes = quotes;
  stringvalue = tokens.front()->str;
  type = Value::STRING;
}

StringValue::~StringValue() {
}


TokenList* StringValue::getTokens() {
  string::iterator i;
  string newstr;

  if (quotes) {
    // add quotes
    newstr.push_back('"');
    for (i = stringvalue.begin(); i != stringvalue.end(); i++) {
      if (*i == '"' || *i == '\\') 
        newstr.push_back('\\');
      newstr.push_back(*i);
    }
    newstr.push_back('"');
    tokens.front()->str = newstr;
  } else
    tokens.front()->str = stringvalue;
  return &tokens;
}

string StringValue::getString() {
  return stringvalue;
}
void StringValue::setString(string stringValue) {
  this->stringvalue = stringValue;
}
  
void StringValue::setQuotes(bool quotes) {
  this->quotes = quotes;
}
bool StringValue::getQuotes() {
  return quotes;
}

Value* StringValue::add(Value* v) {
  bool v_quotes;
  string* newstr;
  
  if (v->type == Value::STRING) {
    v_quotes = ((StringValue*)v)->getQuotes();
    ((StringValue*)v)->setQuotes(false);

    newstr = v->getTokens()->toString();
    ((StringValue*)v)->setQuotes(v_quotes);
  } else
    newstr = v->getTokens()->toString();
  
  stringvalue.append(*newstr);
  delete newstr;
  return this;
}

Value* StringValue::substract(Value* v) {
  (void)v;
  throw new ValueException("Can't substract from strings.");
}
Value* StringValue::multiply(Value* v) {
  string newstr;
  double i;
  
  if (v->type != Value::NUMBER) {
    throw new ValueException("Strings can only be multiplied by a number.");
  }

  for (i = 0; i < ((NumberValue*)v)->getValue(); i++) {
    newstr.append(stringvalue);
  }
  stringvalue = newstr;
  return this;
}

Value* StringValue::divide(Value* v) {
  (void)v;
  throw new ValueException("Can't divide strings.");
}

int StringValue::compare(Value* v) {
  if (v->type == STRING) {
    return getString().compare(((StringValue*)v)->getString());
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

void StringValue::loadFunctions(FunctionLibrary* lib) {
  lib->push("escape", "S", &StringValue::escape);
  lib->push("e", "S", &StringValue::e);
  lib->push("%", "S.+", &StringValue::format);
  lib->push("color", "S", &StringValue::color);
  lib->push("data-uri", "SS?", &StringValue::data_uri);
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
          argStr = *arguments[argc]->getTokens()->toString(); 

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
  Token* t;
  ((StringValue*)arguments[0])->setQuotes(false);
  t = arguments[0]->getTokens()->front();
  return new Color(t->clone());
}
Value* StringValue::data_uri(vector<Value*> arguments) {
  return arguments[0];
}
