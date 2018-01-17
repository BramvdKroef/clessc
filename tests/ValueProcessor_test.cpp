#include <gtest/gtest.h>
#include <less/value/ValueProcessor.h>
#include <less/lessstylesheet/ProcessingContext.h>

TEST(ValueProcessorTest, Operators) {
  TokenList l;
  ValueProcessor vp;
  ProcessingContext c;
  
  // add
  l.push_back(Token("1", Token::NUMBER, 0, 0, 0));
  l.push_back(Token("+", Token::DELIMITER, 0, 0, 0));
  l.push_back(Token("1", Token::NUMBER, 0, 0, 0));
  vp.processValue(l, c);
  
  ASSERT_EQ((uint)1, l.size());
  ASSERT_EQ(Token::NUMBER, l.front().type);
  EXPECT_STREQ("2", l.front().c_str());

  // substract
  l.push_back(Token("-", Token::DELIMITER, 0, 0, 0));
  l.push_back(Token(" ", Token::WHITESPACE, 0, 0, 0));
  l.push_back(Token("1", Token::NUMBER, 0, 0, 0));

  vp.processValue(l, c);
  
  ASSERT_EQ((uint)1, l.size());
  ASSERT_EQ(Token::NUMBER, l.front().type);
  EXPECT_STREQ("1", l.front().c_str());

  // multiply
  l.push_back(Token("*", Token::DELIMITER, 0, 0, 0));
  l.push_back(Token("5", Token::NUMBER, 0, 0, 0));

  vp.processValue(l, c);
  
  ASSERT_EQ((uint)1, l.size());
  ASSERT_EQ(Token::NUMBER, l.front().type);
  EXPECT_STREQ("5", l.front().c_str());

  // divide
  l.push_back(Token("/", Token::DELIMITER, 0, 0, 0));
  l.push_back(Token("2", Token::NUMBER, 0, 0, 0));

  vp.processValue(l, c);
  
  ASSERT_EQ((uint)1, l.size());
  ASSERT_EQ(Token::NUMBER, l.front().type);
  EXPECT_STREQ("2.5", l.front().c_str());
}

TEST(ValueProcessorTest, Conditions) {
  TokenList l;
  ValueProcessor vp;
  ProcessingContext c;

  // compare
  l.push_back(Token("1", Token::NUMBER, 0,0,0));
  l.push_back(Token("<", Token::DELIMITER, 0, 0, 0));
  l.push_back(Token("10", Token::NUMBER, 0, 0, 0));
  
  ASSERT_EQ(vp.validateCondition(l, c), true);

  l.clear();
  l.push_back(Token("1", Token::NUMBER, 0,0,0));
  l.push_back(Token(">", Token::DELIMITER, 0, 0, 0));
  l.push_back(Token("10", Token::NUMBER, 0, 0, 0));

  ASSERT_EQ(vp.validateCondition(l, c), false);

  l.clear();
  l.push_back(Token("1", Token::NUMBER, 0,0,0));
  l.push_back(Token("=", Token::DELIMITER, 0, 0, 0));
  l.push_back(Token("10", Token::NUMBER, 0, 0, 0));

  ASSERT_EQ(vp.validateCondition(l, c), false);

  l.clear();
  l.push_back(Token("1", Token::NUMBER, 0,0,0));
  l.push_back(Token("=", Token::DELIMITER, 0, 0, 0));
  l.push_back(Token("<", Token::DELIMITER, 0, 0, 0));
  l.push_back(Token("10", Token::NUMBER, 0, 0, 0));

  ASSERT_EQ(vp.validateCondition(l, c), true);

  l.clear();
  l.push_back(Token("1", Token::NUMBER, 0,0,0));
  l.push_back(Token(">", Token::DELIMITER, 0, 0, 0));
  l.push_back(Token("=", Token::DELIMITER, 0, 0, 0));
  l.push_back(Token("10", Token::NUMBER, 0, 0, 0));

  ASSERT_EQ(vp.validateCondition(l, c), false);

}

TEST(ValueProcessorTest, OperandMismatch) {
  TokenList l;
  ValueProcessor vp;
  ProcessingContext c;

  l.push_back(Token("true", Token::IDENTIFIER, 0, 0, 0));
  l.push_back(Token("+", Token::DELIMITER,0, 0, 0));
  l.push_back(Token("#FFF", Token::HASH, 0, 0, 0));

  vp.processValue(l, c);

  ASSERT_EQ((uint)5, l.size());
  ASSERT_EQ(Token::IDENTIFIER, l.front().type);
  EXPECT_STREQ("true", l.front().c_str());
  l.pop_front();
  ASSERT_EQ(Token::WHITESPACE, l.front().type);
  l.pop_front();
  ASSERT_EQ(Token::DELIMITER, l.front().type);
  EXPECT_STREQ("+", l.front().c_str());
  ASSERT_EQ(Token::HASH, l.back().type);
  EXPECT_STREQ("#FFF", l.back().c_str());
  
}

TEST(ValueProcessorTest, OperatorsInParens) {
  TokenList l;
  ValueProcessor vp;
  ProcessingContext c;

  l.push_back(Token("25", Token::NUMBER, 0,0,0));
  l.push_back(Token("/", Token::DELIMITER, 0,0,0));
  l.push_back(Token("(", Token::PAREN_OPEN, 0,0,0));
  l.push_back(Token("2", Token::NUMBER, 0,0,0));
  l.push_back(Token("+", Token::DELIMITER, 0,0,0));
  l.push_back(Token("3", Token::NUMBER, 0,0,0));
  l.push_back(Token(")", Token::PAREN_CLOSED, 0,0,0));

  vp.processValue(l, c);
  
  ASSERT_EQ((uint)1, l.size());
  EXPECT_STREQ("5", l.front().c_str());
  ASSERT_EQ(Token::NUMBER, l.front().type);
}


TEST(ValueProcessorTest, StringOperations) {
  TokenList l;
  ValueProcessor vp;
}
TEST(ValueProcessorTest, ColorOperations) {
  TokenList l;
  ValueProcessor vp;
  ProcessingContext c;

  l.push_back(Token("rgb", Token::IDENTIFIER, 0,0,0));
  l.push_back(Token("(", Token::PAREN_OPEN, 0,0,0));
  l.push_back(Token("10", Token::NUMBER, 0,0,0));
  l.push_back(Token(",", Token::DELIMITER, 0,0,0));
  l.push_back(Token("10", Token::NUMBER, 0,0,0));
  l.push_back(Token(",", Token::DELIMITER, 0,0,0));
  l.push_back(Token("10", Token::NUMBER, 0,0,0));
  l.push_back(Token(")", Token::PAREN_CLOSED, 0,0,0));

  vp.processValue(l, c);
  
  ASSERT_EQ((uint)1, l.size());
  EXPECT_STREQ("#0a0a0a", l.front().c_str());
  ASSERT_EQ(Token::HASH, l.front().type);

  l.clear();
  l.push_back(Token("rgba", Token::IDENTIFIER,0,0,0));
  l.push_back(Token("(", Token::PAREN_OPEN,0,0,0));
  l.push_back(Token("10", Token::NUMBER,0,0,0));
  l.push_back(Token(",", Token::DELIMITER,0,0,0));
  l.push_back(Token("10", Token::NUMBER,0,0,0));
  l.push_back(Token(",", Token::DELIMITER,0,0,0));
  l.push_back(Token("10", Token::NUMBER,0,0,0));
  l.push_back(Token(",", Token::DELIMITER,0,0,0));
  l.push_back(Token("10%", Token::PERCENTAGE,0,0,0));
  l.push_back(Token(")", Token::PAREN_CLOSED,0,0,0));

  vp.processValue(l, c);
  
  ASSERT_EQ((uint)10, l.size());
  EXPECT_STREQ("rgba(10,10,10,0.1)", l.toString().c_str());

  l.clear();
  l.push_back(Token("lighten", Token::IDENTIFIER,0,0,0));
  l.push_back(Token("(", Token::PAREN_OPEN,0,0,0));
  l.push_back(Token("#AAA", Token::HASH,0,0,0));
  l.push_back(Token(",", Token::DELIMITER,0,0,0));
  l.push_back(Token("10%", Token::PERCENTAGE,0,0,0));
  l.push_back(Token(")", Token::PAREN_CLOSED,0,0,0));

  vp.processValue(l, c);
  
  ASSERT_EQ((uint)1, l.size());
  ASSERT_EQ(Token::HASH, l.front().type);
  EXPECT_STREQ("#c3c3c3", l.front().c_str());

  l.clear();
  l.push_back(Token("darken", Token::IDENTIFIER,0,0,0));
  l.push_back(Token("(", Token::PAREN_OPEN,0,0,0));
  l.push_back(Token("#AAA", Token::HASH,0,0,0));
  l.push_back(Token(",", Token::DELIMITER,0,0,0));
  l.push_back(Token("10%", Token::PERCENTAGE,0,0,0));
  l.push_back(Token(")", Token::PAREN_CLOSED,0,0,0));

  vp.processValue(l, c);
  
  ASSERT_EQ((uint)1, l.size());
  ASSERT_EQ(Token::HASH, l.front().type);
  EXPECT_STREQ("#909090", l.front().c_str());

  l.clear();
  l.push_back(Token("saturate", Token::IDENTIFIER,0,0,0));
  l.push_back(Token("(", Token::PAREN_OPEN,0,0,0));
  l.push_back(Token("#AAA", Token::HASH,0,0,0));
  l.push_back(Token(",", Token::DELIMITER,0,0,0));
  l.push_back(Token("10%", Token::PERCENTAGE,0,0,0));
  l.push_back(Token(")", Token::PAREN_CLOSED,0,0,0));

  vp.processValue(l, c);
  
  ASSERT_EQ((uint)1, l.size());
  ASSERT_EQ(Token::HASH, l.front().type);
  EXPECT_STREQ("#b3a2a2", l.front().c_str());

  l.clear();
  l.push_back(Token("desaturate", Token::IDENTIFIER,0,0,0));
  l.push_back(Token("(", Token::PAREN_OPEN,0,0,0));
  l.push_back(Token("#123456", Token::HASH,0,0,0));
  l.push_back(Token(",", Token::DELIMITER,0,0,0));
  l.push_back(Token("10%", Token::PERCENTAGE,0,0,0));
  l.push_back(Token(")", Token::PAREN_CLOSED,0,0,0));

  vp.processValue(l, c);
  
  ASSERT_EQ((uint)1, l.size());
  ASSERT_EQ(Token::HASH, l.front().type);
  EXPECT_STREQ("#173451", l.front().c_str());

  l.clear();
  l.push_back(Token("fadein", Token::IDENTIFIER,0,0,0));
  l.push_back(Token("(", Token::PAREN_OPEN,0,0,0));
  l.push_back(Token("rgba", Token::IDENTIFIER,0,0,0));
  l.push_back(Token("(", Token::PAREN_OPEN,0,0,0));
  l.push_back(Token("10", Token::NUMBER,0,0,0));
  l.push_back(Token(",", Token::DELIMITER,0,0,0));
  l.push_back(Token("10", Token::NUMBER,0,0,0));
  l.push_back(Token(",", Token::DELIMITER,0,0,0));
  l.push_back(Token("10", Token::NUMBER,0,0,0));
  l.push_back(Token(",", Token::DELIMITER,0,0,0));
  l.push_back(Token("10%", Token::PERCENTAGE,0,0,0));
  l.push_back(Token(")", Token::PAREN_CLOSED,0,0,0));
  l.push_back(Token(",", Token::DELIMITER,0,0,0));
  l.push_back(Token("10%", Token::PERCENTAGE,0,0,0));
  l.push_back(Token(")", Token::PAREN_CLOSED,0,0,0));

  vp.processValue(l, c);
  
  ASSERT_EQ((uint)10, l.size());
  EXPECT_STREQ("rgba(10,10,10,0.2)", l.toString().c_str());

  l.clear();
  l.push_back(Token("fadeout", Token::IDENTIFIER,0,0,0));
  l.push_back(Token("(", Token::PAREN_OPEN,0,0,0));
  l.push_back(Token("#0A0A0A", Token::HASH,0,0,0));
  l.push_back(Token(",", Token::DELIMITER,0,0,0));
  l.push_back(Token("10%", Token::PERCENTAGE,0,0,0));
  l.push_back(Token(")", Token::PAREN_CLOSED,0,0,0));

  vp.processValue(l, c);
  
  ASSERT_EQ((uint)10, l.size());
  EXPECT_STREQ("rgba(10,10,10,0.9)", l.toString().c_str());
}

TEST(ValueProcessorTest, FunctionErrors) {
  TokenList l;
  ValueProcessor vp;
  ProcessingContext c;

  // function with wrong arguments.
  l.push_back(Token("rgb", Token::IDENTIFIER,0,0,0));
  l.push_back(Token("(", Token::PAREN_OPEN,0,0,0));
  l.push_back(Token("true", Token::IDENTIFIER,0,0,0));
  l.push_back(Token(",", Token::DELIMITER,0,0,0));
  l.push_back(Token("\"abc\"", Token::STRING,0,0,0));
  l.push_back(Token(",", Token::DELIMITER,0,0,0));
  l.push_back(Token("10px", Token::DIMENSION,0,0,0));
  l.push_back(Token(")", Token::PAREN_CLOSED,0,0,0));

  EXPECT_THROW(vp.processValue(l, c), ParseException*);

  // too many arguments.
  l.clear();
  l.push_back(Token("rgb", Token::IDENTIFIER,0,0,0));
  l.push_back(Token("(", Token::PAREN_OPEN,0,0,0));
  l.push_back(Token("10", Token::NUMBER,0,0,0));
  l.push_back(Token(",", Token::DELIMITER,0,0,0));
  l.push_back(Token("10", Token::NUMBER,0,0,0));
  l.push_back(Token(",", Token::DELIMITER,0,0,0));
  l.push_back(Token("10", Token::NUMBER,0,0,0));
  l.push_back(Token(",", Token::DELIMITER,0,0,0));
  l.push_back(Token("10", Token::NUMBER,0,0,0));
  l.push_back(Token(")", Token::PAREN_CLOSED,0,0,0));
  
  EXPECT_THROW(vp.processValue(l, c), ParseException*);

  // not enough arguments.
  l.clear();
  l.push_back(Token("rgb", Token::IDENTIFIER,0,0,0));
  l.push_back(Token("(", Token::PAREN_OPEN,0,0,0));
  l.push_back(Token("10", Token::NUMBER,0,0,0));
  l.push_back(Token(",", Token::DELIMITER,0,0,0));
  l.push_back(Token("10", Token::NUMBER,0,0,0));
  l.push_back(Token(")", Token::PAREN_CLOSED,0,0,0));

  EXPECT_THROW(vp.processValue(l, c), ParseException*);
}

TEST(ValueProcessorTest, GetUnit) {
  TokenList l;
  ValueProcessor vp;
  ProcessingContext c;

  l.push_back(Token("get-unit", Token::IDENTIFIER,0,0,0));
  l.push_back(Token("(", Token::PAREN_OPEN,0,0,0));
  l.push_back(Token("10px", Token::DIMENSION,0,0,0));
  l.push_back(Token(")", Token::PAREN_CLOSED,0,0,0));

  vp.processValue(l, c);
  
  ASSERT_EQ((uint)1, l.size());
  EXPECT_STREQ("px", l.toString().c_str());

  l.clear();
  l.push_back(Token("get-unit", Token::IDENTIFIER,0,0,0));
  l.push_back(Token("(", Token::PAREN_OPEN,0,0,0));
  l.push_back(Token("10", Token::NUMBER,0,0,0));
  l.push_back(Token(")", Token::PAREN_CLOSED,0,0,0));

  ASSERT_EQ((uint)0, l.size());
}

TEST(ValueProcessorTest, StringFormat) {
  TokenList l;
  ValueProcessor vp;
  ProcessingContext c;

  l.push_back(Token("%", Token::IDENTIFIER,0,0,0));
  l.push_back(Token("(", Token::PAREN_OPEN,0,0,0));
  l.push_back(Token("'%s %d %a'", Token::STRING,0,0,0));
  l.push_back(Token(",", Token::DELIMITER,0,0,0));
  l.push_back(Token("\"abc\"", Token::STRING,0,0,0));
  l.push_back(Token(",", Token::DELIMITER,0,0,0));
  l.push_back(Token("10", Token::NUMBER,0,0,0));
  l.push_back(Token(",", Token::DELIMITER,0,0,0));
  l.push_back(Token("10px", Token::DIMENSION,0,0,0));
  l.push_back(Token(")", Token::PAREN_CLOSED,0,0,0));

  vp.processValue(l, c);
  
  ASSERT_EQ((uint)1, l.size());
  EXPECT_STREQ("\"abc 10 10px\"", l.toString().c_str());

  l.clear();
  l.push_back(Token("%", Token::IDENTIFIER,0,0,0));
  l.push_back(Token("(", Token::PAREN_OPEN,0,0,0));
  l.push_back(Token("'%S %D %A'", Token::STRING,0,0,0));
  l.push_back(Token(",", Token::DELIMITER,0,0,0));
  l.push_back(Token("\"ab /c\"", Token::STRING,0,0,0));
  l.push_back(Token(",", Token::DELIMITER,0,0,0));
  l.push_back(Token("10", Token::NUMBER,0,0,0));
  l.push_back(Token(",", Token::DELIMITER,0,0,0));
  l.push_back(Token("10px", Token::DIMENSION,0,0,0));
  l.push_back(Token(")", Token::PAREN_CLOSED,0,0,0));

  ASSERT_EQ((uint)1, l.size());
  EXPECT_STREQ("\"ab%20%2Fc 10 10px\"", l.toString().c_str());
}

TEST(ValueProcessorTest, Replace) {
  TokenList l;
  ValueProcessor vp;
  ProcessingContext c;

  l.push_back(Token("replace", Token::IDENTIFIER,0,0,0));
  l.push_back(Token("(", Token::PAREN_OPEN,0,0,0));
  l.push_back(Token("'Hello, Mars?'", Token::STRING,0,0,0));
  l.push_back(Token(",", Token::DELIMITER,0,0,0));
  l.push_back(Token("'Mars\\?'", Token::STRING,0,0,0));
  l.push_back(Token(",", Token::DELIMITER,0,0,0));
  l.push_back(Token("'Earth!'", Token::STRING,0,0,0));
  l.push_back(Token(")", Token::PAREN_CLOSED,0,0,0));

  vp.processValue(l, c);
  
  ASSERT_EQ((uint)1, l.size());
  EXPECT_STREQ("'Hello, Earth!'", l.toString().c_str());

  l.clear();
  l.push_back(Token("replace", Token::IDENTIFIER,0,0,0));
  l.push_back(Token("(", Token::PAREN_OPEN,0,0,0));
  l.push_back(Token("'One + one = 4'", Token::STRING,0,0,0));
  l.push_back(Token(",", Token::DELIMITER,0,0,0));
  l.push_back(Token("'one'", Token::STRING,0,0,0));
  l.push_back(Token(",", Token::DELIMITER,0,0,0));
  l.push_back(Token("'2'", Token::STRING,0,0,0));
  l.push_back(Token(",", Token::DELIMITER,0,0,0));
  l.push_back(Token("'gi'", Token::STRING,0,0,0));
  l.push_back(Token(")", Token::PAREN_CLOSED,0,0,0));

  vp.processValue(l, c);
  
  ASSERT_EQ((uint)1, l.size());
  EXPECT_STREQ("'2 + 2 = 4'", l.toString().c_str());

  l.clear();
  l.push_back(Token("replace", Token::IDENTIFIER,0,0,0));
  l.push_back(Token("(", Token::PAREN_OPEN,0,0,0));
  l.push_back(Token("'This is a string.'", Token::STRING,0,0,0));
  l.push_back(Token(",", Token::DELIMITER,0,0,0));
  l.push_back(Token("'(string)\\.$'", Token::STRING,0,0,0));
  l.push_back(Token(",", Token::DELIMITER,0,0,0));
  l.push_back(Token("'new $1.'", Token::STRING,0,0,0));
  l.push_back(Token(")", Token::PAREN_CLOSED,0,0,0));

  vp.processValue(l, c);
  
  ASSERT_EQ((uint)1, l.size());
  EXPECT_STREQ("'This is a new string.'", l.toString().c_str());
}

TEST(ValueProcessorTest, Length) {
  TokenList l;
  ValueProcessor vp;
  ProcessingContext c;

  l.push_back(Token("length", Token::IDENTIFIER,0,0,0));
  l.push_back(Token("(", Token::PAREN_OPEN,0,0,0));
  l.push_back(Token("'banana'", Token::STRING,0,0,0));
  l.push_back(Token(" ", Token::WHITESPACE,0,0,0));
  l.push_back(Token("'tomato'", Token::STRING,0,0,0));
  l.push_back(Token(" ", Token::WHITESPACE,0,0,0));
  l.push_back(Token("'potato'", Token::STRING,0,0,0));
  l.push_back(Token(" ", Token::WHITESPACE,0,0,0));
  l.push_back(Token("'peach'", Token::STRING,0,0,0));
  l.push_back(Token(")", Token::PAREN_CLOSED,0,0,0));

  vp.processValue(l, c);
  
  ASSERT_EQ((uint)1, l.size());
  EXPECT_STREQ("4", l.toString().c_str());
}

TEST(ValueProcessorTest, Extract) {
  TokenList l;
  ValueProcessor vp;
  ProcessingContext c;

  l.push_back(Token("extract", Token::IDENTIFIER,0,0,0));
  l.push_back(Token("(", Token::PAREN_OPEN,0,0,0));
  l.push_back(Token("'banana'", Token::STRING,0,0,0));
  l.push_back(Token(" ", Token::WHITESPACE,0,0,0));
  l.push_back(Token("'tomato'", Token::STRING,0,0,0));
  l.push_back(Token(" ", Token::WHITESPACE,0,0,0));
  l.push_back(Token("'potato'", Token::STRING,0,0,0));
  l.push_back(Token(",", Token::DELIMITER,0,0,0));
  l.push_back(Token("3", Token::NUMBER,0,0,0));
  l.push_back(Token(")", Token::PAREN_CLOSED,0,0,0));

  vp.processValue(l, c);
  
  ASSERT_EQ((uint)1, l.size());
  EXPECT_STREQ("'potato'", l.toString().c_str());
}

TEST(ValueProcessorTest, Min) {
  TokenList l;
  ValueProcessor vp;
  ProcessingContext c;

  l.push_back(Token("min", Token::IDENTIFIER,0,0,0));
  l.push_back(Token("(", Token::PAREN_OPEN,0,0,0));
  l.push_back(Token("1", Token::NUMBER,0,0,0));
  l.push_back(Token(",", Token::DELIMITER,0,0,0));
  l.push_back(Token("10", Token::NUMBER,0,0,0));
  l.push_back(Token(",", Token::DELIMITER,0,0,0));
  l.push_back(Token("100", Token::NUMBER,0,0,0));
  l.push_back(Token(")", Token::PAREN_CLOSED,0,0,0));

  vp.processValue(l, c);
  
  EXPECT_STREQ("1", l.toString().c_str());
}

TEST(ValueProcessorTest, Max) {
  TokenList l;
  ValueProcessor vp;
  ProcessingContext c;

  l.push_back(Token("max", Token::IDENTIFIER,0,0,0));
  l.push_back(Token("(", Token::PAREN_OPEN,0,0,0));
  l.push_back(Token("1", Token::NUMBER,0,0,0));
  l.push_back(Token(",", Token::DELIMITER,0,0,0));
  l.push_back(Token("10", Token::NUMBER,0,0,0));
  l.push_back(Token(",", Token::DELIMITER,0,0,0));
  l.push_back(Token("100", Token::NUMBER,0,0,0));
  l.push_back(Token(")", Token::PAREN_CLOSED,0,0,0));

  vp.processValue(l, c);
  
  EXPECT_STREQ("100", l.toString().c_str());

  l.clear();
  l.push_back(Token("max", Token::IDENTIFIER,0,0,0));
  l.push_back(Token("(", Token::PAREN_OPEN,0,0,0));
  l.push_back(Token("'one'", Token::STRING,0,0,0));
  l.push_back(Token(",", Token::DELIMITER,0,0,0));
  l.push_back(Token("10", Token::NUMBER,0,0,0));
  l.push_back(Token(",", Token::DELIMITER,0,0,0));
  l.push_back(Token("100", Token::NUMBER,0,0,0));
  l.push_back(Token(")", Token::PAREN_CLOSED,0,0,0));

  vp.processValue(l, c);
  
  EXPECT_STREQ("max(\"one\", 10, 100)", l.toString().c_str());

  l.clear();
  l.push_back(Token("max", Token::IDENTIFIER,0,0,0));
  l.push_back(Token("(", Token::PAREN_OPEN,0,0,0));
  l.push_back(Token(")", Token::PAREN_CLOSED,0,0,0));

  vp.processValue(l, c);
  
  EXPECT_STREQ("max()", l.toString().c_str());

}

TEST(ValueProcessorTest, IsNumber) {
  TokenList l;
  ValueProcessor vp;
  ProcessingContext c;

  l.push_back(Token("isnumber", Token::IDENTIFIER,0,0,0));
  l.push_back(Token("(", Token::PAREN_OPEN,0,0,0));
  l.push_back(Token("1", Token::NUMBER,0,0,0));
  l.push_back(Token(")", Token::PAREN_CLOSED,0,0,0));

  vp.processValue(l, c);
  
  ASSERT_EQ((uint)1, l.size());
  EXPECT_STREQ("true", l.toString().c_str());

  l.clear();
  l.push_back(Token("isnumber", Token::IDENTIFIER,0,0,0));
  l.push_back(Token("(", Token::PAREN_OPEN,0,0,0));
  l.push_back(Token("1em", Token::DIMENSION,0,0,0));
  l.push_back(Token(")", Token::PAREN_CLOSED,0,0,0));

  vp.processValue(l, c);
  
  ASSERT_EQ((uint)1, l.size());
  EXPECT_STREQ("true", l.toString().c_str());

  l.clear();
  l.push_back(Token("isnumber", Token::IDENTIFIER,0,0,0));
  l.push_back(Token("(", Token::PAREN_OPEN,0,0,0));
  l.push_back(Token("1%", Token::PERCENTAGE,0,0,0));
  l.push_back(Token(")", Token::PAREN_CLOSED,0,0,0));

  vp.processValue(l, c);
  
  ASSERT_EQ((uint)1, l.size());
  EXPECT_STREQ("true", l.toString().c_str());

  l.clear();
  l.push_back(Token("isnumber", Token::IDENTIFIER,0,0,0));
  l.push_back(Token("(", Token::PAREN_OPEN,0,0,0));
  l.push_back(Token("'string'", Token::STRING,0,0,0));
  l.push_back(Token(")", Token::PAREN_CLOSED,0,0,0));

  vp.processValue(l, c);
  
  ASSERT_EQ((uint)1, l.size());
  EXPECT_STREQ("false", l.toString().c_str());
  
  l.clear();
  l.push_back(Token("isnumber", Token::IDENTIFIER,0,0,0));
  l.push_back(Token("(", Token::PAREN_OPEN,0,0,0));
  l.push_back(Token("keyword", Token::IDENTIFIER,0,0,0));
  l.push_back(Token(")", Token::PAREN_CLOSED,0,0,0));

  vp.processValue(l, c);
  
  ASSERT_EQ((uint)1, l.size());
  EXPECT_STREQ("false", l.toString().c_str());
}

TEST(ValueProcessorTest, IsString) {
  TokenList l;
  ValueProcessor vp;
  ProcessingContext c;

  l.push_back(Token("isstring", Token::IDENTIFIER,0,0,0));
  l.push_back(Token("(", Token::PAREN_OPEN,0,0,0));
  l.push_back(Token("1", Token::NUMBER,0,0,0));
  l.push_back(Token(")", Token::PAREN_CLOSED,0,0,0));

  vp.processValue(l, c);
  
  ASSERT_EQ((uint)1, l.size());
  EXPECT_STREQ("false", l.toString().c_str());

  l.clear();
  l.push_back(Token("isstring", Token::IDENTIFIER,0,0,0));
  l.push_back(Token("(", Token::PAREN_OPEN,0,0,0));
  l.push_back(Token("1em", Token::DIMENSION,0,0,0));
  l.push_back(Token(")", Token::PAREN_CLOSED,0,0,0));

  vp.processValue(l, c);
  
  ASSERT_EQ((uint)1, l.size());
  EXPECT_STREQ("false", l.toString().c_str());

  l.clear();
  l.push_back(Token("isstring", Token::IDENTIFIER,0,0,0));
  l.push_back(Token("(", Token::PAREN_OPEN,0,0,0));
  l.push_back(Token("1%", Token::PERCENTAGE,0,0,0));
  l.push_back(Token(")", Token::PAREN_CLOSED,0,0,0));

  vp.processValue(l, c);
  
  ASSERT_EQ((uint)1, l.size());
  EXPECT_STREQ("false", l.toString().c_str());

  l.clear();
  l.push_back(Token("isstring", Token::IDENTIFIER,0,0,0));
  l.push_back(Token("(", Token::PAREN_OPEN,0,0,0));
  l.push_back(Token("'string'", Token::STRING,0,0,0));
  l.push_back(Token(")", Token::PAREN_CLOSED,0,0,0));

  vp.processValue(l, c);
  
  ASSERT_EQ((uint)1, l.size());
  EXPECT_STREQ("true", l.toString().c_str());
  
  l.clear();
  l.push_back(Token("isstring", Token::IDENTIFIER,0,0,0));
  l.push_back(Token("(", Token::PAREN_OPEN,0,0,0));
  l.push_back(Token("keyword", Token::IDENTIFIER,0,0,0));
  l.push_back(Token(")", Token::PAREN_CLOSED,0,0,0));

  vp.processValue(l, c);
  
  ASSERT_EQ((uint)1, l.size());
  EXPECT_STREQ("false", l.toString().c_str());
}

TEST(ValueProcessorTest, IsColor) {
  TokenList l;
  ValueProcessor vp;
  ProcessingContext c;

  l.push_back(Token("iscolor", Token::IDENTIFIER,0,0,0));
  l.push_back(Token("(", Token::PAREN_OPEN,0,0,0));
  l.push_back(Token("1", Token::NUMBER,0,0,0));
  l.push_back(Token(")", Token::PAREN_CLOSED,0,0,0));

  vp.processValue(l, c);
  
  ASSERT_EQ((uint)1, l.size());
  EXPECT_STREQ("false", l.toString().c_str());

  l.clear();
  l.push_back(Token("iscolor", Token::IDENTIFIER,0,0,0));
  l.push_back(Token("(", Token::PAREN_OPEN,0,0,0));
  l.push_back(Token("1em", Token::DIMENSION,0,0,0));
  l.push_back(Token(")", Token::PAREN_CLOSED,0,0,0));

  vp.processValue(l, c);
  
  ASSERT_EQ((uint)1, l.size());
  EXPECT_STREQ("false", l.toString().c_str());

  l.clear();
  l.push_back(Token("iscolor", Token::IDENTIFIER,0,0,0));
  l.push_back(Token("(", Token::PAREN_OPEN,0,0,0));
  l.push_back(Token("1%", Token::PERCENTAGE,0,0,0));
  l.push_back(Token(")", Token::PAREN_CLOSED,0,0,0));

  vp.processValue(l, c);
  
  ASSERT_EQ((uint)1, l.size());
  EXPECT_STREQ("false", l.toString().c_str());

  l.clear();
  l.push_back(Token("iscolor", Token::IDENTIFIER,0,0,0));
  l.push_back(Token("(", Token::PAREN_OPEN,0,0,0));
  l.push_back(Token("'string'", Token::STRING,0,0,0));
  l.push_back(Token(")", Token::PAREN_CLOSED,0,0,0));

  vp.processValue(l, c);
  
  ASSERT_EQ((uint)1, l.size());
  EXPECT_STREQ("false", l.toString().c_str());
  
  l.clear();
  l.push_back(Token("iscolor", Token::IDENTIFIER,0,0,0));
  l.push_back(Token("(", Token::PAREN_OPEN,0,0,0));
  l.push_back(Token("keyword", Token::IDENTIFIER,0,0,0));
  l.push_back(Token(")", Token::PAREN_CLOSED,0,0,0));

  vp.processValue(l, c);
  
  ASSERT_EQ((uint)1, l.size());
  EXPECT_STREQ("false", l.toString().c_str());

  l.clear();
  l.push_back(Token("iscolor", Token::IDENTIFIER,0,0,0));
  l.push_back(Token("(", Token::PAREN_OPEN,0,0,0));
  l.push_back(Token("#fff", Token::HASH,0,0,0));
  l.push_back(Token(")", Token::PAREN_CLOSED,0,0,0));

  vp.processValue(l, c);
  
  ASSERT_EQ((uint)1, l.size());
  EXPECT_STREQ("true", l.toString().c_str());
}

TEST(ValueProcessorTest, IsKeyword) {
  TokenList l;
  ValueProcessor vp;
  ProcessingContext c;

  l.push_back(Token("iskeyword", Token::IDENTIFIER,0,0,0));
  l.push_back(Token("(", Token::PAREN_OPEN,0,0,0));
  l.push_back(Token("1", Token::NUMBER,0,0,0));
  l.push_back(Token(")", Token::PAREN_CLOSED,0,0,0));

  vp.processValue(l, c);
  
  ASSERT_EQ((uint)1, l.size());
  EXPECT_STREQ("false", l.toString().c_str());

  l.clear();
  l.push_back(Token("iskeyword", Token::IDENTIFIER,0,0,0));
  l.push_back(Token("(", Token::PAREN_OPEN,0,0,0));
  l.push_back(Token("1em", Token::DIMENSION,0,0,0));
  l.push_back(Token(")", Token::PAREN_CLOSED,0,0,0));

  vp.processValue(l, c);
  
  ASSERT_EQ((uint)1, l.size());
  EXPECT_STREQ("false", l.toString().c_str());

  l.clear();
  l.push_back(Token("iskeyword", Token::IDENTIFIER,0,0,0));
  l.push_back(Token("(", Token::PAREN_OPEN,0,0,0));
  l.push_back(Token("1%", Token::PERCENTAGE,0,0,0));
  l.push_back(Token(")", Token::PAREN_CLOSED,0,0,0));

  vp.processValue(l, c);
  
  ASSERT_EQ((uint)1, l.size());
  EXPECT_STREQ("false", l.toString().c_str());

  l.clear();
  l.push_back(Token("iskeyword", Token::IDENTIFIER,0,0,0));
  l.push_back(Token("(", Token::PAREN_OPEN,0,0,0));
  l.push_back(Token("'string'", Token::STRING,0,0,0));
  l.push_back(Token(")", Token::PAREN_CLOSED,0,0,0));

  vp.processValue(l, c);
  
  ASSERT_EQ((uint)1, l.size());
  EXPECT_STREQ("false", l.toString().c_str());
  
  l.clear();
  l.push_back(Token("iskeyword", Token::IDENTIFIER,0,0,0));
  l.push_back(Token("(", Token::PAREN_OPEN,0,0,0));
  l.push_back(Token("keyword", Token::IDENTIFIER,0,0,0));
  l.push_back(Token(")", Token::PAREN_CLOSED,0,0,0));

  vp.processValue(l, c);
  
  ASSERT_EQ((uint)1, l.size());
  EXPECT_STREQ("true", l.toString().c_str());

  l.clear();
  l.push_back(Token("iskeyword", Token::IDENTIFIER,0,0,0));
  l.push_back(Token("(", Token::PAREN_OPEN,0,0,0));
  l.push_back(Token("#fff", Token::HASH,0,0,0));
  l.push_back(Token(")", Token::PAREN_CLOSED,0,0,0));

  vp.processValue(l, c);
  
  ASSERT_EQ((uint)1, l.size());
  EXPECT_STREQ("false", l.toString().c_str());
}

TEST(ValueProcessorTest, IsUrl) {
  TokenList l;
  ValueProcessor vp;
  ProcessingContext c;

  l.push_back(Token("isurl", Token::IDENTIFIER,0,0,0));
  l.push_back(Token("(", Token::PAREN_OPEN,0,0,0));
  l.push_back(Token("1", Token::NUMBER,0,0,0));
  l.push_back(Token(")", Token::PAREN_CLOSED,0,0,0));

  vp.processValue(l, c);
  
  ASSERT_EQ((uint)1, l.size());
  EXPECT_STREQ("false", l.toString().c_str());

  l.clear();
  l.push_back(Token("isurl", Token::IDENTIFIER,0,0,0));
  l.push_back(Token("(", Token::PAREN_OPEN,0,0,0));
  l.push_back(Token("1em", Token::DIMENSION,0,0,0));
  l.push_back(Token(")", Token::PAREN_CLOSED,0,0,0));

  vp.processValue(l, c);
  
  ASSERT_EQ((uint)1, l.size());
  EXPECT_STREQ("false", l.toString().c_str());

  l.clear();
  l.push_back(Token("isurl", Token::IDENTIFIER,0,0,0));
  l.push_back(Token("(", Token::PAREN_OPEN,0,0,0));
  l.push_back(Token("1%", Token::PERCENTAGE,0,0,0));
  l.push_back(Token(")", Token::PAREN_CLOSED,0,0,0));

  vp.processValue(l, c);
  
  ASSERT_EQ((uint)1, l.size());
  EXPECT_STREQ("false", l.toString().c_str());

  l.clear();
  l.push_back(Token("isurl", Token::IDENTIFIER,0,0,0));
  l.push_back(Token("(", Token::PAREN_OPEN,0,0,0));
  l.push_back(Token("'string'", Token::STRING,0,0,0));
  l.push_back(Token(")", Token::PAREN_CLOSED,0,0,0));

  vp.processValue(l, c);
  
  ASSERT_EQ((uint)1, l.size());
  EXPECT_STREQ("false", l.toString().c_str());
  
  l.clear();
  l.push_back(Token("isurl", Token::IDENTIFIER,0,0,0));
  l.push_back(Token("(", Token::PAREN_OPEN,0,0,0));
  l.push_back(Token("keyword", Token::IDENTIFIER,0,0,0));
  l.push_back(Token(")", Token::PAREN_CLOSED,0,0,0));

  vp.processValue(l, c);
  
  ASSERT_EQ((uint)1, l.size());
  EXPECT_STREQ("false", l.toString().c_str());

  l.clear();
  l.push_back(Token("isurl", Token::IDENTIFIER,0,0,0));
  l.push_back(Token("(", Token::PAREN_OPEN,0,0,0));
  l.push_back(Token("#fff", Token::HASH,0,0,0));
  l.push_back(Token(")", Token::PAREN_CLOSED,0,0,0));

  vp.processValue(l, c);
  
  ASSERT_EQ((uint)1, l.size());
  EXPECT_STREQ("false", l.toString().c_str());

  l.clear();
  l.push_back(Token("isurl", Token::IDENTIFIER,0,0,0));
  l.push_back(Token("(", Token::PAREN_OPEN,0,0,0));
  l.push_back(Token("url('...')", Token::URL,0,0,0));
  l.push_back(Token(")", Token::PAREN_CLOSED,0,0,0));

  vp.processValue(l, c);
  
  ASSERT_EQ((uint)1, l.size());
  EXPECT_STREQ("true", l.toString().c_str());
}

TEST(ValueProcessorTest, IsPixel) {
  TokenList l;
  ValueProcessor vp;
  ProcessingContext c;

  l.push_back(Token("ispixel", Token::IDENTIFIER,0,0,0));
  l.push_back(Token("(", Token::PAREN_OPEN,0,0,0));
  l.push_back(Token("1", Token::NUMBER,0,0,0));
  l.push_back(Token(")", Token::PAREN_CLOSED,0,0,0));

  vp.processValue(l, c);
  
  ASSERT_EQ((uint)1, l.size());
  EXPECT_STREQ("false", l.toString().c_str());

  l.clear();
  l.push_back(Token("ispixel", Token::IDENTIFIER,0,0,0));
  l.push_back(Token("(", Token::PAREN_OPEN,0,0,0));
  l.push_back(Token("1em", Token::DIMENSION,0,0,0));
  l.push_back(Token(")", Token::PAREN_CLOSED,0,0,0));

  vp.processValue(l, c);
  
  ASSERT_EQ((uint)1, l.size());
  EXPECT_STREQ("false", l.toString().c_str());

  l.clear();
  l.push_back(Token("ispixel", Token::IDENTIFIER,0,0,0));
  l.push_back(Token("(", Token::PAREN_OPEN,0,0,0));
  l.push_back(Token("1%", Token::PERCENTAGE,0,0,0));
  l.push_back(Token(")", Token::PAREN_CLOSED,0,0,0));

  vp.processValue(l, c);
  
  ASSERT_EQ((uint)1, l.size());
  EXPECT_STREQ("false", l.toString().c_str());

  l.clear();
  l.push_back(Token("ispixel", Token::IDENTIFIER,0,0,0));
  l.push_back(Token("(", Token::PAREN_OPEN,0,0,0));
  l.push_back(Token("'string'", Token::STRING,0,0,0));
  l.push_back(Token(")", Token::PAREN_CLOSED,0,0,0));

  vp.processValue(l, c);
  
  ASSERT_EQ((uint)1, l.size());
  EXPECT_STREQ("false", l.toString().c_str());
  
  l.clear();
  l.push_back(Token("ispixel", Token::IDENTIFIER,0,0,0));
  l.push_back(Token("(", Token::PAREN_OPEN,0,0,0));
  l.push_back(Token("keyword", Token::IDENTIFIER,0,0,0));
  l.push_back(Token(")", Token::PAREN_CLOSED,0,0,0));

  vp.processValue(l, c);
  
  ASSERT_EQ((uint)1, l.size());
  EXPECT_STREQ("false", l.toString().c_str());

  l.clear();
  l.push_back(Token("ispixel", Token::IDENTIFIER,0,0,0));
  l.push_back(Token("(", Token::PAREN_OPEN,0,0,0));
  l.push_back(Token("#fff", Token::HASH,0,0,0));
  l.push_back(Token(")", Token::PAREN_CLOSED,0,0,0));

  vp.processValue(l, c);
  
  ASSERT_EQ((uint)1, l.size());
  EXPECT_STREQ("false", l.toString().c_str());

  l.clear();
  l.push_back(Token("ispixel", Token::IDENTIFIER,0,0,0));
  l.push_back(Token("(", Token::PAREN_OPEN,0,0,0));
  l.push_back(Token("url('...')", Token::URL,0,0,0));
  l.push_back(Token(")", Token::PAREN_CLOSED,0,0,0));

  vp.processValue(l, c);
  
  ASSERT_EQ((uint)1, l.size());
  EXPECT_STREQ("false", l.toString().c_str());

  l.clear();
  l.push_back(Token("ispixel", Token::IDENTIFIER,0,0,0));
  l.push_back(Token("(", Token::PAREN_OPEN,0,0,0));
  l.push_back(Token("1px", Token::DIMENSION,0,0,0));
  l.push_back(Token(")", Token::PAREN_CLOSED,0,0,0));

  vp.processValue(l, c);
  
  ASSERT_EQ((uint)1, l.size());
  EXPECT_STREQ("true", l.toString().c_str());
}

TEST(ValueProcessorTest, IsEm) {
  TokenList l;
  ValueProcessor vp;
  ProcessingContext c;

  l.push_back(Token("isem", Token::IDENTIFIER,0,0,0));
  l.push_back(Token("(", Token::PAREN_OPEN,0,0,0));
  l.push_back(Token("1", Token::NUMBER,0,0,0));
  l.push_back(Token(")", Token::PAREN_CLOSED,0,0,0));

  vp.processValue(l, c);
  
  ASSERT_EQ((uint)1, l.size());
  EXPECT_STREQ("false", l.toString().c_str());

  l.clear();
  l.push_back(Token("isem", Token::IDENTIFIER,0,0,0));
  l.push_back(Token("(", Token::PAREN_OPEN,0,0,0));
  l.push_back(Token("1em", Token::DIMENSION,0,0,0));
  l.push_back(Token(")", Token::PAREN_CLOSED,0,0,0));

  vp.processValue(l, c);
  
  ASSERT_EQ((uint)1, l.size());
  EXPECT_STREQ("true", l.toString().c_str());

  l.clear();
  l.push_back(Token("isem", Token::IDENTIFIER,0,0,0));
  l.push_back(Token("(", Token::PAREN_OPEN,0,0,0));
  l.push_back(Token("1%", Token::PERCENTAGE,0,0,0));
  l.push_back(Token(")", Token::PAREN_CLOSED,0,0,0));

  vp.processValue(l, c);
  
  ASSERT_EQ((uint)1, l.size());
  EXPECT_STREQ("false", l.toString().c_str());

  l.clear();
  l.push_back(Token("isem", Token::IDENTIFIER,0,0,0));
  l.push_back(Token("(", Token::PAREN_OPEN,0,0,0));
  l.push_back(Token("'string'", Token::STRING,0,0,0));
  l.push_back(Token(")", Token::PAREN_CLOSED,0,0,0));

  vp.processValue(l, c);
  
  ASSERT_EQ((uint)1, l.size());
  EXPECT_STREQ("false", l.toString().c_str());
  
  l.clear();
  l.push_back(Token("isem", Token::IDENTIFIER,0,0,0));
  l.push_back(Token("(", Token::PAREN_OPEN,0,0,0));
  l.push_back(Token("keyword", Token::IDENTIFIER,0,0,0));
  l.push_back(Token(")", Token::PAREN_CLOSED,0,0,0));

  vp.processValue(l, c);
  
  ASSERT_EQ((uint)1, l.size());
  EXPECT_STREQ("false", l.toString().c_str());

  l.clear();
  l.push_back(Token("isem", Token::IDENTIFIER,0,0,0));
  l.push_back(Token("(", Token::PAREN_OPEN,0,0,0));
  l.push_back(Token("#fff", Token::HASH,0,0,0));
  l.push_back(Token(")", Token::PAREN_CLOSED,0,0,0));

  vp.processValue(l, c);
  
  ASSERT_EQ((uint)1, l.size());
  EXPECT_STREQ("false", l.toString().c_str());

  l.clear();
  l.push_back(Token("isem", Token::IDENTIFIER,0,0,0));
  l.push_back(Token("(", Token::PAREN_OPEN,0,0,0));
  l.push_back(Token("url('...')", Token::URL,0,0,0));
  l.push_back(Token(")", Token::PAREN_CLOSED,0,0,0));

  vp.processValue(l, c);
  
  ASSERT_EQ((uint)1, l.size());
  EXPECT_STREQ("false", l.toString().c_str());

  l.clear();
  l.push_back(Token("isem", Token::IDENTIFIER,0,0,0));
  l.push_back(Token("(", Token::PAREN_OPEN,0,0,0));
  l.push_back(Token("1px", Token::URL,0,0,0));
  l.push_back(Token(")", Token::PAREN_CLOSED,0,0,0));

  vp.processValue(l, c);
  
  ASSERT_EQ((uint)1, l.size());
  EXPECT_STREQ("false", l.toString().c_str());
}

TEST(ValueProcessorTest, IsPercentage) {
  TokenList l;
  ValueProcessor vp;
  ProcessingContext c;

  l.push_back(Token("ispercentage", Token::IDENTIFIER,0,0,0));
  l.push_back(Token("(", Token::PAREN_OPEN,0,0,0));
  l.push_back(Token("1", Token::NUMBER,0,0,0));
  l.push_back(Token(")", Token::PAREN_CLOSED,0,0,0));

  vp.processValue(l, c);

  EXPECT_STREQ("false", l.toString().c_str());

  l.clear();
  l.push_back(Token("ispercentage", Token::IDENTIFIER,0,0,0));
  l.push_back(Token("(", Token::PAREN_OPEN,0,0,0));
  l.push_back(Token("1em", Token::DIMENSION,0,0,0));
  l.push_back(Token(")", Token::PAREN_CLOSED,0,0,0));

  vp.processValue(l, c);

  EXPECT_STREQ("false", l.toString().c_str());

  l.clear();
  l.push_back(Token("ispercentage", Token::IDENTIFIER,0,0,0));
  l.push_back(Token("(", Token::PAREN_OPEN,0,0,0));
  l.push_back(Token("1%", Token::PERCENTAGE,0,0,0));
  l.push_back(Token(")", Token::PAREN_CLOSED,0,0,0));

  vp.processValue(l, c);

  EXPECT_STREQ("true", l.toString().c_str());

  l.clear();
  l.push_back(Token("ispercentage", Token::IDENTIFIER,0,0,0));
  l.push_back(Token("(", Token::PAREN_OPEN,0,0,0));
  l.push_back(Token("'string'", Token::STRING,0,0,0));
  l.push_back(Token(")", Token::PAREN_CLOSED,0,0,0));

  vp.processValue(l, c);

  EXPECT_STREQ("false", l.toString().c_str());

  l.clear();
  l.push_back(Token("ispercentage", Token::IDENTIFIER,0,0,0));
  l.push_back(Token("(", Token::PAREN_OPEN,0,0,0));
  l.push_back(Token("keyword", Token::IDENTIFIER,0,0,0));
  l.push_back(Token(")", Token::PAREN_CLOSED,0,0,0));

  vp.processValue(l, c);

  EXPECT_STREQ("false", l.toString().c_str());

  l.clear();
  l.push_back(Token("ispercentage", Token::IDENTIFIER,0,0,0));
  l.push_back(Token("(", Token::PAREN_OPEN,0,0,0));
  l.push_back(Token("#fff", Token::HASH,0,0,0));
  l.push_back(Token(")", Token::PAREN_CLOSED,0,0,0));

  vp.processValue(l, c);

  EXPECT_STREQ("false", l.toString().c_str());

  l.clear();
  l.push_back(Token("ispercentage", Token::IDENTIFIER,0,0,0));
  l.push_back(Token("(", Token::PAREN_OPEN,0,0,0));
  l.push_back(Token("url('...')", Token::URL,0,0,0));
  l.push_back(Token(")", Token::PAREN_CLOSED,0,0,0));

  vp.processValue(l, c);
  
  EXPECT_STREQ("false", l.toString().c_str());

  l.clear();
  l.push_back(Token("ispercentage", Token::IDENTIFIER,0,0,0));
  l.push_back(Token("(", Token::PAREN_OPEN,0,0,0));
  l.push_back(Token("1px", Token::URL,0,0,0));
  l.push_back(Token(")", Token::PAREN_CLOSED,0,0,0));

  vp.processValue(l, c);
  
  EXPECT_STREQ("false", l.toString().c_str());
}

TEST(ValueProcessorTest, IsUnit) {
  TokenList l;
  ValueProcessor vp;
  ProcessingContext c;

  l.push_back(Token("isunit", Token::IDENTIFIER,0,0,0));
  l.push_back(Token("(", Token::PAREN_OPEN,0,0,0));
  l.push_back(Token("1px", Token::DIMENSION,0,0,0));
  l.push_back(Token(",", Token::DELIMITER,0,0,0));
  l.push_back(Token("px", Token::IDENTIFIER,0,0,0));
  l.push_back(Token(")", Token::PAREN_CLOSED,0,0,0));

  vp.processValue(l, c);
  
  EXPECT_STREQ("true", l.toString().c_str());

  l.clear();
  l.push_back(Token("isunit", Token::IDENTIFIER,0,0,0));
  l.push_back(Token("(", Token::PAREN_OPEN,0,0,0));
  l.push_back(Token("1%", Token::PERCENTAGE,0,0,0));
  l.push_back(Token(",", Token::DELIMITER,0,0,0));
  l.push_back(Token("'%'", Token::STRING,0,0,0));
  l.push_back(Token(")", Token::PAREN_CLOSED,0,0,0));

  vp.processValue(l, c);
  
  EXPECT_STREQ("true", l.toString().c_str());
  
  l.clear();
  l.push_back(Token("isunit", Token::IDENTIFIER,0,0,0));
  l.push_back(Token("(", Token::PAREN_OPEN,0,0,0));
  l.push_back(Token("1", Token::NUMBER,0,0,0));
  l.push_back(Token(",", Token::DELIMITER,0,0,0));
  l.push_back(Token("em", Token::IDENTIFIER,0,0,0));
  l.push_back(Token(")", Token::PAREN_CLOSED,0,0,0));

  vp.processValue(l, c);
  
  EXPECT_STREQ("false", l.toString().c_str());

  l.clear();
  l.push_back(Token("isunit", Token::IDENTIFIER,0,0,0));
  l.push_back(Token("(", Token::PAREN_OPEN,0,0,0));
  l.push_back(Token("1em", Token::DIMENSION,0,0,0));
  l.push_back(Token(",", Token::DELIMITER,0,0,0));
  l.push_back(Token("'%'", Token::IDENTIFIER,0,0,0));
  l.push_back(Token(")", Token::PAREN_CLOSED,0,0,0));

  vp.processValue(l, c);
  
  EXPECT_STREQ("false", l.toString().c_str());
}

// `isruleset`
/*
- `hsla`: no
- `hsv`: no
- `hsva`: no
- `hsvue`: no
- `hsvsaturation`: no
- `hsvvalue`: no
- `luma`: no
- `luminance`: no
- `fade`: no
- `mix`: no
- `tint`: no
- `shade`: no
- `greyscale`: no
- `contrast`: no
- `multiply`: no
- `screen`: no
- `overlay`: no
- `softlight`: no
- `hardlight`: no
- `difference`: no
- `exclusion`: no
- `average`: no
- `negation`: no
*/
