/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2020 Joël Krähemann
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

#include <ags/file/ags_file_link.h>

#include <ags/util/ags_id_generator.h>

#include <ags/object/ags_plugin.h>

#include <ags/file/ags_file.h>
#include <ags/file/ags_file_id_ref.h>

#include <ags/i18n.h>

void ags_file_link_class_init(AgsFileLinkClass *file_link);
void ags_file_link_plugin_interface_init(AgsPluginInterface *plugin);
void ags_file_link_init(AgsFileLink *file_link);
void ags_file_link_set_property(GObject *gobject,
				guint prop_id,
				const GValue *value,
				GParamSpec *param_spec);
void ags_file_link_get_property(GObject *gobject,
				guint prop_id,
				GValue *value,
				GParamSpec *param_spec);
gchar* ags_file_link_get_name(AgsPlugin *plugin);
void ags_file_link_set_name(AgsPlugin *plugin, gchar *name);
gchar* ags_file_link_get_version(AgsPlugin *plugin);
void ags_file_link_set_version(AgsPlugin *plugin, gchar *version);
gchar* ags_file_link_get_build_id(AgsPlugin *plugin);
void ags_file_link_set_build_id(AgsPlugin *plugin, gchar *build_id);
gchar* ags_file_link_get_xml_type(AgsPlugin *plugin);
void ags_file_link_set_xml_type(AgsPlugin *plugin, gchar *xml_type);
void ags_file_link_read(GObject *file, xmlNode *node, AgsPlugin *plugin);
xmlNode* ags_file_link_write(GObject *file, xmlNode *parent, AgsPlugin *plugin);
void ags_file_link_finalize(GObject *gobject);

/**
 * SECTION:ags_file_link
 * @short_description: link objects read of file.
 * @title: AgsFileLink
 * @section_id:
 * @include: ags/file/ags_file_link.h
 *
 * The #AgsFileLink links read objects of file.
 */

enum{
  PROP_0,
  PROP_FILENAME,
  PROP_DATA,
};

static gpointer ags_file_link_parent_class = NULL;

GType
ags_file_link_get_type()
{
  static gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_file_link = 0;

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

    static const GInterfaceInfo ags_plugin_interface_info = {
      (GInterfaceInitFunc) ags_file_link_plugin_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_file_link = g_type_register_static(G_TYPE_OBJECT,
						"AgsFileLink",
						&ags_file_link_info,
						0);

    g_type_add_interface_static(ags_type_file_link,
				AGS_TYPE_PLUGIN,
				&ags_plugin_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_file_link);
  }

  return g_define_type_id__volatile;
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
  /**
   * AgsFileLink:filename:
   *
   * The filename this #AgsFileLink refers.
   *
   * Since: 3.0.0
   */
  param_spec = g_param_spec_string("filename",
				   i18n_pspec("the filename"),
				   i18n_pspec("The filename to locate the file"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FILENAME,
				  param_spec);

  /**
   * AgsFileLink:data:
   *
   * The data this #AgsFileLink contains.
   *
   * Since: 3.0.0
   */
  param_spec = g_param_spec_string("data",
				   i18n_pspec("the data"),
				   i18n_pspec("The embedded data"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DATA,
				  param_spec);
}

void
ags_file_link_plugin_interface_init(AgsPluginInterface *plugin)
{
  plugin->get_name = ags_file_link_get_name;
  plugin->set_name = ags_file_link_set_name;
  plugin->get_version = ags_file_link_get_version;
  plugin->set_version = ags_file_link_set_version;
  plugin->get_build_id = ags_file_link_get_build_id;
  plugin->set_build_id = ags_file_link_set_build_id;
  plugin->get_xml_type = ags_file_link_get_xml_type;
  plugin->set_xml_type = ags_file_link_set_xml_type;
  plugin->get_ports = NULL;
  plugin->read = ags_file_link_read;
  plugin->write = ags_file_link_write;
  plugin->set_ports = NULL;
}

void
ags_file_link_init(AgsFileLink *file_link)
{
  /* file link mutex */
  g_rec_mutex_init(&(file_link->obj_mutex));  

  
  file_link->version = NULL;
  file_link->build_id = NULL;

  file_link->name = NULL;

  file_link->xml_type = NULL;

  file_link->filename = NULL;
  
  file_link->data = NULL;
}

void
ags_file_link_set_property(GObject *gobject,
			   guint prop_id,
			   const GValue *value,
			   GParamSpec *param_spec)
{
  AgsFileLink *file_link;

  GRecMutex *file_link_mutex;

  file_link = AGS_FILE_LINK(gobject);

  /* get file link mutex */
  file_link_mutex = AGS_FILE_LINK_GET_OBJ_MUTEX(file_link);
  
  switch(prop_id){
  case PROP_FILENAME:
    {
      char *filename;

      filename = (char *) g_value_get_string(value);

      g_rec_mutex_lock(file_link_mutex);

      if(filename == file_link->filename){
	g_rec_mutex_unlock(file_link_mutex);

	return;
      }

      if(file_link->filename != NULL){
	free(file_link->filename);
      }
	
      file_link->filename = g_strdup(filename);

      g_rec_mutex_unlock(file_link_mutex);
    }
    break;
  case PROP_DATA:
    {
      char *data;

      data = (char *) g_value_get_string(value);

      g_rec_mutex_lock(file_link_mutex);

      if(data == file_link->data){
	g_rec_mutex_unlock(file_link_mutex);

	return;
      }

      file_link->data = g_strdup(data);

      g_rec_mutex_unlock(file_link_mutex);
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

  GRecMutex *file_link_mutex;

  file_link = AGS_FILE_LINK(gobject);
  
  /* get file link mutex */
  file_link_mutex = AGS_FILE_LINK_GET_OBJ_MUTEX(file_link);

  switch(prop_id){
  case PROP_FILENAME:
    {
      g_rec_mutex_lock(file_link_mutex);

      g_value_set_string(value, file_link->filename);

      g_rec_mutex_unlock(file_link_mutex);
    }
    break;
  case PROP_DATA:
    {
      g_rec_mutex_lock(file_link_mutex);

      g_value_set_string(value, file_link->data);

      g_rec_mutex_unlock(file_link_mutex);
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

  /* filename */
  if(file_link->filename != NULL){
    g_free(file_link->filename);
  }

  /* data */
  if(file_link->data != NULL){
    g_free(file_link->data);
  }

  /* call parent */
  G_OBJECT_CLASS(ags_file_link_parent_class)->finalize(gobject);
}

gchar*
ags_file_link_get_name(AgsPlugin *plugin)
{
  return(AGS_FILE_LINK(plugin)->name);
}

void
ags_file_link_set_name(AgsPlugin *plugin, gchar *name)
{
  AGS_FILE_LINK(plugin)->name = name;
}

gchar*
ags_file_link_get_version(AgsPlugin *plugin)
{
  return(AGS_FILE_LINK(plugin)->version);
}

void
ags_file_link_set_version(AgsPlugin *plugin, gchar *version)
{
  AGS_FILE_LINK(plugin)->version = version;
}

gchar*
ags_file_link_get_build_id(AgsPlugin *plugin)
{
  return(AGS_FILE_LINK(plugin)->build_id);
}

void
ags_file_link_set_build_id(AgsPlugin *plugin, gchar *build_id)
{
  AGS_FILE_LINK(plugin)->build_id = build_id;
}

gchar*
ags_file_link_get_xml_type(AgsPlugin *plugin)
{
  return(AGS_FILE_LINK(plugin)->xml_type);
}

void
ags_file_link_set_xml_type(AgsPlugin *plugin, gchar *xml_type)
{
  AGS_FILE_LINK(plugin)->xml_type = xml_type;
}

void
ags_file_link_read(GObject *file, xmlNode *node, AgsPlugin *plugin)
{
  AgsFileLink *file_link;

  file_link = AGS_FILE_LINK(plugin);

  ags_file_add_id_ref((AgsFile *) file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "node", node,
				   "xpath", g_strdup_printf("xpath=//*[@id='%s']", xmlGetProp(node, "id")),
				   "reference", file_link,
				   NULL));
}

xmlNode*
ags_file_link_write(GObject *file, xmlNode *parent, AgsPlugin *plugin)
{
  AgsFileLink *file_link;
  xmlNode *node;
  gchar *id;

  file_link = AGS_FILE_LINK(plugin);

  id = ags_id_generator_create_uuid();

  node = xmlNewNode(NULL,
		    AGS_FILE_LINK(plugin)->xml_type);
  xmlNewProp(node,
	     "id",
	     id);

  ags_file_add_id_ref((AgsFile *) file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "node", node,
				   "xpath", g_strdup_printf("xpath=//*[@id='%s']", id),
				   "reference", file_link,
				   NULL));

  xmlAddChild(parent,
	      node);

  return(node);
}

/**
 * ags_file_link_set_filename:
 * @file_link: the #AgsFileLink
 * @filename: the filename
 * 
 * Set @filename of @file_link.
 * 
 * Since: 3.6.17
 */
void
ags_file_link_set_filename(AgsFileLink *file_link,
			   gchar *filename)
{
  if(!AGS_IS_FILE_LINK(file_link)){
    return;
  }

  g_object_set(file_link,
	       "filename", filename,
	       NULL);
}

/**
 * ags_file_link_get_filename:
 * @file_link: the #AgsFileLink
 * 
 * Get filename of @file_link.
 * 
 * Returns: (transfer full): the filename
 * 
 * Since: 3.6.17
 */
gchar*
ags_file_link_get_filename(AgsFileLink *file_link)
{
  gchar *filename;
  
  if(!AGS_IS_FILE_LINK(file_link)){
    return(NULL);
  }

  g_object_get(file_link,
	       "filename", &filename,
	       NULL);

  return(filename);
}

/**
 * ags_file_link_set_data:
 * @file_link: the #AgsFileLink
 * @data: the data
 * 
 * Set @data of @file_link.
 * 
 * Since: 3.6.17
 */
void
ags_file_link_set_data(AgsFileLink *file_link,
		       gchar *data)
{
  if(!AGS_IS_FILE_LINK(file_link)){
    return;
  }

  g_object_set(file_link,
	       "data", data,
	       NULL);
}

/**
 * ags_file_link_get_data:
 * @file_link: the #AgsFileLink
 * 
 * Get data of @file_link.
 * 
 * Returns: (transfer full): the data
 * 
 * Since: 3.6.17
 */
gchar*
ags_file_link_get_data(AgsFileLink *file_link)
{
  gchar *data;
  
  if(!AGS_IS_FILE_LINK(file_link)){
    return(NULL);
  }

  g_object_get(file_link,
	       "data", &data,
	       NULL);

  return(data);
}

/**
 * ags_file_link_new:
 *
 * Creates an #AgsFileLink
 *
 * Returns: a new #AgsFileLink
 *
 * Since: 3.0.0
 */
AgsFileLink*
ags_file_link_new()
{
  AgsFileLink *file_link;

  file_link = (AgsFileLink *) g_object_new(AGS_TYPE_FILE_LINK,
					   NULL);

  return(file_link);
}
