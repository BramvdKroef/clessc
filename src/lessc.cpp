#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <getopt.h>
#include <cstring>

#include <less/less/LessTokenizer.h>
#include <less/less/LessParser.h>
#include <less/css/CssWriter.h>
#include <less/css/CssPrettyWriter.h>
#include <less/stylesheet/Stylesheet.h>
#include <less/css/IOException.h>
#include <less/lessstylesheet/LessStylesheet.h>
#include <less/LogStream.h>

#ifdef WITH_LIBGLOG
#include <glog/logging.h>
#endif

using namespace std;

/**
 * /main Less CSS compiler, implemented in C++.
 * 
 */

void usage () {
  cout <<
    "Usage: lessc [OPTION]... [FILE]\n"
    "\n"
    "   FILE				Less source file. If not given, source \
is read from stdin.\n"
    "   -h, --help			Show this message and exit.\n"
    "       --version		Print the program name and version.\n"
    "\n"
    "   -o, --output=<FILE>		Send output to FILE\n"
    "   -f, --format			Format output CSS with newlines and \
indentation. By default the output is unformatted.\n"
    "\n"
    "   -m, --source-map=[FILE]	Generate a source map.\n"
    "       --source-map-rootpath=<PATH>   PATH is prepended to the \
source file references in the source map, and also to the source map \
reference in the css output. \n"
    "       --source-map-basepath=<PATH>   PATH is removed from the \
source file references in the source map, and also from the source \
map reference in the css output.\n"
    "\n"
    "   -v, --verbose=<LEVEL>	Output log data for debugging. LEVEL is \
a number in the range 1-3 that defines granularity.\n" 
    "\n"
    "Example:\n"
    "   lessc in.less -o out.css\n"
    "\n"
    "Report bugs to: " PACKAGE_BUGREPORT "\n"
    PACKAGE_NAME " home page: <" PACKAGE_URL ">\n";
}

void version () {
  cout <<
    PACKAGE_STRING "\n"
    "Copyright 2012 Bram van der Kroef\n"
    "License: MIT License\n";
}

/**
 * Copy a given path to a new string and add a trailing slash if necessary.
 */
char* createPath(const char* path, size_t len) {
  size_t newlen = len +
    (path[len - 1] != '/' ? 1 : 0);
  char* p = new char[newlen + 1];
  std::strncpy(p, path, len);
  p[newlen - 1] = '/';
  p[newlen] = '\0';
  return p;
}

void parsePathList(const char* path, std::list<const char*>& paths) {
  const char* start = path;
  const char* end = path;
  size_t len;

  while((end = std::strchr(start, ':')) != NULL) {
    len = (end - start);
    // skip empty paths
    if (len > 0)
      paths.push_back(createPath(start, len));

    start = end + 1;
  }
  len = std::strlen(start);
  if (len > 0) 
    paths.push_back(createPath(start, len));
}


bool parseInput(LessStylesheet &stylesheet,
                istream &in,
                const char* source,
                std::list<const char*> &sources,
                std::list<const char*> &includePaths){
  std::list<const char*>::iterator i;
  
  LessTokenizer tokenizer(in, source);
  LessParser parser(tokenizer, sources);
  parser.includePaths = &includePaths;
  
  try{
    parser.parseStylesheet(stylesheet);
  } catch(ParseException* e) {

    LogStream().error() << e->getSource() << ": Line " << e->getLineNumber() << ", Column " << 
      e->getColumn() << " Parse Error: " << e->what();
    
    return false;
  } catch(exception* e) {
    LogStream().error() << " Error: " << e->what();

    return false;
  }

  LogStream().notice(1) << "Source files: ";
  for(i = sources.begin(); i != sources.end(); i++) {
    LogStream().notice(1) << (*i);
  }
  
  return true;
}
bool processStylesheet (LessStylesheet &stylesheet,
                  Stylesheet &css) {
  ProcessingContext context;

  try{
    stylesheet.process(css, context);

  } catch(ParseException* e) {
    
    LogStream().error() << e->getSource() << ": Line " << e->getLineNumber() << ", Column " << 
      e->getColumn() << " Parse Error: " << e->what();

    return false;

  } catch(ValueException* e) {

    LogStream().error() << e->getSource() << ": Line " << e->getLineNumber() << ", Column " << 
      e->getColumn() << " Error: " << e->what();
    
    return false;
  } catch(exception* e) {
    LogStream().error()  << "Error: " << e->what();
    return false;
  }
  return true;
}

int main(int argc, char * argv[]){
  istream* in = &cin;
  ostream* out = &cout;
  bool formatoutput = false;
  char* source = NULL;
  string output = "-";
  LessStylesheet stylesheet;
  std::list<const char*> sources;
  Stylesheet css;
  CssWriter* writer;

  std::string sourcemap_file = "";
  ostream* sourcemap_s = NULL;
  SourceMapWriter* sourcemap = NULL;
  const char* sourcemap_rootpath = NULL;
  const char* sourcemap_basepath = NULL;
  const char* rootpath = NULL;

  std::list<const char*> includePaths;

  static struct option long_options[] = {
    {"version",    no_argument,       0, 1},
    {"help",       no_argument,       0, 'h'},
    {"output",     required_argument, 0, 'o'},
    {"format",     no_argument,       0, 'f'},
    {"verbose",    required_argument, 0, 'v'},
    {"source-map", optional_argument, 0, 'm'},
    {"source-map-rootpath", required_argument, 0, 2},
    {"source-map-basepath", required_argument, 0, 3},
    {"include-path", required_argument,        0, 'I'},
    {"rootpath", required_argument,  0, 4},
    {0,0,0,0}
  };
  
#ifdef WITH_LIBGLOG
  FLAGS_logtostderr = 1;
  google::InitGoogleLogging(argv[0]);
  VLOG(1) << "Start.";
#endif
  
  try {
    int c, option_index;

    LogStream().notice(3) << "argc: " << argc;

    while((c = getopt_long(argc, argv, ":o:hfv:m::I:", long_options, &option_index)) != -1) {
      switch (c) {
      case 1:
        version();
        return 0;
      case 'h':
        usage();
        return 0;
      case 'o':
        output = optarg;
        out = new ofstream(optarg);
        break;
      case 'f':
        formatoutput = true;
        break;
      case 'v':
#ifdef WITH_LIBGLOG
        FLAGS_v = atoi(optarg);
#else
        std::cerr << "Warning: -v flag not supported: lessc has to be compiled with libglog.\n";
#endif
        break;
      case 'm':
        if (optarg)
          sourcemap_file = optarg;
        else
          sourcemap_file = "-";
        break;
        
      case 2:
        sourcemap_rootpath = createPath(optarg, std::strlen(optarg));
        break;
      case 3:
        sourcemap_basepath = createPath(optarg, std::strlen(optarg));
        break;

      case 'I':
        parsePathList(optarg, includePaths);
        break;

      case 4:
        rootpath = createPath(optarg, std::strlen(optarg));
        break;

      }
    }
    
    if(argc - optind >= 1){

      LogStream().notice(1) << argv[optind];
      
      source = new char[std::strlen(argv[optind]) + 1];
      std::strcpy(source, argv[optind]);
      
      in = new ifstream(source);
      if (in->fail() || in->bad())
        throw new IOException("Error opening file");

    } else if (sourcemap_file == "-") {
      throw new IOException("source-map option requires that \
a file name is specified for either the source map or the less \
source.");
    } else {
      source = new char[2];
      std::strcpy(source, "-");
    }
    
    if (sourcemap_file == "-") {
      sourcemap_file = source;
      sourcemap_file += ".map";
    }

    sources.push_back(source);
    
    if (parseInput(stylesheet, *in, source, sources, includePaths)) {
      if (sourcemap_file != "") {
        LogStream().notice(1) << "sourcemap: " << sourcemap_file;
        sourcemap_s = new ofstream(sourcemap_file.c_str());
        sourcemap = new SourceMapWriter(*sourcemap_s, sources, output.c_str(),
                                        sourcemap_rootpath,
                                        sourcemap_basepath);

        writer = formatoutput ? new CssPrettyWriter(*out, *sourcemap) :
          new CssWriter(*out, *sourcemap);
      } else {
        writer = formatoutput ? new CssPrettyWriter(*out) :
          new CssWriter(*out);
      }
      writer->rootpath = rootpath;
      
      if (!processStylesheet(stylesheet, css)) {
        return 1;
      }
      css.write(*writer);
      
      if (sourcemap != NULL) {
        if (sourcemap_basepath != NULL &&
            sourcemap_file.compare(0, std::strlen(sourcemap_basepath),
                                   sourcemap_basepath) == 0) {
          sourcemap_file.erase(0, std::strlen(sourcemap_basepath));
        }
        if (sourcemap_rootpath != NULL)
          sourcemap_file.insert(0, sourcemap_rootpath);
        
        writer->writeSourceMapUrl(sourcemap_file.c_str());
        sourcemap->close();
        delete sourcemap;
        if (sourcemap_s != NULL)
          delete sourcemap_s;
      }
      
      delete writer;
      *out << endl;
    } else
      return 1;
    delete [] source;
    
  } catch (IOException* e) {
    LogStream().error() << " Error: " << e->what();
    return 1;
  }
		
  return 0;
}
