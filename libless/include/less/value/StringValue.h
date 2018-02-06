#ifndef __less_value_StringValue_h__
#define __less_value_StringValue_h__

#include <iomanip>
#include <sstream>
#include <vector>
#include "less/value/Color.h"
#include "less/value/Value.h"

class FunctionLibrary;

class StringValue : public Value {
private:
  std::string strvalue;
  bool quotes;

  void updateTokens();

public:
  StringValue(const Token &token, bool quotes);
  StringValue(const std::string &str, bool quotes);
  StringValue(const StringValue &s);
  StringValue(const Value &val, bool quotes);

  virtual ~StringValue();

  std::string getString() const;
  void setString(const std::string &stringValue);

  void setQuotes(bool quotes);
  bool getQuotes() const;

  void append(const Value &v);

  virtual Value *add(const Value &v) const;
  virtual Value *substract(const Value &v) const;
  virtual Value *multiply(const Value &v) const;
  virtual Value *divide(const Value &v) const;

  virtual BooleanValue *equals(const Value &v) const;
  virtual BooleanValue *lessThan(const Value &v) const;

  static std::string escape(std::string rawstr, std::string extraUnreserved = "");

  static void loadFunctions(FunctionLibrary &lib);
  static Value *escape(const std::vector<const Value *> &arguments);
  static Value *e(const std::vector<const Value *> &arguments);
  static Value *format(const std::vector<const Value *> &arguments);
  static Value *replace(const std::vector<const Value *> &arguments);
  static Value *color(const std::vector<const Value *> &arguments);
  static Value *data_uri(const std::vector<const Value *> &arguments);
};

#endif  // __less_value_StringValue_h__
