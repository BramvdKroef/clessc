#ifndef __less_value_NumberFunctions_h__
#define __less_value_NumberFunctions_h__

#include <vector>
class Value;
class FunctionLibrary;

class NumberFunctions {
public:
  static void loadFunctions(FunctionLibrary &lib);
  static Value *unit(const std::vector<const Value *> &args);
  static Value *get_unit(const std::vector<const Value *> &args);
  static Value *is_unit(const std::vector<const Value *> &args);
  static Value *ceil(const std::vector<const Value *> &args);
  static Value *floor(const std::vector<const Value *> &args);
  static Value *percentage(const std::vector<const Value *> &args);
  static Value *round(const std::vector<const Value *> &args);
  static Value *sqrt(const std::vector<const Value *> &args);
  static Value *abs(const std::vector<const Value *> &args);
  static Value *sin(const std::vector<const Value *> &args);
  static Value *asin(const std::vector<const Value *> &args);
  static Value *cos(const std::vector<const Value *> &args);
  static Value *acos(const std::vector<const Value *> &args);
  static Value *tan(const std::vector<const Value *> &args);
  static Value *atan(const std::vector<const Value *> &args);
  static Value *pi(const std::vector<const Value *> &args);
  static Value *pow(const std::vector<const Value *> &args);
  static Value *mod(const std::vector<const Value *> &args);
  static Value *convert(const std::vector<const Value *> &args);

  static Value *min(const std::vector<const Value *> &args);
  static Value *max(const std::vector<const Value *> &args);
  static Value *is_number(const std::vector<const Value *> &args);
  static Value *is_string(const std::vector<const Value *> &args);
  static Value *is_color(const std::vector<const Value *> &args);
  static Value *is_keyword(const std::vector<const Value *> &args);
  static Value *is_url(const std::vector<const Value *> &args);
  static Value *is_pixel(const std::vector<const Value *> &args);
  static Value *is_em(const std::vector<const Value *> &args);
  static Value *is_percentage(const std::vector<const Value *> &args);
};

#endif  // __less_value_NumberFunctions_h__
