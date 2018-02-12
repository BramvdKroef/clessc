#include <gtest/gtest.h>
#include <less/css/CssParser.h>
#include <less/stylesheet/Stylesheet.h>
#include <less/stylesheet/AtRule.h>
#include <less/stylesheet/Ruleset.h>
#include <less/stylesheet/Declaration.h>

class CssParserTest : public ::testing::Test {
public:
  istringstream* in;
    
  CssTokenizer* t;
  CssParser* p;

  virtual void SetUp() {
    in = new istringstream(" ");
    t = new CssTokenizer(*in, "test");
    p = new CssParser(*t);
  }
  virtual void TearDown() {
    delete p;
    delete t;
    delete in;
  }
  
};
  
// At rules
TEST_F(CssParserTest, AtRule) {
  in->str("@import somefile;");
  Stylesheet s;
  AtRule* at;
  
  p->parseStylesheet(s);
  ASSERT_EQ(1, s.getAtRules().size());
  
  at = s.getAtRules().front();
  ASSERT_STREQ("@import", at->getKeyword().c_str());
  ASSERT_STREQ("somefile", at->getRule().toString().c_str());
}

// Rulesets
TEST_F(CssParserTest, Ruleset) {
  in->str("selector {key: value;}");
  
  Stylesheet s;
  Ruleset* set;
  Declaration* d;

  p->parseStylesheet(s);
  ASSERT_EQ(1, s.getRulesets().size());
  set = s.getRulesets().front();
  ASSERT_STREQ("selector", set->getSelector().toString().c_str());
  ASSERT_EQ(1, set->getDeclarations().size());

  d = set->getDeclarations().front();
  ASSERT_STREQ("key", d->getProperty().c_str());
  ASSERT_STREQ("value", d->getValue().toString().c_str());
}

// selector without block
TEST_F(CssParserTest, Selector) {
  in->str("selector ");

  Stylesheet s;

  EXPECT_THROW(p->parseStylesheet(s), ParseException*);
}

// ruleset without block end
TEST_F(CssParserTest, RulesetNoEnding) {
  in->str("selector {key: value");
    
  Stylesheet s;

  EXPECT_THROW(p->parseStylesheet(s), ParseException*);
}

// declaration without value
TEST_F(CssParserTest, DeclarationNoValue) {
  in->str("selector {key: }");
  Stylesheet s;

  EXPECT_THROW(p->parseStylesheet(s), ParseException*);
}

// declaration without colon
TEST_F(CssParserTest, DeclarationNoColon) {
  in->str("selector {key value}");
  Stylesheet s;

  EXPECT_THROW(p->parseStylesheet(s), ParseException*);
}

// @keyword in value
TEST_F(CssParserTest, AtValue) {
  in->str("selector {key: @value}");
    
  Stylesheet s;
  Declaration* d;

  p->parseStylesheet(s);
  ASSERT_EQ(1, s.getRulesets().size());
  ASSERT_EQ(1, s.getRulesets().front()->getDeclarations().size());

  d = s.getRulesets().front()->getDeclarations().front();
  ASSERT_STREQ("key", d->getProperty().c_str());
  ASSERT_STREQ("@value", d->getValue().toString().c_str());
}

// block in value
TEST_F(CssParserTest, BlockValue) {
  in->str("selector {key: {value}}");
    
  Stylesheet s;
  Declaration* d;

  p->parseStylesheet(s);
  ASSERT_EQ(1, s.getRulesets().size());
  ASSERT_EQ(1, s.getRulesets().front()->getDeclarations().size());

  d = s.getRulesets().front()->getDeclarations().front();
  ASSERT_STREQ("key", d->getProperty().c_str());
  ASSERT_STREQ("{value}", d->getValue().toString().c_str());
}
