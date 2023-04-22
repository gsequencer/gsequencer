/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2022 Joël Krähemann
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

#ifndef __AGS_MIDI_PARSER_H__
#define __AGS_MIDI_PARSER_H__

#include <glib.h>
#include <glib-object.h>

#include <libxml/tree.h>

#include <ags/libags.h>

G_BEGIN_DECLS

#define AGS_TYPE_MIDI_PARSER                (ags_midi_parser_get_type ())
#define AGS_MIDI_PARSER(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_MIDI_PARSER, AgsMidiParser))
#define AGS_MIDI_PARSER_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_MIDI_PARSER, AgsMidiParserClass))
#define AGS_IS_MIDI_PARSER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_MIDI_PARSER))
#define AGS_IS_MIDI_PARSER_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_MIDI_PARSER))
#define AGS_MIDI_PARSER_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_MIDI_PARSER, AgsMidiParserClass))

#define AGS_MIDI_PARSER_GET_OBJ_MUTEX(obj) (&(((AgsMidiParser *) obj)->obj_mutex))

#define AGS_MIDI_PARSER_MAX_TEXT_LENGTH (4096)

#define AGS_MIDI_PARSER_MTHD "MThd"
#define AGS_MIDI_PARSER_MTCK "MTrk"

#define AGS_MIDI_EVENT "event"

typedef struct _AgsMidiParser AgsMidiParser;
typedef struct _AgsMidiParserClass AgsMidiParserClass;

typedef enum{
  AGS_MIDI_PARSER_EOF  = 1,
  AGS_MIDI_PARSER_EOT  = 1 << 1,
}AgsMidiParserFlags;

typedef enum{
  AGS_MIDI_CHUNK_HEADER   = 1,
  AGS_MIDI_CHUNK_TRACK    = 1 << 1,
  AGS_MIDI_CHUNK_UNKNOWN  = 1 << 2,
}AgsMidiChunkFlags;

struct _AgsMidiParser
{
  GObject gobject;

  AgsMidiParserFlags flags;

  GRecMutex obj_mutex;

  FILE *file;
  guint nth_chunk;

  guchar *buffer;
  
  size_t file_length;
  size_t offset;

  guint current_time;
  guchar current_status;
  
  xmlDoc *doc;
};

struct _AgsMidiParserClass
{
  GObjectClass gobject;

  int (*midi_getc)(AgsMidiParser *midi_parser);
  void (*on_error)(AgsMidiParser *midi_parser,
		   GError **error);
  
  xmlDoc* (*parse_full)(AgsMidiParser *midi_parser);
  xmlNode* (*parse_bytes)(AgsMidiParser *midi_parser,
			  guchar *midi_buffer,
			  guint buffer_length);
  
  xmlNode* (*parse_header)(AgsMidiParser *midi_parser);
  xmlNode* (*parse_track)(AgsMidiParser *midi_parser);
  
  xmlNode* (*key_on)(AgsMidiParser *midi_parser, guint status);
  xmlNode* (*key_off)(AgsMidiParser *midi_parser, guint status);
  xmlNode* (*key_pressure)(AgsMidiParser *midi_parser, guint status);

  xmlNode* (*change_parameter)(AgsMidiParser *midi_parser, guint status);
  xmlNode* (*change_pitch_bend)(AgsMidiParser *midi_parser, guint status);
  xmlNode* (*change_program)(AgsMidiParser *midi_parser, guint status);
  xmlNode* (*change_channel_pressure)(AgsMidiParser *midi_parser, guint status);
  xmlNode* (*not_defined)(AgsMidiParser *midi_parser, guint status);

  xmlNode* (*sysex)(AgsMidiParser *midi_parser, guint status);
  xmlNode* (*system_common)(AgsMidiParser *midi_parser, guint status);

  xmlNode* (*meta_event)(AgsMidiParser *midi_parser, guint status);
  xmlNode* (*sequence_number)(AgsMidiParser *midi_parser, guint meta_type);
  xmlNode* (*end_of_track)(AgsMidiParser *midi_parser, guint meta_type);
  xmlNode* (*smtpe)(AgsMidiParser *midi_parser, guint meta_type);
  xmlNode* (*tempo)(AgsMidiParser *midi_parser, guint meta_type);
  xmlNode* (*time_signature)(AgsMidiParser *midi_parser, guint meta_type);
  xmlNode* (*key_signature)(AgsMidiParser *midi_parser, guint meta_type);
  xmlNode* (*sequencer_meta_event)(AgsMidiParser *midi_parser, guint meta_type);
  xmlNode* (*text_event)(AgsMidiParser *midi_parser, guint meta_type);
};

GType ags_midi_parser_get_type(void);

gint16 ags_midi_parser_read_gint16(AgsMidiParser *midi_parser);
gint32 ags_midi_parser_read_gint24(AgsMidiParser *midi_parser);
gint32 ags_midi_parser_read_gint32(AgsMidiParser *midi_parser);
long ags_midi_parser_read_varlength(AgsMidiParser *midi_parser);

gchar* ags_midi_parser_read_text(AgsMidiParser *midi_parser,
				 gint length);

gdouble ags_midi_parser_ticks_to_sec(AgsMidiParser *midi_parser,
				     guint ticks, gint division, guint tempo);

int ags_midi_parser_midi_getc(AgsMidiParser *midi_parser);
void ags_midi_parser_on_error(AgsMidiParser *midi_parser,
			      GError **error);

xmlDoc* ags_midi_parser_parse_full(AgsMidiParser *midi_parser);
xmlNode* ags_midi_parser_parse_bytes(AgsMidiParser *midi_parser,
				     guchar *midi_buffer,
				     guint buffer_length);

xmlNode* ags_midi_parser_parse_header(AgsMidiParser *midi_parser);
xmlNode* ags_midi_parser_parse_track(AgsMidiParser *midi_parser);

xmlNode* ags_midi_parser_channel_message(AgsMidiParser *midi_parser, guint status);
xmlNode* ags_midi_parser_key_on(AgsMidiParser *midi_parser, guint status);
xmlNode* ags_midi_parser_key_off(AgsMidiParser *midi_parser, guint status);
xmlNode* ags_midi_parser_key_pressure(AgsMidiParser *midi_parser, guint status);
xmlNode* ags_midi_parser_change_parameter(AgsMidiParser *midi_parser, guint status);
xmlNode* ags_midi_parser_change_pitch_bend(AgsMidiParser *midi_parser, guint status);
xmlNode* ags_midi_parser_change_program(AgsMidiParser *midi_parser, guint status);
xmlNode* ags_midi_parser_change_channel_pressure(AgsMidiParser *midi_parser, guint status);
xmlNode* ags_midi_parser_not_defined(AgsMidiParser *midi_parser, guint status);

xmlNode* ags_midi_parser_sysex(AgsMidiParser *midi_parser, guint status);
xmlNode* ags_midi_parser_system_common(AgsMidiParser *midi_parser, guint status);

xmlNode* ags_midi_parser_meta_event(AgsMidiParser *midi_parser, guint status);
xmlNode* ags_midi_parser_sequence_number(AgsMidiParser *midi_parser, guint meta_type);
xmlNode* ags_midi_parser_end_of_track(AgsMidiParser *midi_parser, guint meta_type);
xmlNode* ags_midi_parser_smtpe(AgsMidiParser *midi_parser, guint meta_type);
xmlNode* ags_midi_parser_tempo(AgsMidiParser *midi_parser, guint meta_type);
xmlNode* ags_midi_parser_time_signature(AgsMidiParser *midi_parser, guint meta_type);
xmlNode* ags_midi_parser_key_signature(AgsMidiParser *midi_parser, guint meta_type);
xmlNode* ags_midi_parser_sequencer_meta_event(AgsMidiParser *midi_parser, guint meta_type);
xmlNode* ags_midi_parser_meta_misc(AgsMidiParser *midi_parser, guint meta_type);
xmlNode* ags_midi_parser_text_event(AgsMidiParser *midi_parser, guint meta_type);

/*  */
void ags_midi_parser_open_filename(AgsMidiParser *midi_parser,
				   gchar *filename);

void ags_midi_parser_set_buffer(AgsMidiParser *midi_parser,
				guchar *buffer);

void ags_midi_parser_set_file_length(AgsMidiParser *midi_parser,
				     size_t file_length);

/* instantiate */
AgsMidiParser* ags_midi_parser_new();
AgsMidiParser* ags_midi_parser_new_from_filename(gchar *filename);

G_END_DECLS

#endif /*__AGS_MIDI_PARSER_H__*/
