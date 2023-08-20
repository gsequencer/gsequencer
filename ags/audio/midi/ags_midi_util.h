/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2023 Joël Krähemann
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
  guint major;
  guint minor;
};

GType ags_midi_util_get_type(void);

AgsMidiUtil* ags_midi_util_alloc();
void ags_midi_util_free(AgsMidiUtil *midi_util);

AgsMidiUtil* ags_midi_util_copy(AgsMidiUtil *midi_util);

/* real-time channel message utility */
gboolean ags_midi_util_is_key_on(AgsMidiUtil *midi_util,
				 guchar *buffer);
gboolean ags_midi_util_is_key_off(AgsMidiUtil *midi_util,
				  guchar *buffer);
gboolean ags_midi_util_is_key_pressure(AgsMidiUtil *midi_util,
				       guchar *buffer);
gboolean ags_midi_util_is_change_parameter(AgsMidiUtil *midi_util,
					   guchar *buffer);
gboolean ags_midi_util_is_pitch_bend(AgsMidiUtil *midi_util,
				     guchar *buffer);
gboolean ags_midi_util_is_change_program(AgsMidiUtil *midi_util,
					 guchar *buffer);
gboolean ags_midi_util_is_change_pressure(AgsMidiUtil *midi_util,
					  guchar *buffer);

/* real-time sysex utility */
gboolean ags_midi_util_is_sysex(AgsMidiUtil *midi_util,
				guchar *buffer);

/* real-time system common utility */
gboolean ags_midi_util_is_quarter_frame(AgsMidiUtil *midi_util,
					guchar *buffer);
gboolean ags_midi_util_is_song_position(AgsMidiUtil *midi_util,
					guchar *buffer);
gboolean ags_midi_util_is_song_select(AgsMidiUtil *midi_util,
				      guchar *buffer);
gboolean ags_midi_util_is_tune_request(AgsMidiUtil *midi_util,
				       guchar *buffer);

/* real-time meta event utility */
gboolean ags_midi_util_is_meta_event(AgsMidiUtil *midi_util,
				     guchar *buffer);

/* real-time channel message getter */
gboolean ags_midi_util_get_key_on(AgsMidiUtil *midi_util,
				  guchar *buffer,
				  gint *channel, gint *key, gint *velocity);
gboolean ags_midi_util_get_key_off(AgsMidiUtil *midi_util,
				   guchar *buffer,
				   gint *channel, gint *key, gint *velocity);
gboolean ags_midi_util_get_key_pressure(AgsMidiUtil *midi_util,
					guchar *buffer,
					gint *channel, gint *key, gint *pressure);
gboolean ags_midi_util_get_change_parameter(AgsMidiUtil *midi_util,
					    guchar *buffer,
					    gint *channel, gint *control, gint *value);
gboolean ags_midi_util_get_pitch_bend(AgsMidiUtil *midi_util,
				      guchar *buffer,
				      gint *channel, gint *pitch, gint *transmitter);
gboolean ags_midi_util_get_change_program(AgsMidiUtil *midi_util,
					  guchar *buffer,
					  gint *channel, gint *program);
gboolean ags_midi_util_get_change_pressure(AgsMidiUtil *midi_util,
					   guchar *buffer,
					   gint *channel, gint *pressure);

/* real-time sysex gettery */
gboolean ags_midi_util_get_sysex(AgsMidiUtil *midi_util,
				 guchar *buffer,
				 guchar **data, gint *length);

/* real-time system common getter */
gboolean ags_midi_util_get_quarter_frame(AgsMidiUtil *midi_util,
					 guchar *buffer,
					 gint *message_type, gint *values);
gboolean ags_midi_util_get_song_position(AgsMidiUtil *midi_util,
					 guchar *buffer,
					 gint *song_position);
gboolean ags_midi_util_get_song_select(AgsMidiUtil *midi_util,
				       guchar *buffer,
				       gint *song_select);

/* create Standard MIDI Format unsigned char buffer */
guchar* ags_midi_util_to_smf(AgsMidiUtil *midi_util,
			     guchar *midi_buffer, guint buffer_length,
			     gint delta_time,
			     guint *smf_buffer_length);

/* note offset to and from delta time */
guint ags_midi_util_delta_time_to_offset(AgsMidiUtil *midi_util,
					 gdouble delay_factor,
					 gint division,
					 gint tempo,
					 gint bpm,
					 gint delta_time);
gint ags_midi_util_offset_to_delta_time(AgsMidiUtil *midi_util,
					gdouble delay_factor,
					gint division,
					gint tempo,
					gint bpm,
					guint x);

G_END_DECLS

#endif /*__AGS_MIDI_UTIL_H__*/
