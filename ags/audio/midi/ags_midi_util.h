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
gboolean ags_midi_util_is_key_on(unsigned char *buffer);
gboolean ags_midi_util_is_key_off(unsigned char *buffer);
gboolean ags_midi_util_is_key_pressure(unsigned char *buffer);
gboolean ags_midi_util_is_change_parameter(unsigned char *buffer);
gboolean ags_midi_util_is_pitch_bend(unsigned char *buffer);
gboolean ags_midi_util_is_change_program(unsigned char *buffer);
gboolean ags_midi_util_is_change_pressure(unsigned char *buffer);

/* real-time sysex utility */
gboolean ags_midi_util_is_sysex(unsigned char *buffer);

/* real-time system common utility */
gboolean ags_midi_util_is_quarter_frame(unsigned char *buffer);
gboolean ags_midi_util_is_song_position(unsigned char *buffer);
gboolean ags_midi_util_is_song_select(unsigned char *buffer);
gboolean ags_midi_util_is_tune_request(unsigned char *buffer);

/* real-time meta event utility */
gboolean ags_midi_util_is_meta_event(unsigned char *buffer);

/*  */
unsigned char* ags_midi_util_to_smf(unsigned char *midi_buffer, guint buffer_length,
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
