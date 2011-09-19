#ifndef __Color_h__
#define __Color_h__

#include "Value.h"
#include <algorithm>
using namespace std;

#define RGB_RED 0
#define RGB_GREEN 1
#define RGB_BLUE 2
#define abs(x) (x < 0 ? 0 - x : x)

class Color: public Value {
private:
  unsigned int color[3];

  double maxArray(double* array, int len);
  double minArray(double* array, int len);
public:
  Color(Token* token);
  Color(unsigned int red, unsigned int green, unsigned int blue);
  virtual ~Color();

  Token* getToken();
  
  bool add(Value* v);
  bool substract(Value* v);
  bool multiply(Value* v);
  bool divide(Value* v);
  
  void setHSL(int hue, double saturation,
              double lightness);
  void lighten(double percent);
  void darken(double percent);
  void saturate(double percent);
  void desaturate(double percent);
  void fadein(double percent);
  void fadeout(double percent);
  void spin(double degrees);

  unsigned int getRed();
  unsigned int getGreen();
  unsigned int getBlue();
  double* getHSL();
  double getHue();
  double getSaturation();
  double getLightness();
};

#endif
