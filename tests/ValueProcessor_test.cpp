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
#include "gtest/gtest.h"

TEST(ValueProcessorTest, Operators) {
  TokenList l;
  ValueProcessor vp;

  // add
  l.push(new Token("1", Token::NUMBER));
  l.push(new Token("+", Token::DELIMITER));
  l.push(new Token("1", Token::NUMBER));

  vp.processValue(&l);
  
  ASSERT_EQ((uint)1, l.size());
  ASSERT_EQ(Token::NUMBER, l.front()->type);
  EXPECT_STREQ("2", l.front()->str.c_str());

  // substract
  l.push(new Token("-", Token::DELIMITER));
  l.push(new Token("1", Token::NUMBER));

  vp.processValue(&l);
  
  ASSERT_EQ((uint)1, l.size());
  ASSERT_EQ(Token::NUMBER, l.front()->type);
  EXPECT_STREQ("1", l.front()->str.c_str());

  // multiply
  l.push(new Token("*", Token::DELIMITER));
  l.push(new Token("5", Token::NUMBER));

  vp.processValue(&l);
  
  ASSERT_EQ((uint)1, l.size());
  ASSERT_EQ(Token::NUMBER, l.front()->type);
  EXPECT_STREQ("5", l.front()->str.c_str());

  // divide
  l.push(new Token("/", Token::DELIMITER));
  l.push(new Token("2", Token::NUMBER));

  vp.processValue(&l);
  
  ASSERT_EQ((uint)1, l.size());
  ASSERT_EQ(Token::NUMBER, l.front()->type);
  EXPECT_STREQ("2.5", l.front()->str.c_str());

  // compare
  l.push(new Token("<", Token::DELIMITER));
  l.push(new Token("10", Token::NUMBER));

  vp.processValue(&l);
  
  ASSERT_EQ((uint)1, l.size());
  ASSERT_EQ(Token::IDENTIFIER, l.front()->type);
  EXPECT_STREQ("true", l.front()->str.c_str());
}

TEST(ValueProcessorTest, OperandMismatch) {
  TokenList l;
  ValueProcessor vp;
  
  l.push(new Token("true", Token::IDENTIFIER));
  l.push(new Token("+", Token::DELIMITER));
  l.push(new Token("#FFF", Token::HASH));

  EXPECT_THROW(vp.processValue(&l), ValueException*);
}

TEST(ValueProcessorTest, OperatorsInParens) {
  TokenList l;
  ValueProcessor vp;

  l.push(new Token("25", Token::NUMBER));
  l.push(new Token("/", Token::DELIMITER));
  l.push(new Token("(", Token::PAREN_OPEN));
  l.push(new Token("2", Token::NUMBER));
  l.push(new Token("+", Token::DELIMITER));
  l.push(new Token("3", Token::NUMBER));
  l.push(new Token(")", Token::PAREN_CLOSED));

  vp.processValue(&l);
  
  ASSERT_EQ((uint)1, l.size());
  EXPECT_STREQ("5", l.front()->str.c_str());
  ASSERT_EQ(Token::NUMBER, l.front()->type);
}


TEST(ValueProcessorTest, StringOperations) {
  TokenList l;
  ValueProcessor vp;
}
TEST(ValueProcessorTest, ColorOperations) {
  TokenList l;
  ValueProcessor vp;

  l.push(new Token("rgb", Token::IDENTIFIER));
  l.push(new Token("(", Token::PAREN_OPEN));
  l.push(new Token("10", Token::NUMBER));
  l.push(new Token(",", Token::DELIMITER));
  l.push(new Token("10", Token::NUMBER));
  l.push(new Token(",", Token::DELIMITER));
  l.push(new Token("10", Token::NUMBER));
  l.push(new Token(")", Token::PAREN_CLOSED));

  vp.processValue(&l);
  
  ASSERT_EQ((uint)1, l.size());
  EXPECT_STREQ("#0a0a0a", l.front()->str.c_str());
  ASSERT_EQ(Token::HASH, l.front()->type);

  l.clear();
  l.push(new Token("rgba", Token::IDENTIFIER));
  l.push(new Token("(", Token::PAREN_OPEN));
  l.push(new Token("10", Token::NUMBER));
  l.push(new Token(",", Token::DELIMITER));
  l.push(new Token("10", Token::NUMBER));
  l.push(new Token(",", Token::DELIMITER));
  l.push(new Token("10", Token::NUMBER));
  l.push(new Token(",", Token::DELIMITER));
  l.push(new Token("10%", Token::PERCENTAGE));
  l.push(new Token(")", Token::PAREN_CLOSED));

  vp.processValue(&l);
  
  ASSERT_EQ((uint)10, l.size());
  EXPECT_STREQ("rgba(10,10,10,0.1)", l.toString()->c_str());

  l.clear();
  l.push(new Token("lighten", Token::IDENTIFIER));
  l.push(new Token("(", Token::PAREN_OPEN));
  l.push(new Token("#AAA", Token::HASH));
  l.push(new Token(",", Token::DELIMITER));
  l.push(new Token("10%", Token::PERCENTAGE));
  l.push(new Token(")", Token::PAREN_CLOSED));

  vp.processValue(&l);
  
  ASSERT_EQ((uint)1, l.size());
  ASSERT_EQ(Token::HASH, l.front()->type);
  EXPECT_STREQ("#c3c3c3", l.front()->str.c_str());

  l.clear();
  l.push(new Token("darken", Token::IDENTIFIER));
  l.push(new Token("(", Token::PAREN_OPEN));
  l.push(new Token("#AAA", Token::HASH));
  l.push(new Token(",", Token::DELIMITER));
  l.push(new Token("10%", Token::PERCENTAGE));
  l.push(new Token(")", Token::PAREN_CLOSED));

  vp.processValue(&l);
  
  ASSERT_EQ((uint)1, l.size());
  ASSERT_EQ(Token::HASH, l.front()->type);
  EXPECT_STREQ("#909090", l.front()->str.c_str());

  l.clear();
  l.push(new Token("saturate", Token::IDENTIFIER));
  l.push(new Token("(", Token::PAREN_OPEN));
  l.push(new Token("#AAA", Token::HASH));
  l.push(new Token(",", Token::DELIMITER));
  l.push(new Token("10%", Token::PERCENTAGE));
  l.push(new Token(")", Token::PAREN_CLOSED));

  vp.processValue(&l);
  
  ASSERT_EQ((uint)1, l.size());
  ASSERT_EQ(Token::HASH, l.front()->type);
  EXPECT_STREQ("#b2a1a1", l.front()->str.c_str());

  l.clear();
  l.push(new Token("desaturate", Token::IDENTIFIER));
  l.push(new Token("(", Token::PAREN_OPEN));
  l.push(new Token("#123456", Token::HASH));
  l.push(new Token(",", Token::DELIMITER));
  l.push(new Token("10%", Token::PERCENTAGE));
  l.push(new Token(")", Token::PAREN_CLOSED));

  vp.processValue(&l);
  
  ASSERT_EQ((uint)1, l.size());
  ASSERT_EQ(Token::HASH, l.front()->type);
  EXPECT_STREQ("#173451", l.front()->str.c_str());

  l.clear();
  l.push(new Token("fadein", Token::IDENTIFIER));
  l.push(new Token("(", Token::PAREN_OPEN));
  l.push(new Token("rgba", Token::IDENTIFIER));
  l.push(new Token("(", Token::PAREN_OPEN));
  l.push(new Token("10", Token::NUMBER));
  l.push(new Token(",", Token::DELIMITER));
  l.push(new Token("10", Token::NUMBER));
  l.push(new Token(",", Token::DELIMITER));
  l.push(new Token("10", Token::NUMBER));
  l.push(new Token(",", Token::DELIMITER));
  l.push(new Token("10%", Token::PERCENTAGE));
  l.push(new Token(")", Token::PAREN_CLOSED));
  l.push(new Token(",", Token::DELIMITER));
  l.push(new Token("10%", Token::PERCENTAGE));
  l.push(new Token(")", Token::PAREN_CLOSED));

  vp.processValue(&l);
  
  ASSERT_EQ((uint)10, l.size());
  EXPECT_STREQ("rgba(10,10,10,0.11)", l.toString()->c_str());

  l.clear();
  l.push(new Token("fadeout", Token::IDENTIFIER));
  l.push(new Token("(", Token::PAREN_OPEN));
  l.push(new Token("#0A0A0A", Token::HASH));
  l.push(new Token(",", Token::DELIMITER));
  l.push(new Token("10%", Token::PERCENTAGE));
  l.push(new Token(")", Token::PAREN_CLOSED));

  vp.processValue(&l);
  
  ASSERT_EQ((uint)10, l.size());
  EXPECT_STREQ("rgba(10,10,10,0.9)", l.toString()->c_str());
}

TEST(ValueProcessorTest, FunctionErrors) {
  TokenList l;
  ValueProcessor vp;

  // function with wrong arguments.
  l.push(new Token("rgb", Token::IDENTIFIER));
  l.push(new Token("(", Token::PAREN_OPEN));
  l.push(new Token("true", Token::IDENTIFIER));
  l.push(new Token(",", Token::DELIMITER));
  l.push(new Token("\"abc\"", Token::STRING));
  l.push(new Token(",", Token::DELIMITER));
  l.push(new Token("10px", Token::DIMENSION));
  l.push(new Token(")", Token::PAREN_CLOSED));

  EXPECT_THROW(vp.processValue(&l), ParseException*);

  // too many arguments.
  l.clear();
  l.push(new Token("rgb", Token::IDENTIFIER));
  l.push(new Token("(", Token::PAREN_OPEN));
  l.push(new Token("10", Token::NUMBER));
  l.push(new Token(",", Token::DELIMITER));
  l.push(new Token("10", Token::NUMBER));
  l.push(new Token(",", Token::DELIMITER));
  l.push(new Token("10", Token::NUMBER));
  l.push(new Token(",", Token::DELIMITER));
  l.push(new Token("10", Token::NUMBER));
  l.push(new Token(")", Token::PAREN_CLOSED));
  
  EXPECT_THROW(vp.processValue(&l), ParseException*);

  // not enough arguments.
  l.clear();
  l.push(new Token("rgb", Token::IDENTIFIER));
  l.push(new Token("(", Token::PAREN_OPEN));
  l.push(new Token("10", Token::NUMBER));
  l.push(new Token(",", Token::DELIMITER));
  l.push(new Token("10", Token::NUMBER));
  l.push(new Token(")", Token::PAREN_CLOSED));

  EXPECT_THROW(vp.processValue(&l), ParseException*);
}
