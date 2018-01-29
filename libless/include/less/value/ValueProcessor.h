#ifndef __less_value_ValueProcessor_h__
#define __less_value_ValueProcessor_h__

#include <cstring>
#include <map>
#include <vector>
#include "less/Token.h"
#include "less/TokenList.h"
#include "less/css/ParseException.h"
#include "less/value/Color.h"
#include "less/value/FunctionLibrary.h"
#include "less/value/NumberValue.h"
#include "less/value/StringValue.h"
#include "less/value/UnitValue.h"
#include "less/value/UrlValue.h"
#include "less/value/Value.h"
#include "less/value/ValueException.h"
#include "less/value/ValueScope.h"

/**
 *
 */
class ValueProcessor {
public:
  enum Operator {
    OP_EQUALS,
    OP_LESS,
    OP_GREATER,
    OP_LESS_EQUALS,
    OP_GREATER_EQUALS,
    OP_ADD,
    OP_SUBSTRACT,
    OP_MULTIPLY,
    OP_DIVIDE,
    OP_NONE
  };

private:
  FunctionLibrary functionLibrary;

  Value *processStatement(const TokenList &tokens,
                          const ValueScope &scope) const;

  Value *processStatement(TokenList::const_iterator &it,
                          TokenList::const_iterator &end,
                          const ValueScope &scope,
                          bool defaultVal = false) const;

  Value *processOperation(TokenList::const_iterator &i,
                          TokenList::const_iterator &end,
                          const Value &operand1,
                          const ValueScope &scope,
                          ValueProcessor::Operator lastop,
                          bool defaultVal = false) const;

  Operator processOperator(TokenList::const_iterator &i,
                           TokenList::const_iterator &end) const;

  Value *processConstant(TokenList::const_iterator &it,
                         TokenList::const_iterator &end,
                         const ValueScope &scope,
                         bool defaultVal = false) const;

  Value *processSubstatement(TokenList::const_iterator &i,
                             TokenList::const_iterator &end,
                             const ValueScope &scope,
                             bool defaultVal = false) const;

  const TokenList *processDeepVariable(TokenList::const_iterator &it,
                                       TokenList::const_iterator &end,
                                       const ValueScope &scope) const;

  Value *processFunction(const Token &function,
                         TokenList::const_iterator &it,
                         TokenList::const_iterator &end,
                         const ValueScope &scope) const;

  bool processArguments(TokenList::const_iterator &it,
                        TokenList::const_iterator &end,
                        const ValueScope &scope,
                        vector<const Value *> &arguments) const;

  Value *processEscape(TokenList::const_iterator &it,
                       TokenList::const_iterator &end,
                       const ValueScope &scope) const;
  UnitValue *processUnit(Token &t) const;

  bool needsSpace(const Token &t, bool before) const;

  Value *processNegative(TokenList::const_iterator &it,
                         TokenList::const_iterator &end,
                         const ValueScope &scope) const;

  void skipWhitespace(TokenList::const_iterator &i,
                      TokenList::const_iterator &end) const;

  const char *operatorToString(ValueProcessor::Operator o) const;

public:
  ValueProcessor();
  virtual ~ValueProcessor();

  /**
   * Determine if a value contains anything that can be processed.
   *
   * @return  true if value contains an @-keyword, an operator (+-* or
   *          /), a LESS function, an escaped value or a url.
   */
  bool needsProcessing(const TokenList &value) const;

  void processValue(TokenList &value, const ValueScope &scope) const;

  bool validateCondition(const TokenList &value,
                         const ValueScope &scope,
                         bool defaultVal = false) const;
  bool validateValue(TokenList::const_iterator &i,
                     TokenList::const_iterator &end,
                     const ValueScope &scope,
                     bool defaultVal = false) const;

  bool functionExists(const char *function) const;

  void interpolate(string &str, const ValueScope &scope) const;
  void interpolate(TokenList &tokens, const ValueScope &scope) const;
};

#endif  // __less_value_ValueProcessor_h__
