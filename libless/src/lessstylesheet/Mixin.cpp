#include "less/lessstylesheet/Mixin.h"
#include "less/lessstylesheet/LessRuleset.h"
#include "less/lessstylesheet/LessStylesheet.h"
#include "less/lessstylesheet/MixinException.h"

Mixin::Mixin(const TokenList &name, const LessStylesheet &parent)
  : lessStylesheet(&parent), lessRuleset(NULL), important(false), name(name) {
}
Mixin::Mixin(const TokenList &name, const LessRuleset &parent)
  : lessStylesheet(NULL), lessRuleset(&parent), important(false), name(name) {
}

Mixin::~Mixin() {
}


bool Mixin::call(ProcessingContext &context,
                 Ruleset *r_target,
                 Stylesheet *s_target) const {

  std::list<const Function *>::iterator i;
  std::list<const Function *> functionList;
  const Function *function;
  bool success = false;
  
  MixinArguments arguments_p;
  
  context.getFunctions(functionList, *this);

  if (functionList.empty()) {
    throw new MixinException(*this);
  }

  arguments_p = arguments;
  arguments_p.process(context);
  
  for (i = functionList.begin(); i != functionList.end(); i++) {
    function = *i;

    if (function->getLessSelector().needsArguments() ||
        !context.isInStack(*function)) {
      context.pushMixinCall(*function, false, isImportant());

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

      if (function->getLessSelector().needsArguments() ||
          !context.isInStack(*function)) {
        context.pushMixinCall(*function, false, isImportant());

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

void Mixin::setImportant(bool b) {
  important = b;
}
bool Mixin::isImportant() const {
  return important;
}

const LessStylesheet *Mixin::getLessStylesheet() const {
  return lessStylesheet;
}

const LessRuleset *Mixin::getLessRuleset() const {
  return lessRuleset;
}

void Mixin::process(Stylesheet &s, void* context) const {
  call(*((ProcessingContext*)context), NULL, &s);
}

void Mixin::process(Ruleset &r, void* context) const {
  call(*((ProcessingContext*)context), &r, NULL);
}

void Mixin::write(CssWriter &writer) const {
  (void)writer;
}
