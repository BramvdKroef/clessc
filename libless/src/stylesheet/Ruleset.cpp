#include "less/stylesheet/Ruleset.h"

 
Ruleset::Ruleset() {
}
Ruleset::Ruleset(Selector& selector): selector(&selector) {
}

Ruleset::~Ruleset() {
  if (selector != NULL)
    delete selector;
  clearStatements();
}

void Ruleset::addStatement(RulesetStatement& statement) {
  statements.push_back(&statement);
  statement.setRuleset(this);
}
Declaration* Ruleset::createDeclaration() {
  Declaration* d = new Declaration();
  declarations.push_back(d);
  addStatement(*d);
  return d;
}
Declaration* Ruleset::createDeclaration(const Token& property) {
  Declaration* d = new Declaration(property);
  declarations.push_back(d);
  addStatement(*d);
  return d;
}

CssComment* Ruleset::createComment() {
  CssComment* c = new CssComment();
  addStatement(*c);
  return c;
}

void Ruleset::deleteStatement(RulesetStatement& statement) {
  statements.remove(&statement);
  delete &statement;
}

void Ruleset::deleteDeclaration(Declaration& declaration) {
  declarations.remove(&declaration);
  deleteStatement(declaration);
}

void Ruleset::addDeclarations(std::list<Declaration>& declarations) {
  std::list<Declaration>::iterator i = declarations.begin();
  for (; i != declarations.end(); i++) {
    this->declarations.push_back(&(*i));
    addStatement(*i);
  }
}

void Ruleset::setSelector(Selector &selector) {
  delete this->selector;
  this->selector = &selector;
}

Selector& Ruleset::getSelector() {
  return *selector;
}
const Selector& Ruleset::getSelector() const {
  return *selector;
}
const std::list<RulesetStatement*>& Ruleset::getStatements() const {
  return statements;
}

const std::list<Declaration*>& Ruleset::getDeclarations() const {
  return declarations;
}

void Ruleset::clearStatements() {
  declarations.clear();
  while (!statements.empty()) {
    delete statements.back();
    statements.pop_back();
  }
}

void Ruleset::processStatements(Ruleset& target, void *context) const {
  std::list<RulesetStatement*> statements = getStatements();
  std::list<RulesetStatement*>::iterator i;
  for (i = statements.begin(); i != statements.end(); i++) {
    (*i)->process(target, context);
  }
}

void Ruleset::process(Stylesheet& s, void* context) const {
  Selector* selector = new Selector(getSelector());
  Ruleset* target = s.createRuleset(*selector);

  processStatements(*target, context);
}

void Ruleset::write(CssWriter& writer) const {
  std::list<RulesetStatement*> statements = getStatements();
  std::list<RulesetStatement*>::iterator i;

  if (getStatements().empty())
    return;

  writer.writeRulesetStart(getSelector());

  for (i = statements.begin(); i != statements.end(); i++) {
    if (i != statements.begin())
      writer.writeDeclarationDeliminator();

    (*i)->write(writer);
  }
  writer.writeRulesetEnd();
}
