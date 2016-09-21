/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2015 Joël Krähemann
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

#ifndef __AGS_MIDI_BUILDER_H__
#define __AGS_MIDI_BUILDER_H__

#include <glib.h>
#include <glib-object.h>

#include <libxml/tree.h>

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#define AGS_TYPE_MIDI_BUILDER                (ags_midi_builder_get_type ())
#define AGS_MIDI_BUILDER(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_MIDI_BUILDER, AgsMidiBuilder))
#define AGS_MIDI_BUILDER_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_MIDI_BUILDER, AgsMidiBuilderClass))
#define AGS_IS_MIDI_BUILDER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_MIDI_BUILDER))
#define AGS_IS_MIDI_BUILDER_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_MIDI_BUILDER))
#define AGS_MIDI_BUILDER_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_MIDI_BUILDER, AgsMidiBuilderClass))

typedef struct _AgsMidiBuilder AgsMidiBuilder;
typedef struct _AgsMidiBuilderClass AgsMidiBuilderClass;
typedef struct _AgsMidiBuilderHeader AgsMidiBuilderHeader;
typedef struct _AgsMidiBuilderTrack AgsMidiBuilderTrack;

struct _AgsMidiBuilder
{
  GObject gobject;

  FILE *file;
  
  AgsMidiBuilderHeader *midi_header;
  
  GList *midi_track;
};

struct _AgsMidiBuilderClass
{
  GObjectClass gobject;

  void (*midi_putc)(AgsMidiBuilder *midi_builder,
		    int c);
  void (*on_error)(AgsMidiBuilder *midi_builder,
		   GError **error);

  /* channel messages */
  void (*append_header)(AgsMidiBuilder *midi_builder,
			guint offset, guint format,
			guint track_count, guint division,
			guint times, guint bpm,
			guint clicks);
  void (*append_track)(AgsMidiBuilder *midi_builder,
		       gchar *track_name);

  void (*append_key_on)(AgsMidiBuilder *midi_builder,
			guint delta_time,
			guint audio_channel,
			guint note,
			guint velocity);
  void (*append_key_off)(AgsMidiBuilder *midi_builder,
			 guint delta_time,
			 guint audio_channel,
			 guint note,
			 guint velocity);
  void (*append_key_pressure)(AgsMidiBuilder *midi_builder,
			      guint delta_time,
			      guint audio_channel,
			      guint note,
			      guint pressure);

  void (*append_change_parameter)(AgsMidiBuilder *midi_builder,
				  guint delta_time,
				  guint channel,
				  guint control,
				  guint value);
  void (*append_change_pitch_bend)(AgsMidiBuilder *midi_builder,
				   guint delta_time,
				   guint channel,
				   guint pitch,
				   guint transmitter);
  void (*append_change_program)(AgsMidiBuilder *midi_builder,
				guint delta_time,
				guint channel,
				guint program);
  void (*append_change_pressure)(AgsMidiBuilder *midi_builder,
				 guint delta_time,
				 guint channel,
				 guint pressure);

  /* sysex and system common */
  void (*append_sysex)(AgsMidiBuilder *midi_builder,
		       guint delta_time,
		       unsigned char *sysex_data, guint length);

  void (*append_quarter_frame)(AgsMidiBuilder *midi_builder,
			       guint delta_time,
			       guint message_type,
			       guint values);
  void (*append_song_position)(AgsMidiBuilder *midi_builder,
			       guint delta_time,
			       guint song_position);
  void (*append_song_select)(AgsMidiBuilder *midi_builder,
			     guint delta_time,
			     guint song_select);
  void (*append_tune_request)(AgsMidiBuilder *midi_builder,
				 guint delta_time);
  
  /* meta events */
  void (*append_sequence_number)(AgsMidiBuilder *midi_builder,
				 guint delta_time,
				 guint sequence);
  void (*append_smtpe)(AgsMidiBuilder *midi_builder,
		       guint delta_time,
		       guint hr, guint mn, guint se, guint fr, guint ff);
  void (*append_tempo)(AgsMidiBuilder *midi_builder,
		       guint delta_time,
		       guint tempo);  
  void (*append_time_signature)(AgsMidiBuilder *midi_builder,
				guint delta_time,
				guint nn, guint dd, guint cc, guint bb);
  void (*append_key_signature)(AgsMidiBuilder *midi_builder,
			       guint delta_time,
			       guint sf, guint mi);
  void (*append_sequencer_meta_event)(AgsMidiBuilder *midi_builder,
				      guint delta_time,
				      guint len, guint id, guint data);
  void (*append_text_event)(AgsMidiBuilder *midi_builder,
			    guint delta_time,
			    gchar *text, guint length);
};

typedef struct _AgsMidiBuilderHeader
{
  guint offset;
  guint format;
  guint count;
  guint division;
  guint times;
  guint beat;
  guint clicks;
};

typedef struct _AgsMidiBuilderTrack
{
  gchar *track_name;
  
  unsigned char *data;
  guint length;
};

GType ags_midi_builder_get_type(void);

void ags_midi_builder_midi_putc(AgsMidiBuilder *midi_builder,
				int c);
void ags_midi_builder_on_error(AgsMidiBuilder *midi_builder,
			       GError **error);

/* channel messages */
void ags_midi_builder_append_header(AgsMidiBuilder *midi_builder,
				    guint offset, guint format,
				    guint track_count, guint division,
				    guint times, guint bpm,
				    guint clicks);
void ags_midi_builder_append_track(AgsMidiBuilder *midi_builder,
				   gchar *track_name);

void ags_midi_builder_append_key_on(AgsMidiBuilder *midi_builder,
				    guint delta_time,
				    guint audio_channel,
				    guint note,
				    guint velocity);
void ags_midi_builder_append_key_off(AgsMidiBuilder *midi_builder,
				     guint delta_time,
				     guint audio_channel,
				     guint note,
				     guint velocity);
void ags_midi_builder_append_key_pressure(AgsMidiBuilder *midi_builder,
					  guint delta_time,
					  guint audio_channel,
					  guint note,
					  guint pressure);

void ags_midi_builder_append_change_parameter(AgsMidiBuilder *midi_builder,
					      guint delta_time,
					      guint channel,
					      guint control,
					      guint value);
void ags_midi_builder_append_change_pitch_bend(AgsMidiBuilder *midi_builder,
					       guint delta_time,
					       guint channel,
					       guint pitch,
					       guint transmitter);
void ags_midi_builder_append_change_program(AgsMidiBuilder *midi_builder,
					    guint delta_time,
					    guint channel,
					    guint program);
void ags_midi_builder_append_change_pressure(AgsMidiBuilder *midi_builder,
					     guint delta_time,
					     guint channel,
					     guint pressure);

/* sysex and system common */
void ags_midi_builder_append_sysex(AgsMidiBuilder *midi_builder,
				   guint delta_time,
				   unsigned char *sysex_data, guint length);

void ags_midi_builder_append_quarter_frame(AgsMidiBuilder *midi_builder,
					   guint delta_time,
					   guint message_type,
					   guint values);
void ags_midi_builder_append_song_position(AgsMidiBuilder *midi_builder,
					   guint delta_time,
					   guint song_position);
void ags_midi_builder_append_song_select(AgsMidiBuilder *midi_builder,
					 guint delta_time,
					 guint song_select);
void ags_midi_builder_append_tune_request(AgsMidiBuilder *midi_builder,
					  guint delta_time);
  
/* meta events */
void ags_midi_builder_append_sequence_number(AgsMidiBuilder *midi_builder,
					     guint delta_time,
					     guint sequence);
void ags_midi_builder_append_smtpe(AgsMidiBuilder *midi_builder,
				   guint delta_time,
				   guint hr, guint mn, guint se, guint fr, guint ff);
void ags_midi_builder_append_tempo(AgsMidiBuilder *midi_builder,
				   guint delta_time,
				   guint tempo);  
void ags_midi_builder_append_time_signature(AgsMidiBuilder *midi_builder,
					    guint delta_time,
					    guint nn, guint dd, guint cc, guint bb);
void ags_midi_builder_append_key_signature(AgsMidiBuilder *midi_builder,
					   guint delta_time,
					   guint sf, guint mi);
void ags_midi_builder_append_sequencer_meta_event(AgsMidiBuilder *midi_builder,
						  guint delta_time,
						  guint len, guint id, guint data);
void ags_midi_builder_append_text_event(AgsMidiBuilder *midi_builder,
					guint delta_time,
					gchar *text, guint length);

/*  */
void ags_midi_builder_append_xml_node(AgsMidiBuilder *midi_builder,
				      xmlNode *node);

void ags_midi_builder_from_xml_doc(AgsMidiBuilder *midi_builder,
				   xmlDoc *doc);

/*  */
void ags_midi_builder_build(AgsMidiBuilder *midi_builder);

/*  */
AgsMidiBuilder* ags_midi_builder_new(FILE *file);

#endif /*__AGS_MIDI_BUILDER_H__*/
