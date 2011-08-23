#ifndef __Stylesheet_h__
#define __Stylesheet_h__

class Ruleset {
  vector declarations;
}

class Stylesheet {
private:
  vector<string> atrules;
  hashmap<Selector, Ruleset> rulesets;
public:
  Stylesheet();
  virtual ~Stylesheet();
}

#endif
