
#ifndef __BooleanValue_h__
#define __BooleanValue_h__

#include "Value.h"
#include "ValueException.h"
#include "StringValue.h"

/**
 * True or false.
 */
class BooleanValue: public Value {
private:
  bool value;
public:
  BooleanValue(bool value);
  BooleanValue(const Token &t, bool value);
  virtual ~BooleanValue();

  bool getValue() const;
  void setValue(bool value);

  virtual Value* add(const Value &v) const;
  virtual Value* substract(const Value &v) const;
  virtual Value* multiply(const Value &v) const;
  virtual Value* divide(const Value &v) const;

  virtual BooleanValue* equals(const Value &v) const;
  virtual BooleanValue* lessThan(const Value &v) const;
};
#endif
  
