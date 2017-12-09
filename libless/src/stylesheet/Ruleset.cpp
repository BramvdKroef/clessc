#include <less/stylesheet/Ruleset.h>

Ruleset::Ruleset() {
}
Ruleset::Ruleset(const Selector &selector){
  this->selector = selector;
}

Ruleset::~Ruleset() {
  clearStatements();
}

void Ruleset::setSelector (const Selector &selector) {
  this->selector = selector;
}
void Ruleset::addStatement (RulesetStatement &statement) {
  statements.push_back(&statement);
  statement.setRuleset(this);
}
Declaration* Ruleset::createDeclaration() {
  Declaration* d = new Declaration();
  declarations.push_back(d);
  addStatement(*d);
  return d;
}
Declaration* Ruleset::createDeclaration(const Token &property) {
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

void Ruleset::deleteStatement(RulesetStatement &statement) {
  statements.remove(&statement);
  delete &statement;
}

void Ruleset::deleteDeclaration(Declaration &declaration) {
  declarations.remove(&declaration);
  deleteStatement(declaration);
}

void Ruleset::addDeclarations (std::list<Declaration> &declarations) {
  std::list<Declaration>::iterator i = declarations.begin();
  for (; i != declarations.end(); i++) {
    this->declarations.push_back(&(*i));
    addStatement(*i);
  }
}

Selector& Ruleset::getSelector() {
  return selector;
}
const Selector& Ruleset::getSelector() const {
  return selector;
}
const std::list<RulesetStatement*>& Ruleset::getStatements() const {
  return statements;
}

std::list<Declaration*>& Ruleset::getDeclarations() {
  return declarations;
}

void Ruleset::clearStatements() {
  declarations.clear();
  while(!statements.empty()) {
    delete statements.back();
    statements.pop_back();
  }
}

void Ruleset::processStatements(Ruleset &target) const {
  std::list<RulesetStatement*> statements = getStatements();
  std::list<RulesetStatement*>::iterator i;
  for (i = statements.begin(); i != statements.end(); i++) {
    (*i)->process(target);
  }
}

void Ruleset::process(Stylesheet &s) {
  Ruleset* target = s.createRuleset();
    
#ifdef WITH_LIBGLOG
  VLOG(2) << "Processing Ruleset: " << getSelector().toString();
#endif
        
  target->setSelector(getSelector());
  processStatements(*target);
}

void Ruleset::write(CssWriter &writer) {
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
