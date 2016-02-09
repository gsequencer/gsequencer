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

#include <ags/X/machine/ags_lv2_bridge.h>
#include <ags/X/machine/ags_lv2_bridge_callbacks.h>

#include <ags/object/ags_connectable.h>

#include <ags/object/ags_marshal.h>
#include <ags/object/ags_plugin.h>

#include <ags/audio/ags_input.h>

#include <ags/X/ags_effect_bridge.h>
#include <ags/X/ags_effect_bulk.h>

void ags_lv2_bridge_class_init(AgsLv2BridgeClass *lv2_bridge);
void ags_lv2_bridge_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_lv2_bridge_plugin_interface_init(AgsPluginInterface *plugin);
void ags_lv2_bridge_init(AgsLv2Bridge *lv2_bridge);
void ags_lv2_bridge_set_property(GObject *gobject,
				 guint prop_id,
				 const GValue *value,
				 GParamSpec *param_spec);
void ags_lv2_bridge_get_property(GObject *gobject,
				 guint prop_id,
				 GValue *value,
				 GParamSpec *param_spec);
void ags_lv2_bridge_connect(AgsConnectable *connectable);
void ags_lv2_bridge_disconnect(AgsConnectable *connectable);
gchar* ags_lv2_bridge_get_version(AgsPlugin *plugin);
void ags_lv2_bridge_set_version(AgsPlugin *plugin, gchar *version);
gchar* ags_lv2_bridge_get_build_id(AgsPlugin *plugin);
void ags_lv2_bridge_set_build_id(AgsPlugin *plugin, gchar *build_id);

/**
 * SECTION:ags_lv2_bridge
 * @short_description: A composite widget to visualize a bunch of #AgsChannel
 * @title: AgsLv2Bridge
 * @section_id:
 * @include: ags/X/ags_lv2_bridge.h
 *
 * #AgsLv2Bridge is a composite widget to visualize all #AgsChannel. It should be
 * packed by an #AgsMachine.
 */

enum{
  PROP_0,
  PROP_FILENAME,
  PROP_URI,
  PROP_INDEX,
};

static gpointer ags_lv2_bridge_parent_class = NULL;
static AgsConnectableInterface* ags_lv2_bridge_parent_connectable_interface;
static AgsPluginInterface* ags_lv2_bridge_parent_plugin_interface;

GType
ags_lv2_bridge_get_type(void)
{
  static GType ags_type_lv2_bridge = 0;

  if(!ags_type_lv2_bridge){
    static const GTypeInfo ags_lv2_bridge_info = {
      sizeof(AgsLv2BridgeClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_lv2_bridge_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsLv2Bridge),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_lv2_bridge_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_lv2_bridge_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_plugin_interface_info = {
      (GInterfaceInitFunc) ags_lv2_bridge_plugin_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_lv2_bridge = g_type_register_static(AGS_TYPE_MACHINE,
						 "AgsLv2Bridge\0", &ags_lv2_bridge_info,
						 0);

    g_type_add_interface_static(ags_type_lv2_bridge,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_lv2_bridge,
				AGS_TYPE_PLUGIN,
				&ags_plugin_interface_info);
  }

  return(ags_type_lv2_bridge);
}

void
ags_lv2_bridge_class_init(AgsLv2BridgeClass *lv2_bridge)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_lv2_bridge_parent_class = g_type_class_peek_parent(lv2_bridge);

  /* GObjectClass */
  gobject = G_OBJECT_CLASS(lv2_bridge);

  gobject->set_property = ags_lv2_bridge_set_property;
  gobject->get_property = ags_lv2_bridge_get_property;

  /* properties */
  /**
   * AgsRecallLv2:filename:
   *
   * The plugins filename.
   * 
   * Since: 0.4.3
   */
  param_spec =  g_param_spec_string("filename\0",
				    "the object file\0",
				    "The filename as string of object file\0",
				    NULL,
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FILENAME,
				  param_spec);

  /**
   * AgsRecallLv2:uri:
   *
   * The uri's name.
   * 
   * Since: 0.4.3
   */
  param_spec =  g_param_spec_string("uri\0",
				    "the uri\0",
				    "The uri's string representation\0",
				    NULL,
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_URI,
				  param_spec);

  /**
   * AgsRecallLv2:index:
   *
   * The uri's index.
   * 
   * Since: 0.4.3
   */
  param_spec =  g_param_spec_ulong("index\0",
				   "index of uri\0",
				   "The numerical index of uri\0",
				   0,
				   65535,
				   0,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_INDEX,
				  param_spec);
}

void
ags_lv2_bridge_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_lv2_bridge_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_lv2_bridge_connect;
  connectable->disconnect = ags_lv2_bridge_disconnect;
}

void
ags_lv2_bridge_plugin_interface_init(AgsPluginInterface *plugin)
{
  plugin->get_name = NULL;
  plugin->set_name = NULL;
  plugin->get_version = ags_lv2_bridge_get_version;
  plugin->set_version = ags_lv2_bridge_set_version;
  plugin->get_build_id = ags_lv2_bridge_get_build_id;
  plugin->set_build_id = ags_lv2_bridge_set_build_id;
  plugin->get_xml_type = NULL;
  plugin->set_xml_type = NULL;
  plugin->get_ports = NULL;
  plugin->read = NULL;
  plugin->write = NULL;
  plugin->set_ports = NULL;
}

void
ags_lv2_bridge_init(AgsLv2Bridge *lv2_bridge)
{
  GtkTable *table;

  AgsAudio *audio;

  audio = AGS_MACHINE(lv2_bridge)->audio;
  audio->flags |= (AGS_AUDIO_SYNC);

  lv2_bridge->flags = 0;

  lv2_bridge->name = NULL;

  lv2_bridge->version = AGS_LV2_BRIDGE_DEFAULT_VERSION;
  lv2_bridge->build_id = AGS_LV2_BRIDGE_DEFAULT_BUILD_ID;
  
  lv2_bridge->mapped_output = 0;
  lv2_bridge->mapped_input = 0;
  
  lv2_bridge->filename = NULL;
  lv2_bridge->uri = NULL;
  lv2_bridge->uri_index = 0;

  AGS_MACHINE(lv2_bridge)->bridge = ags_effect_bridge_new(audio);
  gtk_container_add(gtk_bin_get_child(lv2_bridge),
		    (GtkWidget *) AGS_MACHINE(lv2_bridge)->bridge);

  table = (GtkTable *) gtk_table_new(1, 2, FALSE);
  gtk_box_pack_start(AGS_EFFECT_BRIDGE(AGS_MACHINE(lv2_bridge)->bridge),
		     table,
		     FALSE, FALSE,
		     0);

  AGS_EFFECT_BRIDGE(AGS_MACHINE(lv2_bridge)->bridge)->bulk_input = (GtkWidget *) ags_effect_bulk_new(audio,
												     AGS_TYPE_INPUT);
  AGS_EFFECT_BULK(AGS_EFFECT_BRIDGE(AGS_MACHINE(lv2_bridge)->bridge)->bulk_input)->flags |= (AGS_EFFECT_BULK_HIDE_BUTTONS |
											     AGS_EFFECT_BULK_HIDE_ENTRIES |
											     AGS_EFFECT_BULK_SHOW_LABELS);
  gtk_table_attach(table,
		   AGS_EFFECT_BRIDGE(AGS_MACHINE(lv2_bridge)->bridge)->bulk_input,
		   0, 1,
		   0, 1,
		   GTK_FILL, GTK_FILL,
		   0, 0);
}

void
ags_lv2_bridge_set_property(GObject *gobject,
			    guint prop_id,
			    const GValue *value,
			    GParamSpec *param_spec)
{
  AgsLv2Bridge *lv2_bridge;

  lv2_bridge = AGS_LV2_BRIDGE(gobject);

  switch(prop_id){
  case PROP_FILENAME:
    {
      gchar *filename;

      filename = g_value_get_string(value);

      if(filename == lv2_bridge->filename){
	return;
      }

      if(lv2_bridge->filename != NULL){
	g_free(lv2_bridge->filename);
      }

      lv2_bridge->filename = g_strdup(filename);
    }
    break;
  case PROP_URI:
    {
      gchar *uri;
      
      uri = g_value_get_string(value);

      if(uri == lv2_bridge->uri){
	return;
      }

      if(lv2_bridge->uri != NULL){
	g_free(lv2_bridge->uri);
      }

      lv2_bridge->uri = g_strdup(uri);
    }
    break;
  case PROP_INDEX:
    {
      unsigned long *uri_index;
      
      uri_index = g_value_get_ulong(value);

      if(uri_index == lv2_bridge->uri_index){
	return;
      }

      lv2_bridge->uri_index = uri_index;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_lv2_bridge_get_property(GObject *gobject,
			    guint prop_id,
			    GValue *value,
			    GParamSpec *param_spec)
{
  AgsLv2Bridge *lv2_bridge;

  lv2_bridge = AGS_LV2_BRIDGE(gobject);

  switch(prop_id){
  case PROP_FILENAME:
    {
      g_value_set_string(value, lv2_bridge->filename);
    }
    break;
  case PROP_URI:
    {
      g_value_set_string(value, lv2_bridge->uri);
    }
    break;
  case PROP_INDEX:
    {
      g_value_set_ulong(value, lv2_bridge->uri_index);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_lv2_bridge_connect(AgsConnectable *connectable)
{
  AgsLv2Bridge *lv2_bridge;

  if((AGS_MACHINE_CONNECTED & (AGS_MACHINE(connectable)->flags)) != 0){
    return;
  }

  ags_lv2_bridge_parent_connectable_interface->connect(connectable);

  lv2_bridge = AGS_LV2_BRIDGE(connectable);
}

void
ags_lv2_bridge_disconnect(AgsConnectable *connectable)
{
  //TODO:JK: implement me
}

gchar*
ags_lv2_bridge_get_version(AgsPlugin *plugin)
{
  return(AGS_LV2_BRIDGE(plugin)->version);
}

void
ags_lv2_bridge_set_version(AgsPlugin *plugin, gchar *version)
{
  AgsLv2Bridge *lv2_bridge;

  lv2_bridge = AGS_LV2_BRIDGE(plugin);

  lv2_bridge->version = version;
}

gchar*
ags_lv2_bridge_get_build_id(AgsPlugin *plugin)
{
  return(AGS_LV2_BRIDGE(plugin)->build_id);
}

void
ags_lv2_bridge_set_build_id(AgsPlugin *plugin, gchar *build_id)
{
  AgsLv2Bridge *lv2_bridge;

  lv2_bridge = AGS_LV2_BRIDGE(plugin);

  lv2_bridge->build_id = build_id;
}

void
ags_lv2_bridge_load(AgsLv2Bridge *lv2_bridge)
{
  GList *list;

  list = gtk_container_get_children(AGS_EFFECT_BULK(AGS_EFFECT_BRIDGE(AGS_MACHINE(lv2_bridge)->bridge)->bulk_input)->table);
  
  while(list != NULL){
    gtk_widget_destroy(list->data);
    
    list = list->next;
  }
  
  ags_effect_bulk_add_effect(AGS_EFFECT_BRIDGE(AGS_MACHINE(lv2_bridge)->bridge)->bulk_input,
			     lv2_bridge->filename,
			     lv2_bridge->uri);
}

/**
 * ags_lv2_bridge_new:
 * @soundcard: the assigned soundcard.
 * @filename: the ui.so
 * @uri: the uri's URI
 *
 * Creates an #AgsLv2Bridge
 *
 * Returns: a new #AgsLv2Bridge
 *
 * Since: 0.4.3
 */
AgsLv2Bridge*
ags_lv2_bridge_new(GObject *soundcard,
		   gchar *filename,
		   gchar *uri)
{
  AgsLv2Bridge *lv2_bridge;
  GValue value = {0,};

  lv2_bridge = (AgsLv2Bridge *) g_object_new(AGS_TYPE_LV2_BRIDGE,
					     NULL);

  if(soundcard != NULL){
    g_value_init(&value, G_TYPE_OBJECT);
    g_value_set_object(&value, soundcard);
    g_object_set_property(G_OBJECT(AGS_MACHINE(lv2_bridge)->audio),
			  "soundcard\0", &value);
    g_value_unset(&value);
  }

  g_object_set(lv2_bridge,
	       "filename\0", filename,
	       "uri\0", uri,
	       NULL);
  
  return(lv2_bridge);
}
