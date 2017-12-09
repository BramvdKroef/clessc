
#ifndef __Ruleset_h__
#define __Ruleset_h__

#include "Stylesheet.h"
#include "StylesheetStatement.h"
#include "Selector.h"
#include "CssComment.h"

#include "../Token.h"
#include "../TokenList.h"

#include "../css/CssWriter.h"

class RulesetStatement;
class Declaration;

class Ruleset: public StylesheetStatement {
private:
  std::list<RulesetStatement*> statements;
  std::list<Declaration*> declarations;

protected:
  Selector selector;
  virtual void addStatement(RulesetStatement &statement);
  virtual void deleteStatement(RulesetStatement &statement);
  
public:
  Ruleset ();
  Ruleset(const Selector &selector);
  virtual ~Ruleset();
  virtual void setSelector (const Selector &selector);

  Declaration* createDeclaration();
  Declaration* createDeclaration(const Token &property);
  CssComment* createComment();
  
  void deleteDeclaration(Declaration &declaration);
  
  void addDeclarations (std::list<Declaration> &declarations);

  Selector& getSelector();
  const Selector& getSelector() const;
  const std::list<RulesetStatement*>& getStatements() const;
  std::list<Declaration*>& getDeclarations();

  void clearStatements();
  
  virtual void processStatements(Ruleset &target) const;
  virtual void process(Stylesheet& s);
  virtual void write(CssWriter &writer);
};

#include "RulesetStatement.h"
#include "Declaration.h"

#endif
