#ifndef __less_value_Value_h__
#define __less_value_Value_h__

#include "less/Token.h"
#include "less/TokenList.h"
#include "less/value/ValueException.h"

class BooleanValue;

/**
 *
 */
class Value {
protected:
  TokenList tokens;

public:
  enum Type {
    NUMBER,
    PERCENTAGE,
    DIMENSION,
    COLOR,
    STRING,
    UNIT,
    BOOLEAN,
    URL
  } type;
  Value();
  Value(const Token& token);
  virtual ~Value();

  void setLocation(const Token& ref);

  virtual const TokenList* getTokens() const;

  virtual Value* operator+(const Value& v) const = 0;
  virtual Value* operator-(const Value& v) const = 0;
  virtual Value* operator*(const Value& v) const = 0;
  virtual Value* operator/(const Value& v) const = 0;

  virtual bool operator==(const Value& v) const;
  virtual bool operator!=(const Value& v) const;

  virtual bool operator<(const Value& v) const;
  virtual bool operator>(const Value& v) const;
  virtual bool operator<=(const Value& v) const;
  virtual bool operator>=(const Value& v) const;
  


  static const char* typeToString(const Type& t);
  /**
   * return a type for a type code.
   * N - Number
   * P - Percentage
   * D - Dimension
   * C - Color
   * S - String
   * U - Unit
   * B - Boolean
   * R - URL
   */
  static Type codeToType(const char code);
};

#include "less/value/BooleanValue.h"

#endif  // __less_value_Value_h__
