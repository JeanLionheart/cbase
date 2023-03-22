#ifndef __REGEXH__
#define __REGEXH__

/* 

1. [xyz] ==> x|y|z ; [a-zA-z0-9] ==> "a|b|c...|z and A|B|...|Z and 0|1|2...|9"

2. x+ ==> "xx.." or "x" ; x*==> "xx.." or ""

3. . ==> any single char

4. [^abc] ==> any char but not a|b|c

  */

int regex_match(const char*regex,const char*src);

#endif