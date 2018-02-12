#ifndef __less_stylesheet_Ruleset_h__
#define __less_stylesheet_Ruleset_h__

#include "less/stylesheet/CssComment.h"
#include "less/stylesheet/Selector.h"
#include "less/stylesheet/Stylesheet.h"
#include "less/stylesheet/StylesheetStatement.h"

#include "less/Token.h"
#include "less/TokenList.h"

#include "less/css/CssWriter.h"

class RulesetStatement;
class Declaration;

class Ruleset : public StylesheetStatement {
private:
  std::list<RulesetStatement *> statements;
  std::list<Declaration *> declarations;

protected:
  Selector* selector;
  virtual void addStatement(RulesetStatement &statement);
  virtual void deleteStatement(RulesetStatement &statement);

public:
  Ruleset();
  Ruleset(Selector &selector);
  virtual ~Ruleset();

  Declaration *createDeclaration();
  Declaration *createDeclaration(const Token &property);
  CssComment *createComment();

  void deleteDeclaration(Declaration &declaration);

  void addDeclarations(std::list<Declaration> &declarations);

  Selector &getSelector();
  const Selector &getSelector() const;
  void setSelector(Selector &selector);
  const std::list<RulesetStatement *> &getStatements() const;
  const std::list<Declaration *> &getDeclarations() const;

  void clearStatements();

  virtual void processStatements(Ruleset &target, void* context) const;
  virtual void process(Stylesheet &s, void* context) const;
  virtual void write(CssWriter &writer) const;
};

#include "less/stylesheet/Declaration.h"
#include "less/stylesheet/RulesetStatement.h"

#endif  // __less_stylesheet_Ruleset_h__
