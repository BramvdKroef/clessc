#include "../CssTokenizer.h"
#include "gtest/gtest.h"

TEST(CssTokenizerTest, ReckognizesTokens) {
  istringstream in("identifier @atkeyword #hash 1234 100% 1.5em : ; { } ( ) [ ] /* comment */ function( ~= |= ~ ");
    
  CssTokenizer t(&in);
  EXPECT_EQ(Token::IDENTIFIER, t.readNextToken());
  EXPECT_EQ(Token::WHITESPACE, t.readNextToken());
  EXPECT_EQ(Token::ATKEYWORD, t.readNextToken());
  EXPECT_EQ(Token::WHITESPACE, t.readNextToken());
  EXPECT_EQ(Token::HASH, t.readNextToken());
  EXPECT_EQ(Token::WHITESPACE, t.readNextToken());
  EXPECT_EQ(Token::NUMBER, t.readNextToken());
  EXPECT_EQ(Token::WHITESPACE, t.readNextToken());
  EXPECT_EQ(Token::PERCENTAGE, t.readNextToken());
  EXPECT_EQ(Token::WHITESPACE, t.readNextToken());
  EXPECT_EQ(Token::DIMENSION, t.readNextToken());
  EXPECT_EQ(Token::WHITESPACE, t.readNextToken());
  EXPECT_EQ(Token::COLON, t.readNextToken());
  EXPECT_EQ(Token::WHITESPACE, t.readNextToken());
  EXPECT_EQ(Token::DELIMITER, t.readNextToken());
  EXPECT_EQ(Token::WHITESPACE, t.readNextToken());
  EXPECT_EQ(Token::BRACKET_OPEN, t.readNextToken());
  EXPECT_EQ(Token::WHITESPACE, t.readNextToken());
  EXPECT_EQ(Token::BRACKET_CLOSED, t.readNextToken());
  EXPECT_EQ(Token::WHITESPACE, t.readNextToken());
  EXPECT_EQ(Token::PAREN_OPEN, t.readNextToken());
  EXPECT_EQ(Token::WHITESPACE, t.readNextToken());
  EXPECT_EQ(Token::PAREN_CLOSED, t.readNextToken());
  EXPECT_EQ(Token::WHITESPACE, t.readNextToken());
  EXPECT_EQ(Token::BRACE_OPEN, t.readNextToken());
  EXPECT_EQ(Token::WHITESPACE, t.readNextToken());
  EXPECT_EQ(Token::BRACE_CLOSED, t.readNextToken());
  EXPECT_EQ(Token::WHITESPACE, t.readNextToken());
  EXPECT_EQ(Token::COMMENT, t.readNextToken());
  EXPECT_EQ(Token::WHITESPACE, t.readNextToken());
  EXPECT_EQ(Token::FUNCTION, t.readNextToken());
  EXPECT_EQ(Token::WHITESPACE, t.readNextToken());
  EXPECT_EQ(Token::INCLUDES, t.readNextToken());
  EXPECT_EQ(Token::WHITESPACE, t.readNextToken());
  EXPECT_EQ(Token::DASHMATCH, t.readNextToken());
  EXPECT_EQ(Token::WHITESPACE, t.readNextToken());
  EXPECT_EQ(Token::OTHER, t.readNextToken());
  EXPECT_EQ(Token::WHITESPACE, t.readNextToken());
  EXPECT_EQ(Token::EOS, t.readNextToken());
}

TEST(CssTokenizerTest, String) {
  istringstream in("\"string\"")
}
TEST(CssTokenizerTest, URL) {
  istringstream in("url(../img/img.png)");
}

TEST(CssTokenizerTest, UnicodeRange) {
  istringstream in("u+/123");

}
