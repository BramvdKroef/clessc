#include "../LessParser.h"
#include "gtest/gtest.h"

TEST(LessParserTest, Variable) {
  istringstream in("@x: 10; selector {key: @x}");
    
  LessTokenizer t(&in);
  LessParser p(&t);
  Stylesheet s;
  Declaration* d;
  
  p.parseStylesheet(&s);

  ASSERT_EQ((uint)1, s.getRulesets()->size());
  ASSERT_EQ((uint)1, s.getRulesets()->at(0)->getDeclarations()->size());

  d = s.getRulesets()->at(0)->getDeclarations()->at(0);
  EXPECT_STREQ("key", d->getProperty()->c_str());
  EXPECT_STREQ("10", d->getValue()->toString()->c_str());
}

// mixin
TEST(LessParserTest, Mixin) {
  istringstream in("mixin {key: 10} selector {mixin}");
    
  LessTokenizer t(&in);
  LessParser p(&t);
  Stylesheet s;

  Ruleset* set;
  Declaration* d;
  
  p.parseStylesheet(&s);

  ASSERT_EQ((uint)2, s.getRulesets()->size());
  set = s.getRulesets()->at(1);
  ASSERT_STREQ("selector", set->getSelector()->toString()->c_str());
  
  ASSERT_EQ((uint)1, set->getDeclarations()->size());
  d = set->getDeclarations()->at(0);

  EXPECT_STREQ("key", d->getProperty()->c_str());
  EXPECT_STREQ("10", d->getValue()->toString()->c_str());
}

// nested rule
TEST(LessParserTest, NestedRule) {
  istringstream in("selector { nested {key: value} }");
    
  LessTokenizer t(&in);
  LessParser p(&t);
  Stylesheet s;

  Ruleset* set;
  Declaration* d;
  
  p.parseStylesheet(&s);

  ASSERT_EQ((uint)2, s.getRulesets()->size());

  // Check for 'selector {}'
  set = s.getRulesets()->at(0);
  ASSERT_STREQ("selector", set->getSelector()->toString()->c_str());
  ASSERT_EQ((uint)0, set->getDeclarations()->size());

  // Check for 'selector nested {key: value}'
  set = s.getRulesets()->at(1);
  ASSERT_STREQ("selector nested", set->getSelector()->toString()->c_str());

  ASSERT_EQ((uint)1, set->getDeclarations()->size());
  d = set->getDeclarations()->at(0);

  EXPECT_STREQ("key", d->getProperty()->c_str());
  EXPECT_STREQ("value", d->getValue()->toString()->c_str());
}

// parameter mixin
TEST(LessParserTest, ParameterMixin) {
  istringstream in(".mixin(@x) {key: @x} selector {.mixin(10);}");
    
  LessTokenizer t(&in);
  LessParser p(&t);
  Stylesheet s;

  Ruleset* set;
  Declaration* d;
  
  p.parseStylesheet(&s);

  ASSERT_EQ((uint)1, s.getRulesets()->size());
  set = s.getRulesets()->at(0);
  ASSERT_STREQ("selector", set->getSelector()->toString()->c_str());
  
  ASSERT_EQ((uint)1, set->getDeclarations()->size());
  d = set->getDeclarations()->at(0);

  EXPECT_STREQ("key", d->getProperty()->c_str());
  EXPECT_STREQ("10", d->getValue()->toString()->c_str());
}
