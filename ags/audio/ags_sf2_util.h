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

#ifndef __AGS_SF2_UTIL_H__
#define __AGS_SF2_UTIL_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <ags/audio/file/ags_ipatch.h>
#include <ags/audio/file/ags_ipatch_sample.h>

G_BEGIN_DECLS

#define AGS_SF2(ptr)                ((AgsSF2 *)(ptr))

typedef struct _AgsSF2 AgsSF2;

struct _AgsSF2
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

void ags_sf2_util_copy_s8(AgsSF2 *sf2,
			  gint8 *buffer,
			  guint buffer_size,
			  AgsIpatchSample *ipatch_sample);
void ags_sf2_util_copy_s16(AgsSF2 *sf2,
			   gint16 *buffer,
			   guint buffer_size,
			   AgsIpatchSample *ipatch_sample);
void ags_sf2_util_copy_s24(AgsSF2 *sf2,
			   gint32 *buffer,
			   guint buffer_size,
			   AgsIpatchSample *ipatch_sample);
void ags_sf2_util_copy_s32(AgsSF2 *sf2,
			   gint32 *buffer,
			   guint buffer_size,
			   AgsIpatchSample *ipatch_sample);
void ags_sf2_util_copy_s64(AgsSF2 *sf2,
			   gint64 *buffer,
			   guint buffer_size,
			   AgsIpatchSample *ipatch_sample);
void ags_sf2_util_copy_float(AgsSF2 *sf2,
			     gfloat *buffer,
			     guint buffer_size,
			     AgsIpatchSample *ipatch_sample);
void ags_sf2_util_copy_double(AgsSF2 *sf2,
			      gdouble *buffer,
			      guint buffer_size,
			      AgsIpatchSample *ipatch_sample);
void ags_sf2_util_copy_complex(AgsSF2 *sf2,
			       AgsComplex *buffer,
			       guint buffer_size,
			       AgsIpatchSample *ipatch_sample);

G_END_DECLS

#endif /*__AGS_SF2_UTIL_H__*/
