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

#ifndef __AGS_REGEX_UTIL_H__
#define __AGS_REGEX_UTIL_H__

#include <glib.h>
#include <glib-object.h>

#include <gio/gio.h>

#include <locale.h>

#include <sys/types.h>
#include <regex.h>

G_BEGIN_DECLS

#define AGS_TYPE_REGEX_UTIL                (ags_regex_util_get_type())
#define AGS_REGEX_UTIL(ptr) ((AgsRegexUtil *)(ptr))

typedef struct _AgsRegexUtil AgsRegexUtil;
typedef struct _AgsRegexMatch AgsRegexMatch;

/**
 * AgsRegexUtilCompileFlags:
 * AGS_REGEX_UTIL_POSIX_EXTENDED_SYNTAX: extended regular expression syntax
 * AGS_REGEX_UTIL_CASE_INSENSITIVE: do not differentiate case
 * AGS_REGEX_UTIL_NO_POSITION_REPORT_OF_MATCHES: the match_count and match_ptr parameters of ags_regex_util_execute() are ignored
 * AGS_REGEX_UTIL_NO_NEWLINE_MATCH: do not match newlines as using match-any-character
 * 
 * Enum values to control the behavior of ags_regex_util_compile() and related by
 * enable/disable as flags.
 */
typedef enum{
  AGS_REGEX_UTIL_POSIX_EXTENDED_SYNTAX              = REG_EXTENDED,
  AGS_REGEX_UTIL_CASE_INSENSITIVE                   = REG_ICASE,
  AGS_REGEX_UTIL_NO_POSITION_REPORT_OF_MATCHES      = REG_NOSUB,
  AGS_REGEX_UTIL_NO_NEWLINE_MATCH                   = REG_NEWLINE,
}AgsRegexUtilCompileFlags;

/**
 * AgsRegexUtilExecuteFlags:
 * AGS_REGEX_UTIL_NOT_BEGINNING_OF_LINE: match-beginning-of-line operator always fails, the beginning of of string should not be interpreted as beginning of line
 * AGS_REGEX_UTIL_NOT_END_OF_LINE: match-end-of-line operator always fails
 * AGS_REGEX_UTIL_START_END: use match_ptr[0].start_match_offset and match_ptr[0].end_match_offset as string length, allows the use of NUL bytes
 * 
 * Enum values to control the behavior of ags_regex_util_execute() and such by
 * enable/disable as flags.
 */
typedef enum{
  AGS_REGEX_UTIL_NOT_BEGINNING_OF_LINE = REG_NOTBOL,
  AGS_REGEX_UTIL_NOT_END_OF_LINE       = REG_NOTEOL,
  AGS_REGEX_UTIL_START_END             = REG_STARTEND,
}AgsRegexUtilExecuteFlags;

#define AGS_REGEX_UTIL_ERROR (ags_regex_util_error_quark())

typedef enum{
  AGS_REGEX_UTIL_ERROR_BAD_BACK_REFERENCE,
  AGS_REGEX_UTIL_ERROR_BAD_PATTERN,
  AGS_REGEX_UTIL_ERROR_BAD_REPETITION,
  AGS_REGEX_UTIL_ERROR_UNMATCHED_BRACE_INTERVAL_OPERATORS,
  AGS_REGEX_UTIL_ERROR_UNMATCHED_BRACKET_LIST_OPERATORS,
  AGS_REGEX_UTIL_ERROR_INVALID_COLLATING_ELEMENT,
  AGS_REGEX_UTIL_ERROR_UNKNOWN_CHARACTER_CLASS_NAME,
  AGS_REGEX_UTIL_ERROR_NON_SPECIFIC_ERROR,
  AGS_REGEX_UTIL_ERROR_TRAILING_BACKSLASH,
  AGS_REGEX_UTIL_ERROR_UNMATCHED_PARENTHESIS_GROUP_OPERATORS,
  AGS_REGEX_UTIL_ERROR_INVALID_RANGE_OPERATOR,
  AGS_REGEX_UTIL_ERROR_PATTERN_BUFFER_LARGER_THAN_64KB,
  AGS_REGEX_UTIL_ERROR_OUT_OF_MEMORY,
  AGS_REGEX_UTIL_ERROR_INVALID_BACK_REFERENCE,
  AGS_REGEX_UTIL_ERROR_UNSPECIFIED,
}AgsRegexUtilError;

struct _AgsRegexUtil
{
  gchar *app_encoding;
  
  gchar *encoding;

  gboolean is_unichar;
  gboolean is_unichar2;

  GIConv *converter;

  regex_t regex;
  
  gchar *regex_str;

  gint compile_flags;  
};

struct _AgsRegexMatch
{
  gint start_match_offset;
  gint end_match_offset;
};

GType ags_regex_util_get_type(void);
GType ags_regex_util_compile_flags_get_type();
GType ags_regex_util_execute_flags_get_type();

GQuark ags_regex_util_error_quark();

AgsRegexUtil* ags_regex_util_alloc(gchar *app_encoding,
				   gchar *encoding,
				   gboolean is_unichar, gboolean is_unichar2);

gpointer ags_regex_util_copy(AgsRegexUtil *ptr);
void ags_regex_util_free(AgsRegexUtil *ptr);

/* regex util getter/setter of fields */
gchar* ags_regex_util_get_app_encoding(AgsRegexUtil *regex_util);

gchar* ags_regex_util_get_encoding(AgsRegexUtil *regex_util);

/* regex compile */
gboolean ags_regex_util_compile(AgsRegexUtil *regex_util,
				const gchar *regex_str, gint compile_flags,
				GError **error);

/* regex match struct */
AgsRegexMatch* ags_regex_util_match_alloc(AgsRegexUtil *regex_util,
					  guint match_count);

AgsRegexMatch* ags_regex_util_match_copy(AgsRegexUtil *regex_util,
					 AgsRegexMatch *match,
					 guint match_count);
void ags_regex_util_match_free(AgsRegexUtil *regex_util,
			       AgsRegexMatch *match);

/* regex match getter of fields */
void ags_regex_util_match_get_offset(AgsRegexUtil *regex_util,
				     AgsRegexMatch *match,
				     guint nth_match,
				     gint *start_match_offset, gint *end_match_offset);

/* regex execute */
gboolean ags_regex_util_execute(AgsRegexUtil *regex_util,
				const gchar *str, guint match_count,
				AgsRegexMatch *match,
				guint execute_flags,
				GError **error);
gboolean ags_regex_util_execute_unichar(AgsRegexUtil *regex_util,
					const gunichar *str, guint match_count,
					AgsRegexMatch *match,
					guint execute_flags,
					GError **error);
gboolean ags_regex_util_execute_unichar2(AgsRegexUtil *regex_util,
					 const gunichar2 *str, guint match_count,
					 AgsRegexMatch *match,
					 guint execute_flags,
					 GError **error);

G_END_DECLS

#endif /*__AGS_REGEX_UTIL_H__*/
