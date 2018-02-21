#ifndef __less_value_BooleanValue_h__
#define __less_value_BooleanValue_h__

#include "less/value/StringValue.h"
#include "less/value/Value.h"
#include "less/value/ValueException.h"

/**
 * True or false.
 */
class BooleanValue : public Value {
private:
  bool value;

public:
  BooleanValue(bool value);
  BooleanValue(const Token &t, bool value);
  virtual ~BooleanValue();

  bool getValue() const;
  void setValue(bool value);

  virtual Value *operator+(const Value &v) const;
  virtual Value *operator-(const Value &v) const;
  virtual Value *operator*(const Value &v) const;
  virtual Value *operator/(const Value &v) const;

  virtual bool operator==(const Value &v) const;
  virtual bool operator<(const Value &v) const;
};

#endif  // __less_value_BooleanValue_h__
