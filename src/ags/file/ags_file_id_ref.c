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

#include <ags/file/ags_file_id_ref.h>

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

/**
 * SECTION:ags_file_id_ref
 * @short_description: file id reference
 * @title: AgsFileIdRef
 * @section_id:
 * @include: ags/file/ags_file_id_ref.h
 *
 * The #AgsFileIdRef referes a XML node by its ID. It's used
 * to perform XPath lookup.
 */

enum{
  RESOLVED,
  LAST_SIGNAL,
};

enum{
  PROP_0,
  PROP_NODE,
  PROP_XPATH,
  PROP_REFERENCE,
  PROP_FILE,
  PROP_MAIN,
};

static gpointer ags_file_id_ref_parent_class = NULL;

static guint file_id_ref_signals[LAST_SIGNAL];

GType
ags_file_id_ref_get_type()
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
  param_spec = g_param_spec_pointer("node\0",
				    "the node\0",
				    "The node to find the element\0",
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_NODE,
				  param_spec);

  param_spec = g_param_spec_string("xpath\0",
				   "the xpath\0",
				   "The xpath to find the element\0",
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_XPATH,
				  param_spec);

  param_spec = g_param_spec_pointer("reference\0",
				    "reference of the locator\0",
				    "The reference resulted by the xpath locator\0",
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_REFERENCE,
				  param_spec);

  param_spec = g_param_spec_object("file\0",
				   "file assigned to\0",
				   "The entire file assigned to\0",
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FILE,
				  param_spec);

  param_spec = g_param_spec_object("main\0",
				   "main access\0",
				   "The main object to access the tree\0",
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_MAIN,
				  param_spec);

  /* signals */
  file_id_ref_signals[RESOLVED] = 
    g_signal_new("resolved\0",
		 G_TYPE_FROM_CLASS(file_id_ref),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsFileIdRefClass, resolved),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);
}

void
ags_file_id_ref_init(AgsFileIdRef *file_id_ref)
{
  file_id_ref->main = NULL;
  file_id_ref->file = NULL;

  file_id_ref->node = NULL;
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
  case PROP_NODE:
    {
      xmlNode *node;

      node = (xmlNode *) g_value_get_pointer(value);

      file_id_ref->node = node;
    }
    break;
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
      gpointer ref;

      ref = (gpointer) g_value_get_pointer(value);

      file_id_ref->ref = ref;
    }
    break;
  case PROP_FILE:
    {
      GObject *file;

      file = (GObject *) g_value_get_object(value);

      if(file_id_ref->file != NULL)
	g_object_unref(file_id_ref->file);

      if(file != NULL)
	g_object_ref(file);

      file_id_ref->file = file;
    }
    break;
  case PROP_MAIN:
    {
      GObject *main;

      main = (GObject *) g_value_get_object(value);

      if(file_id_ref->main != NULL)
	g_object_unref(file_id_ref->main);

      if(main != NULL)
	g_object_ref(main);

      file_id_ref->main = main;
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
  case PROP_NODE:
    {
      g_value_set_pointer(value, file_id_ref->node);
    }
    break;
  case PROP_XPATH:
    {
      g_value_set_string(value, file_id_ref->xpath);
    }
    break;
  case PROP_REFERENCE:
    {
      g_value_set_pointer(value, file_id_ref->ref);
    }
    break;
  case PROP_FILE:
    {
      g_value_set_object(value, file_id_ref->file);
    }
    break;
  case PROP_MAIN:
    {
      g_value_set_object(value, file_id_ref->main);
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
  AgsFileIdRef *file_id_ref;

  file_id_ref = AGS_FILE_ID_REF(gobject);

  if(file_id_ref->xpath != NULL){
    g_free(file_id_ref->xpath);
  }

  if(file_id_ref->ref != NULL){
    g_object_unref(file_id_ref->ref);
  }

  if(file_id_ref->file != NULL){
    g_object_unref(file_id_ref->file);
  }

  if(file_id_ref->main != NULL){
    g_object_unref(file_id_ref->main);
  }

  G_OBJECT_CLASS(ags_file_id_ref_parent_class)->finalize(gobject);
}

void
ags_file_id_ref_resolved(AgsFileIdRef *file_id_ref)
{
  g_return_if_fail(AGS_IS_FILE_ID_REF(file_id_ref));

  g_object_ref((GObject *) file_id_ref);
  g_signal_emit(G_OBJECT(file_id_ref),
		file_id_ref_signals[RESOLVED],
		0);
  g_object_unref((GObject *) file_id_ref);
}

/**
 * ags_file_id_ref_new:
 *
 * Creates an #AgsFileIdRef
 *
 * Returns: a new #AgsFileIdRef
 *
 * Since: 0.4
 */
AgsFileIdRef*
ags_file_id_ref_new()
{
  AgsFileIdRef *file_id_ref;

  file_id_ref = (AgsFileIdRef *) g_object_new(AGS_TYPE_FILE_ID_REF,
					      NULL);

  return(file_id_ref);
}
