#ifndef __less_LogStream_h__
#define __less_LogStream_h__

#include <ostream>

class NullBuffer : public std::streambuf
{
public:
  int overflow(int c) { return c; }
};
class NullStream : public std::ostream {
public:
  NullStream() : std::ostream(&m_sb) {}
private:
  NullBuffer m_sb;
};

class LogStream {
private:
  NullStream nullstream;
          
public:
  std::ostream& notice();
  std::ostream& notice(int verbosityLevel);
  std::ostream& warning();
  std::ostream& error();
};

#endif  // __less_LogStream_h__
