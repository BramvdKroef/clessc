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


bool Mixin::call(ProcessingContext &context,
                 Ruleset *r_target,
                 Stylesheet *s_target) const {

  LessRuleset *parent = getLessRuleset();
  
  std::list<const Function *>::iterator i;
  std::list<const Function *> functionList;
  const Function *function;
  
  MixinArguments arguments_p;
  
  context.getFunctions(functionList, *this);

  if (functionList.empty())
    return false;

  arguments_p = arguments;
  arguments_p.process(context);
    
  for (i = functionList.begin(); i != functionList.end(); i++) {
    function = *i;

    if (function->getLessSelector()->needsArguments() ||
        !context.isInStack(*function)) {
      context.pushMixinCall(*function);

      if (r_target != NULL)
        function->call(arguments_p, *r_target, context);
      else
        function->call(arguments_p, *s_target, context);

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

LessRuleset *Mixin::getLessRuleset() const {
  return lessRuleset;
}

void Mixin::process(Stylesheet &s) {
  ProcessingContext *c;
  if (getLessStylesheet() != NULL)
    c = getLessStylesheet()->getContext();
  else
    c = getLessRuleset()->getContext();
  call(*c, NULL, &s);
}

void Mixin::process(Ruleset &r) {
  call(*getLessRuleset()->getContext(), &r, NULL);
}

