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

#include <sys/types.h>
#include <regex.h>

G_BEGIN_DECLS

#define AGS_TYPE_REGEX_UTIL                (ags_regex_util_get_type())
#define AGS_REGEX_UTIL(ptr) ((AgsRegexUtil *)(ptr))

typedef struct _AgsRegexUtil AgsRegexUtil;
typedef struct _AgsRegexMatch AgsRegexMatch;

typedef enum{
  AGS_REGEX_UTIL_NOT_BEGINNING_OF_LINE = REG_NOTBOL,
  AGS_REGEX_UTIL_NOT_END_OF_LINE       = REG_NOTEOL,
  AGS_REGEX_UTIL_START_END             = REG_STARTEND,
}AgsRegexUtilExecuteFlags;

#define AGS_REGEX_UTIL_ERROR (ags_regex_util_error_quark())

typedef enum{
  AGS_REGEX_UTIL_ERROR_BAD_BACK_REFERENCE,
  AGS_REGEX_UTIL_ERROR_BAD_PATTERN_BADPAT,
  AGS_REGEX_UTIL_ERROR_BAD_REPETITION_BADRPT,
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
}AgsRegexUtilError;

struct _AgsRegexUtil
{
  gchar *app_encoding;
  
  gchar *encoding;

  gboolean is_unichar;
  gboolean is_unichar2;

  GIConv *converter;
  GIConv *unichar_converter;
  GIConv *unichar2_converter;
  
  GIConv *reverse_converter;
  GIConv *reverse_unichar_converter;
  GIConv *reverse_unichar2_converter;

  regex_t *regex_ptr;
  
  const gchar *regex_str;

  gint compile_flags;  
};

struct _AgsRegexMatch
{
  gint start_match_offset;
  gint end_match_offset;  
};

GType ags_regex_util_get_type(void);

GQuark ags_regex_util_error_quark();

AgsRegexUtil* ags_regex_util_alloc(gchar *app_encoding,
				   gchar *encoding,
				   gboolean is_unichar, gboolean is_unichar2);

gpointer ags_regex_util_copy(AgsRegexUtil *ptr);
void ags_regex_util_free(AgsRegexUtil *ptr);

gchar* ags_regex_util_get_app_encoding(AgsRegexUtil *regex_util);

gchar* ags_regex_util_get_encoding(AgsRegexUtil *regex_util);

gboolean ags_regex_util_compile(AgsRegexUtil *regex_util,
				const gchar *regex_str, gint compile_flags,
				GError **error);

gboolean ags_regex_util_execute(AgsRegexUtil *regex_util,
				const gchar *str, gsize_t match_count,
				AgsRegexMatch *match_ptr,
				guint execute_flags,
				GError **error);
gboolean ags_regex_util_execute_unichar(AgsRegexUtil *regex_util,
					const gunichar *str, gsize_t match_count,
					AgsRegexMatch *match_ptr,
					guint execute_flags,
					GError **error);
gboolean ags_regex_util_execute_unichar2(AgsRegexUtil *regex_util,
					 const gunichar2 *str, gsize_t match_count,
					 AgsRegexMatch *match_ptr,
					 guint execute_flags,
					 GError **error);

G_END_DECLS

#endif /*__AGS_REGEX_UTIL_H__*/
