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

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#include <ags/X/ags_window.h>
#include <ags/X/ags_machine.h>
#include <ags/X/ags_effect_bridge.h>
#include <ags/X/ags_effect_bulk.h>
#include <ags/X/ags_bulk_member.h>

#include <ags/i18n.h>

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

void ags_live_lv2_bridge_resize_audio_channels(AgsMachine *machine,
					       guint audio_channels, guint audio_channels_old,
					       gpointer data);
void ags_live_lv2_bridge_resize_pads(AgsMachine *machine, GType channel_type,
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

extern GHashTable *ags_effect_bulk_indicator_queue_draw;

GHashTable *ags_live_lv2_bridge_lv2ui_handle = NULL;
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
						      "AgsLiveLv2Bridge", &ags_live_lv2_bridge_info,
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
   * Since: 1.0.0
   */
  param_spec =  g_param_spec_string("filename",
				    "the object file",
				    "The filename as string of object file",
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
   * Since: 1.0.0
   */
  param_spec =  g_param_spec_string("effect",
				    "the effect",
				    "The effect's string representation",
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
   * Since: 1.0.0
   */
  param_spec =  g_param_spec_string("uri",
				    "the uri",
				    "The uri's string representation",
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
   * Since: 1.0.0
   */
  param_spec =  g_param_spec_ulong("index",
				   "index of uri",
				   "The numerical index of uri",
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
   * Since: 1.0.0
   */
  param_spec =  g_param_spec_boolean("has-midi",
				     "has-midi",
				     "If effect has-midi",
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
   * Since: 1.0.0
   */
  param_spec =  g_param_spec_boolean("has-gui",
				     "has-gui",
				     "If effect has-gui",
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
   * Since: 1.0.0
   */
  param_spec =  g_param_spec_string("gui-filename",
				    "the GUI object file",
				    "The filename as string of GUI object file",
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
   * Since: 1.0.0
   */
  param_spec =  g_param_spec_string("gui-uri",
				    "the gui-uri",
				    "The gui-uri's string representation",
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

  if(ags_live_lv2_bridge_lv2ui_handle == NULL){
    ags_live_lv2_bridge_lv2ui_handle = g_hash_table_new_full(g_direct_hash, g_direct_equal,
							   NULL,
							   NULL);
  }

  if(ags_live_lv2_bridge_lv2ui_idle == NULL){
    ags_live_lv2_bridge_lv2ui_idle = g_hash_table_new_full(g_direct_hash, g_direct_equal,
							   NULL,
							   NULL);
  }

  audio = AGS_MACHINE(live_lv2_bridge)->audio;
  audio->flags |= (AGS_AUDIO_OUTPUT_HAS_RECYCLING |
		   AGS_AUDIO_INPUT_HAS_RECYCLING |
		   AGS_AUDIO_SYNC |
		   AGS_AUDIO_ASYNC |
		   AGS_AUDIO_HAS_NOTATION |  
		   AGS_AUDIO_SKIP_INPUT |
		   AGS_AUDIO_REVERSE_MAPPING);
  audio->flags &= (~AGS_AUDIO_NOTATION_DEFAULT);
  g_object_set(audio,
	       "audio-start-mapping", 0,
	       "audio-end-mapping", 128,
	       "midi-start-mapping", 0,
	       "midi-end-mapping", 128,
	       NULL);
  
  ags_machine_popup_add_connection_options((AgsMachine *) live_lv2_bridge,
					   (AGS_MACHINE_POPUP_MIDI_DIALOG));
  
  AGS_MACHINE(live_lv2_bridge)->flags |= (AGS_MACHINE_IS_SYNTHESIZER |
					  AGS_MACHINE_REVERSE_NOTATION);

  g_signal_connect_after(G_OBJECT(live_lv2_bridge), "resize-audio-channels",
			 G_CALLBACK(ags_live_lv2_bridge_resize_audio_channels), NULL);
  
  g_signal_connect_after(G_OBJECT(live_lv2_bridge), "resize-pads",
			 G_CALLBACK(ags_live_lv2_bridge_resize_pads), NULL);
  
  live_lv2_bridge->flags = 0;

  live_lv2_bridge->name = NULL;

  live_lv2_bridge->version = AGS_LIVE_LV2_BRIDGE_DEFAULT_VERSION;
  live_lv2_bridge->build_id = AGS_LIVE_LV2_BRIDGE_DEFAULT_BUILD_ID;

  live_lv2_bridge->xml_type = "ags-live-lv2-bridge";
  
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

  live_lv2_bridge->lv2_descriptor = NULL;
  live_lv2_bridge->lv2_handle = NULL;
  live_lv2_bridge->port_value = NULL;
  
  live_lv2_bridge->program = NULL;
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

  AGS_EFFECT_BRIDGE(AGS_MACHINE(live_lv2_bridge)->bridge)->bulk_output = (GtkWidget *) ags_effect_bulk_new(audio,
													   AGS_TYPE_OUTPUT);
  AGS_EFFECT_BULK(AGS_EFFECT_BRIDGE(AGS_MACHINE(live_lv2_bridge)->bridge)->bulk_output)->flags |= (AGS_EFFECT_BULK_HIDE_BUTTONS |
												   AGS_EFFECT_BULK_HIDE_ENTRIES |
												   AGS_EFFECT_BULK_SHOW_LABELS);
  gtk_table_attach(table,
		   (GtkWidget *) AGS_EFFECT_BRIDGE(AGS_MACHINE(live_lv2_bridge)->bridge)->bulk_output,
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
  item = (GtkImageMenuItem *) gtk_image_menu_item_new_with_label("Lv2");
  gtk_menu_shell_append((GtkMenuShell *) AGS_MACHINE(live_lv2_bridge)->popup,
			(GtkWidget *) item);
  gtk_widget_show((GtkWidget *) item);
  
  live_lv2_bridge->lv2_menu = (GtkMenu *) gtk_menu_new();
  gtk_menu_item_set_submenu((GtkMenuItem *) item,
			    (GtkWidget *) live_lv2_bridge->lv2_menu);

  item = (GtkImageMenuItem *) gtk_image_menu_item_new_with_label(i18n("show GUI"));
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
				g_strdup_printf("Plugin file not present %s",
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
    g_hash_table_remove(ags_live_lv2_bridge_lv2ui_handle,
			live_lv2_bridge->ui_handle);
    
    g_hash_table_remove(ags_live_lv2_bridge_lv2ui_idle,
			live_lv2_bridge->ui_handle);
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

  g_signal_connect(G_OBJECT(list->data), "activate",
		   G_CALLBACK(ags_live_lv2_bridge_show_gui_callback), live_lv2_bridge);

  /* program */
  if(live_lv2_bridge->program != NULL){
    g_signal_connect_after(G_OBJECT(live_lv2_bridge->program), "changed",
			   G_CALLBACK(ags_live_lv2_bridge_program_changed_callback), live_lv2_bridge);
  }
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
	       "filename", xmlGetProp(node,
					"filename"),
	       "effect", xmlGetProp(node,
				      "effect"),
	       NULL);

  /* launch */
  file_launch = (AgsFileLaunch *) g_object_new(AGS_TYPE_FILE_LAUNCH,
					       "node", node,
					       NULL);
  g_signal_connect(G_OBJECT(file_launch), "start",
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
    
    while((recall = ags_recall_template_find_type(recall, AGS_TYPE_PLAY_LV2_AUDIO)) != NULL){
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
		    "ags-live-lv2-bridge");
  xmlNewProp(node,
	     AGS_FILE_ID_PROP,
	     id);

  xmlNewProp(node,
	     "filename",
	     live_lv2_bridge->filename);

  xmlNewProp(node,
	     "effect",
	     live_lv2_bridge->effect);
  
  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context", file->application_context,
				   "file", file,
				   "node", node,
				   "xpath", g_strdup_printf("xpath=//*[@id='%s']", id),
				   "reference", live_lv2_bridge,
				   NULL));

  xmlAddChild(parent,
	      node);

  return(node);
}

void
ags_live_lv2_bridge_resize_audio_channels(AgsMachine *machine,
					  guint audio_channels, guint audio_channels_old,
					  gpointer data)
{
  AgsLiveLv2Bridge *live_lv2_bridge;

  AgsAudio *audio;
  AgsChannel *output, *input;
  AgsChannel *channel, *next_pad;
  AgsRecycling *first_recycling;
  AgsAudioSignal *audio_signal;  

  AgsMutexManager *mutex_manager;

  GObject *soundcard;
  
  guint output_pads, input_pads;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *audio_mutex;
  pthread_mutex_t *channel_mutex;

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  live_lv2_bridge = (AgsLiveLv2Bridge *) machine;

  audio = machine->audio;

  /* get audio mutex */
  pthread_mutex_lock(application_mutex);

  audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					 (GObject *) audio);
  
  pthread_mutex_unlock(application_mutex);

  /* get some fields */
  pthread_mutex_lock(audio_mutex);

  output = audio->output;
  input = audio->input;
  
  output_pads = audio->output_pads;
  input_pads = audio->input_pads;
  
  pthread_mutex_unlock(audio_mutex);
  
  if(input_pads == 0 &&
     output_pads == 0){
    return;
  }

  if(audio_channels > audio_channels_old){
    /* AgsInput */
    channel = input;

    while(channel != NULL){
      /* get channel mutex */
      pthread_mutex_lock(application_mutex);

      channel_mutex = ags_mutex_manager_lookup(mutex_manager,
					     (GObject *) channel);
  
      pthread_mutex_unlock(application_mutex);

      /* get some fields */
      pthread_mutex_lock(channel_mutex);
      
      next_pad = channel->next_pad;

      pthread_mutex_unlock(channel_mutex);

      channel = ags_channel_nth(channel,
				audio_channels_old);

      while(channel != next_pad){
	/* get channel mutex */
	pthread_mutex_lock(application_mutex);

	channel_mutex = ags_mutex_manager_lookup(mutex_manager,
						 (GObject *) channel);
  
	pthread_mutex_unlock(application_mutex);

	/* get some fields */
	pthread_mutex_lock(channel_mutex);
	
      	soundcard = channel->soundcard;
	first_recycling = channel->first_recycling;

	pthread_mutex_unlock(channel_mutex);

	/* audio signal */
	audio_signal = ags_audio_signal_new(soundcard,
					    (GObject *) first_recycling,
					    NULL);
	audio_signal->flags |= AGS_AUDIO_SIGNAL_TEMPLATE;
	ags_audio_signal_stream_resize(audio_signal,
				       1);
	ags_recycling_add_audio_signal(first_recycling,
				       audio_signal);
	
	/* iterate */
	pthread_mutex_lock(channel_mutex);
      
	channel = channel->next;

	pthread_mutex_unlock(channel_mutex);
      }
    }

    /* AgsOutput */
    channel = output;

    while(channel != NULL){
      /* get channel mutex */
      pthread_mutex_lock(application_mutex);

      channel_mutex = ags_mutex_manager_lookup(mutex_manager,
					     (GObject *) channel);
  
      pthread_mutex_unlock(application_mutex);

      /* get some fields */
      pthread_mutex_lock(channel_mutex);
      
      next_pad = channel->next_pad;

      pthread_mutex_unlock(channel_mutex);

      channel = ags_channel_pad_nth(channel,
				    audio_channels_old);

      while(channel != next_pad){
	/* get channel mutex */
	pthread_mutex_lock(application_mutex);

	channel_mutex = ags_mutex_manager_lookup(mutex_manager,
						 (GObject *) channel);
  
	pthread_mutex_unlock(application_mutex);

	/* get some fields */
	pthread_mutex_lock(channel_mutex);

	soundcard = channel->soundcard;
	first_recycling = channel->first_recycling;

	pthread_mutex_unlock(channel_mutex);

	/* audio signal */
	audio_signal = ags_audio_signal_new(soundcard,
					    (GObject *) first_recycling,
					    NULL);
	audio_signal->flags |= AGS_AUDIO_SIGNAL_TEMPLATE;
	ags_audio_signal_stream_resize(audio_signal,
				       3);
	ags_recycling_add_audio_signal(first_recycling,
				       audio_signal);
	
	/* iterate */
	pthread_mutex_lock(channel_mutex);
      
	channel = channel->next;

	pthread_mutex_unlock(channel_mutex);
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
ags_live_lv2_bridge_resize_pads(AgsMachine *machine, GType channel_type,
				guint pads, guint pads_old,
				gpointer data)
{
  AgsLiveLv2Bridge *live_lv2_bridge;

  AgsAudio *audio;
  AgsChannel *output, *input;
  AgsChannel *channel;
  AgsRecycling *first_recycling;
  AgsAudioSignal *audio_signal;
    
  AgsMutexManager *mutex_manager;

  GObject *soundcard;
  
  guint audio_channels;
  gboolean grow;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *audio_mutex;
  pthread_mutex_t *channel_mutex;

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  live_lv2_bridge = (AgsLiveLv2Bridge *) machine;

  audio = machine->audio;

  /* get audio mutex */
  pthread_mutex_lock(application_mutex);

  audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					 (GObject *) audio);
  
  pthread_mutex_unlock(application_mutex);  

  /* get some fields */
  pthread_mutex_lock(audio_mutex);

  output = audio->output;
  input = audio->input;
  
  audio_channels = audio->audio_channels;

  pthread_mutex_unlock(audio_mutex);
  
  if(pads == pads_old ||
     audio_channels == 0){
    return;
  }

  if(pads_old < pads){
    grow = TRUE;
  }else{
    grow = FALSE;
  }
  
  if(g_type_is_a(channel_type, AGS_TYPE_INPUT)){
    if(grow){
      /* AgsInput */
      channel = ags_channel_pad_nth(input,
				    pads_old);

      while(channel != NULL){
	/* get channel mutex */
	pthread_mutex_lock(application_mutex);

	channel_mutex = ags_mutex_manager_lookup(mutex_manager,
						 (GObject *) channel);
  
	pthread_mutex_unlock(application_mutex);

	/* get some fields */
	pthread_mutex_lock(channel_mutex);
	
	soundcard = channel->soundcard;

	first_recycling = channel->first_recycling;

	pthread_mutex_unlock(channel_mutex);

	/* audio signal */
	audio_signal = ags_audio_signal_new(soundcard,
					    (GObject *) first_recycling,
					    NULL);
	audio_signal->flags |= AGS_AUDIO_SIGNAL_TEMPLATE;
	ags_audio_signal_stream_resize(audio_signal,
				       1);
	ags_recycling_add_audio_signal(first_recycling,
				       audio_signal);
	
	/* iterate */
	pthread_mutex_lock(channel_mutex);

	channel = channel->next;

	pthread_mutex_unlock(channel_mutex);
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
      channel = ags_channel_pad_nth(output,
				    pads_old);

      while(channel != NULL){
	/* get channel mutex */
	pthread_mutex_lock(application_mutex);

	channel_mutex = ags_mutex_manager_lookup(mutex_manager,
						 (GObject *) channel);
  
	pthread_mutex_unlock(application_mutex);

	/* get some fields */
	pthread_mutex_lock(channel_mutex);
	
	soundcard = channel->soundcard;

	first_recycling = channel->first_recycling;

	pthread_mutex_unlock(channel_mutex);

	/* audio signal */
	audio_signal = ags_audio_signal_new(soundcard,
					    (GObject *) first_recycling,
					    NULL);
	audio_signal->flags |= AGS_AUDIO_SIGNAL_TEMPLATE;
	ags_audio_signal_stream_resize(audio_signal,
				       3);
	ags_recycling_add_audio_signal(first_recycling,
				       audio_signal);
	
	/* iterate */
	pthread_mutex_lock(channel_mutex);

	channel = channel->next;

	pthread_mutex_unlock(channel_mutex);
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
  AgsPlayLv2Audio *play_lv2_audio;
  AgsPlayLv2AudioRun *play_lv2_audio_run;

  AgsMutexManager *mutex_manager;

  GList *list;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *audio_mutex;
  
  if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0 ||
     (AGS_MACHINE_PREMAPPED_RECALL & (machine->flags)) != 0){
    return;
  }

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  window = (AgsWindow *) gtk_widget_get_ancestor((GtkWidget *) machine,
						 AGS_TYPE_WINDOW);

  live_lv2_bridge = (AgsLiveLv2Bridge *) machine;

  audio = machine->audio;

  /* get audio mutex */
  pthread_mutex_lock(application_mutex);

  audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					 (GObject *) audio);
  
  pthread_mutex_unlock(application_mutex);  

  /* ags-delay */
  ags_recall_factory_create(audio,
			    NULL, NULL,
			    "ags-delay",
			    0, 0,
			    0, 0,
			    (AGS_RECALL_FACTORY_OUTPUT |
			     AGS_RECALL_FACTORY_ADD |
			     AGS_RECALL_FACTORY_PLAY |
			     AGS_RECALL_FACTORY_RECALL),
			    0);

  pthread_mutex_lock(audio_mutex);

  list = ags_recall_find_type(audio->play,
			      AGS_TYPE_DELAY_AUDIO_RUN);

  if(list != NULL){
    play_delay_audio_run = AGS_DELAY_AUDIO_RUN(list->data);
    //    AGS_RECALL(play_delay_audio_run)->flags |= AGS_RECALL_PERSISTENT;
  }else{
    play_delay_audio_run = NULL;
  }
  
  pthread_mutex_unlock(audio_mutex);

  /* ags-count-beats */
  ags_recall_factory_create(audio,
			    NULL, NULL,
			    "ags-count-beats",
			    0, 0,
			    0, 0,
			    (AGS_RECALL_FACTORY_OUTPUT |
			     AGS_RECALL_FACTORY_ADD |
			     AGS_RECALL_FACTORY_PLAY |
			     AGS_RECALL_FACTORY_RECALL),
			    0);
  
  pthread_mutex_lock(audio_mutex);

  list = ags_recall_find_type(audio->play,
			      AGS_TYPE_COUNT_BEATS_AUDIO_RUN);

  if(list != NULL){
    play_count_beats_audio_run = AGS_COUNT_BEATS_AUDIO_RUN(list->data);

    /* set dependency */  
    g_object_set(G_OBJECT(play_count_beats_audio_run),
		 "delay-audio-run", play_delay_audio_run,
		 NULL);
    ags_seekable_seek(AGS_SEEKABLE(play_count_beats_audio_run),
		      window->navigation->position_tact->adjustment->value,
		      TRUE);
  }else{
    play_count_beats_audio_run = NULL;
  }

  pthread_mutex_unlock(audio_mutex);

  /* ags-record-midi */
  ags_recall_factory_create(audio,
			    NULL, NULL,
			    "ags-record-midi",
			    0, 0,
			    0, 0,
			    (AGS_RECALL_FACTORY_INPUT |
			     AGS_RECALL_FACTORY_ADD |
			     AGS_RECALL_FACTORY_RECALL),
			    0);

  pthread_mutex_lock(audio_mutex);

  list = ags_recall_find_type(audio->recall,
			      AGS_TYPE_RECORD_MIDI_AUDIO_RUN);

  if(list != NULL){
    recall_record_midi_audio_run = AGS_RECORD_MIDI_AUDIO_RUN(list->data);
    
    /* set dependency */
    g_object_set(G_OBJECT(recall_record_midi_audio_run),
		 "delay-audio-run", play_delay_audio_run,
		 NULL);

    /* set dependency */
    g_object_set(G_OBJECT(recall_record_midi_audio_run),
		 "count-beats-audio-run", play_count_beats_audio_run,
		 NULL);
  }  

  pthread_mutex_unlock(audio_mutex);

  /* ags-play-lv2 */
  ags_recall_factory_create(audio,
			    NULL, NULL,
			    "ags-play-lv2",
			    0, 0,
			    0, 0,
			    (AGS_RECALL_FACTORY_OUTPUT |
			     AGS_RECALL_FACTORY_ADD |
			     AGS_RECALL_FACTORY_PLAY |
			     AGS_RECALL_FACTORY_BULK),
			    0);

  pthread_mutex_lock(audio_mutex);

  list = ags_recall_find_type(audio->play,
			      AGS_TYPE_PLAY_LV2_AUDIO);
  
  if(list != NULL){
    play_lv2_audio = AGS_PLAY_LV2_AUDIO(list->data);
    
    g_object_set(play_lv2_audio,
		 "filename", live_lv2_bridge->filename,
		 "effect", live_lv2_bridge->effect,
		 NULL);

    ags_play_lv2_audio_load(play_lv2_audio);
    ags_play_lv2_audio_load_ports(play_lv2_audio);
  }

  list = ags_recall_find_type(audio->play,
			      AGS_TYPE_PLAY_LV2_AUDIO_RUN);

  if(list != NULL){
    play_lv2_audio_run = AGS_PLAY_LV2_AUDIO_RUN(list->data);

    /* set dependency */
    g_object_set(G_OBJECT(play_lv2_audio_run),
		 "delay-audio-run", play_delay_audio_run,
		 NULL);

    /* set dependency */
    g_object_set(G_OBJECT(play_lv2_audio_run),
		 "count-beats-audio-run", play_count_beats_audio_run,
		 NULL);
  }

  pthread_mutex_unlock(audio_mutex);

  /* depending on destination */
  ags_live_lv2_bridge_input_map_recall(live_lv2_bridge,
				       0,
				       0);

  /* depending on destination */
  ags_live_lv2_bridge_output_map_recall(live_lv2_bridge,
					0,
					0);

  /* call parent */
  AGS_MACHINE_CLASS(ags_live_lv2_bridge_parent_class)->map_recall(machine);
}

void
ags_live_lv2_bridge_input_map_recall(AgsLiveLv2Bridge *live_lv2_bridge, guint audio_channel_start, guint input_pad_start)
{
  AgsAudio *audio;

  audio = AGS_MACHINE(live_lv2_bridge)->audio;

  if(live_lv2_bridge->mapped_input_pad > input_pad_start){
    return;
  }

  live_lv2_bridge->mapped_input_pad = audio->input_pads;
}

void
ags_live_lv2_bridge_output_map_recall(AgsLiveLv2Bridge *live_lv2_bridge, guint audio_channel_start, guint output_pad_start)
{
  AgsAudio *audio;

  audio = AGS_MACHINE(live_lv2_bridge)->audio;

  if(live_lv2_bridge->mapped_output_pad > output_pad_start){
    return;
  }

  live_lv2_bridge->mapped_output_pad = audio->output_pads;
}

void
ags_live_lv2_bridge_load_program(AgsLiveLv2Bridge *live_lv2_bridge)
{
  AgsLv2Plugin *lv2_plugin;

  LV2_Descriptor *plugin_descriptor;
  LV2_Programs_Interface *program_interface;

  lv2_plugin = ags_lv2_manager_find_lv2_plugin(ags_lv2_manager_get_instance(),
					       live_lv2_bridge->filename,
					       live_lv2_bridge->effect);
  live_lv2_bridge->lv2_descriptor = 
    plugin_descriptor = AGS_LV2_PLUGIN_DESCRIPTOR(AGS_BASE_PLUGIN(lv2_plugin)->plugin_descriptor);
  
  if(plugin_descriptor != NULL &&
     plugin_descriptor->extension_data != NULL &&
     (program_interface = plugin_descriptor->extension_data(LV2_PROGRAMS__Interface)) != NULL){
    GtkListStore *model;
    
    GtkTreeIter iter;
    
    LV2_Program_Descriptor *program_descriptor;

    uint32_t i;

    if(live_lv2_bridge->lv2_handle == NULL){
      live_lv2_bridge->lv2_handle = (LV2_Handle *) ags_base_plugin_instantiate(lv2_plugin,
									       AGS_MACHINE(live_lv2_bridge)->audio->samplerate);
    }

    if(live_lv2_bridge->port_value == NULL){
      GList *port_descriptor;
      
      guint port_count;

      port_count = g_list_length(AGS_BASE_PLUGIN(lv2_plugin)->port);

      if(port_count > 0){
	live_lv2_bridge->port_value = (float *) malloc(port_count * sizeof(float));
      }

      port_descriptor = AGS_BASE_PLUGIN(lv2_plugin)->port;
      
      for(i = 0; port_descriptor != NULL;){
	if((AGS_PORT_DESCRIPTOR_CONTROL & (AGS_PORT_DESCRIPTOR(port_descriptor->data)->flags)) != 0){
	  if((AGS_PORT_DESCRIPTOR_INPUT & (AGS_PORT_DESCRIPTOR(port_descriptor->data)->flags)) != 0){
	    plugin_descriptor->connect_port(live_lv2_bridge->lv2_handle[0],
					    AGS_PORT_DESCRIPTOR(port_descriptor->data)->port_index,
					    &(live_lv2_bridge->port_value[i]));
	    i++;
	  }
	}

	port_descriptor = port_descriptor->next;
      }
    }
    
    if(live_lv2_bridge->program == NULL){
      GtkHBox *hbox;
      GtkLabel *label;
      
      /* program */
      hbox = (GtkHBox *) gtk_hbox_new(FALSE, 0);
      gtk_box_pack_start((GtkBox *) live_lv2_bridge->vbox,
			 (GtkWidget *) hbox,
			 FALSE, FALSE,
			 0);
      gtk_box_reorder_child(GTK_BOX(live_lv2_bridge->vbox),
			    GTK_WIDGET(hbox),
			    0);
  
      label = (GtkLabel *) gtk_label_new(i18n("program"));
      gtk_box_pack_start((GtkBox *) hbox,
			 (GtkWidget *) label,
			 FALSE, FALSE,
			 0);

      live_lv2_bridge->program = gtk_combo_box_text_new();
      gtk_box_pack_start((GtkBox *) hbox,
			 (GtkWidget *) live_lv2_bridge->program,
			 FALSE, FALSE,
			 0);

      if((AGS_MACHINE_CONNECTED & (AGS_MACHINE(live_lv2_bridge)->flags)) != 0){
	g_signal_connect_after(G_OBJECT(live_lv2_bridge->program), "changed",
			       G_CALLBACK(ags_live_lv2_bridge_program_changed_callback), live_lv2_bridge);
      }
      
      model = gtk_list_store_new(3,
				 G_TYPE_STRING,
				 G_TYPE_ULONG,
				 G_TYPE_ULONG);
      
      gtk_combo_box_set_model(GTK_COMBO_BOX(live_lv2_bridge->program),
			      GTK_TREE_MODEL(model));
    }else{
      model = gtk_combo_box_get_model(GTK_COMBO_BOX(live_lv2_bridge->program));
      
      gtk_list_store_clear(GTK_LIST_STORE(model));
    }
    
    for(i = 0; (program_descriptor = program_interface->get_program(live_lv2_bridge->lv2_handle[0], i)) != NULL; i++){
      gtk_list_store_append(model, &iter);
      gtk_list_store_set(model, &iter,
			 0, program_descriptor->name,
			 1, program_descriptor->bank,
			 2, program_descriptor->program,
			 -1);
    }
  }
}

void
ags_live_lv2_bridge_load_preset(AgsLiveLv2Bridge *live_lv2_bridge)
{
  GtkHBox *hbox;
  GtkLabel *label;
  
  AgsLv2Plugin *lv2_plugin;

  GList *list;  
  
  /* preset */
  hbox = (GtkHBox *) gtk_hbox_new(FALSE, 0);
  gtk_box_pack_start((GtkBox *) live_lv2_bridge->vbox,
		     (GtkWidget *) hbox,
		     FALSE, FALSE,
		     0);
  gtk_box_reorder_child(GTK_BOX(live_lv2_bridge->vbox),
  			GTK_WIDGET(hbox),
  			0);
  
  label = (GtkLabel *) gtk_label_new(i18n("preset"));
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
    if(AGS_LV2_PRESET(list->data)->preset_label != NULL){
      gtk_combo_box_text_append_text(live_lv2_bridge->preset,
				     AGS_LV2_PRESET(list->data)->preset_label);
    }
    
    list = list->next;
  }

  gtk_widget_show_all(hbox);

  /* connect preset */
  g_signal_connect_after(G_OBJECT(live_lv2_bridge->preset), "changed",
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
  g_message("ui filename - %s", AGS_BASE_PLUGIN(lv2ui_plugin)->ui_filename);
#endif

  /* apply ui */
  g_object_set(live_lv2_bridge,
	       "has-gui", TRUE,
	       "gui-uri", lv2_plugin->ui_uri,
	       "gui-filename", AGS_BASE_PLUGIN(lv2ui_plugin)->ui_filename,
	       NULL);
}

void
ags_live_lv2_bridge_load(AgsLiveLv2Bridge *live_lv2_bridge)
{
  AgsEffectBulk *effect_bulk;
  AgsBulkMember *bulk_member;
  GtkListStore *model;
  GtkTreeIter iter;

  AgsLv2Plugin *lv2_plugin;

  AgsConfig *config;

  void *plugin_so;

  GList *port;
  GList *list;

  gchar *str;
  
  unsigned long samplerate;
  unsigned long effect_index;
  gdouble step;
  unsigned long port_count;
  gboolean has_output_port;

  guint x, y;
  unsigned long i, j;
  guint k;
    
  lv2_plugin = ags_lv2_manager_find_lv2_plugin(ags_lv2_manager_get_instance(),
					       live_lv2_bridge->filename,
					       live_lv2_bridge->effect);

  if(lv2_plugin == NULL){
    return;
  }

  /* URI */
  g_object_set(live_lv2_bridge,
	       "uri", lv2_plugin->uri,
	       NULL);

  config = ags_config_get_instance();

  /* samplerate */
  str = ags_config_get_value(config,
			     AGS_CONFIG_SOUNDCARD,
			     "samplerate");

  if(str == NULL){
    str = ags_config_get_value(config,
			       AGS_CONFIG_SOUNDCARD_0,
			       "samplerate");
  }
  
  if(str != NULL){
    samplerate = g_ascii_strtoull(str,
				  NULL,
				  10);
    free(str);
  }else{
    samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
  }

  g_message("ags_live_lv2_bridge.c - load %s %s", live_lv2_bridge->filename, live_lv2_bridge->effect);
 
  /* load plugin */
  plugin_so = AGS_BASE_PLUGIN(lv2_plugin)->plugin_so;
  
  /*  */
  effect_index = AGS_BASE_PLUGIN(lv2_plugin)->effect_index;

  effect_bulk = AGS_EFFECT_BRIDGE(AGS_MACHINE(live_lv2_bridge)->bridge)->bulk_output;

  /* retrieve position within table  */
  x = 0;
  y = 0;
  
  list = effect_bulk->table->children;

  while(list != NULL){
    if(y <= ((GtkTableChild *) list->data)->top_attach){
      y = ((GtkTableChild *) list->data)->top_attach + 1;
    }

    list = list->next;
  }

  /* load ports */
  port = AGS_BASE_PLUGIN(lv2_plugin)->port;

  port_count = g_list_length(port);
  k = 0;

  while(port != NULL){
    if((AGS_PORT_DESCRIPTOR_CONTROL & (AGS_PORT_DESCRIPTOR(port->data)->flags)) != 0){
      GtkWidget *child_widget;

      AgsLv2Conversion *lv2_conversion;
      
      GType widget_type;

      gchar *plugin_name;
      gchar *control_port;
      
      guint step_count;
      gboolean disable_seemless;

      disable_seemless = FALSE;
      
      if(x == AGS_EFFECT_BULK_COLUMNS_COUNT){
	x = 0;
	y++;
	gtk_table_resize(effect_bulk->table,
			 y + 1, AGS_EFFECT_BULK_COLUMNS_COUNT);
      }

      if((AGS_PORT_DESCRIPTOR_TOGGLED & (AGS_PORT_DESCRIPTOR(port->data)->flags)) != 0){
	if((AGS_PORT_DESCRIPTOR_OUTPUT & (AGS_PORT_DESCRIPTOR(port->data)->flags)) != 0){
	  widget_type = AGS_TYPE_LED;
	}else{
	  widget_type = GTK_TYPE_TOGGLE_BUTTON;
	}
      }else{
	if((AGS_PORT_DESCRIPTOR_OUTPUT & (AGS_PORT_DESCRIPTOR(port->data)->flags)) != 0){
	  widget_type = AGS_TYPE_HINDICATOR;
	}else{
	  widget_type = AGS_TYPE_DIAL;
	}
      }

      step_count = AGS_DIAL_DEFAULT_PRECISION;

      if((AGS_PORT_DESCRIPTOR_INTEGER & (AGS_PORT_DESCRIPTOR(port->data)->flags)) != 0){
	step_count = AGS_PORT_DESCRIPTOR(port->data)->scale_steps;

	disable_seemless = TRUE;	
      }

      /* add bulk member */
      plugin_name = g_strdup_printf("lv2-<%s>",
				    lv2_plugin->uri);
      control_port = g_strdup_printf("%u/%u",
				     k,
				     port_count);
      bulk_member = (AgsBulkMember *) g_object_new(AGS_TYPE_BULK_MEMBER,
						   "widget-type", widget_type,
						   "widget-label", AGS_PORT_DESCRIPTOR(port->data)->port_name,
						   "plugin-name", plugin_name,
						   "filename", live_lv2_bridge->filename,
						   "effect", live_lv2_bridge->effect,
						   "specifier", AGS_PORT_DESCRIPTOR(port->data)->port_name,
						   "control-port", control_port,
						   "steps", step_count,
						   NULL);
      child_widget = ags_bulk_member_get_widget(bulk_member);

      g_free(plugin_name);
      g_free(control_port);
      
      /* lv2 conversion */
      lv2_conversion = NULL;

      if((AGS_PORT_DESCRIPTOR_LOGARITHMIC & (AGS_PORT_DESCRIPTOR(port->data)->flags)) != 0){
	if(lv2_conversion == NULL ||
	   !AGS_IS_LV2_CONVERSION(lv2_conversion)){
	  lv2_conversion = ags_lv2_conversion_new();
	}
    
	lv2_conversion->flags |= AGS_LV2_CONVERSION_LOGARITHMIC;
      }

      bulk_member->conversion = (AgsConversion *) lv2_conversion;

      /* child widget */
      if((AGS_PORT_DESCRIPTOR_TOGGLED & (AGS_PORT_DESCRIPTOR(port->data)->flags)) != 0){
	bulk_member->port_flags = AGS_BULK_MEMBER_PORT_BOOLEAN;
      }
      
      if((AGS_PORT_DESCRIPTOR_INTEGER & (AGS_PORT_DESCRIPTOR(port->data)->flags)) != 0){
	bulk_member->port_flags = AGS_BULK_MEMBER_PORT_INTEGER;
      }

      if(AGS_IS_DIAL(child_widget)){
	AgsDial *dial;
	GtkAdjustment *adjustment;

	float lower_bound, upper_bound;
	
	dial = (AgsDial *) child_widget;

	if(disable_seemless){
	  dial->flags &= (~AGS_DIAL_SEEMLESS_MODE);
	}

	/* add controls of ports and apply range  */
	lower_bound = g_value_get_float(AGS_PORT_DESCRIPTOR(port->data)->lower_value);
	upper_bound = g_value_get_float(AGS_PORT_DESCRIPTOR(port->data)->upper_value);

	adjustment = (GtkAdjustment *) gtk_adjustment_new(0.0, 0.0, 1.0, 0.1, 0.1, 0.0);
	g_object_set(dial,
		     "adjustment", adjustment,
		     NULL);

	if(upper_bound >= 0.0 && lower_bound >= 0.0){
	  step = (upper_bound - lower_bound) / step_count;
	}else if(upper_bound < 0.0 && lower_bound < 0.0){
	  step = -1.0 * (lower_bound - upper_bound) / step_count;
	}else{
	  step = (upper_bound - lower_bound) / step_count;
	}

	gtk_adjustment_set_step_increment(adjustment,
					  step);
	gtk_adjustment_set_lower(adjustment,
				 lower_bound);
	gtk_adjustment_set_upper(adjustment,
				 upper_bound);
	gtk_adjustment_set_value(adjustment,
				 g_value_get_float(AGS_PORT_DESCRIPTOR(port->data)->default_value));
      }else if(AGS_IS_INDICATOR(child_widget) ||
	       AGS_IS_LED(child_widget)){
	g_hash_table_insert(ags_effect_bulk_indicator_queue_draw,
			    child_widget, ags_effect_bulk_indicator_queue_draw_timeout);
	effect_bulk->queued_drawing = g_list_prepend(effect_bulk->queued_drawing,
						     child_widget);
	g_timeout_add(1000 / 30, (GSourceFunc) ags_effect_bulk_indicator_queue_draw_timeout, (gpointer) child_widget);
      }

#ifdef AGS_DEBUG
      g_message("lv2 bounds: %f %f", lower_bound, upper_bound);
#endif
      
      gtk_table_attach(effect_bulk->table,
		       (GtkWidget *) bulk_member,
		       x, x + 1,
		       y, y + 1,
		       GTK_FILL, GTK_FILL,
		       0, 0);
      ags_connectable_connect(AGS_CONNECTABLE(bulk_member));
      gtk_widget_show_all((GtkWidget *) effect_bulk->table);

      x++;
    }

    port = port->next;    
    k++;
  }

  /* program */
  live_lv2_bridge->lv2_handle = ags_base_plugin_instantiate(lv2_plugin,
							    AGS_MACHINE(live_lv2_bridge)->audio->samplerate);
  
  if((AGS_LV2_PLUGIN_HAS_PROGRAM_INTERFACE & (lv2_plugin->flags)) != 0){
    ags_live_lv2_bridge_load_program(live_lv2_bridge);
  }
  
  /* preset */
  if(lv2_plugin->preset != NULL){
    ags_live_lv2_bridge_load_preset(live_lv2_bridge);
  }

  /* load gui */
  ags_live_lv2_bridge_load_gui(live_lv2_bridge);
}

/**
 * ags_live_lv2_bridge_lv2ui_idle_timeout:
 * @widget: the LV2UI_Handle
 *
 * Idle lv2 ui.
 *
 * Returns: %TRUE if proceed with redraw, otherwise %FALSE
 *
 * Since: 1.0.0
 */
gboolean
ags_live_lv2_bridge_lv2ui_idle_timeout(GtkWidget *widget)
{
  AgsLiveLv2Bridge *live_lv2_bridge;
    
  if((live_lv2_bridge = g_hash_table_lookup(ags_live_lv2_bridge_lv2ui_idle,
					    widget)) != NULL){    
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
 * Since: 1.0.0
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
			  "soundcard", &value);
    g_value_unset(&value);
  }

  g_object_set(live_lv2_bridge,
	       "filename", filename,
	       "effect", effect,
	       NULL);

  return(live_lv2_bridge);
}
