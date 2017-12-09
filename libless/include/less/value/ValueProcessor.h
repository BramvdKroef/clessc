
#ifndef __ValueProcessor_h__
#define __ValueProcessor_h__

#include "../TokenList.h"
#include "../Token.h"
#include "../css/ParseException.h"
#include "Value.h"
#include "Color.h"
#include "NumberValue.h"
#include "StringValue.h"
#include "UnitValue.h"
#include "UrlValue.h"
#include "ValueException.h"
#include "ValueScope.h"
#include "FunctionLibrary.h"
#include <map>
#include <vector>
#include <cstring>
/**
 * 
 */
class ValueProcessor {
public:
  enum Operator{OP_EQUALS, OP_LESS, OP_GREATER, OP_LESS_EQUALS,
                OP_GREATER_EQUALS, OP_ADD, OP_SUBSTRACT,
                OP_MULTIPLY, OP_DIVIDE, OP_NONE}; 

private:
  FunctionLibrary functionLibrary;

  Value* processStatement(const TokenList& tokens,
                          const ValueScope& scope) const;

  Value* processStatement(TokenList::const_iterator &it,
                          TokenList::const_iterator &end,
                          const ValueScope &scope) const;

  Value* processOperation(TokenList::const_iterator &i,
                          TokenList::const_iterator &end,
                          const Value &operand1,
                          const ValueScope &scope,
                          ValueProcessor::Operator lastop) const;

  Operator processOperator(TokenList::const_iterator &i,
                           TokenList::const_iterator &end) const;

  Value* processConstant(TokenList::const_iterator &it,
                         TokenList::const_iterator &end,
                         const ValueScope& scope) const;

  Value* processSubstatement(TokenList::const_iterator &i,
                             TokenList::const_iterator &end,
                             const ValueScope &scope) const;
    
  const TokenList* processDeepVariable (TokenList::const_iterator &it,
                                        TokenList::const_iterator &end,
                                        const ValueScope& scope) const;
  
  Value* processFunction(const Token &function,
                         TokenList::const_iterator &it,
                         TokenList::const_iterator &end,
                         const ValueScope &scope) const;
  
  bool processArguments (TokenList::const_iterator &it,
                         TokenList::const_iterator &end,
                         const ValueScope &scope,
                         vector<const Value*> &arguments) const;
  
  Value* processEscape (TokenList::const_iterator &it,
                        TokenList::const_iterator &end,
                        const ValueScope &scope) const;
  UnitValue* processUnit(Token &t) const;
  
  bool needsSpace(const Token &t, bool before) const;

  Value* processNegative(TokenList::const_iterator &it,
                         TokenList::const_iterator &end,
                         const ValueScope &scope) const;

  void skipWhitespace(TokenList::const_iterator &i,
                      TokenList::const_iterator &end) const;

  const char* operatorToString(ValueProcessor::Operator o) const;
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

  bool validateCondition(const TokenList &value, const ValueScope &scope);
  bool validateValue(TokenList::const_iterator &i,
                     TokenList::const_iterator &end,
                     const ValueScope &scope);
  
  bool functionExists(const char* function) const;

  void interpolate(string &str, const ValueScope &scope) const;
  void interpolate(TokenList &tokens, const ValueScope &scope) const;
};

#endif
