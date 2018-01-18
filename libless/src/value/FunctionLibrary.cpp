#include "less/value/FunctionLibrary.h"

const FuncInfo* FunctionLibrary::getFunction(const char* functionName) const {
  std::map<std::string, FuncInfo*>::const_iterator i = map.find(functionName);

  if (i != map.end())
    return i->second;
  else
    return NULL;
}

void FunctionLibrary::push(
    string name,
    const char* parameterTypes,
    Value* (*func)(const vector<const Value*>& arguments)) {
  FuncInfo* fi = new FuncInfo();
  fi->parameterTypes = parameterTypes;
  fi->func = func;
  map[name] = fi;
}

bool FunctionLibrary::checkArguments(
    const FuncInfo* fi, const vector<const Value*>& arguments) const {
  const char* types = fi->parameterTypes;
  vector<const Value*>::const_iterator it = arguments.begin();
  unsigned int i, len = strlen(types);

  for (i = 0; i < len; i++) {
    if (it == arguments.end()) {
      if (i + 1 < len && (types[i + 1] == '?' || types[i + 1] == '+')) {
        i++;
        continue;
      } else
        return false;
    }

    if (types[i] != '.' && (*it)->type != Value::codeToType(types[i]))
      return false;

    it++;

    if (i + 1 < len) {
      if (types[i + 1] == '+') {
        while (
            it != arguments.end() &&
            (types[i] == '.' || (*it)->type == Value::codeToType(types[i]))) {
          it++;
        }
        i++;
      } else if (types[i + 1] == '?') {
        i++;
      }
    }
  }

  if (it != arguments.end())
    return false;
  return true;
}

const char* FunctionLibrary::functionDefToString(const char* functionName,
                                                 const FuncInfo* fi) const {
  if (fi == NULL)
    fi = getFunction(functionName);
  if (fi == NULL)
    return "";

  string str(functionName);
  const char* types = fi->parameterTypes;
  unsigned int i, len = strlen(types);
  char* retstr;

  str.append("(");
  for (i = 0; i < len; i++) {
    if (types[i] == '.')
      str.append("Any");
    else
      str.append(Value::typeToString(Value::codeToType(types[i])));

    if (i + 1 < len) {
      if (types[i + 1] == '?') {
        str.append(" (optional)");
        i++;
      } else if (types[i + 1] == '+') {
        str.append("...");
        i++;
      }
    }

    if (i != len - 1)
      str.append(", ");
  }
  str.append(")");

  retstr = new char[str.length() + 1];
  std::strcpy(retstr, str.c_str());

  return retstr;
}
