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

#define AGS_TYPE_SF2_UTIL         (ags_sf2_util_get_type())

#define AGS_SF2(ptr)              ((AgsSF2 *)(ptr))

typedef struct _AgsSF2Util AgsSF2Util;
typedef struct _AgsSF2 AgsSF2;

struct _AgsSF2Util
{
  //empty
};

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

  gpointer user_data;
};

GType ags_sf2_util_get_type(void);

gdouble ags_sf2_get_note(AgsSF2 *sf2);
void ags_sf2_set_note(AgsSF2 *sf2,
		      gdouble note);

gdouble ags_sf2_get_volume(AgsSF2 *sf2);
void ags_sf2_set_volume(AgsSF2 *sf2,
			gdouble volume);

guint ags_sf2_get_samplerate(AgsSF2 *sf2);
void ags_sf2_set_samplerate(AgsSF2 *sf2,
			    guint samplerate);

guint ags_sf2_get_offset(AgsSF2 *sf2);
void ags_sf2_set_offset(AgsSF2 *sf2,
			guint offset);

guint ags_sf2_get_n_frames(AgsSF2 *sf2);
void ags_sf2_set_n_frames(AgsSF2 *sf2,
			    guint n_frames);

guint ags_sf2_get_loop_mode(AgsSF2 *sf2);
void ags_sf2_set_loop_mode(AgsSF2 *sf2,
			    guint loop_mode);

guint ags_sf2_get_loop_start(AgsSF2 *sf2);
void ags_sf2_set_loop_start(AgsSF2 *sf2,
			    guint loop_start);

guint ags_sf2_get_loop_end(AgsSF2 *sf2);
void ags_sf2_set_loop_end(AgsSF2 *sf2,
			    guint loop_end);

gpointer ags_sf2_get_generic_pitch(AgsSF2 *sf2);
void ags_sf2_set_generic_pitch(AgsSF2 *sf2,
			       gpointer generic_pitch);

gpointer ags_sf2_get_user_data(AgsSF2 *sf2);
void ags_sf2_set_user_data(AgsSF2 *sf2,
			   gpointer user_data);

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

void ags_sf2_util_copy(AgsSF2 *sf2,
		       void *buffer,
		       guint buffer_size,
		       AgsIpatchSample *ipatch_sample,
		       guint audio_buffer_util_format);

G_END_DECLS

#endif /*__AGS_SF2_UTIL_H__*/
