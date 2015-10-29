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

#include <ags/audio/midi/ags_midi_builder.h>

#include <ags/object/ags_marshal.h>

#include <fcntl.h>
#include <sys/stat.h>

void ags_midi_builder_class_init(AgsMidiBuilderClass *midi_builder);
void ags_midi_builder_init(AgsMidiBuilder *midi_builder);
void ags_midi_builder_finalize(GObject *gobject);

enum{
  ON_ERROR,
  APPEND_HEADER,
  APPEND_TRACK,
  KEY_ON,
  KEY_OFF,
  KEY_PRESSURE,
  CHANGE_PARAMETER,
  CHANGE_PITCH_BEND,
  CHANGE_PROGRAM,
  CHANGE_PRESSURE,
  SYSEX,
  QUARTER_FRAME,
  SONG_POSITION,
  SONG_SELECT,
  TUNE_REQUEST,
  SEQUENCE_NUMBER,
  SMPTE,
  TEMPO,
  TIME_SIGNATURE,
  KEY_SIGNATURE,
  SEQUENCER_META_EVENT,
  TEXT_EVENT,
  LAST_SIGNAL,
};

static gpointer ags_midi_builder_parent_class = NULL;
static guint midi_builder_signals[LAST_SIGNAL];

GType
ags_midi_builder_get_type(void)
{
  static GType ags_type_midi_builder = 0;

  if(!ags_type_midi_builder){
    static const GTypeInfo ags_midi_builder_info = {
      sizeof (AgsMidiBuilderClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_midi_builder_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsMidiBuilder),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_midi_builder_init,
    };

    ags_type_midi_builder = g_type_register_static(G_TYPE_OBJECT,
						   "AgsMidiBuilder\0", &ags_midi_builder_info,
						   0);
  }

  return(ags_type_midi_builder);
}

void
ags_midi_builder_class_init(AgsMidiBuilderClass *midi_builder)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;
  
  ags_midi_builder_parent_class = g_type_class_peek_parent(midi_builder);

  /* GObjectClass */
  gobject = (GObjectClass *) midi_builder;
  
  gobject->finalize = ags_midi_builder_finalize;

  /* AgsMidiBuilderClass */
  midi_builder->midi_putc = ags_midi_builder_midi_putc;
  midi_builder->on_error = ags_midi_builder_on_error;
  midi_builder->append_header = ags_midi_builder_append_header;
  midi_builder->append_track = ags_midi_builder_append_track;
  midi_builder->append_key_on = ags_midi_builder_append_key_on;
  midi_builder->append_key_off = ags_midi_builder_append_key_off;
  midi_builder->append_key_pressure = ags_midi_builder_append_key_pressure;
  midi_builder->append_change_parameter = ags_midi_builder_append_change_parameter;
  midi_builder->append_change_pitch_bend = ags_midi_builder_append_change_pitch_bend;
  midi_builder->append_change_pitch_program = ags_midi_builder_append_change_pitch_program;
  midi_builder->append_change_pressure = ags_midi_builder_append_change_pressure;
  midi_builder->append_sysex = ags_midi_builder_append_sysex;
  midi_builder->append_quarter_frame = ags_midi_builder_append_quarter_frame;
  midi_builder->append_song_position = ags_midi_builder_append_song_position;
  midi_builder->append_song_select = ags_midi_builder_append_song_select;
  midi_builder->append_tune_request = ags_midi_builder_append_tune_request;
  midi_builder->append_sequence_number = ags_midi_builder_append_sequence_number;
  midi_builder->append_smtpe = ags_midi_builder_append_smtpe;
  midi_builder->append_tempo = ags_midi_builder_append_tempo;
  midi_builder->append_time_signature = ags_midi_builder_append_time_signature;
  midi_builder->append_key_signature = ags_midi_builder_append_key_signature;
  midi_builder->append_sequencer_meta_event = ags_midi_builder_append_sequencer_meta_event;
  midi_builder->append_text_event = ags_midi_builder_append_text_event;
}

void
ags_midi_builder_init(AgsMidiBuilder *midi_builder)
{
}

void
ags_midi_builder_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_midi_builder_parent_class)->finalize(gobject);
}

void
ags_midi_builder_midi_putc(AgsMidiBuilder *midi_builder,
			   int c)
{
}

void
ags_midi_builder_on_error(AgsMidiBuilder *builder,
			  GError **error)
{
}

void
ags_midi_builder_append_header(AgsMidiBuilder *midi_builder,
			       guint offset, guint format,
			       guint track_count, guint division,
			       guint times, guint bpm,
			       guint clicks)
{
}

void
ags_midi_builder_append_track(AgsMidiBuilder *midi_builder,
			      gchar *track_name)
{
}

void
ags_midi_builder_append_key_on(AgsMidiBuilder *midi_builder,
			       guint audio_channel,
			       guint note,
			       guint velocity)
{
}

void
ags_midi_builder_append_key_off(AgsMidiBuilder *midi_builder,
				guint audio_channel,
				guint note,
				guint velocity)
{
}

void
ags_midi_builder_append_key_pressure(AgsMidiBuilder *midi_builder,
				     guint audio_channel,
				     guint note,
				     guint pressure)
{
}

void
ags_midi_builder_append_change_parameter(AgsMidiBuilder *midi_builder,
					 guint channel,
					 guint control,
					 guint value)
{
}

void
ags_midi_builder_append_change_pitch_bend(AgsMidiBuilder *midi_builder,
					  guint channel,
					  guint pitch,
					  guint transmitter)
{
}

void
ags_midi_builder_append_change_pitch_program(AgsMidiBuilder *midi_builder,
					     guint channel,
					     guint program)
{
}

void
ags_midi_builder_append_change_pressure(AgsMidiBuilder *midi_builder,
					guint channel,
					guint pressure)
{
}

void
ags_midi_builder_append_sysex(AgsMidiBuilder *midi_builder,
			      unsigned char *sysex_data, guint length)
{
}

void
ags_midi_builder_append_quarter_frame(AgsMidiBuilder *midi_builder,
				      guint quarter_frame)
{
}

void
ags_midi_builder_append_song_position(AgsMidiBuilder *midi_builder,
				      guint song_position)
{
}

void
ags_midi_builder_append_song_select(AgsMidiBuilder *midi_builder,
				    guint song_select)
{
}

void
ags_midi_builder_append_tune_request(AgsMidiBuilder *midi_builder)
{
}

void
ags_midi_builder_append_sequence_number(AgsMidiBuilder *midi_builder,
					guint sequence)
{
}

void
ags_midi_builder_append_smtpe(AgsMidiBuilder *midi_builder,
			      guint hr, guint mn, guint se, guint fr, guint ff)
{
}

void
ags_midi_builder_append_tempo(AgsMidiBuilder *midi_builder,
			      guint tempo)
{
}
  
void
ags_midi_builder_append_time_signature(AgsMidiBuilder *midi_builder,
				       guint nn, guint denom, guint dd, guint cc, guint bb)
{
}

void
ags_midi_builder_append_key_signature(AgsMidiBuilder *midi_builder,
				      guint nn, guint denom, guint dd, guint cc, guint bb)
{
}

void
ags_midi_builder_append_sequencer_meta_event(AgsMidiBuilder *midi_builder,
					     guint len, guint id, guint data)
{
}

void
ags_midi_builder_append_text_event(AgsMidiBuilder *midi_builder,
				   gchar *text, guint length)
{
}

void
ags_midi_builder_append_xml_node(AgsMidiBuilder *midi_builder,
				 xmlNode *node)
{
  //TODO:JK: implement me
}

void
ags_midi_builder_from_xml_doc(AgsMidiBuilder *midi_builder,
			      xmlDoc *doc)
{
  //TODO:JK: implement me
}

void
ags_midi_builder_build(AgsMidiBuilder *midi_builder)
{
  //TODO:JK: implement me
}

AgsMidiBuilder*
ags_midi_builder_new(FILE *file)
{
  AgsMidiBuilder *midi_builder;
  
  midi_builder = (AgsMidiBuilder *) g_object_new(AGS_TYPE_MIDI_BUILDER,
						 NULL);

  
  return(midi_builder);
}
