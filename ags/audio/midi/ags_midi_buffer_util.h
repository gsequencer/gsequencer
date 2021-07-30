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

#ifndef __AGS_MIDI_BUFFER_UTIL_H__
#define __AGS_MIDI_BUFFER_UTIL_H__

#include <glib.h>
#include <glib-object.h>

#include <alsa/seq_event.h>

#include <ags/libags.h>

G_BEGIN_DECLS

#define AGS_TYPE_MIDI_BUFFER_UTIL         (ags_midi_buffer_util_get_type())

typedef struct _AgsMidiBufferUtil AgsMidiBufferUtil;

typedef enum{
  AGS_MIDI_BUFFER_UTIL_MTC_QUARTER_FRAME_FRAME_NUMBER_LSB   = 0x0,
  AGS_MIDI_BUFFER_UTIL_MTC_QUARTER_FRAME_FRAME_NUMBER_MSB   = 0x10,
  AGS_MIDI_BUFFER_UTIL_MTC_QUARTER_FRAME_SECONDS_LSB        = 0x20,
  AGS_MIDI_BUFFER_UTIL_MTC_QUARTER_FRAME_SECONDS_MSB        = 0x30,
  AGS_MIDI_BUFFER_UTIL_MTC_QUARTER_FRAME_MINUTES_LSB        = 0x40,
  AGS_MIDI_BUFFER_UTIL_MTC_QUARTER_FRAME_MINUTES_MSB        = 0x50,
  AGS_MIDI_BUFFER_UTIL_MTC_QUARTER_FRAME_HOURS_LSB          = 0x60,
  AGS_MIDI_BUFFER_UTIL_MTC_QUARTER_FRAME_HOURS_MSB          = 0x70,
}AgsMidiBufferUtilMtcQuarterFrameMessageType;

typedef enum{
  AGS_MIDI_BUFFER_UTIL_SMTPE_FRAME_RATE_24_FPS             = 0x0,
  AGS_MIDI_BUFFER_UTIL_SMTPE_FRAME_RATE_25_FPS             = 0x40,
  AGS_MIDI_BUFFER_UTIL_SMTPE_FRAME_RATE_30_FPS             = 0x80,
  AGS_MIDI_BUFFER_UTIL_SMTPE_FRAME_RATE_30_FPS_DROP_FRAME  = 0xc0,
}AgsMidiBufferUtilSmtpeFrameRate;

struct _AgsMidiBufferUtil
{
  //empty
};

GType ags_midi_buffer_util_get_type(void);

/* varlength */
guint ags_midi_buffer_util_get_varlength_size(glong varlength);
void ags_midi_buffer_util_put_varlength(guchar *buffer,
					glong varlength);
guint ags_midi_buffer_util_get_varlength(guchar *buffer,
					 glong *varlength);

/* integer - 2 bytes */
void ags_midi_buffer_util_put_int16(guchar *buffer,
				    glong val);
void ags_midi_buffer_util_get_int16(guchar *buffer,
				    glong *val);

/* integer - 3 bytes */
void ags_midi_buffer_util_put_int24(guchar *buffer,
				    glong val);
void ags_midi_buffer_util_get_int24(guchar *buffer,
				    glong *val);

/* integer - 4 bytes */
void ags_midi_buffer_util_put_int32(guchar *buffer,
				    glong val);
void ags_midi_buffer_util_get_int32(guchar *buffer,
				    glong *val);

/* channel message */
/* midi header */
void ags_midi_buffer_util_put_header(guchar *buffer,
				     glong offset, glong format,
				     glong track_count, glong division);

guint ags_midi_buffer_util_get_header(guchar *buffer,
				      glong *offset, glong *format,
				      glong *track_count, glong *division);

/* midi track */
void ags_midi_buffer_util_put_track(guchar *buffer,
				    glong offset);
guint ags_midi_buffer_util_get_track(guchar *buffer,
				     glong *offset);

/* key on */
void ags_midi_buffer_util_put_key_on(guchar *buffer,
				     glong delta_time,
				     glong channel,
				     glong key,
				     glong velocity);

guint ags_midi_buffer_util_get_key_on(guchar *buffer,
				      glong *delta_time,
				      glong *channel,
				      glong *key,
				      glong *velocity);

/* key off */
void ags_midi_buffer_util_put_key_off(guchar *buffer,
				      glong delta_time,
				      glong channel,
				      glong key,
				      glong velocity);

guint ags_midi_buffer_util_get_key_off(guchar *buffer,
				       glong *delta_time,
				       glong *channel,
				       glong *key,
				       glong *velocity);

/* key pressure */
void ags_midi_buffer_util_put_key_pressure(guchar *buffer,
					   glong delta_time,
					   glong channel,
					   glong key,
					   glong pressure);

guint ags_midi_buffer_util_get_key_pressure(guchar *buffer,
					    glong *delta_time,
					    glong *channel,
					    glong *key,
					    glong *pressure);

/* change parameter */
void ags_midi_buffer_util_put_change_parameter(guchar *buffer,
					       glong delta_time,
					       glong channel,
					       glong control,
					       glong value);
guint ags_midi_buffer_util_get_change_parameter(guchar *buffer,
						glong *delta_time,
						glong *channel,
						glong *control,
						glong *value);

/* pitch bend */
void ags_midi_buffer_util_put_pitch_bend(guchar *buffer,
					 glong delta_time,
					 glong channel,
					 glong pitch,
					 glong transmitter);
guint ags_midi_buffer_util_get_pitch_bend(guchar *buffer,
					  glong *delta_time,
					  glong *channel,
					  glong *pitch,
					  glong *transmitter);

/* change program */
void ags_midi_buffer_util_put_change_program(guchar *buffer,
					     glong delta_time,
					     glong channel,
					     glong program);
guint ags_midi_buffer_util_get_change_program(guchar *buffer,
					      glong *delta_time,
					      glong *channel,
					      glong *program);

/* change pressure */
void ags_midi_buffer_util_put_change_pressure(guchar *buffer,
					      glong delta_time,
					      glong channel,
					      glong pressure);
guint ags_midi_buffer_util_get_change_pressure(guchar *buffer,
					       glong *delta_time,
					       glong *channel,
					       glong *pressure);

/* sysex and system common */
/* sysex */
void ags_midi_buffer_util_put_sysex(guchar *buffer,
				    glong delta_time,
				    guchar *data, glong length);
guint ags_midi_buffer_util_get_sysex(guchar *buffer,
				     glong *delta_time,
				     guchar **data, glong *length);

/* quarter frame */
void ags_midi_buffer_util_put_quarter_frame(guchar *buffer,
					    glong delta_time,
					    glong message_type,
					    glong values);
guint ags_midi_buffer_util_get_quarter_frame(guchar *buffer,
					     glong *delta_time,
					     glong *message_type, glong *values);

/* song position */
void ags_midi_buffer_util_put_song_position(guchar *buffer,
					    glong delta_time,
					    glong song_position);
guint ags_midi_buffer_util_get_song_position(guchar *buffer,
					     glong *delta_time,
					     glong *song_position);

/* song select */
void ags_midi_buffer_util_put_song_select(guchar *buffer,
					  glong delta_time,
					  glong song_select);
guint ags_midi_buffer_util_get_song_select(guchar *buffer,
					   glong *delta_time,
					   glong *song_select);

/* tune request */
void ags_midi_buffer_util_put_tune_request(guchar *buffer,
					   glong delta_time);
guint ags_midi_buffer_util_get_tune_request(guchar *buffer,
					    glong *delta_time);

/* meta event */
/* sequence number */
void ags_midi_buffer_util_put_sequence_number(guchar *buffer,
					      glong delta_time,
					      glong sequence);
guint ags_midi_buffer_util_get_sequence_number(guchar *buffer,
					       glong *delta_time,
					       glong *sequence);

/* smtpe */
void ags_midi_buffer_util_put_smtpe(guchar *buffer,
				    glong delta_time,
				    glong rr, glong hr, glong mn, glong se, glong fr);
guint ags_midi_buffer_util_get_smtpe(guchar *buffer,
				     glong *delta_time,
				     glong *rr, glong *hr, glong *mn, glong *se, glong *fr);

/* tempo */
void ags_midi_buffer_util_put_tempo(guchar *buffer,
				    glong delta_time,
				    glong tempo);
guint ags_midi_buffer_util_get_tempo(guchar *buffer,
				     glong *delta_time,
				     glong *tempo);

/* time signature */
void ags_midi_buffer_util_put_time_signature(guchar *buffer,
					     glong delta_time,
					     glong nn, glong dd, glong cc, glong bb);
guint ags_midi_buffer_util_get_time_signature(guchar *buffer,
					      glong *delta_time,
					      glong *nn, glong *dd, glong *cc, glong *bb);

/* key signature */
void ags_midi_buffer_util_put_key_signature(guchar *buffer,
					    glong delta_time,
					    glong sf, glong mi);
guint ags_midi_buffer_util_get_key_signature(guchar *buffer,
					     glong *delta_time,
					     glong *sf, glong *mi);

/* sequencer meta event */
void ags_midi_buffer_util_put_sequencer_meta_event(guchar *buffer,
						   glong delta_time,
						   glong len, glong id, glong data);
guint ags_midi_buffer_util_get_sequencer_meta_event(guchar *buffer,
						    glong *delta_time,
						    glong *len, glong *id, glong *data);

/* text event */
void ags_midi_buffer_util_put_text_event(guchar *buffer,
					 glong delta_time,
					 gchar *text, glong length);
guint ags_midi_buffer_util_get_text_event(guchar *buffer,
					  glong *delta_time,
					  gchar **text, glong *length);


/* end of track */
void ags_midi_buffer_util_put_end_of_track(guchar *buffer,
					   glong delta_time);
guint ags_midi_buffer_util_get_end_of_track(guchar *buffer,
					    glong *delta_time);

/* seek */
guchar* ags_midi_buffer_util_seek_message(guchar *buffer,
					  guint message_count,
					  glong *delta_time);

/* ALSA sequencer */
/* decode */
guint ags_midi_buffer_util_decode(guchar *buffer,
				  snd_seq_event_t *event);

G_END_DECLS

#endif /*__AGS_MIDI_BUFFER_UTIL_H__*/
