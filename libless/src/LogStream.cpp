#include "less/LogStream.h"

#ifdef WITH_LIBGLOG

#include <glog/logging.h>

std::ostream& LogStream::notice()
{
  return LOG(INFO);
}

std::ostream& LogStream::notice(int verbosityLevel)
{
  return VLOG(verbosityLevel);
}

std::ostream& LogStream::warning()
{
  return LOG(WARNING);
}

std::ostream& LogStream::error()
{
  return LOG(ERROR);
}

#else // WITH_LIBGLOG

#include <iostream>

std::ostream& LogStream::notice()
{
  return std::clog << '\n';
}

std::ostream& LogStream::notice(int verbosityLevel)
{
  return notice() << "VERBOSE " << verbosityLevel << ": ";
}

std::ostream& LogStream::warning()
{
  return std::cerr << '\n';
}

std::ostream& LogStream::error()
{
  return std::cerr << '\n';
}

#endif // WITH_LIBGLOG
