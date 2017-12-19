#include <gtest/gtest.h>
#include <less/css/CssTokenizer.h>

/**
 * Test if the tokenizer reckognizes the tokens in the input string.
 */
TEST(CssTokenizerTest, ReckognizesTokens) {
  istringstream in("identifier @atkeyword #hash 1234 100% 1.5em : ; { } ( ) [ ] /* comment */ function( ~= |= ~ ");
    
  CssTokenizer t(in, "test");
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
  //EXPECT_EQ(Token::FUNCTION, t.readNextToken());
  EXPECT_EQ(Token::IDENTIFIER, t.readNextToken());
  EXPECT_EQ(Token::PAREN_OPEN, t.readNextToken());
  EXPECT_EQ(Token::WHITESPACE, t.readNextToken());
  EXPECT_EQ(Token::INCLUDES, t.readNextToken());
  EXPECT_EQ(Token::WHITESPACE, t.readNextToken());
  EXPECT_EQ(Token::DASHMATCH, t.readNextToken());
  EXPECT_EQ(Token::WHITESPACE, t.readNextToken());
  EXPECT_EQ(Token::OTHER, t.readNextToken());
  EXPECT_EQ(Token::WHITESPACE, t.readNextToken());
  EXPECT_EQ(Token::EOS, t.readNextToken());
}

/**
 * Test strings quoted with double quotes, single quotes, strings
 * containing newlines, and strings containing unicode characters.
 */
TEST(CssTokenizerTest, String) {
  istringstream in("\"string\" 'string' 'string\\\nstring' '\\12EF'"),
    inBad1("'string\n'"),
    inBad2("'string"),
    newlines("'\\\n \\\r\n \\\r'");

  CssTokenizer t(in, "test"), tBad1(inBad1, "test"), tBad2(inBad2, "test"),
    tNewlines(newlines, "test");

  EXPECT_EQ(Token::STRING, t.readNextToken());
  EXPECT_EQ(Token::WHITESPACE, t.readNextToken());
  EXPECT_EQ(Token::STRING, t.readNextToken());
  EXPECT_EQ(Token::WHITESPACE, t.readNextToken());
  EXPECT_EQ(Token::STRING, t.readNextToken());
  EXPECT_EQ(Token::WHITESPACE, t.readNextToken());
  EXPECT_EQ(Token::STRING, t.readNextToken());

  EXPECT_THROW(tBad1.readNextToken(), ParseException*);
  EXPECT_THROW(tBad2.readNextToken(), ParseException*);

  ASSERT_EQ(Token::STRING, tNewlines.readNextToken());
  EXPECT_STREQ("'\\\n \\\r\n \\\r'", tNewlines.getToken().c_str());
}

TEST(CssTokenizerTest, URL) {
  istringstream in("url(../img/img.png) url('http://example.com/image.jpg')");

  CssTokenizer t(in, "test");
  EXPECT_EQ(Token::URL, t.readNextToken());
  EXPECT_EQ(Token::WHITESPACE, t.readNextToken());
  EXPECT_EQ(Token::URL, t.readNextToken());
}

TEST(CssTokenizerTest, UnicodeRange) {
  istringstream in("u+123 u+123DEF-123");
  CssTokenizer t(in, "test");
  EXPECT_EQ(Token::UNICODE_RANGE, t.readNextToken());
  EXPECT_EQ(Token::WHITESPACE, t.readNextToken());
  EXPECT_EQ(Token::UNICODE_RANGE, t.readNextToken());
}

TEST(CssTokenizerTest, NonAscii) {
  istringstream in("a\xEE€");
  CssTokenizer t(in, "test");
  EXPECT_EQ(Token::IDENTIFIER, t.readNextToken());
  EXPECT_STREQ("a\xEE€", t.getToken().c_str());
}

TEST(CssTokenizerTest, Minus) {
  istringstream in("-10 -50% -5em -identifier -moz-border-radius( -");
  CssTokenizer t(in, "test");
  EXPECT_EQ(Token::NUMBER, t.readNextToken());
  EXPECT_EQ(Token::WHITESPACE, t.readNextToken());
  EXPECT_EQ(Token::PERCENTAGE, t.readNextToken());
  EXPECT_EQ(Token::WHITESPACE, t.readNextToken());
  EXPECT_EQ(Token::DIMENSION, t.readNextToken());
  EXPECT_EQ(Token::WHITESPACE, t.readNextToken());
  EXPECT_EQ(Token::IDENTIFIER, t.readNextToken());
  EXPECT_EQ(Token::WHITESPACE, t.readNextToken());
  EXPECT_EQ(Token::IDENTIFIER, t.readNextToken());
  EXPECT_EQ(Token::PAREN_OPEN, t.readNextToken());
  EXPECT_EQ(Token::WHITESPACE, t.readNextToken());
  EXPECT_EQ(Token::OTHER, t.readNextToken());
}
TEST(CssTokenizerTest, Other) {
  istringstream in("@ - ~ | / + =");
  CssTokenizer t(in, "test");
  EXPECT_EQ(Token::OTHER, t.readNextToken());
  EXPECT_STREQ("@", t.getToken().c_str());
  EXPECT_EQ(Token::WHITESPACE, t.readNextToken());
  EXPECT_EQ(Token::OTHER, t.readNextToken());
  EXPECT_STREQ("-", t.getToken().c_str());
  EXPECT_EQ(Token::WHITESPACE, t.readNextToken());
  EXPECT_EQ(Token::OTHER, t.readNextToken());
  EXPECT_STREQ("~", t.getToken().c_str());
  EXPECT_EQ(Token::WHITESPACE, t.readNextToken());
  EXPECT_EQ(Token::OTHER, t.readNextToken());
  EXPECT_STREQ("|", t.getToken().c_str());
  EXPECT_EQ(Token::WHITESPACE, t.readNextToken());
  EXPECT_EQ(Token::OTHER, t.readNextToken());
  EXPECT_STREQ("/", t.getToken().c_str());
  EXPECT_EQ(Token::WHITESPACE, t.readNextToken());
  EXPECT_EQ(Token::OTHER, t.readNextToken());
  EXPECT_STREQ("+", t.getToken().c_str());
  EXPECT_EQ(Token::WHITESPACE, t.readNextToken());
  EXPECT_EQ(Token::OTHER, t.readNextToken());
  EXPECT_STREQ("=", t.getToken().c_str());
}

TEST(CssTokenizerTest, StringEscape) {
  istringstream in("'string\\''");

  CssTokenizer t(in, "test");

  EXPECT_EQ(Token::STRING, t.readNextToken());
  EXPECT_STREQ("'string\\''", t.getToken().c_str());
}
