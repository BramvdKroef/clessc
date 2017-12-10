#include "less/lessstylesheet/LessRuleset.h"
#include "less/lessstylesheet/LessStylesheet.h"
#include "less/lessstylesheet/MediaQueryRuleset.h"
#include "less/LogStream.h"

LessRuleset::LessRuleset() : Ruleset() {
  parent = NULL;
  lessStylesheet = NULL;
  selector = NULL;
}
LessRuleset::LessRuleset(const Selector &selector) : Ruleset() {
  parent = NULL;
  lessStylesheet = NULL;
  setSelector(selector);
}
LessRuleset::~LessRuleset() {
  while (!nestedRules.empty()) {
    delete nestedRules.back();
    nestedRules.pop_back();
  }
  unprocessedStatements.clear();
  if (selector != NULL)
    delete selector;
}

void LessRuleset::setSelector(const Selector &selector) {
  this->selector = new LessSelector(selector);
  Ruleset::setSelector(*this->selector);
}
LessSelector* LessRuleset::getLessSelector() const {
  return selector;
}

UnprocessedStatement* LessRuleset::createUnprocessedStatement() {
  UnprocessedStatement* s = new UnprocessedStatement();
  
  Ruleset::addStatement(*s);
  s->setLessRuleset(*this);
  unprocessedStatements.push_back(s);
  return s;
}

const std::list<UnprocessedStatement*>&
  LessRuleset::getUnprocessedStatements() const {
  return unprocessedStatements;
}

LessRuleset* LessRuleset::createNestedRule() {
  LessRuleset* r = new LessRuleset();
  
  LogStream().notice(2) << "Creating nested rule" ;

  nestedRules.push_back(r);
  r->setParent(this);
  r->setLessStylesheet(*getLessStylesheet());
  return r;
}

MediaQueryRuleset* LessRuleset::createMediaQuery() {
  MediaQueryRuleset* r = new MediaQueryRuleset();
  
  LogStream().notice(2) << "Creating nested media query";

  nestedRules.push_back(r);
  r->setParent(this);
  r->setLessStylesheet(*getLessStylesheet());
  return r;
}

void LessRuleset::deleteNestedRule(LessRuleset &ruleset) {
  nestedRules.remove(&ruleset);
  delete &ruleset;
}

void LessRuleset::deleteUnprocessedStatement(UnprocessedStatement
                                             &statement) {
  unprocessedStatements.remove(&statement);
  deleteStatement(statement);
}

const std::list<LessRuleset*>& LessRuleset::getNestedRules() const {
  return nestedRules;
}

void LessRuleset::putVariable(const std::string &key, const TokenList &value) {
  variables[key] = value;  
}

VariableMap& LessRuleset::getVariables() {
  return variables;
}

const TokenList* LessRuleset::getVariable(const std::string &key) const {
  return variables.getVariable(key);
}
const TokenList* LessRuleset::getInheritedVariable (const std::string &key,
                                                    const MixinCall &stack) const {
  
  const TokenList* t;
  const VariableMap* m;
  
  if (parent != NULL) {
    if ((t = parent->getVariable(key)) != NULL)
      return t;

    if (parent->getLessSelector()->needsArguments()) {
      m = stack.getArguments(*parent);
      if (m != NULL) {
        if ((t = m->getVariable(key)) != NULL)
          return t;
      }
    }
      
    return parent->getInheritedVariable(key, stack);
  } else
    return lessStylesheet->getVariable(key);
}
 
void LessRuleset::setParent(LessRuleset* r) {
  parent = r;
}
LessRuleset* LessRuleset::getParent() const {
  return parent;
}
const list<Closure*>& LessRuleset::getClosures() const {
  return closures;
}

void LessRuleset::setLessStylesheet(LessStylesheet &s) {
  LogStream().notice(3) << "set LessStylesheet";
  lessStylesheet = &s;
}

LessStylesheet* LessRuleset::getLessStylesheet() const {
  return lessStylesheet;
}

ProcessingContext* LessRuleset::getContext() {
  return getLessStylesheet()->getContext();
}

void LessRuleset::processExtensions(ProcessingContext& context,
                                    Selector* prefix) {
  
  std::list<Extension>& e = getLessSelector()->getExtensions();
  std::list<Extension>::iterator e_it;
  Extension extension;

  for (e_it = e.begin(); e_it != e.end(); e_it++) {
    extension = *e_it;
    if (prefix != NULL) 
      extension.getExtension().addPrefix(*prefix);
    context.addExtension(extension);
  }
}

bool LessRuleset::call(Mixin &mixin, Ruleset &target,
                         ProcessingContext &context) const {
  bool ret = false;

  if (putArguments(mixin, *context.getStackArguments()) &&
      matchConditions(context)) {

    processStatements(target, context);

    addClosures(context);
    // process variables and add to context.variables
    context.addVariables(variables);
    
    ret = true;
  }
  return ret;
}

bool LessRuleset::call(Mixin &mixin, Stylesheet &target,
                       ProcessingContext &context) const {
  bool ret = false;

  if (putArguments(mixin, *context.getStackArguments()) &&
      matchConditions(context)) {

    processStatements(target, context);

    addClosures(context);
    // process variables and add to context.variables
    context.addVariables(variables);
    
    ret = true;
  }

  return ret;
}

void LessRuleset::process(Stylesheet &s) {
  process(s, NULL, *getLessStylesheet()->getContext());
}
void LessRuleset::process(Stylesheet &s, Selector* prefix,
                          ProcessingContext& context) {
  Ruleset* target;
  std::list<Closure*> closureScope;

  if (selector->needsArguments())
    return;
  
  target = s.createRuleset();
  target->setSelector(getSelector());

  if (prefix != NULL)
    target->getSelector().addPrefix(*prefix);

  LogStream().notice(2) << "Processing Less Ruleset: " << getSelector().toString();

  context.interpolate(target->getSelector());

  processExtensions(context, prefix);
  context.pushMixinCall(*this, true);
  processStatements(*target, context);
  context.popMixinCall();
  
  saveReturnValues(context);
}

void LessRuleset::processStatements(Ruleset &target,
                                    ProcessingContext& context) const {

  LogStream().notice(2) << "Inserting statements";

  // process statements
  Ruleset::processStatements(target);

  LogStream().notice(2) << "Inserting nested rules";

  // insert nested rules
  insertNestedRules(*target.getStylesheet(), &target.getSelector(), context);
}

void LessRuleset::processStatements(Stylesheet &target,
                                    ProcessingContext& context) const {

  const list<UnprocessedStatement*>& unprocessedStatements =
    getUnprocessedStatements();
  list<UnprocessedStatement*>::const_iterator up_it;

  LogStream().notice(2) << "Inserting statements";

  // insert mixins
  for (up_it = unprocessedStatements.begin();
       up_it != unprocessedStatements.end();
       up_it++) {
    (*up_it)->process(target);
  }

  LogStream().notice(2) << "Inserting nested rules";

  // insert nested rules
  insertNestedRules(target, NULL, context);
}

void LessRuleset::saveReturnValues(ProcessingContext &context) {
  // move closures from context to this->closures
  context.saveClosures(this->closures);
  
  // move variables from context to this->variables
  context.saveVariables(this->variables);
}

void LessRuleset::getFunctions(list<const Function*> &functionList,
                               const Mixin &mixin,
                               TokenList::const_iterator offset)
  const {

  const list<LessRuleset*>& nestedRules = getNestedRules();
  list<LessRuleset*>::const_iterator r_it;
  list<Closure*>::const_iterator c_it;

  offset = mixin.name.walk(getSelector(), offset);
  
  if (offset == mixin.name.begin())
    return;

  LogStream().notice(3) << "Matching mixin " << mixin.name.toString() << " against " << getSelector().toString();

  while (offset != mixin.name.end() &&
         ((*offset).type == Token::WHITESPACE ||
          *offset == ">")) {
    offset++;
  }

  if (!selector->needsArguments() || selector->matchArguments(mixin)) {
    if (offset == mixin.name.end()) {
      functionList.push_back(this);
    } else {   
      for (r_it = nestedRules.begin(); r_it != nestedRules.end(); r_it++) {
        (*r_it)->getFunctions(functionList, mixin, offset);
      }
      for (c_it = closures.begin(); c_it != closures.end(); c_it++) {
        (*c_it)->getFunctions(functionList, mixin, mixin.name.begin());
      }
    }
  }
}

void LessRuleset::getLocalFunctions(std::list<const Function*> &functionList,
                                    const Mixin &mixin) const {
  getLocalFunctions(functionList, mixin, NULL);
}

void LessRuleset::getLocalFunctions(std::list<const Function*> &functionList,
                                    const Mixin &mixin,
                                    const LessRuleset *exclude) const {
  const std::list<LessRuleset*>& nestedRules = getNestedRules();
  std::list<LessRuleset*>::const_iterator r_it;
  std::list<Closure*>::const_iterator c_it;
  
  for (r_it = nestedRules.begin(); r_it != nestedRules.end(); r_it++) {
    if ((*r_it) != exclude) {
      (*r_it)->getFunctions(functionList, mixin, mixin.name.begin());
    }
  }

  for (c_it = closures.begin(); c_it != closures.end(); c_it++) {
    (*c_it)->getFunctions(functionList, mixin, mixin.name.begin());
  }

  if (!functionList.empty())
    return;
  
  if (getParent() != NULL) {
    getParent()->getLocalFunctions(functionList, mixin,
                                   selector->needsArguments() ? NULL : this);
  } else 
    getLessStylesheet()->getFunctions(functionList, mixin);
}


void LessRuleset::insertNestedRules(Stylesheet &s, Selector *prefix,
                                    ProcessingContext &context) const {
  const std::list<LessRuleset*>& nestedRules = getNestedRules();
  std::list<LessRuleset*>::const_iterator r_it;

  for (r_it = nestedRules.begin(); r_it != nestedRules.end(); r_it++) {
    (*r_it)->process(s, prefix, context);
  }
}

void LessRuleset::addClosures(ProcessingContext &context) const {
  const std::list<LessRuleset*>& nestedRules = getNestedRules();
  std::list<LessRuleset*>::const_iterator r_it;
  
  for (r_it = nestedRules.begin(); r_it != nestedRules.end(); r_it++) {
    if ((*r_it)->selector->needsArguments()) {
      context.addClosure(*(*r_it));
    }
  }
}


bool LessRuleset::matchConditions(ProcessingContext &context) const {
  std::list<TokenList>& conditions = selector->getConditions();
  std::list<TokenList>::iterator cit;
  TokenList condition;

  if (conditions.empty())
    return true;

  for(cit = conditions.begin(); cit != conditions.end(); cit++) {
    condition = (*cit);
    
    LogStream().notice(3) << "Checking condition: " << condition.toString();

    if (context.validateCondition(condition)) {
      LogStream().notice(3) << "Found valid condition: " << condition.toString();
      return true;
    }
  }
  return false;
}
  
bool LessRuleset::putArguments(const Mixin &mixin,
                               VariableMap &scope) const {
  std::list<std::string>& parameters = selector->getParameters();
  std::list<std::string>::iterator pit;
  TokenList argsCombined;
  TokenList restVar;
  const TokenList* variable;
  size_t pos = 0;

  // combine with parameter names and add to local scope
  for(pit = parameters.begin(); pit != parameters.end(); pit++) {
    variable = mixin.getArgument(*pit);

    if (variable == NULL) 
      variable = mixin.getArgument(pos++);

    if (variable == NULL)
      variable = selector->getDefault(*pit);

    if (variable == NULL || variable->empty()) 
      return false;
    
    scope.insert(pair<std::string, TokenList>(*pit, *variable));

    argsCombined.insert(argsCombined.end(),
                        variable->begin(), variable->end());
    argsCombined.push_back(Token::BUILTIN_SPACE);
  }

  argsCombined.trim();

  LogStream().notice(3) << "@arguments: " << argsCombined.toString();

  if (selector->unlimitedArguments() &&
      selector->getRestIdentifier() != "") {

    while (pos < mixin.getArgumentCount()) {
      variable = mixin.getArgument(pos++);
      restVar.insert(restVar.end(),
                     variable->begin(), variable->end());
      restVar.push_back(Token::BUILTIN_SPACE);
    }
    
    restVar.trim();
    scope.insert(pair<std::string,TokenList>(selector->getRestIdentifier(), restVar));
  }
  
  scope.insert(pair<std::string,TokenList>("@arguments", argsCombined));
  return true;
}
