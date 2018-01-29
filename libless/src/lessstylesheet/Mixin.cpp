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

  std::list<const Function *>::iterator i;
  std::list<const Function *> functionList;
  const Function *function;
  bool success;
  
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
        success = function->call(arguments_p, *r_target, context) || success;
      else
        success = function->call(arguments_p, *s_target, context) || success;

      context.popMixinCall();
    }
  }

  // if no functions matched, try it with 'default()' set to true.
  if (!success) {
    for (i = functionList.begin(); i != functionList.end(); i++) {
      function = *i;

      if (function->getLessSelector()->needsArguments() ||
          !context.isInStack(*function)) {
        context.pushMixinCall(*function);

        if (r_target != NULL)
          function->call(arguments_p, *r_target, context, true);
        else
          function->call(arguments_p, *s_target, context, true);

        context.popMixinCall();
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

void Mixin::process(Stylesheet &s, void* context) const {
  call(*((ProcessingContext*)context), NULL, &s);
}

void Mixin::process(Ruleset &r, void* context) const {
  call(*((ProcessingContext*)context), &r, NULL);
}

