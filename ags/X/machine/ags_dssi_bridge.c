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

#include <ags/X/machine/ags_dssi_bridge.h>
#include <ags/X/machine/ags_dssi_bridge_callbacks.h>

#include <ags/object/ags_connectable.h>
#include <ags/object/ags_config.h>
#include <ags/object/ags_soundcard.h>

#include <ags/object/ags_marshal.h>
#include <ags/object/ags_plugin.h>

#include <ags/plugin/ags_dssi_manager.h>

#include <ags/audio/ags_input.h>

#include <ags/X/ags_effect_bridge.h>
#include <ags/X/ags_effect_bulk.h>

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

void ags_dssi_bridge_class_init(AgsDssiBridgeClass *dssi_bridge);
void ags_dssi_bridge_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_dssi_bridge_plugin_interface_init(AgsPluginInterface *plugin);
void ags_dssi_bridge_init(AgsDssiBridge *dssi_bridge);
void ags_dssi_bridge_set_property(GObject *gobject,
				  guint prop_id,
				  const GValue *value,
				  GParamSpec *param_spec);
void ags_dssi_bridge_get_property(GObject *gobject,
				  guint prop_id,
				  GValue *value,
				  GParamSpec *param_spec);
void ags_dssi_bridge_connect(AgsConnectable *connectable);
void ags_dssi_bridge_disconnect(AgsConnectable *connectable);
gchar* ags_dssi_bridge_get_version(AgsPlugin *plugin);
void ags_dssi_bridge_set_version(AgsPlugin *plugin, gchar *version);
gchar* ags_dssi_bridge_get_build_id(AgsPlugin *plugin);
void ags_dssi_bridge_set_build_id(AgsPlugin *plugin, gchar *build_id);

/**
 * SECTION:ags_dssi_bridge
 * @short_description: A composite widget to visualize a bunch of #AgsChannel
 * @title: AgsDssiBridge
 * @section_id:
 * @include: ags/X/ags_dssi_bridge.h
 *
 * #AgsDssiBridge is a composite widget to visualize all #AgsChannel. It should be
 * packed by an #AgsMachine.
 */

enum{
  PROP_0,
  PROP_FILENAME,
  PROP_EFFECT,
  PROP_INDEX,
};

static gpointer ags_dssi_bridge_parent_class = NULL;
static AgsConnectableInterface* ags_dssi_bridge_parent_connectable_interface;
static AgsPluginInterface* ags_dssi_bridge_parent_plugin_interface;

GType
ags_dssi_bridge_get_type(void)
{
  static GType ags_type_dssi_bridge = 0;

  if(!ags_type_dssi_bridge){
    static const GTypeInfo ags_dssi_bridge_info = {
      sizeof(AgsDssiBridgeClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_dssi_bridge_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsDssiBridge),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_dssi_bridge_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_dssi_bridge_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_plugin_interface_info = {
      (GInterfaceInitFunc) ags_dssi_bridge_plugin_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_dssi_bridge = g_type_register_static(AGS_TYPE_MACHINE,
						  "AgsDssiBridge\0", &ags_dssi_bridge_info,
						  0);

    g_type_add_interface_static(ags_type_dssi_bridge,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_dssi_bridge,
				AGS_TYPE_PLUGIN,
				&ags_plugin_interface_info);
  }

  return(ags_type_dssi_bridge);
}

void
ags_dssi_bridge_class_init(AgsDssiBridgeClass *dssi_bridge)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_dssi_bridge_parent_class = g_type_class_peek_parent(dssi_bridge);

  /* GObjectClass */
  gobject = G_OBJECT_CLASS(dssi_bridge);

  gobject->set_property = ags_dssi_bridge_set_property;
  gobject->get_property = ags_dssi_bridge_get_property;

  /* properties */
  /**
   * AgsRecallDssi:filename:
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
   * AgsRecallDssi:effect:
   *
   * The effect's name.
   * 
   * Since: 0.4.3
   */
  param_spec =  g_param_spec_string("effect\0",
				    "the effect\0",
				    "The effect's string representation\0",
				    NULL,
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_EFFECT,
				  param_spec);

  /**
   * AgsRecallDssi:index:
   *
   * The effect's index.
   * 
   * Since: 0.4.3
   */
  param_spec =  g_param_spec_ulong("index\0",
				   "index of effect\0",
				   "The numerical index of effect\0",
				   0,
				   65535,
				   0,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_INDEX,
				  param_spec);
}

void
ags_dssi_bridge_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_dssi_bridge_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_dssi_bridge_connect;
  connectable->disconnect = ags_dssi_bridge_disconnect;
}

void
ags_dssi_bridge_plugin_interface_init(AgsPluginInterface *plugin)
{
  plugin->get_name = NULL;
  plugin->set_name = NULL;
  plugin->get_version = ags_dssi_bridge_get_version;
  plugin->set_version = ags_dssi_bridge_set_version;
  plugin->get_build_id = ags_dssi_bridge_get_build_id;
  plugin->set_build_id = ags_dssi_bridge_set_build_id;
  plugin->get_xml_type = NULL;
  plugin->set_xml_type = NULL;
  plugin->get_ports = NULL;
  plugin->read = NULL;
  plugin->write = NULL;
  plugin->set_ports = NULL;
}

void
ags_dssi_bridge_init(AgsDssiBridge *dssi_bridge)
{
  GtkVBox *vbox;
  GtkTable *table;

  AgsAudio *audio;

  audio = AGS_MACHINE(dssi_bridge)->audio;
  audio->flags |= (AGS_AUDIO_SYNC);
  
  dssi_bridge->flags = 0;

  dssi_bridge->name = NULL;

  dssi_bridge->version = AGS_DSSI_BRIDGE_DEFAULT_VERSION;
  dssi_bridge->build_id = AGS_DSSI_BRIDGE_DEFAULT_BUILD_ID;
  
  dssi_bridge->mapped_output = 0;
  dssi_bridge->mapped_input = 0;
  
  dssi_bridge->filename = NULL;
  dssi_bridge->effect = NULL;
  dssi_bridge->effect_index = 0;

  vbox = (GtkVBox *) gtk_vbox_new(FALSE, 0);
  gtk_container_add(gtk_bin_get_child(dssi_bridge),
		    (GtkWidget *) vbox);

  dssi_bridge->program = (GtkComboBoxText *) gtk_combo_box_text_new();
  gtk_box_pack_start((GtkBox *) vbox,
		     (GtkWidget *) dssi_bridge->program,
		     FALSE, FALSE,
		     0);

  AGS_MACHINE(dssi_bridge)->bridge = ags_effect_bridge_new(audio);
  gtk_box_pack_start((GtkBox *) vbox,
		     (GtkWidget *) AGS_MACHINE(dssi_bridge)->bridge,
		     FALSE, FALSE,
		     0);

  
  table = (GtkTable *) gtk_table_new(1, 2, FALSE);
  gtk_box_pack_start(AGS_EFFECT_BRIDGE(AGS_MACHINE(dssi_bridge)->bridge),
		     table,
		     FALSE, FALSE,
		     0);

  AGS_EFFECT_BRIDGE(AGS_MACHINE(dssi_bridge)->bridge)->bulk_input = (GtkWidget *) ags_effect_bulk_new(audio,
												      AGS_TYPE_INPUT);
  AGS_EFFECT_BULK(AGS_EFFECT_BRIDGE(AGS_MACHINE(dssi_bridge)->bridge)->bulk_input)->flags |= (AGS_EFFECT_BULK_HIDE_BUTTONS |
											      AGS_EFFECT_BULK_HIDE_ENTRIES |
											      AGS_EFFECT_BULK_SHOW_LABELS);
  gtk_table_attach(table,
		   AGS_EFFECT_BRIDGE(AGS_MACHINE(dssi_bridge)->bridge)->bulk_input,
		   0, 1,
		   0, 1,
		   GTK_FILL, GTK_FILL,
		   0, 0);
}

void
ags_dssi_bridge_set_property(GObject *gobject,
			     guint prop_id,
			     const GValue *value,
			     GParamSpec *param_spec)
{
  AgsDssiBridge *dssi_bridge;

  dssi_bridge = AGS_DSSI_BRIDGE(gobject);

  switch(prop_id){
  case PROP_FILENAME:
    {
      gchar *filename;

      filename = g_value_get_string(value);

      if(filename == dssi_bridge->filename){
	return;
      }

      if(dssi_bridge->filename != NULL){
	g_free(dssi_bridge->filename);
      }

      dssi_bridge->filename = g_strdup(filename);
    }
    break;
  case PROP_EFFECT:
    {
      gchar *effect;
      
      effect = g_value_get_string(value);

      if(effect == dssi_bridge->effect){
	return;
      }

      if(dssi_bridge->effect != NULL){
	g_free(dssi_bridge->effect);
      }

      dssi_bridge->effect = g_strdup(effect);
    }
    break;
  case PROP_INDEX:
    {
      unsigned long *effect_index;
      
      effect_index = g_value_get_ulong(value);

      if(effect_index == dssi_bridge->effect_index){
	return;
      }

      dssi_bridge->effect_index = effect_index;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_dssi_bridge_get_property(GObject *gobject,
			     guint prop_id,
			     GValue *value,
			     GParamSpec *param_spec)
{
  AgsDssiBridge *dssi_bridge;

  dssi_bridge = AGS_DSSI_BRIDGE(gobject);

  switch(prop_id){
  case PROP_FILENAME:
    {
      g_value_set_string(value, dssi_bridge->filename);
    }
    break;
  case PROP_EFFECT:
    {
      g_value_set_string(value, dssi_bridge->effect);
    }
    break;
  case PROP_INDEX:
    {
      g_value_set_ulong(value, dssi_bridge->effect_index);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_dssi_bridge_connect(AgsConnectable *connectable)
{
  AgsDssiBridge *dssi_bridge;

  if((AGS_MACHINE_CONNECTED & (AGS_MACHINE(connectable)->flags)) != 0){
    return;
  }

  ags_dssi_bridge_parent_connectable_interface->connect(connectable);

  dssi_bridge = AGS_DSSI_BRIDGE(connectable);
}

void
ags_dssi_bridge_disconnect(AgsConnectable *connectable)
{
  //TODO:JK: implement me
}

gchar*
ags_dssi_bridge_get_version(AgsPlugin *plugin)
{
  return(AGS_DSSI_BRIDGE(plugin)->version);
}

void
ags_dssi_bridge_set_version(AgsPlugin *plugin, gchar *version)
{
  AgsDssiBridge *dssi_bridge;

  dssi_bridge = AGS_DSSI_BRIDGE(plugin);

  dssi_bridge->version = version;
}

gchar*
ags_dssi_bridge_get_build_id(AgsPlugin *plugin)
{
  return(AGS_DSSI_BRIDGE(plugin)->build_id);
}

void
ags_dssi_bridge_set_build_id(AgsPlugin *plugin, gchar *build_id)
{
  AgsDssiBridge *dssi_bridge;

  dssi_bridge = AGS_DSSI_BRIDGE(plugin);

  dssi_bridge->build_id = build_id;
}

void
ags_dssi_bridge_load(AgsDssiBridge *dssi_bridge)
{
  AgsDssiPlugin *dssi_plugin;

  AgsConfig *config;
  
  GList *list;

  gchar *str;

  void *plugin_so;
  DSSI_Descriptor_Function dssi_descriptor;
  DSSI_Descriptor *plugin_descriptor;
  DSSI_Program_Descriptor *program_descriptor;
  unsigned long samplerate;
  unsigned long effect_index;
  unsigned long i;

  config = ags_config_get_instance();
  
  str = ags_config_get_value(config,
			     AGS_CONFIG_SOUNDCARD,
			     "samplerate\0");
  
  if(str != NULL){
    samplerate = g_ascii_strtoull(str,
				  NULL,
				  10);
    free(str);
  }else{
    samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
  }

  /*  */
  list = gtk_container_get_children(AGS_EFFECT_BULK(AGS_EFFECT_BRIDGE(AGS_MACHINE(dssi_bridge)->bridge)->bulk_input)->table);
  
  while(list != NULL){
    gtk_widget_destroy(list->data);
    
    list = list->next;
  }

  g_message("ags_dssi_bridge.c - load %s %s\0",dssi_bridge->filename, dssi_bridge->effect);
 
  /* load plugin */
  ags_dssi_manager_load_file(dssi_bridge->filename);
  dssi_plugin = ags_dssi_manager_find_dssi_plugin(dssi_bridge->filename);

  plugin_so = dssi_plugin->plugin_so;

  /*  */
  gtk_list_store_clear(gtk_combo_box_get_model(dssi_bridge->program));
  
  /*  */
  effect_index = ags_dssi_manager_effect_index(dssi_bridge->filename,
					       dssi_bridge->effect);

  /* load ports */
  if(effect_index != -1 &&
     plugin_so){
    dssi_descriptor = (DSSI_Descriptor_Function) dlsym(plugin_so,
						       "dssi_descriptor\0");

    if(dlerror() == NULL && dssi_descriptor){
      plugin_descriptor = dssi_descriptor(effect_index);

      dssi_bridge->ladspa_handle = plugin_descriptor->LADSPA_Plugin->instantiate(plugin_descriptor->LADSPA_Plugin,
										 samplerate);
      
      if(plugin_descriptor->get_program != NULL){
	for(i = 0; (program_descriptor = plugin_descriptor->get_program(dssi_bridge->ladspa_handle, i)) != NULL; i++){
	  gtk_combo_box_text_append_text(dssi_bridge->program,
					 g_strdup_printf("%d,%d %s\0",
							 program_descriptor->Bank,
							 program_descriptor->Program,
							 program_descriptor->Name));
	}
      }
    }
  }

  /* add to effect bridge */
  ags_effect_bulk_add_effect(AGS_EFFECT_BRIDGE(AGS_MACHINE(dssi_bridge)->bridge)->bulk_input,
			     NULL,
			     dssi_bridge->filename,
			     dssi_bridge->effect);
}

/**
 * ags_dssi_bridge_new:
 * @soundcard: the assigned soundcard.
 * @filename: the plugin.so
 * @effect: the effect
 *
 * Creates an #AgsDssiBridge
 *
 * Returns: a new #AgsDssiBridge
 *
 * Since: 0.4.3
 */
AgsDssiBridge*
ags_dssi_bridge_new(GObject *soundcard,
		    gchar *filename,
		    gchar *effect)
{
  AgsDssiBridge *dssi_bridge;
  GValue value = {0,};

  dssi_bridge = (AgsDssiBridge *) g_object_new(AGS_TYPE_DSSI_BRIDGE,
					       NULL);

  if(soundcard != NULL){
    g_value_init(&value, G_TYPE_OBJECT);
    g_value_set_object(&value, soundcard);
    g_object_set_property(G_OBJECT(AGS_MACHINE(dssi_bridge)->audio),
			  "soundcard\0", &value);
    g_value_unset(&value);
  }

  g_object_set(dssi_bridge,
	       "filename\0", filename,
	       "effect\0", effect,
	       NULL);

  return(dssi_bridge);
}
