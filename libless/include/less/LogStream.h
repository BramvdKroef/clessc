#ifndef __less_LogStream_h__
#define __less_LogStream_h__

#include <ostream>

class LogStream {
public:
  std::ostream& notice();
  std::ostream& notice(int verbosityLevel);
  std::ostream& warning();
  std::ostream& error();
};

#endif  // __less_LogStream_h__
