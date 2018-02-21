#include "less/lessstylesheet/LessRuleset.h"
#include "less/lessstylesheet/LessStylesheet.h"
#include "less/lessstylesheet/MediaQueryRuleset.h"

LessRuleset::LessRuleset(LessSelector& selector,
                         const LessRuleset& parent) :
  Ruleset(selector),
  parent(&parent), lessStylesheet(NULL), selector(&selector) {

}
LessRuleset::LessRuleset(LessSelector& selector,
                         const LessStylesheet& parent) :
  Ruleset(selector),
  parent(NULL), lessStylesheet(&parent), selector(&selector) {
}

LessRuleset::~LessRuleset() {
  while (!nestedRules.empty()) {
    delete nestedRules.back();
    nestedRules.pop_back();
  }
}

const LessSelector& LessRuleset::getLessSelector() const {
  return *selector;
}

void LessRuleset::addExtension(Extension &extension) {
  extensions.push_back(extension);
}

LessDeclaration* LessRuleset::createLessDeclaration() {
  LessDeclaration* d = new LessDeclaration();

  Ruleset::addStatement(*d);
  d->setLessRuleset(*this);
  lessDeclarations.push_back(d);
  return d;
}

const std::list<LessDeclaration*>& LessRuleset::getLessDeclarations()
  const {
  return lessDeclarations;
}

Mixin* LessRuleset::createMixin(const TokenList &selector) {
  Mixin* m = new Mixin(selector, *this);

  Ruleset::addStatement(*m);
  mixins.push_back(m);
  stylesheetStatements.push_back(m);
  return m;
}

const std::list<Mixin*>& LessRuleset::getMixins()
  const {
  return mixins;
}

LessAtRule* LessRuleset::createLessAtRule(const Token& keyword) {
  LessAtRule* r = new LessAtRule(keyword);

  Ruleset::addStatement(*r);
  lessAtRules.push_back(r);
  stylesheetStatements.push_back(r);
  return r;
}

const std::list<LessAtRule*>& LessRuleset::getLessAtRules()
  const {
  return lessAtRules;
}

const std::list<StylesheetStatement*>& LessRuleset::getStylesheetStatements()
  const {
  return stylesheetStatements;
}

LessRuleset* LessRuleset::createNestedRule(LessSelector& selector) {
  LessRuleset* r = new LessRuleset(selector, *this);

  nestedRules.push_back(r);
  return r;
}

MediaQueryRuleset* LessRuleset::createMediaQuery(TokenList &selector) {
  MediaQueryRuleset* r = new MediaQueryRuleset(selector, *this);

  nestedRules.push_back(r);
  return r;
}

void LessRuleset::deleteNestedRule(LessRuleset& ruleset) {
  nestedRules.remove(&ruleset);
  delete &ruleset;
}

const std::list<LessRuleset*>& LessRuleset::getNestedRules() const {
  return nestedRules;
}

void LessRuleset::putVariable(const std::string& key, const TokenList& value) {
  variables[key] = value;
}

VariableMap& LessRuleset::getVariables() {
  return variables;
}

const TokenList* LessRuleset::getVariable(const std::string& key) const {
  return variables.getVariable(key);
}

void LessRuleset::setParent(const LessRuleset* r) {
  parent = r;
}
const LessRuleset* LessRuleset::getParent() const {
  return parent;
}

void LessRuleset::setLessStylesheet(const LessStylesheet& s) {
  lessStylesheet = &s;
}

const LessStylesheet* LessRuleset::getLessStylesheet() const {
  if (lessStylesheet != NULL)
    return lessStylesheet;
  else if (parent != NULL)
    return parent->getLessStylesheet();
  else
    return NULL;
}

void LessRuleset::processExtensions(ProcessingContext& context,
                                    const Selector* prefix) const {
  const std::list<Extension>& e = getLessSelector().getExtensions();
  std::list<Extension>::const_iterator e_it;
  Extension extension;

  for (e_it = e.begin(); e_it != e.end(); e_it++) {
    extension = *e_it;
    if (prefix != NULL)
      extension.getExtension().addPrefix(*prefix);

    context.interpolate(extension.getExtension());
    
    context.addExtension(extension);
  }
}

void LessRuleset::processInlineExtensions(ProcessingContext& context,
                                          const Selector &selector) const {
  std::list<Extension>::const_iterator e_it;
  Extension extension;

  for (e_it = extensions.begin(); e_it != extensions.end(); e_it++) {
    extension = *e_it;

    extension.setExtension(selector);
    
    context.addExtension(extension);
  }
}

bool LessRuleset::call(MixinArguments& args,
                       Ruleset& target,
                       ProcessingContext& context,
                       bool defaultVal) const {
  if (call(args, context, &target, NULL, defaultVal)) {
    processInlineExtensions(context, target.getSelector());
    return true;
  } else
    return false;
}

bool LessRuleset::call(MixinArguments& args,
                       Stylesheet& target,
                       ProcessingContext& context,
                       bool defaultVal) const {
  return call(args, context, NULL, &target, defaultVal);
}

bool LessRuleset::call(MixinArguments& args,
                       ProcessingContext& context,
                       Ruleset* ruleset,
                       Stylesheet* stylesheet,
                       bool defaultVal) const {

  if (putArguments(args, *context.getStackArguments()) &&
      matchConditions(context, defaultVal)) {
    
    if (ruleset != NULL)
      processStatements(*ruleset, &context);
    else 
      processStatements(*stylesheet, &context);

    addClosures(context);
    // process variables and add to context.variables
    if (context.getStackArguments() != NULL)
      context.addVariables(*context.getStackArguments());
    context.addVariables(variables);

    return true;
  } else
    return false;
}

void LessRuleset::process(Stylesheet& s, void* context) const {
  process(s, NULL, *((ProcessingContext*)context));
}
void LessRuleset::process(Stylesheet& s,
                          const Selector* prefix,
                          ProcessingContext& context) const {
  Ruleset* target;
  Selector* selector;

  if (getLessSelector().needsArguments())
    return;

  if (!matchConditions(context))
    return;

  selector = new Selector(getSelector());
  if (prefix != NULL)
    selector->addPrefix(*prefix);

  context.interpolate(*selector);
  
  target = s.createRuleset(*selector);

  processExtensions(context, prefix);
  processInlineExtensions(context, target->getSelector());
  context.pushMixinCall(*this, true);
  processStatements(*target, &context);
  context.popMixinCall();

  mergeDeclarations(*target);
}

void LessRuleset::processStatements(Ruleset& target,
                                    void* context) const {
  // process statements
  Ruleset::processStatements(target, context);
  
  // insert nested rules
  insertNestedRules(*target.getStylesheet(), &target.getSelector(),
                    *(ProcessingContext*)context);
}

void LessRuleset::processStatements(Stylesheet& target,
                                    void* context) const {
  const std::list<StylesheetStatement*>& stylesheetStatements =
    getStylesheetStatements();
  std::list<StylesheetStatement*>::const_iterator it;

  // insert mixins
  for (it = stylesheetStatements.begin();
       it != stylesheetStatements.end();
       it++) {
    (*it)->process(target, context);
  }

  // insert nested rules
  insertNestedRules(target, NULL, *(ProcessingContext*)context);
}

const TokenList* LessRuleset::getVariable(const std::string& key,
                                          const ProcessingContext &context) const {
  const TokenList* t;
  const VariableMap* m;

  if ((t = getVariable(key)) != NULL)
    return t;

  if ((m = context.getStackArguments(this)) != NULL) {
    if ((t = m->getVariable(key)) != NULL)
      return t;
  }
  
  if ((t = context.getFunctionVariable(key, this)) != NULL)
    return t;

  if (parent != NULL)
    return parent->getVariable(key, context);
  else
    return getLessStylesheet()->getVariable(key, context);
}

void LessRuleset::getFunctions(list<const Function*>& functionList,
                               const Mixin& mixin,
                               TokenList::const_iterator offset,
                               const ProcessingContext &context) const {
  std::list<LessRuleset*>::const_iterator r_it;
  const std::list<Closure*>* closures;
  std::list<Closure*>::const_iterator c_it;
  TokenList::const_iterator offset2;
  
  offset2 = getSelector().walk(offset, mixin.name.end());

  if (offset2 == offset)
    return;

  while (offset2 != mixin.name.end() &&
         ((*offset2).type == Token::WHITESPACE || *offset2 == ">")) {
    offset2++;
  }

  if (offset2 == mixin.name.end()) {
    if (selector->matchArguments(mixin.arguments)) {
      functionList.push_back(this);
    }
  } else {
    if (!selector->needsArguments() && matchConditions(context)) {
      
      for (r_it = nestedRules.begin(); r_it != nestedRules.end(); r_it++) {
        (*r_it)->getFunctions(functionList, mixin, offset2, context);
      }
      closures = context.getClosures(this);
      if (closures != NULL) {
        for (c_it = closures->begin(); c_it != closures->end(); c_it++) {
          (*c_it)->getFunctions(functionList, mixin, offset2, context);
        }
      }
    }
  }
}

void LessRuleset::getLocalFunctions(std::list<const Function*>& functionList,
                                    const Mixin& mixin,
                                    const ProcessingContext &context) const {
  getLocalFunctions(functionList, mixin, NULL, context);
}

void LessRuleset::getLocalFunctions(std::list<const Function*>& functionList,
                                    const Mixin& mixin,
                                    const LessRuleset* exclude,
                                    const ProcessingContext &context) const {
  std::list<LessRuleset*>::const_iterator r_it;
  const std::list<Closure*>* closures;
  std::list<Closure*>::const_iterator c_it;

  for (r_it = nestedRules.begin(); r_it != nestedRules.end(); r_it++) {
    if ((*r_it) != exclude) {
      (*r_it)->getFunctions(functionList, mixin, mixin.name.begin(), context);
    }
  }
  
  closures = context.getClosures(this);
  if (closures != NULL) {
    for (c_it = closures->begin(); c_it != closures->end(); c_it++) {
      (*c_it)->getFunctions(functionList, mixin, mixin.name.begin(), context);
    }
  }
  
  if (!functionList.empty())
    return;

  if (getParent() != NULL) {
    getParent()->getLocalFunctions(functionList,
                                   mixin,
                                   selector->needsArguments() ? NULL : this,
                                   context);
  } else
    getLessStylesheet()->getFunctions(functionList, mixin, context);
}

void LessRuleset::insertNestedRules(Stylesheet& s,
                                    const Selector* prefix,
                                    ProcessingContext& context) const {
  std::list<LessRuleset*>::const_iterator r_it;

  for (r_it = nestedRules.begin(); r_it != nestedRules.end(); r_it++) {
    (*r_it)->process(s, prefix, context);
  }
}

void LessRuleset::addClosures(ProcessingContext& context) const {
  std::list<LessRuleset*>::const_iterator r_it;

  for (r_it = nestedRules.begin(); r_it != nestedRules.end(); r_it++) {
    if ((*r_it)->selector->needsArguments()) {
      context.addClosure(*(*r_it));
    }
  }
}

bool LessRuleset::matchConditions(const ProcessingContext& context,
                                  bool defaultVal) const {
  const std::list<TokenList>& conditions = selector->getConditions();
  std::list<TokenList>::const_iterator it;

  if (conditions.empty())
    return true;

  for (it = conditions.begin(); it != conditions.end(); it++) {

    if (context.validateCondition(*it, defaultVal)) {
      return true;
    }
  }
  return false;
}

bool LessRuleset::putArguments(MixinArguments& args, VariableMap& scope) const {
  const std::list<std::string>& parameters = selector->getParameters();
  std::list<std::string>::const_iterator pit;
  TokenList argsCombined;
  TokenList restVar;
  const TokenList* variable;
  size_t pos = 0;

  // combine with parameter names and add to local scope
  for (pit = parameters.begin(); pit != parameters.end(); pit++) {
    variable = args.get(*pit);

    if (variable == NULL)
      variable = args.get(pos++);

    if (variable == NULL)
      variable = selector->getDefault(*pit);

    if (variable == NULL || variable->empty())
      return false;

    scope.insert(pair<std::string, TokenList>(*pit, *variable));

    argsCombined.insert(argsCombined.end(), variable->begin(), variable->end());
    argsCombined.push_back(Token::BUILTIN_SPACE);
  }

  argsCombined.trim();

  if (selector->unlimitedArguments() && selector->getRestIdentifier() != "") {
    while (pos < args.count()) {
      variable = args.get(pos++);
      restVar.insert(restVar.end(), variable->begin(), variable->end());
      restVar.push_back(Token::BUILTIN_SPACE);
    }

    restVar.trim();
    scope.insert(
        pair<std::string, TokenList>(selector->getRestIdentifier(), restVar));
  }

  scope.insert(pair<std::string, TokenList>("@arguments", argsCombined));
  return true;
}

void LessRuleset::mergeDeclarations(Ruleset &ruleset, Declaration* merge) const {
  const std::list<Declaration*> *declarations = &ruleset.getDeclarations();
  std::list<Declaration*>::const_iterator it, del;
  Token* t;
  bool space;

  for (it = declarations->begin(); it != declarations->end(); ) {
    t = &(*it)->getProperty();
    
    if ((t->size() > 0 && t->at(t->size() - 1) == '+') ||
        (t->size() > 1 && t->compare(t->size() - 2, 2, "+_") == 0)) {

      space = t->at(t->size() - 1) == '_';
      
      if (merge == NULL) {

        t->resize(t->size() - (space ? 2 : 1));
        mergeDeclarations(ruleset, *it);
        it++;
        
      } else if (t->compare(0,
                            merge->getProperty().size(),
                            merge->getProperty()) == 0) {
        if (space) {
          merge->getValue().push_back(Token::BUILTIN_SPACE);
        } else {
          merge->getValue().push_back(Token::BUILTIN_COMMA);
          merge->getValue().push_back(Token::BUILTIN_SPACE);
        }
        merge->getValue().insert(merge->getValue().end(),
                                 (*it)->getValue().begin(),
                                 (*it)->getValue().end());
        del = it;
        it++;
        ruleset.deleteDeclaration(**del);
      } else
        it++;
    } else
      it++;
  }
}
