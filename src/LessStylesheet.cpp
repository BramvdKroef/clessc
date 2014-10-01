#include "LessStylesheet.h"

#include <config.h>

#ifdef WITH_LIBGLOG
#include <glog/logging.h>
#endif

LessStylesheet::LessStylesheet() {
}

LessStylesheet::~LessStylesheet() {
  while (!parameterRulesets.empty()) {
    delete parameterRulesets.back();
    parameterRulesets.pop_back();
  }
}

void LessStylesheet::addStatement(LessRuleset* ruleset) {
  lessrulesets.push_back(ruleset);
}


void LessStylesheet::process() {
  vector<StylesheetStatement*>* statements = getStatements();
  vector<StylesheetStatement*>::iterator it;
  LessRuleset* ruleset;
  
  DLOG(INFO) << "Processing start";
    
  for (it = statements->begin(); it != statements->end(); it++) {
    switch ((*it)->getType()) {
    case LessRuleset::RULESET:
      ruleset = (LessRuleset*)*it;

      valueProcessor.interpolateTokenList(ruleset->getSelector());
      
      processRuleset(ruleset);  
      break;
      
    case ParameterMixin::MIXIN:
      processParameterMixin((ParameterMixin*)*it, NULL);
      break;
    }
  }
  DLOG(INFO) << "Processing end";
}

void LessStylesheet::processRuleset(LessRuleset* ruleset) {
  Ruleset target;
  map<string, TokenList*> scope;

  if (ruleset->isProcessed())
    return;
  
  ruleset->setProcessed(true);
  target.setSelector(ruleset->getSelector());
  
  valueProcessor.pushScope(&scope);

  DLOG(INFO) << "Ruleset: " << *ruleset->getSelector()->toString();

  // variables first
  processVariables(ruleset->getVariables());

  // then statements
  processStatements(ruleset->getStatements(), &target);

  // then nested rules
  insertNestedRules(ruleset, ruleset->getSelector());
  
  valueProcessor.popScope();

  ruleset->swap(&target);
  target.clearStatements();
  target.setSelector(NULL);

  DLOG(INFO) << "Ruleset done";
}

void LessStylesheet::processVariables(map<string, TokenList*>* variables) {
  map<string, TokenList*>::iterator it;
  for (it = variables->begin(); it != variables->end(); ++it) {
    valueProcessor.putVariable(it->first, it->second);
  }
}

/**
 * process declarations and mixins
 */
void LessStylesheet::processStatements(vector<RulesetStatement*>*
                                       statements, Ruleset* target) {
  vector<RulesetStatement*>::iterator i;
  UnprocessedStatement* statement;
  
  for (i = statements->begin(); i != statements->end(); ++i) {
    statement = (UnprocessedStatement*)*i;
    if ((*i)->getType() == UnprocessedStatement::UNPROCESSED) {

      processStatement(statement, target);
      DLOG(INFO) << "Statement processed";
    } else {
      DLOG(INFO) << "Copy statement";
      target->addStatement(statement->clone());
    }
  }

}

void LessStylesheet::processStatement(UnprocessedStatement* statement,
                                      Ruleset* target) {
  ParameterMixin mixin;
  Declaration declaration;
  
  // change type to avoid any circular mixins
  statement->type = UnprocessedStatement::PROCESSED;
  DLOG(INFO) << "Statement: " << *statement->getTokens()->toString();
  
  // process mixin
  if (mixin.parse(statement->getTokens()) &&
             processParameterMixin(&mixin, target)) {

  } else if (statement->processDeclaration(&declaration)) {
    DLOG(INFO) << "Declaration: " <<
      *declaration.getProperty() << ": " << *declaration.getValue()->toString();
    valueProcessor.processValue(declaration.getValue());
    target->addStatement(declaration.clone());

    DLOG(INFO) << "Processed declaration: " <<
      *declaration.getProperty() << ": " << *declaration.getValue()->toString();

  } else {
    throw new ParseException(*statement->getTokens()->toString(),
                             "variable, mixin or declaration.",
                             statement->line, statement->column);
  }
  statement->type = UnprocessedStatement::UNPROCESSED;
  DLOG(INFO) << "Statement done";
}

void LessStylesheet::addParameterRuleset(ParameterRuleset* rule) {
  DLOG(INFO) << "Adding parameter ruleset: " <<
    *rule->getSelector()->toString();

  parameterRulesets.push_back(rule);
}

bool LessStylesheet::hasParameterRuleset(ParameterMixin* mixin) {
  vector<ParameterRuleset*>::iterator pri;
  
  for (pri = parameterRulesets.begin(); pri != parameterRulesets.end();
       pri++) {
    
    if ((*pri)->getSelector()->equals(mixin->name) &&
        (*pri)->matchArguments(mixin->arguments)) {
      return true;
    }
  }
  return false;
}


bool LessStylesheet::processParameterMixin(ParameterMixin* mixin,
                                           Ruleset* target) {
  list<TokenList*>::iterator arg_i;
  vector<ParameterRuleset*>::iterator pri;
  ParameterRuleset* pruleset;

  LessRuleset* mixinRuleset;

  bool ret = false;

  DLOG(INFO) << "Mixin: " << *mixin->name->toString();

  for (arg_i = mixin->arguments->begin();
       arg_i != mixin->arguments->end(); arg_i++) {
    valueProcessor.processValue(*arg_i);
    DLOG(INFO) << "Mixin Arg: " << *(*arg_i)->toString();
  }

  if (target != NULL &&
      (mixinRuleset = dynamic_cast<LessRuleset*>(getRuleset(mixin->name))) != NULL) {

    DLOG(INFO) << "Mixin Ruleset: " << *mixinRuleset->getSelector()->toString();
    
    // make sure it's processed
    processRuleset(mixinRuleset);

    target->addDeclarations(mixinRuleset->cloneDeclarations());
    ret = true;
  }
  
  for (pri = parameterRulesets.begin(); pri != parameterRulesets.end();
       pri++) {
    pruleset = *pri;
    
    if (pruleset->getSelector()->equals(mixin->name) &&
        pruleset->matchArguments(mixin->arguments)) {

      DLOG(INFO) << "Mixin: " << *mixin->name->toString();
      
      ret = insertParameterRuleset(pruleset, mixin->arguments,
                                   target) || ret;
    }
  }
  
  return ret;
}

bool LessStylesheet::insertParameterRuleset(ParameterRuleset* pruleset,
                                            list<TokenList*>* arguments,
                                            Ruleset* target) {
  map<string, TokenList*> scope;
  bool ret = false;
  Selector* prefix = NULL;
  
  // new scope to accept arguments
  valueProcessor.pushScope(&scope);

  if (pruleset->putArguments(&valueProcessor, arguments) &&
      pruleset->matchConditions(&valueProcessor)) {

    // variables first
    processVariables(pruleset->getVariables());

    // if this ruleset was called inside a ruleset:
    if (target != NULL) {
      // process the statements
      processStatements(pruleset->getStatements(), target);
      
      // add prefix to nested rules
      prefix = target->getSelector();
    }
    
    // nested rules
    insertNestedRules(pruleset, prefix);
    ret = true;
  }
      
  valueProcessor.popScope();

  return ret;
}

void LessStylesheet::insertNestedRules(LessRuleset* parent, Selector* prefix) {
  list<Ruleset*>* nestedRules = parent->getNestedRules();
  list<Ruleset*>::iterator r_it;
  Ruleset* nestedRule;

  for (r_it = nestedRules->begin(); r_it != nestedRules->end(); r_it++) {
    nestedRule = (*r_it)->clone();

    if (prefix != NULL)
      nestedRule->getSelector()->addPrefix(prefix);

    valueProcessor.interpolateTokenList(nestedRule->getSelector());

    processRuleset((LessRuleset*)nestedRule);
    addStatement(nestedRule);
  }
}

void LessStylesheet::putVariable(string key, TokenList* value) {
  DLOG(INFO) << "Variable: " << key << ": " << *value->toString();
  valueProcessor.putVariable(key, value);
}
