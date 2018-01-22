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
    "       --rootpath=<PATH>           Prefix PATH to urls and import \
statements in output. \n"
    "   -I, --include-path=<FILE>       Specify paths to look for \
imported .less files.\n"
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
                std::list<const char*> &includePaths) {
  std::list<const char*>::iterator i;
  
  LessTokenizer tokenizer(in, source);
  LessParser parser(tokenizer, sources);
  parser.includePaths = &includePaths;
  
  try{
    parser.parseStylesheet(stylesheet);
  } catch(ParseException* e) {

    cerr << e->getSource() << ": Line " << e->getLineNumber() << ", Column " << 
      e->getColumn() << " Parse Error: " << e->what() << endl;
    
    return false;
  } catch(exception* e) {
    cerr << " Error: " << e->what() << endl;

    return false;
  }
  
  return true;
}

bool processStylesheet (LessStylesheet &stylesheet,
                        Stylesheet &css) {
  ProcessingContext context;

  try{
    stylesheet.process(css, context);

  } catch(ParseException* e) {
    
    cerr << e->getSource() << ": Line " << e->getLineNumber() << ", Column " << 
      e->getColumn() << " Parse Error: " << e->what() << endl;
    return false;

  } catch(ValueException* e) {

    cerr << e->getSource() << ": Line " << e->getLineNumber() << ", Column " << 
      e->getColumn() << " Error: " << e->what() << endl;
    return false;
    
  } catch(exception* e) {
    
    cerr << "Error: " << e->what() << endl;
    return false;
  }
  return true;
}

void writeOutput(Stylesheet &css,
                 const char* output,
                 bool formatoutput,
                 const char* rootpath,
                 std::list<const char*> &sources,
                 std::string sourcemap_file,
                 const char* sourcemap_rootpath,
                 const char* sourcemap_basepath) {
  ostream* out = &cout;
  CssWriter* writer;
  ostream* sourcemap_s = NULL;
  SourceMapWriter* sourcemap = NULL;
  
  
  if (strcmp(output, "-") != 0)
    out = new ofstream(output);
      
  if (!sourcemap_file.empty()) {
    sourcemap_s = new ofstream(sourcemap_file);
    sourcemap = new SourceMapWriter(*sourcemap_s,
                                    sources,
                                    output,
                                    sourcemap_rootpath,
                                    sourcemap_basepath);

    writer = formatoutput ? new CssPrettyWriter(*out, *sourcemap) :
      new CssWriter(*out, *sourcemap);
  } else {
    writer = formatoutput ? new CssPrettyWriter(*out) :
      new CssWriter(*out);
  }
  writer->rootpath = rootpath;
      
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
}

void writeDependencies(const char* output, const std::list<const char*> &sources) {
  std::list<const char *>::const_iterator i;

  cout << output << ": ";

  for (i = sources.begin(); i != sources.end(); i++) {
    if (i != sources.begin())
      cout << ", ";
    cout << (*i);
  }
  cout << endl;
}

int main(int argc, char * argv[]){
  istream* in = &cin;
  bool formatoutput = false;
  char* source = NULL;
  string output = "-";
  LessStylesheet stylesheet;
  std::list<const char*> sources;
  Stylesheet css;
  bool depends = false, lint = false;

  std::string sourcemap_file = "";

  const char* sourcemap_rootpath = NULL;
  const char* sourcemap_basepath = NULL;
  const char* rootpath = NULL;

  std::list<const char*> includePaths;

  static struct option long_options[] = {
    {"version",             no_argument,       0, 1},
    {"help",                no_argument,       0, 'h'},
    {"output",              required_argument, 0, 'o'},
    {"format",              no_argument,       0, 'f'},
    {"source-map",          optional_argument, 0, 'm'},
    {"source-map-rootpath", required_argument, 0, 2},
    {"source-map-basepath", required_argument, 0, 3},
    {"include-path",        required_argument, 0, 'I'},
    {"rootpath",            required_argument, 0, 4},
    {"depends",             no_argument,       0, 'M'},
    {"lint",                no_argument,       0, 'l'},
    {0,0,0,0}
  };
  
  try {
    int c, option_index;

    while((c = getopt_long(argc, argv, ":o:hfv:m::I:Ml", long_options, &option_index)) != -1) {
      switch (c) {
      case 1:
        version();
        return 0;
        
      case 'h':
        usage();
        return 0;
        
      case 'o':
        output = optarg;
        break;
        
      case 'f':
        formatoutput = true;
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

      case 'M':
        depends = true;
        break;
        
      case 'l':
        lint = true;
        break;
        
      default:
        cerr << "Unrecognized option. " << endl;
        usage();
        return 1;
                                         
      }
    }
    
    if (argc - optind >= 1) {

      source = new char[std::strlen(argv[optind]) + 1];
      std::strcpy(source, argv[optind]);
      
      in = new ifstream(source);
      if (in->fail() || in->bad())
        throw new IOException("Error opening file.");

    }  else {
      
      source = new char[2];
      std::strcpy(source, "-");
      
    }

    if (sourcemap_file == "-") {
      if (output == "-") {
        throw new IOException("source-map option requires that \
a file name is specified for either the source map or the css \
output file.");
      } else {
        sourcemap_file = output;
        sourcemap_file += ".map";
      }
    }
    
    sources.push_back(source);
    
    if (parseInput(stylesheet, *in, source, sources, includePaths)) {
      if (depends) {
        writeDependencies(output.c_str(), sources);
        return 0;
      }

      if (!processStylesheet(stylesheet, css)) {
        return 1;
      }
      
      if (lint) {
        return 0;
      }

      writeOutput(css,
                  output.c_str(),
                  formatoutput,
                  rootpath,
                  sources,
                  sourcemap_file,
                  sourcemap_rootpath,
                  sourcemap_basepath);
    } else
      return 1;
    delete [] source;
    
  } catch (IOException* e) {
    cerr << " Error: " << e->what() << endl;
    return 1;
  }
		
  return 0;
}
