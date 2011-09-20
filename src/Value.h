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
  
  virtual bool add(Value* v);
  virtual bool substract(Value* v);
  virtual bool multiply(Value* v);
  virtual bool divide(Value* v);
  double getValue();
  string getUnit();
  void setUnit(string unit);
  void setValue(double d);
};

#endif
