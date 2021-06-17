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

#ifndef __AGS_SFZ_UTIL_H__
#define __AGS_SFZ_UTIL_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <ags/audio/file/ags_sfz_file.h>
#include <ags/audio/file/ags_sfz_sample.h>

G_BEGIN_DECLS

#define AGS_SFZ(ptr)                ((AgsSFZ *)(ptr))

typedef struct _AgsSFZ AgsSFZ;

struct _AgsSFZ
{
  gdouble note;

  gdouble volume;

  guint samplerate;

  guint offset;
  guint n_frames;

  guint loop_mode;

  gint loop_start;
  gint loop_end;
  
  gpointer generic_pitch;

  gpointer data;
};

void ags_sfz_util_copy_s8(AgsSFZ *sfz,
			  gint8 *buffer,
			  guint buffer_size,
			  AgsSFZSample *sfz_sample);
void ags_sfz_util_copy_s16(AgsSFZ *sfz,
			   gint16 *buffer,
			   guint buffer_size,
			   AgsSFZSample *sfz_sample);
void ags_sfz_util_copy_s24(AgsSFZ *sfz,
			   gint32 *buffer,
			   guint buffer_size,
			   AgsSFZSample *sfz_sample);
void ags_sfz_util_copy_s32(AgsSFZ *sfz,
			   gint32 *buffer,
			   guint buffer_size,
			   AgsSFZSample *sfz_sample);
void ags_sfz_util_copy_s64(AgsSFZ *sfz,
			   gint64 *buffer,
			   guint buffer_size,
			   AgsSFZSample *sfz_sample);
void ags_sfz_util_copy_float(AgsSFZ *sfz,
			     gfloat *buffer,
			     guint buffer_size,
			     AgsSFZSample *sfz_sample);
void ags_sfz_util_copy_double(AgsSFZ *sfz,
			      gdouble *buffer,
			      guint buffer_size,
			      AgsSFZSample *sfz_sample);
void ags_sfz_util_copy_complex(AgsSFZ *sfz,
			       AgsComplex *buffer,
			       guint buffer_size,
			       AgsSFZSample *sfz_sample);

G_END_DECLS

#endif /*__AGS_SFZ_UTIL_H__*/
