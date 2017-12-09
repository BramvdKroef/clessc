
#ifndef __StringValue_h__
#define __StringValue_h__
#include <vector>
#include <sstream>
#include <iomanip>
#include <less/value/Value.h>
#include <less/value/Color.h>
#include <less/value/FunctionLibrary.h>

class StringValue: public Value {
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

  virtual Value* add(const Value &v) const;
  virtual Value* substract(const Value &v) const;
  virtual Value* multiply(const Value &v) const;
  virtual Value* divide(const Value &v) const;

  virtual BooleanValue* equals(const Value &v) const;
  virtual BooleanValue* lessThan(const Value &v) const;

  static string escape(string rawstr, string extraUnreserved = "");

  static void loadFunctions(FunctionLibrary &lib);
  static Value* escape(const vector<const Value*> &arguments);
  static Value* e(const vector<const Value*> &arguments);
  static Value* format(const vector<const Value*> &arguments);
  static Value* replace(const vector<const Value*> &arguments);
  static Value* color(const vector<const Value*> &arguments);
  static Value* data_uri(const vector<const Value*> &arguments);
};
#endif
