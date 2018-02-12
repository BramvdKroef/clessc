#include <list>
#include <gtest/gtest.h>
#include <less/lessstylesheet/LessStylesheet.h>
#include <less/lessstylesheet/LessRuleset.h>

class LessRulesetTest : public ::testing::Test {
public:
  LessSelector *selector;
  LessStylesheet *stylesheet;
  LessRuleset *ruleset;

  virtual void SetUp() {
    TokenList tokens;
    tokens.push_back(Token(".", Token::OTHER, 0, 0, "-"));
    tokens.push_back(Token("test", Token::IDENTIFIER, 0, 0, "-"));
    
    selector = new LessSelector();
    selector->appendSelector(tokens);
    stylesheet = new LessStylesheet();
    ruleset = stylesheet->createLessRuleset(*selector);
  }
  
  virtual void TearDown() {
    delete stylesheet;
  }
};

TEST_F(LessRulesetTest, Variable) {
  TokenList value;
  value.push_back(Token("1px", Token::DIMENSION, 0, 0, "-"));

  ruleset->putVariable("@test", value);
  ASSERT_STREQ("1px", ruleset->getVariable("@test")->toString().c_str());
}


TEST_F(LessRulesetTest, Parent) {
  ASSERT_EQ(stylesheet, ruleset->getLessStylesheet());
  ASSERT_EQ(NULL, ruleset->getParent());
}


