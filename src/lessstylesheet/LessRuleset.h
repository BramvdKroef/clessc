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

#ifndef __LessRuleset_h__
#define __LessRuleset_h__

#include "../stylesheet/Stylesheet.h"
#include "../stylesheet/Ruleset.h"
#include "../stylesheet/Selector.h"

#include "../css/ParseException.h"
#include "../value/ValueProcessor.h"

#include "../Token.h"
#include "../TokenList.h"

#include "UnprocessedStatement.h"
#include "LessSelector.h"
#include "Mixin.h"
#include "ProcessingContext.h"

#include <map>
#include <list>
#include <string>

class LessStylesheet;
class MediaQueryRuleset;

class LessRuleset: public Ruleset, Function {
  
protected:
  map<string, TokenList> variables;  
  list<LessRuleset*> nestedRules;
  list<UnprocessedStatement*> unprocessedStatements;

  LessRuleset* parent;
  LessStylesheet* lessStylesheet;
  LessSelector* selector;

  ProcessingContext* context;

  void processVariables();
  void insertNestedRules(Stylesheet &s, Selector* prefix,
                         ProcessingContext &context) const;

  void addClosures(ProcessingContext &context) const;
  
public:
  LessRuleset();
  LessRuleset(const Selector &selector);
  virtual ~LessRuleset();

  virtual void setSelector(const Selector &selector);
  virtual LessSelector* getLessSelector() const;

  UnprocessedStatement* createUnprocessedStatement();
  LessRuleset* createNestedRule();
  MediaQueryRuleset* createMediaQuery();

  void deleteNestedRule(LessRuleset &ruleset);
  void deleteUnprocessedStatement(UnprocessedStatement
                                  &statement);
  
  const list<UnprocessedStatement*>& getUnprocessedStatements() const;
  const list<LessRuleset*>& getNestedRules() const;

  void putVariable(const std::string &key, const TokenList &value);
  map<string, TokenList>& getVariables();

  void setParent(LessRuleset* r);
  LessRuleset* getParent() const;

  void setLessStylesheet(LessStylesheet &stylesheet);
  LessStylesheet* getLessStylesheet() const;

  ProcessingContext* getContext();
  
  void processExtensions(ProcessingContext &context,
                         Selector* prefix);
  
  virtual bool insert(Mixin* mixin, Ruleset &target,
                      ProcessingContext& context) const;
  virtual bool insert(Mixin* mixin, Stylesheet &s,
                      ProcessingContext& context) const;

  virtual void process(Stylesheet &s);
  virtual void process(Stylesheet &s, Selector* prefix,
                       ProcessingContext &context);

  virtual void getFunctions(list<const Function*> &functionList,
                            const Mixin &mixin,
                            TokenList::const_iterator selector_offset) const;
  /**
   * Look for a ruleset inside this ruleset and scope up to
   * getParent(), or getLessStylesheet() if getParent() is NULL.
   */
  void getLocalFunctions(list<const Function*> &functionList,
                         const Mixin &mixin) const;

  bool matchConditions(ProcessingContext &context) const;
  bool putArguments(const Mixin &mixin,
                    std::map<std::string, TokenList> &scope) const;
};

#endif
