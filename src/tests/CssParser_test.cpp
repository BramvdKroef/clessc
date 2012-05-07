#include "../CssParser.h"
#include "gtest/gtest.h"

// At rules
TEST(CssParserTest, AtRule) {
  istringstream in("@import somefile;");
    
  CssTokenizer t(&in);
  CssParser p(&t);
  Stylesheet s;
  AtRule* at;

  p.parseStylesheet(&s);
  ASSERT_EQ(1, s.getAtRules()->size());
  
  at = s.getAtRules()->at(0);
  ASSERT_STREQ("@import", at->getKeyword()->c_str());
  ASSERT_STREQ("somefile", at->getRule()->toString()->c_str());
}

// Rulesets
TEST(CssParserTest, Ruleset) {
  istringstream in("selector {key: value;}");
    
  CssTokenizer t(&in);
  CssParser p(&t);
  Stylesheet s;
  Ruleset* set;
  Declaration* d;

  p.parseStylesheet(&s);
  ASSERT_EQ(1, s.getRulesets()->size());
  set = s.getRulesets()->at(0);
  ASSERT_STREQ("selector", set->getSelector()->toString()->c_str());
  ASSERT_EQ(1, set->getDeclarations()->size());

  d = set->getDeclarations()->at(0);
  ASSERT_STREQ("key", d->getProperty()->c_str());
  ASSERT_STREQ("value", d->getValue()->toString()->c_str());
}

// selector without block
TEST(CssParserTest, Selector) {
  istringstream in("selector ");
    
  CssTokenizer t(&in);
  CssParser p(&t);
  Stylesheet s;

  EXPECT_THROW(p.parseStylesheet(&s), ParseException*);
}



