#include <list>
#include <gtest/gtest.h>
#include <less/less/LessParser.h>
#include <less/lessstylesheet/MixinException.h>

class LessParserTest : public ::testing::Test {
public:
  istringstream* in;
  LessTokenizer* t;
  std::list<const char*>* sources;
  LessParser* p;

  LessStylesheet* less;
  Stylesheet* css;
  ProcessingContext* context;
  ostringstream* out;
  CssWriter* writer;


  virtual void SetUp() {
    in = new istringstream(" ");
    t = new LessTokenizer(*in, "test");
    sources = new std::list<const char*>;
    p = new LessParser(*t, *sources);

    less = new LessStylesheet();
    css = new Stylesheet();
    context = new ProcessingContext();
    out = new ostringstream();
    writer = new CssWriter(*out);
  }
  virtual void TearDown() {
    delete p;
    delete sources;
    delete t;
    delete in;
    delete less;
    delete css;
    delete context;
    delete writer;
    delete out;
  }
  
};

TEST_F(LessParserTest, Variable) {
  in->str("@blue: #5B83AD; \
#header { color: @blue; }");
  
  p->parseStylesheet(*less);
  less->process(*css, context);
  css->write(*writer);
  ASSERT_STREQ("#header{color:#5B83AD}", out->str().c_str());
}

TEST_F(LessParserTest, DeepVariable) {
  in->str("@var: value; \
@x: 'var'; \
.class { \
   content: @@x; \
}");

  p->parseStylesheet(*less);
  less->process(*css, context);
  css->write(*writer);
  ASSERT_STREQ(".class{content:value}", out->str().c_str());
}

TEST_F(LessParserTest, Mixin) {
  in->str(".bordered { \
    border: dotted 1px black;\
 }\
 #menu a { \
   color: #111; \
   .bordered; \
}");

  p->parseStylesheet(*less);
  less->process(*css, context);
  css->write(*writer);
  ASSERT_STREQ(".bordered{border:dotted 1px black}\
#menu a{color:#111;border:dotted 1px black}", out->str().c_str());
}

TEST_F(LessParserTest, ParameterMixin) {
  in->str("@radius: 5px; \
  .border-radius(@radius) { \
     border-radius: @radius; \
   } \
   #header { \
     .border-radius(4px ); \
   } \
   .button { \
     .border-radius (6px);  \
   }");

  p->parseStylesheet(*less);
  less->process(*css, context);
  css->write(*writer);
  ASSERT_STREQ("#header{border-radius:4px}\
.button{border-radius:6px}", out->str().c_str());
}

TEST_F(LessParserTest, HiddenMixin) {
  in->str(".wrap () { \
  text-wrap: wrap;\
 }\
pre { .wrap  }");

  p->parseStylesheet(*less);
  less->process(*css, context);
  css->write(*writer);
  ASSERT_STREQ("pre{text-wrap:wrap}", out->str().c_str());
}

TEST_F(LessParserTest, DefaultArguments) {
  in->str(".mixin(@color) { \
  color-1: @color; \
 }\
.mixin(@color; @padding:2) { \
  color-2: @color; \
  padding-2: @padding; \
 }\
.mixin(@color; @padding; @margin: 2) { \
  color-3: @color; \
  padding-3: @padding; \
 margin: @margin @margin @margin @margin; \
 }\
.some .selector div { \
  .mixin(#008000);\
 }\
");
  p->parseStylesheet(*less);
  less->process(*css, context);
  css->write(*writer);
  ASSERT_STREQ(".some .selector div{color-1:#008000;color-2:#008000;\
padding-2:2}", out->str().c_str());
}

TEST_F(LessParserTest, ParameterMatching) {
  in->str(".mixin2 (@a) { \
   color: @a; \
} \
.mixin2 (@a; @b) { \
  color: fadeout(@a; @b); \
} \
.test mixin2a { \
  .mixin2(#000); \
} \
.test mixin2b { \
  .mixin2(#000, 50%); \
}");

  p->parseStylesheet(*less);
  less->process(*css, context);
  css->write(*writer);
  ASSERT_STREQ(".test mixin2a{color:#000}.test mixin2b{color:rgba(0, 0, 0, 0.5)}", out->str().c_str());
}

TEST_F(LessParserTest, Arguments) {
  in->str(".box-shadow (@x: 0, @y: 0, @blur: 1px, @color: #000) { \
  box-shadow: @arguments; \
} \
.box-shadow2 (@x, @y, @blur, @color) { \
  box-shadow: @arguments; \
}\
.box-shadow3 (@x; @y; @blur: 1px; @color: #000) { \
  box-shadow: @arguments; \
 } \
.class { \
  .box-shadow(2px, 5px); \
  .box-shadow2(4px, 10px, 2px, #000); \
  .box-shadow3(8px, 20px); \
} ");
  p->parseStylesheet(*less);
  less->process(*css, context);
  css->write(*writer);
  ASSERT_STREQ(".class{box-shadow:2px 5px 1px #000;\
box-shadow:4px 10px 2px #000;\
box-shadow:8px 20px 1px #000}", out->str().c_str());
}

TEST_F(LessParserTest, RestArgument) {
  in->str(".mixin3 (...) { \
 border1: 1px #000 solid; \
}\
.mixin3 (@a: 1px; ...) { \
 border2: @a #000 solid; \
 } \
.mixin3 (@rest...) { \
 border3: @rest; \
 } \
\
.test mixin3 { \
  .mixin3(4px, #456, dashed); \
}");
  
  p->parseStylesheet(*less);
  less->process(*css, context);
  css->write(*writer);
  ASSERT_STREQ(".test mixin3{border1:1px #000 solid;\
border2:4px #000 solid;border3:4px #456 dashed}", out->str().c_str());
}

TEST_F(LessParserTest, NestedRulesets) {
  in->str("#header { \
color: black; \
\
.navigation { \
  font-size: 12px; \
 }\
.logo { \
 width: 300px; \
  &:hover { text-decoration: none } \
\
  &:hover, \
      &:focus{ text-decoration: none } \
\
  h2 a { \
  height: 10px; \
  } \
 a:hover { \
    text-decoration: underline; \
  } \
  div, span { \
  color: #FFF; \
  } \
 } \
}");
  p->parseStylesheet(*less);
  less->process(*css, context);
  css->write(*writer);
  ASSERT_STREQ("#header{color:black}#header .navigation{font-size:12px}#header .logo{width:300px}#header .logo:hover{text-decoration:none}#header .logo:hover,#header .logo:focus{text-decoration:none}#header .logo h2 a{height:10px}#header .logo a:hover{text-decoration:underline}#header .logo div,#header .logo span{color:#FFF}", out->str().c_str());
}

TEST_F(LessParserTest, PrefixTest) {
  in->str("#header { \
.class1, .class2 { \
  a { \
    font-weight: bold; \
  } \
 } \
}");
  p->parseStylesheet(*less);
  less->process(*css, context);
  css->write(*writer);
  ASSERT_STREQ("#header .class1 a,#header .class2 a{font-weight:bold}", out->str().c_str());
} 

TEST_F(LessParserTest, Operations) {
  in->str("@base: 5%; \
@filler: @base * 2; \
@other: @base + @filler; \
@width: -1px + 10px; \
@number: 10; \
@emUnits: 5em + unit(@width, em); \
\
.class { \
  background-color: @base + #111; \
  width: (@width + 5) * 2; \
  border: @width * 2 solid black; \
  height: 100% / 2 + @filler; \
  top: @emUnits; \
  \
  @scale: 4; \
  @defaultFontSize: 2; \
  @headerHeight: 75px; \
    \
  transform:rotate(45deg); \
  left:unit(-@defaultFontSize/2.2*@scale, px); \
}");

  p->parseStylesheet(*less);
  less->process(*css, context);
  css->write(*writer);
  ASSERT_STREQ(".class{background-color:#161616;width:28px;border:18px solid black;height:60%;top:14em;transform:rotate(45deg);left:-3.636363636px}", out->str().c_str());
} 
  

TEST_F(LessParserTest, ColorOps) {
  in->str("@color: #d41212; \
.class { \
  color: lighten(@color, 10%);\
  color: darken(@color, 10%);\
  color: saturate(@color, 10%); \
  color: desaturate(@color, 10%); \
  \
  color: fadein(@color, 10%); \
  color: fadeout(@color, 10%); \
  \
  color: spin(@color, 10); \
  color: spin(@color, -10); \
  color: hsl(hue(@color), 45%, 90%); \
  clip: rect(1px, 1px, 1px, 1px); \
} ");

  p->parseStylesheet(*less);
  less->process(*css, context);
  css->write(*writer);
  ASSERT_STREQ(".class{color:#ed2c2c;color:#a50e0e;color:#e00606;color:#c91d1d;color:#d41212;color:rgba(212, 18, 18, 0.9);color:#d43212;color:#d41232;color:#f1dada;clip:rect(1px, 1px, 1px, 1px)}", out->str().c_str());
} 

TEST_F(LessParserTest, Namespace) {
  in->str("#bundle { \
.button () { \
  display: block; \
  border: 1px solid black; \
  background-color: grey; \
  &:hover { background-color: white } \
  span { \
    a { \
      display: none; \
    } \
  } \
 } \
} \
\
#header a { \
  color: orange; \
  #bundle .button; \
} \
#header2 a { \
  #bundle > .button; \
}");
  p->parseStylesheet(*less);
  less->process(*css, context);
  css->write(*writer);
  ASSERT_STREQ("#header a{color:orange;display:block;border:1px solid black;background-color:grey}#header a:hover{background-color:white}#header a span a{display:none}#header2 a{display:block;border:1px solid black;background-color:grey}#header2 a:hover{background-color:white}#header2 a span a{display:none}", out->str().c_str());
} 
  
TEST_F(LessParserTest, VariableScope) {
  in->str("@scopevar: red; \
\
#page { \
  @scopevar: white; \
  #header { \
    color: @scopevar; \
  } \
} \
#footer { \
  color: @scopevar; \
}");
  p->parseStylesheet(*less);
  less->process(*css, context);
  css->write(*writer);
  ASSERT_STREQ("#page #header{color:white}#footer{color:red}", out->str().c_str());
} 

TEST_F(LessParserTest, CssComment) {
  in->str("/* comments */");

  p->parseStylesheet(*less);
  less->process(*css, context);
  css->write(*writer);
  ASSERT_STREQ("/* comments */\n", out->str().c_str());
} 
  
TEST_F(LessParserTest, LessComment) {
  in->str("// comment ");
  LessStylesheet stylesheet;

  p->parseStylesheet(stylesheet);
  p->parseStylesheet(*less);
  less->process(*css, context);
  css->write(*writer);
  ASSERT_STREQ("", out->str().c_str());
} 

/*
TEST_F(LessParserTest, Import) {
  in->str("@import \"lib.less\"; \
@import \"lib.css\"; ");
  LessStylesheet stylesheet;

  p->parseStylesheet(stylesheet);
} 
*/
TEST_F(LessParserTest, UrlInterpolation) {
  in->str("@base-url: \"http://assets.fnord.com\"; \
.class { \
  background-image: url(\"@{base-url}/images/bg.png\"); \
}");
  p->parseStylesheet(*less);
  less->process(*css, context);
  css->write(*writer);
  ASSERT_STREQ(".class{background-image:url(\"http://assets.fnord.com/images/bg.png\")}", out->str().c_str());
} 

TEST_F(LessParserTest, Escape) {
  in->str(".class { \
filter: ~\"progid:DXImageTransform.Microsoft.AlphaImageLoader(src='image.png')\"; \
} ");
  p->parseStylesheet(*less);
  less->process(*css, context);
  css->write(*writer);
  ASSERT_STREQ(".class{filter:progid:DXImageTransform.Microsoft.AlphaImageLoader(src='image.png')}", out->str().c_str());
} 

TEST_F(LessParserTest, NotAFunction) {
  in->str("mixin { \
  background-image: linear-gradient(bottom, rgb(232,63,25) 0%, \
                                    rgb(255,130,95) 50%); \
} ");
  p->parseStylesheet(*less);
  less->process(*css, context);
  css->write(*writer);
  ASSERT_STREQ("mixin{background-image:linear-gradient(bottom, #e83f19 0%, #ff825f 50%)}", out->str().c_str());
}

TEST_F(LessParserTest, Minus) {
  in->str("mixin { \
 width: 10-1; \
}");
  p->parseStylesheet(*less);
  less->process(*css, context);
  css->write(*writer);
  ASSERT_STREQ("mixin{width:10-1}", out->str().c_str());
} 

TEST_F(LessParserTest, Equals) {
  in->str("mixin { \
 x: 4 = 1; \
}");
  p->parseStylesheet(*less);
  less->process(*css, context);
css->write(*writer);
ASSERT_STREQ("mixin{x:4 = 1}", out->str().c_str());
}

TEST_F(LessParserTest, NotAFunction2) {
  in->str("body { \
 filter:progid:DXImageTransform.Microsoft.gradient(enabled='false',startColorstr=#57ba52,endColorstr=#2c9027,GradientType=0); \
} ");

  p->parseStylesheet(*less);
  less->process(*css, context);
  css->write(*writer);
ASSERT_STREQ("body{filter:progid:DXImageTransform.Microsoft.gradient(enabled='false',startColorstr=#57ba52,endColorstr=#2c9027,GradientType=0)}", out->str().c_str());
}

TEST_F(LessParserTest, PropertyAsterisk) {
  in->str("video { \
  *zoom: 1; \
} ");
  p->parseStylesheet(*less);
  less->process(*css, context);
  css->write(*writer);
  ASSERT_STREQ("video{*zoom:1}", out->str().c_str());
}

TEST_F(LessParserTest, NotAFunction3) {
  in->str("audio:not([controls]) { \
 display: none; \
}");
  p->parseStylesheet(*less);
  less->process(*css, context);
  css->write(*writer);
  ASSERT_STREQ("audio:not([controls]){display:none}", out->str().c_str());
}
  
TEST_F(LessParserTest, MixinGuards) {
  in->str(".mixin5 (@a) when (lightness(@a) >= 50%) { \
  background-color: black; \
} \
.mixin5 (@a) when (lightness(@a) < 50%) { \
  background-color: white; \
} \
.mixin5 (@a) { \
  color: @a; \
} \
.class1 { .mixin5(#ddd) } \
.class2 { .mixin5(#555) } ");

  p->parseStylesheet(*less);
  less->process(*css, context);
  css->write(*writer);
  ASSERT_STREQ(".class1{background-color:black;color:#ddd}.class2{background-color:white;color:#555}", out->str().c_str());
}

TEST_F(LessParserTest, MixinGuards2) {
  in->str(".mixin5a (@b, @c) when (@b = string) { \
  width: @c; \
} \
.class1a { \
  .mixin5a(string, 10px); \
 } ");

  p->parseStylesheet(*less);
  less->process(*css, context);
  css->write(*writer);
  ASSERT_STREQ(".class1a{width:10px}", out->str().c_str());
} 

TEST_F(LessParserTest, MultipleMixinCalls) {
  in->str(".test1 (@FooterHeight) { \
  test1: 1px; \
    \
  footer, \
  .push { \
    height: @FooterHeight; \
  } \
} \
\
.test2 (@H_H, @F_H)  { \
   test2: 1px; \
  .test1(@F_H); \
  \
  header { \
    height: @H_H; \
  }\
}\
body { \
  .test2(90px, 60px); \
}");

  p->parseStylesheet(*less);
  less->process(*css, context);
  css->write(*writer);
  ASSERT_STREQ("body{test2:1px;test1:1px}body footer,body .push{height:60px}body header{height:90px}", out->str().c_str());
}

TEST_F(LessParserTest, MediaQuery) {
  in->str("@blue: #00F;\
.test (@height) { h1 { height: @height } }\
@media (width: 400px) {\
  .test6 { \
  width: 400px; \
  color: @blue; \
  .test(1px); \
   \
  .test7 { \
     width: 100px; \
  } \
 } \
 .test(10px); \
}");
  p->parseStylesheet(*less);
  less->process(*css, context);
  css->write(*writer);
  ASSERT_STREQ("@media (width: 400px){.test6{width:400px;color:#00F}.test6 h1{height:1px}.test6 .test7{width:100px}h1{height:10px}}", out->str().c_str());
}

TEST_F(LessParserTest, InterpolateSelector) {
  in->str("@name: \"name\"; \
.@{name}{ \
  color: #000; \
}");
  p->parseStylesheet(*less);
  less->process(*css, context);
  css->write(*writer);
  ASSERT_STREQ(".name{color:#000}", out->str().c_str());
}
TEST_F(LessParserTest, InterpolateProperty) {
  in->str("@name: \"name\"; \
.test { \
  property-@{name}: 10px; \
}");
  p->parseStylesheet(*less);
  less->process(*css, context);
  css->write(*writer);
  ASSERT_STREQ(".test{property-name:10px}", out->str().c_str());
}
TEST_F(LessParserTest, InterpolateNestedRuleset) {
  in->str(".mixin6(@name2, @color) { \
  .@{name2} { \
    color: @color; \
  } \
} \
.class {\
   .mixin6('name', #FFF);\
}");
  p->parseStylesheet(*less);
  less->process(*css, context);
  css->write(*writer);
  ASSERT_STREQ(".class .name{color:#FFF}", out->str().c_str());
} 

TEST_F(LessParserTest, Loop) {
  in->str(".loop(@counter) when (@counter > 0) { \
   .loop((@counter - 1)); \
   width: (10px * @counter); \
} \
div { \
  .loop(5); \
}");

  p->parseStylesheet(*less);
  less->process(*css, context);
  css->write(*writer);
  ASSERT_STREQ("div{width:10px;width:20px;width:30px;width:40px;width:50px}", out->str().c_str());
} 

TEST_F(LessParserTest, NestedLoop) {
  in->str(".test-nested-loop () { \
   @name: ~\"abc\"; \
    \
  .nested-loop(@name, @counter) when (@counter > 0) { \
    .nested-loop-@{name} { \
      c: @counter; \
    } \
    .nested-loop(~\"@{name}-@{counter}\", @counter - 1); \
  } \
  \
  .nested-loop(@name, 3); \
 } \
.test-nested-loop();");
  p->parseStylesheet(*less);
  less->process(*css, context);
  css->write(*writer);
  ASSERT_STREQ(".nested-loop-abc-3-2{c:1}.nested-loop-abc-3{c:2}.nested-loop-abc{c:3}", out->str().c_str());
}

TEST_F(LessParserTest, NestedMedia) {
  in->str(".test8 { \
  @media screen { \
    width: 300px; \
    @media print { \
      color: #000; \
    } \
  } \
 }");
  p->parseStylesheet(*less);
  less->process(*css, context);
  css->write(*writer);
  ASSERT_STREQ("@media screen{.test8{width:300px}}@media screen and print{.test8{color:#000}}", out->str().c_str());
 }

TEST_F(LessParserTest, ExtendSelector) {
  in->str(".test9:extend(.test10) { \
  color: #FFF; \
} \
.test10, \
.test10a{ \
   background-color: #000; \
}");
  p->parseStylesheet(*less);
  less->process(*css, context);
  css->write(*writer);
  ASSERT_STREQ(".test9{color:#FFF}.test10,.test10a,.test9{background-color:#000}", out->str().c_str());
}

TEST_F(LessParserTest, ExtendStatement) {
  in->str(".test10b { \
   background-color: #111; \
} \
.test11 { \
  &:extend(.test10b); \
  width: 100px; \
}");
  p->parseStylesheet(*less);
  less->process(*css, context);
  css->write(*writer);
  ASSERT_STREQ(".test10b,.test11{background-color:#111}.test11{width:100px}", out->str().c_str());
}

TEST_F(LessParserTest, ExtendAll) {
  in->str(".test11b:a, \
div.test11b { \
  height: 1px; \
} \
.extension11b { \
  &:extend(.test11b all); \
}");
  p->parseStylesheet(*less);
  less->process(*css, context);
  css->write(*writer);
  ASSERT_STREQ(".test11b:a,div.test11b,.extension11b:a,div.extension11b{height:1px}", out->str().c_str());
} 

TEST_F(LessParserTest, DefaultFunctionArguments) {
  in->str(".test12 (@c: rgb(1,2,3); @b: 4) { \
  color: @c;\
} \
.mixin12 { \
  .test12(); \
}");

  p->parseStylesheet(*less);
  less->process(*css, context);
  css->write(*writer);
  ASSERT_STREQ(".mixin12{color:#010203}", out->str().c_str());
}

TEST_F(LessParserTest, NamedArguments) {
  in->str(".test13(@start-color: #555; @end-color: #333; \
  @start-percent: 0%; @end-percent: 100%) { \
  background-image: -webkit-linear-gradient(left, @start-color @start-percent, \
@end-color @end-percent); \
 } \
.mixin13 { \
  .test13(@end-color: #000, @start-color: #FFF); \
 }");

  p->parseStylesheet(*less);
  less->process(*css, context);
  css->write(*writer);
  ASSERT_STREQ(".mixin13{background-image:-webkit-linear-gradient(left, #FFF 0%, #000 100%)}", out->str().c_str());
}

TEST_F(LessParserTest, SelectorAmpReplacement) {
  in->str(".test14 { \
  &:hover { \
    color: #FFF; \
  } \
  p & { \
    color: #123; \
  } \
  &.class > &[checked] { \
    color: #432; \
  } \
 }");
  p->parseStylesheet(*less);
  less->process(*css, context);
  css->write(*writer);
  ASSERT_STREQ(".test14:hover{color:#FFF}p .test14{color:#123}.test14.class > .test14[checked]{color:#432}", out->str().c_str());
}

TEST_F(LessParserTest, EscapedStringArgument) {
  in->str(".test15(@x) { \
  width: @x; \
}\
.class15 { \
  .test15(~\"1px solid blue\"); \
}");

  p->parseStylesheet(*less);
  less->process(*css, context);
  css->write(*writer);
  ASSERT_STREQ(".class15{width:1px solid blue}", out->str().c_str());
}

TEST_F(LessParserTest, UrlImport) {
  in->str("@import url(\"http://fonts.googleapis.com/css?family=Ubuntu:400,700,400italic\");");

  p->parseStylesheet(*less);
  less->process(*css, context);
  css->write(*writer);
  ASSERT_STREQ("@import url(\"http://fonts.googleapis.com/css?family=Ubuntu:400,700,400italic\");", out->str().c_str());
}

TEST_F(LessParserTest, NotAFunction4) {
  in->str(".icon.dim { opacity: 0.3;filter:alpha(opacity=30); }");

  p->parseStylesheet(*less);
  less->process(*css, context);
  css->write(*writer);
  ASSERT_STREQ(".icon.dim{opacity:0.3;filter:alpha(opacity=30)}", out->str().c_str());
}

TEST_F(LessParserTest, Comments) {
  in->str("div /* a */ .class { \
  /* comment */ \
  color: /* b */ #fff /* c */; \
}");

  p->parseStylesheet(*less);
  less->process(*css, context);
  css->write(*writer);
  ASSERT_STREQ("div  .class{\n/* comment */\n;color:#fff}", out->str().c_str());
}

TEST_F(LessParserTest, EmptyStatement) {
  in->str("  ;");

  p->parseStylesheet(*less);
  less->process(*css, context);
  css->write(*writer);
  ASSERT_STREQ("", out->str().c_str());
}

TEST_F(LessParserTest, NestedAtRule) {
  in->str(".declare-font(@type) { \
  @font-face { \
    font-family: @type; \
    src: url('fonts/@{type}.eot'); \
  } \
 } \
.declare-font('test'); ");

  p->parseStylesheet(*less);
  less->process(*css, context);
  css->write(*writer);
  ASSERT_STREQ("@font-face { font-family: \"test\" ; src: url('fonts/test.eot') ; }", out->str().c_str());
}

TEST_F(LessParserTest, ReturnMixin) {
  in->str(".unlock(@value) { \
  .doSomething() { \
     declaration: @value; \
  } \
} \
#namespace { \
  .unlock(5); \
  .doSomething(); \
}");

  p->parseStylesheet(*less);
  less->process(*css, context);
  css->write(*writer);
  ASSERT_STREQ("#namespace{declaration:5}", out->str().c_str());
}
  
TEST_F(LessParserTest, NotAParameterMixin) {
  in->str("non-parameter-mixin(odd) { \
  color: #000; \
}");

  p->parseStylesheet(*less);
  less->process(*css, context);
  css->write(*writer);
  ASSERT_STREQ("non-parameter-mixin(odd){color:#000}", out->str().c_str());
}

TEST_F(LessParserTest, HashMixin) {
  in->str("#hash-mixin () { \
   color: #fff; \
}");

  p->parseStylesheet(*less);
  less->process(*css, context);
  css->write(*writer);
  ASSERT_STREQ("", out->str().c_str());
}

TEST_F(LessParserTest, SameNameMixin) {
  in->str(".same-name() { \
   height: 100%; \
} \
#namespace { \
.same-name { \
  .same-name(); \
 } \
}");

  p->parseStylesheet(*less);
  less->process(*css, context);
  css->write(*writer);
  ASSERT_STREQ("#namespace .same-name{height:100%}", out->str().c_str());
}

TEST_F(LessParserTest, OperatorOrder) {
  in->str(".conditional(@x) when (@x =< 10 + 1) { \
  width: 100%; \
} \
.test { \
  .conditional(10); \
}");

  p->parseStylesheet(*less);
  less->process(*css, context);
  css->write(*writer);
  ASSERT_STREQ(".test{width:100%}", out->str().c_str());
}

TEST_F(LessParserTest, CommentAtEndOfString) {
  in->str(" \
/* \
**/");
  
  p->parseStylesheet(*less);
  less->process(*css, context);
  css->write(*writer);
  ASSERT_STREQ("/* **/\n", out->str().c_str());
}

TEST_F(LessParserTest, NamespaceGuardTrue) {
  in->str("#namespace when (@x = true) { \
   .mixin { \
     x: x; \
   } \
} \
@x: true;\
.test { \
  #namespace .mixin;\
}");
  
  p->parseStylesheet(*less);
  less->process(*css, context);
  css->write(*writer);
  ASSERT_STREQ("#namespace .mixin{x:x}.test{x:x}", out->str().c_str());
}
TEST_F(LessParserTest, NamespaceGuardFalse) {
  in->str("#namespace when (@x = true) { \
   .mixin { \
     x: x; \
   } \
} \
@x: false;\
.test { \
  #namespace .mixin;\
}");
  
  p->parseStylesheet(*less);
  EXPECT_THROW(less->process(*css, context), MixinException*);
}

TEST_F(LessParserTest, Default) {
  in->str(".mixin(@x) when (@x = 2) { \
  a: @x; \
}\
.mixin (@x) when (default()) { \
  default: @x; \
} \
.test { \
  .mixin(1); \
}");
  
  p->parseStylesheet(*less);
  less->process(*css, context);
  css->write(*writer);
  ASSERT_STREQ(".test{default:1}", out->str().c_str());
}

TEST_F(LessParserTest, NotDefault) {
  in->str(".mixin(@x) when (@x = 1) { \
  a: @x; \
}\
.mixin (@x) when (default()) { \
  default: @x; \
} \
.test { \
  .mixin(1); \
}");
  
  p->parseStylesheet(*less);
  less->process(*css, context);
  css->write(*writer);
  ASSERT_STREQ(".test{a:1}", out->str().c_str());
}

TEST_F(LessParserTest, ImportantMixin) {
  in->str(".mixin(@x) { \
  x: @x; \
} \
.test { \
  .mixin(1) !important; \
}");
  
  p->parseStylesheet(*less);
  less->process(*css, context);
  css->write(*writer);
  ASSERT_STREQ(".test{x:1 !important}", out->str().c_str());
}

TEST_F(LessParserTest, MergeComma) {
  in->str(".mixin(@x) { \
  x+: @x; \
} \
.test { \
  x+: 4; \
  .mixin(1); \
}");
  
  p->parseStylesheet(*less);
  less->process(*css, context);
  css->write(*writer);
  ASSERT_STREQ(".test{x:4, 1}", out->str().c_str());
}

TEST_F(LessParserTest, MergeSpace) {
  in->str(".mixin(@x) { \
  x+_: @x; \
} \
.test { \
  x+: 4; \
  .mixin(1); \
}");
  
  p->parseStylesheet(*less);
  less->process(*css, context);
  css->write(*writer);
  ASSERT_STREQ(".test{x:4 1}", out->str().c_str());
}

TEST_F(LessParserTest, SwitchArgument) {
  in->str(".mixin(x) { \
  x: 1; \
} \
.test { \
  .mixin(x); \
}");
  
  p->parseStylesheet(*less);
  less->process(*css, context);
  css->write(*writer);
  ASSERT_STREQ(".test{x:1}", out->str().c_str());
}

TEST_F(LessParserTest, SwitchArgumentMisMatch) {
  in->str(".mixin(x) { \
  x: 1; \
} \
.test { \
  .mixin(y); \
}");
  
  p->parseStylesheet(*less);
  EXPECT_THROW(less->process(*css, context), MixinException*);
}

TEST_F(LessParserTest, NotGuard) {
  in->str(".mixin(@x) when not (@x = 2) { \
  x: 1; \
} \
.test { \
  .mixin(1); \
}");
  
  p->parseStylesheet(*less);
  less->process(*css, context);
  css->write(*writer);
  ASSERT_STREQ(".test{x:1}", out->str().c_str());
}

TEST_F(LessParserTest, DetachedRuleset) {
  in->str("@x: { \
    x:1; \
  }; \
  .test { \
    @x(); \
  }");
  
  p->parseStylesheet(*less);
  less->process(*css, context);
  css->write(*writer);
  ASSERT_STREQ(".test{x:1}", out->str().c_str());
}

TEST_F(LessParserTest, AttributeSelectorMatch) {
  in->str(".x[a='b'] { \
  x: x; \
} \
.y { \
  &:extend(.x[a=\"b\"]); \
  y:y; \
}");
  
  p->parseStylesheet(*less);
  less->process(*css, context);
  css->write(*writer);
  ASSERT_STREQ(".x[a='b'],.y{x:x}.y{y:y}", out->str().c_str());
}

TEST_F(LessParserTest, ExtendInterpolatedSelector) {
  in->str(".x { \
  x:x; \
} \
@{var}:extend(.x) { \
  &:extend(.x); \
} \
@var:.y;");
  
  p->parseStylesheet(*less);
  less->process(*css, context);
  css->write(*writer);
  ASSERT_STREQ(".x,.y,.y{x:x}", out->str().c_str());
}

TEST_F(LessParserTest, ExtendOutsideMedia) {
  in->str("@media print { \
  .screenClass:extend(.selector) {} \
  .selector {  \
    color: black; \
  } \
} \
.selector { \
  color: red; \
} \
@media screen { \
  .selector { \
    color: blue; \
  } \
}");
  
  p->parseStylesheet(*less);
  less->process(*css, context);
  css->write(*writer);
  ASSERT_STREQ("@media print{.selector,.screenClass{color:black}}\
.selector{color:red}@media screen{.selector{color:blue}}", out->str().c_str());
}

TEST_F(LessParserTest, ExtendNestedMedia) {
  in->str("@media screen {\
  .screenClass:extend(.selector) {} \
  @media (min-width: 1023px) {\
    .selector { \
      color: blue; \
    } \
  } \
}");
  
  p->parseStylesheet(*less);
  less->process(*css, context);
  css->write(*writer);
  ASSERT_STREQ("@media screen{}@media screen and (min-width: 1023px)\
{.selector{color:blue}}", out->str().c_str());
}

