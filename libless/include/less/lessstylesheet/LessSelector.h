#ifndef __less_lessstylesheet_LessSelector_h__
#define __less_lessstylesheet_LessSelector_h__

#include <iterator>
#include <list>
#include <map>
#include <string>

#include "less/lessstylesheet/MixinArguments.h"
#include "less/lessstylesheet/Extension.h"
#include "less/stylesheet/Selector.h"

class LessSelector : public Selector {
private:
  std::list<Extension> extensions;
  std::list<std::string> parameters;
  std::list<TokenList> defaults;
  std::list<TokenList> conditions;

  bool _unlimitedArguments;
  bool _needsArguments;
  std::string restIdentifier;

public:
  LessSelector();
  virtual ~LessSelector();

  void addExtension(Extension &extension);
  void addParameter(Token &keyword, TokenList &value);
  void setUnlimitedArguments(bool b);
  void setUnlimitedArguments(bool b, Token restKeyword);
  void eraseArguments();
  void addCondition(TokenList &condition);
  void setNeedsArguments(bool b);
    
  const std::list<Extension> &getExtensions() const;
  const std::list<std::string> &getParameters() const;
  const TokenList *getDefault(const std::string &parameter) const;

  const std::list<TokenList> &getConditions() const;
  bool matchArguments(const MixinArguments &arguments) const;

  bool needsArguments() const;
  bool unlimitedArguments() const;
  std::string getRestIdentifier() const;

};

#endif  // __less_lessstylesheet_LessSelector_h__
