
#ifndef __Stylesheet_h__
#define __Stylesheet_h__

#include "CssWritable.h"
#include "Selector.h"

#include "../Token.h"
#include "../TokenList.h"

#include "../css/CssWriter.h"

#include <list>

class AtRule;
class Ruleset;
class StylesheetStatement;
class CssComment;
class MediaQuery;

class Stylesheet: public CssWritable {
private:
  std::list<AtRule*> atrules;
  std::list<Ruleset*> rulesets;
  std::list<StylesheetStatement*> statements;

protected:
  virtual void addStatement(StylesheetStatement &statement);
  virtual void addRuleset(Ruleset &ruleset);
  virtual void addAtRule(AtRule &rule);
  void deleteStatement(StylesheetStatement &statement);
  
public:
  Stylesheet() {}
  virtual ~Stylesheet();
  
  Ruleset* createRuleset();
  Ruleset* createRuleset(const Selector &selector);

  AtRule* createAtRule(const Token &keyword);
  virtual MediaQuery* createMediaQuery();

  CssComment* createComment();
  
  void deleteRuleset(Ruleset &ruleset);
  void deleteAtRule(AtRule &atrule);
  void deleteMediaQuery(MediaQuery &query);
  
  std::list<AtRule*>& getAtRules();
  std::list<Ruleset*>& getRulesets();
  std::list<StylesheetStatement*>& getStatements();
  
  virtual Ruleset* getRuleset(const Selector& selector);

  virtual void process(Stylesheet &s);
  virtual void write(CssWriter &writer);
};

#include "StylesheetStatement.h"
#include "AtRule.h"
#include "Ruleset.h"
#include "CssComment.h"
#include "MediaQuery.h"

#endif
