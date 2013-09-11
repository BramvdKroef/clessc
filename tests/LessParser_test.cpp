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

#include "LessParser.h"
#include "gtest/gtest.h"

TEST(LessParserTest, Variable) {
  istringstream in("@x: 10; selector {key: @x}");

  ValueProcessor vp;
  ParameterRulesetLibrary pr(&vp);

  LessTokenizer t(&in);
  LessParser p(&t, &pr, &vp);
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

  ValueProcessor vp;
  ParameterRulesetLibrary pr(&vp);

  LessTokenizer t(&in);
  LessParser p(&t, &pr, &vp);
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

  ValueProcessor vp;
  ParameterRulesetLibrary pr(&vp);

  LessTokenizer t(&in);
  LessParser p(&t, &pr, &vp);
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

  ValueProcessor vp;
  ParameterRulesetLibrary pr(&vp);

  LessTokenizer t(&in);
  LessParser p(&t, &pr, &vp);
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
