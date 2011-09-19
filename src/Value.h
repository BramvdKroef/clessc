#ifndef __Value_h__
#define __Value_h__

#include "Token.h"
#include "ParseException.h"
#include <sstream>
using namespace std;

/**
 * 
 */
class Value {
protected:
  Token* token;
  
public:
  enum Type {NUMBER, PERCENTAGE, DIMENSION, COLOR} type;
  Value();
  Value(Token* token);
  virtual ~Value();
  
  virtual Token* getToken();
  
  virtual bool add(Value* v);
  virtual bool substract(Value* v);
  virtual bool multiply(Value* v);
  virtual bool divide(Value* v);
  double getValue();
  int getPercent();
  string getUnit();
  void setValue(double d);
  void setPercent(int i);
};

#endif
