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

#include <ags/app/machine/ags_audio_unit_bridge.h>
#include <ags/app/machine/ags_audio_unit_bridge_callbacks.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_window.h>
#include <ags/app/ags_composite_editor.h>
#include <ags/app/ags_navigation.h>
#include <ags/app/ags_machine_callbacks.h>
#include <ags/app/ags_effect_bridge.h>
#include <ags/app/ags_effect_bulk.h>
#include <ags/app/ags_bulk_member.h>

#include <ags/i18n.h>

void ags_audio_unit_bridge_class_init(AgsAudioUnitBridgeClass *audio_unit_bridge);
void ags_audio_unit_bridge_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_audio_unit_bridge_init(AgsAudioUnitBridge *audio_unit_bridge);
void ags_audio_unit_bridge_set_property(GObject *gobject,
					guint prop_id,
					const GValue *value,
					GParamSpec *param_spec);
void ags_audio_unit_bridge_get_property(GObject *gobject,
					guint prop_id,
					GValue *value,
					GParamSpec *param_spec);
void ags_audio_unit_bridge_finalize(GObject *gobject);

void ags_audio_unit_bridge_connect(AgsConnectable *connectable);
void ags_audio_unit_bridge_disconnect(AgsConnectable *connectable);

void ags_audio_unit_bridge_resize_audio_channels(AgsMachine *machine,
						 guint audio_channels, guint audio_channels_old,
						 gpointer data);
void ags_audio_unit_bridge_resize_pads(AgsMachine *machine, GType type,
				       guint pads, guint pads_old,
				       gpointer data);

void ags_audio_unit_bridge_map_recall(AgsMachine *machine);

/**
 * SECTION:ags_audio_unit_bridge
 * @short_description: A composite widget to visualize a bunch of #AgsChannel
 * @title: AgsAudioUnitBridge
 * @section_id:
 * @include: ags/app/ags_audio_unit_bridge.h
 *
 * #AgsAudioUnitBridge is a composite widget to visualize all #AgsChannel. It should be
 * packed by an #AgsMachine.
 */

enum{
  PROP_0,
  PROP_FILENAME,
  PROP_EFFECT,
};

static gpointer ags_audio_unit_bridge_parent_class = NULL;
static AgsConnectableInterface* ags_audio_unit_bridge_parent_connectable_interface;

GType
ags_audio_unit_bridge_get_type(void)
{
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
    GType ags_type_audio_unit_bridge = 0;

    static const GTypeInfo ags_audio_unit_bridge_info = {
      sizeof(AgsAudioUnitBridgeClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_audio_unit_bridge_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsAudioUnitBridge),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_audio_unit_bridge_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_audio_unit_bridge_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_audio_unit_bridge = g_type_register_static(AGS_TYPE_MACHINE,
							"AgsAudioUnitBridge", &ags_audio_unit_bridge_info,
							0);

    g_type_add_interface_static(ags_type_audio_unit_bridge,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__static, ags_type_audio_unit_bridge);
  }

  return(g_define_type_id__static);
}

void
ags_audio_unit_bridge_class_init(AgsAudioUnitBridgeClass *audio_unit_bridge)
{
  AgsMachineClass *machine;
  GObjectClass *gobject;

  GParamSpec *param_spec;

  ags_audio_unit_bridge_parent_class = g_type_class_peek_parent(audio_unit_bridge);

  /* GObjectClass */
  gobject = G_OBJECT_CLASS(audio_unit_bridge);

  gobject->set_property = ags_audio_unit_bridge_set_property;
  gobject->get_property = ags_audio_unit_bridge_get_property;

  gobject->finalize = ags_audio_unit_bridge_finalize;
  
  /* properties */
  /**
   * AgsRecallAudioUnit:filename:
   *
   * The plugins filename.
   * 
   * Since: 8.1.2
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
   * AgsRecallAudioUnit:effect:
   *
   * The effect's name.
   * 
   * Since: 8.1.2
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
  machine = (AgsMachineClass *) audio_unit_bridge;

  machine->map_recall = ags_audio_unit_bridge_map_recall;
}

void
ags_audio_unit_bridge_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_audio_unit_bridge_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_audio_unit_bridge_connect;
  connectable->disconnect = ags_audio_unit_bridge_disconnect;
}

void
ags_audio_unit_bridge_init(AgsAudioUnitBridge *audio_unit_bridge)
{
  AgsWindow *window;
  AgsCompositeEditor *composite_editor;
  GtkBox *vbox;
  GtkBox *hbox;
  GtkLabel *label;
  GtkGrid *grid;

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
  gtk_widget_insert_action_group((GtkWidget *) audio_unit_bridge,
				 "audio_unit_bridge",
				 G_ACTION_GROUP(action_group));

  /* show AUDIO_UNIT UI */
  action = g_simple_action_new("show_audio_unit_ui",
			       NULL);
  g_signal_connect(action, "activate",
		   G_CALLBACK(ags_audio_unit_bridge_show_audio_unit_ui_callback), audio_unit_bridge);
  g_action_map_add_action(G_ACTION_MAP(action_group),
			  G_ACTION(action));

  /* machine counter */
  machine_counter_manager = ags_machine_counter_manager_get_instance();

  machine_counter = ags_machine_counter_manager_find_machine_counter(machine_counter_manager,
								     AGS_TYPE_AUDIO_UNIT_BRIDGE);

  machine_name = NULL;

  if(machine_counter != NULL){
    machine_name = g_strdup_printf("Default %d",
				   machine_counter->counter);
  
    ags_machine_counter_increment(machine_counter);
  }
  
  g_object_set(audio_unit_bridge,
	       "machine-name", machine_name,
	       NULL);

  g_free(machine_name);

  /* machine selector */
  window = (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));

  composite_editor = (AgsCompositeEditor *) ags_ui_provider_get_composite_editor(AGS_UI_PROVIDER(application_context));

  position = g_list_length(window->machine);
  
  ags_machine_selector_popup_insert_machine(composite_editor->machine_selector,
					    position,
					    (AgsMachine *) audio_unit_bridge);
  
  audio = AGS_MACHINE(audio_unit_bridge)->audio;
  ags_audio_set_flags(audio, (AGS_AUDIO_SYNC));
  g_object_set(audio,
	       "min-audio-channels", 1,
	       "min-output-pads", 1,
	       "min-input-pads", 1,
	       NULL);

  g_signal_connect_after(G_OBJECT(audio_unit_bridge), "resize-audio-channels",
			 G_CALLBACK(ags_audio_unit_bridge_resize_audio_channels), NULL);

  g_signal_connect_after(G_OBJECT(audio_unit_bridge), "resize-pads",
			 G_CALLBACK(ags_audio_unit_bridge_resize_pads), NULL);
    
  audio_unit_bridge->flags = 0;

  audio_unit_bridge->name = NULL;

  audio_unit_bridge->version = AGS_AUDIO_UNIT_BRIDGE_DEFAULT_VERSION;
  audio_unit_bridge->build_id = AGS_AUDIO_UNIT_BRIDGE_DEFAULT_BUILD_ID;

  audio_unit_bridge->xml_type = "ags-audio-unit-bridge";
  
  audio_unit_bridge->mapped_output_pad = 0;
  audio_unit_bridge->mapped_input_pad = 0;

  audio_unit_bridge->audio_unit_play_container = ags_recall_container_new();
  audio_unit_bridge->audio_unit_recall_container = ags_recall_container_new();

  audio_unit_bridge->envelope_play_container = ags_recall_container_new();
  audio_unit_bridge->envelope_recall_container = ags_recall_container_new();

  audio_unit_bridge->buffer_play_container = ags_recall_container_new();
  audio_unit_bridge->buffer_recall_container = ags_recall_container_new();
				     
  audio_unit_bridge->filename = NULL;
  audio_unit_bridge->effect = NULL;
  
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

  gtk_frame_set_child(AGS_MACHINE(audio_unit_bridge)->frame,
		      (GtkWidget *) vbox);  

  /* effect bridge */
  AGS_MACHINE(audio_unit_bridge)->bridge = (GtkGrid *) ags_effect_bridge_new(audio);

  AGS_EFFECT_BRIDGE(AGS_MACHINE(audio_unit_bridge)->bridge)->parent_machine = (GtkWidget *) audio_unit_bridge;

  gtk_widget_set_hexpand((GtkWidget *) AGS_MACHINE(audio_unit_bridge)->bridge,
			 FALSE);

  gtk_box_append(vbox,
		 (GtkWidget *) AGS_MACHINE(audio_unit_bridge)->bridge);  

  AGS_EFFECT_BRIDGE(AGS_MACHINE(audio_unit_bridge)->bridge)->bulk_input = (GtkWidget *) ags_effect_bulk_new(audio,
													    AGS_TYPE_INPUT);
  ags_effect_bulk_set_flags(AGS_EFFECT_BULK(AGS_EFFECT_BRIDGE(AGS_MACHINE(audio_unit_bridge)->bridge)->bulk_input),
			    (AGS_EFFECT_BULK_HIDE_BUTTONS |
			     AGS_EFFECT_BULK_HIDE_ENTRIES |
			     AGS_EFFECT_BULK_SHOW_LABELS));

  gtk_widget_set_valign(AGS_EFFECT_BRIDGE(AGS_MACHINE(audio_unit_bridge)->bridge)->bulk_input,
			GTK_ALIGN_START);
  gtk_widget_set_halign(AGS_EFFECT_BRIDGE(AGS_MACHINE(audio_unit_bridge)->bridge)->bulk_input,
			GTK_ALIGN_START);
  
  AGS_EFFECT_BULK(AGS_EFFECT_BRIDGE(AGS_MACHINE(audio_unit_bridge)->bridge)->bulk_input)->parent_bridge = (GtkWidget *) AGS_MACHINE(audio_unit_bridge)->bridge;
  gtk_grid_attach(AGS_MACHINE(audio_unit_bridge)->bridge,
		  AGS_EFFECT_BRIDGE(AGS_MACHINE(audio_unit_bridge)->bridge)->bulk_input,
		  0, 0,
		  1, 1);

  /* Audio Unit */
  audio_unit_bridge->audio_unit_plugin = NULL;
  audio_unit_bridge->audio_unit = NULL;

  /* audio unit menu */
  audio_unit_bridge->audio_unit_menu = (GMenu *) g_menu_new();
  g_menu_append_submenu(AGS_MACHINE(audio_unit_bridge)->context_menu,
			"Audio Unit",
			G_MENU_MODEL(audio_unit_bridge->audio_unit_menu));

  item = g_menu_item_new(i18n("show Audio Unit UI"),
			 "audio_unit_bridge.show_audio_unit_ui");
  g_menu_append_item(audio_unit_bridge->audio_unit_menu,
		     item);
  
  audio_unit_bridge->block_control = g_hash_table_new_full(g_direct_hash,
							   g_direct_equal,
							   NULL,
							   NULL);
}

void
ags_audio_unit_bridge_set_property(GObject *gobject,
				   guint prop_id,
				   const GValue *value,
				   GParamSpec *param_spec)
{
  AgsAudioUnitBridge *audio_unit_bridge;

  audio_unit_bridge = AGS_AUDIO_UNIT_BRIDGE(gobject);

  switch(prop_id){
  case PROP_FILENAME:
    {
      gchar *str;
      gchar *filename;

      filename = g_value_get_string(value);

      if(filename == audio_unit_bridge->filename){
	return;
      }

      if(audio_unit_bridge->filename != NULL){
	g_free(audio_unit_bridge->filename);
      }

      if(filename != NULL){
	if(!g_file_test(filename,
			G_FILE_TEST_EXISTS)){
	  AgsWindow *window;

	  window = (AgsWindow *) gtk_widget_get_ancestor((GtkWidget *) audio_unit_bridge,
							 AGS_TYPE_WINDOW);

	  str = g_strdup_printf("%s %s",
				i18n("Plugin file not present"),
				filename);
	  ags_window_show_error(window,
				str);

	  g_free(str);
	}
      }

      audio_unit_bridge->filename = g_strdup(filename);
    }
    break;
  case PROP_EFFECT:
    {
      gchar *effect;
      
      effect = g_value_get_string(value);

      if(effect == audio_unit_bridge->effect){
	return;
      }

      if(audio_unit_bridge->effect != NULL){
	g_free(audio_unit_bridge->effect);
      }

      audio_unit_bridge->effect = g_strdup(effect);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_audio_unit_bridge_get_property(GObject *gobject,
				   guint prop_id,
				   GValue *value,
				   GParamSpec *param_spec)
{
  AgsAudioUnitBridge *audio_unit_bridge;

  audio_unit_bridge = AGS_AUDIO_UNIT_BRIDGE(gobject);

  switch(prop_id){
  case PROP_FILENAME:
    {
      g_value_set_string(value, audio_unit_bridge->filename);
    }
    break;
  case PROP_EFFECT:
    {
      g_value_set_string(value, audio_unit_bridge->effect);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_audio_unit_bridge_finalize(GObject *gobject)
{
  AgsAudioUnitBridge *audio_unit_bridge;

  audio_unit_bridge = (AgsAudioUnitBridge *) gobject;

  g_free(audio_unit_bridge->filename);
  g_free(audio_unit_bridge->effect);
  
  /* call parent */
  G_OBJECT_CLASS(ags_audio_unit_bridge_parent_class)->finalize(gobject);
}

void
ags_audio_unit_bridge_connect(AgsConnectable *connectable)
{
  AgsAudioUnitBridge *audio_unit_bridge;
  AgsEffectBridge *effect_bridge;
  AgsBulkMember *bulk_member;
  GtkWidget *control;

  GList *start_list, *list;

  if(ags_connectable_is_connected(connectable)){
    return;
  }

  //  ags_audio_unit_bridge_parent_connectable_interface->connect(connectable);
  audio_unit_bridge = AGS_AUDIO_UNIT_BRIDGE(connectable);

  AGS_MACHINE(audio_unit_bridge)->connectable_flags |= AGS_CONNECTABLE_CONNECTED;

  g_signal_connect_after(G_OBJECT(audio_unit_bridge), "map-recall",
			 G_CALLBACK(ags_machine_map_recall_callback), NULL);

  ags_machine_map_recall(audio_unit_bridge);

#ifdef AGS_DEBUG
  g_message("find port");
#endif
      
  ags_machine_find_port((AgsMachine *) audio_unit_bridge);

  if(AGS_MACHINE(audio_unit_bridge)->bridge != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(AGS_MACHINE(audio_unit_bridge)->bridge));
  }

  /* bulk member */
  effect_bridge = AGS_EFFECT_BRIDGE(AGS_MACHINE(audio_unit_bridge)->bridge);

  list =
    start_list = ags_effect_bulk_get_bulk_member(AGS_EFFECT_BULK(effect_bridge->bulk_input));

  while(list != NULL){
    bulk_member = list->data;

    control = ags_bulk_member_get_widget(bulk_member);

    if(bulk_member->widget_type == AGS_TYPE_DIAL){
      g_signal_connect_after(GTK_WIDGET(control), "value-changed",
			     G_CALLBACK(ags_audio_unit_bridge_dial_changed_callback), audio_unit_bridge);
    }else if(bulk_member->widget_type == GTK_TYPE_SCALE){
      g_signal_connect_after(GTK_WIDGET(control), "value-changed",
			     G_CALLBACK(ags_audio_unit_bridge_scale_changed_callback), audio_unit_bridge);
    }else if(bulk_member->widget_type == GTK_TYPE_SPIN_BUTTON){
      g_signal_connect_after(GTK_WIDGET(control), "value-changed",
			     G_CALLBACK(ags_audio_unit_bridge_spin_button_changed_callback), audio_unit_bridge);
    }else if(bulk_member->widget_type == GTK_TYPE_CHECK_BUTTON){
      g_signal_connect_after(GTK_WIDGET(control), "clicked",
			     G_CALLBACK(ags_audio_unit_bridge_check_button_clicked_callback), audio_unit_bridge);
    }else if(bulk_member->widget_type == GTK_TYPE_TOGGLE_BUTTON){
      g_signal_connect_after(GTK_WIDGET(control), "clicked",
			     G_CALLBACK(ags_audio_unit_bridge_toggle_button_clicked_callback), audio_unit_bridge);
    }else if(bulk_member->widget_type == GTK_TYPE_BUTTON){
      g_signal_connect_after(GTK_WIDGET(control), "clicked",
			     G_CALLBACK(ags_audio_unit_bridge_button_clicked_callback), audio_unit_bridge);
    }

    list = list->next;
  }

  g_list_free(start_list);
}

void
ags_audio_unit_bridge_disconnect(AgsConnectable *connectable)
{
  AgsAudioUnitBridge *audio_unit_bridge;
  AgsEffectBridge *effect_bridge;
  AgsBulkMember *bulk_member;
  GtkWidget *control;

  GList *start_list, *list;

  if(!ags_connectable_is_connected(connectable)){
    return;
  }

  ags_audio_unit_bridge_parent_connectable_interface->connect(connectable);

  audio_unit_bridge = AGS_AUDIO_UNIT_BRIDGE(connectable);

  /* bulk member */
  effect_bridge = AGS_EFFECT_BRIDGE(AGS_MACHINE(audio_unit_bridge)->bridge);
  
  list =
    start_list = ags_effect_bulk_get_bulk_member(AGS_EFFECT_BULK(effect_bridge->bulk_input));

  while(list != NULL){
    bulk_member = list->data;

    control = ags_bulk_member_get_widget(bulk_member);

    if(bulk_member->widget_type == AGS_TYPE_DIAL){
      g_object_disconnect(GTK_WIDGET(control),
			  "any_signal::value-changed",
			  G_CALLBACK(ags_audio_unit_bridge_dial_changed_callback),
			  audio_unit_bridge,
			  NULL);
    }else if(bulk_member->widget_type == GTK_TYPE_SCALE){
      g_object_disconnect(GTK_WIDGET(control),
			  "any_signal::value-changed",
			  G_CALLBACK(ags_audio_unit_bridge_scale_changed_callback),
			  audio_unit_bridge,
			  NULL);
    }else if(bulk_member->widget_type == GTK_TYPE_SPIN_BUTTON){
      g_object_disconnect(GTK_WIDGET(control),
			  "any_signal::value-changed",
			  G_CALLBACK(ags_audio_unit_bridge_spin_button_changed_callback),
			  audio_unit_bridge,
			  NULL);
    }else if(bulk_member->widget_type == GTK_TYPE_CHECK_BUTTON){
      g_object_disconnect(GTK_WIDGET(control),
			  "any_signal::toggled",
			  G_CALLBACK(ags_audio_unit_bridge_check_button_clicked_callback),
			  audio_unit_bridge,
			  NULL);
    }else if(bulk_member->widget_type == GTK_TYPE_TOGGLE_BUTTON){
      g_object_disconnect(GTK_WIDGET(control),
			  "any_signal::toggled",
			  G_CALLBACK(ags_audio_unit_bridge_toggle_button_clicked_callback),
			  audio_unit_bridge,
			  NULL);
    }else if(bulk_member->widget_type == GTK_TYPE_BUTTON){
      g_object_disconnect(GTK_WIDGET(control),
			  "any_signal::clicked",
			  G_CALLBACK(ags_audio_unit_bridge_button_clicked_callback),
			  audio_unit_bridge,
			  NULL);
    }

    list = list->next;
  }

  g_list_free(start_list);
}

void
ags_audio_unit_bridge_resize_audio_channels(AgsMachine *machine,
					    guint audio_channels, guint audio_channels_old,
					    gpointer data)
{
  AgsAudioUnitBridge *audio_unit_bridge;

  audio_unit_bridge = (AgsAudioUnitBridge *) machine;  
  
  if(audio_channels > audio_channels_old){
    /* recall */
    if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0){
      ags_audio_unit_bridge_input_map_recall(audio_unit_bridge,
					     audio_channels_old,
					     0);

      ags_audio_unit_bridge_output_map_recall(audio_unit_bridge,
					      audio_channels_old,
					      0);
    }
  }
}

void
ags_audio_unit_bridge_resize_pads(AgsMachine *machine, GType type,
				  guint pads, guint pads_old,
				  gpointer data)
{
  AgsAudioUnitBridge *audio_unit_bridge;
  gboolean grow;

  audio_unit_bridge = (AgsAudioUnitBridge *) machine;

  if(pads_old < pads){
    grow = TRUE;
  }else{
    grow = FALSE;
  }
  
  if(g_type_is_a(type, AGS_TYPE_INPUT)){
    if(grow){
      /* recall */
      if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0){
	ags_audio_unit_bridge_input_map_recall(audio_unit_bridge,
					       0,
					       pads_old);
      }
    }else{
      audio_unit_bridge->mapped_input_pad = pads;
    }
  }else{
    if(grow){
      /* recall */
      if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0){
	ags_audio_unit_bridge_output_map_recall(audio_unit_bridge,
						0,
						pads_old);
      }
    }else{
      audio_unit_bridge->mapped_output_pad = pads;
    }
  }
}

void
ags_audio_unit_bridge_map_recall(AgsMachine *machine)
{
  AgsNavigation *navigation;
  AgsAudioUnitBridge *audio_unit_bridge;
  
  AgsAudio *audio;
  AgsAudioUnitPlugin *audio_unit_plugin;      

  AgsApplicationContext *application_context;

  GList *start_play, *start_recall;
  GList *start_list, *list;
  
  gint position;
  guint audio_channels;
  guint input_pads, output_pads;

  if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0 ||
     (AGS_MACHINE_PREMAPPED_RECALL & (machine->flags)) != 0){
    return;
  }

  application_context = ags_application_context_get_instance();

  navigation = (AgsNavigation *) ags_ui_provider_get_navigation(AGS_UI_PROVIDER(application_context));

  audio_unit_bridge = (AgsAudioUnitBridge *) machine;
  
  audio = machine->audio;
  
  position = 0;

  audio_channels = 2;

  input_pads = 1;
  output_pads = 1;

  g_object_get(audio,
	       "audio-channels", &audio_channels,
	       "input-pads", &input_pads,
	       "output-pads", &output_pads,
	       NULL);

  /* add to effect bridge */
  ags_effect_bulk_add_plugin((AgsEffectBulk *) AGS_EFFECT_BRIDGE(machine->bridge)->bulk_input,
			     NULL,
			     audio_unit_bridge->audio_unit_play_container, audio_unit_bridge->audio_unit_recall_container,
			     "ags-fx-audio-unit",
			     audio_unit_bridge->filename,
			     audio_unit_bridge->effect,
			     0, audio_channels,
			     0, input_pads,
			     position,
			     (AGS_FX_FACTORY_ADD | AGS_FX_FACTORY_INPUT), 0);
  
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

    ags_fx_audio_unit_audio_load_port((AgsFxAudioUnitAudio *) list->data);
    
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
				       audio_unit_bridge->envelope_play_container, audio_unit_bridge->envelope_recall_container,
				       "ags-fx-envelope",
				       NULL,
				       NULL,
				       0, audio_channels,
				       0, input_pads,
				       position,
				       (AGS_FX_FACTORY_ADD | AGS_FX_FACTORY_INPUT), 0);

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);

  /* ags-fx-buffer */
  start_recall = ags_fx_factory_create(audio,
				       audio_unit_bridge->buffer_play_container, audio_unit_bridge->buffer_recall_container,
				       "ags-fx-buffer",
				       NULL,
				       NULL,
				       0, audio_channels,
				       0, input_pads,
				       position,
				       (AGS_FX_FACTORY_ADD | AGS_FX_FACTORY_INPUT), 0);

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);

  /* depending on destination */
  //  ags_audio_unit_bridge_input_map_recall(audio_unit_bridge,
  //					 0,
  //					 0);

  /* depending on destination */
  //  ags_audio_unit_bridge_output_map_recall(audio_unit_bridge,
  //					  0,
  //					  0);

  audio_unit_bridge->mapped_input_pad = input_pads;
  audio_unit_bridge->mapped_output_pad = output_pads;
  
  /* call parent */
  AGS_MACHINE_CLASS(ags_audio_unit_bridge_parent_class)->map_recall(machine);
}

void
ags_audio_unit_bridge_input_map_recall(AgsAudioUnitBridge *audio_unit_bridge,
				       guint audio_channel_start,
				       guint input_pad_start)
{
  AgsAudio *audio;

  GList *start_recall;

  gint position;
  guint input_pads;
  guint audio_channels;

  if(audio_unit_bridge->mapped_input_pad > input_pad_start){
    return;
  }

  audio = AGS_MACHINE(audio_unit_bridge)->audio;

  position = 0;

  input_pads = 0;
  audio_channels = 0;
  
  /* get some fields */
  g_object_get(audio,
	       "input-pads", &input_pads,
	       "audio-channels", &audio_channels,
	       NULL);

  if(audio_channel_start == audio_channels &&
     input_pad_start == input_pads){
    return;
  }
  
  /* add to effect bridge */
  ags_effect_bulk_add_plugin((AgsEffectBulk *) AGS_EFFECT_BRIDGE(AGS_MACHINE(audio_unit_bridge)->bridge)->bulk_input,
			     NULL,
			     audio_unit_bridge->audio_unit_play_container, audio_unit_bridge->audio_unit_recall_container,
			     "ags-fx-audio-unit",
			     audio_unit_bridge->filename,
			     audio_unit_bridge->effect,
			     audio_channel_start, audio_channels,
			     input_pad_start, input_pads,
			     position,
			     (AGS_FX_FACTORY_REMAP | AGS_FX_FACTORY_INPUT), 0);

  /* ags-fx-envelope */
  start_recall = ags_fx_factory_create(audio,
				       audio_unit_bridge->envelope_play_container, audio_unit_bridge->envelope_recall_container,
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
				       audio_unit_bridge->buffer_play_container, audio_unit_bridge->buffer_recall_container,
				       "ags-fx-buffer",
				       NULL,
				       NULL,
				       audio_channel_start, audio_channels,
				       input_pad_start, input_pads,
				       position,
				       (AGS_FX_FACTORY_REMAP | AGS_FX_FACTORY_INPUT), 0);

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);

  audio_unit_bridge->mapped_input_pad = input_pads;
}

void
ags_audio_unit_bridge_output_map_recall(AgsAudioUnitBridge *audio_unit_bridge,
					guint audio_channel_start,
					guint output_pad_start)
{
  AgsAudio *audio;

  guint output_pads;

  if(audio_unit_bridge->mapped_output_pad > output_pad_start){
    return;
  }

  audio = AGS_MACHINE(audio_unit_bridge)->audio;

  /* get some fields */
  g_object_get(audio,
	       "output-pads", &output_pads,
	       NULL);
  
  audio_unit_bridge->mapped_output_pad = output_pads;
}

void
ags_audio_unit_bridge_load(AgsAudioUnitBridge *audio_unit_bridge)
{
  AgsAudioUnitPlugin *audio_unit_plugin;

  if(!AGS_IS_AUDIO_UNIT_BRIDGE(audio_unit_bridge)){
    return;
  }
  
  /* load plugin */
  audio_unit_plugin = ags_audio_unit_manager_find_audio_unit_plugin(ags_audio_unit_manager_get_instance(),
								    audio_unit_bridge->filename,
								    audio_unit_bridge->effect);

  if(audio_unit_plugin == NULL){
    return;
  }

  //TODO:JK: implement me
}

void
ags_audio_unit_bridge_reload_port(AgsAudioUnitBridge *audio_unit_bridge)
{
  AgsEffectBridge *effect_bridge;

  AgsAudioUnitPlugin *audio_unit_plugin;

  if(!AGS_IS_AUDIO_UNIT_BRIDGE(audio_unit_bridge)){
    return;
  }

  audio_unit_plugin = audio_unit_bridge->audio_unit_plugin;

  if(audio_unit_plugin == NULL){
    return;
  }

  effect_bridge = AGS_EFFECT_BRIDGE(AGS_MACHINE(audio_unit_bridge)->bridge);

  //TODO:JK: implement me
}

/**
 * ags_audio_unit_bridge_new:
 * @soundcard: the assigned soundcard.
 * @filename: the plugin.so
 * @effect: the effect
 *
 * Create a new instance of #AgsAudioUnitBridge
 *
 * Returns: the new #AgsAudioUnitBridge
 *
 * Since: 8.1.2
 */
AgsAudioUnitBridge*
ags_audio_unit_bridge_new(GObject *soundcard,
			  gchar *filename,
			  gchar *effect)
{
  AgsAudioUnitBridge *audio_unit_bridge;

  audio_unit_bridge = (AgsAudioUnitBridge *) g_object_new(AGS_TYPE_AUDIO_UNIT_BRIDGE,
							  NULL);

  if(soundcard != NULL){
    g_object_set(G_OBJECT(AGS_MACHINE(audio_unit_bridge)->audio),
		 "output-soundcard", soundcard,
		 NULL);
  }

  g_object_set(audio_unit_bridge,
	       "filename", filename,
	       "effect", effect,
	       NULL);

  return(audio_unit_bridge);
}
