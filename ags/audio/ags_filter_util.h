/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2021 Joël Krähemann
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

#ifndef __AGS_FILTER_UTIL_H__
#define __AGS_FILTER_UTIL_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

G_BEGIN_DECLS

#define AGS_TYPE_FILTER_UTIL         (ags_filter_util_get_type())

#define AGS_FILTER_UTIL_DEFAULT_TMP_BUFFER_SIZE (8192)

struct _AgsFilterUtil
{
  //empty
};

GType ags_filter_util_get_type(void);

typedef struct _AgsFilterUtil AgsFilterUtil;

void ags_filter_util_pitch_s8(gint8 *buffer,
			      guint buffer_length,
			      guint samplerate,
			      gdouble base_key,
			      gdouble tuning);
void ags_filter_util_pitch_s16(gint16 *buffer,
			       guint buffer_length,
			       guint samplerate,
			       gdouble base_key,
			       gdouble tuning);
void ags_filter_util_pitch_s24(gint32 *buffer,
			       guint buffer_length,
			       guint samplerate,
			       gdouble base_key,
			       gdouble tuning);
void ags_filter_util_pitch_s32(gint32 *buffer,
			       guint buffer_length,
			       guint samplerate,
			       gdouble base_key,
			       gdouble tuning);
void ags_filter_util_pitch_s64(gint64 *buffer,
			       guint buffer_length,
			       guint samplerate,
			       gdouble base_key,
			       gdouble tuning);
void ags_filter_util_pitch_float(gfloat *buffer,
				 guint buffer_length,
				 guint samplerate,
				 gdouble base_key,
				 gdouble tuning);
void ags_filter_util_pitch_double(gdouble *buffer,
				  guint buffer_length,
				  guint samplerate,
				  gdouble base_key,
				  gdouble tuning);
void ags_filter_util_pitch_complex(AgsComplex *buffer,
				   guint buffer_length,
				   guint samplerate,
				   gdouble base_key,
				   gdouble tuning);

G_END_DECLS

#endif /*__AGS_FILTER_UTIL_H__*/
