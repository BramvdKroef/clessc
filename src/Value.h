#ifndef __Value_h__
#define __Value_h__

#include "Token.h"
#include "TokenList.h"
#include "ParseException.h"
#include <sstream>
using namespace std;

/**
 * 
 */
class Value {
protected:
  TokenList tokens;
  
public:
  enum Type {NUMBER, PERCENTAGE, DIMENSION, COLOR} type;
  Value();
  Value(Token* token);
  virtual ~Value();
  
  virtual TokenList* getTokens();
  
  virtual void add(Value* v);
  virtual void substract(Value* v);
  virtual void multiply(Value* v);
  virtual void divide(Value* v);
  void setType(Value* v);
  
  double getValue();
  string getUnit();
  void setUnit(string unit);
  void setValue(double d);
};

#endif
