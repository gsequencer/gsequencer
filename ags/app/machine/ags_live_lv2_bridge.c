/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2022 Joël Krähemann
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

#include <ags/app/machine/ags_live_lv2_bridge.h>
#include <ags/app/machine/ags_live_lv2_bridge_callbacks.h>

#include <lv2/lv2plug.in/ns/lv2ext/lv2_programs.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_window.h>
#include <ags/app/ags_navigation.h>
#include <ags/app/ags_effect_bridge.h>
#include <ags/app/ags_effect_bulk.h>
#include <ags/app/ags_bulk_member.h>

#include <ags/i18n.h>

void ags_live_lv2_bridge_class_init(AgsLiveLv2BridgeClass *live_lv2_bridge);
void ags_live_lv2_bridge_connectable_interface_init(AgsConnectableInterface *connectable);
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
 * @include: ags/app/ags_live_lv2_bridge.h
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

extern GHashTable *ags_effect_bulk_indicator_queue_draw;

GHashTable *ags_live_lv2_bridge_lv2ui_handle = NULL;
GHashTable *ags_live_lv2_bridge_lv2ui_idle = NULL;

GType
ags_live_lv2_bridge_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_live_lv2_bridge = 0;

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

    ags_type_live_lv2_bridge = g_type_register_static(AGS_TYPE_MACHINE,
						      "AgsLiveLv2Bridge", &ags_live_lv2_bridge_info,
						      0);

    g_type_add_interface_static(ags_type_live_lv2_bridge,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_live_lv2_bridge);
  }

  return g_define_type_id__volatile;
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
   * Since: 3.0.0
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
   * Since: 3.0.0
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
   * Since: 3.0.0
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
   * Since: 3.0.0
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
   * Since: 3.0.0
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
   * Since: 3.0.0
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
   * Since: 3.0.0
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
   * Since: 3.0.0
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
ags_live_lv2_bridge_init(AgsLiveLv2Bridge *live_lv2_bridge)
{
  GtkBox *hbox;
  GtkLabel *label;
      
  GtkListStore *model;
  
  AgsAudio *audio;

  g_signal_connect_after((GObject *) live_lv2_bridge, "parent-set",
			 G_CALLBACK(ags_live_lv2_bridge_parent_set_callback), (gpointer) live_lv2_bridge);

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
  ags_audio_set_flags(audio, (AGS_AUDIO_SYNC |
			      AGS_AUDIO_ASYNC |
			      AGS_AUDIO_OUTPUT_HAS_RECYCLING |
			      AGS_AUDIO_INPUT_HAS_RECYCLING |
			      AGS_AUDIO_SKIP_INPUT));
  ags_audio_set_ability_flags(audio, (AGS_SOUND_ABILITY_NOTATION));
  ags_audio_set_behaviour_flags(audio, (AGS_SOUND_BEHAVIOUR_REVERSE_MAPPING |
					AGS_SOUND_BEHAVIOUR_DEFAULTS_TO_INPUT));
  //  audio->flags &= (~AGS_AUDIO_NOTATION_DEFAULT);
  
  g_object_set(audio,
	       "min-audio-channels", 1,
	       "min-output-pads", 1,
	       "min-input-pads", 1,
	       "max-input-pads", 128,
	       "audio-start-mapping", 0,
	       "audio-end-mapping", 128,
	       "midi-start-mapping", 0,
	       "midi-end-mapping", 128,
	       NULL);
    
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

  live_lv2_bridge->lv2_play_container = ags_recall_container_new();
  live_lv2_bridge->lv2_recall_container = ags_recall_container_new();

  live_lv2_bridge->envelope_play_container = ags_recall_container_new();
  live_lv2_bridge->envelope_recall_container = ags_recall_container_new();

  live_lv2_bridge->buffer_play_container = ags_recall_container_new();
  live_lv2_bridge->buffer_recall_container = ags_recall_container_new();
  
  live_lv2_bridge->filename = NULL;
  live_lv2_bridge->effect = NULL;
  live_lv2_bridge->uri = NULL;
  live_lv2_bridge->uri_index = 0;

  live_lv2_bridge->has_midi = FALSE;

  live_lv2_bridge->lv2_plugin = NULL;

  live_lv2_bridge->lv2_descriptor = NULL;
  live_lv2_bridge->lv2_handle = NULL;
  live_lv2_bridge->port_value = NULL;
  
  live_lv2_bridge->has_gui = FALSE;
  live_lv2_bridge->gui_filename = NULL;
  live_lv2_bridge->gui_uri = NULL;

  live_lv2_bridge->lv2ui_plugin = NULL;

  live_lv2_bridge->ui_descriptor = NULL;

  live_lv2_bridge->ui_feature = (LV2_Feature **) malloc(3 * sizeof(LV2_Feature *));

  live_lv2_bridge->ui_feature[0] = (LV2_Feature *) malloc(sizeof(LV2_Feature));
  live_lv2_bridge->ui_feature[0]->data = NULL;
  
  live_lv2_bridge->ui_feature[1] = (LV2_Feature *) malloc(sizeof(LV2_Feature));
  live_lv2_bridge->ui_feature[1]->data = NULL;

  live_lv2_bridge->ui_feature[2] = NULL;

  live_lv2_bridge->ui_handle = NULL;

  /**/
  live_lv2_bridge->vbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
						 0);
  gtk_frame_set_child(AGS_MACHINE(live_lv2_bridge)->frame,
		      (GtkWidget *) live_lv2_bridge->vbox);

  live_lv2_bridge->program = NULL;
  live_lv2_bridge->preset = NULL;

  /* program */
  hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				0);
  gtk_grid_attach(AGS_MACHINE(live_lv2_bridge)->bridge,
		  (GtkWidget *) hbox,
		  0, 0,
		  1, 1);
  
  label = (GtkLabel *) gtk_label_new(i18n("program"));
  gtk_box_append(hbox,
		 (GtkWidget *) label);

  live_lv2_bridge->program = (GtkComboBoxText *) gtk_combo_box_text_new();
  gtk_box_append(hbox,
		 (GtkWidget *) live_lv2_bridge->program);
      
  model = gtk_list_store_new(3,
			     G_TYPE_STRING,
			     G_TYPE_ULONG,
			     G_TYPE_ULONG);
      
  gtk_combo_box_set_model(GTK_COMBO_BOX(live_lv2_bridge->program),
			  GTK_TREE_MODEL(model));
  
  /* preset */
  hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				0);
  gtk_grid_attach(AGS_MACHINE(live_lv2_bridge)->bridge,
		  (GtkWidget *) hbox,
		  0, 1,
		  1, 1);
  
  label = (GtkLabel *) gtk_label_new(i18n("preset"));
  gtk_box_append(hbox,
		 (GtkWidget *) label);

  live_lv2_bridge->preset = (GtkComboBox *) gtk_combo_box_text_new();
  gtk_box_append(hbox,
		 (GtkWidget *) live_lv2_bridge->preset);
  
  /* effect bridge */  
  AGS_MACHINE(live_lv2_bridge)->bridge = ags_effect_bridge_new(audio);
  gtk_box_append(live_lv2_bridge->vbox,
		 (GtkWidget *) AGS_MACHINE(live_lv2_bridge)->bridge);

  AGS_EFFECT_BRIDGE(AGS_MACHINE(live_lv2_bridge)->bridge)->bulk_input = (GtkWidget *) ags_effect_bulk_new(audio,
													  AGS_TYPE_INPUT);
  AGS_EFFECT_BULK(AGS_EFFECT_BRIDGE(AGS_MACHINE(live_lv2_bridge)->bridge)->bulk_input)->flags |= (AGS_EFFECT_BULK_HIDE_BUTTONS |
												  AGS_EFFECT_BULK_HIDE_ENTRIES |
												  AGS_EFFECT_BULK_SHOW_LABELS);

  gtk_widget_set_valign((GtkWidget *) AGS_EFFECT_BRIDGE(AGS_MACHINE(live_lv2_bridge)->bridge)->bulk_input,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) AGS_EFFECT_BRIDGE(AGS_MACHINE(live_lv2_bridge)->bridge)->bulk_input,
			GTK_ALIGN_FILL);
  
  gtk_grid_attach(AGS_MACHINE(live_lv2_bridge)->bridge,
		  (GtkWidget *) AGS_EFFECT_BRIDGE(AGS_MACHINE(live_lv2_bridge)->bridge)->bulk_input,
		  0, 2,
		  1, 1);
  
  /* plugin widget */
  live_lv2_bridge->lv2_gui = NULL;
  live_lv2_bridge->ui_widget = NULL;

  live_lv2_bridge->lv2_window = NULL;

  live_lv2_bridge->block_control = g_hash_table_new_full(g_direct_hash,
							 g_direct_equal,
							 NULL,
							 NULL);
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

	  window = (AgsWindow *) gtk_widget_get_ancestor((GtkWidget *) live_lv2_bridge,
							 AGS_TYPE_WINDOW);

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

  g_object_disconnect(G_OBJECT(live_lv2_bridge),
		      "any_signal::resize-audio-channels",
		      G_CALLBACK(ags_live_lv2_bridge_resize_audio_channels),
		      NULL,
		      "any_signal::resize-pads",
		      G_CALLBACK(ags_live_lv2_bridge_resize_pads),
		      NULL,
		      NULL);
  
  /* lv2 plugin */
  if(live_lv2_bridge->lv2_plugin != NULL){
    g_object_unref(live_lv2_bridge->lv2_plugin);
  }

  /* lv2ui plugin */
  if(live_lv2_bridge->lv2ui_plugin != NULL){
    g_object_unref(live_lv2_bridge->lv2ui_plugin);
  }

  /* plugin widget */
  if(live_lv2_bridge->ui_handle != NULL){
    live_lv2_bridge->ui_descriptor->cleanup(live_lv2_bridge->ui_handle[0]);
    
    g_hash_table_remove(ags_live_lv2_bridge_lv2ui_handle,
			live_lv2_bridge->ui_handle);
    
    g_hash_table_remove(ags_live_lv2_bridge_lv2ui_idle,
			live_lv2_bridge->ui_handle);
  }

  if(live_lv2_bridge->lv2_window != NULL){
    g_object_run_dispose(live_lv2_bridge->lv2_window);
    g_object_unref(live_lv2_bridge->lv2_window);
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_live_lv2_bridge_parent_class)->finalize(gobject);
}

void
ags_live_lv2_bridge_connect(AgsConnectable *connectable)
{
  AgsLiveLv2Bridge *live_lv2_bridge;
  AgsEffectBridge *effect_bridge;
  AgsBulkMember *bulk_member;
  GtkWidget *control;

  GList *start_list, *list;
  
  if((AGS_CONNECTABLE_CONNECTED & (AGS_MACHINE(connectable)->connectable_flags)) != 0){
    return;
  }

  ags_live_lv2_bridge_parent_connectable_interface->connect(connectable);

  live_lv2_bridge = AGS_LIVE_LV2_BRIDGE(connectable);

  /* program */
  if(live_lv2_bridge->program != NULL){
    g_signal_connect_after(G_OBJECT(live_lv2_bridge->program), "changed",
			   G_CALLBACK(ags_live_lv2_bridge_program_changed_callback), live_lv2_bridge);
  }

  /* bulk member */
  effect_bridge = AGS_EFFECT_BRIDGE(AGS_MACHINE(live_lv2_bridge)->bridge);
  
  list =
    start_list = ags_effect_bulk_get_bulk_member(AGS_EFFECT_BULK(effect_bridge->bulk_input));

  while(list != NULL){
    bulk_member = list->data;

    control = ags_bulk_member_get_widget(bulk_member);

    if(bulk_member->widget_type == AGS_TYPE_DIAL){
      g_signal_connect_after(GTK_WIDGET(control), "value-changed",
			     G_CALLBACK(ags_live_lv2_bridge_dial_changed_callback), live_lv2_bridge);
    }else if(bulk_member->widget_type == GTK_TYPE_SCALE){
      g_signal_connect_after(GTK_WIDGET(control), "value-changed",
			     G_CALLBACK(ags_live_lv2_bridge_scale_changed_callback), live_lv2_bridge);
    }else if(bulk_member->widget_type == GTK_TYPE_SPIN_BUTTON){
      g_signal_connect_after(GTK_WIDGET(control), "value-changed",
			     G_CALLBACK(ags_live_lv2_bridge_spin_button_changed_callback), live_lv2_bridge);
    }else if(bulk_member->widget_type == GTK_TYPE_CHECK_BUTTON){
      g_signal_connect_after(GTK_WIDGET(control), "clicked",
			     G_CALLBACK(ags_live_lv2_bridge_check_button_clicked_callback), live_lv2_bridge);
    }else if(bulk_member->widget_type == GTK_TYPE_TOGGLE_BUTTON){
      g_signal_connect_after(GTK_WIDGET(control), "clicked",
			     G_CALLBACK(ags_live_lv2_bridge_toggle_button_clicked_callback), live_lv2_bridge);
    }else if(bulk_member->widget_type == GTK_TYPE_BUTTON){
      g_signal_connect_after(GTK_WIDGET(control), "clicked",
			     G_CALLBACK(ags_live_lv2_bridge_button_clicked_callback), live_lv2_bridge);
    }

    list = list->next;
  }

  g_list_free(start_list);
}

void
ags_live_lv2_bridge_disconnect(AgsConnectable *connectable)
{
  //TODO:JK: implement me
}

void
ags_live_lv2_bridge_resize_audio_channels(AgsMachine *machine,
					  guint audio_channels, guint audio_channels_old,
					  gpointer data)
{
  AgsLiveLv2Bridge *live_lv2_bridge;
  
  live_lv2_bridge = (AgsLiveLv2Bridge *) machine;

  if(audio_channels > audio_channels_old){
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

  gboolean grow;

  live_lv2_bridge = (AgsLiveLv2Bridge *) machine;

  if(pads_old < pads){
    grow = TRUE;
  }else{
    grow = FALSE;
  }
  
  if(g_type_is_a(channel_type, AGS_TYPE_INPUT)){
    if(grow){
      /* AgsInput */
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
  AgsNavigation *navigation;
  AgsLiveLv2Bridge *live_lv2_bridge;
  
  AgsAudio *audio;

  AgsApplicationContext *application_context;

  GList *start_play, *start_recall;
  GList *start_list, *list;
  
  gint position;
  
  if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0 ||
     (AGS_MACHINE_PREMAPPED_RECALL & (machine->flags)) != 0){
    return;
  }

  application_context = ags_application_context_get_instance();

  navigation = (AgsNavigation *) ags_ui_provider_get_navigation(AGS_UI_PROVIDER(application_context));

  live_lv2_bridge = (AgsLiveLv2Bridge *) machine;

  audio = machine->audio;

  position = 0;

  /* add new controls */
  ags_effect_bulk_add_plugin(AGS_EFFECT_BULK(AGS_EFFECT_BRIDGE(machine->bridge)->bulk_input),
			     NULL,
			     live_lv2_bridge->lv2_play_container, live_lv2_bridge->lv2_recall_container,
			     "ags-fx-lv2",
			     live_lv2_bridge->filename,
			     live_lv2_bridge->effect,
			     0, 0,
			     0, 0,
			     position,
			     (AGS_FX_FACTORY_LIVE | AGS_FX_FACTORY_ADD | AGS_FX_FACTORY_INPUT), 0);

  if((AGS_MACHINE_IS_SYNTHESIZER & (machine->flags)) != 0){
    start_play = ags_audio_get_play(audio);
    start_recall = ags_audio_get_recall(audio);

    list =
      start_list = NULL;

    if(start_play != NULL &&
       start_recall != NULL){
      start_list = g_list_concat(start_play,
				 start_recall);
    }
  
    while((list = ags_recall_template_find_type(list, AGS_TYPE_FX_NOTATION_AUDIO)) != NULL){
      AgsPort *port;

      GValue value = G_VALUE_INIT;
    
      /* loop */
      port = NULL;
    
      g_object_get(list->data,
		   "loop", &port,
		   NULL);

      g_value_init(&value,
		   G_TYPE_BOOLEAN);

      g_value_set_boolean(&value,
			  gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(navigation->loop)));

      ags_port_safe_write(port,
			  &value);

      if(port != NULL){
	g_object_unref(port);
      }
    
      /* loop start */
      port = NULL;
    
      g_object_get(list->data,
		   "loop-start", &port,
		   NULL);

      g_value_unset(&value);
      g_value_init(&value,
		   G_TYPE_UINT64);

      g_value_set_uint64(&value,
			 16 * gtk_spin_button_get_value_as_int(navigation->loop_left_tact));

      ags_port_safe_write(port,
			  &value);

      if(port != NULL){
	g_object_unref(port);
      }
    
      /* loop end */
      port = NULL;
    
      g_object_get(list->data,
		   "loop-end", &port,
		   NULL);

      g_value_unset(&value);
      g_value_init(&value,
		   G_TYPE_UINT64);

      g_value_set_uint64(&value,
			 16 * gtk_spin_button_get_value_as_int(navigation->loop_right_tact));

      ags_port_safe_write(port,
			  &value);

      if(port != NULL){
	g_object_unref(port);
      }

      /* iterate */
      list = list->next;
    }
  
    g_list_free_full(start_list,
		     (GDestroyNotify) g_object_unref);

    /* ags-fx-envelope */
    start_recall = ags_fx_factory_create(audio,
					 live_lv2_bridge->envelope_play_container, live_lv2_bridge->envelope_recall_container,
					 "ags-fx-envelope",
					 NULL,
					 NULL,
					 0, 0,
					 0, 0,
					 position,
					 (AGS_FX_FACTORY_ADD | AGS_FX_FACTORY_INPUT), 0);

    g_list_free_full(start_recall,
		     (GDestroyNotify) g_object_unref);

    /* ags-fx-buffer */
    start_recall = ags_fx_factory_create(audio,
					 live_lv2_bridge->buffer_play_container, live_lv2_bridge->buffer_recall_container,
					 "ags-fx-buffer",
					 NULL,
					 NULL,
					 0, 0,
					 0, 0,
					 position,
					 (AGS_FX_FACTORY_ADD | AGS_FX_FACTORY_INPUT), 0);

    g_list_free_full(start_recall,
		     (GDestroyNotify) g_object_unref);
  }

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

  GList *start_recall;

  gint position;
  guint input_pads;
  guint audio_channels;

  if(live_lv2_bridge->mapped_input_pad > input_pad_start){
    return;
  }

  audio = AGS_MACHINE(live_lv2_bridge)->audio;

  position = 0;

  input_pads = 0;
  audio_channels = 0;

  /* get some fields */
  g_object_get(audio,
	       "input-pads", &input_pads,
	       "audio-channels", &audio_channels,
	       NULL);

  /* add to effect bridge */
  ags_effect_bulk_add_plugin((AgsEffectBulk *) AGS_EFFECT_BRIDGE(AGS_MACHINE(live_lv2_bridge)->bridge)->bulk_input,
			     NULL,
			     live_lv2_bridge->lv2_play_container, live_lv2_bridge->lv2_recall_container,
			     "ags-fx-lv2",
			     live_lv2_bridge->filename,
			     live_lv2_bridge->effect,
			     audio_channel_start, audio_channels,
			     input_pad_start, input_pads,
			     position,
			     (AGS_FX_FACTORY_LIVE | AGS_FX_FACTORY_REMAP | AGS_FX_FACTORY_INPUT), 0);

  /* source */
  if((AGS_MACHINE_IS_SYNTHESIZER & (AGS_MACHINE(live_lv2_bridge)->flags)) != 0){
    /* ags-fx-envelope */
    start_recall = ags_fx_factory_create(audio,
					 live_lv2_bridge->envelope_play_container, live_lv2_bridge->envelope_recall_container,
					 "ags-fx-envelope",
					 NULL,
					 NULL,
					 audio_channel_start, audio_channels,
					 input_pad_start, input_pads,
					 position,
					 (AGS_FX_FACTORY_REMAP | AGS_FX_FACTORY_INPUT), 0);

    g_list_free_full(start_recall,
		     (GDestroyNotify) g_object_unref);

    /* ags-fx-buffer */
    start_recall = ags_fx_factory_create(audio,
					 live_lv2_bridge->buffer_play_container, live_lv2_bridge->buffer_recall_container,
					 "ags-fx-buffer",
					 NULL,
					 NULL,
					 audio_channel_start, audio_channels,
					 input_pad_start, input_pads,
					 position,
					 (AGS_FX_FACTORY_REMAP | AGS_FX_FACTORY_INPUT), 0);

    g_list_free_full(start_recall,
		     (GDestroyNotify) g_object_unref);
  }  

  live_lv2_bridge->mapped_input_pad = input_pads;
}

void
ags_live_lv2_bridge_output_map_recall(AgsLiveLv2Bridge *live_lv2_bridge, guint audio_channel_start, guint output_pad_start)
{
  AgsAudio *audio;

  guint output_pads;
  
  if(live_lv2_bridge->mapped_output_pad > output_pad_start){
    return;
  }

  audio = AGS_MACHINE(live_lv2_bridge)->audio;

  output_pads = 0;

  /* get some fields */
  g_object_get(audio,
	       "output-pads", &output_pads,
	       NULL);

  live_lv2_bridge->mapped_output_pad = output_pads;
}

void
ags_live_lv2_bridge_load_program(AgsLiveLv2Bridge *live_lv2_bridge)
{
  AgsLv2Plugin *lv2_plugin;

  LV2_Descriptor *plugin_descriptor;
  LV2_Programs_Interface *program_interface;

  /* retrieve lv2 plugin */
  lv2_plugin = live_lv2_bridge->lv2_plugin;
  
  if(lv2_plugin == NULL){
    lv2_plugin =
      live_lv2_bridge->lv2_plugin = ags_lv2_manager_find_lv2_plugin(ags_lv2_manager_get_instance(),
								    live_lv2_bridge->filename,
								    live_lv2_bridge->effect);
    g_object_ref(lv2_plugin);

    live_lv2_bridge->lv2_descriptor = AGS_BASE_PLUGIN(lv2_plugin)->plugin_descriptor;
  }
  
  if(lv2_plugin == NULL){
    return;
  }

  plugin_descriptor = live_lv2_bridge->lv2_descriptor;
  
  if(plugin_descriptor != NULL &&
     plugin_descriptor->extension_data != NULL &&
     (program_interface = plugin_descriptor->extension_data(LV2_PROGRAMS__Interface)) != NULL){
    GtkListStore *model;
    
    GtkTreeIter iter;
    
    LV2_Program_Descriptor *program_descriptor;

    uint32_t i;

    if(live_lv2_bridge->lv2_handle == NULL){
      guint samplerate;
      guint buffer_size;

      g_object_get(AGS_MACHINE(live_lv2_bridge)->audio,
		   "samplerate", &samplerate,
		   "buffer-size", &buffer_size,
		   NULL);
      
      live_lv2_bridge->lv2_handle = (LV2_Handle *) ags_base_plugin_instantiate((AgsBasePlugin *) lv2_plugin,
									       samplerate, buffer_size);
    }

    if(live_lv2_bridge->lv2_handle != NULL){
      if(live_lv2_bridge->port_value == NULL){
	GList *start_plugin_port, *plugin_port;
      
	guint port_count;

	port_count = g_list_length(AGS_BASE_PLUGIN(lv2_plugin)->plugin_port);

	if(port_count > 0){
	  live_lv2_bridge->port_value = (float *) malloc(port_count * sizeof(float));
	}

	g_object_get(lv2_plugin,
		     "plugin-port", &start_plugin_port,
		     NULL);
      
	plugin_port = start_plugin_port;
      
	while(plugin_port != NULL){
	  if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_CONTROL)){
	    if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_INPUT)){
	      plugin_descriptor->connect_port(live_lv2_bridge->lv2_handle[0],
					      AGS_PLUGIN_PORT(plugin_port->data)->port_index,
					      &(live_lv2_bridge->port_value[AGS_PLUGIN_PORT(plugin_port->data)->port_index]));
	    }
	  }

	  plugin_port = plugin_port->next;
	}

	g_list_free_full(start_plugin_port,
			 g_object_unref);
      }
    
      if(live_lv2_bridge->program == NULL){
	if((AGS_CONNECTABLE_CONNECTED & (AGS_MACHINE(live_lv2_bridge)->connectable_flags)) != 0){
	  g_signal_connect_after(G_OBJECT(live_lv2_bridge->program), "changed",
				 G_CALLBACK(ags_live_lv2_bridge_program_changed_callback), live_lv2_bridge);
	}
      }else{
	model = GTK_LIST_STORE(gtk_combo_box_get_model(GTK_COMBO_BOX(live_lv2_bridge->program)));
      
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
}

void
ags_live_lv2_bridge_load_preset(AgsLiveLv2Bridge *live_lv2_bridge)
{
  AgsLv2Plugin *lv2_plugin;

  GList *list;  

  /* retrieve lv2 plugin */
  lv2_plugin = live_lv2_bridge->lv2_plugin;
  
  if(lv2_plugin == NULL){
    lv2_plugin =
      live_lv2_bridge->lv2_plugin = ags_lv2_manager_find_lv2_plugin(ags_lv2_manager_get_instance(),
								    live_lv2_bridge->filename,
								    live_lv2_bridge->effect);
    g_object_ref(lv2_plugin);

    live_lv2_bridge->lv2_descriptor = AGS_BASE_PLUGIN(lv2_plugin)->plugin_descriptor;
  }
  
  if(lv2_plugin == NULL){
    return;
  }
  
  /* preset */
  list = lv2_plugin->preset;

  while(list != NULL){
    if(AGS_LV2_PRESET(list->data)->preset_label != NULL){
      gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(live_lv2_bridge->preset),
				     AGS_LV2_PRESET(list->data)->preset_label);
    }
    
    list = list->next;
  }

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
  lv2_plugin = live_lv2_bridge->lv2_plugin;
  
  if(lv2_plugin == NULL){
    lv2_plugin =
      live_lv2_bridge->lv2_plugin = ags_lv2_manager_find_lv2_plugin(ags_lv2_manager_get_instance(),
								    live_lv2_bridge->filename,
								    live_lv2_bridge->effect);
    g_object_ref(lv2_plugin);

    live_lv2_bridge->lv2_descriptor = AGS_BASE_PLUGIN(lv2_plugin)->plugin_descriptor;
  }
  
  if(lv2_plugin == NULL ||
     lv2_plugin->ui_uri == NULL){
    return;
  }

  /* retrieve lv2ui plugin */
  lv2ui_plugin = live_lv2_bridge->lv2ui_plugin;
  
  if(lv2ui_plugin == NULL){
    list = ags_lv2ui_plugin_find_gui_uri(ags_lv2ui_manager_get_instance()->lv2ui_plugin,
					 lv2_plugin->ui_uri);

    if(list == NULL){
      return;
    }

    lv2ui_plugin =
      live_lv2_bridge->lv2ui_plugin = list->data;
    g_object_ref(lv2ui_plugin);

    live_lv2_bridge->ui_descriptor = AGS_BASE_PLUGIN(lv2ui_plugin)->ui_plugin_descriptor;
  }
  
#ifdef AGS_DEBUG
  g_message("ui filename - %s, %s", AGS_BASE_PLUGIN(lv2ui_plugin)->ui_filename, lv2_plugin->ui_uri);
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
  AgsLv2Plugin *lv2_plugin;
  
  gchar *uri;

  guint samplerate;
  guint buffer_size;
  
  /* retrieve lv2 plugin */
  lv2_plugin = live_lv2_bridge->lv2_plugin;

  if(lv2_plugin == NULL){
    lv2_plugin =
      live_lv2_bridge->lv2_plugin = ags_lv2_manager_find_lv2_plugin(ags_lv2_manager_get_instance(),
								    live_lv2_bridge->filename,
								    live_lv2_bridge->effect);
    g_object_ref(lv2_plugin);

    live_lv2_bridge->lv2_descriptor = AGS_BASE_PLUGIN(lv2_plugin)->plugin_descriptor;
  }
  
  if(lv2_plugin == NULL){
    return;
  }

  /* get uri */
  g_object_get(lv2_plugin,
	       "uri", &uri,
	       NULL);
  
  /* URI */
  g_object_set(live_lv2_bridge,
	       "uri", uri,
	       NULL);

  /* samplerate and buffer size */
  samplerate = ags_soundcard_helper_config_get_samplerate(ags_config_get_instance());
  buffer_size = ags_soundcard_helper_config_get_buffer_size(ags_config_get_instance());
  
  /* program */
  live_lv2_bridge->lv2_handle = ags_base_plugin_instantiate((AgsBasePlugin *) lv2_plugin,
							    samplerate, buffer_size);
  
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
 * Since: 3.0.0
 */
gboolean
ags_live_lv2_bridge_lv2ui_idle_timeout(GtkWidget *widget)
{
  AgsLiveLv2Bridge *live_lv2_bridge;

  AgsLv2uiPlugin *lv2ui_plugin;

  int retval;
    
  if((live_lv2_bridge = g_hash_table_lookup(ags_live_lv2_bridge_lv2ui_idle,
					    widget)) != NULL){    
    lv2ui_plugin = live_lv2_bridge->lv2ui_plugin;

    if(lv2ui_plugin->feature != NULL &&
       lv2ui_plugin->feature[0]->data != NULL){
      retval = ((LV2UI_Idle_Interface *) lv2ui_plugin->feature[0]->data)->idle(live_lv2_bridge->ui_handle[0]);

      if(retval != 0){
	g_hash_table_remove(ags_live_lv2_bridge_lv2ui_handle,
			    live_lv2_bridge->ui_handle);
	
	live_lv2_bridge->ui_handle = NULL;

	return(FALSE);
      }
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
 * Create a new instance of #AgsLiveLv2Bridge
 *
 * Returns: the new #AgsLiveLv2Bridge
 *
 * Since: 3.0.0
 */
AgsLiveLv2Bridge*
ags_live_lv2_bridge_new(GObject *soundcard,
			gchar *filename,
			gchar *effect)
{
  AgsLiveLv2Bridge *live_lv2_bridge;

  live_lv2_bridge = (AgsLiveLv2Bridge *) g_object_new(AGS_TYPE_LIVE_LV2_BRIDGE,
						      NULL);

  g_object_set(AGS_MACHINE(live_lv2_bridge)->audio,
	       "output-soundcard", soundcard,
	       NULL);

  g_object_set(live_lv2_bridge,
	       "filename", filename,
	       "effect", effect,
	       NULL);

  return(live_lv2_bridge);
}
