#ifndef __ParseException_h__
#define __ParseException_h__

#include <exception>
#include <string>

using namespace std;

class ParseException: public exception{
public:
  string err;
	
  ParseException(string found, string& expected){
    err.append("Found \"");
    err.append(translate(found));
    err.append("\" when expecting ");
    err.append(expected);
  }
  ParseException(string found, const char* expected){
    err.append("Found \"");
    err.append(translate(found));
    err.append("\" when expecting ");
    err.append(expected);
  }
  ParseException(const char* found, const char* expected){
    err.append("Found \"");
    if (found[0] == -1)
      err.append("end of file");
    else
      err.append(translate(string(found)));
    err.append("\" when expecting ");
    err.append(expected);
  }

  ~ParseException() throw () {};
  
  virtual const char* what() const throw(){
    return err.c_str();
  }

protected:
  string translate(string found) {
    if (found == "\n")
      return "newline";
    return found;
  }

};

#endif
