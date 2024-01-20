/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2024 Joël Krähemann
 *
 * This file is part of GSequencer.
 *
 * GSequencer is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * GSequencer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GSequencer.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <ags/lib/ags_regex_util.h>

#include <stdlib.h>
#include <string.h>

/**
 * SECTION:ags_regex_util
 * @short_description: regex util with locale conversion
 * @title: AgsRegexUtil
 * @section_id:
 * @include: ags/lib/ags_regex_util.h
 *
 * Wrapper around glibc regex functions to provide regex with locale conversion. 
 */

GType
ags_regex_util_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_regex_util = 0;

    ags_type_regex_util =
      g_boxed_type_register_static("AgsRegexUtil",
				   (GBoxedCopyFunc) ags_regex_util_copy,
				   (GBoxedFreeFunc) ags_regex_util_free);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_regex_util);
  }

  return g_define_type_id__volatile;
}

GType
ags_regex_util_compile_flags_get_type()
{
  static volatile gsize g_flags_type_id__volatile;

  if(g_once_init_enter (&g_flags_type_id__volatile)){
    static const GFlagsValue values[] = {
      { AGS_REGEX_UTIL_POSIX_EXTENDED_SYNTAX, "AGS_REGEX_UTIL_POSIX_EXTENDED_SYNTAX", "regex-util-posix-extended-syntax" },
      { AGS_REGEX_UTIL_CASE_INSENSITIVE, "AGS_REGEX_UTIL_CASE_INSENSITIVE", "regex-util-case-insensitive" },
      { AGS_REGEX_UTIL_NO_POSITION_REPORT_OF_MATCHES, "AGS_REGEX_UTIL_NO_POSITION_REPORT_OF_MATCHES", "regex-util-no-position-report-of-matches" },
      { AGS_REGEX_UTIL_NO_NEWLINE_MATCH, "AGS_REGEX_UTIL_NO_NEWLINE_MATCH", "regex-util-no-newline-match" },
      { 0, NULL, NULL }
    };

    GType g_flags_type_id = g_flags_register_static(g_intern_static_string("AgsRegexUtilCompileFlags"), values);

    g_once_init_leave (&g_flags_type_id__volatile, g_flags_type_id);
  }
  
  return g_flags_type_id__volatile;
}

GType
ags_regex_util_execute_flags_get_type()
{
  static volatile gsize g_flags_type_id__volatile;

  if(g_once_init_enter (&g_flags_type_id__volatile)){
    static const GFlagsValue values[] = {
      { AGS_REGEX_UTIL_NOT_BEGINNING_OF_LINE, "AGS_REGEX_UTIL_NOT_BEGINNING_OF_LINE", "regex-util-not-beginning-of-line" },
      { AGS_REGEX_UTIL_NOT_END_OF_LINE, "AGS_REGEX_UTIL_NOT_END_OF_LINE", "regex-util-end-of-line" },
      { AGS_REGEX_UTIL_START_END, "AGS_REGEX_UTIL_START_END", "regex-util-start-end" },
      { 0, NULL, NULL }
    };

    GType g_flags_type_id = g_flags_register_static(g_intern_static_string("AgsRegexUtilExecuteFlags"), values);

    g_once_init_leave (&g_flags_type_id__volatile, g_flags_type_id);
  }
  
  return g_flags_type_id__volatile;
}

GQuark
ags_regex_util_error_quark()
{
  return(g_quark_from_static_string("ags-regex-util-error-quark"));
}

/**
 * ags_regex_util_alloc:
 * @app_encoding: the application encoding
 * @encoding: the input encoding
 *
 * Allocate #AgsRegexUtil-struct
 *
 * Returns: a new #AgsRegexUtil-struct
 *
 * Since: 6.3.2
 */
AgsRegexUtil*
ags_regex_util_alloc(gchar *app_encoding,
		     gchar *encoding,
		     gboolean is_unichar, gboolean is_unichar2)
{
  AgsRegexUtil *ptr;

  gchar *app_localization, *localization;
  
  ptr = (AgsRegexUtil *) g_malloc(sizeof(AgsRegexUtil));

  if(app_encoding == NULL){
    app_encoding = setlocale(LC_ALL,
			     NULL);    
  }

  if(app_encoding == NULL){
    app_encoding = getenv("LANG");    
  }

  if(app_encoding == NULL){
    app_encoding = "C.UTF-8";
  }
  
  ptr->app_encoding = g_strdup(app_encoding);

  ptr->encoding = g_strdup(encoding);

  ptr->is_unichar = is_unichar;
  ptr->is_unichar2 = is_unichar2;

  /* iconv */
  app_localization = strchr(app_encoding, '.');

  if(app_localization == NULL){
    app_localization = app_encoding;
  }else{
    app_localization++;
  }

  localization = strchr(encoding, '.');

  if(localization == NULL){
    localization = encoding;
  }else{
    localization++;
  }

  ptr->converter = (GIConv) -1;

  if((!g_strcmp0(app_localization, localization)) == FALSE){
    ptr->converter = g_iconv_open(app_localization,
				  localization);
  }
  
  ptr->regex_str = NULL;

  ptr->compile_flags = 0;

  return(ptr);
}

/**
 * ags_regex_util_copy:
 * @ptr: the #AgsRegexUtil
 *
 * Create a copy of @ptr.
 *
 * Returns: a pointer of the new #AgsRegexUtil-struct
 *
 * Since: 6.3.2
 */
gpointer
ags_regex_util_copy(AgsRegexUtil *ptr)
{
  AgsRegexUtil *new_ptr;
  
  new_ptr = ags_regex_util_alloc(ptr->app_encoding,
				 ptr->encoding,
				 ptr->is_unichar,
				 ptr->is_unichar2);

  return(new_ptr);
}

/**
 * ags_regex_util_free:
 * @ptr: the #AgsRegexUtil
 *
 * Free the memory of @ptr.
 *
 * Since: 6.3.2
 */
void
ags_regex_util_free(AgsRegexUtil *ptr)
{
  if(ptr == NULL){
    return;
  }
  
  g_free(ptr->app_encoding);
  g_free(ptr->encoding);

  if(ptr->converter != (GIConv) -1){
    g_iconv_close(ptr->converter);
  }
  
  regfree(&(ptr->regex));
  
  g_free(ptr->regex_str);

  g_free(ptr);
}

/**
 * ags_regex_util_get_app_encoding:
 * @regex_util: the #AgsRegexUtil-struct
 * 
 * Get application encoding of @regex_util.
 *
 * Returns: (transfer full): the application encoding
 * 
 * Since: 6.3.2
 */
gchar*
ags_regex_util_get_app_encoding(AgsRegexUtil *regex_util)
{
  gchar *app_encoding;
  
  if(regex_util == NULL){
    return(NULL);
  }

  app_encoding = g_strdup(regex_util->app_encoding);

  return(app_encoding);
}

/**
 * ags_regex_util_get_encoding:
 * @regex_util: the #AgsRegexUtil-struct
 * 
 * Get encoding of @regex_util.
 *
 * Returns: (transfer full): the encoding
 * 
 * Since: 6.3.2
 */
gchar*
ags_regex_util_get_encoding(AgsRegexUtil *regex_util)
{
  gchar *encoding;
  
  if(regex_util == NULL){
    return(NULL);
  }

  encoding = g_strdup(regex_util->encoding);

  return(encoding);
}

/**
 * ags_regex_util_compile:
 * @regex_util: the #AgsRegexUtil
 * @regex_str: the regular expression
 * @compile_flags: the compile flags
 * @error: an error that may occure
 *
 * Compile @regex_str by respecting @compile_flags.
 * 
 * Returns: %TRUE on success, otherwise %FALSE
 *
 * Since: 6.3.2
 */
gboolean
ags_regex_util_compile(AgsRegexUtil *regex_util,
		       const gchar *regex_str, gint compile_flags,
		       GError **error)
{
  int retval;

  if(regex_util == NULL ||
     regex_str == NULL){
    return(FALSE);
  }
  
  retval = regcomp(&(regex_util->regex), regex_str,
		   compile_flags);

  if(retval != 0){
    if(error != NULL){
      switch(retval){
      case REG_BADBR:
	{
	  g_set_error(error,
		      AGS_REGEX_UTIL_ERROR,
		      AGS_REGEX_UTIL_ERROR_BAD_BACK_REFERENCE,
		      "regcomp bad back reference");
	}
	break;
      case REG_BADPAT:
	{
	  g_set_error(error,
		      AGS_REGEX_UTIL_ERROR,
		      AGS_REGEX_UTIL_ERROR_BAD_PATTERN,
		      "regcomp bad pattern");
	}
	break;
      case REG_BADRPT:
	{
	  g_set_error(error,
		      AGS_REGEX_UTIL_ERROR,
		      AGS_REGEX_UTIL_ERROR_BAD_REPETITION,
		      "regcomp bad repetition");
	}
	break;
      case REG_EBRACE:
	{
	  g_set_error(error,
		      AGS_REGEX_UTIL_ERROR,
		      AGS_REGEX_UTIL_ERROR_UNMATCHED_BRACE_INTERVAL_OPERATORS,
		      "regcomp unmatched brace interval operators");
	}
	break;
      case REG_EBRACK:
	{
	  g_set_error(error,
		      AGS_REGEX_UTIL_ERROR,
		      AGS_REGEX_UTIL_ERROR_UNMATCHED_BRACKET_LIST_OPERATORS,
		      "regcomp unmatched bracket list operators");
	}
	break;
      case REG_ECOLLATE:
	{
	  g_set_error(error,
		      AGS_REGEX_UTIL_ERROR,
		      AGS_REGEX_UTIL_ERROR_INVALID_COLLATING_ELEMENT,
		      "regcomp invalid collating element");
	}
	break;
      case REG_ECTYPE:
	{
	  g_set_error(error,
		      AGS_REGEX_UTIL_ERROR,
		      AGS_REGEX_UTIL_ERROR_UNKNOWN_CHARACTER_CLASS_NAME,
		      "regcomp unknown character class name");
	}
	break;
#if !defined(AGS_OSXAPI)
      case REG_EEND:
	{
	  g_set_error(error,
		      AGS_REGEX_UTIL_ERROR,
		      AGS_REGEX_UTIL_ERROR_NON_SPECIFIC_ERROR,
		      "regcomp non specific error");
	}
	break;
#endif
      case REG_EESCAPE:
	{
	  g_set_error(error,
		      AGS_REGEX_UTIL_ERROR,
		      AGS_REGEX_UTIL_ERROR_TRAILING_BACKSLASH,
		      "regcomp trailing backslash");
	}
	break;
      case REG_EPAREN:
	{
	  g_set_error(error,
		      AGS_REGEX_UTIL_ERROR,
		      AGS_REGEX_UTIL_ERROR_UNMATCHED_PARENTHESIS_GROUP_OPERATORS,
		      "regcomp unmatched parenthesis group operators");
	}
	break;
      case REG_ERANGE:
	{
	  g_set_error(error,
		      AGS_REGEX_UTIL_ERROR,
		      AGS_REGEX_UTIL_ERROR_INVALID_RANGE_OPERATOR,
		      "regcomp invalid range operator");
	}
	break;
#if !defined(AGS_OSXAPI)
      case REG_ESIZE:
	{
	  g_set_error(error,
		      AGS_REGEX_UTIL_ERROR,
		      AGS_REGEX_UTIL_ERROR_PATTERN_BUFFER_LARGER_THAN_64KB,
		      "regcomp pattern buffer larger than 64kB");
	}
	break;
#endif
      case REG_ESPACE:
	{
	  g_set_error(error,
		      AGS_REGEX_UTIL_ERROR,
		      AGS_REGEX_UTIL_ERROR_OUT_OF_MEMORY,
		      "regcomp out-of-memory");
	}
	break;
      case REG_ESUBREG:
	{
	  g_set_error(error,
		      AGS_REGEX_UTIL_ERROR,
		      AGS_REGEX_UTIL_ERROR_INVALID_BACK_REFERENCE,
		      "regcomp invalid back reference");
	}
	break;
      default:
	{
	  g_set_error(error,
		      AGS_REGEX_UTIL_ERROR,
		      AGS_REGEX_UTIL_ERROR_UNSPECIFIED,
		      "regcomp unspecified error");
	}
      }
    }
  
    return(FALSE);
  }
  
  return(TRUE);
}

/**
 * ags_regex_util_match_alloc:
 * @regex_util: the #AgsRegexUtil
 * @match_count: the match count
 *
 * Allocate #AgsRegexMatch-struct array.
 *
 * Returns: the new allocated #AgsRegexMatch-struct array
 *
 * Since: 6.3.2
 */
AgsRegexMatch*
ags_regex_util_match_alloc(AgsRegexUtil *regex_util,
			   guint match_count)
{
  AgsRegexMatch *match;

  match = (AgsRegexMatch *) g_new0(AgsRegexMatch,
				   match_count);

  return(match);
}

/**
 * ags_regex_util_match_copy:
 * @regex_util: the #AgsRegexUtil
 * @match: the regex match
 * @match_count: the match count
 *
 * Copy @match.
 *
 * Returns: the newly allocated #AgsRegexMatch
 *
 * Since: 6.3.2
 */
AgsRegexMatch*
ags_regex_util_match_copy(AgsRegexUtil *regex_util,
			  AgsRegexMatch *match,
			  guint match_count)
{
  AgsRegexMatch *new_match;

  new_match = ags_regex_util_match_alloc(regex_util,
					 match_count);

  memcpy(new_match, match, match_count * sizeof(AgsRegexMatch));

  return(new_match);
}

/**
 * ags_regex_util_match_free:
 * @regex_util: the #AgsRegexUtil
 * @match: the regex match
 *
 * Free @match.
 *
 * Since: 6.3.2
 */
void
ags_regex_util_match_free(AgsRegexUtil *regex_util,
			  AgsRegexMatch *match)
{
  g_free(match);
}

/**
 * ags_regex_util_match_get_offset:
 * @regex_util: the #AgsRegexUtil
 * @match: the regex match
 * @nth_match: the match position
 * @start_match_offset: (out): the start match offset
 * @end_match_offset: (out): the end match offset
 *
 * Get offset of @match.
 *
 * Since: 6.3.2
 */
void
ags_regex_util_match_get_offset(AgsRegexUtil *regex_util,
				AgsRegexMatch *match,
				guint nth_match,
				gint *start_match_offset, gint *end_match_offset)
{
  if(start_match_offset != NULL){
    start_match_offset[0] = match[nth_match].start_match_offset;
  }
  
  if(end_match_offset != NULL){
    end_match_offset[0] = match[nth_match].end_match_offset;
  }
}

/**
 * ags_regex_util_execute:
 * @regex_util: the #AgsRegexUtil
 * @str: the input string
 * @match_count: the match count
 * @match: the regex match
 * @execute_flags: the execute flags
 * @error: an error that may occure
 *
 * Execute @regex_util and fill @match.
 *
 * Returns: %TRUE on success, otherwise %FALSE
 *
 * Since: 6.3.2
 */
gboolean
ags_regex_util_execute(AgsRegexUtil *regex_util,
		       const gchar *str, guint match_count,
		       AgsRegexMatch *match,
		       guint execute_flags,
		       GError **error)
{
  gchar *local_str;
  
  int retval;

  GError *local_error;

  if(regex_util == NULL ||
     str == NULL){
    return(FALSE);
  }

  local_str = NULL;

  if(regex_util->converter != (GIConv) -1){
    local_error = NULL;
    local_str = g_convert_with_iconv(str,
				     -1,
				     regex_util->converter,
				     NULL,
				     NULL,
				     &local_error);

    if(local_error != NULL){
      g_warning("iconv convert failed");
      
      g_error_free(local_error);
    }
  }else{
    local_str = g_strdup(str);
  }
  
  retval = regexec(&(regex_util->regex), local_str,
		   match_count, match,
		   execute_flags);

  g_free(local_str);

  if(retval == REG_NOMATCH){
    return(FALSE);
  }
  
  return(TRUE);
}

/**
 * ags_regex_util_execute_unichar:
 * @regex_util: the #AgsRegexUtil
 * @str: the input string
 * @match_count: the match count
 * @match: the regex match
 * @execute_flags: the execute flags
 * @error: an error that may occure
 *
 * Execute @regex_util and fill @match.
 *
 * Returns: %TRUE on success, otherwise %FALSE
 *
 * Since: 6.3.2
 */
gboolean
ags_regex_util_execute_unichar(AgsRegexUtil *regex_util,
			       const gunichar *str, guint match_count,
			       AgsRegexMatch *match,
			       guint execute_flags,
			       GError **error)
{
  gchar *utf8_str;
  gchar *local_str;
  
  int retval;

  GError *local_error;

  if(regex_util == NULL ||
     str == NULL){
    return(FALSE);
  }

  local_error = NULL;
  utf8_str = g_ucs4_to_utf8(str,
			    -1,
			    NULL,
			    NULL,
			    &local_error);

  if(local_error != NULL){
    g_warning("ucs4 conversion failed");
      
    g_error_free(local_error);

    return(FALSE);
  }

  local_str = NULL;
  
  if(regex_util->converter != (GIConv) -1){
    local_error = NULL;
    local_str = g_convert_with_iconv(utf8_str,
				     -1,
				     regex_util->converter,
				     NULL,
				     NULL,
				     &local_error);

    if(local_error != NULL){
      g_warning("iconv convert failed");
      
      g_error_free(local_error);
    }
  }else{
    local_str = g_strdup(str);
  }
  
  retval = regexec(&(regex_util->regex), local_str,
		   match_count, match,
		   execute_flags);

  g_free(utf8_str);
  g_free(local_str);

  if(retval == REG_NOMATCH){
    return(FALSE);
  }
  
  return(TRUE);
}

/**
 * ags_regex_util_execute_unichar2:
 * @regex_util: the #AgsRegexUtil
 * @str: the input string
 * @match_count: the match count
 * @match: the regex match
 * @execute_flags: the execute flags
 * @error: an error that may occure
 *
 * Execute @regex_util and fill @match.
 *
 * Returns: %TRUE on success, otherwise %FALSE
 *
 * Since: 6.3.2
 */
gboolean
ags_regex_util_execute_unichar2(AgsRegexUtil *regex_util,
				const gunichar2 *str, guint match_count,
				AgsRegexMatch *match,
				guint execute_flags,
				GError **error)
{
  gchar *utf8_str;
  gchar *local_str;
  
  int retval;

  GError *local_error;

  if(regex_util == NULL ||
     str == NULL){
    return(FALSE);
  }

  local_error = NULL;
  utf8_str = g_utf16_to_utf8(str,
			     -1,
			     NULL,
			     NULL,
			     &local_error);

  if(local_error != NULL){
    g_warning("UTF-16 conversion failed");
      
    g_error_free(local_error);

    return(FALSE);
  }

  local_str = NULL;
  
  if(regex_util->converter != (GIConv) -1){
    local_error = NULL;
    local_str = g_convert_with_iconv(utf8_str,
				     -1,
				     regex_util->converter,
				     NULL,
				     NULL,
				     &local_error);

    if(local_error != NULL){
      g_warning("iconv convert failed");
      
      g_error_free(local_error);
    }
  }else{
    local_str = g_strdup(str);
  }
  
  retval = regexec(&(regex_util->regex), local_str,
		   match_count, match,
		   execute_flags);

  g_free(utf8_str);
  g_free(local_str);

  if(retval == REG_NOMATCH){
    return(FALSE);
  }
  
  return(TRUE);
}
