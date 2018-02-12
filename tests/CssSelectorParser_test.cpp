#include <list>
#include <gtest/gtest.h>
#include <less/css/CssSelectorParser.h>
#include <less/stylesheet/Selector.h>

class CssSelectorParserTest : public ::testing::Test {
public:
  CssSelectorParser parser;
  Selector* selector;

  virtual void SetUp() {
    selector = new Selector();
  }
  
  virtual void TearDown() {
    delete selector;
  }
};

// .test
TEST_F(CssSelectorParserTest, Simple) {
  TokenList tokens;
  tokens.push_back(Token(".", Token::OTHER, 0, 0, "-"));
  tokens.push_back(Token("test", Token::IDENTIFIER, 0, 0, "-"));

  parser.parse(tokens, *selector);

  ASSERT_EQ(1, selector->getSelectors().size());
  ASSERT_STREQ(".test", selector->getSelectors().back().toString().c_str());
}

// .test1, .test2
TEST_F(CssSelectorParserTest, Multiple) {
  TokenList tokens;
  tokens.push_back(Token(".", Token::OTHER, 0, 0, "-"));
  tokens.push_back(Token("test1", Token::IDENTIFIER, 0, 0, "-"));
  tokens.push_back(Token(",", Token::OTHER, 0, 0, "-"));
  tokens.push_back(Token(" ", Token::WHITESPACE, 0, 0, "-"));
  tokens.push_back(Token(".", Token::OTHER, 0, 0, "-"));
  tokens.push_back(Token("test2", Token::IDENTIFIER, 0, 0, "-"));

  parser.parse(tokens, *selector);

  ASSERT_EQ(2, selector->getSelectors().size());
  ASSERT_STREQ(".test1", selector->getSelectors().front().toString().c_str());
  ASSERT_STREQ(".test2", selector->getSelectors().back().toString().c_str());
}

// .test1(,) .test2
TEST_F(CssSelectorParserTest, Parentheses) {
  TokenList tokens;

  tokens.push_back(Token(".", Token::OTHER, 0, 0, "-"));
  tokens.push_back(Token("test1", Token::IDENTIFIER, 0, 0, "-"));
  tokens.push_back(Token("(", Token::PAREN_OPEN, 0, 0, "-"));
  tokens.push_back(Token(",", Token::OTHER, 0, 0, "-"));
  tokens.push_back(Token(")", Token::PAREN_CLOSED, 0, 0, "-"));
  tokens.push_back(Token(" ", Token::WHITESPACE, 0, 0, "-"));
  tokens.push_back(Token(".", Token::OTHER, 0, 0, "-"));
  tokens.push_back(Token("test2", Token::IDENTIFIER, 0, 0, "-"));

  parser.parse(tokens, *selector);

  ASSERT_EQ(1, selector->getSelectors().size());
  ASSERT_STREQ(".test1(,) .test2", selector->getSelectors().front().toString().c_str());
}
