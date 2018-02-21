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

  virtual Value *operator+(const Value &v) const;
  virtual Value *operator-(const Value &v) const;
  virtual Value *operator*(const Value &v) const;
  virtual Value *operator/(const Value &v) const;

  virtual bool operator<(const Value &v) const;
  virtual bool operator==(const Value &v) const;

  unsigned int getImageWidth() const;
  unsigned int getImageHeight() const;
  Color getImageBackground() const;

};

#endif  // __less_value_UrlValue_h__
