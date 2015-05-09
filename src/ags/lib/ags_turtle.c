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

#include <ags/lib/ags_turtle.h>

void ags_turtle_class_init(AgsTurtleClass *turtle);
void ags_turtle_init (AgsTurtle *turtle);
void ags_turtle_finalize(GObject *gobject);

enum{
  PROP_0,
  PROP_FILENAME,
  PROP_FILTER,
};

static gpointer ags_turtle_parent_class = NULL;

GType
ags_turtle_get_type(void)
{
  static GType ags_type_turtle = 0;

  if(!ags_type_turtle){
    static const GTypeInfo ags_turtle_info = {
      sizeof (AgsTurtleClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_turtle_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsTurtle),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_turtle_init,
    };

    ags_type_turtle = g_type_register_static(G_TYPE_OBJECT,
					     "AgsTurtle\0",
					     &ags_turtle_info,
					     0);
  }

  return (ags_type_turtle);
}

void
ags_turtle_class_init(AgsTurtleClass *turtle)
{
  GObjectClass *gobject;

  ags_turtle_parent_class = g_type_class_peek_parent(turtle);

  /* GObjectClass */
  gobject = (GObjectClass *) turtle;

  gobject->finalize = ags_turtle_finalize;
}

void
ags_turtle_init(AgsTurtle *turtle)
{
  turtle->filename = NULL;
  
  turtle->subject = NULL;

  turtle->filter = NULL;
  turtle->hash_table = g_hash_table_new_full(g_str_hash, (GEqualFunc) g_ascii_strcasecmp,
					     (GDestroyNotify) g_free,
					     (GDestroyNotify) g_free);
}

void
ags_turtle_finalize(GObject *gobject)
{
  AgsTurtle *turtle;
  gchar **str;
  
  turtle = AGS_TURTLE(gobject);

  g_strfreev(turtle->subject);

  g_strfreev(turtle->filter);
  g_hash_table_destroy(turtle->hash_table);
}

gboolean
ags_turtle_insert(AgsTurtle *turtle,
		  gchar *key, gchar *value)
{
  return(g_hash_table_insert(turtle->hash_table,
			     key, value));
}

gboolean
ags_turtle_remove(AgsTurtle *turtle,
		  gchar *key)
{
  g_hash_table_remove(turtle->hash_table,
		      key);
  
  return(TRUE);
}

gchar*
ags_turtle_lookup(AgsTurtle *turtle,
		  gchar *key)
{
  gchar *value;

  value = (gchar *) g_hash_table_lookup(turtle->hash_table,
					key);

  return(value);
}

gchar*
ags_turtle_value_as_string(AgsTurtle *turtle,
			   gchar *value,
			   gchar **verb)
{
  //TODO:JK: implement me

  return(NULL);
}

gchar*
ags_turtle_value_with_verb_as_string(AgsTurtle *turtle,
				     gchar *value,
				     gchar *verb)
{
  //TODO:JK: implement me

  return(NULL);
}

gchar**
ags_turtle_value_as_array(AgsTurtle *turtle,
			  gchar *value,
			  gchar **verbd)
{
  //TODO:JK: implement me

  return(NULL);
}

gchar**
ags_turtle_value_with_verb_as_array(AgsTurtle *turtle,
				    gchar *value,
				    gchar *verb)
{
  //TODO:JK: implement me

  return(NULL);
}

gchar**
ags_turtle_list_subjects(AgsTurtle *turtle)
{
  //TODO:JK: implement me

  return(NULL);
}

void
ags_turtle_load(AgsTurtle *turtle)
{
  auto void ags_turtle_load_triple(gchar *current_path,
				   gchar **ret_key, gchar **ret_value);

  void ags_turtle_load_triple(gchar *current_path,
			      gchar **ret_key, gchar **ret_value){
    //TODO:JK: implement me
  }
  
  //TODO:JK: implement me
}

AgsTurtle*
ags_turtle_new(gchar *filename,
	       gchar **filter)
{
  AgsTurtle *turtle;
  gchar **str;
  
  turtle = g_object_new(AGS_TYPE_TURTLE,
			"filename\0", filename,
			NULL);

  if(filter != NULL){
    str = filter;

    while(*str != NULL){
      g_object_set(turtle,
		   "filter\0", str,
		   NULL);
      str++;
    }
  }
  
  return(turtle);
}
