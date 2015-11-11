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

#ifndef __AGS_TURTLE_H__
#define __AGS_TURTLE_H__

#include <glib.h>
#include <glib-object.h>

#include <libxml/tree.h>

#define AGS_TYPE_TURTLE                (ags_turtle_get_type())
#define AGS_TURTLE(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_TURTLE, AgsTurtle))
#define AGS_TURTLE_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_TURTLE, AgsTurtleClass))
#define AGS_IS_TURTLE(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_TURTLE))
#define AGS_IS_TURTLE_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_TURTLE))
#define AGS_TURTLE_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_TURTLE, AgsTurtleClass))

#define AGS_TURTLE_DEFAULT_ENCODING "UTF-8\0"

#define AGS_TURTLE_STRING_LITERAL_QUOTE "\"\0"
#define AGS_TURTLE_STRING_LITERAL_SINGLE_QUOTE "'\0"
#define AGS_TURTLE_STRING_LITERAL_LONG_QUOTE "\"\"\"\0"
#define AGS_TURTLE_STRING_LITERAL_LONG_SINGLE_QUOTE "'''\0"

typedef struct _AgsTurtle AgsTurtle;
typedef struct _AgsTurtleClass AgsTurtleClass;
typedef struct _AgsTurtleParserContext AgsTurtleParserContext;

typedef enum{
  AGS_TURTLE_READ_SUBJECT    = 1,
  AGS_TURTLE_READ_VERB       = 1 << 1,
  AGS_TURTLE_READ_OBJECT     = 1 << 2,
  AGS_TURTLE_TRIPLE_END      = 1 << 3,
}AgsTurtleFlags;

struct _AgsTurtle
{
  GObject object;

  guint flags;
  
  gchar *filename;

  GList *parser_context;
  
  xmlDoc *doc;
};

struct _AgsTurtleClass
{
  GObjectClass object;
};

struct _AgsTurtleParserContext
{
  gchar *subject_delimiter;
  gchar *verb_delimiter;
  gchar *value_delimiter;
  
  xmlNode *statement_node;
};

GType ags_turtle_get_type(void);

AgsTurtleParserContext* ags_turtle_parser_context_alloc();
void ags_turtle_parser_context_free(AgsTurtleParserContext *parser_context);

GList* ags_turtle_find_xpath(AgsTurtle *turtle,
			     gchar *xpath);
void ags_turtle_load(AgsTurtle *turtle,
		     GError **error);

AgsTurtle* ags_turtle_new(gchar *filename);

#endif /*__AGS_TURTLE_H__*/
