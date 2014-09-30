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

void LessStylesheet::process() {
  vector<StylesheetStatement*>* statements = getStatements();
  vector<StylesheetStatement*>::iterator it;
  Ruleset* ruleset, target;
  
  DLOG(INFO) << "Processing start";
    
  for (it = statements->begin(); it != statements->end(); it++) {
    switch ((*it)->getType()) {
    case StylesheetStatement::RULESET:
      ruleset = (Ruleset*)*it;

      valueProcessor.interpolateTokenList(ruleset->getSelector());
      
      target.setSelector(ruleset->getSelector());
      processRuleset(ruleset, &target);

      ruleset->swap(&target);
      target.clearStatements();
      target.setSelector(NULL);
      
      break;
      
    case ParameterMixin::MIXIN:
      processParameterMixin((ParameterMixin*)*it, NULL);
      break;
    }
  }
  DLOG(INFO) << "Processing end";
}

void LessStylesheet::processRuleset(Ruleset* ruleset,
                                    Ruleset* target) {
  
  vector<RulesetStatement*>* statements = ruleset->getStatements();
  vector<RulesetStatement*>::iterator i;
  map<string, TokenList*> scope;
  UnprocessedStatement* statement;
  
  valueProcessor.pushScope(&scope);
  DLOG(INFO) << "Ruleset: " << *ruleset->getSelector()->toString();
    
  for (i = statements->begin(); i != statements->end(); ++i) {
    if ((*i)->getType() == UnprocessedStatement::UNPROCESSED) {

      statement = (UnprocessedStatement*)*i;
      processStatement(statement, target);
    }
  }
  valueProcessor.popScope();
  DLOG(INFO) << "Ruleset done";
}

void LessStylesheet::processStatement(UnprocessedStatement* statement,
                                      Ruleset* target) {
  ParameterMixin mixin;
  Declaration declaration;
  
  // change type to avoid any circular mixins
  statement->type = UnprocessedStatement::PROCESSED;
  DLOG(INFO) << "Statement: " << *statement->getTokens()->toString();
  
  // process mixin
  if (statement->processVariable(&valueProcessor)) {
    
  } else if (mixin.parse(statement->getTokens()) &&
             processParameterMixin(&mixin, target)) {

  } else if (statement->processDeclaration(&declaration)) {
    valueProcessor.processValue(declaration.getValue());
    target->addStatement(declaration.clone());

    DLOG(INFO) << "Processed declaration: " <<
      *declaration.getProperty() << ": " << *declaration.getValue()->toString();

  } else {
    throw new ParseException(*statement->getTokens()->toString(),
                             "variable, mixin or declaration.",
                             statement->line, statement->column);
  }
  DLOG(INFO) << "Statement done";
  statement->type = UnprocessedStatement::UNPROCESSED;
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

  Ruleset* mixinRuleset;

  bool ret = false;

    
  for (arg_i = mixin->arguments->begin();
       arg_i != mixin->arguments->end(); arg_i++) {
    valueProcessor.processValue(*arg_i);
  }

  if (target != NULL && (mixinRuleset = getRuleset(mixin->name)) != NULL) {
    DLOG(INFO) << "Mixin: " << *mixin->name->toString();
    processRuleset(mixinRuleset, target);
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
  list<Ruleset*>* nestedRules;
  list<Ruleset*>::iterator r_it;
  Ruleset* nestedRule;
  
  map<string, TokenList*> scope;
  
  bool ret = false;
  
  // new scope to accept arguments
  valueProcessor.pushScope(&scope);

  if (pruleset->putArguments(&valueProcessor, arguments) &&
      pruleset->matchConditions(&valueProcessor)) {

    // process the statements
    if (target != NULL) {
      processRuleset(pruleset, target);
    }
    
    // nested rules
    nestedRules = pruleset->getNestedRules();
    for (r_it = nestedRules->begin(); r_it != nestedRules->end(); r_it++) {
      nestedRule = (*r_it)->clone();
      processRuleset(nestedRule, nestedRule);

      if (target != NULL)
        nestedRule->getSelector()->addPrefix(target->getSelector());

      valueProcessor.interpolateTokenList(nestedRule->getSelector());
      addStatement(nestedRule);
    }

    ret = true;
  }
      
  valueProcessor.popScope();

  return ret;
}

void LessStylesheet::putVariable(string key, TokenList* value) {
  DLOG(INFO) << "Variable: " << key << ": " << *value->toString();
  valueProcessor.putVariable(key, value);
}
