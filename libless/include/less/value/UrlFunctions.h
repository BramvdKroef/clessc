#ifndef __less_value_UrlFunctions_h__
#define __less_value_UrlFunctions_h__

#include <vector>

class Value;
class FunctionLibrary;

class UrlFunctions {
public:
  static void loadFunctions(FunctionLibrary &lib);

  static Value *imgheight(const std::vector<const Value *> &arguments);
  static Value *imgwidth(const std::vector<const Value *> &arguments);
  static Value *imgbackground(const std::vector<const Value *> &arguments);

};
  
#endif  // __less_value_UrlFunctions_h__
