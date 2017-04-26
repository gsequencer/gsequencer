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

#include <ags/audio/midi/ags_midi_buffer_util.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>

void ags_midi_builder_class_init(AgsMidiBuilderClass *midi_builder);
void ags_midi_builder_init(AgsMidiBuilder *midi_builder);
void ags_midi_builder_set_property(GObject *gobject,
				   guint prop_id,
				   const GValue *value,
				   GParamSpec *param_spec);
void ags_midi_builder_get_property(GObject *gobject,
				   guint prop_id,
				   GValue *value,
				   GParamSpec *param_spec);
void ags_midi_builder_finalize(GObject *gobject);

void ags_midi_builder_real_midi_putc(AgsMidiBuilder *midi_builder,
				     int c);
void ags_midi_builder_real_on_error(AgsMidiBuilder *builder,
				    GError **error);

/* channel messages */
void ags_midi_builder_real_append_header(AgsMidiBuilder *midi_builder,
					 guint offset, guint format,
					 guint track_count, guint division,
					 guint times, guint bpm,
					 guint clicks);
void ags_midi_builder_real_append_track(AgsMidiBuilder *midi_builder,
					gchar *track_name);

void ags_midi_builder_real_append_key_on(AgsMidiBuilder *midi_builder,
					 guint delta_time,
					 guint audio_channel,
					 guint note,
					 guint velocity);
void ags_midi_builder_real_append_key_off(AgsMidiBuilder *midi_builder,
					  guint delta_time,
					  guint audio_channel,
					  guint note,
					  guint velocity);
void ags_midi_builder_real_append_key_pressure(AgsMidiBuilder *midi_builder,
					       guint delta_time,
					       guint audio_channel,
					       guint note,
					       guint pressure);

void ags_midi_builder_real_append_change_parameter(AgsMidiBuilder *midi_builder,
						   guint delta_time,
						   guint channel,
						   guint control,
						   guint value);
void ags_midi_builder_real_append_change_pitch_bend(AgsMidiBuilder *midi_builder,
						    guint delta_time,
						    guint channel,
						    guint pitch,
						    guint transmitter);
void ags_midi_builder_real_append_change_program(AgsMidiBuilder *midi_builder,
						 guint delta_time,
						 guint channel,
						 guint program);
void ags_midi_builder_real_append_change_pressure(AgsMidiBuilder *midi_builder,
						  guint delta_time,
						  guint channel,
						  guint pressure);

/* sysex and system common */
void ags_midi_builder_real_append_sysex(AgsMidiBuilder *midi_builder,
					guint delta_time,
					unsigned char *sysex_data, guint length);

void ags_midi_builder_real_append_quarter_frame(AgsMidiBuilder *midi_builder,
						guint delta_time,
						guint message_type,
						guint values);
void ags_midi_builder_real_append_song_position(AgsMidiBuilder *midi_builder,
						guint delta_time,
						guint song_position);
void ags_midi_builder_real_append_song_select(AgsMidiBuilder *midi_builder,
					      guint delta_time,
					      guint song_select);
void ags_midi_builder_real_append_tune_request(AgsMidiBuilder *midi_builder,
					       guint delta_time);
  
/* meta events */
void ags_midi_builder_real_append_sequence_number(AgsMidiBuilder *midi_builder,
						  guint delta_time,
						  guint sequence);
void ags_midi_builder_real_append_smtpe(AgsMidiBuilder *midi_builder,
					guint delta_time,
					guint rr, guint hr, guint mn, guint se, guint fr);
void ags_midi_builder_real_append_tempo(AgsMidiBuilder *midi_builder,
					guint delta_time,
					guint tempo);  
void ags_midi_builder_real_append_time_signature(AgsMidiBuilder *midi_builder,
						 guint delta_time,
						 guint nn, guint dd, guint cc, guint bb);
void ags_midi_builder_real_append_key_signature(AgsMidiBuilder *midi_builder,
						guint delta_time,
						guint sf, guint mi);
void ags_midi_builder_real_append_sequencer_meta_event(AgsMidiBuilder *midi_builder,
						       guint delta_time,
						       guint len, guint id, guint data);
void ags_midi_builder_real_append_text_event(AgsMidiBuilder *midi_builder,
					     guint delta_time,
					     gchar *text, guint length);

/**
 * SECTION:ags_midi_builder
 * @short_description: MIDI buidler
 * @title: AgsMidiBuilder
 * @section_id:
 * @include: ags/audio/midi/ags_midi_builder.h
 *
 * The #AgsMidiBuilder lets you create MIDI files using its builder functions.
 */

enum{
  PROP_0,
  PROP_FILE,
};

enum{
  MIDI_PUTC,
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
  SMTPE,
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
  
  gobject->set_property = ags_midi_builder_set_property;
  gobject->get_property = ags_midi_builder_get_property;  

  gobject->finalize = ags_midi_builder_finalize;


  /**
   * AgsMidiBuilder:file:
   *
   * The file to parse data from.
   * 
   * Since: 0.7.0
   */
  param_spec = g_param_spec_pointer("file\0",
				    "the file stream\0",
				    "The file stream to parse\0",
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FILE,
				  param_spec);

  /* AgsMidiBuilderClass */
  midi_builder->midi_putc = ags_midi_builder_real_midi_putc;
  midi_builder->on_error = ags_midi_builder_real_on_error;
  midi_builder->append_header = ags_midi_builder_real_append_header;
  midi_builder->append_track = ags_midi_builder_real_append_track;
  midi_builder->append_key_on = ags_midi_builder_real_append_key_on;
  midi_builder->append_key_off = ags_midi_builder_real_append_key_off;
  midi_builder->append_key_pressure = ags_midi_builder_real_append_key_pressure;
  midi_builder->append_change_parameter = ags_midi_builder_real_append_change_parameter;
  midi_builder->append_change_pitch_bend = ags_midi_builder_real_append_change_pitch_bend;
  midi_builder->append_change_program = ags_midi_builder_real_append_change_program;
  midi_builder->append_change_pressure = ags_midi_builder_real_append_change_pressure;
  midi_builder->append_sysex = ags_midi_builder_real_append_sysex;
  midi_builder->append_quarter_frame = ags_midi_builder_real_append_quarter_frame;
  midi_builder->append_song_position = ags_midi_builder_real_append_song_position;
  midi_builder->append_song_select = ags_midi_builder_real_append_song_select;
  midi_builder->append_tune_request = ags_midi_builder_real_append_tune_request;
  midi_builder->append_sequence_number = ags_midi_builder_real_append_sequence_number;
  midi_builder->append_smtpe = ags_midi_builder_real_append_smtpe;
  midi_builder->append_tempo = ags_midi_builder_real_append_tempo;
  midi_builder->append_time_signature = ags_midi_builder_real_append_time_signature;
  midi_builder->append_key_signature = ags_midi_builder_real_append_key_signature;
  midi_builder->append_sequencer_meta_event = ags_midi_builder_real_append_sequencer_meta_event;
  midi_builder->append_text_event = ags_midi_builder_real_append_text_event;

  /* signals */
  /**
   * AgsMidiBuilder::midi-putc:
   * @midi_builder: the builder
   * @error: the #GError
   *
   * The ::midi-putc signal is emited during putting char to file.
   *
   * Since: 0.7.90
   */
  midi_builder_signals[MIDI_PUTC] =
    g_signal_new("midi-putc\0",
		 G_TYPE_FROM_CLASS(midi_builder),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsMidiBuilderClass, midi_putc),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__INT,
		 G_TYPE_NONE, 1,
		 G_TYPE_INT);

  /**
   * AgsMidiBuilder::on-error:
   * @midi_builder: the builder
   * @error: the #GError
   *
   * The ::on-error signal is emited during building of event.
   *
   * Since: 0.7.0
   */
  midi_builder_signals[ON_ERROR] =
    g_signal_new("on-error\0",
		 G_TYPE_FROM_CLASS(midi_builder),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsMidiBuilderClass, on_error),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__POINTER,
		 G_TYPE_NONE, 1,
		 G_TYPE_POINTER);

  /**
   * AgsMidiBuilder::append-header:
   * @midi_builder: the builder
   * @offset: start delta-time
   * @format: format 0, 1 or 2
   * @track_count: the number of tracks
   * @division: timing division
   * @times: pulses per quarter note
   * @bpm: beats per minute
   * @clicks: timing clicks
   *
   * The ::append-header signal is emited during building of header.
   *
   * Since: 0.7.0
   */
  midi_builder_signals[APPEND_HEADER] =
    g_signal_new("append-header\0",
		 G_TYPE_FROM_CLASS(midi_builder),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsMidiBuilderClass, append_header),
		 NULL, NULL,
		 g_cclosure_user_marshal_VOID__UINT_UINT_UINT_UINT_UINT_UINT_UINT,
		 G_TYPE_NONE, 7,
		 G_TYPE_UINT, G_TYPE_UINT,
		 G_TYPE_UINT, G_TYPE_UINT,
		 G_TYPE_UINT, G_TYPE_UINT,
		 G_TYPE_UINT);

  /**
   * AgsMidiBuilder::append-track:
   * @midi_builder: the builder
   * @track_name: the trach name
   *
   * Returns: The XML node representing the track
   *
   * The ::append-track signal is emited during building of track.
   *
   * Since: 0.7.0
   */
  midi_builder_signals[APPEND_TRACK] =
    g_signal_new("append-track\0",
		 G_TYPE_FROM_CLASS(midi_builder),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsMidiBuilderClass, append_track),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__STRING,
		 G_TYPE_NONE, 1,
		 G_TYPE_STRING);

  /**
   * AgsMidiBuilder::key-on:
   * @midi_builder: the builder
   * @delta_time: delta-time
   * @audio_channel: the audio channel
   * @note: the note to play from 0 to 128
   * @velocity: key dynamics
   *
   * The ::key-on signal is emited during building of event.
   *
   * Since: 0.7.0
   */
  midi_builder_signals[KEY_ON] =
    g_signal_new("key-on\0",
		 G_TYPE_FROM_CLASS(midi_builder),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsMidiBuilderClass, append_key_on),
		 NULL, NULL,
		 g_cclosure_user_marshal_VOID__UINT_UINT_UINT_UINT,
		 G_TYPE_NONE, 4,
		 G_TYPE_UINT,
		 G_TYPE_UINT,
		 G_TYPE_UINT,
		 G_TYPE_UINT);

  /**
   * AgsMidiBuilder::key-off:
   * @midi_builder: the builders
   * @delta_time: delta-time
   * @audio_channel: the audio channel
   * @note: the note to stop play from 0 to 128
   * @velocity: key dynamics
   *
   * The ::key-off signal is emited during building of event.
   *
   * Since: 0.7.0
   */
  midi_builder_signals[KEY_OFF] =
    g_signal_new("key-off\0",
		 G_TYPE_FROM_CLASS(midi_builder),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsMidiBuilderClass, append_key_off),
		 NULL, NULL,
		 g_cclosure_user_marshal_VOID__UINT_UINT_UINT_UINT,
		 G_TYPE_NONE, 4,
		 G_TYPE_UINT,
		 G_TYPE_UINT,
		 G_TYPE_UINT,
		 G_TYPE_UINT);

  /**
   * AgsMidiBuilder::key-pressure:
   * @midi_builder: the builder
   * @delta_time: delta-time
   * @audio_channel: the audio channel
   * @note: the note to press from 0 to 128
   * @pressure: the amount of the pressure
   *
   * The ::key-pressure signal is emited during building of event.
   *
   * Since: 0.7.0
   */
  midi_builder_signals[KEY_PRESSURE] =
    g_signal_new("key-pressure\0",
		 G_TYPE_FROM_CLASS(midi_builder),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsMidiBuilderClass, append_key_pressure),
		 NULL, NULL,
		 g_cclosure_user_marshal_VOID__UINT_UINT_UINT_UINT,
		 G_TYPE_NONE, 4,
		 G_TYPE_UINT,
		 G_TYPE_UINT,
		 G_TYPE_UINT,
		 G_TYPE_UINT);

  /**
   * AgsMidiBuilder::change-parameter:
   * @midi_builder: the builder
   * @delta_time: delta-time
   * @channel: the audio channel
   * @control: the control
   * @value: and its value
   *
   * The ::change-parameter signal is emited during building of event.
   *
   * Since: 0.7.0
   */
  midi_builder_signals[CHANGE_PARAMETER] =
    g_signal_new("change-parameter\0",
		 G_TYPE_FROM_CLASS(midi_builder),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsMidiBuilderClass, append_change_parameter),
		 NULL, NULL,
		 g_cclosure_user_marshal_VOID__UINT_UINT_UINT_UINT,
		 G_TYPE_NONE, 4,
		 G_TYPE_UINT,
		 G_TYPE_UINT,
		 G_TYPE_UINT,
		 G_TYPE_UINT);

  /**
   * AgsMidiBuilder::change-pitch-bend:
   * @midi_builder: the builder
   * @delta_time: delta-time
   * @channel: the audio channel
   * @pitch: amount of pitch as 14 bit quantifier, 0, 0x2000 to 0x3fff
   * @transmitter: sensitivy of the wheel
   *
   * The ::change-pitch-bend signal is emited during building of event.
   *
   * Since: 0.7.0
   */
  midi_builder_signals[CHANGE_PITCH_BEND] =
    g_signal_new("change-pitch-bend\0",
		 G_TYPE_FROM_CLASS(midi_builder),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsMidiBuilderClass, append_change_pitch_bend),
		 NULL, NULL,
		 g_cclosure_user_marshal_VOID__UINT_UINT_UINT_UINT,
		 G_TYPE_NONE, 4,
		 G_TYPE_UINT,
		 G_TYPE_UINT,
		 G_TYPE_UINT,
		 G_TYPE_UINT);

  /**
   * AgsMidiBuilder::change-program:
   * @midi_builder: the builder
   * @channel: the audio channel
   * @program: the new programm
   *
   * The ::change-program signal is emited during building of event.
   *
   * Since: 0.7.0
   */
  midi_builder_signals[CHANGE_PROGRAM] =
    g_signal_new("change-program\0",
		 G_TYPE_FROM_CLASS(midi_builder),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsMidiBuilderClass, append_change_program),
		 NULL, NULL,
		 g_cclosure_user_marshal_VOID__UINT_UINT_UINT,
		 G_TYPE_NONE, 3,
		 G_TYPE_UINT,
		 G_TYPE_UINT,
		 G_TYPE_UINT);

  /**
   * AgsMidiBuilder::change-channel-pressure:
   * @midi_builder: the builder
   * @channel: the audio channel
   * @pressure: the new pressure, aftertouch
   *
   * The ::change-channel-pressure signal is emited during building of event.
   *
   * Since: 0.7.0
   */
  midi_builder_signals[CHANGE_PRESSURE] =
    g_signal_new("change-channel-pressure\0",
		 G_TYPE_FROM_CLASS(midi_builder),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsMidiBuilderClass, append_change_pressure),
		 NULL, NULL,
		 g_cclosure_user_marshal_VOID__UINT_UINT,
		 G_TYPE_NONE, 2,
		 G_TYPE_UINT,
		 G_TYPE_UINT);

  /**
   * AgsMidiBuilder::sysex:
   * @midi_builder: the builder
   * @sysex_data: the data array
   * @length: the length of the array
   *
   * The ::sysex signal is emited during building of event.
   *
   * Since: 0.7.0
   */
  midi_builder_signals[SYSEX] =
    g_signal_new("sysex\0",
		 G_TYPE_FROM_CLASS(midi_builder),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsMidiBuilderClass, append_sysex),
		 NULL, NULL,
		 g_cclosure_user_marshal_VOID__POINTER_UINT,
		 G_TYPE_NONE, 2,
		 G_TYPE_POINTER,
		 G_TYPE_UINT);

  /**
   * AgsMidiBuilder::quarter-frame:
   * @midi_builder: the builder
   * @delta_time: the delta time
   * @message_type: the message type
   * @values: the values
   *
   * The ::quarter-frame signal is emited during building of event.
   *
   * Since: 0.7.0
   */
  midi_builder_signals[QUARTER_FRAME] =
    g_signal_new("quarter-frame\0",
		 G_TYPE_FROM_CLASS(midi_builder),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsMidiBuilderClass, append_quarter_frame),
		 NULL, NULL,
		 g_cclosure_user_marshal_VOID__UINT_UINT_UINT,
		 G_TYPE_NONE, 3,
		 G_TYPE_UINT,
		 G_TYPE_UINT,
		 G_TYPE_UINT);

    /**
   * AgsMidiBuilder::song-position:
   * @midi_builder: the builder
   * @song_position: the current position from 0x0 to 0x3fff
   *
   * The ::song-position signal is emited during building of event.
   *
   * Since: 0.7.0
   */
  midi_builder_signals[SONG_POSITION] =
    g_signal_new("song-position\0",
		 G_TYPE_FROM_CLASS(midi_builder),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsMidiBuilderClass, append_song_position),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__UINT,
		 G_TYPE_NONE, 1,
		 G_TYPE_UINT);

    /**
   * AgsMidiBuilder::song-select:
   * @midi_builder: the builder
   * @song_select: the song or sequence
   *
   * The ::song-select signal is emited during building of event.
   *
   * Since: 0.7.0
   */
  midi_builder_signals[SONG_SELECT] =
    g_signal_new("song-select\0",
		 G_TYPE_FROM_CLASS(midi_builder),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsMidiBuilderClass, append_song_select),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__UINT,
		 G_TYPE_NONE, 1,
		 G_TYPE_UINT);

    /**
   * AgsMidiBuilder::tune-request:
   * @midi_builder: the builder
   *
   * The ::tune-request signal is emited during building of event.
   *
   * Since: 0.7.0
   */
  midi_builder_signals[TUNE_REQUEST] =
    g_signal_new("tune-request\0",
		 G_TYPE_FROM_CLASS(midi_builder),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsMidiBuilderClass, append_tune_request),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);


  /**
   * AgsMidiBuilder::sequence-number:
   * @midi_builder: the builder
   * @sequence: the nth sequence
   *
   * The ::sequence-number signal is emited during building of event.
   *
   * Since: 0.7.0
   */
  midi_builder_signals[SEQUENCE_NUMBER] =
    g_signal_new("sequence-number\0",
		 G_TYPE_FROM_CLASS(midi_builder),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsMidiBuilderClass, append_sequence_number),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__UINT,
		 G_TYPE_NONE, 1,
		 G_TYPE_UINT);
  
  /**
   * AgsMidiBuilder::smtpe:
   * @midi_builder: the builder
   * @hr: hours
   * @mn: minutes
   * @se: seconds
   * @fr: frame number
   * @ff: frequency
   *
   * The ::smtpe signal is emited during building of event.
   *
   * Since: 0.7.0
   */
  midi_builder_signals[SMTPE] =
    g_signal_new("smtpe\0",
		 G_TYPE_FROM_CLASS(midi_builder),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsMidiBuilderClass, append_smtpe),
		 NULL, NULL,
		 g_cclosure_user_marshal_VOID__UINT_UINT_UINT_UINT_UINT_UINT,
		 G_TYPE_NONE, 6,
		 G_TYPE_UINT,
		 G_TYPE_UINT,
		 G_TYPE_UINT,
		 G_TYPE_UINT,
		 G_TYPE_UINT,
		 G_TYPE_UINT);

  /**
   * AgsMidiBuilder::tempo:
   * @midi_builder: the builder
   * @tempo: the tempo number as 24-bit quantifier
   *
   * The ::tempo signal is emited during building of event.
   *
   * Since: 0.7.0
   */
  midi_builder_signals[TEMPO] =
    g_signal_new("tempo\0",
		 G_TYPE_FROM_CLASS(midi_builder),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsMidiBuilderClass, append_tempo),
		 NULL, NULL,
		 g_cclosure_user_marshal_VOID__UINT_UINT,
		 G_TYPE_NONE, 2,
		 G_TYPE_UINT,
		 G_TYPE_UINT);

  /**
   * AgsMidiBuilder::time-signature:
   * @midi_builder: the builder
   * @delta_time: delta time
   * @nn: numerator
   * @dd: denominator
   * @cc: ticks per metronome click
   * @bb: 32nd per quarter note
   *
   * The ::time-signature signal is emited during building of event.
   *
   * Since: 0.7.0
   */
  midi_builder_signals[TIME_SIGNATURE] =
    g_signal_new("time-signature\0",
		 G_TYPE_FROM_CLASS(midi_builder),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsMidiBuilderClass, append_time_signature),
		 NULL, NULL,
		 g_cclosure_user_marshal_VOID__UINT_UINT_UINT_UINT_UINT,
		 G_TYPE_NONE, 5,
		 G_TYPE_UINT,
		 G_TYPE_UINT,
		 G_TYPE_UINT,
		 G_TYPE_UINT,
		 G_TYPE_UINT);

  /**
   * AgsMidiBuilder::key-signature:
   * @midi_builder: the builder
   * @delta_time: delta-time
   * @sf: signature frame
   * @mi: if %TRUE minor else major
   * 
   * The ::key-signature signal is emited during building of event.
   *
   * Since: 0.7.0
   */
  midi_builder_signals[KEY_SIGNATURE] =
    g_signal_new("key-signature\0",
		 G_TYPE_FROM_CLASS(midi_builder),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsMidiBuilderClass, append_key_signature),
		 NULL, NULL,
		 g_cclosure_user_marshal_VOID__UINT_UINT_BOOLEAN,
		 G_TYPE_NONE, 3,
		 G_TYPE_UINT,
		 G_TYPE_UINT,
		 G_TYPE_BOOLEAN);

  /**
   * AgsMidiBuilder::sequencer-meta-event:
   * @midi_builder: the builder
   * @len: length
   * @id: identifier
   * @data: buffer
   *
   * The ::sequencer-meta-event signal is emited during building of event.
   *
   * Since: 0.7.0
   */
  midi_builder_signals[SEQUENCER_META_EVENT] =
    g_signal_new("sequencer-meta-event\0",
		 G_TYPE_FROM_CLASS(midi_builder),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsMidiBuilderClass, append_sequencer_meta_event),
		 NULL, NULL,
		 g_cclosure_user_marshal_VOID__UINT_UINT_UINT,
		 G_TYPE_NONE, 3,
		 G_TYPE_UINT,
		 G_TYPE_UINT,
		 G_TYPE_UINT);

  /**
   * AgsMidiBuilder::text-event:
   * @midi_builder: the builder
   * @text: the text
   * @length: length
   *
   * The ::text-event signal is emited during building of event.
   *
   * Since: 0.7.0
   */
  midi_builder_signals[TEXT_EVENT] =
    g_signal_new("text-event\0",
		 G_TYPE_FROM_CLASS(midi_builder),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsMidiBuilderClass, append_text_event),
		 NULL, NULL,
		 g_cclosure_user_marshal_VOID__UINT_UINT,
		 G_TYPE_NONE, 2,
		 G_TYPE_UINT,
		 G_TYPE_UINT);
}

void
ags_midi_builder_init(AgsMidiBuilder *midi_builder)
{
  midi_builder->flags = 0;

  midi_builder->data = NULL;
  midi_builder->length = 0;

  midi_builder->file = NULL;
  
  midi_builder->midi_header = NULL;
  
  midi_builder->midi_track = NULL;
  midi_builder->current_midi_track = NULL;
}

void
ags_midi_builder_set_property(GObject *gobject,
			      guint prop_id,
			      const GValue *value,
			      GParamSpec *param_spec)
{
  AgsMidiBuilder *midi_builder;

  midi_builder = AGS_MIDI_BUILDER(gobject);
  
  switch(prop_id){
  case PROP_FILE:
    {
      midi_builder->file = g_value_get_pointer(value);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_midi_builder_get_property(GObject *gobject,
			      guint prop_id,
			      GValue *value,
			      GParamSpec *param_spec)
{
  AgsMidiBuilder *midi_builder;

  midi_builder = AGS_MIDI_BUILDER(gobject);
  
  switch(prop_id){
  case PROP_FILE:
    {
      g_value_set_pointer(value,
			  midi_builder->file);
    }
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_midi_builder_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_midi_builder_parent_class)->finalize(gobject);
}

/**
 * ags_midi_builder_header_alloc:
 * 
 * Allocate MIDI builder header.
 * 
 * Returns: the newly allocated #AgsMidiBuilderHeader-struct
 * 
 * Since: 0.7.90
 */
AgsMidiBuilderHeader*
ags_midi_builder_header_alloc()
{
  AgsMidiBuilderHeader *midi_builder_header;

  midi_builder_header = (AgsMidiBuilderHeader *) malloc(sizeof(AgsMidiBuilderHeader));

  midi_builder_header->offset = 0;
  midi_builder_header->format = 1;
  midi_builder_header->count = 0;
  midi_builder_header->division = 0;
  midi_builder_header->times = 0;
  midi_builder_header->beat = 0;
  midi_builder_header->clicks = 0;

  midi_builder_header->data = NULL;
  midi_builder_header->length = 0;
  
  return(midi_builder_header);
}

/**
 * ags_midi_builder_header_free:
 * @midi_builder_track: the #AgsMidiBuilderHeader-struct
 * 
 * Free MIDI builder header.
 * 
 * Since: 0.7.90
 */
void
ags_midi_builder_header_free(AgsMidiBuilderHeader *midi_builder_header)
{
  if(midi_builder_header == NULL){
    return;
  }

  free(midi_builder_header);
}

/**
 * ags_midi_builder_track_alloc:
 * 
 * Allocate MIDI builder track.
 * 
 * Returns: the newly allocated #AgsMidiBuilderTrack-struct
 * 
 * Since: 0.7.90
 */
AgsMidiBuilderTrack*
ags_midi_builder_track_alloc()
{
  AgsMidiBuilderTrack *midi_builder_track;

  midi_builder_track = (AgsMidiBuilderTrack *) malloc(sizeof(AgsMidiBuilderTrack));
  
  midi_builder_track->offset = 0;
  midi_builder_track->track_name = NULL;
  
  midi_builder_track->absolute_time = 0;
  
  midi_builder_track->data = NULL;
  midi_builder_track->length = 0;
  
  return(midi_builder_track);
}

/**
 * ags_midi_builder_track_free:
 * @midi_builder_track: the #AgsMidiBuilderTrack-struct
 * 
 * Free MIDI builder track.
 * 
 * Since: 0.7.90
 */
void
ags_midi_builder_track_free(AgsMidiBuilderTrack *midi_builder_track)
{
  if(midi_builder_track == NULL){
    return;
  }

  if(midi_builder_track->track_name != NULL){
    free(midi_builder_track->track_name);
  }

  if(midi_builder_track->data != NULL){
    free(midi_builder_track->data);
  }
  
  free(midi_builder_track);
}

/**
 * ags_midi_builder_track_find_track_name_with_delta_time:
 * @midi_builder_track: the #AgsMidiBuilderTrack
 * @absolute_time: the absolute time
 * @track_name: the track's string representation
 * 
 * Finds matching #AgsMidiBuilderTrack-struct within @midi_builder_track #GList-struct.
 * 
 * Returns: the matching #GList-struct
 * 
 * Since: 0.7.90
 */
GList*
ags_midi_builder_track_find_delta_time_with_track_name(GList *midi_builder_track,
						       guint64 absolute_time,
						       gchar *track_name)
{
  while(midi_builder_track != NULL){
    if(AGS_MIDI_BUILDER_TRACK(midi_builder_track->data)->absolute_time == absolute_time &&
       !g_ascii_strcasecmp(AGS_MIDI_BUILDER_TRACK(midi_builder_track->data)->track_name,
			   track_name)){
      break;
    }
    
    midi_builder_track = midi_builder_track->next;
  }

  return(midi_builder_track);
}

/**
 * ags_midi_builder_track_insert_midi_message:
 * @midi_builder_track: the #AgsMidiBuilderTrack-struct
 * @buffer: the MIDI message
 * 
 * Insert MIDI message.
 * 
 * Since: 0.7.90
 */
void
ags_midi_builder_track_insert_midi_message(AgsMidiBuilderTrack *midi_builder_track,
					   unsigned char *buffer, guint length)
{
  unsigned char *current;
  
  gint prefix_length, suffix_length;
  guint64 absolute_time;
  glong delta_time;
  
  if(midi_builder_track == NULL ||
     buffer == NULL ||
     length == 0){
    return;
  }

  /* read delta-time */
  ags_midi_buffer_util_get_varlength(buffer,
				     &delta_time);

  midi_builder_track->absolute_time += delta_time;
  
  /* get offset */
  midi_builder_track->data = (unsigned char *) realloc(midi_builder_track->data,
						       (midi_builder_track->length + length) * sizeof(unsigned char));

  current = midi_builder_track->data + midi_builder_track->length;
  
  if(current == NULL){
    prefix_length = 0;
    suffix_length = 0;
  }else{
    /* prefix */    
    if(current <= midi_builder_track->data + midi_builder_track->length){
      prefix_length = current - midi_builder_track->data;
    }else{
      prefix_length = 0;
    }

    /* suffix */
    if(midi_builder_track->length > prefix_length){
      suffix_length = midi_builder_track->length - prefix_length;

      memmove(midi_builder_track->data + prefix_length + length, midi_builder_track->data + prefix_length, suffix_length);
    }
  }

  memcpy(midi_builder_track->data + prefix_length, buffer, length);

  /* set data */
  midi_builder_track->length += length;
  ags_midi_buffer_util_put_int32(midi_builder_track->data + 4,
				 midi_builder_track->length);
}

/**
 * ags_midi_builder_track_get_delta_time_offset:
 * @midi_builder_track: the #AgsMidiBuilderTrack-struct
 * @absolute_time: the absolute time
 * 
 * Get offset by delta time.
 * 
 * Returns: the buffer position before @delta_time
 * 
 * Since: 0.7.90
 */
unsigned char*
ags_midi_builder_track_get_delta_time_offset(AgsMidiBuilderTrack *midi_builder_track,
					     guint64 absolute_time)
{
  unsigned char *prev, *current;

  guint64 time_counter;
  glong current_delta_time;
  
  if(midi_builder_track == NULL ||
     midi_builder_track->data == NULL){
    return(NULL);
  }

  current = midi_builder_track->data;
  
  prev = NULL;
  time_counter = 0;
  
  while(current != NULL){
    current = ags_midi_buffer_util_seek_message(current,
						1,
						&current_delta_time);
    time_counter += current_delta_time;
    
    if(time_counter > absolute_time){
      break;
    }
   
    if(current >= midi_builder_track->data + midi_builder_track->length){
      break;
    }

    prev = current;
  }

  if(current == NULL){
    current = midi_builder_track->data + midi_builder_track->length;
  }
  
  return(current);
}


void
ags_midi_builder_real_midi_putc(AgsMidiBuilder *midi_builder,
				gint c)
{
  if(midi_builder->file == NULL){
    return;
  }
  
  fputc(c,
	midi_builder->file);
}

/**
 * ags_midi_builder_midi_putc:
 * @midi_builder: the #AgsMidiBuilder
 *
 * Put char in MIDI file.
 *
 * Since: 0.7.3
 */
void
ags_midi_builder_midi_putc(AgsMidiBuilder *midi_builder,
			   gint c)
{
  g_return_if_fail(AGS_IS_MIDI_BUILDER(midi_builder));
  
  g_object_ref((GObject *) midi_builder);
  g_signal_emit(G_OBJECT(midi_builder),
		midi_builder_signals[MIDI_PUTC], 0,
		c);
  g_object_unref((GObject *) midi_builder);
}

void
ags_midi_builder_real_on_error(AgsMidiBuilder *midi_builder,
			       GError **error)
{
  if(error != NULL &&
     *error != NULL){
    g_warning("%s\0", (*error)->message);
  }
}

/**
 * ags_midi_builder_on_error:
 * @midi_builder: the #AgsMidiBuilder
 * @error: the error
 *
 * Triggered as an error occurs.
 *
 * Since: 0.7.3
 */
void
ags_midi_builder_on_error(AgsMidiBuilder *midi_builder,
			  GError **error)
{
  g_return_if_fail(AGS_IS_MIDI_BUILDER(midi_builder));
  
  g_object_ref((GObject *) midi_builder);
  g_signal_emit(G_OBJECT(midi_builder),
		midi_builder_signals[ON_ERROR], 0,
		error);
  g_object_unref((GObject *) midi_builder);
}

void
ags_midi_builder_real_append_header(AgsMidiBuilder *midi_builder,
				    guint offset, guint format,
				    guint track_count, guint division,
				    guint times, guint bpm,
				    guint clicks)
{
  AgsMidiBuilderHeader *midi_builder_header;
  
  if(midi_builder->midi_header == NULL){
    midi_builder_header = 
      midi_builder->midi_header = ags_midi_builder_header_alloc();    
  }else{
    midi_builder_header = midi_builder->midi_header;
  }

  if(midi_builder_header->data == NULL){
    midi_builder_header->data = (unsigned char *) malloc(14 * sizeof(unsigned char));
    midi_builder_header->length = 14;
  }
  
  midi_builder_header->offset = offset;
  midi_builder_header->format = format;
  midi_builder_header->count = track_count;
  midi_builder_header->division = division;
  midi_builder_header->times = times;
  midi_builder_header->beat = bpm;
  midi_builder_header->clicks = clicks;

  ags_midi_buffer_util_put_header(midi_builder_header->data,
				  offset, format,
				  track_count, division);
}

/**
 * ags_midi_builder_append_header:
 * @midi_builder: the #AgsMidiBuilder
 * @offset: start delta-time
 * @format: either 0, 1 or 2.
 * @track_count: the number of tracks
 * @division: timing division
 * @times: pulse per quarter note
 * @bpm: beats per minute
 * @clicks: timing clicks
 *
 * Appends MIDI header to @midi_builder.
 *
 * Since: 0.7.3
 */
void
ags_midi_builder_append_header(AgsMidiBuilder *midi_builder,
			       guint offset, guint format,
			       guint track_count, guint division,
			       guint times, guint bpm,
			       guint clicks)
{
  g_return_if_fail(AGS_IS_MIDI_BUILDER(midi_builder));
  
  g_object_ref((GObject *) midi_builder);
  g_signal_emit(G_OBJECT(midi_builder),
		midi_builder_signals[APPEND_HEADER], 0,
		offset, format,
		track_count, division,
		times, bpm,
		clicks);
  g_object_unref((GObject *) midi_builder);
}

void
ags_midi_builder_real_append_track(AgsMidiBuilder *midi_builder,
				   gchar *track_name)
{
  AgsMidiBuilderTrack *midi_builder_track;

  guint delta_time_size;
  guint length;

  /* allocate struct */
  midi_builder_track = ags_midi_builder_track_alloc();
  midi_builder_track->track_name = g_strdup(track_name);
  
  /* fill buffer */
  delta_time_size = ags_midi_buffer_util_get_varlength_size(0);

  if(track_name != NULL){
    length = strlen(track_name);
  }else{
    length = 0;
  }
  
  midi_builder_track->data = (unsigned char *) malloc((8 + delta_time_size + length + 3) * sizeof(unsigned char));
  midi_builder_track->length = (4 + 4) + delta_time_size + length + 3;

  ags_midi_buffer_util_put_track(midi_builder_track->data,
				 0);

  if(track_name != NULL){
    ags_midi_buffer_util_put_text_event(midi_builder_track->data + 8,
					0,
					track_name, length);
  }
  
  /* append track */
  midi_builder->current_midi_track = midi_builder_track;
  midi_builder->midi_track = g_list_append(midi_builder->midi_track,
					   midi_builder_track);
}

/**
 * ags_midi_builder_append_track:
 * @midi_builder: the #AgsMidiBuilder
 * @track_name: the track name
 *
 * Append a track called @track_name to @midi_builder.
 *
 * Since: 0.7.3
 */
void
ags_midi_builder_append_track(AgsMidiBuilder *midi_builder,
			      gchar *track_name)
{
  g_return_if_fail(AGS_IS_MIDI_BUILDER(midi_builder));
  
  g_object_ref((GObject *) midi_builder);
  g_signal_emit(G_OBJECT(midi_builder),
		midi_builder_signals[APPEND_TRACK], 0,
		track_name);
  g_object_unref((GObject *) midi_builder);
}

void
ags_midi_builder_real_append_key_on(AgsMidiBuilder *midi_builder,
				    guint delta_time,
				    guint audio_channel,
				    guint note,
				    guint velocity)
{
  unsigned char *buffer;

  guint delta_time_size;

  delta_time_size = ags_midi_buffer_util_get_varlength_size(delta_time);

  buffer = (unsigned char *) malloc((delta_time_size + 3) * sizeof(unsigned char));
  ags_midi_buffer_util_put_key_on(buffer,
				  delta_time,
				  audio_channel,
				  note,
				  velocity);
  ags_midi_builder_track_insert_midi_message(midi_builder->current_midi_track,
					     buffer, delta_time_size + 3);
}

/**
 * ags_midi_builder_append_key_on:
 * @midi_builder: the #AgsMidiBuilder
 * @delta_time: delta-time
 * @audio_channel: the audio channel
 * @note: the note to play from 0 to 128
 * @velocity: key dynamics
 *
 * Append key-on for @note to @midi_builder with key dynamics @velocity, at @delta_time.
 *
 * Since: 0.7.3
 */
void
ags_midi_builder_append_key_on(AgsMidiBuilder *midi_builder,
			       guint delta_time,
			       guint audio_channel,
			       guint note,
			       guint velocity)
{
  g_return_if_fail(AGS_IS_MIDI_BUILDER(midi_builder));
  
  g_object_ref((GObject *) midi_builder);
  g_signal_emit(G_OBJECT(midi_builder),
		midi_builder_signals[KEY_ON], 0,
		delta_time,
		audio_channel,
		note,
		velocity);
  g_object_unref((GObject *) midi_builder);
}

void
ags_midi_builder_real_append_key_off(AgsMidiBuilder *midi_builder,
				     guint delta_time,
				     guint audio_channel,
				     guint note,
				     guint velocity)
{
  unsigned char *buffer;

  guint delta_time_size;

  delta_time_size = ags_midi_buffer_util_get_varlength_size(delta_time);
  
  buffer = (unsigned char *) malloc((delta_time_size + 3) * sizeof(unsigned char));
  ags_midi_buffer_util_put_key_off(buffer,
				   delta_time,
				   audio_channel,
				   note,
				   velocity);
  ags_midi_builder_track_insert_midi_message(midi_builder->current_midi_track,
					     buffer, delta_time_size + 3);
}

/**
 * ags_midi_builder_append_key_off:
 * @midi_builder: the #AgsMidiBuilder
 * @delta_time: delta-time
 * @audio_channel: the audio channel
 * @note: the note to play from 0 to 128
 * @velocity: key dynamics
 *
 * Append key-off for @note to @midi_builder with key dynamics @velocity, at @delta_time.
 *
 * Since: 0.7.3
 */
void
ags_midi_builder_append_key_off(AgsMidiBuilder *midi_builder,
				guint delta_time,
				guint audio_channel,
				guint note,
				guint velocity)
{
  g_return_if_fail(AGS_IS_MIDI_BUILDER(midi_builder));
  
  g_object_ref((GObject *) midi_builder);
  g_signal_emit(G_OBJECT(midi_builder),
		midi_builder_signals[KEY_OFF], 0,
		delta_time,
		audio_channel,
		note,
		velocity);
  g_object_unref((GObject *) midi_builder);
}

void
ags_midi_builder_real_append_key_pressure(AgsMidiBuilder *midi_builder,
					  guint delta_time,
					  guint audio_channel,
					  guint note,
					  guint pressure)
{
  unsigned char *buffer;

  guint delta_time_size;

  delta_time_size = ags_midi_buffer_util_get_varlength_size(delta_time);
  
  buffer = (unsigned char *) malloc((delta_time_size + 3) * sizeof(unsigned char));
  ags_midi_buffer_util_put_key_pressure(buffer,
					delta_time,
					audio_channel,
					note,
					pressure);
  ags_midi_builder_track_insert_midi_message(midi_builder->current_midi_track,
					     buffer, delta_time_size + 3);
}


/**
 * ags_midi_builder_append_key_pressure:
 * @midi_builder: the #AgsMidiBuilder
 * @delta_time: delta-time
 * @audio_channel: the audio channel
 * @note: the note to play from 0 to 128
 * @pressure: key dynamics
 *
 * Append key-pressure for @note to @midi_builder with key dynamics @pressure, at @delta_time.
 *
 * Since: 0.7.3
 */
void
ags_midi_builder_append_key_pressure(AgsMidiBuilder *midi_builder,
				     guint delta_time,
				     guint audio_channel,
				     guint note,
				     guint pressure)
{
  g_return_if_fail(AGS_IS_MIDI_BUILDER(midi_builder));
  
  g_object_ref((GObject *) midi_builder);
  g_signal_emit(G_OBJECT(midi_builder),
		midi_builder_signals[KEY_PRESSURE], 0,
		delta_time,
		audio_channel,
		note,
		pressure);
  g_object_unref((GObject *) midi_builder);
}

void
ags_midi_builder_real_append_change_parameter(AgsMidiBuilder *midi_builder,
					      guint delta_time,
					      guint channel,
					      guint control,
					      guint value)
{
  unsigned char *buffer;

  guint delta_time_size;

  delta_time_size = ags_midi_buffer_util_get_varlength_size(delta_time);
  
  buffer = (unsigned char *) malloc((delta_time_size + 3) * sizeof(unsigned char));
  ags_midi_buffer_util_put_change_parameter(buffer,
					    delta_time,
					    channel,
					    control,
					    value);
  ags_midi_builder_track_insert_midi_message(midi_builder->current_midi_track,
					     buffer, delta_time_size + 3);
}

/**
 * ags_midi_builder_append_change_parameter:
 * @midi_builder: the #AgsMidiBuilder
 * @delta_time: the delta time
 * @channel: the channel
 * @control: the control
 * @value: the value
 *
 * Appends change parameter.
 *
 * Since: 0.7.3
 */
void
ags_midi_builder_append_change_parameter(AgsMidiBuilder *midi_builder,
					 guint delta_time,
					 guint channel,
					 guint control,
					 guint value)
{
  g_return_if_fail(AGS_IS_MIDI_BUILDER(midi_builder));
  
  g_object_ref((GObject *) midi_builder);
  g_signal_emit(G_OBJECT(midi_builder),
		midi_builder_signals[CHANGE_PARAMETER], 0,
		delta_time,
		channel,
		control,
		value);
  g_object_unref((GObject *) midi_builder);
}

void
ags_midi_builder_real_append_change_pitch_bend(AgsMidiBuilder *midi_builder,
					       guint delta_time,
					       guint channel,
					       guint pitch,
					       guint transmitter)
{
  unsigned char *buffer;

  guint delta_time_size;

  delta_time_size = ags_midi_buffer_util_get_varlength_size(delta_time);
  
  buffer = (unsigned char *) malloc((delta_time_size + 3) * sizeof(unsigned char));
  ags_midi_buffer_util_put_pitch_bend(buffer,
				      delta_time,
				      channel,
				      pitch,
				      transmitter);
  ags_midi_builder_track_insert_midi_message(midi_builder->current_midi_track,
					     buffer, delta_time_size + 3);
}

/**
 * ags_midi_builder_append_change_pitch_bend:
 * @midi_builder: the #AgsMidiBuilder
 * @delta_time: the delta time
 * @channel: the channel
 * @pitch: the pitch
 * @transmitter: the transmitter
 *
 * Change pitch bend.
 * 
 * Since: 0.7.3
 */
void
ags_midi_builder_append_change_pitch_bend(AgsMidiBuilder *midi_builder,
					  guint delta_time,
					  guint channel,
					  guint pitch,
					  guint transmitter)
{
  g_return_if_fail(AGS_IS_MIDI_BUILDER(midi_builder));
  
  g_object_ref((GObject *) midi_builder);
  g_signal_emit(G_OBJECT(midi_builder),
		midi_builder_signals[CHANGE_PITCH_BEND], 0,
		delta_time,
		channel,
		pitch,
		transmitter);
  g_object_unref((GObject *) midi_builder);
}

void
ags_midi_builder_real_append_change_program(AgsMidiBuilder *midi_builder,
					    guint delta_time,
					    guint channel,
					    guint program)
{
  unsigned char *buffer;

  guint delta_time_size;

  delta_time_size = ags_midi_buffer_util_get_varlength_size(delta_time);
  
  buffer = (unsigned char *) malloc((delta_time_size + 2) * sizeof(unsigned char));
  ags_midi_buffer_util_put_change_program(buffer,
					  delta_time,
					  channel,
					  program);
  ags_midi_builder_track_insert_midi_message(midi_builder->current_midi_track,
					     buffer, delta_time_size + 2);
}

/**
 * ags_midi_builder_append_change_program:
 * @midi_builder: the #AgsMidiBuilder
 * @delta_time: the delta time
 * @channel: the channel
 * @program: the program
 *
 * Appends change program.
 *
 * Since: 0.7.3
 */
void
ags_midi_builder_append_change_program(AgsMidiBuilder *midi_builder,
				       guint delta_time,
				       guint channel,
				       guint program)
{
  g_return_if_fail(AGS_IS_MIDI_BUILDER(midi_builder));
  
  g_object_ref((GObject *) midi_builder);
  g_signal_emit(G_OBJECT(midi_builder),
		midi_builder_signals[CHANGE_PROGRAM], 0,
		delta_time,
		channel,
		program);
  g_object_unref((GObject *) midi_builder);
}

void
ags_midi_builder_real_append_change_pressure(AgsMidiBuilder *midi_builder,
					     guint delta_time,
					     guint channel,
					     guint pressure)
{
  unsigned char *buffer;

  guint delta_time_size;

  delta_time_size = ags_midi_buffer_util_get_varlength_size(delta_time);
  
  buffer = (unsigned char *) malloc((delta_time_size + 2) * sizeof(unsigned char));
  ags_midi_buffer_util_put_change_pressure(buffer,
					  delta_time,
					  channel,
					  pressure);
  ags_midi_builder_track_insert_midi_message(midi_builder->current_midi_track,
					     buffer, delta_time_size + 2);
}

/**
 * ags_midi_builder_append_change_pressure:
 * @midi_builder: the #AgsMidiBuilder
 * @delta_time: the delta time
 * @channel: the channel
 * @pressure: the pressure
 * 
 * Appends change pressure.
 *
 * Since: 0.7.3
 */
void
ags_midi_builder_append_change_pressure(AgsMidiBuilder *midi_builder,
					guint delta_time,
					guint channel,
					guint pressure)
{
  g_return_if_fail(AGS_IS_MIDI_BUILDER(midi_builder));
  
  g_object_ref((GObject *) midi_builder);
  g_signal_emit(G_OBJECT(midi_builder),
		midi_builder_signals[CHANGE_PRESSURE], 0,
		delta_time,
		channel,
		pressure);
  g_object_unref((GObject *) midi_builder);
}

void
ags_midi_builder_real_append_sysex(AgsMidiBuilder *midi_builder,
				   guint delta_time,
				   unsigned char *sysex_data, guint length)
{
  unsigned char *buffer;

  guint delta_time_size;

  delta_time_size = ags_midi_buffer_util_get_varlength_size(delta_time);
  
  buffer = (unsigned char *) malloc((delta_time_size + length) * sizeof(unsigned char));
  ags_midi_buffer_util_put_sysex(buffer,
				 delta_time,
				 sysex_data, length);
  ags_midi_builder_track_insert_midi_message(midi_builder->current_midi_track,
					     buffer, delta_time_size + length);
}

/**
 * ags_midi_builder_append_sysex:
 * @midi_builder: the #AgsMidiBuilder
 * @delta_time: the delta time
 * @sysex_data: the sysex data
 * @length: length
 *
 * Appends sysex data.
 *
 * Since: 0.7.3
 */
void
ags_midi_builder_append_sysex(AgsMidiBuilder *midi_builder,
			      guint delta_time,
			      unsigned char *sysex_data, guint length)
{
  g_return_if_fail(AGS_IS_MIDI_BUILDER(midi_builder));
  
  g_object_ref((GObject *) midi_builder);
  g_signal_emit(G_OBJECT(midi_builder),
		midi_builder_signals[SYSEX], 0,
		delta_time,
		sysex_data, length);
  g_object_unref((GObject *) midi_builder);
}

void
ags_midi_builder_real_append_quarter_frame(AgsMidiBuilder *midi_builder,
					   guint delta_time,
					   guint message_type,
					   guint values)
{
  unsigned char *buffer;

  guint delta_time_size;

  delta_time_size = ags_midi_buffer_util_get_varlength_size(delta_time);
  
  buffer = (unsigned char *) malloc((delta_time_size + 2) * sizeof(unsigned char));
  ags_midi_buffer_util_put_quarter_frame(buffer,
					 delta_time,
					 message_type,
					 values);
  ags_midi_builder_track_insert_midi_message(midi_builder->current_midi_track,
					     buffer, delta_time_size + 2);
}

/**
 * ags_midi_builder_append_quarter_frame:
 * @midi_builder: the #AgsMidiBuilder
 * @delta_time: the delta time
 * @message_type: the message type
 * @values: values
 *
 * Appends quarter frame.
 * 
 * Since: 0.7.3
 */
void
ags_midi_builder_append_quarter_frame(AgsMidiBuilder *midi_builder,
				      guint delta_time,
				      guint message_type,
				      guint values)
{
  g_return_if_fail(AGS_IS_MIDI_BUILDER(midi_builder));
  
  g_object_ref((GObject *) midi_builder);
  g_signal_emit(G_OBJECT(midi_builder),
		midi_builder_signals[QUARTER_FRAME], 0,
		delta_time,
		message_type, values);
  g_object_unref((GObject *) midi_builder);
}

void
ags_midi_builder_real_append_song_position(AgsMidiBuilder *midi_builder,
					   guint delta_time,
					   guint song_position)
{
  unsigned char *buffer;

  guint delta_time_size;

  delta_time_size = ags_midi_buffer_util_get_varlength_size(delta_time);
  
  buffer = (unsigned char *) malloc((delta_time_size + 3) * sizeof(unsigned char));
  ags_midi_buffer_util_put_song_position(buffer,
					 delta_time,
					 song_position);
  ags_midi_builder_track_insert_midi_message(midi_builder->current_midi_track,
					     buffer, delta_time_size + 3);
}

/**
 * ags_midi_builder_append_song_position:
 * @midi_builder: the #AgsMidiBuilder
 * @delta_time: the delta time
 * @song_position: the song position
 * 
 * Appends song position.
 *
 * Since: 0.7.3
 */
void
ags_midi_builder_append_song_position(AgsMidiBuilder *midi_builder,
				      guint delta_time,
				      guint song_position)
{
  g_return_if_fail(AGS_IS_MIDI_BUILDER(midi_builder));
  
  g_object_ref((GObject *) midi_builder);
  g_signal_emit(G_OBJECT(midi_builder),
		midi_builder_signals[SONG_POSITION], 0,
		delta_time,
		song_position);
  g_object_unref((GObject *) midi_builder);
}

void
ags_midi_builder_real_append_song_select(AgsMidiBuilder *midi_builder,
					 guint delta_time,
					 guint song_select)
{
  unsigned char *buffer;

  guint delta_time_size;

  delta_time_size = ags_midi_buffer_util_get_varlength_size(delta_time);
  
  buffer = (unsigned char *) malloc((delta_time_size + 2) * sizeof(unsigned char));
  ags_midi_buffer_util_put_song_select(buffer,
				       delta_time,
				       song_select);
  ags_midi_builder_track_insert_midi_message(midi_builder->current_midi_track,
					     buffer, delta_time_size + 2);
}

/**
 * ags_midi_builder_append_song_select:
 * @midi_builder: the #AgsMidiBuilder
 * @delta_time: the delta time
 * @song_select: the song select
 * 
 * Appends song select.
 *
 * Since: 0.7.3
 */
void
ags_midi_builder_append_song_select(AgsMidiBuilder *midi_builder,
				    guint delta_time,
				    guint song_select)
{
  g_return_if_fail(AGS_IS_MIDI_BUILDER(midi_builder));
  
  g_object_ref((GObject *) midi_builder);
  g_signal_emit(G_OBJECT(midi_builder),
		midi_builder_signals[SONG_SELECT], 0,
		delta_time,
		song_select);
  g_object_unref((GObject *) midi_builder);
}

void
ags_midi_builder_real_append_tune_request(AgsMidiBuilder *midi_builder,
					  guint delta_time)
{
  unsigned char *buffer;

  guint delta_time_size;

  delta_time_size = ags_midi_buffer_util_get_varlength_size(delta_time);
  
  buffer = (unsigned char *) malloc((delta_time_size + 1) * sizeof(unsigned char));
  ags_midi_buffer_util_put_tune_request(buffer,
					delta_time);
  ags_midi_builder_track_insert_midi_message(midi_builder->current_midi_track,
					     buffer, delta_time_size + 1);
}

/**
 * ags_midi_builder_append_tune_request:
 * @midi_builder: the #AgsMidiBuilder
 * @delta_time: the delta time
 * 
 * Appends tune request.
 *
 * Since: 0.7.3
 */
void
ags_midi_builder_append_tune_request(AgsMidiBuilder *midi_builder,
				     guint delta_time)
{
  g_return_if_fail(AGS_IS_MIDI_BUILDER(midi_builder));
  
  g_object_ref((GObject *) midi_builder);
  g_signal_emit(G_OBJECT(midi_builder),
		midi_builder_signals[TUNE_REQUEST], 0,
		delta_time);
  g_object_unref((GObject *) midi_builder);
}

void
ags_midi_builder_real_append_sequence_number(AgsMidiBuilder *midi_builder,
					     guint delta_time,
					     guint sequence)
{
  unsigned char *buffer;

  guint delta_time_size;

  delta_time_size = ags_midi_buffer_util_get_varlength_size(delta_time);
  
  buffer = (unsigned char *) malloc((delta_time_size + 5) * sizeof(unsigned char));
  ags_midi_buffer_util_put_sequence_number(buffer,
					   delta_time,
					   sequence);
  ags_midi_builder_track_insert_midi_message(midi_builder->current_midi_track,
					     buffer, delta_time_size + 5);
}

/**
 * ags_midi_builder_append_sequence_number:
 * @midi_builder: the #AgsMidiBuilder
 * @delta_time: the delta time
 * @sequence: the sequence number
 * 
 * Appends sequence number.
 *
 * Since: 0.7.3
 */
void
ags_midi_builder_append_sequence_number(AgsMidiBuilder *midi_builder,
					guint delta_time,
					guint sequence)
{
  g_return_if_fail(AGS_IS_MIDI_BUILDER(midi_builder));
  
  g_object_ref((GObject *) midi_builder);
  g_signal_emit(G_OBJECT(midi_builder),
		midi_builder_signals[SEQUENCE_NUMBER], 0,
		delta_time,
		sequence);
  g_object_unref((GObject *) midi_builder);
}

void
ags_midi_builder_real_append_smtpe(AgsMidiBuilder *midi_builder,
				   guint delta_time,
				   guint rr, guint hr, guint mn, guint se, guint fr)
{
  unsigned char *buffer;

  guint delta_time_size;

  delta_time_size = ags_midi_buffer_util_get_varlength_size(delta_time);
  
  buffer = (unsigned char *) malloc((delta_time_size + 8) * sizeof(unsigned char));
  ags_midi_buffer_util_put_smtpe(buffer,
				 delta_time,
				 rr, hr, mn, se, fr);
  ags_midi_builder_track_insert_midi_message(midi_builder->current_midi_track,
					     buffer, delta_time_size + 8);
}

/**
 * ags_midi_builder_append_smtpe:
 * @midi_builder: the #AgsMidiBuilder
 * @delta_time: the delta time
 * @rr: rate
 * @hr: hours
 * @mn: minutes
 * @se: seconds
 * @fr: fraction
 * @ff: fraction functio
 * 
 * Appends smtpe.
 *
 * Since: 0.7.3
 */
void
ags_midi_builder_append_smtpe(AgsMidiBuilder *midi_builder,
			      guint delta_time,
			      guint rr, guint hr, guint mn, guint se, guint fr)
{
  g_return_if_fail(AGS_IS_MIDI_BUILDER(midi_builder));
  
  g_object_ref((GObject *) midi_builder);
  g_signal_emit(G_OBJECT(midi_builder),
		midi_builder_signals[SMTPE], 0,
		delta_time,
		rr, hr, mn, se, fr);
  g_object_unref((GObject *) midi_builder);
}

void
ags_midi_builder_real_append_tempo(AgsMidiBuilder *midi_builder,
				   guint delta_time,
				   guint tempo)
{
  unsigned char *buffer;

  guint delta_time_size;

  delta_time_size = ags_midi_buffer_util_get_varlength_size(delta_time);
  
  buffer = (unsigned char *) malloc((delta_time_size + 6) * sizeof(unsigned char));
  ags_midi_buffer_util_put_tempo(buffer,
				 delta_time,
				 tempo);
  ags_midi_builder_track_insert_midi_message(midi_builder->current_midi_track,
					     buffer, delta_time_size + 6);
}


/**
 * ags_midi_builder_append_tempo:
 * @midi_builder: the #AgsMidiBuilder
 * @delta_time: the delta time
 * @tempo: the tempo
 * 
 * Appends tempo.
 *
 * Since: 0.7.3
 */
void
ags_midi_builder_append_tempo(AgsMidiBuilder *midi_builder,
			      guint delta_time,
			      guint tempo)
{
  g_return_if_fail(AGS_IS_MIDI_BUILDER(midi_builder));
  
  g_object_ref((GObject *) midi_builder);
  g_signal_emit(G_OBJECT(midi_builder),
		midi_builder_signals[TEMPO], 0,
		delta_time,
		tempo);
  g_object_unref((GObject *) midi_builder);
}

void
ags_midi_builder_real_append_time_signature(AgsMidiBuilder *midi_builder,
					    guint delta_time,
					    guint nn, guint dd, guint cc, guint bb)
{
  unsigned char *buffer;

  guint delta_time_size;

  delta_time_size = ags_midi_buffer_util_get_varlength_size(delta_time);
  
  buffer = (unsigned char *) malloc((delta_time_size + 7) * sizeof(unsigned char));
  ags_midi_buffer_util_put_time_signature(buffer,
					  delta_time,
					  nn, dd, cc, bb);
  ags_midi_builder_track_insert_midi_message(midi_builder->current_midi_track,
					     buffer, delta_time_size + 7);
}

/**
 * ags_midi_builder_append_time_signature:
 * @midi_builder: the #AgsMidiBuilder
 * @delta_time: the delta time
 * @nn: numerator
 * @dd: denominator
 * @cc: ticks per metronome click
 * @bb: 32nd per quarter note
 *
 * Appends time signature.
 *
 * Since: 0.7.3
 */
void
ags_midi_builder_append_time_signature(AgsMidiBuilder *midi_builder,
				       guint delta_time,
				       guint nn, guint dd, guint cc, guint bb)
{
  g_return_if_fail(AGS_IS_MIDI_BUILDER(midi_builder));
  
  g_object_ref((GObject *) midi_builder);
  g_signal_emit(G_OBJECT(midi_builder),
		midi_builder_signals[TIME_SIGNATURE], 0,
		delta_time,
		nn, dd, cc, bb);
  g_object_unref((GObject *) midi_builder);
}

void
ags_midi_builder_real_append_key_signature(AgsMidiBuilder *midi_builder,
					   guint delta_time,
					   guint sf, guint mi)
{
  unsigned char *buffer;

  guint delta_time_size;

  delta_time_size = ags_midi_buffer_util_get_varlength_size(delta_time);
  
  buffer = (unsigned char *) malloc((delta_time_size + 5) * sizeof(unsigned char));
  ags_midi_buffer_util_put_key_signature(buffer,
					 delta_time,
					 sf, mi);
  ags_midi_builder_track_insert_midi_message(midi_builder->current_midi_track,
					     buffer, delta_time_size + 5);
}

/**
 * ags_midi_builder_append_key_signature:
 * @midi_builder: the #AgsMidiBuilder
 * @delta_time: the delta time
 * @sf: key
 * @mi: mode
 *
 * Appends key signature.
 *
 * Since: 0.7.3
 */
void
ags_midi_builder_append_key_signature(AgsMidiBuilder *midi_builder,
				      guint delta_time,
				      guint sf, guint mi)
{
  g_return_if_fail(AGS_IS_MIDI_BUILDER(midi_builder));
  
  g_object_ref((GObject *) midi_builder);
  g_signal_emit(G_OBJECT(midi_builder),
		midi_builder_signals[KEY_SIGNATURE], 0,
		delta_time,
		sf, mi);
  g_object_unref((GObject *) midi_builder);
}

void
ags_midi_builder_real_append_sequencer_meta_event(AgsMidiBuilder *midi_builder,
						  guint delta_time,
						  guint len, guint id, guint data)
{
  unsigned char *buffer;

  guint delta_time_size;

  delta_time_size = ags_midi_buffer_util_get_varlength_size(delta_time);
  
  buffer = (unsigned char *) malloc((delta_time_size + len + 3) * sizeof(unsigned char));
  ags_midi_buffer_util_put_sequencer_meta_event(buffer,
						delta_time,
						len, id, data);
  ags_midi_builder_track_insert_midi_message(midi_builder->current_midi_track,
					     buffer, delta_time_size + len + 3);
}

/**
 * ags_midi_builder_append_sequencer_meta_event:
 * @midi_builder: the #AgsMidiBuilder
 * @delta_time: the delta time
 * @len: the length
 * @id: the id
 * @data: the data
 * 
 * Appends sequencer meta event.
 *
 * Since: 0.7.3
 */
void
ags_midi_builder_append_sequencer_meta_event(AgsMidiBuilder *midi_builder,
					     guint delta_time,
					     guint len, guint id, guint data)
{
  g_return_if_fail(AGS_IS_MIDI_BUILDER(midi_builder));
  
  g_object_ref((GObject *) midi_builder);
  g_signal_emit(G_OBJECT(midi_builder),
		midi_builder_signals[SEQUENCER_META_EVENT], 0,
		delta_time,
		len, id, data);
  g_object_unref((GObject *) midi_builder);
}

void
ags_midi_builder_real_append_text_event(AgsMidiBuilder *midi_builder,
					guint delta_time,
					gchar *text, guint length)
{
  unsigned char *buffer;

  guint delta_time_size;

  if(length > 0){
    if(text[length - 1] == '\0'){
      length--;
    }
  }
  
  delta_time_size = ags_midi_buffer_util_get_varlength_size(delta_time);
  
  buffer = (unsigned char *) malloc((delta_time_size + length + 3) * sizeof(unsigned char));
  ags_midi_buffer_util_put_text_event(buffer,
				      delta_time,
				      text, length);
  ags_midi_builder_track_insert_midi_message(midi_builder->current_midi_track,
					     buffer, delta_time_size + length + 3);
}

/**
 * ags_midi_builder_append_text_event:
 * @midi_builder: the #AgsMidiBuilder
 * @delta_time: the delta time
 * @text: the string
 * @length: the string length
 *
 * Appends text event.
 * 
 * Since: 0.7.3
 */
void
ags_midi_builder_append_text_event(AgsMidiBuilder *midi_builder,
				   guint delta_time,
				   gchar *text, guint length)
{
  g_return_if_fail(AGS_IS_MIDI_BUILDER(midi_builder));
  
  g_object_ref((GObject *) midi_builder);
  g_signal_emit(G_OBJECT(midi_builder),
		midi_builder_signals[TEXT_EVENT], 0,
		delta_time,
		text, length);
  g_object_unref((GObject *) midi_builder);
}

/**
 * ags_midi_builder_append_xml_node:
 * @midi_builder: the #AgsMidiBuilder
 * @node: the xmlNode
 * 
 * Append from XML node @node.
 * 
 * Since: 0.7.3
 */
void
ags_midi_builder_append_xml_node(AgsMidiBuilder *midi_builder,
				 xmlNode *node)
{
  auto void ags_midi_builder_append_xml_node_header(AgsMidiBuilder *midi_builder,
						    xmlNode *node);
  auto void ags_midi_builder_append_xml_node_tracks(AgsMidiBuilder *midi_builder,
						    xmlNode *node);
  auto void ags_midi_builder_append_xml_node_track(AgsMidiBuilder *midi_builder,
						   xmlNode *node);
  auto void ags_midi_builder_append_xml_node_message(AgsMidiBuilder *midi_builder,
						     xmlNode *node);
  auto void ags_midi_builder_append_xml_node_system_common(AgsMidiBuilder *midi_builder,
							   xmlNode *node);
  auto void ags_midi_builder_append_xml_node_meta_event(AgsMidiBuilder *midi_builder,
							xmlNode *node);

  void ags_midi_builder_append_xml_node_header(AgsMidiBuilder *midi_builder,
					       xmlNode *node){
    xmlChar *str;
    
    guint offset;
    guint format;
    guint track_count;
    guint division;

    /* offset */
    str = xmlGetProp(node,
		     "offset\0");
    offset = 0;
    
    if(str != NULL){
      offset = g_ascii_strtoull(str,
				NULL,
				10);
    }

    /* format */
    str = xmlGetProp(node,
		     "format\0");
    format = 0;
    
    if(str != NULL){
      format = g_ascii_strtoull(str,
				NULL,
				10);
    }

    /* track-count */
    str = xmlGetProp(node,
		     "track-count\0");
    track_count = 0;
    
    if(str != NULL){
      track_count = g_ascii_strtoull(str,
				     NULL,
				     10);
    }

    /* division */
    str = xmlGetProp(node,
		     "division\0");
    division = 0;
    
    if(str != NULL){
      division = g_ascii_strtoull(str,
				  NULL,
				  10);
    }

    /* append header */
    ags_midi_builder_append_header(midi_builder,
				   offset, format,
				   track_count, division,
				   0, 0,
				   0);
  }
  
  void ags_midi_builder_append_xml_node_tracks(AgsMidiBuilder *midi_builder,
					       xmlNode *node){
    xmlNode *child;

    child = node->children;

    while(child != NULL){
      if(child->type == XML_ELEMENT_NODE){
	if(!xmlStrncmp(child->name,
		       (xmlChar *) "midi-track\0",
		       11)){
	  ags_midi_builder_append_xml_node_track(midi_builder,
						 child);
	}
      }
      
      child = child->next;
    }
  }
    
  void ags_midi_builder_append_xml_node_track(AgsMidiBuilder *midi_builder,
					      xmlNode *node){
    xmlNode *child;

    xmlChar *str;

    guint offset;

    /* offset */
    str = xmlGetProp(node,
		     "offset\0");
    offset = 0;
    
    if(str != NULL){
      offset = g_ascii_strtoull(str,
				NULL,
				10);
    }

    ags_midi_builder_append_track(midi_builder,
				  NULL);
    midi_builder->current_midi_track->offset = offset;
    ags_midi_buffer_util_put_track(midi_builder->current_midi_track->data,
				   0);

    /* child nodes */
    child = node->children;

    while(child != NULL){
      if(child->type == XML_ELEMENT_NODE){
	if(!xmlStrncmp(child->name,
		       (xmlChar *) "midi-message\0",
		       13)){
	  ags_midi_builder_append_xml_node_message(midi_builder,
						   child);
	}else if(!xmlStrncmp(child->name,
		       (xmlChar *) "midi-system-common\0",
		       19)){
	  ags_midi_builder_append_xml_node_system_common(midi_builder,
							 child);
	}else if(!xmlStrncmp(child->name,
		       (xmlChar *) "meta-event\0",
		       11)){
	  ags_midi_builder_append_xml_node_meta_event(midi_builder,
						      child);
	}
      }
      
      child = child->next;
    }
  }
  
  void ags_midi_builder_append_xml_node_message(AgsMidiBuilder *midi_builder,
						xmlNode *node){
    xmlChar *event;
    xmlChar *str;
    
    guint delta_time;

    
    /* get event */
    event = xmlGetProp(node,
		       "event\0");

    if(event == NULL){
      return;
    }

    /* get event */
    delta_time = 0;
    str = xmlGetProp(node,
		     "delta-time\0");

    if(str != NULL){
      delta_time = g_ascii_strtoull(str,
				    NULL,
				    10);
    }
    
    /* compute event */
    if(!xmlStrncmp(event,
		   "note-on\0",
		   8)){
      guint key, note, velocity;

      /* key */
      key = 0;
      str = xmlGetProp(node,
		       "key\0");
      
      if(str != NULL){
	key = g_ascii_strtoull(str,
			       NULL,
			       10);
      }

      /* note */
      note = 0;
      str = xmlGetProp(node,
		       "note\0");
      
      if(str != NULL){
	note = g_ascii_strtoull(str,
				NULL,
				10);
      }

      /* velocity */
      velocity = 0;
      str = xmlGetProp(node,
		       "velocity\0");
      
      if(str != NULL){
	velocity = g_ascii_strtoull(str,
				    NULL,
				    10);
      }

      /* append */
      ags_midi_builder_append_key_on(midi_builder,
				     delta_time,
				     key,
				     note,
				     velocity);
    }else if(!xmlStrncmp(event,
			 "note-off\0",
			 9)){
      guint key, note, velocity;

      /* key */
      key = 0;
      str = xmlGetProp(node,
		       "key\0");
      
      if(str != NULL){
	key = g_ascii_strtoull(str,
			       NULL,
			       10);
      }

      /* note */
      note = 0;
      str = xmlGetProp(node,
		       "note\0");
      
      if(str != NULL){
	note = g_ascii_strtoull(str,
				NULL,
				10);
      }

      /* velocity */
      velocity = 0;
      str = xmlGetProp(node,
		       "velocity\0");
      
      if(str != NULL){
	velocity = g_ascii_strtoull(str,
				    NULL,
				    10);
      }

      /* append */
      ags_midi_builder_append_key_off(midi_builder,
				      delta_time,
				      key,
				      note,
				      velocity);
    }else if(!xmlStrncmp(event,
			 "polyphonic\0",
			 11)){
      guint key, note, pressure;

      /* key */
      key = 0;
      str = xmlGetProp(node,
		       "key\0");
      
      if(str != NULL){
	key = g_ascii_strtoull(str,
			       NULL,
			       10);
      }

      /* note */
      note = 0;
      str = xmlGetProp(node,
		       "note\0");
      
      if(str != NULL){
	note = g_ascii_strtoull(str,
				NULL,
				10);
      }

      /* pressure */
      pressure = 0;
      str = xmlGetProp(node,
		       "pressure\0");
      
      if(str != NULL){
	pressure = g_ascii_strtoull(str,
				    NULL,
				    10);
      }

      /* append */
      ags_midi_builder_append_key_pressure(midi_builder,
					   delta_time,
					   key,
					   note,
					   pressure);
    }else if(!xmlStrncmp(event,
			 "change-parameter\0",
			 11)){
      guint channel, control, value;
      
      /* channel */
      channel = 0;
      str = xmlGetProp(node,
		       "channel\0");
      
      if(str != NULL){
	channel = g_ascii_strtoull(str,
				   NULL,
				   10);
      }

      /* control */
      control = 0;
      str = xmlGetProp(node,
		       "control\0");
      
      if(str != NULL){
	control = g_ascii_strtoull(str,
				   NULL,
				   10);
      }

      /* value */
      value = 0;
      str = xmlGetProp(node,
		       "value\0");
      
      if(str != NULL){
	value = g_ascii_strtoull(str,
				 NULL,
				 10);
      }

      /* append */
      ags_midi_builder_append_change_parameter(midi_builder,
					       delta_time,
					       channel,
					       control,
					       value);
    }else if(!xmlStrncmp(event,
			 "pitch-bend\0",
			 11)){
      guint channel, pitch, transmitter;

      /* channel */
      channel = 0;
      str = xmlGetProp(node,
		       "channel\0");
      
      if(str != NULL){
	channel = g_ascii_strtoull(str,
				   NULL,
				   10);
      }

      /* pitch */
      pitch = 0;
      str = xmlGetProp(node,
		       "pitch-bend\0");
      
      if(str != NULL){
	pitch = g_ascii_strtoull(str,
				   NULL,
				   10);
      }

      /* transmitter */
      transmitter = 0;
      str = xmlGetProp(node,
		       "transmitter\0");
      
      if(str != NULL){
	transmitter = g_ascii_strtoull(str,
				   NULL,
				   10);
      }

      /* append */
      ags_midi_builder_append_change_pitch_bend(midi_builder,
						delta_time,
						channel,
						pitch,
						transmitter);
    }else if(!xmlStrncmp(event,
			 "program-change\0",
			 15)){
      guint channel, program;

      /* channel */
      channel = 0;
      str = xmlGetProp(node,
		       "channel\0");
      
      if(str != NULL){
	channel = g_ascii_strtoull(str,
				   NULL,
				   10);
      }

      /* program */
      program = 0;
      str = xmlGetProp(node,
		       "program\0");
      
      if(str != NULL){
	program = g_ascii_strtoull(str,
				   NULL,
				   10);
      }

      /* append */
      ags_midi_builder_append_change_program(midi_builder,
					     delta_time,
					     channel,
					     program);
    }else if(!xmlStrncmp(event,
			 "channel-pressure\0",
			 17)){
      guint channel, pressure;
      
      /* channel */
      channel = 0;
      str = xmlGetProp(node,
		       "channel\0");
      
      if(str != NULL){
	channel = g_ascii_strtoull(str,
				   NULL,
				   10);
      }

      /* pressure */
      pressure = 0;
      str = xmlGetProp(node,
		       "pressure\0");
      
      if(str != NULL){
	pressure = g_ascii_strtoull(str,
				    NULL,
				    10);
      }

      /* append */
      ags_midi_builder_append_change_pressure(midi_builder,
					      delta_time,
					      channel,
					      pressure);
    }else if(!xmlStrncmp(event,
			 "misc\0",
			 5)){
      g_warning("not supported MIDI message misc\0");
    }else if(!xmlStrncmp(event,
			 "sequence-number\0",
			 16)){
      guint sequence;

      /* sequence */
      sequence = 0;
      str = xmlGetProp(node,
		       "sequence\0");
      
      if(str != NULL){
	sequence = g_ascii_strtoull(str,
				    NULL,
				    10);
      }

      /* append */
      ags_midi_builder_append_sequence_number(midi_builder,
					      delta_time,
					      sequence);
    }else if(!xmlStrncmp(event,
			 "end-of-track\0",
			 13)){
      g_warning("not supported MIDI message end-of-track\0");      
    }else if(!xmlStrncmp(event,
			 "smtpe\0",
			 6)){
      guint rr, hr, mn, se, fr;

      /* timestamp */
      rr = 0;
      hr = 0;
      mn = 0;
      se = 0;
      fr = 0;

      str = xmlGetProp(node,
		       "rate\0");
      
      if(str != NULL){
	sscanf(str,
	       "%d\0", &rr);
      }
      
      str = xmlGetProp(node,
		       "timestamp\0");
      
      if(str != NULL){
	sscanf(str,
	       "%d %d %d %d\0", &hr, &mn, &se, &fr);
      }

      /* append */
      ags_midi_builder_append_smtpe(midi_builder,
				    delta_time,
				    rr, hr, mn, se, fr);
    }else if(!xmlStrncmp(event,
			 "tempo-number\0",
			 13)){
      gint tempo;

      /* tempo */
      tempo = 0;
      str = xmlGetProp(node,
		       "tempo\0");
      
      if(str != NULL){
	tempo = g_ascii_strtoull(str,
				    NULL,
				    10);
      }

      /* append */
      ags_midi_builder_append_tempo(midi_builder,
				    delta_time,
				    tempo);
    }else if(!xmlStrncmp(event,
			 "time-signature\0",
			 16)){
      guint nn, dd, cc, bb;
      guint denom = 1;

      /* timesig */
      nn = 0;
      dd = 0;
      cc = 0;
      bb = 0;
      
      str = xmlGetProp(node,
		       "timesign\0");
      
      if(str != NULL){
	sscanf(str,
	       "%d/%d %d %d\0", &nn, &denom, &cc, &bb);
      }

      while(denom != 1){
	dd++;
	
	denom /= 2;
      }
      
      /* append */
      ags_midi_builder_append_time_signature(midi_builder,
					     delta_time,
					     nn, dd, cc, bb);
    }else if(!xmlStrncmp(event,
			 "key-signature\0",
			 14)){
      xmlChar *minor;

      gint sf;
      guint mi;

      /* keysig */
      minor = NULL;

      sf = 0;
      mi = 1;

      str = xmlGetProp(node,
		       "timesign\0");
      
      if(str != NULL){
	sscanf(str,
	       "%d %s\0", &sf, &minor);
      }

      /* sharp flats */
      if(sf < 0){
	sf += 256;
      }
      
      /* minor */
      if(!g_ascii_strncasecmp(minor,
			      "minor\0",
			      6)){
	mi = 1;
      }else{
	mi = 0;
      }

      /* append */
      ags_midi_builder_append_key_signature(midi_builder,
					    delta_time,
					    sf, mi);
    }
  }
  
  void ags_midi_builder_append_xml_node_system_common(AgsMidiBuilder *midi_builder,
						      xmlNode *node){
    xmlChar *str;

    guint delta_time;

    /* get delta-time */
    delta_time = 0;
    str = xmlGetProp(node,
		     "delta-time\0");

    if(str != NULL){
      delta_time = g_ascii_strtoull(str,
				    NULL,
				    10);
    }
    
    /* parse node */
    if((str = xmlGetProp(node,
			 "quarter-frame\0")) != NULL){
      guint quarter_frame;
      guint message_type, values;
      
      /* get quarter frame */
      quarter_frame = 0;
      str = xmlGetProp(node,
		       "quarter-frame\0");

      if(str != NULL){
	quarter_frame = g_ascii_strtoull(str,
					 NULL,
					 10);
      }

      message_type = 0x70 & quarter_frame;
      values = 0x0f & quarter_frame;
      
      /* append */
      ags_midi_builder_append_quarter_frame(midi_builder,
					    delta_time,
					    message_type, values);
    }else if((str = xmlGetProp(node,
			       "song-position\0")) != NULL){
      guint song_position;

      /* get song position */
      song_position = 0;
      str = xmlGetProp(node,
		       "song-position\0");

      if(str != NULL){
	song_position = g_ascii_strtoull(str,
					 NULL,
					 10);
      }
          
      /* append */
      ags_midi_builder_append_song_position(midi_builder,
					    delta_time,
					    song_position);
    }else if((str = xmlGetProp(node,
			       "song-select\0")) != NULL){
      guint song_select;
    
      /* get song select */
      song_select = 0;
      str = xmlGetProp(node,
		       "song-select\0");

      if(str != NULL){
	song_select = g_ascii_strtoull(str,
				       NULL,
				       10);
      }

      /* append */
      ags_midi_builder_append_song_select(midi_builder,
					  delta_time,
					  song_select);
    }
  }
  
  void ags_midi_builder_append_xml_node_meta_event(AgsMidiBuilder *midi_builder,
						   xmlNode *node){
    xmlChar *str;
    
    guint delta_time;
    guint len, id, data;

    /* get delta-time */
    delta_time = 0;
    str = xmlGetProp(node,
		     "delta-time\0");

    if(str != NULL){
      delta_time = g_ascii_strtoull(str,
				    NULL,
				    10);
    }
    
    /* length */
    len = 0;
    str = xmlGetProp(node,
		     "length\0");
      
    if(str != NULL){
      len = g_ascii_strtoull(str,
			     NULL,
			     10);
    }

    /* id */
    id = 0;
    str = xmlGetProp(node,
		     "id\0");
      
    if(str != NULL){
      id = g_ascii_strtoull(str,
			    NULL,
			    10);
    }

    /* data */
    data = 0;
    str = xmlGetProp(node,
		     "data\0");
      
    if(str != NULL){
      data = g_ascii_strtoull(str,
			      NULL,
			      10);
    }

    /* append */
    ags_midi_builder_append_sequencer_meta_event(midi_builder,
						 delta_time,
						 len, id, data);
  }
  
  if(!xmlStrncmp(node->name,
		 "midi-header\0",
		 12)){
    ags_midi_builder_append_xml_node_header(midi_builder,
					    node);
  }else if(!xmlStrncmp(node->name,
		       "midi-tracks\0",
		       12)){
    ags_midi_builder_append_xml_node_tracks(midi_builder,
					    node);
  }else if(!xmlStrncmp(node->name,
		       "midi-track\0",
		       12)){
    ags_midi_builder_append_xml_node_track(midi_builder,
					   node);
  }else if(!xmlStrncmp(node->name,
		       "midi-message\0",
		       12)){
    ags_midi_builder_append_xml_node_message(midi_builder,
					     node);
  }else if(!xmlStrncmp(node->name,
		       "midi-system-common\0",
		       12)){
    ags_midi_builder_append_xml_node_system_common(midi_builder,
						   node);
  }else if(!xmlStrncmp(node->name,
		       "meta-event\0",
		       12)){
    ags_midi_builder_append_xml_node_meta_event(midi_builder,
						node);
  }
}

/**
 * ags_midi_builder_from_xml_doc:
 * @midi_builder: the #AgsMidiBuilder
 * @doc: the xmlDocument
 *
 * Builds from XML document @doc.
 * 
 * Since: 0.7.3
 */
void
ags_midi_builder_from_xml_doc(AgsMidiBuilder *midi_builder,
			      xmlDoc *doc)
{
  xmlNode *root_node;
  xmlNode *child;

  if(midi_builder == NULL ||
     doc == NULL){
    return;
  }

  root_node = xmlDocGetRootElement(doc);

  child = root_node->children;

  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp(child->name,
		     (xmlChar *) "midi-header\0",
		     13)){
	ags_midi_builder_append_xml_node(midi_builder,
					 child);
      }else if(!xmlStrncmp(child->name,
		     (xmlChar *) "midi-tracks\0",
		     13)){
	ags_midi_builder_append_xml_node(midi_builder,
					 child);
      }
    }

    child = child->next;
  }
}

/**
 * ags_midi_builder_build:
 * @midi_builder: the #AgsMidiBuilder
 *
 * Build the MIDI data.
 * 
 * Since: 0.7.3
 */
void
ags_midi_builder_build(AgsMidiBuilder *midi_builder)
{
  GList *midi_track;

  unsigned char *offset;
  
  guint length;
  
  if(midi_builder == NULL){
    return;
  }

  /* compute length */
  length = 0;
  
  if(midi_builder->midi_header != NULL){
    length += midi_builder->midi_header->length;
  }

  midi_track = midi_builder->midi_track;

  while(midi_track != NULL){
    length += AGS_MIDI_BUILDER_TRACK(midi_track->data)->length;
    length += 4; //NOTE:JK: EOT
    
    midi_track = midi_track->next;
  }

  /* allocate buffer */
  if(midi_builder->data != NULL){
    free(midi_builder->data);
  }
  
  if(length > 0){
    midi_builder->data = (unsigned char *) malloc(length * sizeof(unsigned char));

    midi_builder->length = length;
  }else{
    midi_builder->data = NULL;
    
    midi_builder->length = 0;

    return;
  }

  /* fill */
  offset = midi_builder->data;
  
  if(midi_builder->midi_header != NULL &
     midi_builder->midi_header->data != NULL){
    memcpy(offset, midi_builder->midi_header->data, midi_builder->midi_header->length * sizeof(unsigned char));
    offset += midi_builder->midi_header->length;
  }
  
  midi_track = midi_builder->midi_track;

  while(midi_track != NULL){
    if(AGS_MIDI_BUILDER_TRACK(midi_track->data)->data != NULL){
      memcpy(offset, AGS_MIDI_BUILDER_TRACK(midi_track->data)->data, AGS_MIDI_BUILDER_TRACK(midi_track->data)->length * sizeof(unsigned char));
      offset += AGS_MIDI_BUILDER_TRACK(midi_track->data)->length;
      
      ags_midi_buffer_util_put_end_of_track(offset,
					    0);
      offset += 4;
    }
  
    midi_track = midi_track->next;
  }
}

/**
 * ags_midi_builder_new:
 * @file: the FILE handle
 * 
 * Creates a new #AgsMidiBuilder
 *
 * Returns: an #AgsMidiBuilder
 * 
 * Since: 0.7.0
 */
AgsMidiBuilder*
ags_midi_builder_new(FILE *file)
{
  AgsMidiBuilder *midi_builder;
  
  midi_builder = (AgsMidiBuilder *) g_object_new(AGS_TYPE_MIDI_BUILDER,
						 "file\0", file,
						 NULL);

  
  return(midi_builder);
}
