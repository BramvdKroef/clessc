#ifndef __less_value_UrlValue_h__
#define __less_value_UrlValue_h__

#include <string>
#include "less/value/Color.h"
#include "less/value/Value.h"

class UrlValue_Img {
public:
  unsigned int width;
  unsigned int height;
  Color background;

  UrlValue_Img();
};

class UrlValue : public Value {
private:
  std::string path;

  bool loadImg(UrlValue_Img &img) const;
  bool loadPng(UrlValue_Img &img) const;
  bool loadJpeg(UrlValue_Img &img) const;

public:
  UrlValue(Token &token, std::string &path);

  virtual ~UrlValue();

  std::string getPath() const;

  std::string getRelativePath() const;

  virtual Value *add(const Value &v) const;
  virtual Value *substract(const Value &v) const;
  virtual Value *multiply(const Value &v) const;
  virtual Value *divide(const Value &v) const;

  virtual BooleanValue *lessThan(const Value &v) const;
  virtual BooleanValue *equals(const Value &v) const;

  unsigned int getImageWidth() const;
  unsigned int getImageHeight() const;
  Color getImageBackground() const;

  static void loadFunctions(FunctionLibrary &lib);

  static Value *imgheight(const vector<const Value *> &arguments);
  static Value *imgwidth(const vector<const Value *> &arguments);
  static Value *imgbackground(const vector<const Value *> &arguments);
};

#endif  // __less_value_UrlValue_h__
