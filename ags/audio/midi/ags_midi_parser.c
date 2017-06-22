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

#include <ags/audio/midi/ags_midi_parser.h>

#include <ags/object/ags_marshal.h>

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

/**
 * SECTION:ags_midi_parser
 * @short_description: the menu bar.
 * @title: AgsMidiParser
 * @section_id:
 * @include: ags/audio/midi/ags_midi_parser.h
 *
 * #AgsMidiParser reads your midi parsers.
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
  LAST_SIGNAL,
};

static gpointer ags_midi_parser_parent_class = NULL;
static guint midi_parser_signals[LAST_SIGNAL];

GType
ags_midi_parser_get_type(void)
{
  static GType ags_type_midi_parser = 0;

  if(!ags_type_midi_parser){
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
  }

  return(ags_type_midi_parser);
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

  /**
   * AgsMidiParser:file:
   *
   * The file to parse data from.
   * 
   * Since: 0.4.2
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

  /* signals */
  /**
   * AgsMidiParser::midi-getc:
   * @midi_parser: the parser
   *
   * The ::midi-getc signal is emited during parsing of event.
   *
   * Returns: The character read
   *
   * Since: 0.5.0
   */
  midi_parser_signals[MIDI_GETC] =
    g_signal_new("midi-getc",
		 G_TYPE_FROM_CLASS(midi_parser),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsMidiParserClass, midi_getc),
		 NULL, NULL,
		 g_cclosure_user_marshal_INT__VOID,
		 G_TYPE_INT, 0);

  /**
   * AgsMidiParser::on-error:
   * @midi_parser: the parser
   *
   * The ::on-error signal is emited as error occurs.
   *
   * Since: 0.5.0
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
   * @midi_parser: the parser
   *
   * The ::parse-full signal is emited during parsing of midi file.
   *
   * Returns: The XML node representing the event
   *
   * Since: 0.5.0
   */
  midi_parser_signals[PARSE_FULL] =
    g_signal_new("parse-full",
		 G_TYPE_FROM_CLASS(midi_parser),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsMidiParserClass, parse_full),
		 NULL, NULL,
		 g_cclosure_user_marshal_POINTER__VOID,
		 G_TYPE_POINTER, 0);

  /**
   * AgsMidiParser::parse-bytes:
   * @midi_parser: the parser
   * @buffer: the MIDI data
   * @buffer_length: the buffer's length
   *
   * The ::parse-bytes signal is emited during parsing of bytes.
   *
   * Returns: The XML node representing the event
   *
   * Since: 0.7.2
   */
  midi_parser_signals[PARSE_BYTES] =
    g_signal_new("parse-bytes",
		 G_TYPE_FROM_CLASS(midi_parser),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsMidiParserClass, parse_bytes),
		 NULL, NULL,
		 g_cclosure_user_marshal_POINTER__POINTER_UINT,
		 G_TYPE_POINTER, 2,
		 G_TYPE_POINTER,
		 G_TYPE_UINT);

  /**
   * AgsMidiParser::parse-header:
   * @midi_parser: the parser
   *
   * The ::parse-header signal is emited during parsing of header.
   *
   * Returns: The XML node representing the header
   *
   * Since: 0.5.0
   */
  midi_parser_signals[PARSE_HEADER] =
    g_signal_new("parse-header",
		 G_TYPE_FROM_CLASS(midi_parser),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsMidiParserClass, parse_header),
		 NULL, NULL,
		 g_cclosure_user_marshal_POINTER__VOID,
		 G_TYPE_POINTER, 0);

  /**
   * AgsMidiParser::parse-track:
   * @midi_parser: the parser
   *
   * The ::parse-track signal is emited during parsing of track.
   *
   * Returns: The XML node representing the track
   *
   * Since: 0.5.0
   */
  midi_parser_signals[PARSE_TRACK] =
    g_signal_new("parse-track",
		 G_TYPE_FROM_CLASS(midi_parser),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsMidiParserClass, parse_track),
		 NULL, NULL,
		 g_cclosure_user_marshal_POINTER__VOID,
		 G_TYPE_POINTER, 0);

  /**
   * AgsMidiParser::key-on:
   * @midi_parser: the parser
   *
   * The ::key-on signal is emited during parsing of event.
   *
   * Returns: The XML node representing the event
   *
   * Since: 0.5.0
   */
  midi_parser_signals[KEY_ON] =
    g_signal_new("key-on",
		 G_TYPE_FROM_CLASS(midi_parser),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsMidiParserClass, key_on),
		 NULL, NULL,
		 g_cclosure_user_marshal_POINTER__UINT,
		 G_TYPE_POINTER, 1,
		 G_TYPE_UINT);

  /**
   * AgsMidiParser::key-off:
   * @midi_parser: the parser
   *
   * The ::key-off signal is emited during parsing of event.
   *
   * Returns: The XML node representing the event
   *
   * Since: 0.5.0
   */
  midi_parser_signals[KEY_OFF] =
    g_signal_new("key-off",
		 G_TYPE_FROM_CLASS(midi_parser),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsMidiParserClass, key_off),
		 NULL, NULL,
		 g_cclosure_user_marshal_POINTER__UINT,
		 G_TYPE_POINTER, 1,
		 G_TYPE_UINT);

  /**
   * AgsMidiParser::key-pressure:
   * @midi_parser: the parser
   *
   * The ::key-pressure signal is emited during parsing of event.
   *
   * Returns: The XML node representing the event
   *
   * Since: 0.5.0
   */
  midi_parser_signals[KEY_PRESSURE] =
    g_signal_new("key-pressure",
		 G_TYPE_FROM_CLASS(midi_parser),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsMidiParserClass, key_pressure),
		 NULL, NULL,
		 g_cclosure_user_marshal_POINTER__UINT,
		 G_TYPE_POINTER, 1,
		 G_TYPE_UINT);

  /**
   * AgsMidiParser::change-parameter:
   * @midi_parser: the parser
   *
   * The ::change-parameter signal is emited during parsing of event.
   *
   * Returns: The XML node representing the event
   *
   * Since: 0.5.0
   */
  midi_parser_signals[CHANGE_PARAMETER] =
    g_signal_new("change-parameter",
		 G_TYPE_FROM_CLASS(midi_parser),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsMidiParserClass, change_parameter),
		 NULL, NULL,
		 g_cclosure_user_marshal_POINTER__UINT,
		 G_TYPE_POINTER, 1,
		 G_TYPE_UINT);

  /**
   * AgsMidiParser::change-pitch-bend:
   * @midi_parser: the parser
   *
   * The ::change-pitch-bend signal is emited during parsing of event.
   *
   * Returns: The XML node representing the event
   *
   * Since: 0.5.0
   */
  midi_parser_signals[CHANGE_PITCH_BEND] =
    g_signal_new("change-pitch-bend",
		 G_TYPE_FROM_CLASS(midi_parser),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsMidiParserClass, change_pitch_bend),
		 NULL, NULL,
		 g_cclosure_user_marshal_POINTER__UINT,
		 G_TYPE_POINTER, 1,
		 G_TYPE_UINT);

  /**
   * AgsMidiParser::change-program:
   * @midi_parser: the parser
   *
   * The ::change-program signal is emited during parsing of event.
   *
   * Returns: The XML node representing the event
   *
   * Since: 0.5.0
   */
  midi_parser_signals[CHANGE_PROGRAM] =
    g_signal_new("change-program",
		 G_TYPE_FROM_CLASS(midi_parser),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsMidiParserClass, change_program),
		 NULL, NULL,
		 g_cclosure_user_marshal_POINTER__UINT,
		 G_TYPE_POINTER, 1,
		 G_TYPE_UINT);

  /**
   * AgsMidiParser::change-channel-pressure:
   * @midi_parser: the parser
   *
   * The ::change-channel-pressure signal is emited during parsing of event.
   *
   * Returns: The XML node representing the event
   *
   * Since: 0.5.0
   */
  midi_parser_signals[CHANGE_CHANNEL_PRESSURE] =
    g_signal_new("change-channel-pressure",
		 G_TYPE_FROM_CLASS(midi_parser),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsMidiParserClass, change_channel_pressure),
		 NULL, NULL,
		 g_cclosure_user_marshal_POINTER__UINT,
		 G_TYPE_POINTER, 1,
		 G_TYPE_UINT);

  /**
   * AgsMidiParser::not-defined:
   * @midi_parser: the parser
   *
   * The ::not-defined signal is emited during parsing of event.
   *
   * Returns: The XML node representing the event
   *
   * Since: 0.5.0
   */
  midi_parser_signals[NOT_DEFINED] =
    g_signal_new("not-defined",
		 G_TYPE_FROM_CLASS(midi_parser),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsMidiParserClass, not_defined),
		 NULL, NULL,
		 g_cclosure_user_marshal_POINTER__UINT,
		 G_TYPE_POINTER, 1,
		 G_TYPE_UINT);

  /**
   * AgsMidiParser::sysex:
   * @midi_parser: the parser
   *
   * The ::sysex signal is emited during parsing of event.
   *
   * Returns: The XML node representing the event
   *
   * Since: 0.5.0
   */
  midi_parser_signals[SYSEX] =
    g_signal_new("sysex",
		 G_TYPE_FROM_CLASS(midi_parser),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsMidiParserClass, sysex),
		 NULL, NULL,
		 g_cclosure_user_marshal_POINTER__UINT,
		 G_TYPE_POINTER, 1,
		 G_TYPE_UINT);

  /**
   * AgsMidiParser::system-common:
   * @midi_parser: the parser
   *
   * The ::system-common signal is emited during parsing of event.
   *
   * Returns: The XML node representing the event
   *
   * Since: 0.5.0
   */
  midi_parser_signals[SYSTEM_COMMON] =
    g_signal_new("system-common",
		 G_TYPE_FROM_CLASS(midi_parser),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsMidiParserClass, system_common),
		 NULL, NULL,
		 g_cclosure_user_marshal_POINTER__UINT,
		 G_TYPE_POINTER, 1,
		 G_TYPE_UINT);

  /**
   * AgsMidiParser::meta-event:
   * @midi_parser: the parser
   *
   * The ::meta-event signal is emited during parsing of event.
   *
   * Returns: The XML node representing the event
   *
   * Since: 0.5.0
   */
  midi_parser_signals[META_EVENT] =
    g_signal_new("meta-event",
		 G_TYPE_FROM_CLASS(midi_parser),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsMidiParserClass, meta_event),
		 NULL, NULL,
		 g_cclosure_user_marshal_POINTER__UINT,
		 G_TYPE_POINTER, 1,
		 G_TYPE_UINT);

  /**
   * AgsMidiParser::sequence-number:
   * @midi_parser: the parser
   *
   * The ::sequence-number signal is emited during parsing of event.
   *
   * Returns: The XML node representing the event
   *
   * Since: 0.5.0
   */
  midi_parser_signals[SEQUENCE_NUMBER] =
    g_signal_new("sequence-number",
		 G_TYPE_FROM_CLASS(midi_parser),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsMidiParserClass, sequence_number),
		 NULL, NULL,
		 g_cclosure_user_marshal_POINTER__UINT,
		 G_TYPE_POINTER, 1,
		 G_TYPE_UINT);

  /**
   * AgsMidiParser::end-of-track:
   * @midi_parser: the parser
   *
   * The ::end-of-track signal is emited during parsing of event.
   *
   * Returns: The XML node representing the event
   *
   * Since: 0.5.0
   */
  midi_parser_signals[END_OF_TRACK] =
    g_signal_new("end-of-track",
		 G_TYPE_FROM_CLASS(midi_parser),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsMidiParserClass, end_of_track),
		 NULL, NULL,
		 g_cclosure_user_marshal_POINTER__UINT,
		 G_TYPE_POINTER, 1,
		 G_TYPE_UINT);

  /**
   * AgsMidiParser::smtpe:
   * @midi_parser: the parser
   *
   * The ::smtpe signal is emited during parsing of event.
   *
   * Returns: The XML node representing the event
   *
   * Since: 0.5.0
   */
  midi_parser_signals[SMTPE] =
    g_signal_new("smtpe",
		 G_TYPE_FROM_CLASS(midi_parser),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsMidiParserClass, smtpe),
		 NULL, NULL,
		 g_cclosure_user_marshal_POINTER__UINT,
		 G_TYPE_POINTER, 1,
		 G_TYPE_UINT);

  /**
   * AgsMidiParser::tempo:
   * @midi_parser: the parser
   *
   * The ::tempo signal is emited during parsing of event.
   *
   * Returns: The XML node representing the event
   *
   * Since: 0.5.0
   */
  midi_parser_signals[TEMPO] =
    g_signal_new("tempo",
		 G_TYPE_FROM_CLASS(midi_parser),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsMidiParserClass, tempo),
		 NULL, NULL,
		 g_cclosure_user_marshal_POINTER__UINT,
		 G_TYPE_POINTER, 1,
		 G_TYPE_UINT);

  /**
   * AgsMidiParser::time-signature:
   * @midi_parser: the parser
   *
   * The ::time-signature signal is emited during parsing of event.
   *
   * Returns: The XML node representing the event
   *
   * Since: 0.5.0
   */
  midi_parser_signals[TIME_SIGNATURE] =
    g_signal_new("time-signature",
		 G_TYPE_FROM_CLASS(midi_parser),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsMidiParserClass, time_signature),
		 NULL, NULL,
		 g_cclosure_user_marshal_POINTER__UINT,
		 G_TYPE_POINTER, 1,
		 G_TYPE_UINT);

  /**
   * AgsMidiParser::key-signature:
   * @midi_parser: the parser
   *
   * The ::key-signature signal is emited during parsing of event.
   *
   * Returns: The XML node representing the event
   *
   * Since: 0.5.0
   */
  midi_parser_signals[KEY_SIGNATURE] =
    g_signal_new("key-signature",
		 G_TYPE_FROM_CLASS(midi_parser),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsMidiParserClass, key_signature),
		 NULL, NULL,
		 g_cclosure_user_marshal_POINTER__UINT,
		 G_TYPE_POINTER, 1,
		 G_TYPE_UINT);

  /**
   * AgsMidiParser::sequencer-meta-event:
   * @midi_parser: the parser
   *
   * The ::sequencer-meta-event signal is emited during parsing of event.
   *
   * Returns: The XML node representing the event
   *
   * Since: 0.5.0
   */
  midi_parser_signals[SEQUENCER_META_EVENT] =
    g_signal_new("sequencer-meta-event",
		 G_TYPE_FROM_CLASS(midi_parser),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsMidiParserClass, sequencer_meta_event),
		 NULL, NULL,
		 g_cclosure_user_marshal_POINTER__UINT,
		 G_TYPE_POINTER, 1,
		 G_TYPE_UINT);

  /**
   * AgsMidiParser::text-event:
   * @midi_parser: the parser
   *
   * The ::text-event signal is emited during parsing of event.
   *
   * Returns: The XML node representing the event
   *
   * Since: 0.5.0
   */
  midi_parser_signals[TEXT_EVENT] =
    g_signal_new("text-event",
		 G_TYPE_FROM_CLASS(midi_parser),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsMidiParserClass, text_event),
		 NULL, NULL,
		 g_cclosure_user_marshal_POINTER__UINT,
		 G_TYPE_POINTER, 1,
		 G_TYPE_UINT);
}

void
ags_midi_parser_init(AgsMidiParser *midi_parser)
{
  midi_parser->flags = 0;
  
  midi_parser->file = NULL;
  midi_parser->nth_chunk = 0;

  midi_parser->file_length = 0;
  midi_parser->offset = 0;

  midi_parser->current_time = 0;
  midi_parser->current_status = 0x0;

  midi_parser->doc = NULL;
}

void
ags_midi_parser_set_property(GObject *gobject,
			     guint prop_id,
			     const GValue *value,
			     GParamSpec *param_spec)
{
  AgsMidiParser *midi_parser;

  midi_parser = AGS_MIDI_PARSER(gobject);
  
  switch(prop_id){
  case PROP_FILE:
    {
      struct stat sb;
      
      midi_parser->file = g_value_get_pointer(value);

      fstat(fileno(midi_parser->file), &sb);
      
      midi_parser->file_length = sb.st_size;

      midi_parser->buffer = (unsigned char *) malloc(midi_parser->file_length * sizeof(unsigned char));

      fread(midi_parser->buffer, sizeof(unsigned char), midi_parser->file_length, midi_parser->file);
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

  midi_parser = AGS_MIDI_PARSER(gobject);
  
  switch(prop_id){
  case PROP_FILE:
    {
      g_value_set_pointer(value,
			  midi_parser->file);
    }
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_midi_parser_finalize(GObject *gobject)
{
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
 * Since: 0.5.0
 */
gint16
ags_midi_parser_read_gint16(AgsMidiParser *midi_parser)
{
  char str[2];
  gint16 value = 0;

  str[0] = (char) 0xff & ags_midi_parser_midi_getc(midi_parser);
  str[1] = (char) 0xff & ags_midi_parser_midi_getc(midi_parser);

  value = (str[0] & 0xff);
  value = (value<<8) + (str[1] & 0xff);
  
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
 * Since: 0.5.0
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

  value = (value<<8) + (str[1] & 0xff);
  value = (value<<8) + (str[2] & 0xff);
  value = (value<<8) + (str[3] & 0xff);
  
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
 * Since: 0.5.0
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
  value = (value<<8) + (str[1] & 0xff);
  value = (value<<8) + (str[2] & 0xff);
  value = (value<<8) + (str[3] & 0xff);
  
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
 * Since: 0.5.0
 */
long
ags_midi_parser_read_varlength(AgsMidiParser *midi_parser)
{
  long value;
  guint i;
  unsigned char c;

  c = ags_midi_parser_midi_getc(midi_parser);
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
 * Since: 0.5.0
 */
gchar*
ags_midi_parser_read_text(AgsMidiParser *midi_parser,
			  gint length)
{
  gchar text[AGS_MIDI_PARSER_MAX_TEXT_LENGTH + 1];
  gchar c;
  guint i;
  
  memset(text, 0, AGS_MIDI_PARSER_MAX_TEXT_LENGTH * sizeof(char));
  i = 0;
  
  while((length <= 0 ||
	 i < length) &&
	(AGS_MIDI_PARSER_EOF & (midi_parser->flags)) == 0){
    (c = (char) 0xff & (ags_midi_parser_midi_getc(midi_parser)));
    
    if(c == '\0' || !(g_ascii_isalnum(c) ||
		      g_ascii_ispunct(c) ||
		      c == ' ')){
      break;
    }

    text[i] = c;
    i++;
  }

  text[i] = '\0';
    
  return(g_strdup(text));
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
 * Since: 0.5.0
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

  if(midi_parser->file == NULL){
    return(-1);
  }
  
  if(midi_parser->offset >= midi_parser->file_length){
    midi_parser->flags |= AGS_MIDI_PARSER_EOF;

    return(-1);
  }

  c = (int) midi_parser->buffer[midi_parser->offset];
  midi_parser->offset += 1;

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
 * Since: 0.5.0
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
 * Since: 0.5.0
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
  xmlDoc *doc;
  xmlNode *root_node;
  xmlNode *tracks_node;
  xmlNode *current;

  /* create xmlDoc and set root node */
  midi_parser->doc = 
    doc = xmlNewDoc("1.0");
  root_node = xmlNewNode(NULL, "midi");
  xmlDocSetRootElement(doc, root_node);

  /* create tracks node */
  tracks_node = xmlNewNode(NULL, "midi-tracks");

  /* parse header */
  current = ags_midi_parser_parse_header(midi_parser);
  xmlAddChild(root_node,
	      current);

#ifdef AGS_DEBUG
  g_message("parsed header");
#endif

  /* parse tracks */
  xmlAddChild(root_node,
	      tracks_node);
  
  while(((AGS_MIDI_PARSER_EOF & (midi_parser->flags))) == 0){
    current = ags_midi_parser_parse_track(midi_parser);
    
    if(current != NULL){
      xmlAddChild(tracks_node,
		  current);
#ifdef AGS_DEBUG
      g_message("parsed track");
#endif
    }else{
      g_warning("skipped input");
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
 * Returns: the xmlDoc representing your MIDI file
 * 
 * Since: 0.5.0
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
			    unsigned char *midi_buffer,
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

  static gchar header[] = "MThd";

  guint offset;
  guint format;
  guint count;
  guint division;
  guint beat, clicks;
  guint n;
  gchar c;

  /* read header */
  n = 0;
  
  while(n < 4 &&
	(AGS_MIDI_PARSER_EOF & (midi_parser->flags)) == 0){
    c = ags_midi_parser_midi_getc(midi_parser);
    
    if(c == header[n]){
      n++;
    }else{
      n = 0;
    }
  }
  
  node = xmlNewNode(NULL,
		    "midi-header");

  /* get some values */
  offset = (guint) ags_midi_parser_read_gint32(midi_parser);
  format = (guint) ags_midi_parser_read_gint16(midi_parser);
  count = (guint) ags_midi_parser_read_gint16(midi_parser);
  division = (guint) ags_midi_parser_read_gint16(midi_parser);

  xmlNewProp(node,
	     "offset",
	     g_strdup_printf("%d", offset));

  xmlNewProp(node,
	     "format",
	     g_strdup_printf("%d", format));

  if(division & 0x8000){
    /* SMTPE */
    xmlNewProp(node,
	       "division",
	       g_strdup_printf("%d %d", -((-(division>>8))&0xff), division&0xff));
  }else{
    xmlNewProp(node,
	       "division",
	       g_strdup_printf("%d", division));
  }
  
  if(format > 2){
    g_warning("Can't deal with format %d files\n", format);

    return(NULL);
  }
  
  beat =
    clicks = division;

  xmlNewProp(node,
	     "beat",
	     g_strdup_printf("%d", beat));

  xmlNewProp(node,
	     "track-count",
	     g_strdup_printf("%d", count));

  return(node);
}

/**
 * ags_midi_parser_parse_header:
 * @midi_parser: the #AgsMidiParser
 * 
 * Parse MIDI header
 * 
 * Returns: the xmlNode containing parsed data
 * 
 * Since: 0.5.0
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
  xmlNode *node, *current;

  static gchar track[] = "MTrk";

  gint offset;
  long delta_time;
  guint status;
  guint n;
  gchar c;

  n = 0;
  
  while(n < 4 &&
	(AGS_MIDI_PARSER_EOF & (midi_parser->flags)) == 0){
    c = ags_midi_parser_midi_getc(midi_parser);
    
    if(c == track[n]){
      n++;
    }else{
      n = 0;
    }
  }

  if((AGS_MIDI_PARSER_EOF & (midi_parser->flags)) != 0){
    return(NULL);
  }

  node = xmlNewNode(NULL, "midi-track");

  offset = ags_midi_parser_read_gint32(midi_parser);

#ifdef AGS_DEBUG
  g_message("n = %d\noffset = %d", n, offset);
#endif
  
  if(offset < 0){
    return(NULL);
  }

  midi_parser->current_time = 0;
  
  for(; midi_parser->offset < midi_parser->file_length; ){
    delta_time = ags_midi_parser_read_varlength(midi_parser);
    midi_parser->current_time += (guint) delta_time;
    
    status = ags_midi_parser_midi_getc(midi_parser);
    
    if((0xf0 & status) == 0xf0){
#ifdef AGS_DEBUG
      g_message("status message");
#endif
      
      switch(status){
      case 0xf0:
	{
	  /* start of system exclusive */
	  ags_midi_parser_sysex(midi_parser,
				status);

	}
	break;
      case 0xf1:
      case 0xf2:
      case 0xf3:
      case 0xf4:
      case 0xf5:
      case 0xf6:
	{
	  ags_midi_parser_system_common(midi_parser,
					status);
	}
	break;
      case 0xf7:
	{
	  /* sysex continuation or arbitrary stuff */
#ifdef AGS_DEBUG
	  g_message("sysex end");
#endif
	}
	break;
      case 0xff:
	{
	  /* meta event */
	  current = ags_midi_parser_meta_event(midi_parser,
					       status);

	  if(current != NULL){
	    xmlChar *str;
	    
	    xmlNewProp(current,
		       "delta-time",
		       g_strdup_printf("%d", midi_parser->current_time));
	    
	    xmlAddChild(node,
			current);

	    str = xmlGetProp(current,
			     AGS_MIDI_EVENT);
	    
	    if(str != NULL &&
	       !g_ascii_strcasecmp(str,
				   "end-of-track")){
	      return(node);
	    }
	  }
	}
	break;
      default:
	g_warning("bad byte");
      }
    }else{
      current = ags_midi_parser_channel_message(midi_parser,
						status);

      if(current != NULL){
	xmlNewProp(current,
		   "delta-time",
		   g_strdup_printf("%u", midi_parser->current_time));
	
	xmlAddChild(node,
		    current);
      }else{
	g_message("unknown channel message 0x%x", status);
      }
      
#ifdef AGS_DEBUG
      g_message("channel message");
#endif
    }
  }
  
  return(node);
}

/**
 * ags_midi_parser_parse_track:
 * @midi_parser: the #AgsMidiParser
 * 
 * Parse MIDI track.
 * 
 * Returns: the xmlNode containing parsed data
 * 
 * Since: 0.5.0
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
 * 
 * Parse channel message.
 * 
 * Returns: the xmlNode containing parsed data
 * 
 * Since: 0.5.0
 */
xmlNode*
ags_midi_parser_channel_message(AgsMidiParser *midi_parser, guint status)
{
  xmlNode *node;

  node = NULL;

#ifdef AGS_DEBUG
  g_message("channel message");
#endif

 ags_midi_parser_channel_message_CHECK:
  
  switch(status & 0xf0){
  case 0x80:
    {
      node = ags_midi_parser_key_off(midi_parser, status);

      midi_parser->current_status = status;
    }
    break;
  case 0x90:
    {
      node = ags_midi_parser_key_on(midi_parser, status);

      midi_parser->current_status = status;
    }
    break;
  case 0xa0:
    {
      node = ags_midi_parser_key_pressure(midi_parser, status);

      midi_parser->current_status = status;
    }
    break;
  case 0xb0:
    {
      node = ags_midi_parser_change_parameter(midi_parser, status);

      midi_parser->current_status = status;
    }
    break;
  case 0xc0:
    {
      node = ags_midi_parser_change_program(midi_parser, status);

      midi_parser->current_status = status;
    }
    break;
  case 0xd0:
    {
      node = ags_midi_parser_change_channel_pressure(midi_parser, status);

      midi_parser->current_status = status;
    }
    break;
  case 0xe0:
    {
      node = ags_midi_parser_change_pitch_bend(midi_parser, status);

      midi_parser->current_status = status;
    }
    break;
  default:
    {
      if(midi_parser->current_status >= 0x80 &&
	 midi_parser->current_status <= 0xef){
#ifdef AGS_DEBUG
	g_message("repeat status=0x%x", midi_parser->current_status);
#endif

	midi_parser->offset -= 1;
	status = midi_parser->current_status;
	
	goto ags_midi_parser_channel_message_CHECK;
      }
    }
  }

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
 * Returns: the xmlNode containing parsed data
 * 
 * Since: 0.5.0
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
 * Returns: the xmlNode containing parsed data
 * 
 * Since: 0.5.0
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
 * Returns: the xmlNode containing parsed data
 * 
 * Since: 0.5.0
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
 * Returns: the xmlNode containing parsed data
 * 
 * Since: 0.5.0
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
  int channel, pitch, transmitter;

#ifdef AGS_DEBUG
  g_message("change pitch bend");
#endif

  channel = 0xf & status;
  pitch = 0x7f & ags_midi_parser_midi_getc(midi_parser);
  transmitter = 0x7f & ags_midi_parser_midi_getc(midi_parser);
  
  node = xmlNewNode(NULL,
		    "midi-message");

  xmlNewProp(node,
	     "channel",
	     g_strdup_printf("%d", channel));

  xmlNewProp(node,
	     AGS_MIDI_EVENT,
	     "pitch-bend");

  xmlNewProp(node,
	     "pitch",
	     g_strdup_printf("%d", pitch));	  

  xmlNewProp(node,
	     "transmitter",
	     g_strdup_printf("%d", transmitter));	  
  
  return(node);
}

/**
 * ags_midi_parser_change_pitch_bend:
 * @midi_parser: the #AgsMidiParser
 * @status: the status byte
 * 
 * Parse change pitch bend.
 * 
 * Returns: the xmlNode containing parsed data
 * 
 * Since: 0.5.0
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
  int channel, program;

#ifdef AGS_DEBUG
  g_message("change program");
#endif

  channel = 0xf & status;
  program = 0x7f & ags_midi_parser_midi_getc(midi_parser);
  
  node = xmlNewNode(NULL,
		    "midi-message");

  xmlNewProp(node,
	     AGS_MIDI_EVENT,
	     "program-change");

  xmlNewProp(node,
	     "channel",
	     g_strdup_printf("%d", channel));

  xmlNewProp(node,
	     "program",
	     g_strdup_printf("%d", program));
  
  return(node);
}

/**
 * ags_midi_parser_change_program:
 * @midi_parser: the #AgsMidiParser
 * @status: the status byte
 * 
 * Parse change program.
 * 
 * Returns: the xmlNode containing parsed data
 * 
 * Since: 0.5.0
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
 * Returns: the xmlNode containing parsed data
 * 
 * Since: 0.5.0
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

  return(NULL);
}

/**
 * ags_midi_parser_not_defined:
 * @midi_parser: the #AgsMidiParser
 * @status: the status byte
 * 
 * Parse not defined.
 * 
 * Returns: the xmlNode containing parsed data
 * 
 * Since: 0.5.0
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
  gchar c;
  
  while((c = ags_midi_parser_midi_getc(midi_parser)) != 0xf7 &&
	(AGS_MIDI_PARSER_EOF & (midi_parser->flags)) == 0);

#ifdef AGS_DEBUG
  g_message("discarded sysex");
#endif
}

/**
 * ags_midi_parser_sysex:
 * @midi_parser: the #AgsMidiParser
 * @status: the status byte
 * 
 * Parse sysex.
 * 
 * Returns: the xmlNode containing parsed data
 * 
 * Since: 0.5.0
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
  int c;

  node = NULL;

#ifdef AGS_DEBUG
  g_message("system common");
#endif

  switch(status){
  case 0xf1:
    {
      guint quarter_frame;
      
      quarter_frame = 0xff & (ags_midi_parser_midi_getc(midi_parser));

      node = xmlNewNode(NULL,
			"midi-system-common");
      xmlNewProp(node,
		 "quarter-frame",
		 g_strdup_printf("%d", quarter_frame));
    }
    break;
  case 0xf2:
    {
      guint song_position;
      
      song_position = 0x7f & (ags_midi_parser_midi_getc(midi_parser)) << 7;
      song_position |= 0x7f & (ags_midi_parser_midi_getc(midi_parser));

      node = xmlNewNode(NULL,
			"midi-system-common");
      xmlNewProp(node,
		 "song-position",
		 g_strdup_printf("%d", song_position));
    }
    break;
  case 0xf3:
    {
      guint song_select;
      
      song_select = 0x7f & (ags_midi_parser_midi_getc(midi_parser));

      node = xmlNewNode(NULL,
			"midi-system-common");
      xmlNewProp(node,
		 "song-select",
		 g_strdup_printf("%d", song_select));
    }
    break;
  case 0xf4:
    {
#ifdef AGS_DEBUG
      g_message("undefined");
#endif
    }
    break;
  case 0xf5:
    {
#ifdef AGS_DEBUG
      g_message("undefined");
#endif
    }
    break;
  case 0xf6:
    {
#ifdef AGS_DEBUG
      g_message("tune request");
#endif
    }
    break;
  }

  return(node);
}

/**
 * ags_midi_parser_system_common:
 * @midi_parser: the #AgsMidiParser
 * @status: the status byte
 * 
 * Parse system common.
 * 
 * Returns: the xmlNode containing parsed data
 * 
 * Since: 0.5.0
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
  char chunk_meta_length[4];
  guint meta_type;
  
#ifdef AGS_DEBUG
  g_message("meta event");
#endif

  meta_type = 0xff & (ags_midi_parser_midi_getc(midi_parser));

  node = NULL;
  
  switch(meta_type){
  case 0x00:
    {
      int c;

      c = ags_midi_parser_midi_getc(midi_parser);

      if(c == 0x02){
	node = ags_midi_parser_sequence_number(midi_parser, meta_type);
      }
    }
    break;
  case 0x01:      /* Text event */
  case 0x02:      /* Copyright notice */
  case 0x03:      /* Sequence/Track name */
  case 0x04:      /* Instrument name */
  case 0x05:      /* Lyric */
  case 0x06:      /* Marker */
  case 0x07:      /* Cue point */
  case 0x08:
  case 0x09:
  case 0x0a:
  case 0x0b:
  case 0x0c:
  case 0x0d:
  case 0x0e:
  case 0x0f:
    {
      /* These are all text events */
      node = ags_midi_parser_text_event(midi_parser, meta_type);
    }
    break;
  case 0x2f:
    {
      /* End of Track */
      node = ags_midi_parser_end_of_track(midi_parser, meta_type);
    }
    break;
  case 0x51:
    {
      /* Set tempo */
      node = ags_midi_parser_tempo(midi_parser, meta_type);
    }
    break;
  case 0x54:
    {
      node = ags_midi_parser_smtpe(midi_parser, meta_type);
    }
    break;
  case 0x58:
    {
      node = ags_midi_parser_time_signature(midi_parser, meta_type);
    }
    break;
  case 0x59:
    {
      node = ags_midi_parser_key_signature(midi_parser, meta_type);
    }
    break;
  case 0x7f:
    {
      node = ags_midi_parser_sequencer_meta_event(midi_parser, meta_type);
    }
    break;
  default:
    midi_parser->offset += 5;
      
    node = xmlNewNode(NULL,
		      "midi-message");
    xmlNewProp(node,
	       AGS_MIDI_EVENT,
	       "misc");
  }
  
#ifdef AGS_DEBUG
  g_message("meta type 0x%x", meta_type);
#endif
  
  return(node);
}

/**
 * ags_midi_parser_meta_event:
 * @midi_parser: the #AgsMidiParser
 * @status: the status byte
 * 
 * Parse meta event.
 * 
 * Returns: the xmlNode containing parsed data
 * 
 * Since: 0.5.0
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
  guint sequence;
  
#ifdef AGS_DEBUG
  g_message("sequence number");
#endif

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
 * Returns: the xmlNode containing parsed data
 * 
 * Since: 0.5.0
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
 * Returns: the xmlNode containing parsed data
 * 
 * Since: 0.5.0
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
  int length;
  int rr, hr, mn, se, fr, ff;

#ifdef AGS_DEBUG
  g_message("SMTPE");
#endif
  
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
 * Returns: the xmlNode containing parsed data
 * 
 * Since: 0.5.0
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

  guint length;
  gint tempo;

#ifdef AGS_DEBUG
  g_message("tempo");
#endif
  
  node = xmlNewNode(NULL,
		    "midi-message");

  length = 0xff & ags_midi_parser_midi_getc(midi_parser);

  tempo = ags_midi_parser_read_gint24(midi_parser);
  
  xmlNewProp(node,
	     AGS_MIDI_EVENT,
	     "tempo-number");
  
  xmlNewProp(node,
	     "tempo",
	     g_strdup_printf("%d", tempo));
  
  return(node);
}

/**
 * ags_midi_parser_tempo:
 * @midi_parser: the #AgsMidiParser
 * @meta_type: the meta type byte
 * 
 * Parse tempo.
 * 
 * Returns: the xmlNode containing parsed data
 * 
 * Since: 0.5.0
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
  int length;
  int nn, dd, cc, bb;
  int denom = 1;

#ifdef AGS_DEBUG
  g_message("time signature");
#endif
  
  node = xmlNewNode(NULL,
		    "midi-message");

  length = 0xff & ags_midi_parser_midi_getc(midi_parser);

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

  xmlNewProp(node,
	     "timesig",
	     g_strdup_printf("%d/%d %d %d", nn, denom, cc, bb));

  return(node);
}

/**
 * ags_midi_parser_time_signature:
 * @midi_parser: the #AgsMidiParser
 * @meta_type: the meta type byte
 * 
 * Parse time signature.
 * 
 * Returns: the xmlNode containing parsed data
 * 
 * Since: 0.5.0
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
  int length;
  int sf, mi;

#ifdef AGS_DEBUG
  g_message("key signature");
#endif
  
  node = xmlNewNode(NULL,
		    "midi-message");

  xmlNewProp(node,
	     AGS_MIDI_EVENT,
	     "key-signature");

  length = ags_midi_parser_midi_getc(midi_parser);

  sf = ags_midi_parser_midi_getc(midi_parser);
  mi = ags_midi_parser_midi_getc(midi_parser);

  xmlNewProp(node,
	     "keysig",
	     g_strdup_printf("%d %s", (sf > 127 ? sf - 256: sf), (mi ? "minor": "major")));
  
  return(node);
}

/**
 * ags_midi_parser_key_signature:
 * @midi_parser: the #AgsMidiParser
 * @meta_type: the meta type byte
 * 
 * Parse key signature.
 * 
 * Returns: the xmlNode containing parsed data
 * 
 * Since: 0.5.0
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
  guint len, id, data;

#ifdef AGS_DEBUG
  g_message("sequencer meta event");
#endif

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
  guint text_length;

#ifdef AGS_DEBUG
  g_message("text event");
#endif

  node = xmlNewNode(NULL,
		    "midi-message");

  //TODO:JK: verify me
  text_length = 0x7f & ags_midi_parser_midi_getc(midi_parser);
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
 * @meta_type: the meta type byte
 * 
 * Parse text event.
 * 
 * Returns: the xmlNode containing parsed data
 * 
 * Since: 0.5.0
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

AgsMidiParser*
ags_midi_parser_new(FILE *file)
{
  AgsMidiParser *midi_parser;
  struct stat sb;
  
  midi_parser = (AgsMidiParser *) g_object_new(AGS_TYPE_MIDI_PARSER,
					       "file", file,
					       NULL);

  
  return(midi_parser);
}

