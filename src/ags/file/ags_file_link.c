/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2013 Joël Krähemann
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

#include <ags/file/ags_file_link.h>

void ags_file_link_class_init(AgsFileLinkClass *file_link);
void ags_file_link_init(AgsFileLink *file_link);
void ags_file_link_set_property(GObject *gobject,
				guint prop_id,
				const GValue *value,
				GParamSpec *param_spec);
void ags_file_link_get_property(GObject *gobject,
				guint prop_id,
				GValue *value,
				GParamSpec *param_spec);
void ags_file_link_finalize(GObject *gobject);

enum{
  PROP_0,
  PROP_URL,
  PROP_DATA,
  PROP_TIMESTAMP,
};

static gpointer ags_file_link_parent_class = NULL;

GType
ags_file_link_get_type()
{
  static GType ags_type_file_link = 0;

  if(!ags_type_file_link){
    static const GTypeInfo ags_file_link_info = {
      sizeof (AgsFileLinkClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_file_link_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsFileLink),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_file_link_init,
    };

    ags_type_file_link = g_type_register_static(G_TYPE_OBJECT,
						"AgsFileLink\0",
						&ags_file_link_info,
						0);
  }

  return (ags_type_file_link);
}

void
ags_file_link_class_init(AgsFileLinkClass *file_link)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_file_link_parent_class = g_type_class_peek_parent(file_link);

  /* GObjectClass */
  gobject = (GObjectClass *) file_link;

  gobject->set_property = ags_file_link_set_property;
  gobject->get_property = ags_file_link_get_property;

  gobject->finalize = ags_file_link_finalize;

  /* properties */
  param_spec = g_param_spec_string("url\0",
				   "the URL\0",
				   "The URL to locate the file\0",
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_URL,
				  param_spec);

  param_spec = g_param_spec_string("data\0",
				   "the data\0",
				   "The embedded data\0",
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DATA,
				  param_spec);

  param_spec = g_param_spec_object("timestamp\0",
				   "timestamp\0",
				   "The timestamp\0",
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_TIMESTAMP,
				  param_spec);
}

void
ags_file_link_init(AgsFileLink *file_link)
{
  file_link->url = NULL;
  file_link->data = NULL;
  file_link->timestamp = NULL;
}

void
ags_file_link_set_property(GObject *gobject,
			   guint prop_id,
			   const GValue *value,
			   GParamSpec *param_spec)
{
  AgsFileLink *file_link;

  file_link = AGS_FILE_LINK(gobject);
  
  switch(prop_id){
  case PROP_URL:
    {
      char *url;

      url = (char *) g_value_get_string(value);

      if(file_link->url != NULL)
	g_free(file_link->url);

      file_link->url = url;
    }
    break;
  case PROP_DATA:
    {
      char *data;

      data = (char *) g_value_get_string(value);

      if(file_link->data != NULL)
	g_free(file_link->data);

      file_link->data = data;
    }
    break;
  case PROP_TIMESTAMP:
    {
      GObject *timestamp;

      timestamp = (GObject *) g_value_get_object(value);

      if(file_link->timestamp != NULL)
	g_object_unref(file_link->timestamp);

      if(timestamp != NULL)
	g_object_ref(timestamp);

      file_link->timestamp = timestamp;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_file_link_get_property(GObject *gobject,
			   guint prop_id,
			   GValue *value,
			   GParamSpec *param_spec)
{
  AgsFileLink *file_link;

  file_link = AGS_FILE_LINK(gobject);
  
  switch(prop_id){
  case PROP_URL:
    {
      g_value_set_string(value, file_link->url);
    }
  case PROP_DATA:
    {
      g_value_set_string(value, file_link->data);
    }
  case PROP_TIMESTAMP:
    {
      g_value_set_object(value, file_link->timestamp);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_file_link_finalize(GObject *gobject)
{
  AgsFileLink *file_link;

  file_link = AGS_FILE_LINK(gobject);

  if(file_link->timestamp != NULL){
    g_object_unref(file_link->timestamp);
  }
}

AgsFileLink*
ags_file_link_new()
{
  AgsFileLink *file_link;

  file_link = (AgsFileLink *) g_object_new(AGS_TYPE_FILE_LINK,
					   NULL);

  return(file_link);
}
