#include "../LessParser.h"
#include "gtest/gtest.h"

// At rules
TEST(LessParserTest, AtRule) {
  istringstream in("@x: 10; selector {key: @x}");
    
  LessTokenizer t(&in);
  LessParser p(&t);
  Stylesheet s;
  
  p.parseStylesheet(&s);

  Ruleset* set;
  Declaration* d;

  p.parseStylesheet(&s);

  ASSERT_EQ(1, s.getRulesets()->size());
  ASSERT_EQ(1, s.getRulesets()->at(0)->getDeclarations()->size());

  d = s.getRulesets()->at(0)->getDeclarations()->at(0);
  ASSERT_STREQ("key", d->getProperty()->c_str());
  ASSERT_STREQ("10", d->getValue()->toString()->c_str());
}

