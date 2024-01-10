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

#ifndef __AGS_FILE_UTIL_H__
#define __AGS_FILE_UTIL_H__

#include <glib.h>
#include <glib-object.h>

#include <gio/gio.h>

#include <ags/lib/ags_complex.h>

#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>

#include <complex.h>

#include <stdarg.h>

G_BEGIN_DECLS

#define AGS_TYPE_FILE_UTIL                (ags_file_util_get_type())
#define AGS_FILE_UTIL(ptr) ((AgsFileUtil *)(ptr))

#define AGS_FILE_UTIL_64BIT_MAX_STRING_LENGTH (20)
#define AGS_FILE_UTIL_DOUBLE_MAX_STRING_LENGTH (16)

typedef struct _AgsFileUtil AgsFileUtil;

struct _AgsFileUtil
{
  gchar *app_encoding;
  
  gchar *encoding;

  GIConv *converter;
  
  GIConv *reverse_converter;
};

GType ags_file_util_get_type(void);

AgsFileUtil* ags_file_util_alloc(gchar *app_encoding,
				 gchar *encoding);

gpointer ags_file_util_copy(AgsFileUtil *ptr);
void ags_file_util_free(AgsFileUtil *ptr);

gchar* ags_file_util_get_app_encoding(AgsFileUtil *file_util);

gchar* ags_file_util_get_encoding(AgsFileUtil *file_util);

gint ags_file_util_get_int(AgsFileUtil *file_util,
			   gchar *str);
gchar* ags_file_util_put_int(AgsFileUtil *file_util,
			     gint value);

guint ags_file_util_get_uint(AgsFileUtil *file_util,
			     gchar *str);
gchar* ags_file_util_put_uint(AgsFileUtil *file_util,
			      guint value);

gint64 ags_file_util_get_int64(AgsFileUtil *file_util,
				gchar *str);
gchar* ags_file_util_put_int64(AgsFileUtil *file_util,
			       gint64 value);

guint64 ags_file_util_get_uint64(AgsFileUtil *file_util,
				  gchar *str);
gchar* ags_file_util_put_uint64(AgsFileUtil *file_util,
				guint64 value);

gfloat ags_file_util_get_float(AgsFileUtil *file_util,
			       gchar *str);
gchar* ags_file_util_put_float(AgsFileUtil *file_util,
			       gfloat value);

gdouble ags_file_util_get_double(AgsFileUtil *file_util,
				 gchar *str);
gchar* ags_file_util_put_double(AgsFileUtil *file_util,
				gdouble value);

AgsComplex* ags_file_util_get_complex(AgsFileUtil *file_util,
				      gchar *str);
gchar* ags_file_util_put_complex(AgsFileUtil *file_util,
				 AgsComplex *value);

gchar* ags_file_util_get_string(AgsFileUtil *file_util,
				gchar *str,
				gint max_length);
gchar* ags_file_util_put_string(AgsFileUtil *file_util,
				gchar *str,
				gint length);

G_END_DECLS

#endif /*__AGS_FILE_UTIL_H__*/
