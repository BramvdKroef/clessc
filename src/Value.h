#ifndef __Value_h__
#define __Value_h__

#include "Token.h"
#include "ParseException.h"
#include <sstream>
using namespace std;

/**
 * 
 */
class Value {
protected:
  Token* token;
  
public:
  enum Type {NUMBER, PERCENTAGE, DIMENSION, COLOR} type;
  Value();
  Value(Token* token);
  virtual ~Value();
  
  virtual Token* getToken();
  
  bool add(Value* v);
  bool substract(Value* v);
  bool multiply(Value* v);
  bool divide(Value* v);
  double getValue();
  int getPercent();
  string getUnit();
  void setValue(double d);
};

class Color: public Value {
private:
  unsigned int color[3];
public:
  Color(Token* token);
  Color(unsigned int red, unsigned int green, unsigned int blue);
  virtual ~Color();

  Token* getToken();
  
  void setHSL(unsigned int hue, unsigned int saturation,
              unsigned int lightness);
  void lighten(unsigned int percent);
  void darken(unsigned int percent);
  void saturate(unsigned int percent);
  void desaturate(unsigned int percent);
  void fadein(unsigned int percent);
  void fadeout(unsigned int percent);
  void spin(unsigned int percent);

  int getHue();
  int getSaturation();
  int getLightness();
};

#endif
