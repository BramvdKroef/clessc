#include <less/lessstylesheet/Mixin.h>
#include <less/lessstylesheet/LessStylesheet.h>
#include <less/lessstylesheet/LessRuleset.h>
#include <less/LogStream.h>

Mixin::Mixin() {
}

Mixin::Mixin(const Selector &name) {
  this->name = name;
}


Mixin::~Mixin() {
}

const TokenList* Mixin::getArgument(const size_t i) const {
  if (i < arguments.size())
    return &arguments[i];
  else
    return NULL;
}
size_t Mixin::getArgumentCount() const {
  return arguments.size();
}
const TokenList* Mixin::getArgument(const string &name) const{
  map<string, TokenList>::const_iterator i;

  i = namedArguments.find(name);
  
  if (i != namedArguments.end())
    return &i->second;
  else
    return NULL;
}


bool Mixin::parse(const Selector &selector) {
  
  TokenList::const_iterator i = selector.begin();
  
  for (; i != selector.end() &&
         (*i).type != Token::PAREN_OPEN; i++) {
    this->name.push_back(*i);
  }

  this->name.rtrim();

  parseArguments(i, selector);

  return true;
}


bool Mixin::call(Stylesheet &s, ProcessingContext &context,
                   Ruleset* target, LessRuleset* parent) {

  vector<TokenList>::iterator arg_i;
  map<string, TokenList>::iterator argn_i;
  list<const Function*>::iterator i;
  list<const Function*> functionList;
  const Function* function;

  LogStream().notice(2) << "Mixin: \"" << name.toString() << "\"";

  if (parent != NULL) 
    context.getFunctions(functionList, *this);
  else
    getLessStylesheet()->getFunctions(functionList, *this);

  if (functionList.empty())
    return false;
  
  for (arg_i = arguments.begin(); arg_i != arguments.end(); arg_i++) {
    LogStream().notice(3) << "Mixin Arg: " << (*arg_i).toString();
    context.processValue(*arg_i);
  }

  for (argn_i = namedArguments.begin();
       argn_i != namedArguments.end(); argn_i++) {
    LogStream().notice(3) << "Mixin Arg " << argn_i->first << ": " << argn_i->second.toString();
    context.processValue(argn_i->second);
  }
  
  for (i = functionList.begin(); i != functionList.end(); i++) {
    function = *i;

    LogStream().notice(3) << "Mixin: " << function->getLessSelector()->toString();

    if (function->getLessSelector()->needsArguments() ||
        !context.isInStack(*function)) {
      context.pushMixinCall(*function);
      
      if (target != NULL)
        function->call(*this, *target, context);
      else
        function->call(*this, s, context);

      context.popMixinCall();
      if (parent != NULL)  {
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

LessStylesheet* Mixin::getLessStylesheet() {
  return lessStylesheet;
}


void Mixin::process(Stylesheet &s) {
  call(s, *getLessStylesheet()->getContext(), NULL, NULL);
}

void Mixin::parseArguments(TokenList::const_iterator i, const Selector &selector) {
  TokenList::const_iterator j;
  string delimiter = ",";

  TokenList argument;
  size_t nestedParenthesis = 0;
  string argName;

  if (i != selector.end() &&
      (*i).type == Token::PAREN_OPEN) {
    i++;
  }
    
  // if a ';' token occurs then that is the delimiter instead of the ','.
  for(j = i; j != selector.end(); j++) {
    if (*j == ";") {
      delimiter = ";";
      break;
    }
  }

  while (i != selector.end() && (*i).type != Token::PAREN_CLOSED) {
    while (i != selector.end() &&
           (*i).type == Token::WHITESPACE) {
      i++;
    }

    if ((*i).type == Token::ATKEYWORD) {
      argName = (*i);
      i++;
      if (i != selector.end() &&
          (*i).type == Token::COLON) {
        i++;
      } else {
        argName = "";
        i--;
      }
    }

    while (i != selector.end() &&
           (nestedParenthesis > 0 ||
            ((*i) != delimiter &&
             (*i).type != Token::PAREN_CLOSED))) {
      
      if ((*i).type == Token::PAREN_OPEN)
        nestedParenthesis++;
      
      if ((*i).type == Token::PAREN_CLOSED)
        nestedParenthesis--;
      
      argument.push_back(*i);
      
      i++;
    }

    if (*i == delimiter)
      i++;

    if (argName == "")
      this->arguments.push_back(argument);
    else {
      this->namedArguments.insert(pair<string,
                                  TokenList>(argName,argument));
      argName = "";
    }
    argument.clear();
  }
}
