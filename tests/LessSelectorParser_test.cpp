#include <list>
#include <gtest/gtest.h>
#include <less/less/LessSelectorParser.h>

class LessSelectorParserTest : public ::testing::Test {
public:
  LessSelectorParser parser;
  LessSelector* selector;

  virtual void SetUp() {
    selector = new LessSelector();
  }
  
  virtual void TearDown() {
    delete selector;
  }
};

// .test:extend(.mixin)
TEST_F(LessSelectorParserTest, Extension) {
  TokenList tokens;
  tokens.push_back(Token(".", Token::OTHER, 0, 0, "-"));
  tokens.push_back(Token("test", Token::IDENTIFIER, 0, 0, "-"));
  tokens.push_back(Token(":", Token::COLON, 0, 0, "-"));
  tokens.push_back(Token("extend", Token::IDENTIFIER, 0, 0, "-"));
  tokens.push_back(Token("(", Token::PAREN_OPEN, 0, 0, "-"));
  tokens.push_back(Token(".", Token::OTHER, 0, 0, "-"));
  tokens.push_back(Token("mixin", Token::IDENTIFIER, 0, 0, "-"));
  tokens.push_back(Token(")", Token::PAREN_CLOSED, 0, 0, "-"));

  parser.parse(tokens, *selector);

  ASSERT_EQ(1, selector->size());
  ASSERT_EQ(".test", selector->front().toString());

  ASSERT_EQ(1, selector->getExtensions().size());
  ASSERT_EQ(".mixin", selector->getExtensions().back().getTarget().toString());
  ASSERT_EQ(".test", selector->getExtensions().back().getExtension().toString());
}

// .test:extend(.mixin1, .mixin2)
TEST_F(LessSelectorParserTest, ExtensionMultiple1) {
  TokenList tokens;
  tokens.push_back(Token(".", Token::OTHER, 0, 0, "-"));
  tokens.push_back(Token("test", Token::IDENTIFIER, 0, 0, "-"));
  tokens.push_back(Token(":", Token::COLON, 0, 0, "-"));
  tokens.push_back(Token("extend", Token::IDENTIFIER, 0, 0, "-"));
  tokens.push_back(Token("(", Token::PAREN_OPEN, 0, 0, "-"));
  tokens.push_back(Token(".", Token::OTHER, 0, 0, "-"));
  tokens.push_back(Token("mixin1", Token::IDENTIFIER, 0, 0, "-"));
  tokens.push_back(Token(",", Token::OTHER, 0, 0, "-"));
  tokens.push_back(Token(" ", Token::WHITESPACE, 0, 0, "-"));
  tokens.push_back(Token(".", Token::OTHER, 0, 0, "-"));
  tokens.push_back(Token("mixin2", Token::IDENTIFIER, 0, 0, "-"));
  tokens.push_back(Token(")", Token::PAREN_CLOSED, 0, 0, "-"));

  parser.parse(tokens, *selector);

  ASSERT_EQ(1, selector->size());
  ASSERT_EQ(".test", selector->front().toString());

  ASSERT_EQ(1, selector->getExtensions().size());
  ASSERT_EQ(".test", selector->getExtensions().back().getExtension().toString());

  ASSERT_EQ(2, selector->getExtensions().back().getTarget().size());
  ASSERT_EQ(".mixin1, .mixin2", selector->getExtensions().back().getTarget().toString());
}

// .test:extend(.mixin1):extend(.mixin2)
TEST_F(LessSelectorParserTest, ExtensionMultiple2) {
  TokenList tokens;
  tokens.push_back(Token(".", Token::OTHER, 0, 0, "-"));
  tokens.push_back(Token("test", Token::IDENTIFIER, 0, 0, "-"));
  tokens.push_back(Token(":", Token::COLON, 0, 0, "-"));
  tokens.push_back(Token("extend", Token::IDENTIFIER, 0, 0, "-"));
  tokens.push_back(Token("(", Token::PAREN_OPEN, 0, 0, "-"));
  tokens.push_back(Token(".", Token::OTHER, 0, 0, "-"));
  tokens.push_back(Token("mixin1", Token::IDENTIFIER, 0, 0, "-"));
  tokens.push_back(Token(")", Token::PAREN_CLOSED, 0, 0, "-"));
  tokens.push_back(Token(":", Token::COLON, 0, 0, "-"));
  tokens.push_back(Token("extend", Token::IDENTIFIER, 0, 0, "-"));
  tokens.push_back(Token("(", Token::PAREN_OPEN, 0, 0, "-"));
  tokens.push_back(Token(".", Token::OTHER, 0, 0, "-"));
  tokens.push_back(Token("mixin2", Token::IDENTIFIER, 0, 0, "-"));
  tokens.push_back(Token(")", Token::PAREN_CLOSED, 0, 0, "-"));

  parser.parse(tokens, *selector);

  ASSERT_EQ(1, selector->size());
  ASSERT_EQ(".test", selector->front().toString());

  ASSERT_EQ(2, selector->getExtensions().size());
  ASSERT_EQ(".mixin1", selector->getExtensions().front().getTarget().toString());
  ASSERT_EQ(".test", selector->getExtensions().front().getExtension().toString());
  ASSERT_EQ(".mixin2", selector->getExtensions().back().getTarget().toString());
  ASSERT_EQ(".test", selector->getExtensions().back().getExtension().toString());
}

// .test1:extend(.mixin1), .test2:extend(.mixin2)
TEST_F(LessSelectorParserTest, ExtensionMultiple3) {
  TokenList tokens;
  tokens.push_back(Token(".", Token::OTHER, 0, 0, "-"));
  tokens.push_back(Token("test1", Token::IDENTIFIER, 0, 0, "-"));
  tokens.push_back(Token(":", Token::COLON, 0, 0, "-"));
  tokens.push_back(Token("extend", Token::IDENTIFIER, 0, 0, "-"));
  tokens.push_back(Token("(", Token::PAREN_OPEN, 0, 0, "-"));
  tokens.push_back(Token(".", Token::OTHER, 0, 0, "-"));
  tokens.push_back(Token("mixin1", Token::IDENTIFIER, 0, 0, "-"));
  tokens.push_back(Token(")", Token::PAREN_CLOSED, 0, 0, "-"));
  tokens.push_back(Token(",", Token::OTHER, 0, 0, "-"));
  tokens.push_back(Token(" ", Token::WHITESPACE, 0, 0, "-"));
  tokens.push_back(Token(".", Token::OTHER, 0, 0, "-"));
  tokens.push_back(Token("test2", Token::IDENTIFIER, 0, 0, "-"));
  tokens.push_back(Token(":", Token::COLON, 0, 0, "-"));
  tokens.push_back(Token("extend", Token::IDENTIFIER, 0, 0, "-"));
  tokens.push_back(Token("(", Token::PAREN_OPEN, 0, 0, "-"));
  tokens.push_back(Token(".", Token::OTHER, 0, 0, "-"));
  tokens.push_back(Token("mixin2", Token::IDENTIFIER, 0, 0, "-"));
  tokens.push_back(Token(")", Token::PAREN_CLOSED, 0, 0, "-"));

  parser.parse(tokens, *selector);

  ASSERT_EQ(2, selector->size());
  ASSERT_EQ(".test1", selector->front().toString());
  ASSERT_EQ(".test2", selector->back().toString());

  ASSERT_EQ(2, selector->getExtensions().size());
  ASSERT_EQ(".mixin1", selector->getExtensions().front().getTarget().toString());
  ASSERT_EQ(".test1", selector->getExtensions().front().getExtension().toString());
  ASSERT_EQ(".mixin2", selector->getExtensions().back().getTarget().toString());
  ASSERT_EQ(".test2", selector->getExtensions().back().getExtension().toString());
}


// .test()
TEST_F(LessSelectorParserTest, Parameters) {
  TokenList tokens;
  tokens.push_back(Token(".", Token::OTHER, 0, 0, "-"));
  tokens.push_back(Token("test", Token::IDENTIFIER, 0, 0, "-"));
  tokens.push_back(Token("(", Token::PAREN_OPEN, 0, 0, "-"));
  tokens.push_back(Token(")", Token::PAREN_CLOSED, 0, 0, "-"));

  parser.parse(tokens, *selector);

  ASSERT_EQ(1, selector->size());
  EXPECT_EQ(".test", selector->front().toString());

  ASSERT_EQ(true, selector->needsArguments());
  ASSERT_EQ(0, selector->getParameters().size());
}

// .test(switch, @key)
TEST_F(LessSelectorParserTest, Parameters2) {
  TokenList tokens;
  tokens.push_back(Token(".", Token::OTHER, 0, 0, "-"));
  tokens.push_back(Token("test", Token::IDENTIFIER, 0, 0, "-"));
  tokens.push_back(Token("(", Token::PAREN_OPEN, 0, 0, "-"));
  tokens.push_back(Token("switch", Token::IDENTIFIER, 0, 0, "-"));
  tokens.push_back(Token(",", Token::OTHER, 0, 0, "-"));
  tokens.push_back(Token(" ", Token::WHITESPACE, 0, 0, "-"));
  tokens.push_back(Token("@key", Token::ATKEYWORD, 0, 0, "-"));
  tokens.push_back(Token(")", Token::PAREN_CLOSED, 0, 0, "-"));

  parser.parse(tokens, *selector);

  ASSERT_EQ(1, selector->size());
  EXPECT_EQ(".test", selector->front().toString());

  EXPECT_EQ(true, selector->needsArguments());
  ASSERT_EQ(2, selector->getParameters().size());
  ASSERT_EQ("switch", selector->getParameters().front());
  ASSERT_EQ("@key", selector->getParameters().back());
}

// .test(@key: (1px))
TEST_F(LessSelectorParserTest, Defaults) {
  TokenList tokens;
  tokens.push_back(Token(".", Token::OTHER, 0, 0, "-"));
  tokens.push_back(Token("test", Token::IDENTIFIER, 0, 0, "-"));
  tokens.push_back(Token("(", Token::PAREN_OPEN, 0, 0, "-"));
  tokens.push_back(Token("@key", Token::ATKEYWORD, 0, 0, "-"));
  tokens.push_back(Token(":", Token::COLON, 0, 0, "-"));
  tokens.push_back(Token("(", Token::PAREN_OPEN, 0, 0, "-"));
  tokens.push_back(Token("1px", Token::DIMENSION, 0, 0, "-"));
  tokens.push_back(Token(")", Token::PAREN_CLOSED, 0, 0, "-"));
  tokens.push_back(Token(")", Token::PAREN_CLOSED, 0, 0, "-"));

  parser.parse(tokens, *selector);

  ASSERT_EQ(1, selector->size());
  EXPECT_EQ(".test", selector->front().toString());

  EXPECT_EQ(true, selector->needsArguments());
  ASSERT_EQ(1, selector->getParameters().size());
  ASSERT_EQ("@key", selector->getParameters().back());
  ASSERT_EQ("(1px)", selector->getDefault("@key")->toString());
}

TEST_F(LessSelectorParserTest, ParametersUnlimited) {
  TokenList tokens;
  tokens.push_back(Token(".", Token::OTHER, 0, 0, "-"));
  tokens.push_back(Token("test", Token::IDENTIFIER, 0, 0, "-"));
  tokens.push_back(Token("(", Token::PAREN_OPEN, 0, 0, "-"));
  tokens.push_back(Token("@key", Token::ATKEYWORD, 0, 0, "-"));
  tokens.push_back(Token(",", Token::OTHER, 0, 0, "-"));
  tokens.push_back(Token(" ", Token::WHITESPACE, 0, 0, "-"));
  tokens.push_back(Token(".", Token::OTHER, 0, 0, "-"));
  tokens.push_back(Token(".", Token::OTHER, 0, 0, "-"));
  tokens.push_back(Token(".", Token::OTHER, 0, 0, "-"));
  tokens.push_back(Token(")", Token::PAREN_CLOSED, 0, 0, "-"));

  parser.parse(tokens, *selector);

  ASSERT_EQ(1, selector->size());
  EXPECT_EQ(".test", selector->front().toString());

  EXPECT_EQ(true, selector->needsArguments());
  ASSERT_EQ(1, selector->getParameters().size());
  EXPECT_EQ("@key", selector->getParameters().back());
  ASSERT_EQ(true, selector->unlimitedArguments());
}

TEST_F(LessSelectorParserTest, ParametersRest) {
  TokenList tokens;
  tokens.push_back(Token(".", Token::OTHER, 0, 0, "-"));
  tokens.push_back(Token("test", Token::IDENTIFIER, 0, 0, "-"));
  tokens.push_back(Token("(", Token::PAREN_OPEN, 0, 0, "-"));
  tokens.push_back(Token("@key", Token::ATKEYWORD, 0, 0, "-"));
  tokens.push_back(Token(",", Token::OTHER, 0, 0, "-"));
  tokens.push_back(Token(" ", Token::WHITESPACE, 0, 0, "-"));
  tokens.push_back(Token("@rest", Token::ATKEYWORD, 0, 0, "-"));
  tokens.push_back(Token(".", Token::OTHER, 0, 0, "-"));
  tokens.push_back(Token(".", Token::OTHER, 0, 0, "-"));
  tokens.push_back(Token(".", Token::OTHER, 0, 0, "-"));
  tokens.push_back(Token(")", Token::PAREN_CLOSED, 0, 0, "-"));

  parser.parse(tokens, *selector);

  ASSERT_EQ(1, selector->size());
  EXPECT_EQ(".test", selector->front().toString());

  EXPECT_EQ(true, selector->needsArguments());
  ASSERT_EQ(1, selector->getParameters().size());
  EXPECT_EQ("@key", selector->getParameters().back());
  ASSERT_EQ(true, selector->unlimitedArguments());
  ASSERT_EQ("@rest", selector->getRestIdentifier());
}

TEST_F(LessSelectorParserTest, Condition) {
  TokenList tokens;
  tokens.push_back(Token(".", Token::OTHER, 0, 0, "-"));
  tokens.push_back(Token("test", Token::IDENTIFIER, 0, 0, "-"));
  tokens.push_back(Token(" ", Token::WHITESPACE, 0, 0, "-"));
  tokens.push_back(Token("when", Token::IDENTIFIER, 0, 0, "-"));
  tokens.push_back(Token(" ", Token::WHITESPACE, 0, 0, "-"));
  tokens.push_back(Token("@key", Token::ATKEYWORD, 0, 0, "-"));
  tokens.push_back(Token(" ", Token::WHITESPACE, 0, 0, "-"));
  tokens.push_back(Token("=", Token::DELIMITER, 0, 0, "-"));
  tokens.push_back(Token(" ", Token::WHITESPACE, 0, 0, "-"));
  tokens.push_back(Token("1px", Token::DIMENSION, 0, 0, "-"));
  
  parser.parse(tokens, *selector);

  ASSERT_EQ(1, selector->size());
  EXPECT_EQ(".test", selector->front().toString());

  EXPECT_EQ(false, selector->needsArguments());
  ASSERT_EQ(1, selector->getConditions().size());
  EXPECT_EQ("@key = 1px", selector->getConditions().front().toString());
}

TEST_F(LessSelectorParserTest, ParameterCondition) {
  TokenList tokens;
  tokens.push_back(Token(".", Token::OTHER, 0, 0, "-"));
  tokens.push_back(Token("test", Token::IDENTIFIER, 0, 0, "-"));
  tokens.push_back(Token("(", Token::PAREN_OPEN, 0, 0, "-"));
  tokens.push_back(Token(")", Token::PAREN_CLOSED, 0, 0, "-"));
  tokens.push_back(Token(" ", Token::WHITESPACE, 0, 0, "-"));
  tokens.push_back(Token("when", Token::IDENTIFIER, 0, 0, "-"));
  tokens.push_back(Token(" ", Token::WHITESPACE, 0, 0, "-"));
  tokens.push_back(Token("@key", Token::ATKEYWORD, 0, 0, "-"));
  tokens.push_back(Token(" ", Token::WHITESPACE, 0, 0, "-"));
  tokens.push_back(Token("=", Token::DELIMITER, 0, 0, "-"));
  tokens.push_back(Token(" ", Token::WHITESPACE, 0, 0, "-"));
  tokens.push_back(Token("1px", Token::DIMENSION, 0, 0, "-"));
  
  parser.parse(tokens, *selector);

  ASSERT_EQ(1, selector->size());
  EXPECT_EQ(".test", selector->front().toString());

  EXPECT_EQ(true, selector->needsArguments());
  ASSERT_EQ(1, selector->getConditions().size());
  EXPECT_EQ("@key = 1px", selector->getConditions().front().toString());
}

TEST_F(LessSelectorParserTest, ConditionMultiple) {
  TokenList tokens;
  tokens.push_back(Token(".", Token::OTHER, 0, 0, "-"));
  tokens.push_back(Token("test", Token::IDENTIFIER, 0, 0, "-"));
  tokens.push_back(Token(" ", Token::WHITESPACE, 0, 0, "-"));
  tokens.push_back(Token("when", Token::IDENTIFIER, 0, 0, "-"));
  tokens.push_back(Token(" ", Token::WHITESPACE, 0, 0, "-"));
  tokens.push_back(Token("@key", Token::ATKEYWORD, 0, 0, "-"));
  tokens.push_back(Token(" ", Token::WHITESPACE, 0, 0, "-"));
  tokens.push_back(Token("=", Token::DELIMITER, 0, 0, "-"));
  tokens.push_back(Token(" ", Token::WHITESPACE, 0, 0, "-"));
  tokens.push_back(Token("1px", Token::DIMENSION, 0, 0, "-"));
  tokens.push_back(Token(",", Token::OTHER, 0, 0, "-"));
  tokens.push_back(Token(" ", Token::WHITESPACE, 0, 0, "-"));
  tokens.push_back(Token("@key2", Token::ATKEYWORD, 0, 0, "-"));
  tokens.push_back(Token(" ", Token::WHITESPACE, 0, 0, "-"));
  tokens.push_back(Token("=", Token::DELIMITER, 0, 0, "-"));
  tokens.push_back(Token(" ", Token::WHITESPACE, 0, 0, "-"));
  tokens.push_back(Token("true", Token::DIMENSION, 0, 0, "-"));
  
  parser.parse(tokens, *selector);

  ASSERT_EQ(1, selector->size());
  EXPECT_EQ(".test", selector->front().toString());

  EXPECT_EQ(false, selector->needsArguments());
  ASSERT_EQ(2, selector->getConditions().size());
  EXPECT_EQ("@key = 1px", selector->getConditions().front().toString());
  EXPECT_EQ("@key2 = true", selector->getConditions().back().toString());
}
