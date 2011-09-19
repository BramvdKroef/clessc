#ifndef __Color_h__
#define __Color_h__

class Color: public Value {
private:
  unsigned int color[3];
  const int RED = 0, GREEN = 1, BLUE = 2;
public:
  Color(Token* token);
  Color(unsigned int red, unsigned int green, unsigned int blue);
  virtual ~Color();

  Token* getToken();
  
  bool add(Value* v);
  bool substract(Value* v);
  bool multiply(Value* v);
  bool divide(Value* v);
  
  void setHSL(double hue, unsigned double saturation,
              unsigned double lightness);
  void lighten(unsigned double percent);
  void darken(unsigned double percent);
  void saturate(unsigned double percent);
  void desaturate(unsigned double percent);
  void fadein(unsigned double percent);
  void fadeout(unsigned double percent);
  void spin(double degrees);

  unsigned int getRed();
  unsigned int getGreen();
  unsigned int getBlue();
  unsigned double[] getHSL();
  unsigned double getHue();
  unsigned double getSaturation();
  unsigned double getLightness();
};

#endif
