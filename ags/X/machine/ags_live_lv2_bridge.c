/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2017 Joël Krähemann
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

#include <ags/X/machine/ags_live_lv2_bridge.h>
#include <ags/X/machine/ags_live_lv2_bridge_callbacks.h>

#include <ags/util/ags_id_generator.h>

#include <ags/lib/ags_string_util.h>

#include <ags/object/ags_application_context.h>
#include <ags/object/ags_marshal.h>
#include <ags/object/ags_connectable.h>
#include <ags/object/ags_config.h>
#include <ags/object/ags_soundcard.h>
#include <ags/object/ags_plugin.h>
#include <ags/object/ags_seekable.h>

#include <ags/file/ags_file.h>
#include <ags/file/ags_file_stock.h>
#include <ags/file/ags_file_id_ref.h>
#include <ags/file/ags_file_launch.h>

#include <ags/thread/ags_mutex_manager.h>
#include <ags/thread/ags_thread-posix.h>

#include <ags/plugin/ags_lv2_manager.h>
#include <ags/plugin/ags_lv2ui_manager.h>
#include <ags/plugin/ags_base_plugin.h>
#include <ags/plugin/ags_lv2_plugin.h>
#include <ags/plugin/ags_lv2_preset.h>
#include <ags/plugin/ags_lv2ui_plugin.h>

#include <ags/audio/ags_input.h>
#include <ags/audio/ags_recall_factory.h>
#include <ags/audio/ags_recall.h>
#include <ags/audio/ags_recall_lv2.h>
#include <ags/audio/ags_recall_container.h>

#include <ags/audio/recall/ags_delay_audio.h>
#include <ags/audio/recall/ags_delay_audio_run.h>
#include <ags/audio/recall/ags_count_beats_audio.h>
#include <ags/audio/recall/ags_count_beats_audio_run.h>
#include <ags/audio/recall/ags_play_lv2_audio.h>
#include <ags/audio/recall/ags_play_lv2_audio_run.h>
#include <ags/audio/recall/ags_record_midi_audio.h>
#include <ags/audio/recall/ags_record_midi_audio_run.h>

#include <ags/widget/ags_dial.h>

#include <ags/X/ags_window.h>
#include <ags/X/ags_effect_bridge.h>
#include <ags/X/ags_effect_bulk.h>
#include <ags/X/ags_bulk_member.h>

void ags_live_lv2_bridge_class_init(AgsLiveLv2BridgeClass *live_lv2_bridge);
void ags_live_lv2_bridge_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_live_lv2_bridge_plugin_interface_init(AgsPluginInterface *plugin);
void ags_live_lv2_bridge_init(AgsLiveLv2Bridge *live_lv2_bridge);
void ags_live_lv2_bridge_set_property(GObject *gobject,
				      guint prop_id,
				      const GValue *value,
				      GParamSpec *param_spec);
void ags_live_lv2_bridge_get_property(GObject *gobject,
				      guint prop_id,
				      GValue *value,
				      GParamSpec *param_spec);
void ags_live_lv2_bridge_finalize(GObject *gobject);
void ags_live_lv2_bridge_connect(AgsConnectable *connectable);
void ags_live_lv2_bridge_disconnect(AgsConnectable *connectable);
gchar* ags_live_lv2_bridge_get_version(AgsPlugin *plugin);
void ags_live_lv2_bridge_set_version(AgsPlugin *plugin, gchar *version);
gchar* ags_live_lv2_bridge_get_build_id(AgsPlugin *plugin);
void ags_live_lv2_bridge_set_build_id(AgsPlugin *plugin, gchar *build_id);
gchar* ags_live_lv2_bridge_get_xml_type(AgsPlugin *plugin);
void ags_live_lv2_bridge_set_xml_type(AgsPlugin *plugin, gchar *xml_type);
void ags_live_lv2_bridge_read(AgsFile *file, xmlNode *node, AgsPlugin *plugin);
void ags_live_lv2_bridge_launch_task(AgsFileLaunch *file_launch, AgsLiveLv2Bridge *live_lv2_bridge);
xmlNode* ags_live_lv2_bridge_write(AgsFile *file, xmlNode *parent, AgsPlugin *plugin);

void ags_live_lv2_bridge_set_audio_channels(AgsAudio *audio,
					    guint audio_channels, guint audio_channels_old,
					    gpointer data);
void ags_live_lv2_bridge_set_pads(AgsAudio *audio, GType type,
				  guint pads, guint pads_old,
				  gpointer data);

void ags_live_lv2_bridge_map_recall(AgsMachine *machine);

/**
 * SECTION:ags_live_lv2_bridge
 * @short_description: A composite widget to visualize a bunch of #AgsChannel
 * @title: AgsLiveLv2Bridge
 * @section_id:
 * @include: ags/X/ags_live_lv2_bridge.h
 *
 * #AgsLiveLv2Bridge is a composite widget to visualize all #AgsChannel. It should be
 * packed by an #AgsMachine.
 */

enum{
  PROP_0,
  PROP_FILENAME,
  PROP_EFFECT,
  PROP_URI,
  PROP_INDEX,
  PROP_HAS_MIDI,
  PROP_HAS_GUI,
  PROP_GUI_FILENAME,
  PROP_GUI_URI,
};

static gpointer ags_live_lv2_bridge_parent_class = NULL;
static AgsConnectableInterface* ags_live_lv2_bridge_parent_connectable_interface;
static AgsPluginInterface* ags_live_lv2_bridge_parent_plugin_interface;

GHashTable *ags_live_lv2_bridge_lv2ui_idle = NULL;

GType
ags_live_lv2_bridge_get_type(void)
{
  static GType ags_type_live_lv2_bridge = 0;

  if(!ags_type_live_lv2_bridge){
    static const GTypeInfo ags_live_lv2_bridge_info = {
      sizeof(AgsLiveLv2BridgeClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_live_lv2_bridge_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsLiveLv2Bridge),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_live_lv2_bridge_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_live_lv2_bridge_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_plugin_interface_info = {
      (GInterfaceInitFunc) ags_live_lv2_bridge_plugin_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_live_lv2_bridge = g_type_register_static(AGS_TYPE_MACHINE,
						      "AgsLiveLv2Bridge\0", &ags_live_lv2_bridge_info,
						      0);

    g_type_add_interface_static(ags_type_live_lv2_bridge,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_live_lv2_bridge,
				AGS_TYPE_PLUGIN,
				&ags_plugin_interface_info);
  }

  return(ags_type_live_lv2_bridge);
}

void
ags_live_lv2_bridge_class_init(AgsLiveLv2BridgeClass *live_lv2_bridge)
{
  AgsMachineClass *machine;
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_live_lv2_bridge_parent_class = g_type_class_peek_parent(live_lv2_bridge);

  /* GObjectClass */
  gobject = G_OBJECT_CLASS(live_lv2_bridge);

  gobject->set_property = ags_live_lv2_bridge_set_property;
  gobject->get_property = ags_live_lv2_bridge_get_property;

  gobject->finalize = ags_live_lv2_bridge_finalize;
  
  /* properties */
  /**
   * AgsRecallLiveLv2:filename:
   *
   * The plugin's filename.
   * 
   * Since: 0.7.134
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
   * AgsRecallLiveLv2:effect:
   *
   * The effect's name.
   * 
   * Since: 0.7.134
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
   * AgsRecallLiveLv2:uri:
   *
   * The uri's name.
   * 
   * Since: 0.7.134
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
   * AgsRecallLiveLv2:index:
   *
   * The uri's index.
   * 
   * Since: 0.7.134
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

  /**
   * AgsRecallLiveLv2:has-midi:
   *
   * If has-midi is set to %TRUE appropriate flag is set
   * to audio in order to become a sequencer.
   * 
   * Since: 0.7.134
   */
  param_spec =  g_param_spec_boolean("has-midi\0",
				     "has-midi\0",
				     "If effect has-midi\0",
				     FALSE,
				     G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_HAS_MIDI,
				  param_spec);

  /**
   * AgsRecallLiveLv2:has-gui:
   *
   * If has-gui is set to %TRUE 128 inputs are allocated and appropriate flag is set
   * to audio in order to become a sequencer.
   * 
   * Since: 0.7.134
   */
  param_spec =  g_param_spec_boolean("has-gui\0",
				     "has-gui\0",
				     "If effect has-gui\0",
				     FALSE,
				     G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_HAS_GUI,
				  param_spec);

  /**
   * AgsRecallLiveLv2:gui-filename:
   *
   * The plugin's GUI filename.
   * 
   * Since: 0.7.134
   */
  param_spec =  g_param_spec_string("gui-filename\0",
				    "the GUI object file\0",
				    "The filename as string of GUI object file\0",
				    NULL,
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_GUI_FILENAME,
				  param_spec);

  /**
   * AgsRecallLiveLv2:gui-uri:
   *
   * The GUI's uri name.
   * 
   * Since: 0.7.134
   */
  param_spec =  g_param_spec_string("gui-uri\0",
				    "the gui-uri\0",
				    "The gui-uri's string representation\0",
				    NULL,
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_GUI_URI,
				  param_spec);

  /* AgsMachine */
  machine = (AgsMachineClass *) live_lv2_bridge;

  machine->map_recall = ags_live_lv2_bridge_map_recall;
}

void
ags_live_lv2_bridge_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_live_lv2_bridge_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_live_lv2_bridge_connect;
  connectable->disconnect = ags_live_lv2_bridge_disconnect;
}

void
ags_live_lv2_bridge_plugin_interface_init(AgsPluginInterface *plugin)
{
  plugin->get_name = NULL;
  plugin->set_name = NULL;
  plugin->get_version = ags_live_lv2_bridge_get_version;
  plugin->set_version = ags_live_lv2_bridge_set_version;
  plugin->get_build_id = ags_live_lv2_bridge_get_build_id;
  plugin->set_build_id = ags_live_lv2_bridge_set_build_id;
  plugin->get_xml_type = NULL;
  plugin->set_xml_type = NULL;
  plugin->get_ports = NULL;
  plugin->read = NULL;
  plugin->write = NULL;
  plugin->set_ports = NULL;
}

void
ags_live_lv2_bridge_init(AgsLiveLv2Bridge *live_lv2_bridge)
{
  GtkTable *table;
  GtkImageMenuItem *item;

  AgsAudio *audio;

  if(ags_live_lv2_bridge_live_lv2ui_idle == NULL){
    ags_live_lv2_bridge_live_lv2ui_idle = g_hash_table_new_full(g_direct_hash, g_direct_equal,
								NULL,
								NULL);
  }

  audio = AGS_MACHINE(live_lv2_bridge)->audio;
  audio->flags |= (AGS_AUDIO_SYNC);

  g_signal_connect_after(G_OBJECT(audio), "set_audio_channels\0",
			 G_CALLBACK(ags_live_lv2_bridge_set_audio_channels), NULL);
  
  g_signal_connect_after(G_OBJECT(audio), "set_pads\0",
			 G_CALLBACK(ags_live_lv2_bridge_set_pads), NULL);
  
  live_lv2_bridge->flags = 0;

  live_lv2_bridge->name = NULL;

  live_lv2_bridge->version = AGS_LIVE_LV2_BRIDGE_DEFAULT_VERSION;
  live_lv2_bridge->build_id = AGS_LIVE_LV2_BRIDGE_DEFAULT_BUILD_ID;

  live_lv2_bridge->xml_type = "ags-live-lv2-bridge\0";
  
  live_lv2_bridge->mapped_output_pad = 0;
  live_lv2_bridge->mapped_input_pad = 0;
  
  live_lv2_bridge->filename = NULL;
  live_lv2_bridge->effect = NULL;
  live_lv2_bridge->uri = NULL;
  live_lv2_bridge->uri_index = 0;

  live_lv2_bridge->has_midi = FALSE;

  live_lv2_bridge->vbox = (GtkVBox *) gtk_vbox_new(FALSE, 0);
  gtk_container_add((GtkContainer *) gtk_bin_get_child((GtkBin *) live_lv2_bridge),
		    (GtkWidget *) live_lv2_bridge->vbox);

  live_lv2_bridge->preset = NULL;
  
  /* effect bridge */  
  AGS_MACHINE(live_lv2_bridge)->bridge = (GtkContainer *) ags_effect_bridge_new(audio);
  gtk_box_pack_start((GtkBox *) live_lv2_bridge->vbox,
		     (GtkWidget *) AGS_MACHINE(live_lv2_bridge)->bridge,
		     FALSE, FALSE,
		     0);

  table = (GtkTable *) gtk_table_new(1, 2, FALSE);
  gtk_box_pack_start((GtkBox *) AGS_EFFECT_BRIDGE(AGS_MACHINE(live_lv2_bridge)->bridge),
		     (GtkWidget *) table,
		     FALSE, FALSE,
		     0);

  AGS_EFFECT_BRIDGE(AGS_MACHINE(live_lv2_bridge)->bridge)->bulk_input = (GtkWidget *) ags_effect_bulk_new(audio,
													  AGS_TYPE_INPUT);
  AGS_EFFECT_BULK(AGS_EFFECT_BRIDGE(AGS_MACHINE(live_lv2_bridge)->bridge)->bulk_input)->flags |= (AGS_EFFECT_BULK_HIDE_BUTTONS |
												  AGS_EFFECT_BULK_HIDE_ENTRIES |
												  AGS_EFFECT_BULK_SHOW_LABELS);
  gtk_table_attach(table,
		   (GtkWidget *) AGS_EFFECT_BRIDGE(AGS_MACHINE(live_lv2_bridge)->bridge)->bulk_input,
		   0, 1,
		   0, 1,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  live_lv2_bridge->has_gui = FALSE;
  live_lv2_bridge->gui_filename = NULL;
  live_lv2_bridge->gui_uri = NULL;

  live_lv2_bridge->ui_handle = NULL;
  
  live_lv2_bridge->lv2_gui = NULL;

  /* lv2 menu */
  item = (GtkImageMenuItem *) gtk_image_menu_item_new_with_label("Lv2\0");
  gtk_menu_shell_append((GtkMenuShell *) AGS_MACHINE(lv2_bridge)->popup,
			(GtkWidget *) item);
  gtk_widget_show((GtkWidget *) item);
  
  live_lv2_bridge->lv2_menu = (GtkMenu *) gtk_menu_new();
  gtk_menu_item_set_submenu((GtkMenuItem *) item,
			    (GtkWidget *) live_lv2_bridge->lv2_menu);

  item = (GtkImageMenuItem *) gtk_image_menu_item_new_with_label("show GUI\0");
  gtk_menu_shell_append((GtkMenuShell *) live_lv2_bridge->lv2_menu,
			(GtkWidget *) item);

  gtk_widget_show_all((GtkWidget *) live_lv2_bridge->lv2_menu);
}

void
ags_live_lv2_bridge_set_property(GObject *gobject,
				 guint prop_id,
				 const GValue *value,
				 GParamSpec *param_spec)
{
  AgsLiveLv2Bridge *live_lv2_bridge;

  live_lv2_bridge = AGS_LIVE_LV2_BRIDGE(gobject);

  switch(prop_id){
  case PROP_FILENAME:
    {
      gchar *filename;

      filename = g_value_get_string(value);

      if(filename == live_lv2_bridge->filename){
	return;
      }

      if(live_lv2_bridge->filename != NULL){
	g_free(live_lv2_bridge->filename);
      }

      if(filename != NULL){
	if(!g_file_test(filename,
			G_FILE_TEST_EXISTS)){
	  AgsWindow *window;

	  window = (AgsWindow *) gtk_widget_get_toplevel((GtkWidget *) live_lv2_bridge);

	  ags_window_show_error(window,
				g_strdup_printf("Plugin file not present %s\0",
						filename));
	}
      }

      live_lv2_bridge->filename = g_strdup(filename);
    }
    break;
  case PROP_EFFECT:
    {
      gchar *effect;
      
      effect = g_value_get_string(value);

      if(effect == live_lv2_bridge->effect){
	return;
      }

      if(live_lv2_bridge->effect != NULL){
	g_free(live_lv2_bridge->effect);
      }

      live_lv2_bridge->effect = g_strdup(effect);
    }
    break;
  case PROP_URI:
    {
      gchar *uri;
      
      uri = g_value_get_string(value);

      if(uri == live_lv2_bridge->uri){
	return;
      }

      if(live_lv2_bridge->uri != NULL){
	g_free(live_lv2_bridge->uri);
      }

      live_lv2_bridge->uri = g_strdup(uri);
    }
    break;
  case PROP_INDEX:
    {
      unsigned long uri_index;
      
      uri_index = g_value_get_ulong(value);

      if(uri_index == live_lv2_bridge->uri_index){
	return;
      }

      live_lv2_bridge->uri_index = uri_index;
    }
    break;
  case PROP_HAS_MIDI:
    {
      gboolean has_midi;

      has_midi = g_value_get_boolean(value);

      if(live_lv2_bridge->has_midi == has_midi){
	return;
      }

      live_lv2_bridge->has_midi = has_midi;
    }
    break;
  case PROP_HAS_GUI:
    {
      GtkWindow *window;
      gboolean has_gui;

      has_gui = g_value_get_boolean(value);

      if(live_lv2_bridge->has_gui == has_gui){
	return;
      }

      live_lv2_bridge->has_gui = has_gui;
    }
    break;
  case PROP_GUI_FILENAME:
    {
      GtkWindow *window;
      
      gchar *gui_filename;

      gui_filename = g_value_get_string(value);

      if(live_lv2_bridge->gui_filename == gui_filename){
	return;
      }

      live_lv2_bridge->gui_filename = g_strdup(gui_filename);
    }
    break;
  case PROP_GUI_URI:
    {
      gchar *gui_uri;
      
      gui_uri = g_value_get_string(value);

      if(gui_uri == live_lv2_bridge->gui_uri){
	return;
      }

      if(live_lv2_bridge->gui_uri != NULL){
	g_free(live_lv2_bridge->gui_uri);
      }

      live_lv2_bridge->gui_uri = g_strdup(gui_uri);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_live_lv2_bridge_get_property(GObject *gobject,
				 guint prop_id,
				 GValue *value,
				 GParamSpec *param_spec)
{
  AgsLiveLv2Bridge *live_lv2_bridge;

  live_lv2_bridge = AGS_LIVE_LV2_BRIDGE(gobject);

  switch(prop_id){
  case PROP_FILENAME:
    {
      g_value_set_string(value, live_lv2_bridge->filename);
    }
    break;
  case PROP_EFFECT:
    {
      g_value_set_string(value, live_lv2_bridge->effect);
    }
    break;
  case PROP_URI:
    {
      g_value_set_string(value, live_lv2_bridge->uri);
    }
    break;
  case PROP_INDEX:
    {
      g_value_set_ulong(value, live_lv2_bridge->uri_index);
    }
    break;
  case PROP_HAS_MIDI:
    {
      g_value_set_boolean(value, live_lv2_bridge->has_midi);
    }
    break;
  case PROP_HAS_GUI:
    {
      g_value_set_boolean(value, live_lv2_bridge->has_gui);
    }
    break;
  case PROP_GUI_FILENAME:
    {
      g_value_set_string(value, live_lv2_bridge->gui_filename);
    }
    break;
  case PROP_GUI_URI:
    {
      g_value_set_string(value, live_lv2_bridge->gui_uri);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_live_lv2_bridge_finalize(GObject *gobject)
{
  AgsLiveLv2Bridge *live_lv2_bridge;

  live_lv2_bridge = AGS_LIVE_LV2_BRIDGE(gobject);
  
  if(live_lv2_bridge->ui_handle != NULL){
    g_hash_table_remove(ags_live_lv2_bridge_lv2ui_idle,
			live_lv2_bridge);
  }
  
  G_OBJECT_CLASS(ags_live_lv2_bridge_parent_class)->finalize(gobject);
}

void
ags_live_lv2_bridge_connect(AgsConnectable *connectable)
{
  AgsLiveLv2Bridge *live_lv2_bridge;

  GList *list;
  
  if((AGS_MACHINE_CONNECTED & (AGS_MACHINE(connectable)->flags)) != 0){
    return;
  }

  ags_live_lv2_bridge_parent_connectable_interface->connect(connectable);

  live_lv2_bridge = AGS_LIVE_LV2_BRIDGE(connectable);

  /* menu */
  list = gtk_container_get_children((GtkContainer *) live_lv2_bridge->lv2_menu);

  g_signal_connect(G_OBJECT(list->data), "activate\0",
		   G_CALLBACK(ags_live_lv2_bridge_show_gui_callback), live_lv2_bridge);
}

void
ags_live_lv2_bridge_disconnect(AgsConnectable *connectable)
{
  //TODO:JK: implement me
}

gchar*
ags_live_lv2_bridge_get_version(AgsPlugin *plugin)
{
  return(AGS_LIVE_LV2_BRIDGE(plugin)->version);
}

void
ags_live_lv2_bridge_set_version(AgsPlugin *plugin, gchar *version)
{
  AgsLiveLv2Bridge *live_lv2_bridge;

  live_lv2_bridge = AGS_LIVE_LV2_BRIDGE(plugin);

  live_lv2_bridge->version = version;
}

gchar*
ags_live_lv2_bridge_get_build_id(AgsPlugin *plugin)
{
  return(AGS_LIVE_LV2_BRIDGE(plugin)->build_id);
}

void
ags_live_lv2_bridge_set_build_id(AgsPlugin *plugin, gchar *build_id)
{
  AgsLiveLv2Bridge *live_lv2_bridge;

  live_lv2_bridge = AGS_LIVE_LV2_BRIDGE(plugin);

  live_lv2_bridge->build_id = build_id;
}


gchar*
ags_live_lv2_bridge_get_xml_type(AgsPlugin *plugin)
{
  return(AGS_LIVE_LV2_BRIDGE(plugin)->xml_type);
}

void
ags_live_lv2_bridge_set_xml_type(AgsPlugin *plugin, gchar *xml_type)
{
  AGS_LIVE_LV2_BRIDGE(plugin)->xml_type = xml_type;
}

void
ags_live_lv2_bridge_read(AgsFile *file, xmlNode *node, AgsPlugin *plugin)
{
  AgsLiveLv2Bridge *gobject;
  AgsFileLaunch *file_launch;

  gobject = AGS_LIVE_LV2_BRIDGE(plugin);

  g_object_set(gobject,
	       "filename\0", xmlGetProp(node,
					"filename\0"),
	       "effect\0", xmlGetProp(node,
				      "effect\0"),
	       NULL);

  /* launch */
  file_launch = (AgsFileLaunch *) g_object_new(AGS_TYPE_FILE_LAUNCH,
					       "node\0", node,
					       NULL);
  g_signal_connect(G_OBJECT(file_launch), "start\0",
		   G_CALLBACK(ags_live_lv2_bridge_launch_task), gobject);
  ags_file_add_launch(file,
		      G_OBJECT(file_launch));
}

void
ags_live_lv2_bridge_launch_task(AgsFileLaunch *file_launch, AgsLiveLv2Bridge *live_lv2_bridge)
{
  GtkTreeModel *model;

  GtkTreeIter iter;

  GList *list, *list_start;
  GList *recall;
  
  ags_live_lv2_bridge_load(live_lv2_bridge);

  /* block update bulk port */
  list_start = 
    list = gtk_container_get_children((GtkContainer *) AGS_EFFECT_BULK(AGS_EFFECT_BRIDGE(AGS_MACHINE(live_lv2_bridge)->bridge)->bulk_input)->table);

  while(list != NULL){
    if(AGS_IS_BULK_MEMBER(list->data)){
      AGS_BULK_MEMBER(list->data)->flags |= AGS_BULK_MEMBER_NO_UPDATE;
    }

    list = list->next;
  }

  /* update value and unblock update bulk port */
  recall = NULL;
  
  if(AGS_MACHINE(live_lv2_bridge)->audio->input != NULL){
    recall = AGS_MACHINE(live_lv2_bridge)->audio->input->recall;
    
    while((recall = ags_recall_template_find_type(recall, AGS_TYPE_RECALL_LIVE_LV2)) != NULL){
      if(!g_strcmp0(AGS_PLAY_LV2_AUDIO(recall->data)->filename,
		    live_lv2_bridge->filename) &&
	 !g_strcmp0(AGS_PLAY_LV2_AUDIO(recall->data)->effect,
		    live_lv2_bridge->effect)){
	break;
      }

      recall = recall->next;
    }
  }

  while(list != NULL){
    if(AGS_IS_BULK_MEMBER(list->data)){
      GtkWidget *child_widget;
      
      GList *port;

      child_widget = gtk_bin_get_child(list->data);
      
      if(recall != NULL){
	port = AGS_RECALL(recall->data)->port;

	while(port != port->next){
	  if(!g_strcmp0(AGS_BULK_MEMBER(list->data)->specifier,
			AGS_PORT(port->data)->specifier)){
	    if(AGS_IS_DIAL(child_widget)){
	      gtk_adjustment_set_value(AGS_DIAL(child_widget)->adjustment,
				       AGS_PORT(port->data)->port_value.ags_port_ladspa);
	      ags_dial_draw((AgsDial *) child_widget);
	    }else if(GTK_IS_TOGGLE_BUTTON(child_widget)){
	      gtk_toggle_button_set_active((GtkToggleButton *) child_widget,
					   ((AGS_PORT(port->data)->port_value.ags_port_ladspa != 0.0) ? TRUE: FALSE));
	    }

	    break;
	  }

	  port = port->next;
	}
      }
     
      AGS_BULK_MEMBER(list->data)->flags &= (~AGS_BULK_MEMBER_NO_UPDATE);
    }
    
    list = list->next;
  }

  g_list_free(list_start);
}

xmlNode*
ags_live_lv2_bridge_write(AgsFile *file, xmlNode *parent, AgsPlugin *plugin)
{
  AgsLiveLv2Bridge *live_lv2_bridge;

  xmlNode *node;

  gchar *id;
  
  live_lv2_bridge = AGS_LIVE_LV2_BRIDGE(plugin);

  id = ags_id_generator_create_uuid();
    
  node = xmlNewNode(NULL,
		    "ags-live-lv2-bridge\0");
  xmlNewProp(node,
	     AGS_FILE_ID_PROP,
	     id);

  xmlNewProp(node,
	     "filename\0",
	     live_lv2_bridge->filename);

  xmlNewProp(node,
	     "effect\0",
	     live_lv2_bridge->effect);
  
  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context\0", file->application_context,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", id),
				   "reference\0", live_lv2_bridge,
				   NULL));

  xmlAddChild(parent,
	      node);

  return(node);
}

void
ags_live_lv2_bridge_set_audio_channels(AgsAudio *audio,
				       guint audio_channels, guint audio_channels_old,
				       gpointer data)
{
  AgsMachine *machine;
  AgsLiveLv2Bridge *live_lv2_bridge;

  AgsChannel *channel, *next_pad;
  AgsAudioSignal *audio_signal;  

  /* get machine */
  live_lv2_bridge = (AgsLiveLv2Bridge *) audio->machine;
  machine = AGS_MACHINE(live_lv2_bridge);
  
  if(audio->input_pads == 0 &&
     audio->output_pads == 0){
    return;
  }

  if(audio_channels > audio_channels_old){
    /* AgsInput */
    channel = audio->input;

    while(channel != NULL){
      next_pad = channel->next_pad;
      channel = ags_channel_nth(channel,
				audio_channels_old);

      while(channel != next_pad){
	audio_signal = ags_audio_signal_new(audio->soundcard,
					    (GObject *) channel->first_recycling,
					    NULL);
	audio_signal->flags |= AGS_AUDIO_SIGNAL_TEMPLATE;
	//	audio_signal->loop_start = 0;
	//	audio_signal->loop_end = audio_signal->buffer_size;
	ags_audio_signal_stream_resize(audio_signal,
				       1);
	ags_recycling_add_audio_signal(channel->first_recycling,
				       audio_signal);
	
	channel = channel->next;
      }
    }

    /* AgsOutput */
    channel = audio->output;

    while(channel != NULL){
      next_pad = channel->next_pad;
      channel = ags_channel_pad_nth(channel,
				    audio_channels_old);

      while(channel != next_pad){
	audio_signal = ags_audio_signal_new(audio->soundcard,
					    (GObject *) channel->first_recycling,
					    NULL);
	audio_signal->flags |= AGS_AUDIO_SIGNAL_TEMPLATE;
	ags_audio_signal_stream_resize(audio_signal,
				       3);
	ags_recycling_add_audio_signal(channel->first_recycling,
				       audio_signal);
	
	channel = channel->next;
      }
    }

    /* recall */
    if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0){
      ags_live_lv2_bridge_input_map_recall(live_lv2_bridge,
					   audio_channels_old,
					   0);

      ags_live_lv2_bridge_output_map_recall(live_lv2_bridge,
					    audio_channels_old,
					    0);
    }
  }
}

void
ags_live_lv2_bridge_set_pads(AgsAudio *audio, GType type,
			     guint pads, guint pads_old,
			     gpointer data)
{
  AgsMachine *machine;
  AgsLiveLv2Bridge *live_lv2_bridge;

  AgsChannel *channel;
  AgsAudioSignal *audio_signal;
  
  gboolean grow;

  if(pads == pads_old ||
     audio->audio_channels == 0){
    return;
  }

  /* get machine */
  live_lv2_bridge = (AgsLiveLv2Bridge *) audio->machine;
  machine = AGS_MACHINE(live_lv2_bridge);

  if(pads_old < pads){
    grow = TRUE;
  }else{
    grow = FALSE;
  }
  
  if(g_type_is_a(type, AGS_TYPE_INPUT)){
    if(grow){
      /* AgsInput */
      channel = ags_channel_pad_nth(audio->input,
				    pads_old);

      while(channel != NULL){
	audio_signal = ags_audio_signal_new(audio->soundcard,
					    (GObject *) channel->first_recycling,
					    NULL);
	audio_signal->flags |= AGS_AUDIO_SIGNAL_TEMPLATE;
	//	audio_signal->loop_start = 0;
	//	audio_signal->loop_end = audio_signal->buffer_size;
	ags_audio_signal_stream_resize(audio_signal,
				       1);
	ags_recycling_add_audio_signal(channel->first_recycling,
				       audio_signal);
	
	channel = channel->next;
      }

      /* recall */
      if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0){
	ags_live_lv2_bridge_input_map_recall(live_lv2_bridge,
					     0,
					     pads_old);
      }
    }else{
      live_lv2_bridge->mapped_input_pad = pads;
    }
  }else{
    if(grow){
      /* AgsOutput */
      channel = ags_channel_pad_nth(audio->output,
				    pads_old);

      while(channel != NULL){
	audio_signal = ags_audio_signal_new(audio->soundcard,
					    (GObject *) channel->first_recycling,
					    NULL);
	audio_signal->flags |= AGS_AUDIO_SIGNAL_TEMPLATE;
	ags_audio_signal_stream_resize(audio_signal,
				       3);
	ags_recycling_add_audio_signal(channel->first_recycling,
				       audio_signal);
	
	channel = channel->next;
      }

      /* recall */
      if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0){
	ags_live_lv2_bridge_output_map_recall(live_lv2_bridge,
					      0,
					      pads_old);
      }
    }else{
      live_lv2_bridge->mapped_output_pad = pads;
    }
  }
}

void
ags_live_lv2_bridge_map_recall(AgsMachine *machine)
{  
  AgsWindow *window;
  AgsLiveLv2Bridge *live_lv2_bridge;
  
  AgsAudio *audio;

  AgsDelayAudio *play_delay_audio;
  AgsDelayAudioRun *play_delay_audio_run;
  AgsCountBeatsAudio *play_count_beats_audio;
  AgsCountBeatsAudioRun *play_count_beats_audio_run;
  AgsRecordMidiAudio *recall_record_midi_audio;
  AgsRecordMidiAudioRun *recall_record_midi_audio_run;
  AgsPlayLv2Audio *recall_lv2_audio;
  AgsPlayLv2AudioRun *recall_lv2_audio_run;

  GList *list;
  
  if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0 ||
     (AGS_MACHINE_PREMAPPED_RECALL & (machine->flags)) != 0){
    return;
  }

  window = (AgsWindow *) gtk_widget_get_ancestor((GtkWidget *) machine,
						 AGS_TYPE_WINDOW);

  live_lv2_bridge = (AgsLiveLv2Bridge *) machine;
  audio = machine->audio;

  /* ags-delay */
  if((AGS_MACHINE_IS_SYNTHESIZER & (machine->flags)) != 0){
    ags_recall_factory_create(audio,
			      NULL, NULL,
			      "ags-delay\0",
			      0, 0,
			      0, 0,
			      (AGS_RECALL_FACTORY_OUTPUT |
			       AGS_RECALL_FACTORY_ADD |
			       AGS_RECALL_FACTORY_PLAY |
			       AGS_RECALL_FACTORY_RECALL),
			      0);

    list = ags_recall_find_type(audio->play,
				AGS_TYPE_DELAY_AUDIO_RUN);

    if(list != NULL){
      play_delay_audio_run = AGS_DELAY_AUDIO_RUN(list->data);
      //    AGS_RECALL(play_delay_audio_run)->flags |= AGS_RECALL_PERSISTENT;
    }else{
      play_delay_audio_run = NULL;
    }
  
    /* ags-count-beats */
    ags_recall_factory_create(audio,
			      NULL, NULL,
			      "ags-count-beats\0",
			      0, 0,
			      0, 0,
			      (AGS_RECALL_FACTORY_OUTPUT |
			       AGS_RECALL_FACTORY_ADD |
			       AGS_RECALL_FACTORY_PLAY |
			       AGS_RECALL_FACTORY_RECALL),
			      0);
  
    list = ags_recall_find_type(audio->play,
				AGS_TYPE_COUNT_BEATS_AUDIO_RUN);

    if(list != NULL){
      play_count_beats_audio_run = AGS_COUNT_BEATS_AUDIO_RUN(list->data);

      /* set dependency */  
      g_object_set(G_OBJECT(play_count_beats_audio_run),
		   "delay-audio-run\0", play_delay_audio_run,
		   NULL);
      ags_seekable_seek(AGS_SEEKABLE(play_count_beats_audio_run),
			window->navigation->position_tact->adjustment->value,
			TRUE);
    }else{
      play_count_beats_audio_run = NULL;
    }

    /* ags-record-midi */
    ags_recall_factory_create(audio,
			      NULL, NULL,
			      "ags-record-midi\0",
			      0, 0,
			      0, 0,
			      (AGS_RECALL_FACTORY_INPUT |
			       AGS_RECALL_FACTORY_ADD |
			       AGS_RECALL_FACTORY_RECALL),
			      0);

    list = ags_recall_find_type(audio->recall, AGS_TYPE_RECORD_MIDI_AUDIO_RUN);

    if(list != NULL){
      recall_record_midi_audio_run = AGS_RECORD_MIDI_AUDIO_RUN(list->data);
    
      /* set dependency */
      g_object_set(G_OBJECT(recall_record_midi_audio_run),
		   "delay-audio-run\0", play_delay_audio_run,
		   NULL);

      /* set dependency */
      g_object_set(G_OBJECT(recall_record_midi_audio_run),
		   "count-beats-audio-run\0", play_count_beats_audio_run,
		   NULL);
    }  

    /* ags-play-notation */
    ags_recall_factory_create(audio,
			      NULL, NULL,
			      "ags-play-notation\0",
			      0, 0,
			      0, 0,
			      (AGS_RECALL_FACTORY_INPUT |
			       AGS_RECALL_FACTORY_ADD |
			       AGS_RECALL_FACTORY_RECALL),
			      0);

    list = ags_recall_find_type(audio->recall,
				AGS_TYPE_PLAY_NOTATION_AUDIO_RUN);

    if(list != NULL){
      recall_notation_audio_run = AGS_PLAY_NOTATION_AUDIO_RUN(list->data);

      /* set dependency */
      g_object_set(G_OBJECT(recall_notation_audio_run),
		   "delay-audio-run\0", play_delay_audio_run,
		   NULL);

      /* set dependency */
      g_object_set(G_OBJECT(recall_notation_audio_run),
		   "count-beats-audio-run\0", play_count_beats_audio_run,
		   NULL);
    }
  }
  
  /* depending on destination */
  ags_live_lv2_bridge_input_map_recall(live_lv2_bridge, 0, 0);

  /* add new controls */
  ags_effect_bulk_add_effect((AgsEffectBulk *) AGS_EFFECT_BRIDGE(AGS_MACHINE(live_lv2_bridge)->bridge)->bulk_input,
			     NULL,
			     live_lv2_bridge->filename,
			     live_lv2_bridge->effect);

  /* depending on destination */
  ags_live_lv2_bridge_output_map_recall(live_lv2_bridge, 0, 0);

  /* call parent */
  AGS_MACHINE_CLASS(ags_live_lv2_bridge_parent_class)->map_recall(machine);
}

void
ags_live_lv2_bridge_input_map_recall(AgsLiveLv2Bridge *live_lv2_bridge, guint audio_channel_start, guint input_pad_start)
{
  AgsAudio *audio;
  AgsChannel *source, *current;
  
  AgsConfig *config;
  
  GList *list;

  gchar *str;

  gboolean performance_mode;

  audio = AGS_MACHINE(live_lv2_bridge)->audio;

  if(live_lv2_bridge->mapped_input_pad > input_pad_start){
    return;
  }

  source = ags_channel_nth(audio->input,
			   audio_channel_start + input_pad_start * audio->audio_channels);

  config = ags_config_get_instance();

  if((AGS_MACHINE_IS_SYNTHESIZER & (AGS_MACHINE(live_lv2_bridge)->flags)) != 0){
    /* map dependending on output */
    str = ags_config_get_value(config,
			       AGS_CONFIG_GENERIC,
			       "engine-mode\0");
    performance_mode = FALSE;
  
    if(str != NULL &&
       !g_ascii_strncasecmp(str,
			    "performance\0",
			    12)){
      current = source;

      while(current != NULL){
	/* ags-copy */
	ags_recall_factory_create(audio,
				  NULL, NULL,
				  "ags-copy\0",
				  audio_channel_start, audio->audio_channels, 
				  current->pad, current->pad + 1,
				  (AGS_RECALL_FACTORY_INPUT |
				   AGS_RECALL_FACTORY_RECALL |
				   AGS_RECALL_FACTORY_ADD),
				  0);

	current = current->next_pad;
      }

      /* set performance mode */
      performance_mode = TRUE;
    }else{    
      current = source;

      while(current != NULL){
	/* ags-buffer */
	ags_recall_factory_create(audio,
				  NULL, NULL,
				  "ags-buffer\0",
				  audio_channel_start, audio->audio_channels, 
				  current->pad, current->pad + 1,
				  (AGS_RECALL_FACTORY_INPUT |
				   AGS_RECALL_FACTORY_RECALL |
				   AGS_RECALL_FACTORY_ADD),
				  0);
    
	current = current->next_pad;
      }
    }
  
    /*  */
    current = source;

    while(current != NULL){
      /* ags-play */
      ags_recall_factory_create(audio,
				NULL, NULL,
				"ags-play\0",
				audio_channel_start, audio->audio_channels, 
				current->pad, current->pad + 1,
				(AGS_RECALL_FACTORY_INPUT |
				 AGS_RECALL_FACTORY_PLAY |
				 AGS_RECALL_FACTORY_ADD),
				0);

      current = current->next_pad;
    }

    /*  */
    current = source;

    while(current != NULL){
      /* ags-stream */
      ags_recall_factory_create(audio,
				NULL, NULL,
				"ags-stream\0",
				audio_channel_start, audio->audio_channels, 
				current->pad, current->pad + 1,
				(AGS_RECALL_FACTORY_INPUT |
				 AGS_RECALL_FACTORY_PLAY |
				 AGS_RECALL_FACTORY_RECALL | 
				 AGS_RECALL_FACTORY_ADD),
				0);

      current = current->next_pad;
    }
  }
  
  live_lv2_bridge->mapped_input_pad = audio->input_pads;
}

void
ags_live_lv2_bridge_output_map_recall(AgsLiveLv2Bridge *live_lv2_bridge, guint audio_channel_start, guint output_pad_start)
{
  AgsAudio *audio;
  AgsChannel *input, *current;

  AgsDelayAudio *recall_delay_audio;
  AgsCountBeatsAudioRun *recall_count_beats_audio_run;

  AgsConfig *config;

  GList *list;

  gchar *str;

  gboolean performance_mode;

  audio = AGS_MACHINE(live_lv2_bridge)->audio;

  if(live_lv2_bridge->mapped_output_pad > output_pad_start){
    return;
  }

  config = ags_config_get_instance();

  if((AGS_MACHINE_IS_SYNTHESIZER & (AGS_MACHINE(live_lv2_bridge)->flags)) != 0){
    /* map dependending on output */
    str = ags_config_get_value(config,
			       AGS_CONFIG_GENERIC,
			       "engine-mode\0");
    performance_mode = FALSE;
  
    /* remap for input */
    if(str != NULL &&
       !g_ascii_strncasecmp(str,
			    "performance\0",
			    12)){
      input = audio->input;

      while(input != NULL){
	/* ags-copy */
	ags_recall_factory_create(audio,
				  NULL, NULL,
				  "ags-copy\0",
				  audio_channel_start, audio->audio_channels, 
				  input->pad, input->pad + 1,
				  (AGS_RECALL_FACTORY_INPUT |
				   AGS_RECALL_FACTORY_RECALL |
				   AGS_RECALL_FACTORY_REMAP),
				  0);
    
	input = input->next_pad;
      }

      /* set performance mode */
      performance_mode = TRUE;
    }else{    
      input = audio->input;

      while(input != NULL){
	/* ags-buffer */
	ags_recall_factory_create(audio,
				  NULL, NULL,
				  "ags-buffer\0",
				  audio_channel_start, audio->audio_channels, 
				  input->pad, input->pad + 1,
				  (AGS_RECALL_FACTORY_INPUT |
				   AGS_RECALL_FACTORY_RECALL |
				   AGS_RECALL_FACTORY_REMAP),
				  0);
    
	input = input->next_pad;
      }
    }
    
    /*  */
    if(!performance_mode){
      current = ags_channel_nth(audio->output,
				audio_channel_start + output_pad_start * audio->audio_channels);

      while(current != NULL){
	/* ags-stream */
	ags_recall_factory_create(audio,
				  NULL, NULL,
				  "ags-stream\0",
				  audio_channel_start, audio->audio_channels,
				  current->pad, current->pad + 1,
				  (AGS_RECALL_FACTORY_OUTPUT |
				   AGS_RECALL_FACTORY_PLAY |
				   AGS_RECALL_FACTORY_RECALL | 
				   AGS_RECALL_FACTORY_ADD),
				  0);

	current = current->next_pad;
      }
    }
  }
  
  live_lv2_bridge->mapped_output_pad = audio->output_pads;
}

void
ags_live_lv2_bridge_load_preset(AgsLiveLv2Bridge *live_lv2_bridge)
{
  GtkHBox *hbox;
  GtkLabel *label;
  
  AgsLv2Plugin *lv2_plugin;

  GList *list;  
  
  /* program */
  hbox = (GtkHBox *) gtk_hbox_new(FALSE, 0);
  gtk_box_pack_start((GtkBox *) live_lv2_bridge->vbox,
		     (GtkWidget *) hbox,
		     FALSE, FALSE,
		     0);
  gtk_box_reorder_child(GTK_BOX(live_lv2_bridge->vbox),
  			GTK_WIDGET(hbox),
  			0);
  
  label = (GtkLabel *) gtk_label_new("preset\0");
  gtk_box_pack_start((GtkBox *) hbox,
		     (GtkWidget *) label,
		     FALSE, FALSE,
		     0);

  live_lv2_bridge->preset = gtk_combo_box_text_new();
  gtk_box_pack_start((GtkBox *) hbox,
		     (GtkWidget *) live_lv2_bridge->preset,
		     FALSE, FALSE,
		     0);
  
  /* retrieve lv2 plugin */
  lv2_plugin = ags_lv2_manager_find_lv2_plugin(ags_lv2_manager_get_instance(),
					       live_lv2_bridge->filename,
					       live_lv2_bridge->effect);

  /* preset */
  list = lv2_plugin->preset;

  while(list != NULL){
    gtk_combo_box_text_append_text(lv2_bridge->preset,
				   AGS_LV2_PRESET(list->data)->preset_label);

    list = list->next;
  }

  gtk_widget_show_all(hbox);

  /* connect preset */
  g_signal_connect_after(G_OBJECT(live_lv2_bridge->preset), "changed\0",
			 G_CALLBACK(ags_live_lv2_bridge_preset_changed_callback), live_lv2_bridge);
}

void
ags_live_lv2_bridge_load_midi(AgsLiveLv2Bridge *live_lv2_bridge)
{
  //TODO:JK: implement me
}

void
ags_live_lv2_bridge_load_gui(AgsLiveLv2Bridge *live_lv2_bridge)
{
  AgsLv2Plugin *lv2_plugin;
  AgsLv2uiPlugin *lv2ui_plugin;

  GList *list;
  
  /* retrieve lv2 plugin */
  lv2_plugin = ags_lv2_manager_find_lv2_plugin(ags_lv2_manager_get_instance(),
					       live_lv2_bridge->filename,
					       live_lv2_bridge->effect);
  
  if(lv2_plugin == NULL ||
     lv2_plugin->ui_uri == NULL){
    return;
  }

  /* retrieve lv2ui plugin */
  list = ags_lv2ui_plugin_find_gui_uri(ags_lv2ui_manager_get_instance()->lv2ui_plugin,
				       lv2_plugin->ui_uri);

  if(list == NULL){
    return;
  }

  lv2ui_plugin = list->data;
  
#ifdef AGS_DEBUG
  g_message("ui filename - %s\0", AGS_BASE_PLUGIN(lv2ui_plugin)->ui_filename);
#endif

  /* apply ui */
  g_object_set(live_lv2_bridge,
	       "has-gui\0", TRUE,
	       "gui-uri\0", lv2_plugin->ui_uri,
	       "gui-filename\0", AGS_BASE_PLUGIN(lv2ui_plugin)->ui_filename,
	       NULL);
}

void
ags_live_lv2_bridge_load(AgsLiveLv2Bridge *live_lv2_bridge)
{
  AgsLv2Plugin *lv2_plugin;
    
  lv2_plugin = ags_lv2_manager_find_lv2_plugin(ags_lv2_manager_get_instance(),
					       live_lv2_bridge->filename,
					       live_lv2_bridge->effect);

  if(lv2_plugin == NULL){
    return;
  }

  /* URI */
  g_object_set(live_lv2_bridge,
	       "uri\0", lv2_plugin->uri,
	       NULL);

  /* preset */
  if(lv2_plugin->preset != NULL){
    ags_live_lv2_bridge_load_preset(live_lv2_bridge);
  }

  /* load gui */
  ags_live_lv2_bridge_load_gui(live_lv2_bridge);
}

/**
 * ags_live_lv2_bridge_lv2ui_idle_timeout:
 * @widget: the #AgsLiveLv2Bridge
 *
 * Idle lv2 ui.
 *
 * Returns: %TRUE if proceed with redraw, otherwise %FALSE
 *
 * Since: 0.7.134
 */
gboolean
ags_live_lv2_bridge_lv2ui_idle_timeout(GtkWidget *widget)
{
  if(g_hash_table_lookup(ags_live_lv2_bridge_lv2ui_idle,
			 widget) != NULL){
    AgsLiveLv2Bridge *live_lv2_bridge;
    
    live_lv2_bridge = AGS_LIVE_LV2_BRIDGE(widget);
    
    if(live_lv2_bridge->ui_feature != NULL &&
       live_lv2_bridge->ui_feature[0]->data != NULL){
      ((struct _LV2UI_Idle_Interface *) live_lv2_bridge->ui_feature[0]->data)->idle(live_lv2_bridge->ui_handle);
    }
    
    return(TRUE);
  }else{
    return(FALSE);
  }
}

/**
 * ags_live_lv2_bridge_new:
 * @soundcard: the assigned soundcard.
 * @filename: the plugin.so
 * @effect: the effect
 *
 * Creates an #AgsLiveLv2Bridge
 *
 * Returns: a new #AgsLiveLv2Bridge
 *
 * Since: 0.7.134
 */
AgsLiveLv2Bridge*
ags_live_lv2_bridge_new(GObject *soundcard,
			gchar *filename,
			gchar *effect)
{
  AgsLiveLv2Bridge *live_lv2_bridge;
  GValue value = {0,};

  live_lv2_bridge = (AgsLiveLv2Bridge *) g_object_new(AGS_TYPE_LIVE_LV2_BRIDGE,
						      NULL);

  if(soundcard != NULL){
    g_value_init(&value, G_TYPE_OBJECT);
    g_value_set_object(&value, soundcard);
    g_object_set_property(G_OBJECT(AGS_MACHINE(live_lv2_bridge)->audio),
			  "soundcard\0", &value);
    g_value_unset(&value);
  }

  g_object_set(live_lv2_bridge,
	       "filename\0", filename,
	       "effect\0", effect,
	       NULL);

  return(live_lv2_bridge);
}
