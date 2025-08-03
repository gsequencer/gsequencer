/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2025 Joël Krähemann
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

#include <ags/app/machine/ags_live_audio_unit_bridge.h>
#include <ags/app/machine/ags_live_audio_unit_bridge_callbacks.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_window.h>
#include <ags/app/ags_composite_editor.h>
#include <ags/app/ags_navigation.h>
#include <ags/app/ags_effect_bridge.h>
#include <ags/app/ags_effect_bulk.h>
#include <ags/app/ags_bulk_member.h>

#include <ags/i18n.h>

void ags_live_audio_unit_bridge_class_init(AgsLiveAudioUnitBridgeClass *live_audio_unit_bridge);
void ags_live_audio_unit_bridge_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_live_audio_unit_bridge_init(AgsLiveAudioUnitBridge *live_audio_unit_bridge);
void ags_live_audio_unit_bridge_set_property(GObject *gobject,
					     guint prop_id,
					     const GValue *value,
					     GParamSpec *param_spec);
void ags_live_audio_unit_bridge_get_property(GObject *gobject,
					     guint prop_id,
					     GValue *value,
					     GParamSpec *param_spec);
void ags_live_audio_unit_bridge_finalize(GObject *gobject);

void ags_live_audio_unit_bridge_connect(AgsConnectable *connectable);
void ags_live_audio_unit_bridge_disconnect(AgsConnectable *connectable);

void ags_live_audio_unit_bridge_resize_audio_channels(AgsMachine *machine,
						      guint audio_channels, guint audio_channels_old,
						      gpointer data);
void ags_live_audio_unit_bridge_resize_pads(AgsMachine *machine, GType type,
					    guint pads, guint pads_old,
					    gpointer data);

void ags_live_audio_unit_bridge_map_recall(AgsMachine *machine);

/**
 * SECTION:ags_live_audio_unit_bridge
 * @short_description: A composite widget to visualize a bunch of #AgsChannel
 * @title: AgsLiveAudioUnitBridge
 * @section_id:
 * @include: ags/app/machine/ags_live_audio_unit_bridge.h
 *
 * #AgsLiveAudioUnitBridge is a composite widget to visualize all #AgsChannel. It should be
 * packed by an #AgsMachine.
 */

enum{
  PROP_0,
  PROP_FILENAME,
  PROP_EFFECT,
};

static gpointer ags_live_audio_unit_bridge_parent_class = NULL;
static AgsConnectableInterface* ags_live_audio_unit_bridge_parent_connectable_interface;

extern GHashTable *ags_effect_bulk_indicator_queue_draw;

GType
ags_live_audio_unit_bridge_get_type(void)
{
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
    GType ags_type_live_audio_unit_bridge = 0;

    static const GTypeInfo ags_live_audio_unit_bridge_info = {
      sizeof(AgsLiveAudioUnitBridgeClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_live_audio_unit_bridge_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsLiveAudioUnitBridge),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_live_audio_unit_bridge_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_live_audio_unit_bridge_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_live_audio_unit_bridge = g_type_register_static(AGS_TYPE_MACHINE,
							     "AgsLiveAudioUnitBridge", &ags_live_audio_unit_bridge_info,
							     0);

    g_type_add_interface_static(ags_type_live_audio_unit_bridge,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__static, ags_type_live_audio_unit_bridge);
  }

  return(g_define_type_id__static);
}

void
ags_live_audio_unit_bridge_class_init(AgsLiveAudioUnitBridgeClass *live_audio_unit_bridge)
{
  AgsMachineClass *machine;
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_live_audio_unit_bridge_parent_class = g_type_class_peek_parent(live_audio_unit_bridge);

  /* GObjectClass */
  gobject = G_OBJECT_CLASS(live_audio_unit_bridge);

  gobject->set_property = ags_live_audio_unit_bridge_set_property;
  gobject->get_property = ags_live_audio_unit_bridge_get_property;

  gobject->finalize = ags_live_audio_unit_bridge_finalize;
  
  /* properties */
  /**
   * AgsRecallLiveAudioUnit:filename:
   *
   * The plugins filename.
   * 
   * Since: 3.10.5
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
   * AgsRecallLiveAudioUnit:effect:
   *
   * The effect's name.
   * 
   * Since: 3.10.5
   */
  param_spec =  g_param_spec_string("effect",
				    i18n_pspec("the effect"),
				    i18n_pspec("The effect's string representation"),
				    NULL,
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_EFFECT,
				  param_spec);

  /* AgsMachine */
  machine = (AgsMachineClass *) live_audio_unit_bridge;

  machine->map_recall = ags_live_audio_unit_bridge_map_recall;
}

void
ags_live_audio_unit_bridge_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_live_audio_unit_bridge_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_live_audio_unit_bridge_connect;
  connectable->disconnect = ags_live_audio_unit_bridge_disconnect;
}

void
ags_live_audio_unit_bridge_init(AgsLiveAudioUnitBridge *live_audio_unit_bridge)
{
  AgsWindow *window;
  AgsCompositeEditor *composite_editor;
  GtkBox *vbox;
  GtkBox *hbox;
  GtkLabel *label;
  GtkGrid *grid;
  GtkCellRenderer *cell_renderer;

  AgsAudio *audio;

  AgsMachineCounterManager *machine_counter_manager;
  AgsMachineCounter *machine_counter;

  GMenuItem *item;

  GSimpleActionGroup *action_group;
  GSimpleAction *action;
  
  AgsApplicationContext *application_context;
  
  gchar *machine_name;

  gint position;

  application_context = ags_application_context_get_instance();
  
  action_group = g_simple_action_group_new();
  gtk_widget_insert_action_group((GtkWidget *) live_audio_unit_bridge,
				 "live_audio_unit_bridge",
				 G_ACTION_GROUP(action_group));

  /* add index */
  action = g_simple_action_new("show_audio_unit_ui",
			       NULL);
  g_signal_connect(action, "activate",
		   G_CALLBACK(ags_live_audio_unit_bridge_show_audio_unit_ui_callback), live_audio_unit_bridge);
  g_action_map_add_action(G_ACTION_MAP(action_group),
			  G_ACTION(action));

  /* machine counter */
  machine_counter_manager = ags_machine_counter_manager_get_instance();

  machine_counter = ags_machine_counter_manager_find_machine_counter(machine_counter_manager,
								     AGS_TYPE_LIVE_AUDIO_UNIT_BRIDGE);

  machine_name = NULL;

  if(machine_counter != NULL){
    machine_name = g_strdup_printf("Default %d",
				   machine_counter->counter);
  
    ags_machine_counter_increment(machine_counter);
  }
  
  g_object_set(live_audio_unit_bridge,
	       "machine-name", machine_name,
	       NULL);

  g_free(machine_name);

  /* machine selector */
  window = (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));

  composite_editor = (AgsCompositeEditor *) ags_ui_provider_get_composite_editor(AGS_UI_PROVIDER(application_context));

  position = g_list_length(window->machine);
  
  ags_machine_selector_popup_insert_machine(composite_editor->machine_selector,
					    position,
					    (AgsMachine *) live_audio_unit_bridge);

  audio = AGS_MACHINE(live_audio_unit_bridge)->audio;
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
  
  AGS_MACHINE(live_audio_unit_bridge)->flags |= (AGS_MACHINE_IS_SYNTHESIZER |
						 AGS_MACHINE_REVERSE_NOTATION);

  g_signal_connect_after(G_OBJECT(live_audio_unit_bridge), "resize-audio-channels",
			 G_CALLBACK(ags_live_audio_unit_bridge_resize_audio_channels), NULL);

  g_signal_connect_after(G_OBJECT(live_audio_unit_bridge), "resize-pads",
			 G_CALLBACK(ags_live_audio_unit_bridge_resize_pads), NULL);
    
  live_audio_unit_bridge->flags = 0;

  live_audio_unit_bridge->name = NULL;

  live_audio_unit_bridge->version = AGS_LIVE_AUDIO_UNIT_BRIDGE_DEFAULT_VERSION;
  live_audio_unit_bridge->build_id = AGS_LIVE_AUDIO_UNIT_BRIDGE_DEFAULT_BUILD_ID;

  live_audio_unit_bridge->xml_type = "ags-live-audio-unit-bridge";
  
  live_audio_unit_bridge->mapped_output_pad = 0;
  live_audio_unit_bridge->mapped_input_pad = 0;

  live_audio_unit_bridge->audio_unit_play_container = ags_recall_container_new();
  live_audio_unit_bridge->audio_unit_recall_container = ags_recall_container_new();

  live_audio_unit_bridge->envelope_play_container = ags_recall_container_new();
  live_audio_unit_bridge->envelope_recall_container = ags_recall_container_new();

  live_audio_unit_bridge->buffer_play_container = ags_recall_container_new();
  live_audio_unit_bridge->buffer_recall_container = ags_recall_container_new();

  live_audio_unit_bridge->filename = NULL;
  live_audio_unit_bridge->effect = NULL;
 
  vbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
				AGS_UI_PROVIDER_DEFAULT_SPACING);
  
  gtk_widget_set_valign((GtkWidget *) vbox,
			GTK_ALIGN_START);
  gtk_widget_set_halign((GtkWidget *) vbox,
			GTK_ALIGN_START);

  gtk_widget_set_vexpand((GtkWidget *) vbox,
			 FALSE);
  gtk_widget_set_hexpand((GtkWidget *) vbox,
			 FALSE);

  gtk_frame_set_child(AGS_MACHINE(live_audio_unit_bridge)->frame,
		      (GtkWidget *) vbox);
  
  /* effect bridge */
  AGS_MACHINE(live_audio_unit_bridge)->bridge = (GtkGrid *) ags_effect_bridge_new(audio);

  AGS_EFFECT_BRIDGE(AGS_MACHINE(live_audio_unit_bridge)->bridge)->parent_machine = (GtkWidget *) live_audio_unit_bridge;

  gtk_widget_set_hexpand((GtkWidget *) AGS_MACHINE(live_audio_unit_bridge)->bridge,
			 FALSE);

  gtk_box_append(vbox,
		 (GtkWidget *) AGS_MACHINE(live_audio_unit_bridge)->bridge);
  
  AGS_EFFECT_BRIDGE(AGS_MACHINE(live_audio_unit_bridge)->bridge)->bulk_input = (GtkWidget *) ags_effect_bulk_new(audio,
														 AGS_TYPE_INPUT);
  ags_effect_bulk_set_flags(AGS_EFFECT_BULK(AGS_EFFECT_BRIDGE(AGS_MACHINE(live_audio_unit_bridge)->bridge)->bulk_input),
			    (AGS_EFFECT_BULK_HIDE_BUTTONS |
			     AGS_EFFECT_BULK_HIDE_ENTRIES |
			     AGS_EFFECT_BULK_SHOW_LABELS));

  gtk_widget_set_valign((GtkWidget *) AGS_EFFECT_BRIDGE(AGS_MACHINE(live_audio_unit_bridge)->bridge)->bulk_input,
			GTK_ALIGN_START);
  gtk_widget_set_halign((GtkWidget *) AGS_EFFECT_BRIDGE(AGS_MACHINE(live_audio_unit_bridge)->bridge)->bulk_input,
			GTK_ALIGN_START);
  
  AGS_EFFECT_BULK(AGS_EFFECT_BRIDGE(AGS_MACHINE(live_audio_unit_bridge)->bridge)->bulk_input)->parent_bridge = (GtkWidget *) AGS_MACHINE(live_audio_unit_bridge)->bridge;
  gtk_grid_attach(AGS_MACHINE(live_audio_unit_bridge)->bridge,
		  (GtkWidget *) AGS_EFFECT_BRIDGE(AGS_MACHINE(live_audio_unit_bridge)->bridge)->bulk_input,
		  0, 0,
		  1, 1);

  /* audio unit */
  live_audio_unit_bridge->audio_unit_plugin = NULL;
  live_audio_unit_bridge->audio_unit = NULL;
  
  /* audio unit menu */
  live_audio_unit_bridge->audio_unit_menu = (GMenu *) g_menu_new();
  g_menu_append_submenu(AGS_MACHINE(live_audio_unit_bridge)->context_menu,
			"Audio Unit",
			G_MENU_MODEL(live_audio_unit_bridge->audio_unit_menu));

  item = g_menu_item_new(i18n("show AUDIO_UNIT UI"),
			 "live_audio_unit_bridge.show_audio_unit_ui");
  g_menu_append_item(live_audio_unit_bridge->audio_unit_menu,
		     item);
  
  live_audio_unit_bridge->block_control = g_hash_table_new_full(g_direct_hash,
								g_direct_equal,
								NULL,
								NULL);
}

void
ags_live_audio_unit_bridge_set_property(GObject *gobject,
					guint prop_id,
					const GValue *value,
					GParamSpec *param_spec)
{
  AgsLiveAudioUnitBridge *live_audio_unit_bridge;

  live_audio_unit_bridge = AGS_LIVE_AUDIO_UNIT_BRIDGE(gobject);

  switch(prop_id){
  case PROP_FILENAME:
    {
      gchar *str;
      gchar *filename;

      filename = g_value_get_string(value);

      if(filename == live_audio_unit_bridge->filename){
	return;
      }

      if(live_audio_unit_bridge->filename != NULL){
	g_free(live_audio_unit_bridge->filename);
      }

      if(filename != NULL){
	if(!g_file_test(filename,
			G_FILE_TEST_EXISTS)){
	  AgsWindow *window;

	  window = (AgsWindow *) gtk_widget_get_ancestor((GtkWidget *) live_audio_unit_bridge,
							 AGS_TYPE_WINDOW);

	  str = g_strdup_printf("%s %s",
				i18n("Plugin file not present"),
				filename);
	  ags_window_show_error(window,
				str);

	  g_free(str);
	}
      }

      live_audio_unit_bridge->filename = g_strdup(filename);
    }
    break;
  case PROP_EFFECT:
    {
      gchar *effect;
      
      effect = g_value_get_string(value);

      if(effect == live_audio_unit_bridge->effect){
	return;
      }

      if(live_audio_unit_bridge->effect != NULL){
	g_free(live_audio_unit_bridge->effect);
      }

      live_audio_unit_bridge->effect = g_strdup(effect);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_live_audio_unit_bridge_get_property(GObject *gobject,
					guint prop_id,
					GValue *value,
					GParamSpec *param_spec)
{
  AgsLiveAudioUnitBridge *live_audio_unit_bridge;

  live_audio_unit_bridge = AGS_LIVE_AUDIO_UNIT_BRIDGE(gobject);

  switch(prop_id){
  case PROP_FILENAME:
    {
      g_value_set_string(value, live_audio_unit_bridge->filename);
    }
    break;
  case PROP_EFFECT:
    {
      g_value_set_string(value, live_audio_unit_bridge->effect);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_live_audio_unit_bridge_finalize(GObject *gobject)
{
  AgsLiveAudioUnitBridge *live_audio_unit_bridge;

  live_audio_unit_bridge = (AgsLiveAudioUnitBridge *) gobject;

  g_free(live_audio_unit_bridge->filename);
  g_free(live_audio_unit_bridge->effect);
  
  /* call parent */
  G_OBJECT_CLASS(ags_live_audio_unit_bridge_parent_class)->finalize(gobject);
}

void
ags_live_audio_unit_bridge_connect(AgsConnectable *connectable)
{
  AgsLiveAudioUnitBridge *live_audio_unit_bridge;
  AgsEffectBridge *effect_bridge;
  AgsBulkMember *bulk_member;
  GtkWidget *control;

  GList *start_list, *list;

  if(ags_connectable_is_connected(connectable)){
    return;
  }

  ags_live_audio_unit_bridge_parent_connectable_interface->connect(connectable);

  live_audio_unit_bridge = AGS_LIVE_AUDIO_UNIT_BRIDGE(connectable);

  /* bulk member */
  effect_bridge = AGS_EFFECT_BRIDGE(AGS_MACHINE(live_audio_unit_bridge)->bridge);
  
  list =
    start_list = ags_effect_bulk_get_bulk_member(AGS_EFFECT_BULK(effect_bridge->bulk_input));

  while(list != NULL){
    bulk_member = list->data;

    control = ags_bulk_member_get_widget(bulk_member);

    if(bulk_member->widget_type == AGS_TYPE_DIAL){
      g_signal_connect_after(GTK_WIDGET(control), "value-changed",
			     G_CALLBACK(ags_live_audio_unit_bridge_dial_changed_callback), live_audio_unit_bridge);
    }else if(bulk_member->widget_type == GTK_TYPE_SCALE){
      g_signal_connect_after(GTK_WIDGET(control), "value-changed",
			     G_CALLBACK(ags_live_audio_unit_bridge_scale_changed_callback), live_audio_unit_bridge);
    }else if(bulk_member->widget_type == GTK_TYPE_SPIN_BUTTON){
      g_signal_connect_after(GTK_WIDGET(control), "value-changed",
			     G_CALLBACK(ags_live_audio_unit_bridge_spin_button_changed_callback), live_audio_unit_bridge);
    }else if(bulk_member->widget_type == GTK_TYPE_CHECK_BUTTON){
      g_signal_connect_after(GTK_WIDGET(control), "clicked",
			     G_CALLBACK(ags_live_audio_unit_bridge_check_button_clicked_callback), live_audio_unit_bridge);
    }else if(bulk_member->widget_type == GTK_TYPE_TOGGLE_BUTTON){
      g_signal_connect_after(GTK_WIDGET(control), "clicked",
			     G_CALLBACK(ags_live_audio_unit_bridge_toggle_button_clicked_callback), live_audio_unit_bridge);
    }else if(bulk_member->widget_type == GTK_TYPE_CHECK_BUTTON){
      g_signal_connect_after(GTK_WIDGET(control), "clicked",
			     G_CALLBACK(ags_live_audio_unit_bridge_check_button_clicked_callback), live_audio_unit_bridge);
    }else if(bulk_member->widget_type == GTK_TYPE_BUTTON){
      g_signal_connect_after(GTK_WIDGET(control), "clicked",
			     G_CALLBACK(ags_live_audio_unit_bridge_button_clicked_callback), live_audio_unit_bridge);
    }

    list = list->next;
  }

  g_list_free(start_list);
}

void
ags_live_audio_unit_bridge_disconnect(AgsConnectable *connectable)
{
  AgsLiveAudioUnitBridge *live_audio_unit_bridge;
  AgsEffectBridge *effect_bridge;
  AgsBulkMember *bulk_member;
  GtkWidget *control;

  GList *start_list, *list;

  if(!ags_connectable_is_connected(connectable)){
    return;
  }

  ags_live_audio_unit_bridge_parent_connectable_interface->connect(connectable);

  live_audio_unit_bridge = AGS_LIVE_AUDIO_UNIT_BRIDGE(connectable);

  /* bulk member */
  effect_bridge = AGS_EFFECT_BRIDGE(AGS_MACHINE(live_audio_unit_bridge)->bridge);
  
  list =
    start_list = ags_effect_bulk_get_bulk_member(AGS_EFFECT_BULK(effect_bridge->bulk_input));

  while(list != NULL){
    bulk_member = list->data;

    control = ags_bulk_member_get_widget(bulk_member);

    if(bulk_member->widget_type == AGS_TYPE_DIAL){
      g_object_disconnect(GTK_WIDGET(control),
			  "any_signal::value-changed",
			  G_CALLBACK(ags_live_audio_unit_bridge_dial_changed_callback),
			  live_audio_unit_bridge,
			  NULL);
    }else if(bulk_member->widget_type == GTK_TYPE_SCALE){
      g_object_disconnect(GTK_WIDGET(control),
			  "any_signal::value-changed",
			  G_CALLBACK(ags_live_audio_unit_bridge_scale_changed_callback),
			  live_audio_unit_bridge,
			  NULL);
    }else if(bulk_member->widget_type == GTK_TYPE_SPIN_BUTTON){
      g_object_disconnect(GTK_WIDGET(control),
			  "any_signal::value-changed",
			  G_CALLBACK(ags_live_audio_unit_bridge_spin_button_changed_callback),
			  live_audio_unit_bridge,
			  NULL);
    }else if(bulk_member->widget_type == GTK_TYPE_CHECK_BUTTON){
      g_object_disconnect(GTK_WIDGET(control),
			  "any_signal::clicked",
			  G_CALLBACK(ags_live_audio_unit_bridge_check_button_clicked_callback),
			  live_audio_unit_bridge,
			  NULL);
    }else if(bulk_member->widget_type == GTK_TYPE_TOGGLE_BUTTON){
      g_object_disconnect(GTK_WIDGET(control),
			  "any_signal::clicked",
			  G_CALLBACK(ags_live_audio_unit_bridge_toggle_button_clicked_callback),
			  live_audio_unit_bridge,
			  NULL);
    }else if(bulk_member->widget_type == GTK_TYPE_BUTTON){
      g_object_disconnect(GTK_WIDGET(control),
			  "any_signal::clicked",
			  G_CALLBACK(ags_live_audio_unit_bridge_button_clicked_callback),
			  live_audio_unit_bridge,
			  NULL);
    }

    list = list->next;
  }

  g_list_free(start_list);
}

void
ags_live_audio_unit_bridge_resize_audio_channels(AgsMachine *machine,
						 guint audio_channels, guint audio_channels_old,
						 gpointer data)
{
  AgsLiveAudioUnitBridge *live_audio_unit_bridge;
  
  live_audio_unit_bridge = (AgsLiveAudioUnitBridge *) machine;

  if(audio_channels > audio_channels_old){
    /* recall */
    if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0){
      ags_live_audio_unit_bridge_input_map_recall(live_audio_unit_bridge,
						  audio_channels_old,
						  0);

      ags_live_audio_unit_bridge_output_map_recall(live_audio_unit_bridge,
						   audio_channels_old,
						   0);
    }
  }
}

void
ags_live_audio_unit_bridge_resize_pads(AgsMachine *machine, GType channel_type,
				       guint pads, guint pads_old,
				       gpointer data)
{
  AgsLiveAudioUnitBridge *live_audio_unit_bridge;

  gboolean grow;

  live_audio_unit_bridge = (AgsLiveAudioUnitBridge *) machine;

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
	ags_live_audio_unit_bridge_input_map_recall(live_audio_unit_bridge,
						    0,
						    pads_old);
      }
    }else{
      live_audio_unit_bridge->mapped_input_pad = pads;
    }
  }else{
    if(grow){
      /* recall */
      if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0){
	ags_live_audio_unit_bridge_output_map_recall(live_audio_unit_bridge,
						     0,
						     pads_old);
      }
    }else{
      live_audio_unit_bridge->mapped_output_pad = pads;
    }
  }
}

void
ags_live_audio_unit_bridge_map_recall(AgsMachine *machine)
{  
  AgsNavigation *navigation;
  AgsLiveAudioUnitBridge *live_audio_unit_bridge;
  
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

  live_audio_unit_bridge = (AgsLiveAudioUnitBridge *) machine;

  audio = machine->audio;

  position = 0;

  /* add new controls */
  ags_effect_bulk_add_plugin(AGS_EFFECT_BULK(AGS_EFFECT_BRIDGE(machine->bridge)->bulk_input),
			     NULL,
			     live_audio_unit_bridge->audio_unit_play_container, live_audio_unit_bridge->audio_unit_recall_container,
			     "ags-fx-audio-unit",
			     live_audio_unit_bridge->filename,
			     live_audio_unit_bridge->effect,
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
			gtk_check_button_get_active(navigation->loop));

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
				       live_audio_unit_bridge->envelope_play_container, live_audio_unit_bridge->envelope_recall_container,
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
				       live_audio_unit_bridge->buffer_play_container, live_audio_unit_bridge->buffer_recall_container,
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
  ags_live_audio_unit_bridge_input_map_recall(live_audio_unit_bridge,
					      0,
					      0);

  /* depending on destination */
  ags_live_audio_unit_bridge_output_map_recall(live_audio_unit_bridge,
					       0,
					       0);

  /* call parent */
  AGS_MACHINE_CLASS(ags_live_audio_unit_bridge_parent_class)->map_recall(machine);
}

void
ags_live_audio_unit_bridge_input_map_recall(AgsLiveAudioUnitBridge *live_audio_unit_bridge,
					    guint audio_channel_start,
					    guint input_pad_start)
{
  AgsAudio *audio;

  GList *start_recall;

  gint position;
  guint input_pads;
  guint audio_channels;

  if(live_audio_unit_bridge->mapped_input_pad > input_pad_start){
    return;
  }

  audio = AGS_MACHINE(live_audio_unit_bridge)->audio;

  position = 0;

  input_pads = 0;
  audio_channels = 0;

  /* get some fields */
  g_object_get(audio,
	       "input-pads", &input_pads,
	       "audio-channels", &audio_channels,
	       NULL);

  /* add to effect bridge */
  ags_effect_bulk_add_plugin((AgsEffectBulk *) AGS_EFFECT_BRIDGE(AGS_MACHINE(live_audio_unit_bridge)->bridge)->bulk_input,
			     NULL,
			     live_audio_unit_bridge->audio_unit_play_container, live_audio_unit_bridge->audio_unit_recall_container,
			     "ags-fx-audio-unit",
			     live_audio_unit_bridge->filename,
			     live_audio_unit_bridge->effect,
			     audio_channel_start, audio_channels,
			     input_pad_start, input_pads,
			     position,
			     (AGS_FX_FACTORY_LIVE | AGS_FX_FACTORY_REMAP | AGS_FX_FACTORY_INPUT), 0);

  /* source */
  if((AGS_MACHINE_IS_SYNTHESIZER & (AGS_MACHINE(live_audio_unit_bridge)->flags)) != 0){
    /* ags-fx-envelope */
    start_recall = ags_fx_factory_create(audio,
					 live_audio_unit_bridge->envelope_play_container, live_audio_unit_bridge->envelope_recall_container,
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
					 live_audio_unit_bridge->buffer_play_container, live_audio_unit_bridge->buffer_recall_container,
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

  live_audio_unit_bridge->mapped_input_pad = input_pads;
}

void
ags_live_audio_unit_bridge_output_map_recall(AgsLiveAudioUnitBridge *live_audio_unit_bridge,
					     guint audio_channel_start,
					     guint output_pad_start)
{
  AgsAudio *audio;

  guint output_pads;
  
  if(live_audio_unit_bridge->mapped_output_pad > output_pad_start){
    return;
  }

  audio = AGS_MACHINE(live_audio_unit_bridge)->audio;

  output_pads = 0;

  /* get some fields */
  g_object_get(audio,
	       "output-pads", &output_pads,
	       NULL);

  live_audio_unit_bridge->mapped_output_pad = output_pads;
}

void
ags_live_audio_unit_bridge_load(AgsLiveAudioUnitBridge *live_audio_unit_bridge)
{
  AgsAudioUnitPlugin *audio_unit_plugin;

  if(!AGS_IS_LIVE_AUDIO_UNIT_BRIDGE(live_audio_unit_bridge)){
    return;
  }
  
  /* load plugin */
  audio_unit_plugin = ags_audio_unit_manager_find_audio_unit_plugin(ags_audio_unit_manager_get_instance(),
								    live_audio_unit_bridge->filename,
								    live_audio_unit_bridge->effect);

  if(audio_unit_plugin == NULL){
    return;
  }

  //TODO:JK: implement me
}

void
ags_live_audio_unit_bridge_reload_port(AgsLiveAudioUnitBridge *live_audio_unit_bridge)
{
  AgsEffectBridge *effect_bridge;

  AgsAudioUnitPlugin *audio_unit_plugin;

  GRecMutex *base_plugin_mutex;

  if(!AGS_IS_LIVE_AUDIO_UNIT_BRIDGE(live_audio_unit_bridge)){
    return;
  }

  audio_unit_plugin = live_audio_unit_bridge->audio_unit_plugin;

  if(audio_unit_plugin == NULL){
    return;
  }
  
  base_plugin_mutex = AGS_BASE_PLUGIN_GET_OBJ_MUTEX(audio_unit_plugin);

  effect_bridge = AGS_EFFECT_BRIDGE(AGS_MACHINE(live_audio_unit_bridge)->bridge);

  //TODO:JK: implement me
}

/**
 * ags_live_audio_unit_bridge_new:
 * @soundcard: the assigned soundcard.
 * @filename: the plugin.so
 * @effect: the effect
 *
 * Create a new instance of #AgsLiveAudioUnitBridge
 *
 * Returns: the new #AgsLiveAudioUnitBridge
 *
 * Since: 3.10.5
 */
AgsLiveAudioUnitBridge*
ags_live_audio_unit_bridge_new(GObject *soundcard,
			       gchar *filename,
			       gchar *effect)
{
  AgsLiveAudioUnitBridge *live_audio_unit_bridge;

  live_audio_unit_bridge = (AgsLiveAudioUnitBridge *) g_object_new(AGS_TYPE_LIVE_AUDIO_UNIT_BRIDGE,
								   NULL);

  if(soundcard != NULL){
    g_object_set(G_OBJECT(AGS_MACHINE(live_audio_unit_bridge)->audio),
		 "output-soundcard", soundcard,
		 NULL);
  }

  g_object_set(live_audio_unit_bridge,
	       "filename", filename,
	       "effect", effect,
	       NULL);

  return(live_audio_unit_bridge);
}
