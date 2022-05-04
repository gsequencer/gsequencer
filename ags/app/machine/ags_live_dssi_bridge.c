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

#include <ags/app/machine/ags_live_dssi_bridge.h>
#include <ags/app/machine/ags_live_dssi_bridge_callbacks.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_window.h>
#include <ags/app/ags_composite_editor.h>
#include <ags/app/ags_navigation.h>
#include <ags/app/ags_effect_bridge.h>
#include <ags/app/ags_effect_bulk.h>
#include <ags/app/ags_bulk_member.h>

#include <ags/config.h>

#if defined(AGS_W32API)
#include <windows.h>
#else
#include <dlfcn.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include <ladspa.h>
#include <dssi.h>

#include <ags/i18n.h>

void ags_live_dssi_bridge_class_init(AgsLiveDssiBridgeClass *live_dssi_bridge);
void ags_live_dssi_bridge_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_live_dssi_bridge_init(AgsLiveDssiBridge *live_dssi_bridge);
void ags_live_dssi_bridge_set_property(GObject *gobject,
				       guint prop_id,
				       const GValue *value,
				       GParamSpec *param_spec);
void ags_live_dssi_bridge_get_property(GObject *gobject,
				       guint prop_id,
				       GValue *value,
				       GParamSpec *param_spec);
void ags_live_dssi_bridge_finalize(GObject *gobject);

void ags_live_dssi_bridge_connect(AgsConnectable *connectable);
void ags_live_dssi_bridge_disconnect(AgsConnectable *connectable);

void ags_live_dssi_bridge_resize_audio_channels(AgsMachine *machine,
						guint audio_channels, guint audio_channels_old,
						gpointer data);
void ags_live_dssi_bridge_resize_pads(AgsMachine *machine, GType type,
				      guint pads, guint pads_old,
				      gpointer data);

void ags_live_dssi_bridge_map_recall(AgsMachine *machine);

/**
 * SECTION:ags_live_dssi_bridge
 * @short_description: A composite widget to visualize a bunch of #AgsChannel
 * @title: AgsLiveDssiBridge
 * @section_id:
 * @include: ags/app/machine/ags_live_dssi_bridge.h
 *
 * #AgsLiveDssiBridge is a composite widget to visualize all #AgsChannel. It should be
 * packed by an #AgsMachine.
 */

enum{
  PROP_0,
  PROP_FILENAME,
  PROP_EFFECT,
  PROP_INDEX,
};

static gpointer ags_live_dssi_bridge_parent_class = NULL;
static AgsConnectableInterface* ags_live_dssi_bridge_parent_connectable_interface;

extern GHashTable *ags_effect_bulk_indicator_queue_draw;

GType
ags_live_dssi_bridge_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_live_dssi_bridge = 0;

    static const GTypeInfo ags_live_dssi_bridge_info = {
      sizeof(AgsLiveDssiBridgeClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_live_dssi_bridge_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsLiveDssiBridge),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_live_dssi_bridge_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_live_dssi_bridge_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_live_dssi_bridge = g_type_register_static(AGS_TYPE_MACHINE,
						       "AgsLiveDssiBridge", &ags_live_dssi_bridge_info,
						       0);

    g_type_add_interface_static(ags_type_live_dssi_bridge,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_live_dssi_bridge);
  }

  return g_define_type_id__volatile;
}

void
ags_live_dssi_bridge_class_init(AgsLiveDssiBridgeClass *live_dssi_bridge)
{
  AgsMachineClass *machine;
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_live_dssi_bridge_parent_class = g_type_class_peek_parent(live_dssi_bridge);

  /* GObjectClass */
  gobject = G_OBJECT_CLASS(live_dssi_bridge);

  gobject->set_property = ags_live_dssi_bridge_set_property;
  gobject->get_property = ags_live_dssi_bridge_get_property;

  gobject->finalize = ags_live_dssi_bridge_finalize;
  
  /* properties */
  /**
   * AgsRecallLiveDssi:filename:
   *
   * The plugins filename.
   * 
   * Since: 3.0.0
   */
  param_spec =  g_param_spec_string("filename",
				    i18n_pspec("the object file"),
				    i18n_pspec("The filename as string of object file"),
				    NULL,
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FILENAME,
				  param_spec);

  /**
   * AgsRecallLiveDssi:effect:
   *
   * The effect's name.
   * 
   * Since: 3.0.0
   */
  param_spec =  g_param_spec_string("effect",
				    i18n_pspec("the effect"),
				    i18n_pspec("The effect's string representation"),
				    NULL,
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_EFFECT,
				  param_spec);

  /**
   * AgsRecallLiveDssi:index:
   *
   * The effect's index.
   * 
   * Since: 3.0.0
   */
  param_spec =  g_param_spec_uint("index",
				  i18n_pspec("index of effect"),
				  i18n_pspec("The numerical index of effect"),
				  0,
				  65535,
				  0,
				  G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_INDEX,
				  param_spec);

  /* AgsMachine */
  machine = (AgsMachineClass *) live_dssi_bridge;

  machine->map_recall = ags_live_dssi_bridge_map_recall;
}

void
ags_live_dssi_bridge_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_live_dssi_bridge_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_live_dssi_bridge_connect;
  connectable->disconnect = ags_live_dssi_bridge_disconnect;
}

void
ags_live_dssi_bridge_init(AgsLiveDssiBridge *live_dssi_bridge)
{
  AgsWindow *window;
  AgsCompositeEditor *composite_editor;
  GtkBox *vbox;
  GtkBox *hbox;
  GtkLabel *label;

  AgsAudio *audio;

  AgsMachineCounterManager *machine_counter_manager;
  AgsMachineCounter *machine_counter;

  AgsApplicationContext *application_context;
  
  gchar *machine_name;

  gint position;

  application_context = ags_application_context_get_instance();
  
  /* machine counter */
  machine_counter_manager = ags_machine_counter_manager_get_instance();

  machine_counter = ags_machine_counter_manager_find_machine_counter(machine_counter_manager,
								     AGS_TYPE_LIVE_DSSI_BRIDGE);

  machine_name = NULL;

  if(machine_counter != NULL){
    machine_name = g_strdup_printf("Default %d",
				   machine_counter->counter);
  
    ags_machine_counter_increment(machine_counter);
  }
  
  g_object_set(live_dssi_bridge,
	       "machine-name", machine_name,
	       NULL);

  g_free(machine_name);
  
  /* machine selector */
  window = ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));

  composite_editor = ags_ui_provider_get_composite_editor(AGS_UI_PROVIDER(application_context));

  position = g_list_length(window->machine);
  
  ags_machine_selector_popup_insert_machine(composite_editor->machine_selector,
					    position,
					    live_dssi_bridge);

  audio = AGS_MACHINE(live_dssi_bridge)->audio;
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
  
  AGS_MACHINE(live_dssi_bridge)->flags |= (AGS_MACHINE_IS_SYNTHESIZER |
					   AGS_MACHINE_REVERSE_NOTATION);

  g_signal_connect_after(G_OBJECT(live_dssi_bridge), "resize-audio-channels",
			 G_CALLBACK(ags_live_dssi_bridge_resize_audio_channels), NULL);

  g_signal_connect_after(G_OBJECT(live_dssi_bridge), "resize-pads",
			 G_CALLBACK(ags_live_dssi_bridge_resize_pads), NULL);
    
  live_dssi_bridge->flags = 0;

  live_dssi_bridge->name = NULL;

  live_dssi_bridge->version = AGS_LIVE_DSSI_BRIDGE_DEFAULT_VERSION;
  live_dssi_bridge->build_id = AGS_LIVE_DSSI_BRIDGE_DEFAULT_BUILD_ID;

  live_dssi_bridge->xml_type = "ags-live-dssi-bridge";
  
  live_dssi_bridge->mapped_output_pad = 0;
  live_dssi_bridge->mapped_input_pad = 0;

  live_dssi_bridge->dssi_play_container = ags_recall_container_new();
  live_dssi_bridge->dssi_recall_container = ags_recall_container_new();

  live_dssi_bridge->envelope_play_container = ags_recall_container_new();
  live_dssi_bridge->envelope_recall_container = ags_recall_container_new();

  live_dssi_bridge->buffer_play_container = ags_recall_container_new();
  live_dssi_bridge->buffer_recall_container = ags_recall_container_new();

  live_dssi_bridge->filename = NULL;
  live_dssi_bridge->effect = NULL;
  live_dssi_bridge->effect_index = 0;

  live_dssi_bridge->port_values = NULL;
  live_dssi_bridge->dssi_descriptor = NULL;
  
  vbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
				0);
  gtk_frame_set_child(AGS_MACHINE(live_dssi_bridge)->frame,
		      (GtkWidget *) vbox);

  /* program */
  hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				0);
  gtk_box_append(vbox,
		 (GtkWidget *) hbox);

  label = (GtkLabel *) gtk_label_new(i18n("program"));
  gtk_box_append(hbox,
		 (GtkWidget *) label);

  live_dssi_bridge->program = (GtkComboBoxText *) gtk_combo_box_text_new();
  gtk_box_append(hbox,
		 (GtkWidget *) live_dssi_bridge->program);

  /* effect bridge */
  AGS_MACHINE(live_dssi_bridge)->bridge = ags_effect_bridge_new(audio);
  gtk_box_append(vbox,
		 (GtkWidget *) AGS_MACHINE(live_dssi_bridge)->bridge);
  
  AGS_EFFECT_BRIDGE(AGS_MACHINE(live_dssi_bridge)->bridge)->bulk_input = (GtkWidget *) ags_effect_bulk_new(audio,
													   AGS_TYPE_INPUT);
  ags_effect_bulk_set_flags(AGS_EFFECT_BULK(AGS_EFFECT_BRIDGE(AGS_MACHINE(live_dssi_bridge)->bridge)->bulk_input),
			    (AGS_EFFECT_BULK_HIDE_BUTTONS |
			     AGS_EFFECT_BULK_HIDE_ENTRIES |
			     AGS_EFFECT_BULK_SHOW_LABELS));

  gtk_widget_set_valign((GtkWidget *) AGS_EFFECT_BRIDGE(AGS_MACHINE(live_dssi_bridge)->bridge)->bulk_input,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) AGS_EFFECT_BRIDGE(AGS_MACHINE(live_dssi_bridge)->bridge)->bulk_input,
			GTK_ALIGN_FILL);
  
  gtk_grid_attach(AGS_MACHINE(live_dssi_bridge)->bridge,
		  (GtkWidget *) AGS_EFFECT_BRIDGE(AGS_MACHINE(live_dssi_bridge)->bridge)->bulk_input,
		  0, 0,
		  1, 1);
}

void
ags_live_dssi_bridge_set_property(GObject *gobject,
				  guint prop_id,
				  const GValue *value,
				  GParamSpec *param_spec)
{
  AgsLiveDssiBridge *live_dssi_bridge;

  live_dssi_bridge = AGS_LIVE_DSSI_BRIDGE(gobject);

  switch(prop_id){
  case PROP_FILENAME:
    {
      gchar *str;
      gchar *filename;

      filename = g_value_get_string(value);

      if(filename == live_dssi_bridge->filename){
	return;
      }

      if(live_dssi_bridge->filename != NULL){
	g_free(live_dssi_bridge->filename);
      }

      if(filename != NULL){
	if(!g_file_test(filename,
			G_FILE_TEST_EXISTS)){
	  AgsWindow *window;

	  window = (AgsWindow *) gtk_widget_get_ancestor((GtkWidget *) live_dssi_bridge,
							 AGS_TYPE_WINDOW);

	  str = g_strdup_printf("%s %s",
				i18n("Plugin file not present"),
				filename);
	  ags_window_show_error(window,
				str);

	  g_free(str);
	}
      }

      live_dssi_bridge->filename = g_strdup(filename);
    }
    break;
  case PROP_EFFECT:
    {
      gchar *effect;
      
      effect = g_value_get_string(value);

      if(effect == live_dssi_bridge->effect){
	return;
      }

      if(live_dssi_bridge->effect != NULL){
	g_free(live_dssi_bridge->effect);
      }

      live_dssi_bridge->effect = g_strdup(effect);
    }
    break;
  case PROP_INDEX:
    {
      unsigned long effect_index;
      
      effect_index = (unsigned long) g_value_get_uint(value);

      if(effect_index == live_dssi_bridge->effect_index){
	return;
      }

      live_dssi_bridge->effect_index = effect_index;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_live_dssi_bridge_get_property(GObject *gobject,
				  guint prop_id,
				  GValue *value,
				  GParamSpec *param_spec)
{
  AgsLiveDssiBridge *live_dssi_bridge;

  live_dssi_bridge = AGS_LIVE_DSSI_BRIDGE(gobject);

  switch(prop_id){
  case PROP_FILENAME:
    {
      g_value_set_string(value, live_dssi_bridge->filename);
    }
    break;
  case PROP_EFFECT:
    {
      g_value_set_string(value, live_dssi_bridge->effect);
    }
    break;
  case PROP_INDEX:
    {
      g_value_set_uint(value, (guint) live_dssi_bridge->effect_index);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_live_dssi_bridge_finalize(GObject *gobject)
{
  AgsLiveDssiBridge *live_dssi_bridge;

  live_dssi_bridge = (AgsLiveDssiBridge *) gobject;
  
  g_object_disconnect(G_OBJECT(live_dssi_bridge),
		      "any_signal::resize-audio-channels",
		      G_CALLBACK(ags_live_dssi_bridge_resize_audio_channels),
		      NULL,
		      "any_signal::resize-pads",
		      G_CALLBACK(ags_live_dssi_bridge_resize_pads),
		      NULL,
		      NULL);

  g_free(live_dssi_bridge->filename);
  g_free(live_dssi_bridge->effect);
  
  /* call parent */
  G_OBJECT_CLASS(ags_live_dssi_bridge_parent_class)->finalize(gobject);
}

void
ags_live_dssi_bridge_connect(AgsConnectable *connectable)
{
  AgsLiveDssiBridge *live_dssi_bridge;

  if((AGS_CONNECTABLE_CONNECTED & (AGS_MACHINE(connectable)->connectable_flags)) != 0){
    return;
  }

  ags_live_dssi_bridge_parent_connectable_interface->connect(connectable);

  live_dssi_bridge = AGS_LIVE_DSSI_BRIDGE(connectable);

  g_signal_connect_after(G_OBJECT(live_dssi_bridge->program), "changed",
			 G_CALLBACK(ags_live_dssi_bridge_program_changed_callback), live_dssi_bridge);
}

void
ags_live_dssi_bridge_disconnect(AgsConnectable *connectable)
{
  AgsLiveDssiBridge *live_dssi_bridge;

  if((AGS_CONNECTABLE_CONNECTED & (AGS_MACHINE(connectable)->connectable_flags)) == 0){
    return;
  }

  ags_live_dssi_bridge_parent_connectable_interface->connect(connectable);

  live_dssi_bridge = AGS_LIVE_DSSI_BRIDGE(connectable);

  g_object_disconnect(G_OBJECT(live_dssi_bridge->program),
		      "any_signal::changed",
		      G_CALLBACK(ags_live_dssi_bridge_program_changed_callback),
		      live_dssi_bridge,
		      NULL);
}

void
ags_live_dssi_bridge_resize_audio_channels(AgsMachine *machine,
					   guint audio_channels, guint audio_channels_old,
					   gpointer data)
{
  AgsLiveDssiBridge *live_dssi_bridge;
  
  live_dssi_bridge = (AgsLiveDssiBridge *) machine;

  if(audio_channels > audio_channels_old){
    /* recall */
    if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0){
      ags_live_dssi_bridge_input_map_recall(live_dssi_bridge,
					    audio_channels_old,
					    0);

      ags_live_dssi_bridge_output_map_recall(live_dssi_bridge,
					     audio_channels_old,
					     0);
    }
  }
}

void
ags_live_dssi_bridge_resize_pads(AgsMachine *machine, GType channel_type,
				 guint pads, guint pads_old,
				 gpointer data)
{
  AgsLiveDssiBridge *live_dssi_bridge;

  gboolean grow;

  live_dssi_bridge = (AgsLiveDssiBridge *) machine;

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
	ags_live_dssi_bridge_input_map_recall(live_dssi_bridge,
					      0,
					      pads_old);
      }
    }else{
      live_dssi_bridge->mapped_input_pad = pads;
    }
  }else{
    if(grow){
      /* recall */
      if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0){
	ags_live_dssi_bridge_output_map_recall(live_dssi_bridge,
					       0,
					       pads_old);
      }
    }else{
      live_dssi_bridge->mapped_output_pad = pads;
    }
  }
}

void
ags_live_dssi_bridge_map_recall(AgsMachine *machine)
{  
  AgsNavigation *navigation;
  AgsLiveDssiBridge *live_dssi_bridge;
  
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

  live_dssi_bridge = (AgsLiveDssiBridge *) machine;

  audio = machine->audio;

  position = 0;

  /* add new controls */
  ags_effect_bulk_add_plugin(AGS_EFFECT_BULK(AGS_EFFECT_BRIDGE(machine->bridge)->bulk_input),
			     NULL,
			     live_dssi_bridge->dssi_play_container, live_dssi_bridge->dssi_recall_container,
			     "ags-fx-dssi",
			     live_dssi_bridge->filename,
			     live_dssi_bridge->effect,
			     0, 0,
			     0, 0,
			     position,
			     (AGS_FX_FACTORY_LIVE | AGS_FX_FACTORY_ADD | AGS_FX_FACTORY_INPUT), 0);

  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);

  list =
    start_list = NULL;

  if(start_play != NULL &&
     start_recall != NULL){
    list =
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
				       live_dssi_bridge->envelope_play_container, live_dssi_bridge->envelope_recall_container,
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
				       live_dssi_bridge->buffer_play_container, live_dssi_bridge->buffer_recall_container,
				       "ags-fx-buffer",
				       NULL,
				       NULL,
				       0, 0,
				       0, 0,
				       position,
				       (AGS_FX_FACTORY_ADD | AGS_FX_FACTORY_INPUT), 0);

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);

  /* depending on destination */
  ags_live_dssi_bridge_input_map_recall(live_dssi_bridge,
					0,
					0);

  /* depending on destination */
  ags_live_dssi_bridge_output_map_recall(live_dssi_bridge,
					 0,
					 0);

  /* call parent */
  AGS_MACHINE_CLASS(ags_live_dssi_bridge_parent_class)->map_recall(machine);
}

void
ags_live_dssi_bridge_input_map_recall(AgsLiveDssiBridge *live_dssi_bridge,
				      guint audio_channel_start,
				      guint input_pad_start)
{
  AgsAudio *audio;

  GList *start_recall;

  gint position;
  guint input_pads;
  guint audio_channels;

  if(live_dssi_bridge->mapped_input_pad > input_pad_start){
    return;
  }

  audio = AGS_MACHINE(live_dssi_bridge)->audio;

  position = 0;

  input_pads = 0;
  audio_channels = 0;

  /* get some fields */
  g_object_get(audio,
	       "input-pads", &input_pads,
	       "audio-channels", &audio_channels,
	       NULL);

  /* add to effect bridge */
  ags_effect_bulk_add_plugin((AgsEffectBulk *) AGS_EFFECT_BRIDGE(AGS_MACHINE(live_dssi_bridge)->bridge)->bulk_input,
			     NULL,
			     live_dssi_bridge->dssi_play_container, live_dssi_bridge->dssi_recall_container,
			     "ags-fx-dssi",
			     live_dssi_bridge->filename,
			     live_dssi_bridge->effect,
			     audio_channel_start, audio_channels,
			     input_pad_start, input_pads,
			     position,
			     (AGS_FX_FACTORY_LIVE | AGS_FX_FACTORY_REMAP | AGS_FX_FACTORY_INPUT), 0);

  /* source */
  if((AGS_MACHINE_IS_SYNTHESIZER & (AGS_MACHINE(live_dssi_bridge)->flags)) != 0){
    /* ags-fx-envelope */
    start_recall = ags_fx_factory_create(audio,
					 live_dssi_bridge->envelope_play_container, live_dssi_bridge->envelope_recall_container,
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
					 live_dssi_bridge->buffer_play_container, live_dssi_bridge->buffer_recall_container,
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

  live_dssi_bridge->mapped_input_pad = input_pads;
}

void
ags_live_dssi_bridge_output_map_recall(AgsLiveDssiBridge *live_dssi_bridge,
				       guint audio_channel_start,
				       guint output_pad_start)
{
  AgsAudio *audio;

  guint output_pads;
  
  if(live_dssi_bridge->mapped_output_pad > output_pad_start){
    return;
  }

  audio = AGS_MACHINE(live_dssi_bridge)->audio;

  output_pads = 0;

  /* get some fields */
  g_object_get(audio,
	       "output-pads", &output_pads,
	       NULL);

  live_dssi_bridge->mapped_output_pad = output_pads;
}

void
ags_live_dssi_bridge_load(AgsLiveDssiBridge *live_dssi_bridge)
{
  GtkListStore *model;

  GtkTreeIter iter;

  AgsDssiPlugin *dssi_plugin;

  GList *start_plugin_port, *plugin_port;
  
  void *plugin_so;
  DSSI_Descriptor_Function dssi_descriptor;
  DSSI_Descriptor *plugin_descriptor;
  DSSI_Program_Descriptor *program_descriptor;
  LADSPA_PortDescriptor *port_descriptor;

  unsigned long samplerate;
  unsigned long effect_index;
  unsigned long port_count;
  unsigned long i;
  
  samplerate = ags_soundcard_helper_config_get_samplerate(ags_config_get_instance());

  g_message("ags_live_dssi_bridge.c - load %s %s", live_dssi_bridge->filename, live_dssi_bridge->effect);
 
  /* load plugin */
  dssi_plugin = ags_dssi_manager_find_dssi_plugin(ags_dssi_manager_get_instance(),
						  live_dssi_bridge->filename,
						  live_dssi_bridge->effect);

  plugin_so = AGS_BASE_PLUGIN(dssi_plugin)->plugin_so;

  /*  */
  gtk_list_store_clear(GTK_LIST_STORE(gtk_combo_box_get_model(GTK_COMBO_BOX(live_dssi_bridge->program))));
  
  /*  */
  effect_index = AGS_BASE_PLUGIN(dssi_plugin)->effect_index;

  /* load ports */
  model = gtk_list_store_new(3,
			     G_TYPE_STRING,
			     G_TYPE_ULONG,
			     G_TYPE_ULONG);

  if(effect_index != -1 &&
     plugin_so){
    gboolean success;

    success = FALSE;
    
#ifdef AGS_W32API
    dssi_descriptor = (DSSI_Descriptor_Function) GetProcAddress(plugin_so,
								"dssi_descriptor");

    success = (!dssi_descriptor) ? FALSE: TRUE;
#else
    dssi_descriptor = (DSSI_Descriptor_Function) dlsym(plugin_so,
						       "dssi_descriptor");

    success = (dlerror() == NULL) ? TRUE: FALSE;
#endif

    if(success && dssi_descriptor){
      live_dssi_bridge->dssi_descriptor = 
	plugin_descriptor = dssi_descriptor(effect_index);

      live_dssi_bridge->ladspa_handle = plugin_descriptor->LADSPA_Plugin->instantiate(plugin_descriptor->LADSPA_Plugin,
										      samplerate);
      port_count = plugin_descriptor->LADSPA_Plugin->PortCount;
      port_descriptor = plugin_descriptor->LADSPA_Plugin->PortDescriptors;

      g_object_get(dssi_plugin,
		   "plugin-port", &start_plugin_port,
		   NULL);
  
      live_dssi_bridge->port_values = (LADSPA_Data *) malloc(plugin_descriptor->LADSPA_Plugin->PortCount * sizeof(LADSPA_Data));
      
      if(live_dssi_bridge->ladspa_handle != NULL){
	for(i = 0; i < port_count; i++){
	  if(LADSPA_IS_PORT_CONTROL(port_descriptor[i])){
	    if(LADSPA_IS_PORT_INPUT(port_descriptor[i]) ||
	       LADSPA_IS_PORT_OUTPUT(port_descriptor[i])){
	      gchar *specifier;
	    
	      plugin_port = start_plugin_port;
	      specifier = plugin_descriptor->LADSPA_Plugin->PortNames[i];

	      while(plugin_port != NULL){
		if(!g_strcmp0(specifier,
			      AGS_PLUGIN_PORT(plugin_port->data)->port_name)){
		  live_dssi_bridge->port_values[i] = g_value_get_float(AGS_PLUGIN_PORT(plugin_port->data)->default_value);

		  break;
		}

		plugin_port = plugin_port->next;
	      }
	    
	      plugin_descriptor->LADSPA_Plugin->connect_port(live_dssi_bridge->ladspa_handle,
							     i,
							     &(live_dssi_bridge->port_values[i]));
	    }
	  }
	}

	if(plugin_descriptor->get_program != NULL){
	  for(i = 0; (program_descriptor = plugin_descriptor->get_program(live_dssi_bridge->ladspa_handle, i)) != NULL; i++){
	    gtk_list_store_append(model, &iter);
	    gtk_list_store_set(model, &iter,
			       0, program_descriptor->Name,
			       1, program_descriptor->Bank,
			       2, program_descriptor->Program,
			       -1);
	  }
	}

	g_list_free_full(start_plugin_port,
			 g_object_unref);
      }
    }
  }
  
  gtk_combo_box_set_model(GTK_COMBO_BOX(live_dssi_bridge->program),
			  GTK_TREE_MODEL(model));
}

/**
 * ags_live_dssi_bridge_new:
 * @soundcard: the assigned soundcard.
 * @filename: the plugin.so
 * @effect: the effect
 *
 * Create a new instance of #AgsLiveDssiBridge
 *
 * Returns: the new #AgsLiveDssiBridge
 *
 * Since: 3.0.0
 */
AgsLiveDssiBridge*
ags_live_dssi_bridge_new(GObject *soundcard,
			 gchar *filename,
			 gchar *effect)
{
  AgsLiveDssiBridge *live_dssi_bridge;

  live_dssi_bridge = (AgsLiveDssiBridge *) g_object_new(AGS_TYPE_LIVE_DSSI_BRIDGE,
							NULL);

  if(soundcard != NULL){
    g_object_set(G_OBJECT(AGS_MACHINE(live_dssi_bridge)->audio),
		 "output-soundcard", soundcard,
		 NULL);
  }

  g_object_set(live_dssi_bridge,
	       "filename", filename,
	       "effect", effect,
	       NULL);

  return(live_dssi_bridge);
}
