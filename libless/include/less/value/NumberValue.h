#ifndef __less_value_NumberValue_h__
#define __less_value_NumberValue_h__

#include <cmath>
#include <vector>
#include "less/value/StringValue.h"
#include "less/value/UnitValue.h"
#include "less/value/Value.h"

class FunctionLibrary;

class NumberValue : public Value {
  static bool isNumber(const Value &val);

  void verifyUnits(const NumberValue &n);
  double convert(const std::string &unit) const;

public:
  NumberValue(const Token &token);
  NumberValue(double value);
  NumberValue(double value, Token::Type type, const std::string *unit);
  NumberValue(const NumberValue &n);
  virtual ~NumberValue();

  virtual Value *add(const Value &v) const;
  virtual Value *substract(const Value &v) const;
  virtual Value *multiply(const Value &v) const;
  virtual Value *divide(const Value &v) const;

  virtual BooleanValue *equals(const Value &v) const;
  virtual BooleanValue *lessThan(const Value &v) const;

  void setType(const NumberValue &n);

  std::string getUnit() const;
  void setUnit(std::string unit);
  double getValue() const;
  void setValue(double d);

  static void loadFunctions(FunctionLibrary &lib);
  static Value *unit(const vector<const Value *> &args);
  static Value *get_unit(const vector<const Value *> &args);
  static Value *is_unit(const vector<const Value *> &args);
  static Value *ceil(const vector<const Value *> &args);
  static Value *floor(const vector<const Value *> &args);
  static Value *percentage(const vector<const Value *> &args);
  static Value *round(const vector<const Value *> &args);
  static Value *sqrt(const vector<const Value *> &args);
  static Value *abs(const vector<const Value *> &args);
  static Value *sin(const vector<const Value *> &args);
  static Value *asin(const vector<const Value *> &args);
  static Value *cos(const vector<const Value *> &args);
  static Value *acos(const vector<const Value *> &args);
  static Value *tan(const vector<const Value *> &args);
  static Value *atan(const vector<const Value *> &args);
  static Value *pi(const vector<const Value *> &args);
  static Value *pow(const vector<const Value *> &args);
  static Value *mod(const vector<const Value *> &args);
  static Value *convert(const vector<const Value *> &args);

  static Value *is_number(const vector<const Value *> &args);
  static Value *is_string(const vector<const Value *> &args);
  static Value *is_color(const vector<const Value *> &args);
  static Value *is_keyword(const vector<const Value *> &args);
  static Value *is_url(const vector<const Value *> &args);
  static Value *is_pixel(const vector<const Value *> &args);
  static Value *is_em(const vector<const Value *> &args);
  static Value *is_percentage(const vector<const Value *> &args);
};

#endif  // __less_value_NumberValue_h__
