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

#ifndef __AGS_TURTLE_H__
#define __AGS_TURTLE_H__

#include <glib.h>
#include <glib-object.h>

#define AGS_TYPE_TURTLE                (ags_turtle_get_type())
#define AGS_TURTLE(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_TURTLE, AgsTurtle))
#define AGS_TURTLE_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_TURTLE, AgsTurtleClass))
#define AGS_IS_TURTLE(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_TURTLE))
#define AGS_IS_TURTLE_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_TURTLE))
#define AGS_TURTLE_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_TURTLE, AgsTurtleClass))

typedef struct _AgsTurtle AgsTurtle;
typedef struct _AgsTurtleClass AgsTurtleClass;

struct _AgsTurtle
{
  GObject object;

  gchar *filename;

  gchar **subject;
  
  gchar **filter;
  GHashMap *hashmap;
};

struct _AgsTurtleClass
{
  GObjectClass object;
};

GType ags_turtle_get_type(void);

gboolean ags_turtle_insert(AgsTurtle *turtle,
			   gchar *key, gchar *value);
gboolean ags_turtle_remove(AgsTurtle *turtle,
			   gchar *key);

gchar* ags_turtle_lookup(AgsTurtle *turtle,
			 gchar *key);

gchar* ags_turtle_value_as_string(AgsTurtle *turtle,
				  gchar *value,
				  gchar **verb);
gchar* ags_turtle_value_with_verb_as_string(AgsTurtle *turtle,
					    gchar *value,
					    gchar *verb);

gchar** ags_turtle_value_as_array(AgsTurtle *turtle,
				  gchar *value,
				  gchar **verb);
gchar** ags_turtle_value_with_verb_as_array(AgsTurtle *turtle,
					    gchar *value,
					    gchar *verb);

gchar** ags_turtle_list_subjects(AgsTurtle *turtle);
void ags_turtle_load(AgsTurtle *turtle);

AgsTurtle* ags_turtle_new(gchar *filename,
			  gchar **filter);

#endif /*__AGS_TURTLE_H__*/
