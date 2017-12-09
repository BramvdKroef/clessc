
#ifndef __Mixin_h__
#define __Mixin_h__

#include "../stylesheet/Selector.h"
#include "../stylesheet/Stylesheet.h"
#include "../stylesheet/StylesheetStatement.h"
#include "../stylesheet/Ruleset.h"

#include "../css/CssWriter.h"

#include "../TokenList.h"

#include "ProcessingContext.h"
#include <vector>
#include <map>
#include <string>

class LessStylesheet;
class LessRuleset;
class ProcessingContext;

class Mixin: public StylesheetStatement{
private:
  vector<TokenList> arguments;
  map<std::string, TokenList> namedArguments;

  LessStylesheet* lessStylesheet;
  void parseArguments(TokenList::const_iterator i, const Selector &s);

public:
  Selector name;

  Mixin();
  Mixin(const Selector &name);
  virtual ~Mixin() ;
  
  const TokenList* getArgument(const size_t i) const;
  size_t getArgumentCount() const;
  
  const TokenList* getArgument(const string &name) const;

  bool call(Stylesheet &s, ProcessingContext &context,
              Ruleset* ruleset, LessRuleset* parent);
  bool parse(const Selector &selector);

  virtual void setLessStylesheet(LessStylesheet &stylesheet);
  LessStylesheet* getLessStylesheet();

  virtual void process(Stylesheet &s);
  virtual void write(CssWriter &writer) {};
};

#endif
