/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2015 Joël Krähemann
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <midi2ags/midi/ags_midi_parser.h>
#include <midi2ags/object/ags_marshal.h>

#include <stdlib.h>
#include <stdio.h>

#include <fcntl.h>
#include <sys/stat.h>

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
xmlNode* ags_midi_parser_real_smpte(AgsMidiParser *midi_parser, guint meta_type);
xmlNode* ags_midi_parser_real_tempo(AgsMidiParser *midi_parser, guint meta_type);
xmlNode* ags_midi_parser_real_time_signature(AgsMidiParser *midi_parser, guint meta_type);
xmlNode* ags_midi_parser_real_key_signature(AgsMidiParser *midi_parser, guint meta_type);
xmlNode* ags_midi_parser_real_sequencer_meta_event(AgsMidiParser *midi_parser, guint meta_type);
xmlNode* ags_midi_parser_real_text_event(AgsMidiParser *midi_parser, guint meta_type);

#define AGS_MIDI_PARSER_MAX_TEXT_LENGTH (4096)

#define AGS_MIDI_EVENT "event\0"

/**
 * SECTION:ags_midi_parser
 * @short_description: the menu bar.
 * @title: AgsMidiParser
 * @section_id:
 * @include: ags/X/ags_midi_parser.h
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
  SMPTE,
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
						  "AgsMidiParser\0", &ags_midi_parser_info,
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
   * AgsMidiParser:dsp-channels:
   *
   * The dsp channel count
   * 
   * Since: 0.4.2
   */
  param_spec = g_param_spec_pointer("file\0",
				    "the file stream\0",
				    "The file stream to parse\0",
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FILE,
				  param_spec);

  /* AgsMidiParser */
  midi_parser->midi_getc = ags_midi_parser_real_midi_getc;
  midi_parser->on_error = ags_midi_parser_real_on_error;

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
  midi_parser->smpte = ags_midi_parser_real_smpte;
  midi_parser->tempo = ags_midi_parser_real_tempo;
  midi_parser->time_signature = ags_midi_parser_real_time_signature;
  midi_parser->key_signature = ags_midi_parser_real_key_signature;
  midi_parser->sequencer_meta_event = ags_midi_parser_real_sequencer_meta_event;
  midi_parser->text_event = ags_midi_parser_real_text_event;

  midi_parser->parse_full = ags_midi_parser_real_parse_full;
  
  /* signals */
  /**
   * AgsMidiParser::midi-getc:
   * @midi_parser: the parser
   *
   * Returns: The XML node representing the event
   *
   * The ::midi-getc signal is emited during parsing of event.
   */
  midi_parser_signals[MIDI_GETC] =
    g_signal_new("midi-getc\0",
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
   * Returns: The XML node representing the event
   *
   * The ::on-error signal is emited during parsing of event.
   */
  midi_parser_signals[ON_ERROR] =
    g_signal_new("on-error\0",
		 G_TYPE_FROM_CLASS(midi_parser),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsMidiParserClass, on_error),
		 NULL, NULL,
		 g_cclosure_user_marshal_POINTER__VOID,
		 G_TYPE_POINTER, 0);


  /**
   * AgsMidiParser::parse-header:
   * @midi_parser: the parser
   *
   * Returns: The XML node representing the header
   *
   * The ::parse-header signal is emited during parsing of header.
   */
  midi_parser_signals[PARSE_HEADER] =
    g_signal_new("parse-header\0",
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
   * Returns: The XML node representing the track
   *
   * The ::parse-track signal is emited during parsing of track.
   */
  midi_parser_signals[PARSE_TRACK] =
    g_signal_new("parse-track\0",
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
   * Returns: The XML node representing the event
   *
   * The ::key-on signal is emited during parsing of event.
   */
  midi_parser_signals[KEY_ON] =
    g_signal_new("key-on\0",
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
   * Returns: The XML node representing the event
   *
   * The ::key-off signal is emited during parsing of event.
   */
  midi_parser_signals[KEY_OFF] =
    g_signal_new("key-off\0",
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
   * Returns: The XML node representing the event
   *
   * The ::key-pressure signal is emited during parsing of event.
   */
  midi_parser_signals[KEY_PRESSURE] =
    g_signal_new("key-pressure\0",
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
   * Returns: The XML node representing the event
   *
   * The ::change-parameter signal is emited during parsing of event.
   */
  midi_parser_signals[CHANGE_PARAMETER] =
    g_signal_new("change-parameter\0",
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
   * Returns: The XML node representing the event
   *
   * The ::change-pitch-bend signal is emited during parsing of event.
   */
  midi_parser_signals[CHANGE_PITCH_BEND] =
    g_signal_new("change-pitch-bend\0",
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
   * Returns: The XML node representing the event
   *
   * The ::change-program signal is emited during parsing of event.
   */
  midi_parser_signals[CHANGE_PROGRAM] =
    g_signal_new("change-program\0",
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
   * Returns: The XML node representing the event
   *
   * The ::change-channel-pressure signal is emited during parsing of event.
   */
  midi_parser_signals[CHANGE_CHANNEL_PRESSURE] =
    g_signal_new("change-channel-pressure\0",
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
   * Returns: The XML node representing the event
   *
   * The ::not-defined signal is emited during parsing of event.
   */
  midi_parser_signals[NOT_DEFINED] =
    g_signal_new("not-defined\0",
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
   * Returns: The XML node representing the event
   *
   * The ::sysex signal is emited during parsing of event.
   */
  midi_parser_signals[SYSEX] =
    g_signal_new("sysex\0",
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
   * Returns: The XML node representing the event
   *
   * The ::system-common signal is emited during parsing of event.
   */
  midi_parser_signals[SYSTEM_COMMON] =
    g_signal_new("system-common\0",
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
   * Returns: The XML node representing the event
   *
   * The ::meta-event signal is emited during parsing of event.
   */
  midi_parser_signals[META_EVENT] =
    g_signal_new("meta-event\0",
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
   * Returns: The XML node representing the event
   *
   * The ::sequence-number signal is emited during parsing of event.
   */
  midi_parser_signals[SEQUENCE_NUMBER] =
    g_signal_new("sequence-number\0",
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
   * Returns: The XML node representing the event
   *
   * The ::end-of-track signal is emited during parsing of event.
   */
  midi_parser_signals[END_OF_TRACK] =
    g_signal_new("end-of-track\0",
		 G_TYPE_FROM_CLASS(midi_parser),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsMidiParserClass, end_of_track),
		 NULL, NULL,
		 g_cclosure_user_marshal_POINTER__UINT,
		 G_TYPE_POINTER, 1,
		 G_TYPE_UINT);

  /**
   * AgsMidiParser::smpte:
   * @midi_parser: the parser
   *
   * Returns: The XML node representing the event
   *
   * The ::smpte signal is emited during parsing of event.
   */
  midi_parser_signals[SMPTE] =
    g_signal_new("smpte\0",
		 G_TYPE_FROM_CLASS(midi_parser),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsMidiParserClass, smpte),
		 NULL, NULL,
		 g_cclosure_user_marshal_POINTER__UINT,
		 G_TYPE_POINTER, 1,
		 G_TYPE_UINT);

  /**
   * AgsMidiParser::tempo:
   * @midi_parser: the parser
   *
   * Returns: The XML node representing the event
   *
   * The ::tempo signal is emited during parsing of event.
   */
  midi_parser_signals[TEMPO] =
    g_signal_new("tempo\0",
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
   * Returns: The XML node representing the event
   *
   * The ::time-signature signal is emited during parsing of event.
   */
  midi_parser_signals[TIME_SIGNATURE] =
    g_signal_new("time-signature\0",
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
   * Returns: The XML node representing the event
   *
   * The ::key-signature signal is emited during parsing of event.
   */
  midi_parser_signals[KEY_SIGNATURE] =
    g_signal_new("key-signature\0",
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
   * Returns: The XML node representing the event
   *
   * The ::sequencer-meta-event signal is emited during parsing of event.
   */
  midi_parser_signals[SEQUENCER_META_EVENT] =
    g_signal_new("sequencer-meta-event\0",
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
   * Returns: The XML node representing the event
   *
   * The ::text-event signal is emited during parsing of event.
   */
  midi_parser_signals[TEXT_EVENT] =
    g_signal_new("text-event\0",
		 G_TYPE_FROM_CLASS(midi_parser),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsMidiParserClass, text_event),
		 NULL, NULL,
		 g_cclosure_user_marshal_POINTER__UINT,
		 G_TYPE_POINTER, 1,
		 G_TYPE_UINT);

  /**
   * AgsMidiParser::parse-full:
   * @midi_parser: the parser
   *
   * Returns: The XML node representing the event
   *
   * The ::parse-full signal is emited during parsing of midi file.
   */
  midi_parser_signals[PARSE_FULL] =
    g_signal_new("parse-full\0",
		 G_TYPE_FROM_CLASS(midi_parser),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsMidiParserClass, parse_full),
		 NULL, NULL,
		 g_cclosure_user_marshal_POINTER__VOID,
		 G_TYPE_POINTER, 0);
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
      midi_parser->file = g_value_get_pointer(value);
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

long
ags_midi_parser_read_varlength(AgsMidiParser *midi_parser)
{
  long value;
  guint i;
  char c;
  
  c = ags_midi_parser_midi_getc(midi_parser);
  value = c;
  i = 1;
  
  if(c & 0x80){
    value &= 0x7F;
   
    do{
      value = (value << 7) + ((c = ags_midi_parser_midi_getc(midi_parser)) & 0x7F);
      i++;
    }while(c & 0x80);
  }
  
  return(value);
}

gchar*
ags_midi_parser_read_text(AgsMidiParser *midi_parser,
			  gint length)
{
  gchar text[AGS_MIDI_PARSER_MAX_TEXT_LENGTH];
  gchar c;
  guint i;

  memset(text, 0, AGS_MIDI_PARSER_MAX_TEXT_LENGTH * sizeof(char));
  i = 0;
  
  while((length <= 0 ||
	 i < length) && (c = (char) 0xff & ags_midi_parser_midi_getc(midi_parser)) != EOF){
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
    gdouble smpte_format, smpte_resolution;

    smpte_format = (gdouble) ((0xff00 & division) >> 8);
    smpte_resolution = (gdouble) (0xff & division);

    retval = (((gdouble) ticks) /
	      (gdouble) (smpte_format * smpte_resolution * 1000000.0));
    
    return(retval);
  }
}

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

int
ags_midi_parser_real_midi_getc(AgsMidiParser *midi_parser)
{
  int c;

  c = fgetc(midi_parser->file);
  
  if(c == EOF){
    g_warning("reached end of file\0");
    midi_parser->flags |= AGS_MIDI_PARSER_EOF;
  }

  return(c);
}

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

void
ags_midi_parser_real_on_error(AgsMidiParser *midi_parser,
			      GError **error)
{
}

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

xmlDoc*
ags_midi_parser_real_parse_full(AgsMidiParser *midi_parser)
{
  xmlDoc *doc;
  xmlNode *root_node;
  xmlNode *tracks_node;
  xmlNode *current;

  GError *error;

  midi_parser->doc = 
    doc = xmlNewDoc("1.0\0");
  root_node = xmlNewNode(NULL, "midi\0");
  xmlDocSetRootElement(doc, root_node);

  tracks_node = xmlNewNode(NULL, "midi-tracks\0");

  error = NULL;

  current = ags_midi_parser_parse_header(midi_parser);
  xmlAddChild(root_node,
	      current);
#ifdef DEBUG
  g_message("parsed header\0");
#endif
  
  xmlAddChild(root_node,
	      tracks_node);
  
  while(((AGS_MIDI_PARSER_EOF & (midi_parser->flags))) == 0){
    current = ags_midi_parser_parse_track(midi_parser);
    
    if(current != NULL){
      xmlAddChild(tracks_node,
		  current);
#ifdef DEBUG
      g_message("parsed track\0");
#endif
    }else{
      g_warning("skipped input\0");
    }
  }

  return(doc);
}

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
ags_midi_parser_real_parse_header(AgsMidiParser *midi_parser)
{
  xmlNode *node;

  static gchar header[] = "MThd";
  guint format;
  guint count;
  guint division;
  guint offset;
  guint times;
  guint beat, clicks;
  guint n;
  gchar c;
  
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
		    "midi-header\0");

  offset = (guint) ags_midi_parser_read_gint32(midi_parser);
  format = (guint) ags_midi_parser_read_gint16(midi_parser);
  count = (guint) ags_midi_parser_read_gint16(midi_parser);
  division = (guint) ags_midi_parser_read_gint16(midi_parser);

  xmlNewProp(node,
	     "format\0",
	     g_strdup_printf("%d\0", format));

  if(division & 0x8000){
    /* SMPTE */
    times = 0; /* Can't do beats */

    xmlNewProp(node,
	       "division\0",
	       g_strdup_printf("%d %d\0", -((-(division>>8))&0xff), division&0xff));
  }else{
    xmlNewProp(node,
	       "division\0",
	       g_strdup_printf("%d\0", division));
  }
  
  if(format > 2){
    fprintf(stderr, "Can't deal with format %d files\n", format);
    exit(-1);
  }
  
  beat =
    clicks = division;

  xmlNewProp(node,
	     "beat\0",
	     g_strdup_printf("%d\0", beat));

  xmlNewProp(node,
	     "track-count\0",
	     g_strdup_printf("%d\0", count));

  return(node);
}

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

xmlNode*
ags_midi_parser_real_parse_track(AgsMidiParser *midi_parser)
{
  xmlNode *node, *current;
  static gchar track[] = "MTrk";
  gint offset, start_offset;
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

  node = xmlNewNode(NULL, "midi-track\0");

  offset = ags_midi_parser_read_gint32(midi_parser);

#ifdef DEBUG
  g_message("n = %d\noffset = %d\0", n, offset);
#endif
  
  start_offset = ftell(midi_parser->file);

  if(offset < 0){
    return(NULL);
  }

  midi_parser->current_time = 0;
  
  for(; ftell(midi_parser->file) < start_offset + offset; ){
    delta_time = ags_midi_parser_read_varlength(midi_parser);
    midi_parser->current_time += delta_time;
    
    status = ags_midi_parser_midi_getc(midi_parser);
    
    if((0xf0 & (0xf0 & status)) != 0xf0){
      current = ags_midi_parser_channel_message(midi_parser,
						status);

      if(current != NULL){
	xmlNewProp(current,
		   "delta-time\0",
		   g_strdup_printf("%d\0", midi_parser->current_time));
	
	xmlAddChild(node,
		    current);
      }
      
#ifdef DEBUG
      g_message("channel message");
#endif
    }else{
#ifdef DEBUG
      g_message("status message");
#endif
      
      switch(status){
      case 0xf0:
	{
	  /* start of system exclusive */
	  ags_midi_parser_sysex(midi_parser,
				status);

	}
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
#ifdef DEBUG
	  g_message("sysex end\0");
#endif
	}
	break;
      case 0xff:
	{
	  /* meta event */
	  current = ags_midi_parser_meta_event(midi_parser,
					       status);

	  if(current != NULL){
	    xmlNewProp(current,
		       "delta-time\0",
		       g_strdup_printf("%d\0", midi_parser->current_time));
	    
	    xmlAddChild(node,
			current);
	  }
	}
	break;
      default:
	g_warning("bad byte\0");
	break;
      }
    }
  }
  
  return(node);
}

xmlNode*
ags_midi_parser_channel_message(AgsMidiParser *midi_parser, guint status)
{
  xmlNode *node;

  node = NULL;
  
  switch(status & 0xf0){
  case 0x80:
    {
      node = ags_midi_parser_key_off(midi_parser, status);
    }
    break;
  case 0x90:
    {
      node = ags_midi_parser_key_on(midi_parser, status);
    }
    break;
  case 0xa0:
    {
      node = ags_midi_parser_key_pressure(midi_parser, status);
    }
    break;
  case 0xb0:
    {
      node = ags_midi_parser_change_parameter(midi_parser, status);
    }
    break;
  case 0xc0:
    {
      node = ags_midi_parser_change_program(midi_parser, status);
    }
    break;
  case 0xd0:
    {
      node = ags_midi_parser_change_channel_pressure(midi_parser, status);
    }
    break;
  case 0xe0:
    {
      node = ags_midi_parser_change_pitch_bend(midi_parser, status);
    }
    break;
  }

  return(node);
}

xmlNode*  
ags_midi_parser_real_key_on(AgsMidiParser *midi_parser, guint status)
{
  xmlNode *node;
  int channel, note, velocity;

  channel = 0xf & status;
  note = (0x7f) & ags_midi_parser_midi_getc(midi_parser);
  velocity = (0x7f) & ags_midi_parser_midi_getc(midi_parser);

  node = xmlNewNode(NULL,
		    "midi-message\0");

  xmlNewProp(node,
	     AGS_MIDI_EVENT,
	     "note-on\0");

  xmlNewProp(node,
	     "key\0",
	     g_strdup_printf("%d\0", channel));

  xmlNewProp(node,
	     "note\0",
	     g_strdup_printf("%d\0", note));

  xmlNewProp(node,
	     "velocity\0",
	     g_strdup_printf("%d\0", velocity));
  
  return(node);
}

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

  channel = 0xf & status;
  note = (0x7f) & ags_midi_parser_midi_getc(midi_parser);
  velocity = (0x7f) & ags_midi_parser_midi_getc(midi_parser);

  node = xmlNewNode(NULL,
		    "midi-message\0");
  
  xmlNewProp(node,
	     AGS_MIDI_EVENT,
	     "note-off\0");


  xmlNewProp(node,
	     "key\0",
	     g_strdup_printf("%d\0", channel));

  xmlNewProp(node,
	     "note\0",
	     g_strdup_printf("%d\0", note));

  xmlNewProp(node,
	     "velocity\0",
	     g_strdup_printf("%d\0", velocity));
  
  return(node);
}

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

  channel = 0xf & status;
  note = 0x7f & ags_midi_parser_midi_getc(midi_parser);
  pressure = 0x7f & ags_midi_parser_midi_getc(midi_parser);
  
  node = xmlNewNode(NULL,
		    "midi-message\0");

  xmlNewProp(node,
	     AGS_MIDI_EVENT,
	     "polyphonic\0");
	
  xmlNewProp(node,
	     "key\0",
	     g_strdup_printf("%d\0", channel));
  
  xmlNewProp(node,
	     "note\0",
	     g_strdup_printf("%d\0", note));

  xmlNewProp(node,
	     "pressure\0",
	     g_strdup_printf("%d\0", pressure));

  return(node);
}

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

  channel = 0xf & status;
  control = 0x7f & ags_midi_parser_midi_getc(midi_parser);
  value = 0x7f & ags_midi_parser_midi_getc(midi_parser);

  node = xmlNewNode(NULL,
		    "midi-message\0");

  if(control < 120){
    xmlNewProp(node,
	       AGS_MIDI_EVENT,
	       "change-parameter\0");

    xmlNewProp(node,
	       "channel\0",
	       g_strdup_printf("%d\0", channel));

    xmlNewProp(node,
	       "control\0",
	       g_strdup_printf("%d\0", control));

    xmlNewProp(node,
	       "value\0",
	       g_strdup_printf("%d\0", value));
  }else{
    xmlNewProp(node,
	       AGS_MIDI_EVENT,
	       "change-mode\0");

    xmlNewProp(node,
	       "channel\0",
	       g_strdup_printf("%d\0", channel));

    switch(control){
    case 120:
      {
	xmlNewProp(node,
		   "mode\0",
		   "all-sound-off\0");
      }
      break;
    case 121:
      {
	xmlNewProp(node,
		   "mode\0",
		   "reset-control-all\0");
	    
	xmlNewProp(node,
		   "value\0",
		   g_strdup_printf("%d\0", value));
      }
      break;
    case 122:
      {
	if(value == 0){
	  xmlNewProp(node,
		     "mode\0",
		     "local-control-off\0");
	}else{
	  xmlNewProp(node,
		     "mode\0",
		     "local-control-on\0");
	}
      }
      break;
    case 123:
      {
	xmlNewProp(node,
		   "mode\0",
		   "all-notes-off");
      }
      break;
    case 124:
      {
	xmlNewProp(node,
		   "mode\0",
		   "omni-mode-on");
      }
      break;
    case 125:
      {
	xmlNewProp(node,
		   "mode\0",
		   "omni-mode-off\0");
      }
      break;
    case 126:
      {
	xmlNewProp(node,
		   "mode\0",
		   "mono-mode-on\0");

	xmlNewProp(node,
		   "omni-off-channels\0",
		   g_strdup_printf("%d\0", value));
      }
      break;
    case 127:
      {
	xmlNewProp(node,
		   "mode\0",
		   "poly-mode-on\0");
      }
      break;
    }
  }
  
  return(node);
}

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

  channel = 0xf & status;
  pitch = 0x7f & ags_midi_parser_midi_getc(midi_parser);
  transmitter = 0x7f & ags_midi_parser_midi_getc(midi_parser);
  
  node = xmlNewNode(NULL,
		    "midi-message\0");

  xmlNewProp(node,
	     "channel\0",
	     g_strdup_printf("%d\0", channel));

  xmlNewProp(node,
	     AGS_MIDI_EVENT,
	     "pitch-bend\0");

  xmlNewProp(node,
	     "pitch\0",
	     g_strdup_printf("%d\0", pitch));	  

  xmlNewProp(node,
	     "transmitter\0",
	     g_strdup_printf("%d\0", transmitter));	  
  
  return(node);
}

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

  channel = 0xf & status;
  program = 0x7f & ags_midi_parser_midi_getc(midi_parser);
  
  node = xmlNewNode(NULL,
		    "midi-message\0");

  xmlNewProp(node,
	     AGS_MIDI_EVENT,
	     "program-change\0");

  xmlNewProp(node,
	     "channel\0",
	     g_strdup_printf("%d\0", channel));

  xmlNewProp(node,
	     "program\0",
	     g_strdup_printf("%d\0", program));
  
  return(node);
}

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

  channel = 0xf & status;
  pressure = 0x7f & ags_midi_parser_midi_getc(midi_parser);
  
  node = xmlNewNode(NULL,
		    "midi-message\0");

  xmlNewProp(node,
	     AGS_MIDI_EVENT,
	     "channel-pressure\0");

  xmlNewProp(node,
	     "channel\0",
	     g_strdup_printf("%d\0", channel));

  xmlNewProp(node,
	     "pressure\0",
	     g_strdup_printf("%d\0", pressure));
  
  
  return(node);
}

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
  return(NULL);
}

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
	c != EOF);

#ifdef DEBUG
  g_message("discarded sysex\0");
#endif
}

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
  
  switch(status){
  case 0xf1:
    {
      guint quarter_frame;
      
      quarter_frame = 0xff & (ags_midi_parser_midi_getc(midi_parser));
    }
    break;
  case 0xf2:
    {
      guint song_position;
      
      song_position = 0x7f & (ags_midi_parser_midi_getc(midi_parser)) << 7;
      song_position = 0x7f & (ags_midi_parser_midi_getc(midi_parser));
    }
    break;
  case 0xf3:
    {
      guint song_select;
      
      song_select = 0x7f & (ags_midi_parser_midi_getc(midi_parser));
    }
    break;
  case 0xf4:
    {
#ifdef DEBUG
      g_message("undefined\0");
#endif
    }
    break;
  case 0xf5:
    {
#ifdef DEBUG
      g_message("undefined\0");
#endif
    }
    break;
  case 0xf6:
    {
#ifdef DEBUG
      g_message("tune request\0");
#endif
    }
    break;
  }

  return(node);
}

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
  
  meta_type = 0xff & (ags_midi_parser_midi_getc(midi_parser));
  midi_parser->offset += 1;

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
      int c;

      c = ags_midi_parser_midi_getc(midi_parser);

      if(c == 0x0){
	/* End of Track */
	node = ags_midi_parser_end_of_track(midi_parser, meta_type);
      }
    }
    break;
  case 0x51:
    {
      int c;

      c = ags_midi_parser_midi_getc(midi_parser);

      if(c == 0x03){
	/* Set tempo */
	node = ags_midi_parser_tempo(midi_parser, meta_type);
      }
    }
    break;
  case 0x54:
    {
      int c;

      c = ags_midi_parser_midi_getc(midi_parser);

      if(c == 0x05){
	node = ags_midi_parser_smpte(midi_parser, meta_type);
      }
    }
    break;
  case 0x58:
    {
      int c;

      c = ags_midi_parser_midi_getc(midi_parser);
      
      if(c == 0x04){
	node = ags_midi_parser_time_signature(midi_parser, meta_type);
      }
    }
    break;
  case 0x59:
    {
      int c;

      c = ags_midi_parser_midi_getc(midi_parser);

      if(c == 0x02){
	node = ags_midi_parser_key_signature(midi_parser, meta_type);
      }
    }
    break;
  case 0x7f:
    {
      node = ags_midi_parser_sequencer_meta_event(midi_parser, meta_type);
    }
    break;
  default:
    {
      node = xmlNewNode(NULL,
			"midi-message\0");
      xmlNewProp(node,
		 AGS_MIDI_EVENT,
		 "misc\0");
    }
  }
	
#ifdef DEBUG
  g_message("meta type 0x%x\0", meta_type);
#endif
  
  return(node);
}

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
  
  node = xmlNewNode(NULL,
		    "midi-message\0");

  sequence = (guint) ags_midi_parser_read_gint16(midi_parser);
  
  xmlNewProp(node,
	     AGS_MIDI_EVENT,
	     "sequence-number\0");
  
  xmlNewProp(node,
	     "sequence\0",
	     g_strdup_printf("%d\0", sequence));
    
  return(node);
}

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

  node = xmlNewNode(NULL,
		    "midi-message\0");

  xmlNewProp(node,
	     AGS_MIDI_EVENT,
	     "end-of-track\0");
  
  return(node);
}

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
ags_midi_parser_real_smpte(AgsMidiParser *midi_parser, guint meta_type)
{
  xmlNode *node;
  int hr, mn, se, fr, ff;
  
  node = xmlNewNode(NULL,
		    "midi-message\0");

  hr = ags_midi_parser_midi_getc(midi_parser);
  mn = ags_midi_parser_midi_getc(midi_parser);
  se = ags_midi_parser_midi_getc(midi_parser);
  fr = ags_midi_parser_midi_getc(midi_parser);
  ff = ags_midi_parser_midi_getc(midi_parser);
  
  xmlNewProp(node,
	     AGS_MIDI_EVENT,
	     "smpte\0");
  
  xmlNewProp(node,
	     "timestamp\0",
	     g_strdup_printf("%d %d %d %d %d\0", hr, mn, se, fr, ff));
  
  return(node);
}

xmlNode*  
ags_midi_parser_smpte(AgsMidiParser *midi_parser, guint meta_type)
{
  xmlNode *node;
  
  g_return_val_if_fail(AGS_IS_MIDI_PARSER(midi_parser), NULL);
  
  g_object_ref((GObject *) midi_parser);
  g_signal_emit(G_OBJECT(midi_parser),
		midi_parser_signals[SMPTE], 0,
		meta_type,
		&node);
  g_object_unref((GObject *) midi_parser);

  return(node);
}

xmlNode*  
ags_midi_parser_real_tempo(AgsMidiParser *midi_parser, guint meta_type)
{
  xmlNode *node;
  gint tempo;
  
  node = xmlNewNode(NULL,
		    "midi-message\0");

  tempo = ags_midi_parser_read_gint24(midi_parser);
  
  xmlNewProp(node,
	     AGS_MIDI_EVENT,
	     "tempo-number\0");
  
  xmlNewProp(node,
	     "tempo\0",
	     g_strdup_printf("%ld\0", tempo));
  
  return(node);
}

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
  int nn, dd, cc, bb;
  int denom = 1;
  
  node = xmlNewNode(NULL,
		    "midi-message\0");

  nn = ags_midi_parser_midi_getc(midi_parser);
  dd = ags_midi_parser_midi_getc(midi_parser);
  cc = ags_midi_parser_midi_getc(midi_parser);
  bb = ags_midi_parser_midi_getc(midi_parser);

  while(dd-- > 0){
    denom *= 2;
  }

  xmlNewProp(node,
	     AGS_MIDI_EVENT,
	     "time-signature\0");

  xmlNewProp(node,
	     "timesig\0",
	     g_strdup_printf("%d/%d %d %d\0", nn, denom, cc, bb));

  return(node);
}

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
  int sf, mi;
  
  node = xmlNewNode(NULL,
		    "midi-message\0");

  xmlNewProp(node,
	     AGS_MIDI_EVENT,
	     "key-signature\0");
  
  sf = ags_midi_parser_midi_getc(midi_parser);
  mi = ags_midi_parser_midi_getc(midi_parser);

  xmlNewProp(node,
	     "keysig\0",
	     g_strdup_printf("%d %s\0", (sf>127?sf-256:sf), (mi?"minor":"major")));
  
  return(node);
}

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

  node = NULL;

  len = ags_midi_parser_midi_getc(midi_parser);
  id = ags_midi_parser_midi_getc(midi_parser);
  data = ags_midi_parser_midi_getc(midi_parser);
  
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

  node = xmlNewNode(NULL,
		    "midi-message\0");

  text_length = ags_midi_parser_read_varlength(midi_parser);
  text = ags_midi_parser_read_text(midi_parser,
				   text_length);
  
  switch(0x0f & meta_type){
  case 0x01:      /* Text event */
    break;
  case 0x02:
    {
      /* Copyright notice */
      xmlNewProp(node,
		 "copyright\0",
		 text);
    }
    break;
  case 0x03:
    {
      /* Sequence/Track name */
      xmlNewProp(node,
		 "sequence-name\0",
		 text);
    }
    break;
  case 0x04:
    {
      /* Instrument name */
      xmlNewProp(node,
		 "instrument-name\0",
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
    g_warning("unknown text event\0");
  }
    
  return(node);
}

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
					       "file\0", file,
					       NULL);

  
  return(midi_parser);
}

