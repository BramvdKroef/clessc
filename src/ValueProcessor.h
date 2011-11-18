#ifndef __ValueProcessor_h__
#define __ValueProcessor_h__

#include "TokenList.h"
#include "Token.h"
#include "Value.h"
#include "Color.h"
#include "ValueException.h"
#include <map>
#include <vector>

/**
 * 
 */
class ValueProcessor {
private:
  list<map<string, TokenList*>*> scopes;
  
  Value* processStatement(TokenList* value);
  Value* processOperator(TokenList* value, Value* v1,
                         Token* lastop = NULL);
  Value* processConstant(TokenList* value);
  TokenList* processDeepVariable (TokenList* value);
  Value* processFunction(Token* function,
                         TokenList* value);
  vector<Value*> processArguments (TokenList* value,
                                   unsigned int len);
  void processString(Token* str);
  Token* processEscape (TokenList* value);
  
public:
  ValueProcessor();
  virtual ~ValueProcessor();

  void putVariable(string key, TokenList* value);
  TokenList* getVariable(string key);
  TokenList* processValue(TokenList* value);

  void pushScope();
  void popScope();
};

#endif
