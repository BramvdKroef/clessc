#include "less/value/ValueProcessor.h"
#include "less/value/ColorFunctions.h"
#include "less/value/NumberFunctions.h"
#include "less/value/StringFunctions.h"
#include "less/value/UrlFunctions.h"


ValueProcessor::ValueProcessor() {
  NumberFunctions::loadFunctions(functionLibrary);
  ColorFunctions::loadFunctions(functionLibrary);
  StringFunctions::loadFunctions(functionLibrary);
  UrlFunctions::loadFunctions(functionLibrary);
}
ValueProcessor::~ValueProcessor() {
}

void ValueProcessor::processValue(TokenList &value,
                                  const ValueScope &scope) const {
  TokenList::iterator i;
  TokenList newvalue;
  Value *v;
  const TokenList *var;
  TokenList variable;
  const TokenList *oldvalue = &value;
  TokenList::const_iterator i2, itmp, end;

  if (!needsProcessing(value)) {
    // interpolate strings
    for (i = value.begin(); i != value.end(); i++) {
      if ((*i).type == Token::STRING)
        interpolate((*i), scope);
    }
    return;
  }

  end = oldvalue->end();
  for (i2 = oldvalue->begin(); i2 != end;) {
    try {
      itmp = i2;
      v = processStatement(itmp, end, scope);
      i2 = itmp;
    } catch (ValueException*) {
      v = NULL;
    }

    // add spaces between values
    if (v != NULL || i2 != end) {
      if (newvalue.size() == 0 || !needsSpace(newvalue.back(), false) ||
          (v == NULL && !needsSpace(*i2, true))) {
      } else {
        newvalue.push_back(Token::BUILTIN_SPACE);
      }
    }

    if (v != NULL) {
      var = v->getTokens();
      newvalue.insert(
          newvalue.end(), var->begin(), var->end());
      var = NULL;
      delete v;
    } else if (i2 != end) {
      // variable containing a non-value.
      if ((*i2).type == Token::ATKEYWORD &&
          (var = scope.getVariable(*i2)) != NULL) {
        variable = *var;
        processValue(variable, scope);

        newvalue.insert(newvalue.end(), variable.begin(), variable.end());
        i2++;

        // deep variable
      } else if ((var = processDeepVariable(i2, end, scope)) != NULL) {
        variable = *var;
        processValue(variable, scope);

        newvalue.insert(newvalue.end(), variable.begin(), variable.end());

      } else if ((*i2).type == Token::IDENTIFIER) {
        newvalue.push_back(*i2);
        i2++;

        if (i2 != end && (*i2).type == Token::PAREN_OPEN) {
          newvalue.push_back(*i2);
          i2++;
        }
      } else {
        newvalue.push_back(*i2);
        i2++;
      }
    }
  }

  value = newvalue;
  return;
}

bool ValueProcessor::needsProcessing(const TokenList &value) const {
  TokenList::const_iterator i;
  const Token *t;
  string operators("+-*/");

  for (i = value.begin(); i != value.end(); i++) {
    // variable
    if ((*i).type == Token::ATKEYWORD ||
        // url
        (*i).type == Token::URL ||
        // operator
        operators.find(*i) != string::npos) {
      return true;

    } else {
      if (*i == "~") {
        if ((*++i).type == Token::STRING)
          return true;
        else
          i--;

      } else if ((*i).type == Token::IDENTIFIER || (*i).type == Token::OTHER) {
        // function
        t = &(*i);
        i++;
        if (i != value.end() && (*i).type == Token::PAREN_OPEN &&
            functionExists((*t).c_str())) {
          return true;
        } else
          i--;
      }
    }
  }

  return false;
}

bool ValueProcessor::validateCondition(const TokenList &value,
                                       const ValueScope &scope,
                                       bool defaultVal) const {
  TokenList::const_iterator i = value.begin();
  TokenList::const_iterator end = value.end();
  bool negate = false;
  bool ret;

  skipWhitespace(i, end);

  if (*i == "not") {
    negate = true;
    i++;
  }
    
  ret = validateValue(i, end, scope, defaultVal);

  skipWhitespace(i, end);

  while (ret == true && i != value.end() && *i == "and") {
    i++;

    skipWhitespace(i, end);

    ret = validateValue(i, end, scope, defaultVal);

    skipWhitespace(i, end);
  }

  return negate ? !ret : ret;
}

bool ValueProcessor::validateValue(TokenList::const_iterator &i,
                                   TokenList::const_iterator &end,
                                   const ValueScope &scope,
                                   bool defaultVal) const {
  const Token *reference;
  Value *v;
  const BooleanValue trueVal(true);
  bool ret;

  if (i == end)
    return false;

  reference = &(*i);
  v = processStatement(i, end, scope, defaultVal);

  if (v == NULL) {
    throw new ParseException(*reference,
                             "condition",
                             reference->line,
                             reference->column,
                             reference->source);
  }

  ret = (*v == trueVal);

  delete v;

  return ret;
}

Value *ValueProcessor::processStatement(const TokenList &tokens,
                                        const ValueScope &scope) const {
  TokenList::const_iterator i = tokens.begin();
  TokenList::const_iterator end = tokens.end();
  Value *ret = processStatement(i, end, scope);

  if (i != end)
    return NULL;
  return ret;
}

Value *ValueProcessor::processStatement(TokenList::const_iterator &i,
                                        TokenList::const_iterator &end,
                                        const ValueScope &scope,
                                        bool defaultVal) const {
  Value *op, *v;

  skipWhitespace(i, end);
  v = processConstant(i, end, scope, defaultVal);

  if (v != NULL) {
    skipWhitespace(i, end);

    while ((op = processOperation(i, end, *v, scope, OP_NONE, defaultVal)) != NULL) {
      delete v;
      v = op;

      skipWhitespace(i, end);
    }

    return v;
  } else
    return NULL;
}

Value *ValueProcessor::processOperation(TokenList::const_iterator &i,
                                        TokenList::const_iterator &end,
                                        const Value &operand1,
                                        const ValueScope &scope,
                                        ValueProcessor::Operator lastop,
                                        bool defaultVal) const {
  TokenList::const_iterator tmp;
  const Value *operand2;
  Value *result;
  Operator op;
  const Token *opToken;

  if (i == end)
    return NULL;

  opToken = &(*i);
  tmp = i;

  if ((op = processOperator(tmp, end)) == OP_NONE ||
      (lastop != OP_NONE && lastop >= op))
    return NULL;

  i = tmp;
  skipWhitespace(i, end);

  operand2 = processConstant(i, end, scope, defaultVal);
  if (operand2 == NULL) {
    if (i == end)
      throw new ParseException("end of line",
                               "Constant or @-variable",
                               opToken->line,
                               opToken->column,
                               opToken->source);
    else
      throw new ParseException(
          *i, "Constant or @-variable", (*i).line, (*i).column, (*i).source);
  }

  skipWhitespace(i, end);

  while ((result = processOperation(i, end, *operand2, scope, op, defaultVal))) {
    delete operand2;
    operand2 = result;

    skipWhitespace(i, end);
  }

  if (op == OP_ADD)
    result = operand1 + *operand2;
  else if (op == OP_SUBSTRACT)
    result = operand1 - *operand2;
  else if (op == OP_MULTIPLY)
    result = operand1 * *operand2;
  else if (op == OP_DIVIDE)
    result = operand1 / *operand2;
  else if (op == OP_EQUALS)
    result = new BooleanValue(operand1 == *operand2);
  else if (op == OP_LESS)
    result = new BooleanValue(operand1 < *operand2);
  else if (op == OP_GREATER)
    result = new BooleanValue(operand1 > *operand2);
  else if (op == OP_LESS_EQUALS)
    result = new BooleanValue(operand1 <= *operand2);
  else if (op == OP_GREATER_EQUALS)
    result = new BooleanValue(operand1 >= *operand2);

  delete operand2;
  result->setLocation(*opToken);
  return result;
}

ValueProcessor::Operator ValueProcessor::processOperator(
    TokenList::const_iterator &i, TokenList::const_iterator &end) const {
  Token t = *i;
  i++;

  if (t == "=") {
    if (i != end && *i == "<") {
      i++;
      return OP_LESS_EQUALS;
    } else
      return OP_EQUALS;

  } else if (t == "<") {
    return OP_LESS;

  } else if (t == ">") {
    if (i != end && *i == "=") {
      i++;
      return OP_GREATER_EQUALS;
    } else
      return OP_GREATER;

  } else if (t == "+") {
    return OP_ADD;
  } else if (t == "-") {
    if (i != end && (*i).type != Token::WHITESPACE) {
      i--;
      return OP_NONE;
    }
    return OP_SUBSTRACT;
  } else if (t == "*") {
    return OP_MULTIPLY;
  } else if (t == "/") {
    return OP_DIVIDE;
  } else
    return OP_NONE;
}

const char *ValueProcessor::operatorToString(ValueProcessor::Operator o) const {
  switch (o) {
    case OP_EQUALS:
      return "=";
    case OP_LESS:
      return "<";
    case OP_GREATER:
      return ">";
    case OP_LESS_EQUALS:
      return "=<";
    case OP_GREATER_EQUALS:
      return ">=";
    case OP_ADD:
      return "+";
    case OP_SUBSTRACT:
      return "-";
    case OP_MULTIPLY:
      return "*";
    case OP_DIVIDE:
      return "/";
    case OP_NONE:
    default:
      return "not an operator";
  }
}

Value *ValueProcessor::processConstant(TokenList::const_iterator &i,
                                       TokenList::const_iterator &end,
                                       const ValueScope &scope,
                                       bool defaultVal) const {
  Token token;
  Value *ret;
  const TokenList *var;
  TokenList variable;
  bool hasQuotes;
  std::string str;

  if (i == end)
    return NULL;

  token = *i;

  switch (token.type) {
    case Token::HASH:
      i++;
      // generate color from hex value
      return new Color(token);
      
    case Token::NUMBER:
    case Token::PERCENTAGE:
    case Token::DIMENSION:
      i++;
      return new NumberValue(token);

    case Token::ATKEYWORD:
      if ((var = scope.getVariable(token)) != NULL) {
        variable = *var;

        ret = processStatement(variable, scope);

        if (ret != NULL) {
          i++;
          //ret->setLocation(token);

          return ret;
        }
      }
      return NULL;

    case Token::STRING:
      i++;
      hasQuotes = token.stringHasQuotes();
      interpolate(token, scope);
      token.removeQuotes();
      return new StringValue(token, hasQuotes);

    case Token::URL:
      i++;
      interpolate(token, scope);
      str = token.getUrlString();

      return new UrlValue(token, str);

    case Token::IDENTIFIER:
      i++;

      if (i != end && (*i).type == Token::PAREN_OPEN) {
        if (token == "default") {
          i++;
          if ((*i).type != Token::PAREN_CLOSED) {
            throw new ParseException(*i,
                                     ")",
                                     (*i).line, (*i).column, (*i).source);
          }
          return new BooleanValue(token, defaultVal);
        } else if (functionExists(token.c_str())) {
          i++;

          ret = processFunction(token, i, end, scope);
          if (ret == NULL) {
            i--;
            i--;
            return NULL;
          } else
            return ret;

        } else {
          i--;
          return NULL;
        }

      } else if (token.compare("true") == 0) {
        return new BooleanValue(token, true);
      } else if ((ret = processUnit(token)) != NULL) {
        return ret;
      } else if ((ret = Color::fromName(token)) != NULL) {
        return ret;
      } else {
        return new StringValue(token, false);
      }

    case Token::PAREN_OPEN:
      return processSubstatement(i, end, scope, defaultVal);

    default:
      break;
  }

  if ((var = processDeepVariable(i, end, scope)) != NULL) {
    variable = *var;
    ret = processStatement(variable, scope);
    if (ret != NULL) {
      //ret->setLocation(token);
    }
    return ret;
  }
  if (token == "%") {
    i++;
    if (i != end && (*i).type == Token::PAREN_OPEN) {
      i++;

      if ((ret = processFunction(token, i, end, scope)) != NULL)
        return ret;

      i--;
    }
    i--;
  }
  if ((ret = processEscape(i, end, scope)) != NULL) {
    return ret;
  } else if ((ret = processNegative(i, end, scope)) != NULL) {
    return ret;
  }
  return NULL;
}

Value *ValueProcessor::processSubstatement(TokenList::const_iterator &i,
                                           TokenList::const_iterator &end,
                                           const ValueScope &scope,
                                           bool defaultVal) const {
  Value *ret;
  TokenList::const_iterator i2 = i;

  if (i == end || (*i).type != Token::PAREN_OPEN)
    return NULL;

  i2++;

  ret = processStatement(i2, end, scope, defaultVal);

  if (ret == NULL)
    return NULL;

  ret->setLocation(*i);

  skipWhitespace(i2, end);

  if (i2 == end || (*i2).type != Token::PAREN_CLOSED) {
    delete ret;
    return NULL;
  }

  i2++;

  i = i2;

  return ret;
}

const TokenList *ValueProcessor::processDeepVariable(
    TokenList::const_iterator &i,
    TokenList::const_iterator &end,
    const ValueScope &scope) const {
  const TokenList *var;
  TokenList variable;
  std::string key = "@";

  if (i == end || (*i).type != Token::OTHER || (*i) != "@")
    return NULL;

  i++;

  if (i == end || (*i).type != Token::ATKEYWORD ||
      (var = scope.getVariable((*i))) == NULL) {
    i--;
    return NULL;
  }

  variable = *var;
  processValue(variable, scope);

  if (variable.size() != 1 || variable.front().type != Token::STRING) {
    i--;
    return NULL;
  }

  i++;
  // generate key with '@' + var without quotes
  variable.front().removeQuotes();
  key.append(variable.front());

  return scope.getVariable(key);
}

bool ValueProcessor::functionExists(const char *function) const {
  return ((functionLibrary.getFunction(function)) != NULL);
}

Value *ValueProcessor::processFunction(const Token &function,
                                       TokenList::const_iterator &i,
                                       TokenList::const_iterator &end,
                                       const ValueScope &scope) const {
  // Use a temporary iterator so we don't disturb <code>i</code> if
  // processing fails
  TokenList::const_iterator i2 = i;

  vector<const Value *> arguments;

  const FuncInfo *fi;
  Value *ret = NULL;
  vector<const Value *>::iterator it;
  string arg_str;
  
  std::ostringstream fnc_str;
  
  fi = functionLibrary.getFunction(function.c_str());

  if (fi == NULL)
    return NULL;

  if (processArguments(i2, end, scope, arguments)) {
    
    if (!functionLibrary.checkArguments(fi, arguments)) {
      fnc_str << function << "(";
      for (it = arguments.begin(); it != arguments.end(); it++) {
        if (it != arguments.begin())
          fnc_str << ", ";
        fnc_str << (*it)->getTokens()->toString() << "(" <<
          (*it)->typeToString((*it)->type) << ")";
      }
      fnc_str << ")";
      throw new ParseException(fnc_str.str(),
                               functionLibrary.functionDefToString(function.c_str(), fi),
                               function.line, function.column, function.source);
    }
    ret = fi->func(arguments);
    ret->setLocation(function);
    // advance the iterator
    i = i2;
  } else
    ret = NULL;

  // delete arguments
  for (it = arguments.begin(); it != arguments.end(); it++) {
    delete (*it);
  }

  return ret;
}

bool ValueProcessor::processArguments(TokenList::const_iterator &i,
                                      TokenList::const_iterator &end,
                                      const ValueScope &scope,
                                      vector<const Value *> &arguments) const {
  Value *argument;

  if (i == end)
    return false;

  if ((*i).type != Token::PAREN_CLOSED) {
    argument = processStatement(i, end, scope);
    if (argument != NULL)
      arguments.push_back(argument);
    else {
      arguments.push_back(new StringValue(*i, false));
      i++;
    }
  }

  while (i != end && ((*i) == "," || (*i) == ";")) {
    i++;

    argument = processStatement(i, end, scope);

    if (argument != NULL) {
      arguments.push_back(argument);
    } else if ((*i).type != Token::PAREN_CLOSED) {
      arguments.push_back(new StringValue(*i, false));
      i++;
    }
  }

  if (i == end)
    throw new ParseException("end of value", ")", 0, 0, "");

  if ((*i).type != Token::PAREN_CLOSED)
    throw new ParseException(*i, ")", (*i).line, (*i).column, (*i).source);

  i++;
  return true;
}

Value *ValueProcessor::processEscape(TokenList::const_iterator &i,
                                     TokenList::const_iterator &end,
                                     const ValueScope &scope) const {
  Token t;

  if (i == end || *i != "~")
    return NULL;

  i++;

  if ((*i).type != Token::STRING) {
    i--;
    return NULL;
  }

  t = *i;
  i++;
  interpolate(t, scope);
  t.removeQuotes();
  return new StringValue(t, false);
}

UnitValue *ValueProcessor::processUnit(Token &t) const {
  // em,ex,px,ch,in,mm,cm,pt,pc,ms
  string units("emexpxchinmmcmptpcms");
  size_t pos;
  if (t.size() == 2 && (pos = units.find(t)) != string::npos && pos % 2 == 0) {
    return new UnitValue(t);
  } else if (t.compare("m") == 0 || t.compare("s") == 0 ||
             t.compare("rad") == 0 || t.compare("deg") == 0 ||
             t.compare("grad") == 0 || t.compare("turn") == 0) {
    return new UnitValue(t);
  } else
    return NULL;
}

bool ValueProcessor::needsSpace(const Token &t, bool before) const {
  if (t.type == Token::OTHER && t.size() == 1 &&
      string(":=.").find(t[0]) != string::npos) {
    return false;
  }
  if (before && t.type == Token::COLON)
    return false;
  if (!before && t.type == Token::PAREN_OPEN)
    return false;
  if (before && t.type == Token::PAREN_CLOSED)
    return false;
  if (before && t == ",")
    return false;
  return true;
}

void ValueProcessor::skipWhitespace(TokenList::const_iterator &i,
                                    TokenList::const_iterator &end) const {
  while (i != end && (*i).type == Token::WHITESPACE)
    i++;
}

Value *ValueProcessor::processNegative(TokenList::const_iterator &i,
                                       TokenList::const_iterator &end,
                                       const ValueScope &scope) const {
  Token minus;
  Value *constant;
  Value *zero, *ret;
  Token t_zero("0", Token::NUMBER, 0, 0, "generated");

  if (i == end || (*i) != "-")
    return NULL;

  minus = *i;
  i++;

  skipWhitespace(i, end);

  constant = processConstant(i, end, scope);
  if (constant == NULL) {
    i--;
    return NULL;
  }

  zero = new NumberValue(t_zero);
  ret = *zero - *constant;

  ret->setLocation(minus);

  delete constant;
  delete zero;

  return ret;
}

void ValueProcessor::interpolate(std::string &str,
                                 const ValueScope &scope) const {
  size_t start, end = 0;
  string key, value;
  const TokenList *var;
  TokenList variable;

  while ((start = str.find("@{", end)) != string::npos &&
         (end = str.find("}", start)) != string::npos) {
    key = "@";
    key.append(str.substr(start + 2, end - (start + 2)));

    var = scope.getVariable(key);

    if (var != NULL) {
      variable = *var;

      processValue(variable, scope);

      // Remove quotes off strings.
      if (variable.size() == 1 && variable.front().type == Token::STRING) {
        variable.front().removeQuotes();
      }

      value = variable.toString();

      str.replace(start, (end + 1) - start, value);
      end = start + value.length();
    }
  }
}

void ValueProcessor::interpolate(TokenList &tokens,
                                 const ValueScope &scope) const {
  TokenList::iterator i;

  for (i = tokens.begin(); i != tokens.end(); i++) {
    interpolate((*i), scope);
  }
}
