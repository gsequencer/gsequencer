#include <sys/types.h>
#include <regex.h>

#define AGS_TURTLE_UTF8_RANGE_0 "([A-Za-z])"
#define AGS_TURTLE_UTF8_RANGE_1 "(\xC3[\x80-\x96])"
#define AGS_TURTLE_UTF8_RANGE_2 "(0xC3[\x98-\xB6])"
#define AGS_TURTLE_UTF8_RANGE_3 "((\xC3[\xB8-\xBF])|([\xC3-\xCA][\x80-\xBF])|(\xCB[\x80-\xBF]))"
#define AGS_TURTLE_UTF8_RANGE_4 "(\xCD[\xB0-\xBD])"
#define AGS_TURTLE_UTF8_RANGE_5 "((\xCD[\xBF-\xDF])|([\xCE-\xDF][\x80-\xBF])|([\xE0-\xE1][\x80\xBF][\x80-\xBF]))"
#define AGS_TURTLE_UTF8_RANGE_6 "(\xE2\x80[\x8C-\x8D])"
#define AGS_TURTLE_UTF8_RANGE_7 "((\xE2\x81[\xB0-\xBF])|(\xE2[\x81-\x85][\x80-\xBF])|(\xE2\x86[\x80-\x8F]))"
#define AGS_TURTLE_UTF8_RANGE_8 "((\xE2[\xB0-\xBE][\x80-\xBF])(\xE2\xBF[\x80-\xAF]))"
#define AGS_TURTLE_UTF8_RANGE_9 "((\xE3[\x80-\xBF][\x81-\xBF])|([\xE4-\xEC][\x80-\x9F][\x80-\xBF]))"
#define AGS_TURTLE_UTF8_RANGE_10 "((\xEF[\xA4-\xB6][\x80-\xBF])|(\xEF\xB7[\x80-\x8F]))"
#define AGS_TURTLE_UTF8_RANGE_11 "((\xEF\xB7[\xB0-\xBF])|(\xEF[\xB8-\xBE][\x80-\xBF])|(\xEF\xBF[\x80-\xBD]))"
#define AGS_TURTLE_UTF8_RANGE_12 "(([\xF0-\xF3][\x90-\xAF][\x80-\xBF][\x80-\xBF]))"

#define AGS_TURTLE_UTF8_RANGE_ALL "(" AGS_TURTLE_UTF8_RANGE_0 "|" \
  AGS_TURTLE_UTF8_RANGE_1 "|" \
  AGS_TURTLE_UTF8_RANGE_2 "|" \
  AGS_TURTLE_UTF8_RANGE_3 "|" \
  AGS_TURTLE_UTF8_RANGE_4 "|" \
  AGS_TURTLE_UTF8_RANGE_5 "|" \
  AGS_TURTLE_UTF8_RANGE_6 "|" \
  AGS_TURTLE_UTF8_RANGE_7 "|" \
  AGS_TURTLE_UTF8_RANGE_8 "|" \
  AGS_TURTLE_UTF8_RANGE_9 "|" \
  AGS_TURTLE_UTF8_RANGE_10 "|" \
  AGS_TURTLE_UTF8_RANGE_11 "|" \
  AGS_TURTLE_UTF8_RANGE_12 ")"

#define AGS_TURLTE_UTF8_RANGE_ALL_PATTERN "^" AGS_TURTLE_UTF8_RANGE_ALL "*"

static const char *test_string = "@prefix : <http://lv2plug.in/ns/lv2core#> .\0";

int
main(int argc, char **argv)
{
  regmatch_t match_arr[1];
    
  char *str;
  
  static regex_t chars_base_regex;
  static regex_t prefix_id_regex;
  static int regex_compiled = 0;

  static const char *prefix_id_pattern = "^(@prefix)\0";  
  static const char *chars_base_pattern = AGS_TURLTE_UTF8_RANGE_ALL_PATTERN;

  static const size_t max_matches = 1;

  int success = 0;
  
  regcomp(&prefix_id_regex, prefix_id_pattern, REG_EXTENDED);
  regcomp(&chars_base_regex, chars_base_pattern, REG_EXTENDED);

  str = test_string;
  
  if(regexec(&prefix_id_regex, str, max_matches, match_arr, 0) == 0){
    str += (match_arr[0].rm_eo + 1);
  }else{
    success = -1;
  }
  
  if(regexec(&chars_base_regex, str, max_matches, match_arr, 0) == 0){
    str += (match_arr[0].rm_eo + 1);
  }else{
    success = -1;
  }
  
  return(0);
}
