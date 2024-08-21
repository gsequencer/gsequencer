/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2024 Joël Krähemann
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

#include <ags/audio/midi/ags_midi_parser.h>

#include <ags/libags.h>

#include <string.h>

#include <fcntl.h>
#include <sys/stat.h>

#include <ags/i18n.h>

void ags_midi_parser_class_init(AgsMidiParserClass *midi_parser);
void ags_midi_parser_init(AgsMidiParser *midi_parser);
void ags_midi_parser_set_property(GObject *gobject,
				  guint prop_id,
				  const GValue *value,
				  GParamSpec *param_spec);
void ags_midi_parser_get_property(GObject *gobject,
				  guint prop_id,
				  GValue *value,
				  GParamSpec *param_spec);
void ags_midi_parser_finalize(GObject *gobject);

int ags_midi_parser_real_midi_getc(AgsMidiParser *midi_parser);
void ags_midi_parser_real_on_error(AgsMidiParser *midi_parser,
				   GError **error);

xmlDoc* ags_midi_parser_real_parse_full(AgsMidiParser *midi_parser);
xmlNode* ags_midi_parser_real_parse_header(AgsMidiParser *midi_parser);
xmlNode* ags_midi_parser_real_parse_track(AgsMidiParser *midi_parser);

xmlNode* ags_midi_parser_real_key_on(AgsMidiParser *midi_parser, guint status);
xmlNode* ags_midi_parser_real_key_off(AgsMidiParser *midi_parser, guint status);
xmlNode* ags_midi_parser_real_key_pressure(AgsMidiParser *midi_parser, guint status);
xmlNode* ags_midi_parser_real_change_parameter(AgsMidiParser *midi_parser, guint status);
xmlNode* ags_midi_parser_real_change_pitch_bend(AgsMidiParser *midi_parser, guint status);
xmlNode* ags_midi_parser_real_change_program(AgsMidiParser *midi_parser, guint status);
xmlNode* ags_midi_parser_real_change_channel_pressure(AgsMidiParser *midi_parser, guint status);
xmlNode* ags_midi_parser_real_not_defined(AgsMidiParser *midi_parser, guint status);

xmlNode* ags_midi_parser_real_sysex(AgsMidiParser *midi_parser, guint status);
xmlNode* ags_midi_parser_real_system_common(AgsMidiParser *midi_parser, guint status);

xmlNode* ags_midi_parser_real_meta_event(AgsMidiParser *midi_parser, guint status);
xmlNode* ags_midi_parser_real_sequence_number(AgsMidiParser *midi_parser, guint meta_type);
xmlNode* ags_midi_parser_real_end_of_track(AgsMidiParser *midi_parser, guint meta_type);
xmlNode* ags_midi_parser_real_smtpe(AgsMidiParser *midi_parser, guint meta_type);
xmlNode* ags_midi_parser_real_tempo(AgsMidiParser *midi_parser, guint meta_type);
xmlNode* ags_midi_parser_real_time_signature(AgsMidiParser *midi_parser, guint meta_type);
xmlNode* ags_midi_parser_real_key_signature(AgsMidiParser *midi_parser, guint meta_type);
xmlNode* ags_midi_parser_real_sequencer_meta_event(AgsMidiParser *midi_parser, guint meta_type);
xmlNode* ags_midi_parser_real_text_event(AgsMidiParser *midi_parser, guint meta_type);

xmlNode* ags_midi_parser_real_meta_misc(AgsMidiParser *midi_parser, guint meta_type);

xmlNode* ags_midi_parser_real_midi_channel_prefix(AgsMidiParser *midi_parser, guint meta_type);

xmlNode* ags_midi_parser_real_quarter_frame(AgsMidiParser *midi_parser, guint status);
xmlNode* ags_midi_parser_real_song_position(AgsMidiParser *midi_parser, guint status);
xmlNode* ags_midi_parser_real_song_select(AgsMidiParser *midi_parser, guint status);
xmlNode* ags_midi_parser_real_tune_request(AgsMidiParser *midi_parser, guint status);

/**
 * SECTION:ags_midi_parser
 * @short_description: the MIDI parser
 * @title: AgsMidiParser
 * @section_id:
 * @include: ags/audio/midi/ags_midi_parser.h
 *
 * #AgsMidiParser parses your midi.
 */

enum{
  PROP_0,
  PROP_FILE,
};

enum{
  MIDI_GETC,
  ON_ERROR,
  PARSE_FULL,
  PARSE_BYTES,
  PARSE_HEADER,
  PARSE_TRACK,
  KEY_ON,
  KEY_OFF,
  KEY_PRESSURE,
  CHANGE_PARAMETER,
  CHANGE_PITCH_BEND,
  CHANGE_PROGRAM,
  CHANGE_CHANNEL_PRESSURE,
  NOT_DEFINED,
  SYSEX,
  SYSTEM_COMMON,
  META_EVENT,
  SEQUENCE_NUMBER,
  END_OF_TRACK,
  SMTPE,
  TEMPO,
  TIME_SIGNATURE,
  KEY_SIGNATURE,
  SEQUENCER_META_EVENT,
  TEXT_EVENT,
  META_MISC,
  MIDI_CHANNEL_PREFIX,
  QUARTER_FRAME,
  SONG_SELECT,
  SONG_POSITION,
  TUNE_REQUEST,
  LAST_SIGNAL,
};

static gpointer ags_midi_parser_parent_class = NULL;
static guint midi_parser_signals[LAST_SIGNAL];

GType
ags_midi_parser_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_midi_parser = 0;

    static const GTypeInfo ags_midi_parser_info = {
      sizeof (AgsMidiParserClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_midi_parser_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsMidiParser),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_midi_parser_init,
    };

    ags_type_midi_parser = g_type_register_static(G_TYPE_OBJECT,
						  "AgsMidiParser", &ags_midi_parser_info,
						  0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_midi_parser);
  }

  return g_define_type_id__volatile;
}

void
ags_midi_parser_class_init(AgsMidiParserClass *midi_parser)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;
  
  ags_midi_parser_parent_class = g_type_class_peek_parent(midi_parser);

  /* GObjectClass */
  gobject = (GObjectClass *) midi_parser;

  gobject->set_property = ags_midi_parser_set_property;
  gobject->get_property = ags_midi_parser_get_property;
  
  gobject->finalize = ags_midi_parser_finalize;

  /* properties */
  /**
   * AgsMidiParser:file:
   *
   * The file to parse data from.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_pointer("file",
				    i18n_pspec("the file stream"),
				    i18n_pspec("The file stream to parse"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FILE,
				  param_spec);

  /* AgsMidiParser */
  midi_parser->midi_getc = ags_midi_parser_real_midi_getc;
  midi_parser->on_error = ags_midi_parser_real_on_error;

  midi_parser->parse_full = ags_midi_parser_real_parse_full;
  
  midi_parser->parse_header = ags_midi_parser_real_parse_header;
  midi_parser->parse_track = ags_midi_parser_real_parse_track;

  midi_parser->key_on = ags_midi_parser_real_key_on;
  midi_parser->key_off = ags_midi_parser_real_key_off;
  midi_parser->key_pressure = ags_midi_parser_real_key_pressure;
  midi_parser->change_parameter = ags_midi_parser_real_change_parameter;
  midi_parser->change_pitch_bend = ags_midi_parser_real_change_pitch_bend;
  midi_parser->change_program = ags_midi_parser_real_change_program;
  midi_parser->change_channel_pressure = ags_midi_parser_real_change_channel_pressure;
  midi_parser->not_defined = ags_midi_parser_real_not_defined;

  midi_parser->sysex = ags_midi_parser_real_sysex;
  midi_parser->system_common = ags_midi_parser_real_system_common;

  midi_parser->meta_event = ags_midi_parser_real_meta_event;
  midi_parser->sequence_number = ags_midi_parser_real_sequence_number;
  midi_parser->end_of_track = ags_midi_parser_real_end_of_track;
  midi_parser->smtpe = ags_midi_parser_real_smtpe;
  midi_parser->tempo = ags_midi_parser_real_tempo;
  midi_parser->time_signature = ags_midi_parser_real_time_signature;
  midi_parser->key_signature = ags_midi_parser_real_key_signature;
  midi_parser->sequencer_meta_event = ags_midi_parser_real_sequencer_meta_event;
  midi_parser->text_event = ags_midi_parser_real_text_event;

  midi_parser->meta_misc = ags_midi_parser_meta_misc;

  midi_parser->midi_channel_prefix = ags_midi_parser_midi_channel_prefix;

  midi_parser->quarter_frame = ags_midi_parser_real_quarter_frame;
  midi_parser->song_position = ags_midi_parser_real_song_position;
  midi_parser->song_select = ags_midi_parser_real_song_select;
  midi_parser->tune_request = ags_midi_parser_real_tune_request;

  /* signals */
  /**
   * AgsMidiParser::midi-getc:
   * @midi_parser: the #AgsMidiParser
   *
   * The ::midi-getc signal is emited during parsing of event.
   *
   * Returns: The character read
   *
   * Since: 3.0.0
   */
  midi_parser_signals[MIDI_GETC] =
    g_signal_new("midi-getc",
		 G_TYPE_FROM_CLASS(midi_parser),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsMidiParserClass, midi_getc),
		 NULL, NULL,
		 ags_cclosure_marshal_INT__VOID,
		 G_TYPE_INT, 0);

  /**
   * AgsMidiParser::on-error:
   * @midi_parser: the #AgsMidiParser
   * @error: the return location of #GError
   *
   * The ::on-error signal is emited as error occurs.
   *
   * Since: 3.0.0
   */
  midi_parser_signals[ON_ERROR] =
    g_signal_new("on-error",
		 G_TYPE_FROM_CLASS(midi_parser),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsMidiParserClass, on_error),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__POINTER,
		 G_TYPE_NONE, 1,
		 G_TYPE_POINTER);

  /**
   * AgsMidiParser::parse-full:
   * @midi_parser: the #AgsMidiParser
   *
   * The ::parse-full signal is emited during parsing of midi file.
   *
   * Returns: The XML doc
   *
   * Since: 3.0.0
   */
  midi_parser_signals[PARSE_FULL] =
    g_signal_new("parse-full",
		 G_TYPE_FROM_CLASS(midi_parser),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsMidiParserClass, parse_full),
		 NULL, NULL,
		 ags_cclosure_marshal_POINTER__VOID,
		 G_TYPE_POINTER, 0);

  /**
   * AgsMidiParser::parse-bytes:
   * @midi_parser: the #AgsMidiParser
   * @buffer: the MIDI data
   * @buffer_length: the buffer's length
   *
   * The ::parse-bytes signal is emited during parsing of bytes.
   *
   * Returns: The XML node representing the event
   *
   * Since: 3.0.0
   */
  midi_parser_signals[PARSE_BYTES] =
    g_signal_new("parse-bytes",
		 G_TYPE_FROM_CLASS(midi_parser),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsMidiParserClass, parse_bytes),
		 NULL, NULL,
		 ags_cclosure_marshal_POINTER__POINTER_UINT,
		 G_TYPE_POINTER, 2,
		 G_TYPE_POINTER,
		 G_TYPE_UINT);

  /**
   * AgsMidiParser::parse-header:
   * @midi_parser: the #AgsMidiParser
   *
   * The ::parse-header signal is emited during parsing of header.
   *
   * Returns: The XML node representing the header
   *
   * Since: 3.0.0
   */
  midi_parser_signals[PARSE_HEADER] =
    g_signal_new("parse-header",
		 G_TYPE_FROM_CLASS(midi_parser),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsMidiParserClass, parse_header),
		 NULL, NULL,
		 ags_cclosure_marshal_POINTER__VOID,
		 G_TYPE_POINTER, 0);

  /**
   * AgsMidiParser::parse-track:
   * @midi_parser: the #AgsMidiParser
   *
   * The ::parse-track signal is emited during parsing of track.
   *
   * Returns: The XML node representing the track
   *
   * Since: 3.0.0
   */
  midi_parser_signals[PARSE_TRACK] =
    g_signal_new("parse-track",
		 G_TYPE_FROM_CLASS(midi_parser),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsMidiParserClass, parse_track),
		 NULL, NULL,
		 ags_cclosure_marshal_POINTER__VOID,
		 G_TYPE_POINTER, 0);

  /**
   * AgsMidiParser::key-on:
   * @midi_parser: the #AgsMidiParser
   * @status: the MIDI status byte
   *
   * The ::key-on signal is emited during parsing of event.
   *
   * Returns: The XML node representing the event
   *
   * Since: 3.0.0
   */
  midi_parser_signals[KEY_ON] =
    g_signal_new("key-on",
		 G_TYPE_FROM_CLASS(midi_parser),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsMidiParserClass, key_on),
		 NULL, NULL,
		 ags_cclosure_marshal_POINTER__UINT,
		 G_TYPE_POINTER, 1,
		 G_TYPE_UINT);

  /**
   * AgsMidiParser::key-off:
   * @midi_parser: the #AgsMidiParser
   * @status: the MIDI status byte
   *
   * The ::key-off signal is emited during parsing of event.
   *
   * Returns: The XML node representing the event
   *
   * Since: 3.0.0
   */
  midi_parser_signals[KEY_OFF] =
    g_signal_new("key-off",
		 G_TYPE_FROM_CLASS(midi_parser),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsMidiParserClass, key_off),
		 NULL, NULL,
		 ags_cclosure_marshal_POINTER__UINT,
		 G_TYPE_POINTER, 1,
		 G_TYPE_UINT);

  /**
   * AgsMidiParser::key-pressure:
   * @midi_parser: the #AgsMidiParser
   * @status: the MIDI status byte
   *
   * The ::key-pressure signal is emited during parsing of event.
   *
   * Returns: The XML node representing the event
   *
   * Since: 3.0.0
   */
  midi_parser_signals[KEY_PRESSURE] =
    g_signal_new("key-pressure",
		 G_TYPE_FROM_CLASS(midi_parser),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsMidiParserClass, key_pressure),
		 NULL, NULL,
		 ags_cclosure_marshal_POINTER__UINT,
		 G_TYPE_POINTER, 1,
		 G_TYPE_UINT);

  /**
   * AgsMidiParser::change-parameter:
   * @midi_parser: the #AgsMidiParser
   * @status: the MIDI status byte
   *
   * The ::change-parameter signal is emited during parsing of event.
   *
   * Returns: The XML node representing the event
   *
   * Since: 3.0.0
   */
  midi_parser_signals[CHANGE_PARAMETER] =
    g_signal_new("change-parameter",
		 G_TYPE_FROM_CLASS(midi_parser),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsMidiParserClass, change_parameter),
		 NULL, NULL,
		 ags_cclosure_marshal_POINTER__UINT,
		 G_TYPE_POINTER, 1,
		 G_TYPE_UINT);

  /**
   * AgsMidiParser::change-pitch-bend:
   * @midi_parser: the #AgsMidiParser
   * @status: the MIDI status byte
   *
   * The ::change-pitch-bend signal is emited during parsing of event.
   *
   * Returns: The XML node representing the event
   *
   * Since: 3.0.0
   */
  midi_parser_signals[CHANGE_PITCH_BEND] =
    g_signal_new("change-pitch-bend",
		 G_TYPE_FROM_CLASS(midi_parser),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsMidiParserClass, change_pitch_bend),
		 NULL, NULL,
		 ags_cclosure_marshal_POINTER__UINT,
		 G_TYPE_POINTER, 1,
		 G_TYPE_UINT);

  /**
   * AgsMidiParser::change-program:
   * @midi_parser: the #AgsMidiParser
   * @status: the MIDI status byte
   *
   * The ::change-program signal is emited during parsing of event.
   *
   * Returns: The XML node representing the event
   *
   * Since: 3.0.0
   */
  midi_parser_signals[CHANGE_PROGRAM] =
    g_signal_new("change-program",
		 G_TYPE_FROM_CLASS(midi_parser),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsMidiParserClass, change_program),
		 NULL, NULL,
		 ags_cclosure_marshal_POINTER__UINT,
		 G_TYPE_POINTER, 1,
		 G_TYPE_UINT);

  /**
   * AgsMidiParser::change-channel-pressure:
   * @midi_parser: the #AgsMidiParser
   * @status: the MIDI status byte
   *
   * The ::change-channel-pressure signal is emited during parsing of event.
   *
   * Returns: The XML node representing the event
   *
   * Since: 3.0.0
   */
  midi_parser_signals[CHANGE_CHANNEL_PRESSURE] =
    g_signal_new("change-channel-pressure",
		 G_TYPE_FROM_CLASS(midi_parser),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsMidiParserClass, change_channel_pressure),
		 NULL, NULL,
		 ags_cclosure_marshal_POINTER__UINT,
		 G_TYPE_POINTER, 1,
		 G_TYPE_UINT);

  /**
   * AgsMidiParser::not-defined:
   * @midi_parser: the #AgsMidiParser
   * @status: the MIDI status byte
   *
   * The ::not-defined signal is emited during parsing of event.
   *
   * Returns: The XML node representing the event
   *
   * Since: 3.0.0
   */
  midi_parser_signals[NOT_DEFINED] =
    g_signal_new("not-defined",
		 G_TYPE_FROM_CLASS(midi_parser),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsMidiParserClass, not_defined),
		 NULL, NULL,
		 ags_cclosure_marshal_POINTER__UINT,
		 G_TYPE_POINTER, 1,
		 G_TYPE_UINT);

  /**
   * AgsMidiParser::sysex:
   * @midi_parser: the #AgsMidiParser
   * @status: the MIDI status byte
   *
   * The ::sysex signal is emited during parsing of event.
   *
   * Returns: The XML node representing the event
   *
   * Since: 3.0.0
   */
  midi_parser_signals[SYSEX] =
    g_signal_new("sysex",
		 G_TYPE_FROM_CLASS(midi_parser),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsMidiParserClass, sysex),
		 NULL, NULL,
		 ags_cclosure_marshal_POINTER__UINT,
		 G_TYPE_POINTER, 1,
		 G_TYPE_UINT);

  /**
   * AgsMidiParser::system-common:
   * @midi_parser: the #AgsMidiParser
   * @status: the MIDI status byte
   *
   * The ::system-common signal is emited during parsing of event.
   *
   * Returns: The XML node representing the event
   *
   * Since: 3.0.0
   */
  midi_parser_signals[SYSTEM_COMMON] =
    g_signal_new("system-common",
		 G_TYPE_FROM_CLASS(midi_parser),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsMidiParserClass, system_common),
		 NULL, NULL,
		 ags_cclosure_marshal_POINTER__UINT,
		 G_TYPE_POINTER, 1,
		 G_TYPE_UINT);

  /**
   * AgsMidiParser::meta-event:
   * @midi_parser: the #AgsMidiParser
   * @status: the MIDI status byte
   *
   * The ::meta-event signal is emited during parsing of event.
   *
   * Returns: The XML node representing the event
   *
   * Since: 3.0.0
   */
  midi_parser_signals[META_EVENT] =
    g_signal_new("meta-event",
		 G_TYPE_FROM_CLASS(midi_parser),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsMidiParserClass, meta_event),
		 NULL, NULL,
		 ags_cclosure_marshal_POINTER__UINT,
		 G_TYPE_POINTER, 1,
		 G_TYPE_UINT);

  /**
   * AgsMidiParser::sequence-number:
   * @midi_parser: the #AgsMidiParser
   * @status: the MIDI status byte
   *
   * The ::sequence-number signal is emited during parsing of event.
   *
   * Returns: The XML node representing the event
   *
   * Since: 3.0.0
   */
  midi_parser_signals[SEQUENCE_NUMBER] =
    g_signal_new("sequence-number",
		 G_TYPE_FROM_CLASS(midi_parser),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsMidiParserClass, sequence_number),
		 NULL, NULL,
		 ags_cclosure_marshal_POINTER__UINT,
		 G_TYPE_POINTER, 1,
		 G_TYPE_UINT);

  /**
   * AgsMidiParser::end-of-track:
   * @midi_parser: the #AgsMidiParser
   * @status: the MIDI status byte
   *
   * The ::end-of-track signal is emited during parsing of event.
   *
   * Returns: The XML node representing the event
   *
   * Since: 3.0.0
   */
  midi_parser_signals[END_OF_TRACK] =
    g_signal_new("end-of-track",
		 G_TYPE_FROM_CLASS(midi_parser),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsMidiParserClass, end_of_track),
		 NULL, NULL,
		 ags_cclosure_marshal_POINTER__UINT,
		 G_TYPE_POINTER, 1,
		 G_TYPE_UINT);

  /**
   * AgsMidiParser::smtpe:
   * @midi_parser: the #AgsMidiParser
   * @status: the MIDI status byte
   *
   * The ::smtpe signal is emited during parsing of event.
   *
   * Returns: The XML node representing the event
   *
   * Since: 3.0.0
   */
  midi_parser_signals[SMTPE] =
    g_signal_new("smtpe",
		 G_TYPE_FROM_CLASS(midi_parser),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsMidiParserClass, smtpe),
		 NULL, NULL,
		 ags_cclosure_marshal_POINTER__UINT,
		 G_TYPE_POINTER, 1,
		 G_TYPE_UINT);

  /**
   * AgsMidiParser::tempo:
   * @midi_parser: the #AgsMidiParser
   * @status: the MIDI status byte
   *
   * The ::tempo signal is emited during parsing of event.
   *
   * Returns: The XML node representing the event
   *
   * Since: 3.0.0
   */
  midi_parser_signals[TEMPO] =
    g_signal_new("tempo",
		 G_TYPE_FROM_CLASS(midi_parser),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsMidiParserClass, tempo),
		 NULL, NULL,
		 ags_cclosure_marshal_POINTER__UINT,
		 G_TYPE_POINTER, 1,
		 G_TYPE_UINT);

  /**
   * AgsMidiParser::time-signature:
   * @midi_parser: the #AgsMidiParser
   * @status: the MIDI status byte
   *
   * The ::time-signature signal is emited during parsing of event.
   *
   * Returns: The XML node representing the event
   *
   * Since: 3.0.0
   */
  midi_parser_signals[TIME_SIGNATURE] =
    g_signal_new("time-signature",
		 G_TYPE_FROM_CLASS(midi_parser),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsMidiParserClass, time_signature),
		 NULL, NULL,
		 ags_cclosure_marshal_POINTER__UINT,
		 G_TYPE_POINTER, 1,
		 G_TYPE_UINT);

  /**
   * AgsMidiParser::key-signature:
   * @midi_parser: the #AgsMidiParser
   * @status: the MIDI status byte
   *
   * The ::key-signature signal is emited during parsing of event.
   *
   * Returns: The XML node representing the event
   *
   * Since: 3.0.0
   */
  midi_parser_signals[KEY_SIGNATURE] =
    g_signal_new("key-signature",
		 G_TYPE_FROM_CLASS(midi_parser),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsMidiParserClass, key_signature),
		 NULL, NULL,
		 ags_cclosure_marshal_POINTER__UINT,
		 G_TYPE_POINTER, 1,
		 G_TYPE_UINT);

  /**
   * AgsMidiParser::sequencer-meta-event:
   * @midi_parser: the #AgsMidiParser
   * @status: the MIDI status byte
   *
   * The ::sequencer-meta-event signal is emited during parsing of event.
   *
   * Returns: The XML node representing the event
   *
   * Since: 3.0.0
   */
  midi_parser_signals[SEQUENCER_META_EVENT] =
    g_signal_new("sequencer-meta-event",
		 G_TYPE_FROM_CLASS(midi_parser),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsMidiParserClass, sequencer_meta_event),
		 NULL, NULL,
		 ags_cclosure_marshal_POINTER__UINT,
		 G_TYPE_POINTER, 1,
		 G_TYPE_UINT);

  /**
   * AgsMidiParser::text-event:
   * @midi_parser: the #AgsMidiParser
   * @status: the MIDI status byte
   *
   * The ::text-event signal is emited during parsing of event.
   *
   * Returns: The XML node representing the event
   *
   * Since: 3.0.0
   */
  midi_parser_signals[TEXT_EVENT] =
    g_signal_new("text-event",
		 G_TYPE_FROM_CLASS(midi_parser),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsMidiParserClass, text_event),
		 NULL, NULL,
		 ags_cclosure_marshal_POINTER__UINT,
		 G_TYPE_POINTER, 1,
		 G_TYPE_UINT);

  /**
   * AgsMidiParser::meta-misc:
   * @midi_parser: the #AgsMidiParser
   * @meta_type: the MIDI meta message type
   *
   * The ::meta-misc signal is emited during parsing of event.
   *
   * Returns: The XML node representing the event
   *
   * Since: 6.16.18
   */
  midi_parser_signals[META_MISC] =
    g_signal_new("meta-misc",
		 G_TYPE_FROM_CLASS(midi_parser),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsMidiParserClass, meta_misc),
		 NULL, NULL,
		 ags_cclosure_marshal_POINTER__UINT,
		 G_TYPE_POINTER, 1,
		 G_TYPE_UINT);

  /**
   * AgsMidiParser::midi-channel-prefix:
   * @midi_parser: the #AgsMidiParser
   * @meta_type: the MIDI meta message type
   *
   * The ::midi-channel-prefix signal is emited during parsing of event.
   *
   * Returns: The XML node representing the event
   *
   * Since: 6.16.18
   */
  midi_parser_signals[MIDI_CHANNEL_PREFIX] =
    g_signal_new("midi-channel-prefix",
		 G_TYPE_FROM_CLASS(midi_parser),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsMidiParserClass, midi_channel_prefix),
		 NULL, NULL,
		 ags_cclosure_marshal_POINTER__UINT,
		 G_TYPE_POINTER, 1,
		 G_TYPE_UINT);

  /**
   * AgsMidiParser::quarter-frame:
   * @midi_parser: the #AgsMidiParser
   * @status: the MIDI status byte
   *
   * The ::quarter-frame signal is emited during parsing of event.
   *
   * Returns: The XML node representing the event
   *
   * Since: 6.16.18
   */
  midi_parser_signals[QUARTER_FRAME] =
    g_signal_new("quarter-frame",
		 G_TYPE_FROM_CLASS(midi_parser),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsMidiParserClass, quarter_frame),
		 NULL, NULL,
		 ags_cclosure_marshal_POINTER__UINT,
		 G_TYPE_POINTER, 1,
		 G_TYPE_UINT);

  /**
   * AgsMidiParser::song-position:
   * @midi_parser: the #AgsMidiParser
   * @status: the MIDI status byte
   *
   * The ::song-position signal is emited during parsing of event.
   *
   * Returns: The XML node representing the event
   *
   * Since: 6.16.18
   */
  midi_parser_signals[SONG_POSITION] =
    g_signal_new("song-position",
		 G_TYPE_FROM_CLASS(midi_parser),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsMidiParserClass, song_position),
		 NULL, NULL,
		 ags_cclosure_marshal_POINTER__UINT,
		 G_TYPE_POINTER, 1,
		 G_TYPE_UINT);

  /**
   * AgsMidiParser::song-select:
   * @midi_parser: the #AgsMidiParser
   * @status: the MIDI status byte
   *
   * The ::song-select signal is emited during parsing of event.
   *
   * Returns: The XML node representing the event
   *
   * Since: 6.16.18
   */
  midi_parser_signals[SONG_SELECT] =
    g_signal_new("song-select",
		 G_TYPE_FROM_CLASS(midi_parser),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsMidiParserClass, song_select),
		 NULL, NULL,
		 ags_cclosure_marshal_POINTER__UINT,
		 G_TYPE_POINTER, 1,
		 G_TYPE_UINT);

  /**
   * AgsMidiParser::tune-request:
   * @midi_parser: the #AgsMidiParser
   * @status: the MIDI status byte
   *
   * The ::tune-request signal is emited during parsing of event.
   *
   * Returns: The XML node representing the event
   *
   * Since: 6.16.18
   */
  midi_parser_signals[TUNE_REQUEST] =
    g_signal_new("tune-request",
		 G_TYPE_FROM_CLASS(midi_parser),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsMidiParserClass, tune_request),
		 NULL, NULL,
		 ags_cclosure_marshal_POINTER__UINT,
		 G_TYPE_POINTER, 1,
		 G_TYPE_UINT);
}

void
ags_midi_parser_init(AgsMidiParser *midi_parser)
{
  midi_parser->flags = 0;
  
  /* midi parser mutex */
  g_rec_mutex_init(&(midi_parser->obj_mutex));

  midi_parser->file = NULL;
  midi_parser->nth_chunk = 0;

  midi_parser->file_length = 0;  
  midi_parser->offset = 0;

  midi_parser->current_smf_offset = 0;
  midi_parser->current_smf_length = 0;
  
  midi_parser->current_smf_time = 0;
  midi_parser->current_smf_status = 0x0;

  midi_parser->doc = NULL;

  midi_parser->midi_util = ags_midi_util_alloc();
  midi_parser->midi_smf_util = ags_midi_smf_util_alloc();
}

void
ags_midi_parser_set_property(GObject *gobject,
			     guint prop_id,
			     const GValue *value,
			     GParamSpec *param_spec)
{
  AgsMidiParser *midi_parser;

  GRecMutex *midi_parser_mutex;

  midi_parser = AGS_MIDI_PARSER(gobject);

  /* get midi parser mutex */
  midi_parser_mutex = AGS_MIDI_PARSER_GET_OBJ_MUTEX(midi_parser);
  
  switch(prop_id){
  case PROP_FILE:
    {
      FILE *f;

      struct stat sb;

      size_t file_length;

      f = g_value_get_pointer(value);
      
      g_rec_mutex_lock(midi_parser_mutex);
      
      midi_parser->file = f;

      g_rec_mutex_unlock(midi_parser_mutex);

      /* read file */
      if(f != NULL){
	fstat(fileno(f), &sb);
      
	file_length = sb.st_size;

	midi_parser->buffer = (guchar *) malloc(file_length * sizeof(guchar));

	fread(midi_parser->buffer, sizeof(guchar), file_length, f);

	g_rec_mutex_lock(midi_parser_mutex);
      
	midi_parser->file_length = file_length;

	g_rec_mutex_unlock(midi_parser_mutex);
      }
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_midi_parser_get_property(GObject *gobject,
			     guint prop_id,
			     GValue *value,
			     GParamSpec *param_spec)
{
  AgsMidiParser *midi_parser;

  GRecMutex *midi_parser_mutex;

  midi_parser = AGS_MIDI_PARSER(gobject);

  /* get midi parser mutex */
  midi_parser_mutex = AGS_MIDI_PARSER_GET_OBJ_MUTEX(midi_parser);
  
  switch(prop_id){
  case PROP_FILE:
    {
      g_rec_mutex_lock(midi_parser_mutex);

      g_value_set_pointer(value,
			  midi_parser->file);

      g_rec_mutex_unlock(midi_parser_mutex);
    }
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_midi_parser_finalize(GObject *gobject)
{
  AgsMidiParser *midi_parser;
    
  midi_parser = (AgsMidiParser *) gobject;

  /* call parent */
  G_OBJECT_CLASS(ags_midi_parser_parent_class)->finalize(gobject);
}

/**
 * ags_midi_parser_read_gint16:
 * @midi_parser: the #AgsMidiParser
 * 
 * Read gint16.
 * 
 * Returns: the read gint16
 * 
 * Since: 3.0.0
 */
gint16
ags_midi_parser_read_gint16(AgsMidiParser *midi_parser)
{
  char str[2];
  gint16 value = 0;

  str[0] = (char) 0xff & ags_midi_parser_midi_getc(midi_parser);
  str[1] = (char) 0xff & ags_midi_parser_midi_getc(midi_parser);

  value = (str[0] & 0xff);
  value = (value << 8) + (str[1] & 0xff);
  
  return(value);
}

/**
 * ags_midi_parser_read_gint24:
 * @midi_parser: the #AgsMidiParser
 * 
 * Read gint32.
 * 
 * Returns: the read gint32
 * 
 * Since: 3.0.0
 */
gint32
ags_midi_parser_read_gint24(AgsMidiParser *midi_parser)
{
  char str[4];
  gint32 value = 0;
  
  str[0] = (char) 0x00;
  str[1] = (char) 0xff & ags_midi_parser_midi_getc(midi_parser);
  str[2] = (char) 0xff & ags_midi_parser_midi_getc(midi_parser);
  str[3] = (char) 0xff & ags_midi_parser_midi_getc(midi_parser);

  value = (value << 8) + (str[1] & 0xff);
  value = (value << 8) + (str[2] & 0xff);
  value = (value << 8) + (str[3] & 0xff);
  
  return(value);
}

/**
 * ags_midi_parser_read_gint32:
 * @midi_parser: the #AgsMidiParser
 * 
 * Read gint32.
 * 
 * Returns: the read gint32
 * 
 * Since: 3.0.0
 */
gint32
ags_midi_parser_read_gint32(AgsMidiParser *midi_parser)
{
  char str[4];
  gint32 value;
  
  str[0] = (char) 0xff & ags_midi_parser_midi_getc(midi_parser);
  str[1] = (char) 0xff & ags_midi_parser_midi_getc(midi_parser);
  str[2] = (char) 0xff & ags_midi_parser_midi_getc(midi_parser);
  str[3] = (char) 0xff & ags_midi_parser_midi_getc(midi_parser);

  value = (str[0] & 0xff);
  value = (value << 8) + (str[1] & 0xff);
  value = (value << 8) + (str[2] & 0xff);
  value = (value << 8) + (str[3] & 0xff);
  
  return(value);
}

/**
 * ags_midi_parser_read_varlength:
 * @midi_parser: the #AgsMidiParser
 * 
 * Read varlength as long.
 * 
 * Returns: the read varlength
 * 
 * Since: 3.0.0
 */
gint
ags_midi_parser_read_varlength(AgsMidiParser *midi_parser)
{
  gint value;
  guint i;
  guchar c;

  c = (guchar) ags_midi_parser_midi_getc(midi_parser);
  value = c;
  i = 1;
  
  if(0x80 & c){
    value &= 0x7F;
   
    do{
      c = ags_midi_parser_midi_getc(midi_parser);
      value = (value << 7) + (0x7F & (c));
      i++;
    }while(0x80 & c);
  }

  return(value);
}

/**
 * ags_midi_parser_read_text:
 * @midi_parser: the #AgsMidiParser
 * @length: the length
 * 
 * Read text.
 * 
 * Returns: the text read as string
 * 
 * Since: 3.0.0
 */
gchar*
ags_midi_parser_read_text(AgsMidiParser *midi_parser,
			  gint length)
{
  gchar *text;

  gchar c;
  guint i;

  text = (gchar *) g_malloc(AGS_MIDI_PARSER_MAX_TEXT_LENGTH * sizeof(gchar));

  memset(text, 0, AGS_MIDI_PARSER_MAX_TEXT_LENGTH * sizeof(char));
  
  i = 0;
  c = '\0';
  
  if(length > 0){
    while((i < length) &&
	  (AGS_MIDI_PARSER_EOF & (midi_parser->flags)) == 0 &&
	  i < AGS_MIDI_PARSER_MAX_TEXT_LENGTH - 1){
      c = (gchar) (0xff & (ags_midi_parser_midi_getc(midi_parser)));
      
      text[i] = c;
      i++;

      if(c == '\0'){
	break;
      }
    }
  }

  //  c = (gchar) (0xff & (ags_midi_parser_midi_getc(midi_parser)));

  if(c != '\0'){
    g_critical("expected nul byte");
  }

  if(i != length){
    g_critical("text length mismatch");
  }
  
  //  if(i > 0){
    //    midi_parser->offset -= 1;
  //  }
  
  return(text);
}

/**
 * ags_midi_parser_ticks_to_sec:
 * @midi_parser: the #AgsMidiParser
 * @ticks: the ticks count
 * @division: the division
 * @tempo: the tempo
 * 
 * Convert ticks to seconds.
 * 
 * Returns: the seconds read from ticks
 * 
 * Since: 3.0.0
 */
gdouble
ags_midi_parser_ticks_to_sec(AgsMidiParser *midi_parser,
			     guint ticks, gint division, guint tempo)
{
  gdouble retval;
  
  if(division > 0){
    retval = ((gdouble) ticks * tempo) /
	      (gdouble) (division * 1000000.0);
    return(retval);
  }else{
    gdouble smtpe_format, smtpe_resolution;

    smtpe_format = (gdouble) ((0xff00 & division) >> 8);
    smtpe_resolution = (gdouble) (0xff & division);

    retval = (((gdouble) ticks) /
	      (gdouble) (smtpe_format * smtpe_resolution * 1000000.0));
    
    return(retval);
  }
}

int
ags_midi_parser_real_midi_getc(AgsMidiParser *midi_parser)
{
  int c;

  if(midi_parser->buffer == NULL){
    return(0);
  }
  
  if(midi_parser->offset + 1 >= midi_parser->file_length){
    midi_parser->flags |= AGS_MIDI_PARSER_EOF;

    return(0);
  }

  c = (int) (0xff & (midi_parser->buffer[midi_parser->offset]));
  midi_parser->offset += 1;
  midi_parser->current_smf_offset += 1;

  return(c);
}

/**
 * ags_midi_parser_midi_getc:
 * @midi_parser: the #AgsMidiParser
 * 
 * Read one byte.
 * 
 * Returns: the byte read
 * 
 * Since: 3.0.0
 */
int
ags_midi_parser_midi_getc(AgsMidiParser *midi_parser)
{
  int c;
  
  g_return_val_if_fail(AGS_IS_MIDI_PARSER(midi_parser), '\0');
  
  g_object_ref((GObject *) midi_parser);
  g_signal_emit(G_OBJECT(midi_parser),
		midi_parser_signals[MIDI_GETC], 0,
		&c);
  g_object_unref((GObject *) midi_parser);

  return(c);
}

void
ags_midi_parser_real_on_error(AgsMidiParser *midi_parser,
			      GError **error)
{
  //TODO:JK: implement me
}

/**
 * ags_midi_parser_on_error:
 * @midi_parser: the #AgsMidiParser
 * @error: the return location of #GError-struct
 * 
 * Error reporting.
 * 
 * Since: 3.0.0
 */
void
ags_midi_parser_on_error(AgsMidiParser *midi_parser,
			 GError **error)
{
  g_return_if_fail(AGS_IS_MIDI_PARSER(midi_parser));
  
  g_object_ref((GObject *) midi_parser);
  g_signal_emit(G_OBJECT(midi_parser),
		midi_parser_signals[ON_ERROR], 0,
		error);
  g_object_unref((GObject *) midi_parser);
}

xmlDoc*
ags_midi_parser_real_parse_full(AgsMidiParser *midi_parser)
{
  AgsMidiUtil *midi_util;
  AgsMidiSmfUtil *midi_smf_util;
  
  xmlDoc *doc;
  xmlNode *root_node;
  xmlNode *tracks_node;
  xmlNode *current;
  xmlNode *current_message;

  guchar *buffer;
  gchar *str;
  
  size_t file_length;
  size_t offset;
  size_t current_smf_length;
  size_t current_smf_offset;
  guint delta_time_varlength_size;
  guint status;
  guint i;
  guint retval;
  gboolean end_of_track;
  gboolean success;
  
  GRecMutex *midi_parser_mutex;

  /* get midi parser mutex */
  midi_parser_mutex = AGS_MIDI_PARSER_GET_OBJ_MUTEX(midi_parser);

  g_rec_mutex_lock(midi_parser_mutex);
  
  midi_util = midi_parser->midi_util;
  
  midi_smf_util = midi_parser->midi_smf_util;

  doc = NULL;

  root_node = NULL;
  tracks_node = NULL;
  current = NULL;
  current_message = NULL;

  buffer = midi_parser->buffer;
  
  file_length = midi_parser->file_length;

  offset = 
    midi_parser->offset = 0;

  current_smf_length = 0;
  
  current_smf_offset =
    midi_parser->current_smf_offset = 0;
  
  g_rec_mutex_unlock(midi_parser_mutex);
  
  /* create xmlDoc and set root node */
  midi_parser->doc = 
    doc = xmlNewDoc("1.0");
  
  root_node = xmlNewNode(NULL, "midi");
  xmlDocSetRootElement(doc,
		       root_node);

  /* parse header */
  if(file_length >= 14){
    static guchar header[] = "MThd";

    gint chunk_length;
    gint format;
    gint track_count;
    gint division;
    gint beat, clicks;

#ifdef AGS_DEBUG
    g_message("parse header");
#endif

    success = TRUE;
    
    for(i = 0; i < 4; i++){
      if(header[i] != buffer[i]){
	success = FALSE;

	break;
      }
    }

    if(!success){
      g_warning("failed to parse SMF header");
      
      return(doc);
    }
    
    retval = ags_midi_smf_util_get_header(midi_smf_util,
					  buffer,
					  &chunk_length, &format,
					  &track_count, &division);

    if(retval == 0){
      g_warning("failed to parse SMF header");
      
      return(doc);
    }

    /* emit event */
    current = ags_midi_parser_parse_header(midi_parser);
    
    xmlAddChild(root_node,
		current);
  }

  /* create tracks node */
  if(file_length > 22){  
    gint chunk_length;
    gint delta_time;
    
    static guchar track[] = "MTrk";

    tracks_node = xmlNewNode(NULL, "midi-tracks");

    xmlAddChild(root_node,
		tracks_node);
    
    /* parse tracks */
    while(((AGS_MIDI_PARSER_EOF & (midi_parser->flags))) == 0 && midi_parser->offset < midi_parser->file_length){
#ifdef AGS_DEBUG
      g_message("parse track");
#endif

      offset = midi_parser->offset;

      success = TRUE;
    
      for(i = 0; i < 4; i++){
	if(track[i] != buffer[offset + i]){
	  success = FALSE;

	  break;
	}
      }

      if(!success){
	g_warning("failed to parse SMF track");
      
	return(doc);
      }
      
      midi_parser->current_smf_time = 0;

      retval = ags_midi_smf_util_get_track(midi_smf_util,
					   buffer + offset,
					   &chunk_length);

      current = ags_midi_parser_parse_track(midi_parser);    

      xmlAddChild(tracks_node,
		  current);

      current_smf_length = 
	midi_parser->current_smf_length = chunk_length;
      
      current_smf_offset =
	midi_parser->current_smf_offset = 0;

      end_of_track = FALSE;
      
      for(; midi_parser->offset < midi_parser->file_length && !end_of_track /* && midi_parser->current_smf_offset < midi_parser->current_smf_length */; ){
	offset = midi_parser->offset;
	current_smf_offset = midi_parser->current_smf_offset;
	
	success = FALSE;

	/* status and channel messages */
	delta_time = 0;
	
	retval = ags_midi_smf_util_get_varlength(midi_smf_util,
						 buffer + offset,
						 &delta_time);

	midi_parser->current_smf_time += (guint) delta_time;

	delta_time_varlength_size = retval;

	if(offset + retval >= midi_parser->file_length){
	  g_warning("premature end of SMF file");
	  
	  break;
	}

	//FIXME:JK: unsure to remove
#if 0
	if(current_smf_offset + retval >= current_smf_length){
	  g_warning("premature end of SMF track");
	  
	  break;
	}
#endif
	
	if(!success &&
	   offset + retval + 1 < midi_parser->file_length /* &&
							     current_smf_offset + retval + 1 < current_smf_length */){
	  if(ags_midi_util_is_tune_request(midi_util,
					   buffer + offset + delta_time_varlength_size)){
	    status = midi_parser->buffer[offset + delta_time_varlength_size];

	    current_message = ags_midi_parser_tune_request(midi_parser, status);

	    xmlAddChild(current,
			current_message);
	    
	    midi_parser->current_node = current_message;

	    success = TRUE;
	  }else if(ags_midi_util_is_undefined(midi_util,
					      buffer + offset + delta_time_varlength_size)){
	    /* silently skip bytes */
	    ags_midi_parser_read_varlength(midi_parser); // delta-time
  
	    ags_midi_parser_midi_getc(midi_parser); // status
	  }
	}

	if(!success &&
	   offset + retval + 2 < midi_parser->file_length /* &&
							     current_smf_offset + retval + 2 < current_smf_length */){
	  if(ags_midi_util_is_change_program(midi_util,
					     buffer + offset + delta_time_varlength_size)){
	    gint channel;
	    gint program;
	    
	    channel = 0;
	    program = 0;
	    
	    retval = ags_midi_smf_util_get_change_program(midi_smf_util,
							  buffer + offset,
							  NULL,
							  &channel,
							  &program);
	    
	    status = midi_parser->buffer[offset + delta_time_varlength_size];
	    
	    current_message = ags_midi_parser_change_program(midi_parser,
							     status);
	    
	    xmlAddChild(current,
			current_message);
	    
	    midi_parser->current_node = current_message;

	    success = TRUE;
	  }else if(ags_midi_util_is_change_pressure(midi_util,
						    buffer + offset + delta_time_varlength_size)){
	    gint channel;
	    gint pressure;
	    
	    channel = 0;
	    pressure = 0;
	    
	    retval = ags_midi_smf_util_get_change_pressure(midi_smf_util,
							   buffer + offset,
							   NULL,
							   &channel,
							   &pressure);
	    
	    status = midi_parser->buffer[offset + delta_time_varlength_size];
	    
	    current_message = ags_midi_parser_change_channel_pressure(midi_parser,
								      status);
	    
	    xmlAddChild(current,
			current_message);
	    
	    midi_parser->current_node = current_message;

	    success = TRUE;
	  }else if(ags_midi_util_is_quarter_frame(midi_util,
						  buffer + offset + delta_time_varlength_size)){
	    gint message_type;
	    gint values;

	    message_type = 0;
	    values = 0;

	    ags_midi_smf_util_get_quarter_frame(midi_smf_util,
						buffer + offset,
						NULL,
						&message_type, &values);

	    status = midi_parser->buffer[offset + delta_time_varlength_size];

	    current_message = ags_midi_parser_quarter_frame(midi_parser, status);
	    
	    xmlAddChild(current,
			current_message);
	    
	    midi_parser->current_node = current_message;

	    success = TRUE;
	  }else if(ags_midi_util_is_song_select(midi_util,
						buffer + offset + delta_time_varlength_size)){
	    gint song_select;

	    song_select = 0;

	    ags_midi_smf_util_get_song_select(midi_smf_util,
					      buffer + offset,
					      NULL,
					      &song_select);

	    status = midi_parser->buffer[offset + delta_time_varlength_size];

	    current_message = ags_midi_parser_song_select(midi_parser, status);
	    
	    xmlAddChild(current,
			current_message);
	    
	    midi_parser->current_node = current_message;

	    success = TRUE;
	  }
	}

	if(!success &&
	   offset + retval + 3 < midi_parser->file_length /* &&
							     current_smf_offset + retval + 3 < current_smf_length */ ){
	  if(ags_midi_util_is_key_on(midi_util,
				     buffer + offset + delta_time_varlength_size)){
	    gint channel;
	    gint key;
	    gint velocity;
	    
	    channel = 0;
	    key = 0;
	    velocity = 0;
	    
	    retval = ags_midi_smf_util_get_key_on(midi_smf_util,
						  buffer + offset,
						  NULL,
						  &channel,
						  &key,
						  &velocity);

	    status = midi_parser->buffer[offset + delta_time_varlength_size];
	    
	    current_message = ags_midi_parser_key_on(midi_parser,
						     status);
	    
	    xmlAddChild(current,
			current_message);
	    
	    midi_parser->current_node = current_message;

	    success = TRUE;
	  }else if(ags_midi_util_is_key_off(midi_util,
					    buffer + offset + delta_time_varlength_size)){
	    gint channel;
	    gint key;
	    gint velocity;
	    
	    channel = 0;
	    key = 0;
	    velocity = 0;
	    
	    retval = ags_midi_smf_util_get_key_off(midi_smf_util,
						   buffer + offset,
						   NULL,
						   &channel,
						   &key,
						   &velocity);
	    
	    status = midi_parser->buffer[offset + delta_time_varlength_size];
	    
	    current_message = ags_midi_parser_key_off(midi_parser,
						      status);
	    
	    xmlAddChild(current,
			current_message);
	    
	    midi_parser->current_node = current_message;
	    
	    success = TRUE;
	  }else if(ags_midi_util_is_key_pressure(midi_util,
						 buffer + offset + delta_time_varlength_size)){
	    gint channel;
	    gint key;
	    gint pressure;
	    
	    channel = 0;
	    key = 0;
	    pressure = 0;
	    
	    retval = ags_midi_smf_util_get_key_pressure(midi_smf_util,
							buffer + offset,
							NULL,
							&channel,
							&key,
							&pressure);
	    
	    status = midi_parser->buffer[offset + delta_time_varlength_size];
	    
	    current_message = ags_midi_parser_key_pressure(midi_parser,
							   status);
	    
	    xmlAddChild(current,
			current_message);
	    
	    midi_parser->current_node = current_message;

	    success = TRUE;
	  }else if(ags_midi_util_is_change_parameter(midi_util,
						     buffer + offset + delta_time_varlength_size)){
	    gint channel;
	    gint control;
	    gint value;
	    
	    channel = 0;
	    control = 0;
	    value = 0;
	    
	    retval = ags_midi_smf_util_get_change_parameter(midi_smf_util,
							    buffer + offset,
							    NULL,
							    &channel,
							    &control,
							    &value);
	    
	    status = midi_parser->buffer[offset + delta_time_varlength_size];
	    
	    current_message = ags_midi_parser_change_parameter(midi_parser,
							       status);
	    
	    xmlAddChild(current,
			current_message);
	    
	    midi_parser->current_node = current_message;

	    success = TRUE;
	  }else if(ags_midi_util_is_pitch_bend(midi_util,
					       buffer + offset + delta_time_varlength_size)){
	    gint channel;
	    gint pitch;
	    gint transmitter;
	    
	    channel = 0;
	    pitch = 0;
	    transmitter = 0;
	    
	    retval = ags_midi_smf_util_get_pitch_bend(midi_smf_util,
						      buffer + offset,
						      NULL,
						      &channel,
						      &pitch,
						      &transmitter);
	    
	    status = midi_parser->buffer[offset + delta_time_varlength_size];
	    
	    current_message = ags_midi_parser_change_pitch_bend(midi_parser,
								status);
	    
	    xmlAddChild(current,
			current_message);
	    
	    midi_parser->current_node = current_message;

	    success = TRUE;
	  }else if(ags_midi_util_is_song_position(midi_util,
						  buffer + offset + delta_time_varlength_size)){
	    gint song_position;

	    song_position = 0;

	    ags_midi_smf_util_get_song_position(midi_smf_util,
						buffer + offset,
						NULL,
						&song_position);

	    status = midi_parser->buffer[offset + delta_time_varlength_size];

	    current_message = ags_midi_parser_song_position(midi_parser, status);
	    
	    xmlAddChild(current,
			current_message);
	    
	    midi_parser->current_node = current_message;

	    success = TRUE;
	  }else if(ags_midi_util_is_end_of_track(midi_util,
						 buffer + offset + delta_time_varlength_size)){
	    current_message = ags_midi_parser_end_of_track(midi_parser,
							   0x2f);
	    
	    xmlAddChild(current,
			current_message);
	    
	    midi_parser->current_node = current_message;

	    end_of_track = TRUE;
	    
	    success = TRUE;
	  }
	}

	if(!success &&
	   ags_midi_util_is_sysex(midi_util,
				  buffer + offset + delta_time_varlength_size)){
	  status = midi_parser->buffer[offset + delta_time_varlength_size];
	  
	  current_message = ags_midi_parser_sysex(midi_parser,
						  status);

	  xmlAddChild(current,
		      current_message);
	    
	  midi_parser->current_node = current_message;

	  success = TRUE;
	}

	if(!success &&
	   ags_midi_util_is_text_event(midi_util,
				       buffer + offset + delta_time_varlength_size)){
	  gint length;
	  guint retval;

	  length = 0;
	    
	  retval = ags_midi_smf_util_get_text_event(midi_smf_util,
						    buffer + offset,
						    NULL,
						    NULL, &length);

	  if(midi_parser->offset + retval < midi_parser->file_length /* &&
									midi_parser->current_smf_offset + retval < midi_parser->current_smf_length */){
	    current_message = ags_midi_parser_text_event(midi_parser, 0x1);
	      
	    xmlAddChild(current,
			current_message);
	    
	    midi_parser->current_node = current_message;
	      
	    success = TRUE;
	  }else{
	    g_warning("corrupted SMF text event");
	  }
	}

	if(!success &&
	   ags_midi_util_is_copyright_notice(midi_util,
					     buffer + offset + delta_time_varlength_size)){
	  gint length;
	  guint retval;

	  length = 0;
	    
	  retval = ags_midi_smf_util_get_text_event(midi_smf_util,
						    buffer + offset,
						    NULL,
						    NULL, &length);

	  if(midi_parser->offset + retval < midi_parser->file_length /* &&
									midi_parser->current_smf_offset + retval < midi_parser->current_smf_length */){
	    current_message = ags_midi_parser_text_event(midi_parser, 0x2);
	      
	    xmlAddChild(current,
			current_message);
	    
	    midi_parser->current_node = current_message;
	      
	    success = TRUE;
	  }else{
	    g_warning("corrupted SMF copyright notice");
	  }
	}	

	if(!success &&
	   ags_midi_util_is_sequence_name(midi_util,
					  buffer + offset + delta_time_varlength_size)){
	  gint length;
	  guint retval;

	  length = 0;
	    
	  retval = ags_midi_smf_util_get_text_event(midi_smf_util,
						    buffer + offset,
						    NULL,
						    NULL, &length);

	  if(midi_parser->offset + retval < midi_parser->file_length /* &&
									midi_parser->current_smf_offset + retval < midi_parser->current_smf_length */){
	    current_message = ags_midi_parser_text_event(midi_parser, 0x3);
	      
	    xmlAddChild(current,
			current_message);
	    
	    midi_parser->current_node = current_message;
	      
	    success = TRUE;
	  }else{
	    g_warning("corrupted SMF sequence name");
	  }
	}

	if(!success &&
	   ags_midi_util_is_instrument_name(midi_util,
					    buffer + offset + delta_time_varlength_size)){
	  gint length;
	  guint retval;

	  length = 0;
	    
	  retval = ags_midi_smf_util_get_text_event(midi_smf_util,
						    buffer + offset,
						    NULL,
						    NULL, &length);

	  if(midi_parser->offset + retval < midi_parser->file_length /* &&
									midi_parser->current_smf_offset + retval < midi_parser->current_smf_length */){
	    current_message = ags_midi_parser_text_event(midi_parser, 0x4);

	    xmlAddChild(current,
			current_message);	    
	      
	    midi_parser->current_node = current_message;
	      
	    success = TRUE;
	  }else{
	    g_warning("corrupted SMF instrument name");
	  }
	}

	if(!success &&
	   ags_midi_util_is_lyric(midi_util,
				  buffer + offset + delta_time_varlength_size)){
	  gint length;
	  guint retval;

	  length = 0;
	    
	  retval = ags_midi_smf_util_get_text_event(midi_smf_util,
						    buffer + offset,
						    NULL,
						    NULL, &length);

	  if(midi_parser->offset + retval < midi_parser->file_length /* &&
									midi_parser->current_smf_offset + retval < midi_parser->current_smf_length */){
	    current_message = ags_midi_parser_text_event(midi_parser, 0x5);
	      
	    midi_parser->current_node = current_message;
	      
	    xmlAddChild(current,
			current_message);
	    
	    success = TRUE;
	  }else{
	    g_warning("corrupted SMF lyric");
	  }
	}

	if(!success &&
	   ags_midi_util_is_marker(midi_util,
				  buffer + offset + delta_time_varlength_size)){
	  gint length;
	  guint retval;

	  length = 0;
	    
	  retval = ags_midi_smf_util_get_text_event(midi_smf_util,
						    buffer + offset,
						    NULL,
						    NULL, &length);

	  if(midi_parser->offset + retval < midi_parser->file_length /* &&
									midi_parser->current_smf_offset + retval < midi_parser->current_smf_length */){
	    current_message = ags_midi_parser_text_event(midi_parser, 0x6);
	      
	    midi_parser->current_node = current_message;
	      
	    xmlAddChild(current,
			current_message);
	    
	    success = TRUE;
	  }else{
	    g_warning("corrupted SMF marker");
	  }
	}

	if(!success &&
	   ags_midi_util_is_cue_point(midi_util,
				      buffer + offset + delta_time_varlength_size)){
	  gint length;
	  guint retval;

	  length = 0;
	    
	  retval = ags_midi_smf_util_get_text_event(midi_smf_util,
						    buffer + offset,
						    NULL,
						    NULL, &length);

	  if(midi_parser->offset + retval < midi_parser->file_length /* &&
									midi_parser->current_smf_offset + retval < midi_parser->current_smf_length */){
	    current_message = ags_midi_parser_text_event(midi_parser, 0x7);
	      
	    xmlAddChild(current,
			current_message);
	    
	    midi_parser->current_node = current_message;
	      
	    success = TRUE;
	  }else{
	    g_warning("corrupted SMF cue point");
	  }
	}

	if(!success &&
	   ags_midi_util_is_unknown_text_event(midi_util,
					       buffer + offset + delta_time_varlength_size)){
	  gint length;
	  guint retval;

	  length = 0;
	    
	  retval = ags_midi_smf_util_get_text_event(midi_smf_util,
						    buffer + offset,
						    NULL,
						    NULL, &length);

	  if(midi_parser->offset + retval < midi_parser->file_length /* &&
									midi_parser->current_smf_offset + retval < midi_parser->current_smf_length */){
	    current_message = ags_midi_parser_text_event(midi_parser, buffer[offset + delta_time_varlength_size + 1]);
	    
	    xmlAddChild(current,
			current_message);
	    
	    midi_parser->current_node = current_message;
	      
	    success = TRUE;
	  }else{
	    g_warning("corrupted SMF unknown text event");
	  }
	}

	if(!success &&
	   offset + retval + 4 < midi_parser->file_length /* &&
							     current_smf_offset + retval + 4 < current_smf_length */){
	   
	  if(ags_midi_util_is_midi_channel_prefix(midi_util,
						  buffer + offset + delta_time_varlength_size)){
	    current_message = ags_midi_parser_midi_channel_prefix(midi_parser, 0x20);

	    xmlAddChild(current,
			current_message);	    
	    
	    midi_parser->current_node = current_message;
	      
	    success = TRUE;
	  }
	}

	if(!success &&
	   offset + retval + 5 < midi_parser->file_length /* &&
							     current_smf_offset + retval + 5 < current_smf_length */){
	   
	  if(ags_midi_util_is_misc_event(midi_util,
					 buffer + offset + delta_time_varlength_size)){
	    current_message = ags_midi_parser_meta_misc(midi_parser, 0x0);	    

	    xmlAddChild(current,
			current_message);	    
	    
	    midi_parser->current_node = current_message;
	      
	    success = TRUE;
	  }else if(ags_midi_util_is_key_signature(midi_util,
						  buffer + offset + delta_time_varlength_size)){
	    current_message = ags_midi_parser_key_signature(midi_parser, 0x0);	    

	    xmlAddChild(current,
			current_message);	    
	    
	    midi_parser->current_node = current_message;
	      
	    success = TRUE;
	  }
	}

	if(!success &&
	   offset + retval + 6 < midi_parser->file_length /* &&
							     current_smf_offset + retval + 5 < current_smf_length */){
	   
	  if(ags_midi_util_is_tempo(midi_util,
				    buffer + offset + delta_time_varlength_size)){
	    current_message = ags_midi_parser_tempo(midi_parser, 0x51);	    

	    xmlAddChild(current,
			current_message); 
	    
	    midi_parser->current_node = current_message;
	      
	    success = TRUE;
	  }
	}

	if(!success &&
	   offset + retval + 7 < midi_parser->file_length /* &&
							     current_smf_offset + retval + 5 < current_smf_length */){
	   
	  if(ags_midi_util_is_time_signature(midi_util,
					     buffer + offset + delta_time_varlength_size)){
	    current_message = ags_midi_parser_time_signature(midi_parser, 0x58);	    

	    xmlAddChild(current,
			current_message); 
	    
	    midi_parser->current_node = current_message;
	      
	    success = TRUE;
	  }
	}

	if(success){
	  str = g_strdup_printf("%d",
				midi_parser->current_smf_time);
	  
	  xmlNewProp(current_message,
		     "delta-time",
		     str);

	  g_free(str);
	}
	
	if(!success){
	  midi_parser->current_node = NULL;
	  
	  g_warning("bad byte");

	  ags_midi_parser_midi_getc(midi_parser);
	}
      }

    }
  }
  
  return(doc);
}

/**
 * ags_midi_parser_parse_full:
 * @midi_parser: the #AgsMidiParser
 * 
 * Parse all data of #AgsMidiParser:file.
 * 
 * Returns: (transfer none): the xmlDoc representing your MIDI file
 * 
 * Since: 3.0.0
 */
xmlDoc*  
ags_midi_parser_parse_full(AgsMidiParser *midi_parser)
{
  xmlDoc *doc;
  
  g_return_val_if_fail(AGS_IS_MIDI_PARSER(midi_parser), NULL);
  
  g_object_ref((GObject *) midi_parser);
  g_signal_emit(G_OBJECT(midi_parser),
		midi_parser_signals[PARSE_FULL], 0,
		&doc);
  g_object_unref((GObject *) midi_parser);

  return(doc);
}

xmlNode*
ags_midi_parser_parse_bytes(AgsMidiParser *midi_parser,
			    guchar *midi_buffer,
			    guint buffer_length)
{
  xmlNode *node;

  node = NULL;
  
  //TODO:JK: implement me

  return(node);
}

xmlNode*
ags_midi_parser_real_parse_header(AgsMidiParser *midi_parser)
{
  xmlNode *node;

  gchar *str;
  
  gint chunk_length;
  gint format;
  gint track_count;
  gint division;
  gint beat, clicks;
  guint i;
  
  chunk_length = 0;
  format = 0;
  track_count = 0;
  division = 0;

  ags_midi_smf_util_get_header(midi_parser->midi_smf_util,
			       midi_parser->buffer + midi_parser->offset,
			       &chunk_length, &format,
			       &track_count, &division);

  /* midi-header XML node */
  node = xmlNewNode(NULL,
		    "midi-header");

  str = g_strdup_printf("%d",
			chunk_length);
    
  xmlNewProp(node,
	     "offset",
	     str);

  g_free(str);

  str = g_strdup_printf("%d",
			format);
    
  xmlNewProp(node,
	     "format",
	     str);

  g_free(str);

  if(division & 0x8000){
    /* SMTPE */
    str = g_strdup_printf("%d %d",
			  -((-(division>>8)) & 0xff),
			  (division & 0xff));
      
    xmlNewProp(node,
	       "division",
	       str);

    g_free(str);
  }else{
    str = g_strdup_printf("%d",
			  division);
      
    xmlNewProp(node,
	       "division",
	       str);

    g_free(str);
  }

  for(i = 0; i < 14; i++){
    ags_midi_parser_midi_getc(midi_parser);
  }    

  return(node);
}

/**
 * ags_midi_parser_parse_header:
 * @midi_parser: the #AgsMidiParser
 * 
 * Parse MIDI header
 * 
 * Returns: (transfer none): the xmlNode containing parsed data
 * 
 * Since: 3.0.0
 */
xmlNode*  
ags_midi_parser_parse_header(AgsMidiParser *midi_parser)
{
  xmlNode *node;
  
  g_return_val_if_fail(AGS_IS_MIDI_PARSER(midi_parser), NULL);
  
  g_object_ref((GObject *) midi_parser);
  g_signal_emit(G_OBJECT(midi_parser),
		midi_parser_signals[PARSE_HEADER], 0,
		&node);
  g_object_unref((GObject *) midi_parser);

  return(node);
}

xmlNode*
ags_midi_parser_real_parse_track(AgsMidiParser *midi_parser)
{
  xmlNode *node;

  gchar *str;
  
  gint chunk_length;
  guint i;
  
  chunk_length = 0;
  
  ags_midi_smf_util_get_track(midi_parser->midi_smf_util,
			      midi_parser->buffer + midi_parser->offset,
			      &chunk_length);

  node = xmlNewNode(NULL,
		    "midi-track");

  str = g_strdup_printf("%d",
			chunk_length);
    
  xmlNewProp(node,
	     "offset",
	     str);

  g_free(str);

  for(i = 0; i < 8; i++){
    ags_midi_parser_midi_getc(midi_parser);
  }    
  
  return(node);
}

/**
 * ags_midi_parser_parse_track:
 * @midi_parser: the #AgsMidiParser
 * 
 * Parse MIDI track.
 * 
 * Returns: (transfer none): the xmlNode containing parsed data
 * 
 * Since: 3.0.0
 */
xmlNode*  
ags_midi_parser_parse_track(AgsMidiParser *midi_parser)
{
  xmlNode *node;
  
  g_return_val_if_fail(AGS_IS_MIDI_PARSER(midi_parser), NULL);
  
  g_object_ref((GObject *) midi_parser);
  g_signal_emit(G_OBJECT(midi_parser),
		midi_parser_signals[PARSE_TRACK], 0,
		&node);
  g_object_unref((GObject *) midi_parser);

  return(node);
}

/**
 * ags_midi_parser_channel_message:
 * @midi_parser: the #AgsMidiParser
 * @status: the status byte
 * 
 * Parse channel message.
 * 
 * Returns: (transfer none): the xmlNode containing parsed data
 * 
 * Since: 3.0.0
 */
xmlNode*
ags_midi_parser_channel_message(AgsMidiParser *midi_parser, guint status)
{
  xmlNode *node;

#ifdef AGS_DEBUG
  g_message("channel message");
#endif

  node = midi_parser->current_node;  
  
  return(node);
}

xmlNode*  
ags_midi_parser_real_key_on(AgsMidiParser *midi_parser, guint status)
{
  xmlNode *node;
  int channel, note, velocity;

#ifdef AGS_DEBUG
  g_message("key on");
#endif

  ags_midi_parser_read_varlength(midi_parser); // delta-time
  
  ags_midi_parser_midi_getc(midi_parser); // status

  channel = 0xf & status;
  note = (0x7f) & ags_midi_parser_midi_getc(midi_parser);
  velocity = (0x7f) & ags_midi_parser_midi_getc(midi_parser);

  node = xmlNewNode(NULL,
		    "midi-message");

  xmlNewProp(node,
	     AGS_MIDI_EVENT,
	     "note-on");

  xmlNewProp(node,
	     "key",
	     g_strdup_printf("%d", channel));

  xmlNewProp(node,
	     "note",
	     g_strdup_printf("%d", note));

  xmlNewProp(node,
	     "velocity",
	     g_strdup_printf("%d", velocity));
  
  return(node);
}

/**
 * ags_midi_parser_key_on:
 * @midi_parser: the #AgsMidiParser
 * @status: the status byte
 * 
 * Parse key on.
 * 
 * Returns: (transfer none): the xmlNode containing parsed data
 * 
 * Since: 3.0.0
 */
xmlNode*  
ags_midi_parser_key_on(AgsMidiParser *midi_parser, guint status)
{
  xmlNode *node;
  
  g_return_val_if_fail(AGS_IS_MIDI_PARSER(midi_parser), NULL);
  
  g_object_ref((GObject *) midi_parser);
  g_signal_emit(G_OBJECT(midi_parser),
		midi_parser_signals[KEY_ON], 0,
		status,
		&node);
  g_object_unref((GObject *) midi_parser);

  return(node);
}

xmlNode*  
ags_midi_parser_real_key_off(AgsMidiParser *midi_parser, guint status)
{
  xmlNode *node;
  int channel, note, velocity;

#ifdef AGS_DEBUG
  g_message("key off");
#endif

  ags_midi_parser_read_varlength(midi_parser); // delta-time
  
  ags_midi_parser_midi_getc(midi_parser); // status

  channel = 0xf & status;
  note = (0x7f) & ags_midi_parser_midi_getc(midi_parser);
  velocity = (0x7f) & ags_midi_parser_midi_getc(midi_parser);

  node = xmlNewNode(NULL,
		    "midi-message");
  
  xmlNewProp(node,
	     AGS_MIDI_EVENT,
	     "note-off");

  xmlNewProp(node,
	     "key",
	     g_strdup_printf("%d", channel));

  xmlNewProp(node,
	     "note",
	     g_strdup_printf("%d", note));

  xmlNewProp(node,
	     "velocity",
	     g_strdup_printf("%d", velocity));
  
  return(node);
}

/**
 * ags_midi_parser_key_off:
 * @midi_parser: the #AgsMidiParser
 * @status: the status byte
 * 
 * Parse key off.
 * 
 * Returns: (transfer none): the xmlNode containing parsed data
 * 
 * Since: 3.0.0
 */
xmlNode*  
ags_midi_parser_key_off(AgsMidiParser *midi_parser, guint status)
{
  xmlNode *node;
  
  g_return_val_if_fail(AGS_IS_MIDI_PARSER(midi_parser), NULL);
  
  g_object_ref((GObject *) midi_parser);
  g_signal_emit(G_OBJECT(midi_parser),
		midi_parser_signals[KEY_OFF], 0,
		status,
		&node);
  g_object_unref((GObject *) midi_parser);

  return(node);
}

xmlNode*  
ags_midi_parser_real_key_pressure(AgsMidiParser *midi_parser, guint status)
{
  xmlNode *node;
  int channel, note, pressure;

#ifdef AGS_DEBUG
  g_message("key pressure");
#endif

  ags_midi_parser_read_varlength(midi_parser); // delta-time
  
  ags_midi_parser_midi_getc(midi_parser); // status

  channel = 0xf & status;
  note = 0x7f & ags_midi_parser_midi_getc(midi_parser);
  pressure = 0x7f & ags_midi_parser_midi_getc(midi_parser);
  
  node = xmlNewNode(NULL,
		    "midi-message");

  xmlNewProp(node,
	     AGS_MIDI_EVENT,
	     "polyphonic");
	
  xmlNewProp(node,
	     "key",
	     g_strdup_printf("%d", channel));
  
  xmlNewProp(node,
	     "note",
	     g_strdup_printf("%d", note));

  xmlNewProp(node,
	     "pressure",
	     g_strdup_printf("%d", pressure));

  return(node);
}

/**
 * ags_midi_parser_key_pressure:
 * @midi_parser: the #AgsMidiParser
 * @status: the status byte
 * 
 * Parse key pressure.
 * 
 * Returns: (transfer none): the xmlNode containing parsed data
 * 
 * Since: 3.0.0
 */
xmlNode*
ags_midi_parser_key_pressure(AgsMidiParser *midi_parser, guint status)
{
  xmlNode *node;
  
  g_return_val_if_fail(AGS_IS_MIDI_PARSER(midi_parser), NULL);
  
  g_object_ref((GObject *) midi_parser);
  g_signal_emit(G_OBJECT(midi_parser),
		midi_parser_signals[KEY_PRESSURE], 0,
		status,
		&node);
  g_object_unref((GObject *) midi_parser);

  return(node);
}

xmlNode*  
ags_midi_parser_real_change_parameter(AgsMidiParser *midi_parser, guint status)
{
  xmlNode *node;
  int channel, control, value;

#ifdef AGS_DEBUG
  g_message("change parameter");
#endif

  ags_midi_parser_read_varlength(midi_parser); // delta-time
  
  ags_midi_parser_midi_getc(midi_parser); // status

  channel = 0xf & status;
  control = 0x7f & ags_midi_parser_midi_getc(midi_parser);
  value = 0x7f & ags_midi_parser_midi_getc(midi_parser);

  node = xmlNewNode(NULL,
		    "midi-message");

  if(control < 120){
    xmlNewProp(node,
	       AGS_MIDI_EVENT,
	       "change-parameter");

    xmlNewProp(node,
	       "channel",
	       g_strdup_printf("%d", channel));

    xmlNewProp(node,
	       "control",
	       g_strdup_printf("%d", control));

    xmlNewProp(node,
	       "value",
	       g_strdup_printf("%d", value));
  }else{
    xmlNewProp(node,
	       AGS_MIDI_EVENT,
	       "change-mode");

    xmlNewProp(node,
	       "channel",
	       g_strdup_printf("%d", channel));

    switch(control){
    case 120:
      {
	xmlNewProp(node,
		   "mode",
		   "all-sound-off");
      }
      break;
    case 121:
      {
	xmlNewProp(node,
		   "mode",
		   "reset-control-all");
	    
	xmlNewProp(node,
		   "value",
		   g_strdup_printf("%d", value));
      }
      break;
    case 122:
      {
	if(value == 0){
	  xmlNewProp(node,
		     "mode",
		     "local-control-off");
	}else{
	  xmlNewProp(node,
		     "mode",
		     "local-control-on");
	}
      }
      break;
    case 123:
      {
	xmlNewProp(node,
		   "mode",
		   "all-notes-off");
      }
      break;
    case 124:
      {
	xmlNewProp(node,
		   "mode",
		   "omni-mode-on");
      }
      break;
    case 125:
      {
	xmlNewProp(node,
		   "mode",
		   "omni-mode-off");
      }
      break;
    case 126:
      {
	xmlNewProp(node,
		   "mode",
		   "mono-mode-on");

	xmlNewProp(node,
		   "omni-off-channels",
		   g_strdup_printf("%d", value));
      }
      break;
    case 127:
      {
	xmlNewProp(node,
		   "mode",
		   "poly-mode-on");
      }
      break;
    }
  }
  
  return(node);
}

/**
 * ags_midi_parser_change_parameter:
 * @midi_parser: the #AgsMidiParser
 * @status: the status byte
 * 
 * Parse change parameter.
 * 
 * Returns: (transfer none): the xmlNode containing parsed data
 * 
 * Since: 3.0.0
 */
xmlNode*  
ags_midi_parser_change_parameter(AgsMidiParser *midi_parser, guint status)
{
  xmlNode *node;
  
  g_return_val_if_fail(AGS_IS_MIDI_PARSER(midi_parser), NULL);
  
  g_object_ref((GObject *) midi_parser);
  g_signal_emit(G_OBJECT(midi_parser),
		midi_parser_signals[CHANGE_PARAMETER], 0,
		status,
		&node);
  g_object_unref((GObject *) midi_parser);

  return(node);
}

xmlNode*  
ags_midi_parser_real_change_pitch_bend(AgsMidiParser *midi_parser, guint status)
{
  xmlNode *node;

  gchar *str;
  
  int channel, pitch, transmitter;

#ifdef AGS_DEBUG
  g_message("change pitch bend");
#endif

  ags_midi_parser_read_varlength(midi_parser); // delta-time
  
  ags_midi_parser_midi_getc(midi_parser); // status

  channel = 0xf & status;
  pitch = 0x7f & ags_midi_parser_midi_getc(midi_parser);
  transmitter = 0x7f & ags_midi_parser_midi_getc(midi_parser);
  
  node = xmlNewNode(NULL,
		    "midi-message");

  str = g_strdup_printf("%d",
			channel);
  
  xmlNewProp(node,
	     "channel",
	     str);

  g_free(str);

  xmlNewProp(node,
	     AGS_MIDI_EVENT,
	     "pitch-bend");

  str = g_strdup_printf("%d",
			pitch);
  
  xmlNewProp(node,
	     "pitch",
	     str);	  
  
  g_free(str);

  str = g_strdup_printf("%d",
			transmitter);
  
  xmlNewProp(node,
	     "transmitter",
	     str);	  
  
  g_free(str);

  return(node);
}

/**
 * ags_midi_parser_change_pitch_bend:
 * @midi_parser: the #AgsMidiParser
 * @status: the status byte
 * 
 * Parse change pitch bend.
 * 
 * Returns: (transfer none): the xmlNode containing parsed data
 * 
 * Since: 3.0.0
 */
xmlNode*  
ags_midi_parser_change_pitch_bend(AgsMidiParser *midi_parser, guint status)
{
  xmlNode *node;
  
  g_return_val_if_fail(AGS_IS_MIDI_PARSER(midi_parser), NULL);
  
  g_object_ref((GObject *) midi_parser);
  g_signal_emit(G_OBJECT(midi_parser),
		midi_parser_signals[CHANGE_PITCH_BEND], 0,
		status,
		&node);
  g_object_unref((GObject *) midi_parser);

  return(node);
}

xmlNode*  
ags_midi_parser_real_change_program(AgsMidiParser *midi_parser, guint status)
{
  xmlNode *node;

  gchar *str;
  
  int channel;
  int program;

#ifdef AGS_DEBUG
  g_message("change program");
#endif

  ags_midi_parser_read_varlength(midi_parser); // delta-time
  
  ags_midi_parser_midi_getc(midi_parser); // status

  channel = 0xf & status;
  program = 0x7f & ags_midi_parser_midi_getc(midi_parser);
  
  node = xmlNewNode(NULL,
		    "midi-message");

  xmlNewProp(node,
	     AGS_MIDI_EVENT,
	     "program-change");

  str = g_strdup_printf("%d",
			channel);
  
  xmlNewProp(node,
	     "channel",
	     str);

  g_free(str);

  str = g_strdup_printf("%d",
			program);
  
  xmlNewProp(node,
	     "program",
	     str);

  g_free(str);
  
  return(node);
}

/**
 * ags_midi_parser_change_program:
 * @midi_parser: the #AgsMidiParser
 * @status: the status byte
 * 
 * Parse change program.
 * 
 * Returns: (transfer none): the xmlNode containing parsed data
 * 
 * Since: 3.0.0
 */
xmlNode*  
ags_midi_parser_change_program(AgsMidiParser *midi_parser, guint status)
{
  xmlNode *node;
  
  g_return_val_if_fail(AGS_IS_MIDI_PARSER(midi_parser), NULL);
  
  g_object_ref((GObject *) midi_parser);
  g_signal_emit(G_OBJECT(midi_parser),
		midi_parser_signals[CHANGE_PROGRAM], 0,
		status,
		&node);
  g_object_unref((GObject *) midi_parser);

  return(node);
}

xmlNode*  
ags_midi_parser_real_change_channel_pressure(AgsMidiParser *midi_parser, guint status)
{
  xmlNode *node;
  int channel, pressure;
  
#ifdef AGS_DEBUG
  g_message("change channel pressure");
#endif

  ags_midi_parser_read_varlength(midi_parser); // delta-time
  
  ags_midi_parser_midi_getc(midi_parser); // status

  channel = 0xf & status;
  pressure = 0x7f & ags_midi_parser_midi_getc(midi_parser);
  
  node = xmlNewNode(NULL,
		    "midi-message");

  xmlNewProp(node,
	     AGS_MIDI_EVENT,
	     "channel-pressure");

  xmlNewProp(node,
	     "channel",
	     g_strdup_printf("%d", channel));

  xmlNewProp(node,
	     "pressure",
	     g_strdup_printf("%d", pressure));
  
  
  return(node);
}

/**
 * ags_midi_parser_change_channel_pressure:
 * @midi_parser: the #AgsMidiParser
 * @status: the status byte
 * 
 * Parse change channel pressure.
 * 
 * Returns: (transfer none): the xmlNode containing parsed data
 * 
 * Since: 3.0.0
 */
xmlNode*  
ags_midi_parser_change_channel_pressure(AgsMidiParser *midi_parser, guint status)
{
  xmlNode *node;
  
  g_return_val_if_fail(AGS_IS_MIDI_PARSER(midi_parser), NULL);
  
  g_object_ref((GObject *) midi_parser);
  g_signal_emit(G_OBJECT(midi_parser),
		midi_parser_signals[CHANGE_CHANNEL_PRESSURE], 0,
		status,
		&node);
  g_object_unref((GObject *) midi_parser);

  return(node);
}

xmlNode*  
ags_midi_parser_real_not_defined(AgsMidiParser *midi_parser, guint status)
{
#ifdef AGS_DEBUG
  g_message("not defined");
#endif

  ags_midi_parser_read_varlength(midi_parser); // delta-time
  
  ags_midi_parser_midi_getc(midi_parser); // status

  return(NULL);
}

/**
 * ags_midi_parser_not_defined:
 * @midi_parser: the #AgsMidiParser
 * @status: the status byte
 * 
 * Parse not defined.
 * 
 * Returns: (transfer none): the xmlNode containing parsed data
 * 
 * Since: 3.0.0
 */
xmlNode*  
ags_midi_parser_not_defined(AgsMidiParser *midi_parser, guint status)
{
  xmlNode *node;
  
  g_return_val_if_fail(AGS_IS_MIDI_PARSER(midi_parser), NULL);
  
  g_object_ref((GObject *) midi_parser);
  g_signal_emit(G_OBJECT(midi_parser),
		midi_parser_signals[NOT_DEFINED], 0,
		status,
		&node);
  g_object_unref((GObject *) midi_parser);

  return(node);
}

xmlNode*
ags_midi_parser_real_sysex(AgsMidiParser *midi_parser, guint status)
{
  xmlNode *node;

  gint c;
  guint i;

  ags_midi_parser_read_varlength(midi_parser); // delta-time
  
  ags_midi_parser_midi_getc(midi_parser); // status

  ags_midi_parser_read_varlength(midi_parser); // sysex length

  node = xmlNewNode(NULL,
		    "midi-message");

  xmlNewProp(node,
	     AGS_MIDI_EVENT,
	     "sysex");

  i = 0;
  
  while((c = ags_midi_parser_midi_getc(midi_parser)) != 0xf7 &&
	(AGS_MIDI_PARSER_EOF & (midi_parser->flags)) == 0){
    i++;
  }
  
#ifdef AGS_DEBUG
  g_message("discarded sysex [%d]", i);
#endif

  return(node);
}

/**
 * ags_midi_parser_sysex:
 * @midi_parser: the #AgsMidiParser
 * @status: the status byte
 * 
 * Parse sysex.
 * 
 * Returns: (transfer none): the xmlNode containing parsed data
 * 
 * Since: 3.0.0
 */
xmlNode*
ags_midi_parser_sysex(AgsMidiParser *midi_parser, guint status)
{
  xmlNode *node;
  
  g_return_val_if_fail(AGS_IS_MIDI_PARSER(midi_parser), NULL);
  
  g_object_ref((GObject *) midi_parser);
  g_signal_emit(G_OBJECT(midi_parser),
		midi_parser_signals[SYSEX], 0,
		status,
		&node);
  g_object_unref((GObject *) midi_parser);

  return(node);
}

xmlNode*
ags_midi_parser_real_system_common(AgsMidiParser *midi_parser, guint status)
{
  xmlNode *node;

#ifdef AGS_DEBUG
  g_message("system common");
#endif

  node = midi_parser->current_node;

  return(node);
}

/**
 * ags_midi_parser_system_common:
 * @midi_parser: the #AgsMidiParser
 * @status: the status byte
 * 
 * Parse system common.
 * 
 * Returns: (transfer none): the xmlNode containing parsed data
 * 
 * Since: 3.0.0
 */
xmlNode*
ags_midi_parser_system_common(AgsMidiParser *midi_parser, guint status)
{
  xmlNode *node;
  
  g_return_val_if_fail(AGS_IS_MIDI_PARSER(midi_parser), NULL);
  
  g_object_ref((GObject *) midi_parser);
  g_signal_emit(G_OBJECT(midi_parser),
		midi_parser_signals[SYSTEM_COMMON], 0,
		status,
		&node);
  g_object_unref((GObject *) midi_parser);

  return(node);
}

xmlNode*  
ags_midi_parser_real_meta_event(AgsMidiParser *midi_parser, guint status)
{
  xmlNode *node;

#ifdef AGS_DEBUG
  g_message("meta event");
#endif

  node = midi_parser->current_node;

  return(node);
}

/**
 * ags_midi_parser_meta_event:
 * @midi_parser: the #AgsMidiParser
 * @status: the status byte
 * 
 * Parse meta event.
 * 
 * Returns: (transfer none): the xmlNode containing parsed data
 * 
 * Since: 3.0.0
 */
xmlNode*  
ags_midi_parser_meta_event(AgsMidiParser *midi_parser, guint status)
{
  xmlNode *node;
  
  g_return_val_if_fail(AGS_IS_MIDI_PARSER(midi_parser), NULL);
  
  g_object_ref((GObject *) midi_parser);
  g_signal_emit(G_OBJECT(midi_parser),
		midi_parser_signals[META_EVENT], 0,
		status,
		&node);
  g_object_unref((GObject *) midi_parser);

  return(node);
}

xmlNode*  
ags_midi_parser_real_sequence_number(AgsMidiParser *midi_parser, guint meta_type)
{
  xmlNode *node;

  gchar *str;
  
  guint sequence;
  
#ifdef AGS_DEBUG
  g_message("sequence number");
#endif

  ags_midi_parser_read_varlength(midi_parser); // delta-time
  
  ags_midi_parser_midi_getc(midi_parser); // status

  ags_midi_parser_midi_getc(midi_parser); // meta type

  node = xmlNewNode(NULL,
		    "midi-message");

  sequence = (guint) ags_midi_parser_read_gint16(midi_parser);
  
  xmlNewProp(node,
	     AGS_MIDI_EVENT,
	     "sequence-number");
  
  xmlNewProp(node,
	     "sequence",
	     g_strdup_printf("%d", sequence));
    
  return(node);
}

/**
 * ags_midi_parser_sequence_number:
 * @midi_parser: the #AgsMidiParser
 * @meta_type: the meta type byte
 * 
 * Parse sequence number.
 * 
 * Returns: (transfer none): the xmlNode containing parsed data
 * 
 * Since: 3.0.0
 */
xmlNode*  
ags_midi_parser_sequence_number(AgsMidiParser *midi_parser, guint meta_type)
{
  xmlNode *node;
  
  g_return_val_if_fail(AGS_IS_MIDI_PARSER(midi_parser), NULL);
  
  g_object_ref((GObject *) midi_parser);
  g_signal_emit(G_OBJECT(midi_parser),
		midi_parser_signals[SEQUENCE_NUMBER], 0,
		meta_type,
		&node);
  g_object_unref((GObject *) midi_parser);

  return(node);
}

xmlNode*  
ags_midi_parser_real_end_of_track(AgsMidiParser *midi_parser, guint meta_type)
{
  xmlNode *node;

  int length;

#ifdef AGS_DEBUG
  g_message("EOT");
#endif

  ags_midi_parser_read_varlength(midi_parser); // delta-time
  
  ags_midi_parser_midi_getc(midi_parser); // status

  ags_midi_parser_midi_getc(midi_parser); // meta type

  node = xmlNewNode(NULL,
		    "midi-message");

  length = ags_midi_parser_midi_getc(midi_parser);

  xmlNewProp(node,
	     AGS_MIDI_EVENT,
	     "end-of-track");
  
  return(node);
}

/**
 * ags_midi_parser_end_of_track:
 * @midi_parser: the #AgsMidiParser
 * @meta_type: the meta type byte
 * 
 * Parse end of track.
 * 
 * Returns: (transfer none): the xmlNode containing parsed data
 * 
 * Since: 3.0.0
 */
xmlNode*  
ags_midi_parser_end_of_track(AgsMidiParser *midi_parser, guint meta_type)
{
  xmlNode *node;
  
  g_return_val_if_fail(AGS_IS_MIDI_PARSER(midi_parser), NULL);
  
  g_object_ref((GObject *) midi_parser);
  g_signal_emit(G_OBJECT(midi_parser),
		midi_parser_signals[END_OF_TRACK], 0,
		meta_type,
		&node);
  g_object_unref((GObject *) midi_parser);

  return(node);
}

xmlNode*  
ags_midi_parser_real_smtpe(AgsMidiParser *midi_parser, guint meta_type)
{
  xmlNode *node;

  gchar *str;

  int length;
  int rr, hr, mn, se, fr, ff;

#ifdef AGS_DEBUG
  g_message("SMTPE");
#endif
  
  ags_midi_parser_read_varlength(midi_parser); // delta-time
  
  ags_midi_parser_midi_getc(midi_parser); // status

  ags_midi_parser_midi_getc(midi_parser); // meta type

  node = xmlNewNode(NULL,
		    "midi-message");

  length = ags_midi_parser_midi_getc(midi_parser);

  rr = 
    hr = ags_midi_parser_midi_getc(midi_parser);

  rr = (0x60 & rr) >> 5;
  hr = (0x1f & hr);
  
  mn = 0xff & ags_midi_parser_midi_getc(midi_parser);
  se = 0xff & ags_midi_parser_midi_getc(midi_parser);
  fr = 0xff & ags_midi_parser_midi_getc(midi_parser);
  ff = 0xff & ags_midi_parser_midi_getc(midi_parser);
  
  xmlNewProp(node,
	     AGS_MIDI_EVENT,
	     "smtpe");

  xmlNewProp(node,
	     "rate",
	     g_strdup_printf("%d", rr));
  
  xmlNewProp(node,
	     "timestamp",
	     g_strdup_printf("%d %d %d %d", hr, mn, se, fr));
  
  return(node);
}

/**
 * ags_midi_parser_smtpe:
 * @midi_parser: the #AgsMidiParser
 * @meta_type: the meta type byte
 * 
 * Parse smtpe.
 * 
 * Returns: (transfer none): the xmlNode containing parsed data
 * 
 * Since: 3.0.0
 */
xmlNode*  
ags_midi_parser_smtpe(AgsMidiParser *midi_parser, guint meta_type)
{
  xmlNode *node;
  
  g_return_val_if_fail(AGS_IS_MIDI_PARSER(midi_parser), NULL);
  
  g_object_ref((GObject *) midi_parser);
  g_signal_emit(G_OBJECT(midi_parser),
		midi_parser_signals[SMTPE], 0,
		meta_type,
		&node);
  g_object_unref((GObject *) midi_parser);

  return(node);
}

xmlNode*  
ags_midi_parser_real_tempo(AgsMidiParser *midi_parser, guint meta_type)
{
  xmlNode *node;

  gchar *str;

  gint tempo;

#ifdef AGS_DEBUG
  g_message("tempo");
#endif
  
  ags_midi_parser_read_varlength(midi_parser); // delta-time
  
  ags_midi_parser_midi_getc(midi_parser); // status

  ags_midi_parser_midi_getc(midi_parser); // meta type
  ags_midi_parser_midi_getc(midi_parser);

  node = xmlNewNode(NULL,
		    "midi-message");

  tempo = ags_midi_parser_read_gint24(midi_parser);
  
  xmlNewProp(node,
	     AGS_MIDI_EVENT,
	     "tempo-number");

  str = g_strdup_printf("%d",
			tempo);
  
  xmlNewProp(node,
	     "tempo",
	     str);

  g_free(str);
  
  return(node);
}

/**
 * ags_midi_parser_tempo:
 * @midi_parser: the #AgsMidiParser
 * @meta_type: the meta type byte
 * 
 * Parse tempo.
 * 
 * Returns: (transfer none): the xmlNode containing parsed data
 * 
 * Since: 3.0.0
 */
xmlNode*  
ags_midi_parser_tempo(AgsMidiParser *midi_parser, guint meta_type)
{
  xmlNode *node;
  
  g_return_val_if_fail(AGS_IS_MIDI_PARSER(midi_parser), NULL);
  
  g_object_ref((GObject *) midi_parser);
  g_signal_emit(G_OBJECT(midi_parser),
		midi_parser_signals[TEMPO], 0,
		meta_type,
		&node);
  g_object_unref((GObject *) midi_parser);

  return(node);
}

xmlNode*  
ags_midi_parser_real_time_signature(AgsMidiParser *midi_parser, guint meta_type)
{
  xmlNode *node;

  gchar *str;

  int nn, dd, cc, bb;
  int denom = 1;

#ifdef AGS_DEBUG
  g_message("time signature");
#endif
  
  ags_midi_parser_read_varlength(midi_parser); // delta-time
  
  ags_midi_parser_midi_getc(midi_parser); // status

  ags_midi_parser_midi_getc(midi_parser); // meta type
  ags_midi_parser_midi_getc(midi_parser);

  node = xmlNewNode(NULL,
		    "midi-message");

  nn = 0xff & ags_midi_parser_midi_getc(midi_parser);
  dd = 0xff & ags_midi_parser_midi_getc(midi_parser);
  cc = 0xff & ags_midi_parser_midi_getc(midi_parser);
  bb = 0xff & ags_midi_parser_midi_getc(midi_parser);
  
  while(dd > 0){
    denom *= 2;
    dd--;
  }

  xmlNewProp(node,
	     AGS_MIDI_EVENT,
	     "time-signature");

  str = g_strdup_printf("%d/%d %d %d",
			nn,
			denom,
			cc,
			bb);
  
  xmlNewProp(node,
	     "timesig",
	     str);

  g_free(str);

  return(node);
}

/**
 * ags_midi_parser_time_signature:
 * @midi_parser: the #AgsMidiParser
 * @meta_type: the meta type byte
 * 
 * Parse time signature.
 * 
 * Returns: (transfer none): the xmlNode containing parsed data
 * 
 * Since: 3.0.0
 */
xmlNode*  
ags_midi_parser_time_signature(AgsMidiParser *midi_parser, guint meta_type)
{
  xmlNode *node;
  
  g_return_val_if_fail(AGS_IS_MIDI_PARSER(midi_parser), NULL);
  
  g_object_ref((GObject *) midi_parser);
  g_signal_emit(G_OBJECT(midi_parser),
		midi_parser_signals[TIME_SIGNATURE], 0,
		meta_type,
		&node);
  g_object_unref((GObject *) midi_parser);

  return(node);
}

xmlNode*  
ags_midi_parser_real_key_signature(AgsMidiParser *midi_parser, guint meta_type)
{
  xmlNode *node;

  gchar *str;

  int sf, mi;

#ifdef AGS_DEBUG
  g_message("key signature");
#endif
  
  ags_midi_parser_read_varlength(midi_parser); // delta-time
  
  ags_midi_parser_midi_getc(midi_parser); // status

  ags_midi_parser_midi_getc(midi_parser); // meta type
  ags_midi_parser_midi_getc(midi_parser);

  node = xmlNewNode(NULL,
		    "midi-message");

  xmlNewProp(node,
	     AGS_MIDI_EVENT,
	     "key-signature");

  sf = ags_midi_parser_midi_getc(midi_parser);
  mi = ags_midi_parser_midi_getc(midi_parser);

  str = g_strdup_printf("%d %s",
			(sf > 127 ? sf - 256: sf),
			(mi ? "minor": "major"));
  
  xmlNewProp(node,
	     "keysig",
	     str);

  g_free(str);
  
  return(node);
}

/**
 * ags_midi_parser_key_signature:
 * @midi_parser: the #AgsMidiParser
 * @meta_type: the meta type byte
 * 
 * Parse key signature.
 * 
 * Returns: (transfer none): the xmlNode containing parsed data
 * 
 * Since: 3.0.0
 */
xmlNode*  
ags_midi_parser_key_signature(AgsMidiParser *midi_parser, guint meta_type)
{
  xmlNode *node;
  
  g_return_val_if_fail(AGS_IS_MIDI_PARSER(midi_parser), NULL);
  
  g_object_ref((GObject *) midi_parser);
  g_signal_emit(G_OBJECT(midi_parser),
		midi_parser_signals[KEY_SIGNATURE], 0,
		meta_type,
		&node);
  g_object_unref((GObject *) midi_parser);

  return(node);
}

xmlNode*  
ags_midi_parser_real_sequencer_meta_event(AgsMidiParser *midi_parser, guint meta_type)
{
  xmlNode *node;

  gchar *str;

  guint len, id, data;

#ifdef AGS_DEBUG
  g_message("sequencer meta event");
#endif

  ags_midi_parser_read_varlength(midi_parser); // delta-time
  
  ags_midi_parser_midi_getc(midi_parser); // status

  ags_midi_parser_midi_getc(midi_parser); // meta type

  node = xmlNewNode(NULL,
		    "midi-meta-event");

  len = ags_midi_parser_midi_getc(midi_parser);
  id = ags_midi_parser_midi_getc(midi_parser);
  data = ags_midi_parser_midi_getc(midi_parser);

  xmlNewProp(node,
	     "length",
	     g_strdup_printf("%d", len));

  xmlNewProp(node,
	     "id",
	     g_strdup_printf("%d", id));

  xmlNewProp(node,
	     "data",
	     g_strdup_printf("%d", data));

  return(node);
}

xmlNode*  
ags_midi_parser_sequencer_meta_event(AgsMidiParser *midi_parser, guint meta_type)
{
  xmlNode *node;
  
  g_return_val_if_fail(AGS_IS_MIDI_PARSER(midi_parser), NULL);
  
  g_object_ref((GObject *) midi_parser);
  g_signal_emit(G_OBJECT(midi_parser),
		midi_parser_signals[META_EVENT], 0,
		meta_type,
		&node);
  g_object_unref((GObject *) midi_parser);

  return(node);
}

xmlNode*  
ags_midi_parser_real_text_event(AgsMidiParser *midi_parser, guint meta_type)
{
  xmlNode *node;
  gchar *text;
  
  size_t tmp_offset;
  guint text_length;
  guint varlength_length;

#ifdef AGS_DEBUG
  g_message("text event");
#endif

  ags_midi_parser_read_varlength(midi_parser); // delta-time
  
  ags_midi_parser_midi_getc(midi_parser); // status

  ags_midi_parser_midi_getc(midi_parser); // meta type

  node = xmlNewNode(NULL,
		    "midi-message");

  text_length = ags_midi_parser_read_varlength(midi_parser);

  varlength_length = ags_midi_smf_util_get_varlength_size(midi_parser->midi_smf_util,
							  (gint) text_length);

  if(text_length > AGS_MIDI_PARSER_MAX_TEXT_LENGTH){
    g_warning("SMF text length exceeds maximum text length [%d]", AGS_MIDI_PARSER_MAX_TEXT_LENGTH);
  }

  tmp_offset = midi_parser->offset;
  text = ags_midi_parser_read_text(midi_parser,
				   text_length);
  
  switch(0x0f & meta_type){
  case 0x01:
    /* Text event */
    xmlNewProp(node,
	       "text",
	       text);
    
    break;
  case 0x02:
    {
      /* Copyright notice */
      xmlNewProp(node,
		 "copyright",
		 text);
    }
    break;
  case 0x03:
    {
      /* Sequence/Track name */
      xmlNewProp(node,
		 "sequence-name",
		 text);
    }
    break;
  case 0x04:
    {
      /* Instrument name */
      xmlNewProp(node,
		 "instrument-name",
		 text);
    }
    break;
  case 0x05:      /* Lyric */
    break;
  case 0x06:      /* Marker */
    break;
  case 0x07:      /* Cue point */
    break;
  case 0x08:
    break;
  case 0x09:
    break;
  case 0x0a:
    break;
  case 0x0b:
    break;
  case 0x0c:
    break;
  case 0x0d:
    break;
  case 0x0e:
    break;
  case 0x0f:
    break;
  default:
    g_warning("unknown text event");
  }
    
  return(node);
}

/**
 * ags_midi_parser_text_event:
 * @midi_parser: the #AgsMidiParser
 * @meta_type: the meta message type
 * 
 * Parse text event.
 * 
 * Returns: (transfer none): the xmlNode containing parsed data
 * 
 * Since: 3.0.0
 */
xmlNode*  
ags_midi_parser_text_event(AgsMidiParser *midi_parser, guint meta_type)
{
  xmlNode *node;
  
  g_return_val_if_fail(AGS_IS_MIDI_PARSER(midi_parser), NULL);
  
  g_object_ref((GObject *) midi_parser);
  g_signal_emit(G_OBJECT(midi_parser),
		midi_parser_signals[TEXT_EVENT], 0,
		meta_type,
		&node);
  g_object_unref((GObject *) midi_parser);

  return(node);
}

xmlNode*  
ags_midi_parser_real_meta_misc(AgsMidiParser *midi_parser, guint meta_type)
{
  xmlNode *node;

  guchar data[5];
  
#ifdef AGS_DEBUG
  g_message("meta-misc");
#endif
  
  node = xmlNewNode(NULL,
		    "midi-message");

  ags_midi_parser_read_varlength(midi_parser); // delta-time  

  ags_midi_parser_midi_getc(midi_parser); // status
  
  ags_midi_parser_midi_getc(midi_parser); // meta misc
  ags_midi_parser_midi_getc(midi_parser);
  ags_midi_parser_midi_getc(midi_parser);
  ags_midi_parser_midi_getc(midi_parser);
  
  xmlNewProp(node,
	     AGS_MIDI_EVENT,
	     "meta-misc");
  
  return(node);
}

/**
 * ags_midi_parser_meta_misc:
 * @midi_parser: the #AgsMidiParser
 * @meta_type: the meta message type
 * 
 * Parse meta_misc.
 * 
 * Returns: (transfer none): the xmlNode containing parsed data
 * 
 * Since: 3.0.0
 */
xmlNode*  
ags_midi_parser_meta_misc(AgsMidiParser *midi_parser, guint meta_type)
{
  xmlNode *node;
  
  g_return_val_if_fail(AGS_IS_MIDI_PARSER(midi_parser), NULL);
  
  g_object_ref((GObject *) midi_parser);
  g_signal_emit(G_OBJECT(midi_parser),
		midi_parser_signals[META_MISC], 0,
		meta_type,
		&node);
  g_object_unref((GObject *) midi_parser);

  return(node);
}

xmlNode*  
ags_midi_parser_real_midi_channel_prefix(AgsMidiParser *midi_parser, guint meta_type)
{
  xmlNode *node;

  gchar *str;
  
  guint length;
  gint midi_channel;

#ifdef AGS_DEBUG
  g_message("midi-channel-prefix");
#endif

  ags_midi_parser_read_varlength(midi_parser); // delta-time
  
  ags_midi_parser_midi_getc(midi_parser); // status

  ags_midi_parser_midi_getc(midi_parser); // meta type
  ags_midi_parser_midi_getc(midi_parser);
  
  node = xmlNewNode(NULL,
		    "midi-message");

  midi_channel = ags_midi_parser_midi_getc(midi_parser);
  
  xmlNewProp(node,
	     AGS_MIDI_EVENT,
	     "midi-channel-prefix");

  str = g_strdup_printf("%d", midi_channel);
  
  xmlNewProp(node,
	     "midi-channel",
	     str);

  g_free(str);
  
  return(node);
}

/**
 * ags_midi_parser_midi_channel_prefix:
 * @midi_parser: the #AgsMidiParser
 * @meta_type: the meta message type
 * 
 * Parse MIDI channel prefix.
 * 
 * Returns: (transfer none): the xmlNode containing parsed data
 * 
 * Since: 6.16.18
 */
xmlNode*  
ags_midi_parser_midi_channel_prefix(AgsMidiParser *midi_parser, guint meta_type)
{
  xmlNode *node;
  
  g_return_val_if_fail(AGS_IS_MIDI_PARSER(midi_parser), NULL);
  
  g_object_ref((GObject *) midi_parser);
  g_signal_emit(G_OBJECT(midi_parser),
		midi_parser_signals[MIDI_CHANNEL_PREFIX], 0,
		meta_type,
		&node);
  g_object_unref((GObject *) midi_parser);

  return(node);
}

xmlNode*
ags_midi_parser_real_quarter_frame(AgsMidiParser *midi_parser, guint status)
{
  xmlNode *node;

  gchar *str;
  
  gint quarter_frame;

  ags_midi_parser_read_varlength(midi_parser); // delta-time
  
  ags_midi_parser_midi_getc(midi_parser); // status
      
  quarter_frame = 0xff & (ags_midi_parser_midi_getc(midi_parser));

  node = xmlNewNode(NULL,
		    "midi-system-common");

  str = g_strdup_printf("%d",
			quarter_frame);
  
  xmlNewProp(node,
	     "quarter-frame",
	     str);

  g_free(str);
  
  return(node);
}

/**
 * ags_midi_parser_quarter_frame:
 * @midi_parser: the #AgsMidiParser
 * @status: the status
 * 
 * Quarter frame.
 * 
 * Returns: (transfer none): the xmlNode containing parsed data
 * 
 * Since: 6.16.18
 */
xmlNode*
ags_midi_parser_quarter_frame(AgsMidiParser *midi_parser, guint status)
{
  xmlNode *node;
  
  g_return_val_if_fail(AGS_IS_MIDI_PARSER(midi_parser), NULL);
  
  g_object_ref((GObject *) midi_parser);
  g_signal_emit(G_OBJECT(midi_parser),
		midi_parser_signals[QUARTER_FRAME], 0,
		status,
		&node);
  g_object_unref((GObject *) midi_parser);

  return(node);
}

xmlNode*
ags_midi_parser_real_song_position(AgsMidiParser *midi_parser, guint status)
{
  xmlNode *node;

  gchar *str;

  gint song_position;

  ags_midi_parser_read_varlength(midi_parser); // delta-time
  
  ags_midi_parser_midi_getc(midi_parser); // status

  song_position = 0x7f & (ags_midi_parser_midi_getc(midi_parser)) << 7;
  song_position |= 0x7f & (ags_midi_parser_midi_getc(midi_parser));
      
  node = xmlNewNode(NULL,
		    "midi-system-common");

  str = g_strdup_printf("%d",
			song_position);
  
  xmlNewProp(node,
	     "song-position",
	     str);

  g_free(str);
    
  return(node);
}

/**
 * ags_midi_parser_song_position:
 * @midi_parser: the #AgsMidiParser
 * @status: the status
 * 
 * Song position.
 * 
 * Returns: (transfer none): the xmlNode containing parsed data
 * 
 * Since: 6.16.18
 */
xmlNode*
ags_midi_parser_song_position(AgsMidiParser *midi_parser, guint status)
{
  xmlNode *node;
  
  g_return_val_if_fail(AGS_IS_MIDI_PARSER(midi_parser), NULL);
  
  g_object_ref((GObject *) midi_parser);
  g_signal_emit(G_OBJECT(midi_parser),
		midi_parser_signals[SONG_POSITION], 0,
		status,
		&node);
  g_object_unref((GObject *) midi_parser);

  return(node);
}

xmlNode*
ags_midi_parser_real_song_select(AgsMidiParser *midi_parser, guint status)
{
  xmlNode *node;

  gchar *str;

  gint song_select;

  ags_midi_parser_read_varlength(midi_parser); // delta-time
  
  ags_midi_parser_midi_getc(midi_parser); // status

  song_select = 0x7f & (ags_midi_parser_midi_getc(midi_parser)) << 7;
      
  node = xmlNewNode(NULL,
		    "midi-system-common");

  str = g_strdup_printf("%d",
			song_select);
  
  xmlNewProp(node,
	     "song-select",
	     str);

  g_free(str);
    
  return(node);
}

/**
 * ags_midi_parser_song_select:
 * @midi_parser: the #AgsMidiParser
 * @status: the status
 * 
 * Song select.
 * 
 * Returns: (transfer none): the xmlNode containing parsed data
 * 
 * Since: 6.16.18
 */
xmlNode*
ags_midi_parser_song_select(AgsMidiParser *midi_parser, guint status)
{
  xmlNode *node;
  
  g_return_val_if_fail(AGS_IS_MIDI_PARSER(midi_parser), NULL);
  
  g_object_ref((GObject *) midi_parser);
  g_signal_emit(G_OBJECT(midi_parser),
		midi_parser_signals[SONG_SELECT], 0,
		status,
		&node);
  g_object_unref((GObject *) midi_parser);

  return(node);
}

xmlNode*
ags_midi_parser_real_tune_request(AgsMidiParser *midi_parser, guint status)
{
  xmlNode *node;

  ags_midi_parser_read_varlength(midi_parser); // delta-time
  
  ags_midi_parser_midi_getc(midi_parser); // status
      
  node = xmlNewNode(NULL,
		    "midi-system-common");
  
  xmlNewProp(node,
	     "tune-request",
	     NULL);
    
  return(node);
}

/**
 * ags_midi_parser_tune_request:
 * @midi_parser: the #AgsMidiParser
 * @status: the status
 * 
 * Song select.
 * 
 * Returns: (transfer none): the xmlNode containing parsed data
 * 
 * Since: 6.16.18
 */
xmlNode*
ags_midi_parser_tune_request(AgsMidiParser *midi_parser, guint status)
{
  xmlNode *node;
  
  g_return_val_if_fail(AGS_IS_MIDI_PARSER(midi_parser), NULL);
  
  g_object_ref((GObject *) midi_parser);
  g_signal_emit(G_OBJECT(midi_parser),
		midi_parser_signals[TUNE_REQUEST], 0,
		status,
		&node);
  g_object_unref((GObject *) midi_parser);

  return(node);
}

/**
 * ags_midi_parser_open_filename:
 * @midi_parser: the #AgsMidiParser
 * @filename: the filename
 * 
 * Open @filename and assign FILE stream to @midi_parser.
 * 
 * Since: 3.2.0
 */
void
ags_midi_parser_open_filename(AgsMidiParser *midi_parser,
			      gchar *filename)
{
  FILE *file;

  if(!AGS_IS_MIDI_PARSER(midi_parser) ||
     filename == NULL){
    return;
  }

  file = fopen(filename, "r");
  
  g_object_set(midi_parser,
	       "file", file,
	       NULL);
}

/**
 * ags_midi_parser_set_buffer:
 * @midi_parser: the #AgsMidiParser
 * @buffer: the buffer
 * 
 * Set buffer of @midi_parser
 * 
 * Since: 3.2.0
 */
void
ags_midi_parser_set_buffer(AgsMidiParser *midi_parser,
			   guchar *buffer)
{
  if(!AGS_IS_MIDI_PARSER(midi_parser)){
    return;
  }

  midi_parser->buffer = buffer;
}

/**
 * ags_midi_parser_set_file_length:
 * @midi_parser: the #AgsMidiParser
 * @file_length: the file length
 * 
 * Set file length of @midi_parser
 * 
 * Since: 3.7.38
 */
void
ags_midi_parser_set_file_length(AgsMidiParser *midi_parser,
				size_t file_length)
{
  if(!AGS_IS_MIDI_PARSER(midi_parser)){
    return;
  }

  midi_parser->file_length = file_length;
}

/**
 * ags_midi_parser_new:
 * 
 * Creates a new instance of #AgsMidiParser
 *
 * Returns: the new #AgsMidiParser
 * 
 * Since: 3.0.0
 */
AgsMidiParser*
ags_midi_parser_new()
{
  AgsMidiParser *midi_parser;
  
  midi_parser = (AgsMidiParser *) g_object_new(AGS_TYPE_MIDI_PARSER,
					       NULL);

  
  return(midi_parser);
}

/**
 * ags_midi_parser_new_from_filename:
 * @filename: (nullable): the filename
 * 
 * Creates a new instance of #AgsMidiParser
 *
 * Returns: the new #AgsMidiParser
 * 
 * Since: 3.2.0
 */
AgsMidiParser*
ags_midi_parser_new_from_filename(gchar *filename)
{
  AgsMidiParser *midi_parser;
  
  midi_parser = (AgsMidiParser *) g_object_new(AGS_TYPE_MIDI_PARSER,
					       NULL);
  
  ags_midi_parser_open_filename(midi_parser,
				filename);
  
  return(midi_parser);
}
