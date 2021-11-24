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

#ifndef __AGS_MIDI_UTIL_H__
#define __AGS_MIDI_UTIL_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

G_BEGIN_DECLS

#define AGS_TYPE_MIDI_UTIL         (ags_midi_util_get_type())

typedef struct _AgsMidiUtil AgsMidiUtil;

struct _AgsMidiUtil
{
  //empty
};

GType ags_midi_util_get_type(void);

/* real-time channel message utility */
gboolean ags_midi_util_is_key_on(guchar *buffer);
gboolean ags_midi_util_is_key_off(guchar *buffer);
gboolean ags_midi_util_is_key_pressure(guchar *buffer);
gboolean ags_midi_util_is_change_parameter(guchar *buffer);
gboolean ags_midi_util_is_pitch_bend(guchar *buffer);
gboolean ags_midi_util_is_change_program(guchar *buffer);
gboolean ags_midi_util_is_change_pressure(guchar *buffer);

/* real-time sysex utility */
gboolean ags_midi_util_is_sysex(guchar *buffer);

/* real-time system common utility */
gboolean ags_midi_util_is_quarter_frame(guchar *buffer);
gboolean ags_midi_util_is_song_position(guchar *buffer);
gboolean ags_midi_util_is_song_select(guchar *buffer);
gboolean ags_midi_util_is_tune_request(guchar *buffer);

/* real-time meta event utility */
gboolean ags_midi_util_is_meta_event(guchar *buffer);

/*  */
gboolean ags_midi_util_get_key_on(guchar *buffer,
				  gint *channel, gint *key, gint *velocity);
gboolean ags_midi_util_get_key_off(guchar *buffer,
				   gint *channel, gint *key, gint *velocity);
gboolean ags_midi_util_get_key_pressure(guchar *buffer,
					gint *channel, gint *key, gint *pressure);
gboolean ags_midi_util_get_change_parameter(guchar *buffer,
					    gint *channel, gint *control, gint *value);
gboolean ags_midi_util_get_pitch_bend(guchar *buffer,
				      gint *channel, gint *pitch, gint *transmitter);
gboolean ags_midi_util_get_change_program(guchar *buffer,
					  gint *channel, gint *program);
gboolean ags_midi_util_get_change_pressure(guchar *buffer,
					   gint *channel, gint *pressure);

gboolean ags_midi_util_get_sysex(guchar *buffer,
				 guchar **data, gint *length);

gboolean ags_midi_util_get_quarter_frame(guchar *buffer,
					 gint *message_type, gint *values);
gboolean ags_midi_util_get_song_position(guchar *buffer,
					 gint *song_position);
gboolean ags_midi_util_get_song_select(guchar *buffer,
				       gint *song_select);

/*  */
guchar* ags_midi_util_to_smf(guchar *midi_buffer, guint buffer_length,
			     glong delta_time,
			     guint *smf_buffer_length);

guint ags_midi_util_delta_time_to_offset(gdouble delay_factor,
					 glong division,
					 glong tempo,
					 glong bpm,
					 glong delta_time);
glong ags_midi_util_offset_to_delta_time(gdouble delay_factor,
					 glong division,
					 glong tempo,
					 glong bpm,
					 guint x);

G_END_DECLS

#endif /*__AGS_MIDI_UTIL_H__*/
