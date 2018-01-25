#ifndef __less_stylesheet_Stylesheet_h__
#define __less_stylesheet_Stylesheet_h__

#include <list>

#include "less/stylesheet/CssWritable.h"
#include "less/stylesheet/Selector.h"

#include "less/Token.h"
#include "less/TokenList.h"

#include "less/css/CssWriter.h"

class AtRule;
class Ruleset;
class StylesheetStatement;
class CssComment;
class MediaQuery;

class Stylesheet : public CssWritable {
private:
  std::list<AtRule *> atrules;
  std::list<Ruleset *> rulesets;
  std::list<StylesheetStatement *> statements;

protected:
  virtual void addStatement(StylesheetStatement &statement);
  virtual void addRuleset(Ruleset &ruleset);
  virtual void addAtRule(AtRule &rule);
  void deleteStatement(StylesheetStatement &statement);

public:
  Stylesheet() {
  }
  virtual ~Stylesheet();

  Ruleset *createRuleset();
  Ruleset *createRuleset(const Selector &selector);

  AtRule *createAtRule(const Token &keyword);
  virtual MediaQuery *createMediaQuery();

  CssComment *createComment();

  void deleteRuleset(Ruleset &ruleset);
  void deleteAtRule(AtRule &atrule);
  void deleteMediaQuery(MediaQuery &query);

  const std::list<AtRule *> &getAtRules() const;
  const std::list<Ruleset *> &getRulesets() const;
  const std::list<StylesheetStatement *> &getStatements() const;

  virtual Ruleset *getRuleset(const Selector &selector) const;

  virtual void process(Stylesheet &s, void* context) const;
  virtual void write(CssWriter &writer) const;
};

#include "less/stylesheet/AtRule.h"
#include "less/stylesheet/CssComment.h"
#include "less/stylesheet/MediaQuery.h"
#include "less/stylesheet/Ruleset.h"
#include "less/stylesheet/StylesheetStatement.h"

#endif  // __less_stylesheet_Stylesheet_h__
