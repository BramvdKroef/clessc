/*
 * Copyright 2012 Bram van der Kroef
 *
 * This file is part of LESS CSS Compiler.
 *
 * LESS CSS Compiler is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * LESS CSS Compiler is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with LESS CSS Compiler.  If not, see <http://www.gnu.org/licenses/>. 
 *
 * Author: Bram van der Kroef <bram@vanderkroef.net>
 */

#include "value/ValueProcessor.h"
#include "lessstylesheet/ProcessingContext.h"
#include "gtest/gtest.h"

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
