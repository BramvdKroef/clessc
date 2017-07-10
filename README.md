# Introduction

This is a LESS CSS compiler written in C++.

A LESS CSS compiler compiles stylesheets in the LESS format to
CSS. Read more on http://lesscss.org/ 

# Differences from the original compiler

By default, this compiler does not apply formatting to the code. The
CSS in the output contains no whitespace except where necessary. You
can use the `-f` option to indent the output. If you need it to be more
user-readable you can use a CSS formatter. Try CSSTidy:
http://csstidy.sourceforge.net/

CSS comments are not included in the output. The purpose of this
compiler is to produce the smallest amount of output as possible. 

Parentheses are not required in compound values:

```
border: @width * 2 solid black;
```

The `spin()` function produces slightly different results.

There is an `imgwidth()` and a `imgheight()` function which can be used
with image variables to access their dimensions. e.g.

```
    img.thumbup {
       content: url(browser/path/to/thumbup.png);
       @image: url('compile/time/path/to/thumbup.png');
       width: imgwidth(@image);
       height: imgheight(@image);
    }
```

# Installation

If compiling from the source checked out with git you first have to
run

```
./autogen.sh
```

to generate the `configure` script. This requires autotools. The
tarball distribution will already have the `configure` script.

To build the compiler just run

```
./configure
make
```

This requires a c++ compiler and the libpng and libjpeg libraries. To
build without the libraries run configure with these arguments:

```
./configure --without-libpng --without-libjpeg
make
```

After compiling you will have the binary `lessc`. You can run `make
install` as root to install the binary.

Note that the original compiler is also named `lessc`. 

# Usage

To compile the LESS stylesheet stylesheet.less and write the resulting
CSS to stylsheet.css run:

```
lessc stylesheet.less > stylesheet.css
```

or:

```
lessc stylesheet.less -o stylesheet.css
```

To also generate a source map:

```
lessc stylesheet.less -o stylesheet.css --source-map=stylesheet.map
```

# LESS Support Status

Here follows a list of LESS language features and their support
status:

    - Variables: yes
      - Default variables (variable overriding): yes
      - Lazy Evaluation: yes
      - Scope: yes
      - Variable interpolation
        - Selectors: yes
        - URLs: yes
        - Import statements: yes
        - Properties: yes
        - Variables: yes
    - Nested rules: yes
      - parent selectors `&`: yes
    - Nested directives: yes
    - Comments: yes
    - Importing: yes
      - Import directives: yes
      - File extensions: yes
        - inline: no
    - Mixins
      - Selectors in mixins: yes
      - Namespaces: yes
      - Guarded namespaces: no
      - `default()`: no
      - `!important` keyword: no
      - merge: no
        - `+` append with comma: no
        - `+_` append with space: no
    - Parametric Mixins
      - Arguments
        - default values: yes
        - separated by commas/semicolons: yes
        - named parameters: yes
        - `@arguments` variable: yes
        - `...` rest variable: yes
        - switch argument: no
      - Return values
        - Variables: yes
        - Mixins: yes
      - Mixin guards: yes
        - `and` keyword: yes
        - multiple conditions with `,`: yes
        - `not` keyword: no
        - `default()`: no
    - Detached rulesets: no
        
    - Operations
      - Operators (`+-*/`): yes
      - Unit conversion: yes *(1px = 1/96 inch)
      - Escaping: yes
      - Functions: partial
    - Extend
      - In ruleset: yes
      - In selector: yes
      - Quote type in attribute selector: no (`[type='identifier']` does not
        match `[type=identifier]`)
      - Extend `all`: yes
      - Attached to interpolated selector: no
      - Match only inside `@media`: no
  
    - Functions
      - `color`: yes
      - `image-size`: no 
      - `image-width`: no
      - `image-height`: no
      - `convert`: yes
      - `data-uri`: no
      - `default`: no
      - `unit`: yes
      - `get-unit`: no
      - `svg-gradient`: no
      - `escape`: yes
      - `e`: yes
      - `%`: no
      - `replace`: no
      - `length` : no
      - `extract`: no
      - `ceil`: yes
      - `floor`: yes
      - `percentage`: yes
      - `round`: yes
      - `sqrt`: yes
      - `abs`: yes
      - `sin`: yes
      - `asin`: yes
      - `cos`: yes
      - `acos`: yes
      - `tan`: yes
      - `atan`: yes
      - `pi`: yes
      - `pow`: yes
      - `mod`: yes
      - `min`: no
      - `max`: no
      - `isnumber`: no
      - `isstring`: no
      - `iscolor`: no
      - `iskeyword`: no
      - `isurl`: no
      - `ispixel`: no
      - `isem`: no
      - `ispercentage`: no
      - `isunit`: no
      - `isruleset`: no
      - `rgb`: yes
      - `rgba`: yes
      - `argb`: yes
      - `hsl`: yes
      - `hsla`: no
      - `hsv`: no
      - `hsva`: no
      - `hue`: yes
      - `saturation`: yes
      - `lightness`: yes
      - `hsvue`: no
      - `hsvsaturation`: no
      - `hsvvalue`: no
      - `red`: yes
      - `green`: yes
      - `blue`: yes
      - `alpha`: yes
      - `luma`: no
      - `luminance`: no
      - `saturate`: yes
      - `desaturate`: yes
      - `lighten`: yes
      - `darken`: yes
      - `fadein`: yes
      - `fadeout`: yes
      - `fade`: no
      - `spin`: yes
      - `mix`: no
      - `tint`: no
      - `shade`: no
      - `greyscale`: no
      - `contrast`: no
      - `multiply`: no
      - `screen`: no
      - `overlay`: no
      - `softlight`: no
      - `hardlight`: no
      - `difference`: no
      - `exclusion`: no
      - `average`: no
      - `negation`: no
