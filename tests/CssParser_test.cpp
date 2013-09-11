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

#include "CssParser.h"
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

// ruleset without block end
TEST(CssParserTest, RulesetNoEnding) {
  istringstream in("selector {key: value");
    
  CssTokenizer t(&in);
  CssParser p(&t);
  Stylesheet s;

  EXPECT_THROW(p.parseStylesheet(&s), ParseException*);
}

// declaration without value
TEST(CssParserTest, DeclarationNoValue) {
  istringstream in("selector {key: }");
    
  CssTokenizer t(&in);
  CssParser p(&t);
  Stylesheet s;

  EXPECT_THROW(p.parseStylesheet(&s), ParseException*);
}

// declaration without colon
TEST(CssParserTest, DeclarationNoColon) {
  istringstream in("selector {key value}");
    
  CssTokenizer t(&in);
  CssParser p(&t);
  Stylesheet s;

  EXPECT_THROW(p.parseStylesheet(&s), ParseException*);
}

// @keyword in value
TEST(CssParserTest, AtValue) {
  istringstream in("selector {key: @value}");
    
  CssTokenizer t(&in);
  CssParser p(&t);
  Stylesheet s;
  Declaration* d;

  p.parseStylesheet(&s);
  ASSERT_EQ(1, s.getRulesets()->size());
  ASSERT_EQ(1, s.getRulesets()->at(0)->getDeclarations()->size());

  d = s.getRulesets()->at(0)->getDeclarations()->at(0);
  ASSERT_STREQ("key", d->getProperty()->c_str());
  ASSERT_STREQ("@value", d->getValue()->toString()->c_str());
}

// block in value
TEST(CssParserTest, BlockValue) {
  istringstream in("selector {key: {value}}");
    
  CssTokenizer t(&in);
  CssParser p(&t);
  Stylesheet s;
  Declaration* d;

  p.parseStylesheet(&s);
  ASSERT_EQ(1, s.getRulesets()->size());
  ASSERT_EQ(1, s.getRulesets()->at(0)->getDeclarations()->size());

  d = s.getRulesets()->at(0)->getDeclarations()->at(0);
  ASSERT_STREQ("key", d->getProperty()->c_str());
  ASSERT_STREQ("{value}", d->getValue()->toString()->c_str());
}


