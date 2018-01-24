#ifndef __less_lessstylesheet_LessSelector_h__
#define __less_lessstylesheet_LessSelector_h__

#include <iterator>
#include <list>
#include <map>

#include "less/lessstylesheet/Extension.h"
#include "less/lessstylesheet/MixinArguments.h"
#include "less/stylesheet/Selector.h"


class LessSelector : public Selector {
private:
  std::list<Extension> extensions;
  std::list<string> parameters;
  std::list<TokenList> defaults;
  std::list<TokenList> conditions;

  bool _unlimitedArguments;
  bool _needsArguments;
  std::string restIdentifier;

  bool parseExtension(Selector &selector, Selector &extension);
  bool parseArguments(TokenList &selector);
  bool validateArguments(const TokenList &arguments,
                         const std::string &delimiter);
  bool parseParameter(TokenList &selector, const std::string &delimiter);
  bool parseDefaultValue(TokenList &arguments,
                         const std::string &delimiter,
                         TokenList &value);
  bool parseConditions(TokenList &selector);

public:
  LessSelector(const Selector &original);
  virtual ~LessSelector();

  std::list<Extension> &getExtensions();
  std::list<std::string> &getParameters();
  TokenList *getDefault(const std::string &parameter);

  std::list<TokenList> &getConditions();
  bool matchArguments(const MixinArguments &arguments);

  bool needsArguments();
  bool unlimitedArguments();
  std::string getRestIdentifier();
};

#endif  // __less_lessstylesheet_LessSelector_h__
