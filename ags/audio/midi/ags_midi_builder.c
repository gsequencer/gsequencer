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
					guint hr, guint mn, guint se, guint fr, guint ff);
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
		 g_cclosure_user_marshal_VOID__UINT_UINT,
		 G_TYPE_NONE, 2,
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
		 g_cclosure_user_marshal_VOID__UINT_UINT_UINT_UINT_UINT,
		 G_TYPE_NONE, 5,
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
		 g_cclosure_marshal_VOID__UINT,
		 G_TYPE_NONE, 1,
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
		 g_cclosure_marshal_VOID__UINT,
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
  midi_builder->file = NULL;
  
  midi_builder->midi_header = NULL;
  
  midi_builder->midi_track = NULL;
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

void
ags_midi_builder_midi_putc(AgsMidiBuilder *midi_builder,
			   int c)
{
  //TODO:JK: implement me
}

void
ags_midi_builder_real_midi_putc(AgsMidiBuilder *midi_builder,
				int c)
{
  //TODO:JK: implement me
}

void
ags_midi_builder_real_on_error(AgsMidiBuilder *midi_builder,
			       GError **error)
{
  if(error != NULL){
    g_warning("%s\0", error);
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
  //TODO:JK: implement me
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
  //TODO:JK: implement me
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
  //TODO:JK: implement me
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
  //TODO:JK: implement me
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
  //TODO:JK: implement me
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
  //TODO:JK: implement me
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
  //TODO:JK: implement me
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
  //TODO:JK: implement me
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
  //TODO:JK: implement me
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
  //TODO:JK: implement me
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
  //TODO:JK: implement me
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
  //TODO:JK: implement me
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
  //TODO:JK: implement me
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
  //TODO:JK: implement me
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
  //TODO:JK: implement me
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
				   guint hr, guint mn, guint se, guint fr, guint ff)
{
  //TODO:JK: implement me
}

/**
 * ags_midi_builder_append_smtpe:
 * @midi_builder: the #AgsMidiBuilder
 * @delta_time: the delta time
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
			      guint hr, guint mn, guint se, guint fr, guint ff)
{
  g_return_if_fail(AGS_IS_MIDI_BUILDER(midi_builder));
  
  g_object_ref((GObject *) midi_builder);
  g_signal_emit(G_OBJECT(midi_builder),
		midi_builder_signals[SMTPE], 0,
		delta_time,
		hr, mn, se, fr, ff);
  g_object_unref((GObject *) midi_builder);
}

void
ags_midi_builder_real_append_tempo(AgsMidiBuilder *midi_builder,
				   guint delta_time,
				   guint tempo)
{
  //TODO:JK: implement me
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
  //TODO:JK: implement me
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
  //TODO:JK: implement me
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
  //TODO:JK: implement me
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
  //TODO:JK: implement me
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
  //TODO:JK: implement me
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
  //TODO:JK: implement me
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
  //TODO:JK: implement me
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
