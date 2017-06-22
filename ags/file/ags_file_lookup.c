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

#include <ags/file/ags_file_lookup.h>

#include <ags/i18n.h>

void ags_file_lookup_class_init(AgsFileLookupClass *file_lookup);
void ags_file_lookup_init (AgsFileLookup *file_lookup);
void ags_file_lookup_set_property(GObject *gobject,
				  guint prop_id,
				  const GValue *value,
				  GParamSpec *param_spec);
void ags_file_lookup_get_property(GObject *gobject,
				  guint prop_id,
				  GValue *value,
				  GParamSpec *param_spec);
void ags_file_lookup_finalize(GObject *gobject);

/**
 * SECTION:ags_file_lookup
 * @short_description: resolve objects read of file.
 * @title: AgsFileLookup
 * @section_id:
 * @include: ags/file/ags_file_lookup.h
 *
 * The #AgsFileLookup resolve objects of file.
 */

enum{
  RESOLVE,
  LAST_SIGNAL,
};

enum{
  PROP_0,
  PROP_FILE,
  PROP_NODE,
  PROP_REFERENCE,
};

static gpointer ags_file_lookup_parent_class = NULL;
static guint file_lookup_signals[LAST_SIGNAL];

GType
ags_file_lookup_get_type (void)
{
  static GType ags_type_file_lookup = 0;

  if(!ags_type_file_lookup){
    static const GTypeInfo ags_file_lookup_info = {
      sizeof (AgsFileLookupClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_file_lookup_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsFileLookup),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_file_lookup_init,
    };

    ags_type_file_lookup = g_type_register_static(G_TYPE_OBJECT,
						  "AgsFileLookup",
						  &ags_file_lookup_info,
						  0);
  }

  return (ags_type_file_lookup);
}

void
ags_file_lookup_class_init(AgsFileLookupClass *file_lookup)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_file_lookup_parent_class = g_type_class_peek_parent(file_lookup);

  /* GObjectClass */
  gobject = (GObjectClass *) file_lookup;

  gobject->get_property = ags_file_lookup_get_property;
  gobject->set_property = ags_file_lookup_set_property;

  gobject->finalize = ags_file_lookup_finalize;

  /* properties */
  /**
   * AgsFile:file:
   *
   * The assigned #AgsFile to resolve.
   *
   * Since: 0.4.2
   */
  param_spec = g_param_spec_object("file",
				   i18n_pspec("assigned file"),
				   i18n_pspec("The file it is assigned with"),
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FILE,
				  param_spec);

  /**
   * AgsFile:node:
   *
   * The assigned #xmlNode to resolve.
   *
   * Since: 0.4.2
   */
  param_spec = g_param_spec_pointer("node",
				    i18n_pspec("assigned node"),
				    i18n_pspec("The node it is assigned with"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_NODE,
				  param_spec);

  /**
   * AgsFile:reference:
   *
   * The assigned #gpointer to resolve.
   *
   * Since: 0.4.2
   */
  param_spec = g_param_spec_pointer("reference",
				    i18n_pspec("assigned reference"),
				    i18n_pspec("The reference it is assigned with"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_REFERENCE,
				  param_spec);

  /* AgsFileLookupClass */
  file_lookup->resolve = NULL;

  /**
   * AgsFile::resolve:
   * @file_lookup: the #AgsFileLookup
   * 
   * Resolve @file_lookup either for reading or writing XPath and retrieving
   * appropriate UUID.
   *
   * Since: 0.4.2
   */
  file_lookup_signals[RESOLVE] =
    g_signal_new("resolve",
		 G_TYPE_FROM_CLASS(file_lookup),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsFileLookupClass, resolve),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);
}

void
ags_file_lookup_init(AgsFileLookup *file_lookup)
{
  file_lookup->file = NULL;

  file_lookup->node = NULL;
  file_lookup->ref = NULL;
}

void
ags_file_lookup_set_property(GObject *gobject,
			     guint prop_id,
			     const GValue *value,
			     GParamSpec *param_spec)
{
  AgsFileLookup *file_lookup;

  file_lookup = AGS_FILE_LOOKUP(gobject);

  switch(prop_id){
  case PROP_FILE:
    {
      AgsFile *file;

      file = (AgsFile *) g_value_get_object(value);

      if(file_lookup->file == file){
	return;
      }

      if(file_lookup->file != NULL){
	g_object_unref(G_OBJECT(file_lookup->file));
      }

      if(file != NULL){
	g_object_ref(file);
      }

      file_lookup->file = file;
    }
    break;
  case PROP_NODE:
    {
      xmlNode *node;

      node = (xmlNode *) g_value_get_pointer(value);

      file_lookup->node = node;
    }
    break;
  case PROP_REFERENCE:
    {
      gpointer ref;

      ref = (gpointer) g_value_get_pointer(value);

      file_lookup->ref = ref;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_file_lookup_get_property(GObject *gobject,
			     guint prop_id,
			     GValue *value,
			     GParamSpec *param_spec)
{
  AgsFileLookup *file_lookup;

  file_lookup = AGS_FILE_LOOKUP(gobject);

  switch(prop_id){
  case PROP_FILE:
    g_value_set_object(value, file_lookup->file);
    break;
  case PROP_NODE:
    g_value_set_pointer(value, file_lookup->node);
    break;
  case PROP_REFERENCE:
    g_value_set_pointer(value, file_lookup->ref);
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_file_lookup_finalize(GObject *gobject)
{
  AgsFileLookup *file_lookup;

  file_lookup = AGS_FILE_LOOKUP(gobject);

  if(file_lookup->file != NULL){
    g_object_unref(G_OBJECT(file_lookup->file));
  }
}

/**
 * ags_file_lookup_resolve:
 * @file_lookup: the #AgsFileLookup
 *
 * The ::resolve signal. 
 *
 * Since: 0.4.2
 */
void
ags_file_lookup_resolve(AgsFileLookup *file_lookup)
{
  g_return_if_fail(AGS_IS_FILE_LOOKUP(file_lookup));

  g_object_ref((GObject *) file_lookup);
  g_signal_emit(G_OBJECT(file_lookup),
		file_lookup_signals[RESOLVE], 0);
  g_object_unref((GObject *) file_lookup);
}

/**
 * ags_file_lookup_find_by_node:
 * @file_lookup: the #GList containing #AgsFileLookup
 * @node: a #xmlNode
 *
 * Find #AgsFileLookup by #xmlNode
 *
 * Returns: The list containing #AgsFileLookup if found otherwise %NULL
 *
 * Since: 0.4.2
 */
GList*
ags_file_lookup_find_by_node(GList *file_lookup,
			     xmlNode *node)
{
  while(file_lookup != NULL && AGS_FILE_LOOKUP(file_lookup->data)->node != node){
    file_lookup = file_lookup->next;
  }

  return(file_lookup);
}

/**
 * ags_file_lookup_find_by_reference:
 * @file_lookup: the #GList containing #AgsFileLookup
 * @ref: a #gpointer
 *
 * Find #AgsFileLookup by @ref
 *
 * Returns: The list containing #AgsFileLookup if found otherwise %NULL
 *
 * Since: 0.7.8
 */
GList*
ags_file_lookup_find_by_reference(GList *file_lookup,
				  gpointer ref)
{
  while(file_lookup != NULL && AGS_FILE_LOOKUP(file_lookup->data)->ref != ref){
    file_lookup = file_lookup->next;
  }

  return(file_lookup);
}

/**
 * ags_file_lookup_new:
 *
 * Creates an #AgsFileLookup
 *
 * Returns: a new #AgsFileLookup
 *
 * Since: 0.4
 */
AgsFileLookup*
ags_file_lookup_new()
{
  AgsFileLookup *file_lookup;

  file_lookup = (AgsFileLookup *) g_object_new(AGS_TYPE_FILE_LOOKUP,
					       NULL);
  
  return(file_lookup);
}
