#ifndef __less_lessstylesheet_Mixin_h__
#define __less_lessstylesheet_Mixin_h__

#include <vector>
#include <map>
#include <string>

#include "less/stylesheet/Selector.h"
#include "less/stylesheet/Stylesheet.h"
#include "less/stylesheet/StylesheetStatement.h"
#include "less/stylesheet/Ruleset.h"

#include "less/css/CssWriter.h"

#include "less/TokenList.h"

#include "less/lessstylesheet/ProcessingContext.h"

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

#endif // __less_lessstylesheet_Mixin_h__
