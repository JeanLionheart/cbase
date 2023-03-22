// #include "array.h"
// #include <list.h>
// #include <regex.h>
// #include <string.h>
// /*

// 1. [xyz] ==> x|y|z ; [a-zA-z0-9] ==> "a|b|c...|z and A|B|...|Z and 0|1|2...|9"

// 2. x+ ==> "xx.." or "x" ; x*==> "xx.." or ""

// 3. . ==> any single char

// 4. [^abc] ==> any char but not a|b|c

// */

// /* private struct regex */

// /* analyse the regex; add brackets into it to keep calculate-order */
// void regex_parse(const char* regex_dst, const char* regex_src);

// int regex_match(const char* regex, const char* src);

// int regex_bracket_match(const char* bracket,
//     const char* match_start); // for []

// int regex_plus_match(const char* match_start);

// /* . and * can match any single; *can match any str */

// void regex_parse(const char* regex_dst, const char* regex_src)
// {
//     regex_dst = 0;
// }

// int regex_match(const char* regex, const char* src)
// {
//     array_t reg_array = array_construct(sizeof(const char*));
//     int src_match[strlen(src)];
//     return 1;
// }