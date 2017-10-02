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

#include <ags/file/ags_file_id_ref.h>

#include <ags/i18n.h>

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
  PROP_APPLICATION_CONTEXT,
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
						  "AgsFileIdRef",
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
  /**
   * AgsFileIdRef:node:
   *
   * The assigned xmlNode being refered by this #AgsFileIdRef.
   *
   * Since: 1.0.0
   */
  param_spec = g_param_spec_pointer("node",
				    i18n_pspec("the node"),
				    i18n_pspec("The node to find the element"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_NODE,
				  param_spec);

  /**
   * AgsFileIdRef:xpath:
   *
   * The XPath short-cut that can be used with this #AgsFileIdRef.
   *
   * Since: 1.0.0
   */
  param_spec = g_param_spec_string("xpath",
				   i18n_pspec("the xpath"),
				   i18n_pspec("The xpath to find the element"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_XPATH,
				  param_spec);

  /**
   * AgsFileIdRef:reference:
   *
   * The object refered by this #AgsFileIdRef.
   *
   * Since: 1.0.0
   */
  param_spec = g_param_spec_pointer("reference",
				    i18n_pspec("reference of the locator"),
				    i18n_pspec("The reference resulted by the xpath locator"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_REFERENCE,
				  param_spec);


  /**
   * AgsFileIdRef:file:
   *
   * The #AgsFile this #AgsFileIdRef belongs to.
   *
   * Since: 1.0.0
   */
  param_spec = g_param_spec_object("file",
				   i18n_pspec("file assigned to"),
				   i18n_pspec("The entire file assigned to"),
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FILE,
				  param_spec);

  /**
   * AgsFileIdRef:application-context:
   *
   * The #AgsApplicationContext to be used.
   *
   * Since: 1.0.0
   */
  param_spec = g_param_spec_object("application-context",
				   i18n_pspec("application context access"),
				   i18n_pspec("The application-context object to access the tree"),
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_APPLICATION_CONTEXT,
				  param_spec);

  /* signals */
  /**
   * AgsFileIdRef::resolved:
   * @file_id_ref: the #AgsFileIdRef
   * 
   * Signal ::resolved to notify about resolved :reference.
   *
   * Since: 1.0.0
   */
  file_id_ref_signals[RESOLVED] = 
    g_signal_new("resolved",
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
  file_id_ref->application_context = NULL;
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
  case PROP_APPLICATION_CONTEXT:
    {
      GObject *application_context;

      application_context = (GObject *) g_value_get_object(value);

      if(file_id_ref->application_context != NULL)
	g_object_unref(file_id_ref->application_context);

      if(application_context != NULL)
	g_object_ref(application_context);

      file_id_ref->application_context = application_context;
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
  case PROP_APPLICATION_CONTEXT:
    {
      g_value_set_object(value, file_id_ref->application_context);
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

  if(file_id_ref->application_context != NULL){
    g_object_unref(file_id_ref->application_context);
  }

  G_OBJECT_CLASS(ags_file_id_ref_parent_class)->finalize(gobject);
}

/**
 * ags_file_id_ref_resolved:
 * @file_id_ref: the #AgsFileIdRef
 *
 * Notify about resolved reference.
 * 
 * Since: 1.0.0
 */
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
 * Since: 1.0.0
 */
AgsFileIdRef*
ags_file_id_ref_new()
{
  AgsFileIdRef *file_id_ref;

  file_id_ref = (AgsFileIdRef *) g_object_new(AGS_TYPE_FILE_ID_REF,
					      NULL);

  return(file_id_ref);
}
