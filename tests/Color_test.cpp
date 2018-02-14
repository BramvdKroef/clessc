#include <gtest/gtest.h>
#include <less/value/ValueProcessor.h>
#include <less/lessstylesheet/ProcessingContext.h>

class ColorTest : public ::testing::Test {
public:
  TokenList l;
  ValueProcessor vp;
  ProcessingContext c;

  void rgb(const char* r, const char* g, const char* b) {
    l.push_back(Token("rgb", Token::IDENTIFIER, 0, 0, "-"));
    l.push_back(Token("(", Token::PAREN_OPEN, 0, 0, "-"));
    l.push_back(Token(r, Token::NUMBER, 0, 0, "-"));
    l.push_back(Token(",", Token::DELIMITER, 0, 0, "-"));
    l.push_back(Token(g, Token::NUMBER, 0, 0, "-"));
    l.push_back(Token(",", Token::DELIMITER, 0, 0, "-"));
    l.push_back(Token(b, Token::NUMBER, 0, 0, "-"));
    l.push_back(Token(")", Token::PAREN_CLOSED, 0, 0, "-"));
  }

  void rgba(const char* r, const char* g, const char* b,
            const char* a) {
    l.push_back(Token("rgba", Token::IDENTIFIER,0, 0, "-"));
    l.push_back(Token("(", Token::PAREN_OPEN,0, 0, "-"));
    l.push_back(Token(r, Token::NUMBER,0, 0, "-"));
    l.push_back(Token(",", Token::DELIMITER,0, 0, "-"));
    l.push_back(Token(g, Token::NUMBER,0, 0, "-"));
    l.push_back(Token(",", Token::DELIMITER,0, 0, "-"));
    l.push_back(Token(b, Token::NUMBER,0, 0, "-"));
    l.push_back(Token(",", Token::DELIMITER,0, 0, "-"));
    l.push_back(Token(a, Token::PERCENTAGE,0, 0, "-"));
    l.push_back(Token(")", Token::PAREN_CLOSED,0, 0, "-"));
  }

  void hsl(const char* h, const char* s, const char* lu) {
    l.push_back(Token("hsl", Token::IDENTIFIER,0, 0, "-"));
    l.push_back(Token("(", Token::PAREN_OPEN,0, 0, "-"));
    l.push_back(Token(h, Token::NUMBER,0, 0, "-"));
    l.push_back(Token(",", Token::DELIMITER,0, 0, "-"));
    l.push_back(Token(s, Token::PERCENTAGE,0, 0, "-"));
    l.push_back(Token(",", Token::DELIMITER,0, 0, "-"));
    l.push_back(Token(lu, Token::PERCENTAGE, 0, 0, "-"));
    l.push_back(Token(")", Token::PAREN_CLOSED,0, 0, "-"));
  }

  void hsla(const char* h, const char* s, const char* lu,
            const char* a) {
    l.push_back(Token("hsla", Token::IDENTIFIER,0, 0, "-"));
    l.push_back(Token("(", Token::PAREN_OPEN,0, 0, "-"));
    l.push_back(Token(h, Token::NUMBER,0, 0, "-"));
    l.push_back(Token(",", Token::DELIMITER,0, 0, "-"));
    l.push_back(Token(s, Token::PERCENTAGE,0, 0, "-"));
    l.push_back(Token(",", Token::DELIMITER,0, 0, "-"));
    l.push_back(Token(lu, Token::PERCENTAGE, 0, 0, "-"));
    l.push_back(Token(",", Token::DELIMITER,0, 0, "-"));
    l.push_back(Token(a, Token::NUMBER, 0, 0, "-"));
    l.push_back(Token(")", Token::PAREN_CLOSED,0, 0, "-"));
  }

  void blend(const char* fn, const char* c1, const char* c2) {
    l.push_back(Token(fn, Token::IDENTIFIER,0, 0, "-"));
    l.push_back(Token("(", Token::PAREN_OPEN,0, 0, "-"));
    l.push_back(Token(c1, Token::HASH,0, 0, "-"));
    l.push_back(Token(",", Token::DELIMITER,0, 0, "-"));
    l.push_back(Token(c2, Token::HASH,0, 0, "-"));
    l.push_back(Token(")", Token::PAREN_CLOSED,0, 0, "-"));
  }

  void blendtests(const char* fn, const char* out[]) {
    const char* in[] = {
      "#000000",
      "#333333",
      "#666666",
      "#999999",
      "#cccccc",
      "#ffffff",
      "#ff0000",
      "#00ff00",
      "#0000ff"};
    for (size_t i=0; i < 9; i++) {
      l.clear();
      blend(fn, "#ff6600", in[i]);
      vp.processValue(l, c);

      ASSERT_EQ((uint)1, l.size());
      EXPECT_EQ(out[i], l.toString());
    }
  }
};
  
TEST_F(ColorTest, RGB) {
  rgb("10", "10", "10");

  vp.processValue(l, c);
  
  ASSERT_EQ((uint)1, l.size());
  EXPECT_EQ("#0a0a0a", l.front());
  ASSERT_EQ(Token::HASH, l.front().type);
}

TEST_F(ColorTest, RGBA) {
  rgba("10", "10", "10", "10%");

  vp.processValue(l, c);

  ASSERT_EQ((uint)13, l.size());
  EXPECT_EQ("rgba(10, 10, 10, 0.1)", l.toString());
}

TEST_F(ColorTest, ARGB) {
  l.push_back(Token("argb", Token::IDENTIFIER,0, 0, "-"));
  l.push_back(Token("(", Token::PAREN_OPEN,0, 0, "-"));
  rgba("10", "10", "10", ".4");
  l.push_back(Token(")", Token::PAREN_CLOSED,0, 0, "-"));

  vp.processValue(l, c);

  ASSERT_EQ((uint)1, l.size());
  EXPECT_EQ("#100a0a0a", l.toString());
}

TEST_F(ColorTest, HSL) {
  
  hsl("90", "100%", "50%");

  vp.processValue(l, c);

  ASSERT_EQ((uint)1, l.size());
  EXPECT_EQ("#80ff00", l.toString());
}

TEST_F(ColorTest, HSLA) {

  hsla("90", "100%", "50%", ".5");

  vp.processValue(l, c);

  EXPECT_EQ((uint)13, l.size());
  EXPECT_EQ("rgba(128, 255, 0, 0.5)", l.toString());
}

TEST_F(ColorTest, HSV) {
  l.push_back(Token("hsv", Token::IDENTIFIER,0, 0, "-"));
  l.push_back(Token("(", Token::PAREN_OPEN,0, 0, "-"));
  l.push_back(Token("90", Token::NUMBER,0, 0, "-"));
  l.push_back(Token(",", Token::DELIMITER,0, 0, "-"));
  l.push_back(Token("100%", Token::PERCENTAGE,0, 0, "-"));
  l.push_back(Token(",", Token::DELIMITER,0, 0, "-"));
  l.push_back(Token("50%", Token::PERCENTAGE, 0, 0, "-"));
  l.push_back(Token(")", Token::PAREN_CLOSED,0, 0, "-"));

  vp.processValue(l, c);

  ASSERT_EQ((uint)1, l.size());
  EXPECT_EQ("#408000", l.toString());
}

TEST_F(ColorTest, HSVA) {
  l.push_back(Token("hsva", Token::IDENTIFIER,0, 0, "-"));
  l.push_back(Token("(", Token::PAREN_OPEN,0, 0, "-"));
  l.push_back(Token("90", Token::NUMBER,0, 0, "-"));
  l.push_back(Token(",", Token::DELIMITER,0, 0, "-"));
  l.push_back(Token("100%", Token::PERCENTAGE,0, 0, "-"));
  l.push_back(Token(",", Token::DELIMITER,0, 0, "-"));
  l.push_back(Token("50%", Token::PERCENTAGE, 0, 0, "-"));
  l.push_back(Token(",", Token::DELIMITER,0, 0, "-"));
  l.push_back(Token(".5", Token::NUMBER, 0, 0, "-"));
  l.push_back(Token(")", Token::PAREN_CLOSED,0, 0, "-"));

  vp.processValue(l, c);

  ASSERT_EQ((uint)13, l.size());
  EXPECT_EQ("rgba(64, 128, 0, 0.5)", l.toString());
}

TEST_F(ColorTest, Hue) {
  l.push_back(Token("hue", Token::IDENTIFIER,0, 0, "-"));
  l.push_back(Token("(", Token::PAREN_OPEN,0, 0, "-"));
  hsl("90", "100%", "50%");
  l.push_back(Token(")", Token::PAREN_CLOSED,0, 0, "-"));

  vp.processValue(l, c);

  ASSERT_EQ((uint)1, l.size());
  EXPECT_EQ("90", l.toString());
}

TEST_F(ColorTest, Saturation) {
  l.push_back(Token("saturation", Token::IDENTIFIER,0, 0, "-"));
  l.push_back(Token("(", Token::PAREN_OPEN,0, 0, "-"));
  hsl("90", "100%", "50%");
  l.push_back(Token(")", Token::PAREN_CLOSED,0, 0, "-"));

  vp.processValue(l, c);

  ASSERT_EQ((uint)1, l.size());
  EXPECT_EQ("100%", l.toString());
}

TEST_F(ColorTest, Lightness) {
  l.push_back(Token("lightness", Token::IDENTIFIER,0, 0, "-"));
  l.push_back(Token("(", Token::PAREN_OPEN,0, 0, "-"));
  hsl("90", "100%", "50%");
  l.push_back(Token(")", Token::PAREN_CLOSED,0, 0, "-"));

  vp.processValue(l, c);

  ASSERT_EQ((uint)1, l.size());
  EXPECT_EQ("50%", l.toString());
}

TEST_F(ColorTest, HSVHue) {
  l.push_back(Token("hsvhue", Token::IDENTIFIER,0, 0, "-"));
  l.push_back(Token("(", Token::PAREN_OPEN,0, 0, "-"));
  l.push_back(Token("hsv", Token::IDENTIFIER,0, 0, "-"));
  l.push_back(Token("(", Token::PAREN_OPEN,0, 0, "-"));
  l.push_back(Token("90", Token::NUMBER,0, 0, "-"));
  l.push_back(Token(",", Token::DELIMITER,0, 0, "-"));
  l.push_back(Token("100%", Token::PERCENTAGE,0, 0, "-"));
  l.push_back(Token(",", Token::DELIMITER,0, 0, "-"));
  l.push_back(Token("50%", Token::PERCENTAGE, 0, 0, "-"));
  l.push_back(Token(")", Token::PAREN_CLOSED,0, 0, "-"));
  l.push_back(Token(")", Token::PAREN_CLOSED,0, 0, "-"));

  vp.processValue(l, c);

  ASSERT_EQ((uint)1, l.size());
  EXPECT_EQ("90", l.toString());
}

TEST_F(ColorTest, HSVSaturation) {
  l.push_back(Token("hsvsaturation", Token::IDENTIFIER,0, 0, "-"));
  l.push_back(Token("(", Token::PAREN_OPEN,0, 0, "-"));
  l.push_back(Token("hsv", Token::IDENTIFIER,0, 0, "-"));
  l.push_back(Token("(", Token::PAREN_OPEN,0, 0, "-"));
  l.push_back(Token("90", Token::NUMBER,0, 0, "-"));
  l.push_back(Token(",", Token::DELIMITER,0, 0, "-"));
  l.push_back(Token("100%", Token::PERCENTAGE,0, 0, "-"));
  l.push_back(Token(",", Token::DELIMITER,0, 0, "-"));
  l.push_back(Token("50%", Token::PERCENTAGE, 0, 0, "-"));
  l.push_back(Token(")", Token::PAREN_CLOSED,0, 0, "-"));
  l.push_back(Token(")", Token::PAREN_CLOSED,0, 0, "-"));

  vp.processValue(l, c);

  ASSERT_EQ((uint)1, l.size());
  EXPECT_EQ("100%", l.toString());
}

TEST_F(ColorTest, HSVValue) {
  l.push_back(Token("hsvvalue", Token::IDENTIFIER,0, 0, "-"));
  l.push_back(Token("(", Token::PAREN_OPEN,0, 0, "-"));
  l.push_back(Token("hsv", Token::IDENTIFIER,0, 0, "-"));
  l.push_back(Token("(", Token::PAREN_OPEN,0, 0, "-"));
  l.push_back(Token("90", Token::NUMBER,0, 0, "-"));
  l.push_back(Token(",", Token::DELIMITER,0, 0, "-"));
  l.push_back(Token("100%", Token::PERCENTAGE,0, 0, "-"));
  l.push_back(Token(",", Token::DELIMITER,0, 0, "-"));
  l.push_back(Token("50%", Token::PERCENTAGE, 0, 0, "-"));
  l.push_back(Token(")", Token::PAREN_CLOSED,0, 0, "-"));
  l.push_back(Token(")", Token::PAREN_CLOSED,0, 0, "-"));

  vp.processValue(l, c);

  ASSERT_EQ((uint)1, l.size());
  EXPECT_EQ("50%", l.toString());
}

TEST_F(ColorTest, Red) {
  l.push_back(Token("red", Token::IDENTIFIER,0, 0, "-"));
  l.push_back(Token("(", Token::PAREN_OPEN,0, 0, "-"));
  rgb("10", "20", "30");
  l.push_back(Token(")", Token::PAREN_CLOSED,0, 0, "-"));

  vp.processValue(l, c);

  ASSERT_EQ((uint)1, l.size());
  EXPECT_EQ("10", l.toString());
}

TEST_F(ColorTest, Green) {
  l.push_back(Token("green", Token::IDENTIFIER,0, 0, "-"));
  l.push_back(Token("(", Token::PAREN_OPEN,0, 0, "-"));
  rgb("10", "20", "30");
  l.push_back(Token(")", Token::PAREN_CLOSED,0, 0, "-"));

  vp.processValue(l, c);

  ASSERT_EQ((uint)1, l.size());
  EXPECT_EQ("20", l.toString());
}

TEST_F(ColorTest, Blue) {
  l.push_back(Token("blue", Token::IDENTIFIER,0, 0, "-"));
  l.push_back(Token("(", Token::PAREN_OPEN,0, 0, "-"));
  rgb("10", "20", "30");
  l.push_back(Token(")", Token::PAREN_CLOSED,0, 0, "-"));

  vp.processValue(l, c);

  ASSERT_EQ((uint)1, l.size());
  EXPECT_EQ("30", l.toString());
}

TEST_F(ColorTest, Alpha) {
  l.push_back(Token("alpha", Token::IDENTIFIER,0, 0, "-"));
  l.push_back(Token("(", Token::PAREN_OPEN,0, 0, "-"));
  rgba("10", "20", "30", ".5");
  l.push_back(Token(")", Token::PAREN_CLOSED,0, 0, "-"));

  vp.processValue(l, c);

  ASSERT_EQ((uint)1, l.size());
  EXPECT_EQ("0.5", l.toString());
}

TEST_F(ColorTest, Luma) {
  l.push_back(Token("luma", Token::IDENTIFIER,0, 0, "-"));
  l.push_back(Token("(", Token::PAREN_OPEN,0, 0, "-"));
  rgb("100", "200", "30");
  l.push_back(Token(")", Token::PAREN_CLOSED,0, 0, "-"));

  vp.processValue(l, c);

  ASSERT_EQ((uint)1, l.size());
  EXPECT_EQ("44%", l.toString());
}

TEST_F(ColorTest, Luminance) {
  l.push_back(Token("luminance", Token::IDENTIFIER,0, 0, "-"));
  l.push_back(Token("(", Token::PAREN_OPEN,0, 0, "-"));
  rgb("100", "200", "30");
  l.push_back(Token(")", Token::PAREN_CLOSED,0, 0, "-"));

  vp.processValue(l, c);

  ASSERT_EQ((uint)1, l.size());
  EXPECT_EQ("65%", l.toString());
}

TEST_F(ColorTest, Saturate) {
  l.push_back(Token("saturate", Token::IDENTIFIER,0, 0, "-"));
  l.push_back(Token("(", Token::PAREN_OPEN,0, 0, "-"));
  hsl("90", "80%", "50%");
  l.push_back(Token(",", Token::DELIMITER,0, 0, "-"));
  l.push_back(Token("20%", Token::PERCENTAGE, 0, 0, "-"));
  l.push_back(Token(")", Token::PAREN_CLOSED,0, 0, "-"));

  vp.processValue(l, c);

  ASSERT_EQ((uint)1, l.size());
  EXPECT_EQ("#80ff00", l.toString());
}

TEST_F(ColorTest, Desaturate) {
  l.push_back(Token("desaturate", Token::IDENTIFIER,0, 0, "-"));
  l.push_back(Token("(", Token::PAREN_OPEN,0, 0, "-"));
  hsl("90", "80%", "50%");
  l.push_back(Token(",", Token::DELIMITER,0, 0, "-"));
  l.push_back(Token("20%", Token::PERCENTAGE, 0, 0, "-"));
  l.push_back(Token(")", Token::PAREN_CLOSED,0, 0, "-"));

  vp.processValue(l, c);

  ASSERT_EQ((uint)1, l.size());
  EXPECT_EQ("#80cc33", l.toString());
}

TEST_F(ColorTest, Lighten) {
  l.push_back(Token("lighten", Token::IDENTIFIER,0, 0, "-"));
  l.push_back(Token("(", Token::PAREN_OPEN,0, 0, "-"));
  hsl("90", "80%", "50%");
  l.push_back(Token(",", Token::DELIMITER,0, 0, "-"));
  l.push_back(Token("20%", Token::PERCENTAGE, 0, 0, "-"));
  l.push_back(Token(")", Token::PAREN_CLOSED,0, 0, "-"));

  vp.processValue(l, c);

  ASSERT_EQ((uint)1, l.size());
  EXPECT_EQ("#b3f075", l.toString());
}

TEST_F(ColorTest, Darken) {
  l.push_back(Token("darken", Token::IDENTIFIER,0, 0, "-"));
  l.push_back(Token("(", Token::PAREN_OPEN,0, 0, "-"));
  hsl("90", "80%", "50%");
  l.push_back(Token(",", Token::DELIMITER,0, 0, "-"));
  l.push_back(Token("20%", Token::PERCENTAGE, 0, 0, "-"));
  l.push_back(Token(")", Token::PAREN_CLOSED,0, 0, "-"));

  vp.processValue(l, c);

  ASSERT_EQ((uint)1, l.size());
  EXPECT_EQ("#4d8a0f", l.toString());
}

TEST_F(ColorTest, FadeIn) {
  l.push_back(Token("fadein", Token::IDENTIFIER,0, 0, "-"));
  l.push_back(Token("(", Token::PAREN_OPEN,0, 0, "-"));
  hsla("90", "80%", "50%", ".5");
  l.push_back(Token(",", Token::DELIMITER,0, 0, "-"));
  l.push_back(Token("10%", Token::PERCENTAGE, 0, 0, "-"));
  l.push_back(Token(")", Token::PAREN_CLOSED,0, 0, "-"));

  vp.processValue(l, c);

  ASSERT_EQ((uint)13, l.size());
  EXPECT_EQ("rgba(128, 242, 13, .6)", l.toString());
}

TEST_F(ColorTest, FadeOut) {
  l.push_back(Token("fadeout", Token::IDENTIFIER,0, 0, "-"));
  l.push_back(Token("(", Token::PAREN_OPEN,0, 0, "-"));
  hsla("90", "80%", "50%", ".5");
  l.push_back(Token(",", Token::DELIMITER,0, 0, "-"));
  l.push_back(Token("10%", Token::PERCENTAGE, 0, 0, "-"));
  l.push_back(Token(")", Token::PAREN_CLOSED,0, 0, "-"));

  vp.processValue(l, c);

  ASSERT_EQ((uint)13, l.size());
  EXPECT_EQ("rgba(128, 242, 13, .4)", l.toString());
}

TEST_F(ColorTest, Fade) {
  l.push_back(Token("fade", Token::IDENTIFIER,0, 0, "-"));
  l.push_back(Token("(", Token::PAREN_OPEN,0, 0, "-"));
  hsl("90", "80%", "50%");
  l.push_back(Token(",", Token::DELIMITER,0, 0, "-"));
  l.push_back(Token("10%", Token::PERCENTAGE, 0, 0, "-"));
  l.push_back(Token(")", Token::PAREN_CLOSED,0, 0, "-"));

  vp.processValue(l, c);

  ASSERT_EQ((uint)13, l.size());
  EXPECT_EQ("rgba(128, 242, 13, .1)", l.toString());
}

TEST_F(ColorTest, Spin) {
  l.push_back(Token("spin", Token::IDENTIFIER,0, 0, "-"));
  l.push_back(Token("(", Token::PAREN_OPEN,0, 0, "-"));
  hsl("10", "90%", "50%");
  l.push_back(Token(",", Token::DELIMITER,0, 0, "-"));
  l.push_back(Token("30", Token::NUMBER, 0, 0, "-"));
  l.push_back(Token(")", Token::PAREN_CLOSED,0, 0, "-"));

  vp.processValue(l, c);

  ASSERT_EQ((uint)1, l.size());
  EXPECT_EQ("#f2a60d", l.toString());

  l.clear();
  l.push_back(Token("spin", Token::IDENTIFIER,0, 0, "-"));
  l.push_back(Token("(", Token::PAREN_OPEN,0, 0, "-"));
  hsl("10", "90%", "50%");
  l.push_back(Token(",", Token::DELIMITER,0, 0, "-"));
  l.push_back(Token("-30", Token::NUMBER, 0, 0, "-"));
  l.push_back(Token(")", Token::PAREN_CLOSED,0, 0, "-"));

  vp.processValue(l, c);

  ASSERT_EQ((uint)1, l.size());
  EXPECT_EQ("#f20d59", l.toString());
}

TEST_F(ColorTest, Mix) {
  l.push_back(Token("mix", Token::IDENTIFIER,0, 0, "-"));
  l.push_back(Token("(", Token::PAREN_OPEN,0, 0, "-"));
  l.push_back(Token("#ff0000", Token::HASH,0, 0, "-"));
  l.push_back(Token(",", Token::DELIMITER,0, 0, "-"));
  l.push_back(Token("#0000ff", Token::HASH,0, 0, "-"));
  l.push_back(Token(",", Token::DELIMITER,0, 0, "-"));
  l.push_back(Token("50%", Token::PERCENTAGE, 0, 0, "-"));
  l.push_back(Token(")", Token::PAREN_CLOSED,0, 0, "-"));

  vp.processValue(l, c);

  ASSERT_EQ((uint)1, l.size());
  EXPECT_EQ("#800080", l.toString());

  l.clear();
  l.push_back(Token("mix", Token::IDENTIFIER,0, 0, "-"));
  l.push_back(Token("(", Token::PAREN_OPEN,0, 0, "-"));
  rgba("100", "0", "0", "1.0");
  l.push_back(Token(",", Token::DELIMITER,0, 0, "-"));
  rgba("0", "100", "0", ".5");
  l.push_back(Token(",", Token::DELIMITER,0, 0, "-"));
  l.push_back(Token("50%", Token::PERCENTAGE, 0, 0, "-"));
  l.push_back(Token(")", Token::PAREN_CLOSED,0, 0, "-"));

  vp.processValue(l, c);

  ASSERT_EQ((uint)13, l.size());
  EXPECT_EQ("rgba(75, 25, 0, 0.75)", l.toString());
}

TEST_F(ColorTest, Tint) {
  l.push_back(Token("tint", Token::IDENTIFIER,0, 0, "-"));
  l.push_back(Token("(", Token::PAREN_OPEN,0, 0, "-"));
  l.push_back(Token("#007fff", Token::HASH,0, 0, "-"));
  l.push_back(Token(",", Token::DELIMITER,0, 0, "-"));
  l.push_back(Token("50%", Token::PERCENTAGE, 0, 0, "-"));
  l.push_back(Token(")", Token::PAREN_CLOSED,0, 0, "-"));

  vp.processValue(l, c);

  ASSERT_EQ((uint)1, l.size());
  EXPECT_EQ("#80bfff", l.toString());

  l.clear();
  l.push_back(Token("tint", Token::IDENTIFIER,0, 0, "-"));
  l.push_back(Token("(", Token::PAREN_OPEN,0, 0, "-"));
  rgba("0", "0", "255", ".5");
  l.push_back(Token(",", Token::DELIMITER,0, 0, "-"));
  l.push_back(Token("50%", Token::PERCENTAGE, 0, 0, "-"));
  l.push_back(Token(")", Token::PAREN_CLOSED,0, 0, "-"));

  vp.processValue(l, c);

  ASSERT_EQ((uint)13, l.size());
  EXPECT_EQ("rgba(191, 191, 255, 0.75)", l.toString());
}

TEST_F(ColorTest, Shade) {
  l.push_back(Token("shade", Token::IDENTIFIER,0, 0, "-"));
  l.push_back(Token("(", Token::PAREN_OPEN,0, 0, "-"));
  l.push_back(Token("#007fff", Token::HASH,0, 0, "-"));
  l.push_back(Token(",", Token::DELIMITER,0, 0, "-"));
  l.push_back(Token("50%", Token::PERCENTAGE, 0, 0, "-"));
  l.push_back(Token(")", Token::PAREN_CLOSED,0, 0, "-"));

  vp.processValue(l, c);

  ASSERT_EQ((uint)1, l.size());
  EXPECT_EQ("#004080", l.toString());

  l.clear();
  l.push_back(Token("shade", Token::IDENTIFIER,0, 0, "-"));
  l.push_back(Token("(", Token::PAREN_OPEN,0, 0, "-"));
  rgba("0", "0", "255", ".5");
  l.push_back(Token(",", Token::DELIMITER,0, 0, "-"));
  l.push_back(Token("50%", Token::PERCENTAGE, 0, 0, "-"));
  l.push_back(Token(")", Token::PAREN_CLOSED,0, 0, "-"));

  vp.processValue(l, c);

  ASSERT_EQ((uint)13, l.size());
  EXPECT_EQ("rgba(0, 0, 64, 0.75)", l.toString());
}

TEST_F(ColorTest, Greyscale) {
  l.push_back(Token("greyscale", Token::IDENTIFIER,0, 0, "-"));
  l.push_back(Token("(", Token::PAREN_OPEN,0, 0, "-"));
  hsl("90", "90%", "50%");
  l.push_back(Token(")", Token::PAREN_CLOSED,0, 0, "-"));

  vp.processValue(l, c);

  ASSERT_EQ((uint)1, l.size());
  EXPECT_EQ("#808080", l.toString());
}

TEST_F(ColorTest, Contrast) {
  l.push_back(Token("contrast", Token::IDENTIFIER,0, 0, "-"));
  l.push_back(Token("(", Token::PAREN_OPEN,0, 0, "-"));
  l.push_back(Token("#bbbbbb", Token::HASH,0, 0, "-"));
  l.push_back(Token(")", Token::PAREN_CLOSED,0, 0, "-"));

  vp.processValue(l, c);

  ASSERT_EQ((uint)1, l.size());
  EXPECT_EQ("#000000", l.toString());

  l.clear();
  l.push_back(Token("contrast", Token::IDENTIFIER,0, 0, "-"));
  l.push_back(Token("(", Token::PAREN_OPEN,0, 0, "-"));
  l.push_back(Token("#222222", Token::HASH,0, 0, "-"));
  l.push_back(Token(",", Token::DELIMITER,0, 0, "-"));
  l.push_back(Token("#101010", Token::HASH,0, 0, "-"));
  l.push_back(Token(")", Token::PAREN_CLOSED,0, 0, "-"));

  vp.processValue(l, c);

  ASSERT_EQ((uint)1, l.size());
  EXPECT_EQ("#ffffff", l.toString());

  l.clear();
  l.push_back(Token("contrast", Token::IDENTIFIER,0, 0, "-"));
  l.push_back(Token("(", Token::PAREN_OPEN,0, 0, "-"));
  l.push_back(Token("#222222", Token::HASH,0, 0, "-"));
  l.push_back(Token(",", Token::DELIMITER,0, 0, "-"));
  l.push_back(Token("#101010", Token::HASH,0, 0, "-"));
  l.push_back(Token(",", Token::DELIMITER,0, 0, "-"));
  l.push_back(Token("#dddddd", Token::HASH,0, 0, "-"));
  l.push_back(Token(")", Token::PAREN_CLOSED,0, 0, "-"));

  vp.processValue(l, c);

  ASSERT_EQ((uint)1, l.size());
  EXPECT_EQ("#dddddd", l.toString());

  l.clear();
  l.push_back(Token("contrast", Token::IDENTIFIER,0, 0, "-"));
  l.push_back(Token("(", Token::PAREN_OPEN,0, 0, "-"));
  hsl("90", "100%", "50%");
  l.push_back(Token(",", Token::DELIMITER,0, 0, "-"));
  l.push_back(Token("#000000", Token::HASH,0, 0, "-"));
  l.push_back(Token(",", Token::DELIMITER,0, 0, "-"));
  l.push_back(Token("#ffffff", Token::HASH,0, 0, "-"));
  l.push_back(Token(",", Token::DELIMITER,0, 0, "-"));
  l.push_back(Token("30%", Token::PERCENTAGE, 0, 0, "-"));
  l.push_back(Token(")", Token::PAREN_CLOSED,0, 0, "-"));

  vp.processValue(l, c);

  ASSERT_EQ((uint)1, l.size());
  EXPECT_EQ("#000000", l.toString());

  l.clear();
  l.push_back(Token("contrast", Token::IDENTIFIER,0, 0, "-"));
  l.push_back(Token("(", Token::PAREN_OPEN,0, 0, "-"));
  hsl("90", "100%", "50%");
  l.push_back(Token(",", Token::DELIMITER,0, 0, "-"));
  l.push_back(Token("#000000", Token::HASH,0, 0, "-"));
  l.push_back(Token(",", Token::DELIMITER,0, 0, "-"));
  l.push_back(Token("#ffffff", Token::HASH,0, 0, "-"));
  l.push_back(Token(",", Token::DELIMITER,0, 0, "-"));
  l.push_back(Token("80%", Token::PERCENTAGE, 0, 0, "-"));
  l.push_back(Token(")", Token::PAREN_CLOSED,0, 0, "-"));

  vp.processValue(l, c);

  ASSERT_EQ((uint)1, l.size());
  EXPECT_EQ("#ffffff", l.toString());
}

TEST_F(ColorTest, Multiply) {
  const char* out[] = {"#000000", "#331400", "#662900",
                      "#993d00", "#cc5200", "#ff6600",
                      "#ff0000", "#006600","#000000"};
  blendtests("multiply", out);
}

TEST_F(ColorTest, Screen) {
  const char* out[] = { "#ff6600", "#ff8533", "#ffa366",
                        "#ffc299", "#ffe0cc", "#ffffff",
                        "#ff6600", "#ffff00", "#ff66ff"};
  blendtests("screen", out);
}

TEST_F(ColorTest, Overlay) {
  const char* out[] = {"#ff0000", "#ff2900", "#ff5200",
                       "#ff7a00", "#ffa300", "#ffcc00",
                       "#ff0000", "#ffcc00", "#ff0000"};
  blendtests("overlay", out);
}

TEST_F(ColorTest, Softlight) {
  const char* out[] = {"#ff2900", "#ff4100", "#ff5a00", 
                       "#ff7200", "#ff8a00", "#ffa100", 
                       "#ff2900", "#ffa100", "#ff2900"};
  blendtests("softlight", out);
}

TEST_F(ColorTest, Hardlight) {
  const char* out[] = {"#000000", "#662900", "#cc5200", 
                       "#ff8533", "#ffc299", "#ffffff", 
                       "#ff0000", "#00ff00", "#0000ff"};
  blendtests("hardlight", out);
}

TEST_F(ColorTest, Difference) {
  const char* out[] = {"#ff6600", "#cc3333", "#990066", 
                       "#663399", "#3366cc", "#0099ff", 
                       "#006600", "#ff9900", "#ff66ff"};
  blendtests("difference", out);
}

TEST_F(ColorTest, Exclusion) {
  const char* out[] = {"#ff6600", "#cc7033", "#997a66", 
                       "#668599", "#338fcc", "#0099ff", 
                       "#006600", "#ff9900", "#ff66ff"};
  blendtests("exclusion", out);
}

TEST_F(ColorTest, Average) {
  const char* out[] = {"#803300", "#994d1a", "#b36633", 
                       "#cc804d", "#e69966", "#ffb380", 
                       "#ff3300", "#80b300", "#803380"};
  blendtests("average", out);
}

TEST_F(ColorTest, Negate) {
  const char* out[] = {"#ff6600", "#cc9933", "#99cc66", 
                       "#66ff99", "#33cccc", "#0099ff", 
                       "#006600", "#ff9900", "#ff66ff"};
  blendtests("negate", out);
}
