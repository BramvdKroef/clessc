#include "less/lessstylesheet/Mixin.h"
#include "less/lessstylesheet/LessRuleset.h"
#include "less/lessstylesheet/LessStylesheet.h"

Mixin::Mixin() {
  lessStylesheet = NULL;
  lessRuleset = NULL;

}

Mixin::Mixin(const Selector &name) {
  this->name = name;
  lessStylesheet = NULL;
  lessRuleset = NULL;
}

Mixin::~Mixin() {
}


bool Mixin::call(Stylesheet &s,
                 ProcessingContext &context,
                 Ruleset *target,
                 LessRuleset *parent) const {

  std::list<const Function *>::iterator i;
  std::list<const Function *> functionList;
  const Function *function;
  
  MixinArguments arguments_p;
  

  if (parent != NULL)
    context.getFunctions(functionList, *this);
  else
    getLessStylesheet()->getFunctions(functionList, *this);

  if (functionList.empty())
    return false;

  arguments_p = arguments;
  arguments_p.process(context);
    
  for (i = functionList.begin(); i != functionList.end(); i++) {
    function = *i;

    if (function->getLessSelector()->needsArguments() ||
        !context.isInStack(*function)) {
      context.pushMixinCall(*function);

      if (target != NULL)
        function->call(arguments_p, *target, context);
      else
        function->call(arguments_p, s, context);

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

LessStylesheet *Mixin::getLessStylesheet() const {
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

