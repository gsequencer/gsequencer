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

#ifndef __AGS_MIDI_SMF_UTIL_H__
#define __AGS_MIDI_SMF_UTIL_H__

#include <glib.h>
#include <glib-object.h>

#include <alsa/seq_event.h>

#include <ags/libags.h>

G_BEGIN_DECLS

#define AGS_TYPE_MIDI_SMF_UTIL         (ags_midi_smf_util_get_type())

typedef struct _AgsMidiSmfUtil AgsMidiSmfUtil;

typedef enum{
  AGS_MIDI_SMF_UTIL_MTC_QUARTER_FRAME_FRAME_NUMBER_LSB   = 0x0,
  AGS_MIDI_SMF_UTIL_MTC_QUARTER_FRAME_FRAME_NUMBER_MSB   = 0x10,
  AGS_MIDI_SMF_UTIL_MTC_QUARTER_FRAME_SECONDS_LSB        = 0x20,
  AGS_MIDI_SMF_UTIL_MTC_QUARTER_FRAME_SECONDS_MSB        = 0x30,
  AGS_MIDI_SMF_UTIL_MTC_QUARTER_FRAME_MINUTES_LSB        = 0x40,
  AGS_MIDI_SMF_UTIL_MTC_QUARTER_FRAME_MINUTES_MSB        = 0x50,
  AGS_MIDI_SMF_UTIL_MTC_QUARTER_FRAME_HOURS_LSB          = 0x60,
  AGS_MIDI_SMF_UTIL_MTC_QUARTER_FRAME_HOURS_MSB          = 0x70,
}AgsMidiSmfUtilMtcQuarterFrameMessageType;

typedef enum{
  AGS_MIDI_SMF_UTIL_SMTPE_FRAME_RATE_24_FPS             = 0x0,
  AGS_MIDI_SMF_UTIL_SMTPE_FRAME_RATE_25_FPS             = 0x40,
  AGS_MIDI_SMF_UTIL_SMTPE_FRAME_RATE_30_FPS             = 0x80,
  AGS_MIDI_SMF_UTIL_SMTPE_FRAME_RATE_30_FPS_DROP_FRAME  = 0xc0,
}AgsMidiSmfUtilSmtpeFrameRate;

struct _AgsMidiSmfUtil
{
  guint major;
  guint minor;
};

GType ags_midi_smf_util_get_type(void);

AgsMidiSmfUtil* ags_midi_smf_util_alloc();
void ags_midi_smf_util_free(AgsMidiSmfUtil *midi_smf_util);

AgsMidiSmfUtil* ags_midi_smf_util_copy(AgsMidiSmfUtil *midi_smf_util);

/* varlength */
guint ags_midi_smf_util_get_varlength_size(AgsMidiSmfUtil *midi_smf_util,
					   gint varlength);
void ags_midi_smf_util_put_varlength(AgsMidiSmfUtil *midi_smf_util,
				     guchar *buffer,
				     gint varlength);
guint ags_midi_smf_util_get_varlength(AgsMidiSmfUtil *midi_smf_util,
				      guchar *buffer,
				      gint *varlength);

/* integer - 2 bytes */
void ags_midi_smf_util_put_int16(AgsMidiSmfUtil *midi_smf_util,
				 guchar *buffer,
				 gint val);
void ags_midi_smf_util_get_int16(AgsMidiSmfUtil *midi_smf_util,
				 guchar *buffer,
				 gint *val);

/* integer - 3 bytes */
void ags_midi_smf_util_put_int24(AgsMidiSmfUtil *midi_smf_util,
				 guchar *buffer,
				 gint val);
void ags_midi_smf_util_get_int24(AgsMidiSmfUtil *midi_smf_util,
				 guchar *buffer,
				 gint *val);

/* integer - 4 bytes */
void ags_midi_smf_util_put_int32(AgsMidiSmfUtil *midi_smf_util,
				 guchar *buffer,
				 gint val);
void ags_midi_smf_util_get_int32(AgsMidiSmfUtil *midi_smf_util,
				 guchar *buffer,
				 gint *val);

/* channel message */
/* midi header */
void ags_midi_smf_util_put_header(AgsMidiSmfUtil *midi_smf_util,
				  guchar *buffer,
				  gint offset, gint format,
				  gint track_count, gint division);

guint ags_midi_smf_util_get_header(AgsMidiSmfUtil *midi_smf_util,
				   guchar *buffer,
				   gint *offset, gint *format,
				   gint *track_count, gint *division);

/* midi track */
void ags_midi_smf_util_put_track(AgsMidiSmfUtil *midi_smf_util,
				 guchar *buffer,
				 gint offset);
guint ags_midi_smf_util_get_track(AgsMidiSmfUtil *midi_smf_util,
				  guchar *buffer,
				  gint *offset);

/* key on */
void ags_midi_smf_util_put_key_on(AgsMidiSmfUtil *midi_smf_util,
				  guchar *buffer,
				  gint delta_time,
				  gint channel,
				  gint key,
				  gint velocity);

guint ags_midi_smf_util_get_key_on(AgsMidiSmfUtil *midi_smf_util,
				   guchar *buffer,
				   gint *delta_time,
				   gint *channel,
				   gint *key,
				   gint *velocity);

/* key off */
void ags_midi_smf_util_put_key_off(AgsMidiSmfUtil *midi_smf_util,
				   guchar *buffer,
				   gint delta_time,
				   gint channel,
				   gint key,
				   gint velocity);

guint ags_midi_smf_util_get_key_off(AgsMidiSmfUtil *midi_smf_util,
				    guchar *buffer,
				    gint *delta_time,
				    gint *channel,
				    gint *key,
				    gint *velocity);

/* key pressure */
void ags_midi_smf_util_put_key_pressure(AgsMidiSmfUtil *midi_smf_util,
					guchar *buffer,
					gint delta_time,
					gint channel,
					gint key,
					gint pressure);

guint ags_midi_smf_util_get_key_pressure(AgsMidiSmfUtil *midi_smf_util,
					 guchar *buffer,
					 gint *delta_time,
					 gint *channel,
					 gint *key,
					 gint *pressure);

/* change parameter */
void ags_midi_smf_util_put_change_parameter(AgsMidiSmfUtil *midi_smf_util,
					    guchar *buffer,
					    gint delta_time,
					    gint channel,
					    gint control,
					    gint value);
guint ags_midi_smf_util_get_change_parameter(AgsMidiSmfUtil *midi_smf_util,
					     guchar *buffer,
					     gint *delta_time,
					     gint *channel,
					     gint *control,
					     gint *value);

/* pitch bend */
void ags_midi_smf_util_put_pitch_bend(AgsMidiSmfUtil *midi_smf_util,
				      guchar *buffer,
				      gint delta_time,
				      gint channel,
				      gint pitch,
				      gint transmitter);
guint ags_midi_smf_util_get_pitch_bend(AgsMidiSmfUtil *midi_smf_util,
				       guchar *buffer,
				       gint *delta_time,
				       gint *channel,
				       gint *pitch,
				       gint *transmitter);

/* change program */
void ags_midi_smf_util_put_change_program(AgsMidiSmfUtil *midi_smf_util,
					  guchar *buffer,
					  gint delta_time,
					  gint channel,
					  gint program);
guint ags_midi_smf_util_get_change_program(AgsMidiSmfUtil *midi_smf_util,
					   guchar *buffer,
					   gint *delta_time,
					   gint *channel,
					   gint *program);

/* change pressure */
void ags_midi_smf_util_put_change_pressure(AgsMidiSmfUtil *midi_smf_util,
					   guchar *buffer,
					   gint delta_time,
					   gint channel,
					   gint pressure);
guint ags_midi_smf_util_get_change_pressure(AgsMidiSmfUtil *midi_smf_util,
					    guchar *buffer,
					    gint *delta_time,
					    gint *channel,
					    gint *pressure);

/* sysex and system common */
/* sysex */
void ags_midi_smf_util_put_sysex(AgsMidiSmfUtil *midi_smf_util,
				 guchar *buffer,
				 gint delta_time,
				 guchar *data, gint length);
guint ags_midi_smf_util_get_sysex(AgsMidiSmfUtil *midi_smf_util,
				  guchar *buffer,
				  gint *delta_time,
				  guchar **data, gint *length);

/* quarter frame */
void ags_midi_smf_util_put_quarter_frame(AgsMidiSmfUtil *midi_smf_util,
					 guchar *buffer,
					 gint delta_time,
					 gint message_type,
					 gint values);
guint ags_midi_smf_util_get_quarter_frame(AgsMidiSmfUtil *midi_smf_util,
					  guchar *buffer,
					  gint *delta_time,
					  gint *message_type, gint *values);

/* song position */
void ags_midi_smf_util_put_song_position(AgsMidiSmfUtil *midi_smf_util,
					 guchar *buffer,
					 gint delta_time,
					 gint song_position);
guint ags_midi_smf_util_get_song_position(AgsMidiSmfUtil *midi_smf_util,
					  guchar *buffer,
					  gint *delta_time,
					  gint *song_position);

/* song select */
void ags_midi_smf_util_put_song_select(AgsMidiSmfUtil *midi_smf_util,
				       guchar *buffer,
				       gint delta_time,
				       gint song_select);
guint ags_midi_smf_util_get_song_select(AgsMidiSmfUtil *midi_smf_util,
					guchar *buffer,
					gint *delta_time,
					gint *song_select);

/* tune request */
void ags_midi_smf_util_put_tune_request(AgsMidiSmfUtil *midi_smf_util,
					guchar *buffer,
					gint delta_time);
guint ags_midi_smf_util_get_tune_request(AgsMidiSmfUtil *midi_smf_util,
					 guchar *buffer,
					 gint *delta_time);

/* meta event */
/* sequence number */
void ags_midi_smf_util_put_sequence_number(AgsMidiSmfUtil *midi_smf_util,
					   guchar *buffer,
					   gint delta_time,
					   gint sequence);
guint ags_midi_smf_util_get_sequence_number(AgsMidiSmfUtil *midi_smf_util,
					    guchar *buffer,
					    gint *delta_time,
					    gint *sequence);

/* smtpe */
void ags_midi_smf_util_put_smtpe(AgsMidiSmfUtil *midi_smf_util,
				 guchar *buffer,
				 gint delta_time,
				 gint rr, gint hr, gint mn, gint se, gint fr);
guint ags_midi_smf_util_get_smtpe(AgsMidiSmfUtil *midi_smf_util,
				  guchar *buffer,
				  gint *delta_time,
				  gint *rr, gint *hr, gint *mn, gint *se, gint *fr);

/* tempo */
void ags_midi_smf_util_put_tempo(AgsMidiSmfUtil *midi_smf_util,
				 guchar *buffer,
				 gint delta_time,
				 gint tempo);
guint ags_midi_smf_util_get_tempo(AgsMidiSmfUtil *midi_smf_util,
				  guchar *buffer,
				  gint *delta_time,
				  gint *tempo);

/* time signature */
void ags_midi_smf_util_put_time_signature(AgsMidiSmfUtil *midi_smf_util,
					  guchar *buffer,
					  gint delta_time,
					  gint nn, gint dd, gint cc, gint bb);
guint ags_midi_smf_util_get_time_signature(AgsMidiSmfUtil *midi_smf_util,
					   guchar *buffer,
					   gint *delta_time,
					   gint *nn, gint *dd, gint *cc, gint *bb);

/* key signature */
void ags_midi_smf_util_put_key_signature(AgsMidiSmfUtil *midi_smf_util,
					 guchar *buffer,
					 gint delta_time,
					 gint sf, gint mi);
guint ags_midi_smf_util_get_key_signature(AgsMidiSmfUtil *midi_smf_util,
					  guchar *buffer,
					  gint *delta_time,
					  gint *sf, gint *mi);

/* sequencer meta event */
void ags_midi_smf_util_put_sequencer_meta_event(AgsMidiSmfUtil *midi_smf_util,
						guchar *buffer,
						gint delta_time,
						gint len, gint id, gint data);
guint ags_midi_smf_util_get_sequencer_meta_event(AgsMidiSmfUtil *midi_smf_util,
						 guchar *buffer,
						 gint *delta_time,
						 gint *len, gint *id, gint *data);

/* text event */
void ags_midi_smf_util_put_text_event(AgsMidiSmfUtil *midi_smf_util,
				      guchar *buffer,
				      gint delta_time,
				      gchar *text, gint length);
guint ags_midi_smf_util_get_text_event(AgsMidiSmfUtil *midi_smf_util,
				       guchar *buffer,
				       gint *delta_time,
				       gchar **text, gint *length);


/* end of track */
void ags_midi_smf_util_put_end_of_track(AgsMidiSmfUtil *midi_smf_util,
					guchar *buffer,
					gint delta_time);
guint ags_midi_smf_util_get_end_of_track(AgsMidiSmfUtil *midi_smf_util,
					 guchar *buffer,
					 gint *delta_time);

/* seek */
guchar* ags_midi_smf_util_seek_message(AgsMidiSmfUtil *midi_smf_util,
				       guchar *buffer,
				       guint message_count,
				       gint *delta_time);

/* ALSA sequencer */
/* decode */
guint ags_midi_smf_util_decode(AgsMidiSmfUtil *midi_smf_util,
			       guchar *buffer,
			       snd_seq_event_t *event);

G_END_DECLS

#endif /*__AGS_MIDI_SMF_UTIL_H__*/
