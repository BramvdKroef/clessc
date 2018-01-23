#include "less/lessstylesheet/Mixin.h"
#include "less/lessstylesheet/LessRuleset.h"
#include "less/lessstylesheet/LessStylesheet.h"

Mixin::Mixin() {
}

Mixin::Mixin(const Selector &name) {
  this->name = name;
  lessStylesheet = NULL;
  lessRuleset = NULL;
}

Mixin::~Mixin() {
}

const TokenList *Mixin::getArgument(const size_t i) const {
  if (i < arguments.size())
    return &arguments[i];
  else
    return NULL;
}
size_t Mixin::getArgumentCount() const {
  return arguments.size();
}
const TokenList *Mixin::getArgument(const std::string &name) const {
  std::map<std::string, TokenList>::const_iterator i;

  i = namedArguments.find(name);

  if (i != namedArguments.end())
    return &i->second;
  else
    return NULL;
}

void Mixin::addArgument(TokenList &argument) {
  arguments.push_back(argument);
}
void Mixin::addArgument(std::string name, TokenList &argument) {
  namedArguments.insert(std::pair<std::string, TokenList>(name, argument));
}


bool Mixin::call(Stylesheet &s,
                 ProcessingContext &context,
                 Ruleset *target,
                 LessRuleset *parent) {
  std::vector<TokenList>::iterator arg_i;
  std::map<std::string, TokenList>::iterator argn_i;
  std::list<const Function *>::iterator i;
  std::list<const Function *> functionList;
  const Function *function;

  if (parent != NULL)
    context.getFunctions(functionList, *this);
  else
    getLessStylesheet()->getFunctions(functionList, *this);

  if (functionList.empty())
    return false;

  for (arg_i = arguments.begin(); arg_i != arguments.end(); arg_i++) {
    context.processValue(*arg_i);
  }

  for (argn_i = namedArguments.begin(); argn_i != namedArguments.end();
       argn_i++) {
    context.processValue(argn_i->second);
  }

  for (i = functionList.begin(); i != functionList.end(); i++) {
    function = *i;


    if (function->getLessSelector()->needsArguments() ||
        !context.isInStack(*function)) {
      context.pushMixinCall(*function);

      if (target != NULL)
        function->call(*this, *target, context);
      else
        function->call(*this, s, context);

      context.popMixinCall();
      if (parent != NULL) {
        if (context.isSavePoint())
          parent->saveReturnValues(context);

      } else {
        getLessStylesheet()->saveReturnValues(context);
      }
    }
  }

  return true;
}

void Mixin::setLessStylesheet(LessStylesheet &s) {
  lessStylesheet = &s;
  stylesheet = &s;
}

LessStylesheet *Mixin::getLessStylesheet() {
  return lessStylesheet;
}
void Mixin::setLessRuleset(LessRuleset &r) {
  lessRuleset = &r;
}
LessRuleset *Mixin::getLessRuleset() {
  return lessRuleset;
}

void Mixin::process(Stylesheet &s) {
  call(s, *getLessStylesheet()->getContext(), NULL, getLessRuleset());
}
void Mixin::process(Ruleset &r) {
  call(*r.getStylesheet(),
       *getLessRuleset()->getContext(),
       &r,
       getLessRuleset());
}

