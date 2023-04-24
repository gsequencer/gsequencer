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

#ifndef __AGS_OSC_PARSER_H__
#define __AGS_OSC_PARSER_H__

#include <glib.h>
#include <glib-object.h>

#include <libxml/tree.h>

#include <ags/libags.h>

G_BEGIN_DECLS

#define AGS_TYPE_OSC_PARSER                (ags_osc_parser_get_type ())
#define AGS_OSC_PARSER(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_OSC_PARSER, AgsOscParser))
#define AGS_OSC_PARSER_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_OSC_PARSER, AgsOscParserClass))
#define AGS_IS_OSC_PARSER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_OSC_PARSER))
#define AGS_IS_OSC_PARSER_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_OSC_PARSER))
#define AGS_OSC_PARSER_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_OSC_PARSER, AgsOscParserClass))

#define AGS_OSC_PARSER_GET_OBJ_MUTEX(obj) (&(((AgsOscParser *) obj)->obj_mutex))

#define AGS_OSC_PARSER_MAX_TEXT_LENGTH (4096)

typedef struct _AgsOscParser AgsOscParser;
typedef struct _AgsOscParserClass AgsOscParserClass;

/**
 * AgsOscParserFlags:
 * @AGS_OSC_PARSER_EOF: the parser reached EOF
 * 
 * Enum values to control the behavior or indicate internal state of #AgsOscParser by
 * enable/disable as flags.
 */
typedef enum{
  AGS_OSC_PARSER_EOF        = 1,
}AgsOscParserFlags;

struct _AgsOscParser
{
  GObject gobject;

  AgsOscParserFlags flags;

  GRecMutex obj_mutex;

  guchar *buffer;
  
  size_t file_length;
  size_t offset;

  gsize start_offset;
  gint32 packet_size;
  
  xmlDoc *doc;
};

struct _AgsOscParserClass
{
  GObjectClass gobject;

  int (*osc_getc)(AgsOscParser *osc_parser);
  void (*on_error)(AgsOscParser *osc_parser,
		   GError **error);

  xmlDoc* (*parse_full)(AgsOscParser *osc_parser);
  xmlNode* (*parse_bytes)(AgsOscParser *osc_parser,
			  guchar *osc_buffer,
			  guint buffer_length);

  xmlNode* (*packet)(AgsOscParser *osc_parser);

  xmlNode* (*bundle)(AgsOscParser *osc_parser);

  xmlNode* (*message)(AgsOscParser *osc_parser);

  xmlNode* (*value)(AgsOscParser *osc_parser,
		    guint v_type);
};

GType ags_osc_parser_get_type(void);

gint32 ags_osc_parser_read_gint32(AgsOscParser *osc_parser);
gint64 ags_osc_parser_read_gint64(AgsOscParser *osc_parser);

gfloat ags_osc_parser_read_gfloat(AgsOscParser *osc_parser);
gdouble ags_osc_parser_read_gdouble(AgsOscParser *osc_parser);

gchar* ags_osc_parser_read_text(AgsOscParser *osc_parser,
				gint length);

int ags_osc_parser_osc_getc(AgsOscParser *osc_parser);
void ags_osc_parser_on_error(AgsOscParser *osc_parser,
			     GError **error);

xmlDoc* ags_osc_parser_parse_full(AgsOscParser *osc_parser);
xmlNode* ags_osc_parser_parse_bytes(AgsOscParser *osc_parser,
				    guchar *osc_buffer,
				    guint buffer_length);

xmlNode* ags_osc_parser_packet(AgsOscParser *osc_parser);

xmlNode* ags_osc_parser_bundle(AgsOscParser *osc_parser);

xmlNode* ags_osc_parser_message(AgsOscParser *osc_parser);

xmlNode* ags_osc_parser_value(AgsOscParser *osc_parser,
			      guint v_type);

AgsOscParser* ags_osc_parser_new();

G_END_DECLS

#endif /*__AGS_OSC_PARSER_H__*/
