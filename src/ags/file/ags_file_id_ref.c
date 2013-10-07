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

#include <ags/audio/ags_file_id_ref.h>

void ags_file_id_ref_class_init(AgsFileIdRefClass *file_id_ref);
void ags_file_id_ref_init(AgsFileIdRef *file_id_ref);
void ags_file_id_ref_set_property(GObject *gobject,
				  guint prop_id,
				  const GValue *value,
				  GParamSpec *param_spec);
void ags_file_id_ref_get_property(GObject *gobject,
				  guint prop_id,
				  GValue *value,
				  GParamSpec *param_spec);
void ags_file_id_ref_finalize(GObject *gobject);

enum{
  PROP_0,
  PROP_XPATH,
  PROP_REFERENCE,
};

static gpointer ags_file_id_ref_parent_class = NULL;

GType
ags_file_id_ref_get_type (void)
{
  static GType ags_type_file_id_ref = 0;

  if(!ags_type_file_id_ref){
    static const GTypeInfo ags_file_id_ref_info = {
      sizeof (AgsFileIdRefClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_file_id_ref_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsFileIdRef),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_file_id_ref_init,
    };

    ags_type_file_id_ref = g_type_register_static(G_TYPE_OBJECT,
					     "AgsFileIdRef\0",
					     &ags_file_id_ref_info,
					     0);
  }

  return (ags_type_file_id_ref);
}

void
ags_file_id_ref_class_init(AgsFileIdRefClass *file_id_ref)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_file_id_ref_parent_class = g_type_class_peek_parent(file_id_ref);

  /* GObjectClass */
  gobject = (GObjectClass *) file_id_ref;

  gobject->set_property = ags_file_id_ref_set_property;
  gobject->get_property = ags_file_id_ref_get_property;

  gobject->finalize = ags_file_id_ref_finalize;

  /* properties */
  param_spec = g_param_spec_string("xpath\0",
				   "the xpath\0",
				   "The xpath to find the element\0",
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_XPATH,
				  param_spec);

  param_spec = g_param_spec_object("reference\0",
				   "reference of the locator\0",
				   "The reference resulted by the xpath locator\0",
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_REF,
				  param_spec);
}

void
ags_file_id_ref_init(AgsFileIdRef *file_id_ref)
{
  file_id_ref->xpath = NULL;
  file_id_ref->ref = NULL;
}


void
ags_file_id_ref_set_property(GObject *gobject,
			     guint prop_id,
			     const GValue *value,
			     GParamSpec *param_spec)
{
  AgsFileIdRef *file_id_ref;

  file_id_ref = AGS_FILE_ID_REF(gobject);
  
  switch(prop_id){
  case PROP_XPATH:
    {
      char *xpath;

      xpath = (char *) g_value_get_string(value);

      if(file_id_ref->xpath != NULL)
	g_free(file_id_ref->xpath);

      file_id_ref->xpath = xpath;
    }
    break;
  case PROP_REFERENCE:
    {
      GObject *ref;

      ref = (GObject *) g_value_get_object(value);

      if(file_id_ref->ref != NULL)
	g_object_unref(file_id_ref->ref);

      if(ref != NULL)
	g_object_ref(ref);

      file_id_ref->ref = ref;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_file_id_ref_get_property(GObject *gobject,
			     guint prop_id,
			     GValue *value,
			     GParamSpec *param_spec)
{
  AgsFileIdRef *file_id_ref;

  file_id_ref = AGS_FILE_ID_REF(gobject);
  
  switch(prop_id){
  case PROP_DEVICE:
    {
      g_value_set_string(value, file_id_ref->xpath);
    }
  case PROP_REFERENCE:
    {
      g_value_set_object(value, file_id_ref->reference);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_file_id_ref_finalize(GObject *gobject)
{
  if(file_id_ref->xpath != NULL){
    g_free(file_id_ref->xpath);
  }

  if(file_id_ref->ref){
    g_object_unref(file_id_ref->ref);
  }
}

AgsFileIdRef*
ags_file_id_ref_new(gchar *xpath)
{
  AgsFileIdRef *file_id_ref;

  file_id_ref = (AgsFileIdRef *) g_object_new(AGS_TYPE_FILE_ID_REF,
					      "xpath\0", xpath,
					      NULL);

  return(file_id_ref);
}
