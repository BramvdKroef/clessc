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
source file references in the source map. \n"
    "       --source-map-basepath=<PATH>   PATH is removed from the \
source file references in the source map.\n"
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
char* path_create(const char* path, size_t len) {
  size_t newlen = len +
    (path[len - 1] != '/' ? 1 : 0);
  char* p = new char[newlen + 1];
  std::strncpy(p, path, len);
  p[newlen - 1] = '/';
  p[newlen] = '\0';
  return p;
}

void path_parse_list(const char* path, std::list<const char*>& paths) {
  const char* start = path;
  const char* end = path;
  size_t len;

  while((end = std::strchr(start, ':')) != NULL) {
    len = (end - start);
    // skip empty paths
    if (len > 0)
      paths.push_back(path_create(start, len));

    start = end + 1;
  }
  len = std::strlen(start);
  if (len > 0) 
    paths.push_back(path_create(start, len));
}

char* path_create_relative(const char* path, const char* relative) {
  const char* p_root;
  const char* r_root;
  size_t desc_n;
  char* ret, *ret_p;

  // strip common directories
  while ((p_root = strchr(path, '/')) != NULL &&
         (r_root = strchr(relative, '/')) != NULL &&
         p_root - path == r_root - relative &&
         strncmp(path, relative, p_root - path) == 0) {
    path = p_root + 1;
    relative = r_root + 1;
  }

  // count how many folders to go up.
  for (desc_n = 0; relative != NULL; desc_n++) {
    relative = strchr(relative + 1, '/');
  }
  desc_n--;

  ret = new char[strlen(path) + 3 * desc_n + 1];
  ret_p = ret;
  while (desc_n > 0) {
    memcpy(ret_p, "../", 3);
    ret_p += 3;
    desc_n--;
  }
  
  strcpy(ret_p, path);
  return ret;
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
                 const char* sourcemap_file,
                 const char* sourcemap_rootpath,
                 const char* sourcemap_basepath) {
  ostream* out = &cout;
  CssWriter* writer;
  ostream* sourcemap_s = NULL;
  SourceMapWriter* sourcemap = NULL;
  char* tmp;
  std::list<const char*> relative_sources;
  std::list<const char*>::iterator it;
  
  if (strcmp(output, "-") != 0) 
    out = new ofstream(output);

  if (sourcemap_file != NULL) {
    if (strcmp(sourcemap_file, "-") == 0) {
      if (strcmp(output, "-") == 0) {
        throw new IOException("source-map option requires that \
a file name is specified for either the source map or the css  \
output file.");
      } else {
        tmp = new char[strlen(output) + 5];
        sprintf(tmp, "%s.map", output);
        sourcemap_file = tmp;
      }
    }
    for (it = sources.begin(); it != sources.end(); it++) {
      relative_sources.push_back(path_create_relative(*it, sourcemap_file));
    }
    
    sourcemap_s = new ofstream(sourcemap_file);
    sourcemap = new SourceMapWriter(*sourcemap_s,
                                    sources,
                                    relative_sources,
                                    path_create_relative(output, sourcemap_file),
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
    writer->writeSourceMapUrl(path_create_relative(sourcemap_file, output));
    
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
  const char* output = "-";
  LessStylesheet stylesheet;
  std::list<const char*> sources;
  Stylesheet css;
  bool depends = false, lint = false;

  const char* sourcemap_file = NULL;

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
        return EXIT_SUCCESS;
        
      case 'h':
        usage();
        return EXIT_SUCCESS;
        
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
        sourcemap_rootpath = path_create(optarg, std::strlen(optarg));
        break;
        
      case 3:
        sourcemap_basepath = path_create(optarg, std::strlen(optarg));
        break;

      case 'I':
        path_parse_list(optarg, includePaths);
        break;

      case 4:
        rootpath = path_create(optarg, std::strlen(optarg));
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
        return EXIT_FAILURE;
                                         
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
    
    sources.push_back(source);
    
    if (parseInput(stylesheet, *in, source, sources, includePaths)) {
      if (depends) {
        writeDependencies(output, sources);
        return EXIT_SUCCESS;
      }

      if (!processStylesheet(stylesheet, css))
        return EXIT_FAILURE;
     
      if (lint) 
        return EXIT_SUCCESS;
     
      writeOutput(css,
                  output,
                  formatoutput,
                  rootpath,
                  sources,
                  sourcemap_file,
                  sourcemap_rootpath,
                  sourcemap_basepath);
    } else
      return EXIT_FAILURE;
    delete [] source;
    
  } catch (IOException* e) {
    cerr << " Error: " << e->what() << endl;
    return EXIT_FAILURE;
  }
		
  return EXIT_SUCCESS;
}
