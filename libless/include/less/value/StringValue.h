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

  virtual Value *operator+(const Value &v) const;
  virtual Value *operator-(const Value &v) const;
  virtual Value *operator*(const Value &v) const;
  virtual Value *operator/(const Value &v) const;

  virtual bool operator==(const Value &v) const;
  virtual bool operator<(const Value &v) const;

  static std::string escape(std::string rawstr, std::string extraUnreserved = "");

};

#endif  // __less_value_StringValue_h__
