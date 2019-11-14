/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
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

#ifndef __AGS_LV2_TURTLE_PARSER_H__
#define __AGS_LV2_TURTLE_PARSER_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

G_BEGIN_DECLS

#define AGS_TYPE_LV2_TURTLE_PARSER                (ags_lv2_turtle_parser_get_type())
#define AGS_LV2_TURTLE_PARSER(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_LV2_TURTLE_PARSER, AgsLv2TurtleParser))
#define AGS_LV2_TURTLE_PARSER_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_LV2_TURTLE_PARSER, AgsLv2TurtleParserClass))
#define AGS_IS_LV2_TURTLE_PARSER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_LV2_TURTLE_PARSER))
#define AGS_IS_LV2_TURTLE_PARSER_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_LV2_TURTLE_PARSER))
#define AGS_LV2_TURTLE_PARSER_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_LV2_TURTLE_PARSER, AgsLv2TurtleParserClass))

#define AGS_LV2_TURTLE_PARSER_GET_OBJ_MUTEX(obj) (&(((AgsLv2TurtleParser *) obj)->obj_mutex))

typedef struct _AgsLv2TurtleParser AgsLv2TurtleParser;
typedef struct _AgsLv2TurtleParserClass AgsLv2TurtleParserClass;

struct _AgsLv2TurtleParser
{
  GObject gobject;

  guint flags;
  
  GRecMutex obj_mutex;

  GList *turtle;

  GList *plugin;
  GList *ui_plugin;
  GList *preset;
};

struct _AgsLv2TurtleParserClass
{
  GObjectClass gobject;
};

GType ags_lv2_turtle_parser_get_type(void);

pthread_mutex_t* ags_lv2_turtle_parser_get_class_mutex();

void ags_lv2_turtle_parser_parse_names(AgsLv2TurtleParser *lv2_turtle_parser,
				       AgsTurtle **turtle, guint n_turtle);

void ags_lv2_turtle_parser_parse(AgsLv2TurtleParser *lv2_turtle_parser,
				 AgsTurtle **turtle, guint n_turtle);

AgsLv2TurtleParser* ags_lv2_turtle_parser_new(AgsTurtle *manifest);

G_END_DECLS

#endif /*__AGS_LV2_TURTLE_PARSER_H__*/
