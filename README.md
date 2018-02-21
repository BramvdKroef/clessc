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

Parentheses are not required in compound values:

```
border: @width * 2 solid black;
```

The `spin()` function produces slightly different results.


# Installation

If compiling from the source checked out with git you first have to
run

```
cmake
```

To build the compiler just run

```
make
```

This searches for a c++ compiler and the libpng and libjpeg libraries.
The libraries are not used if not present.

After compiling you will have the binary `clessc`. You can run `make
install` as root to install the binary.

# Usage

To compile the LESS stylesheet stylesheet.less and write the resulting
CSS to stylsheet.css run:

```
clessc stylesheet.less > stylesheet.css
```

or:

```
clessc stylesheet.less -o stylesheet.css
```

To also generate a source map:

```
clessc stylesheet.less -o stylesheet.css --source-map=stylesheet.map
```

# LESS Support Status

Here follows a list of LESS language features and their support
status:

 * Variables: yes
   * Default variables (variable overriding): yes
   * Lazy Evaluation: yes
   * Scope: yes
   * Variable interpolation
     * Selectors: yes
     * URLs: yes
     * Import statements: yes
     * Properties: yes
     * Variables: yes
 * Nested rules: yes
   * parent selectors `&`: yes
   * Nested directives: yes
   * Comments: yes
   * Importing: yes
   * Import directives: yes
   * File extensions: yes
     * inline: no
 * Mixins
   * Selectors in mixins: yes
   * Namespaces: yes
   * Guarded namespaces: yes
   * `default()`: yes
   * `!important` keyword: yes
   * merge: no
     * `+` append with comma: no
     * `+_` append with space: no
 * Parametric Mixins
   * Arguments
     * default values: yes
     * separated by commas/semicolons: yes
     * named parameters: yes
       * `@arguments` variable: yes
       * `...` rest variable: yes
       * switch argument: yes
     * Return values
       * Variables: yes
       * Mixins: yes
     * Mixin guards: yes
       * `and` keyword: yes
       * multiple conditions with `,`: yes
       * `not` keyword: yes
       * `default()`: yes
   * Detached rulesets: no
        
 * Operations
   * Operators (`+-*/`): yes
   * Unit conversion: yes *(1px = 1/96 inch)
   * Escaping: yes
   * Functions: partial
 * Extend
   * In ruleset: yes
   * In selector: yes
   * Quote type in attribute selector: no (`[type='identifier']` does not
     match `[type=identifier]`)
   * Extend `all`: yes
   * Attached to interpolated selector: no
   * Match only inside `@media`: yes
  
 * Functions
   * `color`: yes
   * `image-size`: no 
   * `image-width`: yes
   * `image-height`: yes
   * `convert`: yes
   * `data-uri`: no
   * `default`: yes
   * `unit`: yes
   * `get-unit`: yes
   * `svg-gradient`: no
   * `escape`: yes
   * `e`: yes
   * `%`: yes
   * `replace`: yes
   * `length` : no
   * `extract`: no
   * `ceil`: yes
   * `floor`: yes
   * `percentage`: yes
   * `round`: yes
   * `sqrt`: yes
   * `abs`: yes
   * `sin`: yes
   * `asin`: yes
   * `cos`: yes
   * `acos`: yes
   * `tan`: yes
   * `atan`: yes
   * `pi`: yes
   * `pow`: yes
   * `mod`: yes
   * `min`: yes
   * `max`: yes
   * `isnumber`: yes
   * `isstring`: yes
   * `iscolor`: yes
   * `iskeyword`: yes
   * `isurl`: yes
   * `ispixel`: yes
   * `isem`: yes
   * `ispercentage`: yes
   * `isunit`: yes
   * `isruleset`: no
   * `rgb`: yes
   * `rgba`: yes
   * `argb`: yes
   * `hsl`: yes
   * `hsla`: yes
   * `hsv`: yes
   * `hsva`: yes
   * `hue`: yes
   * `saturation`: yes
   * `lightness`: yes
   * `hsvue`: yes
   * `hsvsaturation`: yes
   * `hsvvalue`: yes
   * `red`: yes
   * `green`: yes
   * `blue`: yes
   * `alpha`: yes
   * `luma`: yes
   * `luminance`: yes
   * `saturate`: yes
   * `desaturate`: yes
   * `lighten`: yes
   * `darken`: yes
   * `fadein`: yes
   * `fadeout`: yes
   * `fade`: yes
   * `spin`: yes
   * `mix`: yes
   * `tint`: yes
   * `shade`: yes
   * `greyscale`: yes
   * `contrast`: yes
   * `multiply`: yes
   * `screen`: yes
   * `overlay`: yes
   * `softlight`: no
   * `hardlight`: yes
   * `difference`: yes
   * `exclusion`: yes
   * `average`: yes
   * `negation`: no
