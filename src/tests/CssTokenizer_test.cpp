#include "../CssTokenizer.h"
#include "gtest/gtest.h"

TEST(CssTokenizerTest, ReckognizesTokens) {
  istringstream in("identifier @atkeyword \"string\" #hash 1234 %100 1.5em url(../img/img.png) u+123 : ; { } ( ) [ ] /* comment */ function( ~= |= ~ ");
    
  CssTokenizer t(&in);
  EXPECT_EQ(Token::IDENTIFIER, t.readNextToken());
  EXPECT_EQ(Token::WHITESPACE, t.readNextToken());
  EXPECT_EQ(Token::ATKEYWORD, t.readNextToken());
  EXPECT_EQ(Token::WHITESPACE, t.readNextToken());
}
