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

#include <ags/app/machine/ags_vst3_bridge.h>
#include <ags/app/machine/ags_vst3_bridge_callbacks.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_window.h>
#include <ags/app/ags_navigation.h>
#include <ags/app/ags_effect_bridge.h>
#include <ags/app/ags_effect_bulk.h>
#include <ags/app/ags_bulk_member.h>

#include <ags/i18n.h>

void ags_vst3_bridge_class_init(AgsVst3BridgeClass *vst3_bridge);
void ags_vst3_bridge_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_vst3_bridge_init(AgsVst3Bridge *vst3_bridge);
void ags_vst3_bridge_set_property(GObject *gobject,
				  guint prop_id,
				  const GValue *value,
				  GParamSpec *param_spec);
void ags_vst3_bridge_get_property(GObject *gobject,
				  guint prop_id,
				  GValue *value,
				  GParamSpec *param_spec);
void ags_vst3_bridge_finalize(GObject *gobject);

void ags_vst3_bridge_connect(AgsConnectable *connectable);
void ags_vst3_bridge_disconnect(AgsConnectable *connectable);

void ags_vst3_bridge_resize_audio_channels(AgsMachine *machine,
					   guint audio_channels, guint audio_channels_old,
					   gpointer data);
void ags_vst3_bridge_resize_pads(AgsMachine *machine, GType type,
				 guint pads, guint pads_old,
				 gpointer data);

void ags_vst3_bridge_map_recall(AgsMachine *machine);

/**
 * SECTION:ags_vst3_bridge
 * @short_description: A composite widget to visualize a bunch of #AgsChannel
 * @title: AgsVst3Bridge
 * @section_id:
 * @include: ags/app/ags_vst3_bridge.h
 *
 * #AgsVst3Bridge is a composite widget to visualize all #AgsChannel. It should be
 * packed by an #AgsMachine.
 */

enum{
  PROP_0,
  PROP_FILENAME,
  PROP_EFFECT,
  PROP_INDEX,
};

static gpointer ags_vst3_bridge_parent_class = NULL;
static AgsConnectableInterface* ags_vst3_bridge_parent_connectable_interface;

GType
ags_vst3_bridge_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_vst3_bridge = 0;

    static const GTypeInfo ags_vst3_bridge_info = {
      sizeof(AgsVst3BridgeClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_vst3_bridge_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsVst3Bridge),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_vst3_bridge_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_vst3_bridge_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_vst3_bridge = g_type_register_static(AGS_TYPE_MACHINE,
						  "AgsVst3Bridge", &ags_vst3_bridge_info,
						  0);

    g_type_add_interface_static(ags_type_vst3_bridge,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_vst3_bridge);
  }

  return g_define_type_id__volatile;
}

void
ags_vst3_bridge_class_init(AgsVst3BridgeClass *vst3_bridge)
{
  AgsMachineClass *machine;
  GObjectClass *gobject;

  GParamSpec *param_spec;

  ags_vst3_bridge_parent_class = g_type_class_peek_parent(vst3_bridge);

  /* GObjectClass */
  gobject = G_OBJECT_CLASS(vst3_bridge);

  gobject->set_property = ags_vst3_bridge_set_property;
  gobject->get_property = ags_vst3_bridge_get_property;

  gobject->finalize = ags_vst3_bridge_finalize;
  
  /* properties */
  /**
   * AgsRecallVst3:filename:
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
   * AgsRecallVst3:effect:
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

  /**
   * AgsRecallVst3:index:
   *
   * The effect's index.
   * 
   * Since: 3.10.5
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
  machine = (AgsMachineClass *) vst3_bridge;

  machine->map_recall = ags_vst3_bridge_map_recall;
}

void
ags_vst3_bridge_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_vst3_bridge_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_vst3_bridge_connect;
  connectable->disconnect = ags_vst3_bridge_disconnect;
}

void
ags_vst3_bridge_init(AgsVst3Bridge *vst3_bridge)
{
  GtkBox *vbox;
  GtkBox *hbox;
  GtkLabel *label;
  GtkGrid *grid;
  GtkCellRenderer *cell_renderer;

  AgsAudio *audio;

  AgsMachineCounterManager *machine_counter_manager;
  AgsMachineCounter *machine_counter;
  
  gchar *machine_name;

  machine_counter_manager = ags_machine_counter_manager_get_instance();

  machine_counter = ags_machine_counter_manager_find_machine_counter(machine_counter_manager,
								     AGS_TYPE_VST3_BRIDGE);

  machine_name = NULL;

  if(machine_counter != NULL){
    machine_name = g_strdup_printf("Default %d",
				   machine_counter->counter);
  
    ags_machine_counter_increment(machine_counter);
  }
  
  g_object_set(vst3_bridge,
	       "machine-name", machine_name,
	       NULL);

  g_free(machine_name);
  
  audio = AGS_MACHINE(vst3_bridge)->audio;
  ags_audio_set_flags(audio, (AGS_AUDIO_SYNC));
  g_object_set(audio,
	       "min-audio-channels", 1,
	       "min-output-pads", 1,
	       "min-input-pads", 1,
	       NULL);

  g_signal_connect_after(G_OBJECT(vst3_bridge), "resize-audio-channels",
			 G_CALLBACK(ags_vst3_bridge_resize_audio_channels), NULL);

  g_signal_connect_after(G_OBJECT(vst3_bridge), "resize-pads",
			 G_CALLBACK(ags_vst3_bridge_resize_pads), NULL);
    
  vst3_bridge->flags = 0;

  vst3_bridge->name = NULL;

  vst3_bridge->version = AGS_VST3_BRIDGE_DEFAULT_VERSION;
  vst3_bridge->build_id = AGS_VST3_BRIDGE_DEFAULT_BUILD_ID;

  vst3_bridge->xml_type = "ags-vst3-bridge";
  
  vst3_bridge->mapped_output_pad = 0;
  vst3_bridge->mapped_input_pad = 0;

  vst3_bridge->vst3_play_container = ags_recall_container_new();
  vst3_bridge->vst3_recall_container = ags_recall_container_new();

  vst3_bridge->envelope_play_container = ags_recall_container_new();
  vst3_bridge->envelope_recall_container = ags_recall_container_new();

  vst3_bridge->buffer_play_container = ags_recall_container_new();
  vst3_bridge->buffer_recall_container = ags_recall_container_new();
				     
  vst3_bridge->filename = NULL;
  vst3_bridge->effect = NULL;
  vst3_bridge->effect_index = 0;
  
  vbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
				0);
  gtk_container_add((GtkContainer *) gtk_bin_get_child((GtkBin *) vst3_bridge),
		    (GtkWidget *) vbox);

  /* program */
  hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				0);
  gtk_box_append(vbox,
		 (GtkWidget *) hbox);

  label = (GtkLabel *) gtk_label_new(i18n("program"));
  gtk_box_append(hbox,
		 (GtkWidget *) label);

  vst3_bridge->program = (GtkComboBoxText *) gtk_combo_box_new();

  cell_renderer = gtk_cell_renderer_text_new();
  gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(vst3_bridge->program),
			     cell_renderer,
			     FALSE);
  gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(vst3_bridge->program), cell_renderer,
				 "text", 0,
				 NULL);

  cell_renderer = gtk_cell_renderer_text_new();
  gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(vst3_bridge->program),
			     cell_renderer,
			     FALSE);
  gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(vst3_bridge->program), cell_renderer,
				 "text", 1,
				 NULL);

  cell_renderer = gtk_cell_renderer_text_new();
  gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(vst3_bridge->program),
			     cell_renderer,
			     FALSE);
  gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(vst3_bridge->program), cell_renderer,
				 "text", 2,
				 NULL);

  gtk_box_append(hbox,
		 (GtkWidget *) vst3_bridge->program);

  /* effect bridge */
  AGS_MACHINE(vst3_bridge)->bridge = (GtkContainer *) ags_effect_bridge_new(audio);
  gtk_box_append(vbox,
		 (GtkWidget *) AGS_MACHINE(vst3_bridge)->bridge);
  
  grid = (GtkGrid *) gtk_grid_new();
  gtk_box_append((GtkBox *) AGS_EFFECT_BRIDGE(AGS_MACHINE(vst3_bridge)->bridge),
		 (GtkWidget *) grid);

  AGS_EFFECT_BRIDGE(AGS_MACHINE(vst3_bridge)->bridge)->bulk_input = (GtkWidget *) ags_effect_bulk_new(audio,
												      AGS_TYPE_INPUT);
  AGS_EFFECT_BULK(AGS_EFFECT_BRIDGE(AGS_MACHINE(vst3_bridge)->bridge)->bulk_input)->flags |= (AGS_EFFECT_BULK_HIDE_BUTTONS |
											      AGS_EFFECT_BULK_HIDE_ENTRIES |
											      AGS_EFFECT_BULK_SHOW_LABELS);

  gtk_widget_set_valign(AGS_EFFECT_BRIDGE(AGS_MACHINE(vst3_bridge)->bridge)->bulk_input,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign(AGS_EFFECT_BRIDGE(AGS_MACHINE(vst3_bridge)->bridge)->bulk_input,
			GTK_ALIGN_FILL);
  
  gtk_grid_attach(grid,
		  AGS_EFFECT_BRIDGE(AGS_MACHINE(vst3_bridge)->bridge)->bulk_input,
		  0, 0,
		  1, 1);

  vst3_bridge->icomponent = NULL;
  vst3_bridge->iedit_controller = NULL;

  vst3_bridge->icomponent_handler = NULL;
  
  vst3_bridge->iplug_view = NULL;

  /* vst3 menu */
  //TODO:JK: implement me
  
  vst3_bridge->block_control = g_hash_table_new_full(g_direct_hash,
						     g_direct_equal,
						     NULL,
						     NULL);
}

void
ags_vst3_bridge_set_property(GObject *gobject,
			     guint prop_id,
			     const GValue *value,
			     GParamSpec *param_spec)
{
  AgsVst3Bridge *vst3_bridge;

  vst3_bridge = AGS_VST3_BRIDGE(gobject);

  switch(prop_id){
  case PROP_FILENAME:
    {
      gchar *str;
      gchar *filename;

      filename = g_value_get_string(value);

      if(filename == vst3_bridge->filename){
	return;
      }

      if(vst3_bridge->filename != NULL){
	g_free(vst3_bridge->filename);
      }

      if(filename != NULL){
	if(!g_file_test(filename,
			G_FILE_TEST_EXISTS)){
	  AgsWindow *window;

	  window = (AgsWindow *) gtk_widget_get_ancestor((GtkWidget *) vst3_bridge,
							 AGS_TYPE_WINDOW);

	  str = g_strdup_printf("%s %s",
				i18n("Plugin file not present"),
				filename);
	  ags_window_show_error(window,
				str);

	  g_free(str);
	}
      }

      vst3_bridge->filename = g_strdup(filename);
    }
    break;
  case PROP_EFFECT:
    {
      gchar *effect;
      
      effect = g_value_get_string(value);

      if(effect == vst3_bridge->effect){
	return;
      }

      if(vst3_bridge->effect != NULL){
	g_free(vst3_bridge->effect);
      }

      vst3_bridge->effect = g_strdup(effect);
    }
    break;
  case PROP_INDEX:
    {
      unsigned long effect_index;
      
      effect_index = (unsigned long) g_value_get_uint(value);

      if(effect_index == vst3_bridge->effect_index){
	return;
      }

      vst3_bridge->effect_index = effect_index;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_vst3_bridge_get_property(GObject *gobject,
			     guint prop_id,
			     GValue *value,
			     GParamSpec *param_spec)
{
  AgsVst3Bridge *vst3_bridge;

  vst3_bridge = AGS_VST3_BRIDGE(gobject);

  switch(prop_id){
  case PROP_FILENAME:
    {
      g_value_set_string(value, vst3_bridge->filename);
    }
    break;
  case PROP_EFFECT:
    {
      g_value_set_string(value, vst3_bridge->effect);
    }
    break;
  case PROP_INDEX:
    {
      g_value_set_uint(value, (guint) vst3_bridge->effect_index);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_vst3_bridge_finalize(GObject *gobject)
{
  AgsVst3Bridge *vst3_bridge;

  vst3_bridge = (AgsVst3Bridge *) gobject;
  
  g_object_disconnect(G_OBJECT(vst3_bridge),
		      "any_signal::resize-audio-channels",
		      G_CALLBACK(ags_vst3_bridge_resize_audio_channels),
		      NULL,
		      "any_signal::resize-pads",
		      G_CALLBACK(ags_vst3_bridge_resize_pads),
		      NULL,
		      NULL);

  g_free(vst3_bridge->filename);
  g_free(vst3_bridge->effect);
  
  /* call parent */
  G_OBJECT_CLASS(ags_vst3_bridge_parent_class)->finalize(gobject);
}

void
ags_vst3_bridge_connect(AgsConnectable *connectable)
{
  AgsVst3Bridge *vst3_bridge;
  AgsEffectBridge *effect_bridge;
  AgsBulkMember *bulk_member;
  GtkWidget *control;

  GList *list, *list_start;


  if((AGS_MACHINE_CONNECTED & (AGS_MACHINE(connectable)->flags)) != 0){
    return;
  }

  ags_vst3_bridge_parent_connectable_interface->connect(connectable);

  vst3_bridge = AGS_VST3_BRIDGE(connectable);

  g_signal_connect_after(G_OBJECT(vst3_bridge->program), "changed",
			 G_CALLBACK(ags_vst3_bridge_program_changed_callback), vst3_bridge);

  /* menu */
  list =
    list_start = gtk_container_get_children((GtkContainer *) vst3_bridge->vst3_menu);

  g_signal_connect(G_OBJECT(list->data), "activate",
		   G_CALLBACK(ags_vst3_bridge_show_gui_callback), vst3_bridge);

  g_list_free(list_start);

  /* bulk member */
  effect_bridge = AGS_EFFECT_BRIDGE(AGS_MACHINE(vst3_bridge)->bridge);
  
  list =
    list_start = gtk_container_get_children((GtkContainer *) AGS_EFFECT_BULK(effect_bridge->bulk_input)->grid);

  while(list != NULL){
    bulk_member = list->data;

    control = gtk_bin_get_child(GTK_BIN(bulk_member));

    if(bulk_member->widget_type == AGS_TYPE_DIAL){
      g_signal_connect_after(GTK_WIDGET(control), "value-changed",
			     G_CALLBACK(ags_vst3_bridge_dial_changed_callback), vst3_bridge);
    }else if(bulk_member->widget_type == GTK_TYPE_SCALE){
      g_signal_connect_after(GTK_WIDGET(control), "value-changed",
			     G_CALLBACK(ags_vst3_bridge_scale_changed_callback), vst3_bridge);
    }else if(bulk_member->widget_type == GTK_TYPE_SPIN_BUTTON){
      g_signal_connect_after(GTK_WIDGET(control), "value-changed",
			     G_CALLBACK(ags_vst3_bridge_spin_button_changed_callback), vst3_bridge);
    }else if(bulk_member->widget_type == GTK_TYPE_CHECK_BUTTON){
      g_signal_connect_after(GTK_WIDGET(control), "clicked",
			     G_CALLBACK(ags_vst3_bridge_check_button_clicked_callback), vst3_bridge);
    }else if(bulk_member->widget_type == GTK_TYPE_TOGGLE_BUTTON){
      g_signal_connect_after(GTK_WIDGET(control), "clicked",
			     G_CALLBACK(ags_vst3_bridge_toggle_button_clicked_callback), vst3_bridge);
    }else if(bulk_member->widget_type == GTK_TYPE_BUTTON){
      g_signal_connect_after(GTK_WIDGET(control), "clicked",
			     G_CALLBACK(ags_vst3_bridge_button_clicked_callback), vst3_bridge);
    }

    list = list->next;
  }

  g_list_free(list_start);
}

void
ags_vst3_bridge_disconnect(AgsConnectable *connectable)
{
  AgsVst3Bridge *vst3_bridge;

  if((AGS_MACHINE_CONNECTED & (AGS_MACHINE(connectable)->flags)) == 0){
    return;
  }

  ags_vst3_bridge_parent_connectable_interface->connect(connectable);

  vst3_bridge = AGS_VST3_BRIDGE(connectable);

  g_object_disconnect(G_OBJECT(vst3_bridge->program),
		      "any_signal::changed",
		      G_CALLBACK(ags_vst3_bridge_program_changed_callback),
		      vst3_bridge,
		      NULL);

  //TODO:JK: implement me
}

void
ags_vst3_bridge_resize_audio_channels(AgsMachine *machine,
				      guint audio_channels, guint audio_channels_old,
				      gpointer data)
{
  AgsVst3Bridge *vst3_bridge;

  vst3_bridge = (AgsVst3Bridge *) machine;  
  
  if(audio_channels > audio_channels_old){
    /* recall */
    if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0){
      ags_vst3_bridge_input_map_recall(vst3_bridge,
				       audio_channels_old,
				       0);

      ags_vst3_bridge_output_map_recall(vst3_bridge,
					audio_channels_old,
					0);
    }
  }
}

void
ags_vst3_bridge_resize_pads(AgsMachine *machine, GType type,
			    guint pads, guint pads_old,
			    gpointer data)
{
  AgsVst3Bridge *vst3_bridge;
  gboolean grow;

  vst3_bridge = (AgsVst3Bridge *) machine;

  if(pads_old < pads){
    grow = TRUE;
  }else{
    grow = FALSE;
  }
  
  if(g_type_is_a(type, AGS_TYPE_INPUT)){
    if(grow){
      /* recall */
      if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0){
	ags_vst3_bridge_input_map_recall(vst3_bridge,
					 0,
					 pads_old);
      }
    }else{
      vst3_bridge->mapped_input_pad = pads;
    }
  }else{
    if(grow){
      /* recall */
      if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0){
	ags_vst3_bridge_output_map_recall(vst3_bridge,
					  0,
					  pads_old);
      }
    }else{
      vst3_bridge->mapped_output_pad = pads;
    }
  }
}

void
ags_vst3_bridge_map_recall(AgsMachine *machine)
{
  AgsNavigation *navigation;
  AgsVst3Bridge *vst3_bridge;
  
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

  vst3_bridge = (AgsVst3Bridge *) machine;
  
  audio = machine->audio;

  position = 0;

  /* add to effect bridge */
  ags_effect_bulk_add_plugin((AgsEffectBulk *) AGS_EFFECT_BRIDGE(machine->bridge)->bulk_input,
			     NULL,
			     vst3_bridge->vst3_play_container, vst3_bridge->vst3_recall_container,
			     "ags-fx-vst3",
			     vst3_bridge->filename,
			     vst3_bridge->effect,
			     0, 0,
			     0, 0,
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
				       vst3_bridge->envelope_play_container, vst3_bridge->envelope_recall_container,
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
				       vst3_bridge->buffer_play_container, vst3_bridge->buffer_recall_container,
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
  ags_vst3_bridge_input_map_recall(vst3_bridge,
				   0,
				   0);

  /* depending on destination */
  ags_vst3_bridge_output_map_recall(vst3_bridge,
				    0,
				    0);
  
  /* call parent */
  AGS_MACHINE_CLASS(ags_vst3_bridge_parent_class)->map_recall(machine);
}

void
ags_vst3_bridge_input_map_recall(AgsVst3Bridge *vst3_bridge,
				 guint audio_channel_start,
				 guint input_pad_start)
{
  AgsAudio *audio;

  GList *start_recall;

  gint position;
  guint input_pads;
  guint audio_channels;

  if(vst3_bridge->mapped_input_pad > input_pad_start){
    return;
  }

  audio = AGS_MACHINE(vst3_bridge)->audio;

  position = 0;

  input_pads = 0;
  audio_channels = 0;
  
  /* get some fields */
  g_object_get(audio,
	       "input-pads", &input_pads,
	       "audio-channels", &audio_channels,
	       NULL);
  
  /* add to effect bridge */
  ags_effect_bulk_add_plugin((AgsEffectBulk *) AGS_EFFECT_BRIDGE(AGS_MACHINE(vst3_bridge)->bridge)->bulk_input,
			     NULL,
			     vst3_bridge->vst3_play_container, vst3_bridge->vst3_recall_container,
			     "ags-fx-vst3",
			     vst3_bridge->filename,
			     vst3_bridge->effect,
			     audio_channel_start, audio_channels,
			     input_pad_start, input_pads,
			     position,
			     (AGS_FX_FACTORY_REMAP | AGS_FX_FACTORY_INPUT), 0);

  /* ags-fx-envelope */
  start_recall = ags_fx_factory_create(audio,
				       vst3_bridge->envelope_play_container, vst3_bridge->envelope_recall_container,
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
				       vst3_bridge->buffer_play_container, vst3_bridge->buffer_recall_container,
				       "ags-fx-buffer",
				       NULL,
				       NULL,
				       audio_channel_start, audio_channels,
				       input_pad_start, input_pads,
				       position,
				       (AGS_FX_FACTORY_REMAP | AGS_FX_FACTORY_INPUT), 0);

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);

  vst3_bridge->mapped_input_pad = input_pads;
}

void
ags_vst3_bridge_output_map_recall(AgsVst3Bridge *vst3_bridge,
				  guint audio_channel_start,
				  guint output_pad_start)
{
  AgsAudio *audio;

  guint output_pads;

  if(vst3_bridge->mapped_output_pad > output_pad_start){
    return;
  }

  audio = AGS_MACHINE(vst3_bridge)->audio;

  /* get some fields */
  g_object_get(audio,
	       "output-pads", &output_pads,
	       NULL);
  
  vst3_bridge->mapped_output_pad = output_pads;
}

void
ags_vst3_bridge_load(AgsVst3Bridge *vst3_bridge)
{
  GtkListStore *model;

  GtkTreeIter iter;

  AgsVst3Plugin *vst3_plugin;

  AgsConfig *config;

  AgsVstComponentHandler *component_handler;
  AgsVstIUnitInfo *iunit_info;    

  GList *start_program, *program;

  gchar **parameter_name;

  guint buffer_size;
  guint samplerate;
  guint n_params;
  guint i, i_stop;
  guint j, j_stop;
  gint32 parameter_count;
  AgsVstTResult val;

  GValue *value;

  if(!AGS_IS_VST3_BRIDGE(vst3_bridge)){
    return;
  }
  
  /* load plugin */
  vst3_plugin = ags_vst3_manager_find_vst3_plugin(ags_vst3_manager_get_instance(),
						  vst3_bridge->filename,
						  vst3_bridge->effect);

  if(vst3_plugin == NULL ||
     vst3_plugin->iedit_controller == NULL){
    return;
  }

  /* ui */
  config = ags_config_get_instance();
  
#if HAVE_GLIB_2_68
  strv_builder = g_strv_builder_new();

  g_strv_builder_add(strv_builder,
		     "buffer-size");
  g_strv_builder_add(strv_builder,
		     "samplerate");
  g_strv_builder_add(strv_builder,
		     "iedit-controller");
  g_strv_builder_add(strv_builder,
		     "iaudio-processor");
  g_strv_builder_add(strv_builder,
		     "iedit-controller-host-editing");
  
  parameter_name = g_strv_builder_end(strv_builder);
#else
  parameter_name = (gchar **) g_malloc(6 * sizeof(gchar *));

  parameter_name[0] = g_strdup("buffer-size");
  parameter_name[1] = g_strdup("samplerate");
  parameter_name[2] = g_strdup("iedit-controller");
  parameter_name[3] = g_strdup("iaudio-processor");
  parameter_name[4] = g_strdup("iedit-controller-host-editing");
  parameter_name[5] = NULL;
#endif

  n_params = 5;

  value = g_new0(GValue,
		 5);

  g_value_init(value,
	       G_TYPE_UINT);
    
  g_value_init(value + 1,
	       G_TYPE_UINT);
    
  g_value_init(value + 2,
	       G_TYPE_POINTER);

  g_value_init(value + 3,
	       G_TYPE_POINTER);

  g_value_init(value + 4,
	       G_TYPE_POINTER);

  samplerate = ags_soundcard_helper_config_get_samplerate(config);
  buffer_size = ags_soundcard_helper_config_get_buffer_size(config);

  g_value_set_uint(value,
		   buffer_size);

  g_value_set_uint(value + 1,
		   samplerate);

  vst3_bridge->icomponent = ags_base_plugin_instantiate_with_params((AgsBasePlugin *) vst3_plugin,
								    &n_params,
								    &parameter_name, &value);
  
  vst3_bridge->iedit_controller = g_value_get_pointer(value + 2);
  
  component_handler = ags_vst_component_handler_new();

  ags_vst_funknown_query_interface(component_handler,
				   ags_vst_icomponent_handler_get_iid(), &(vst3_bridge->icomponent_handler));

  vst3_bridge->perform_edit_handler = ags_vst_component_handler_connect_handler(component_handler, "performEdit", ags_vst3_bridge_perform_edit_callback, vst3_bridge);
  vst3_bridge->restart_component_handler = ags_vst_component_handler_connect_handler(component_handler, "restartComponent", ags_vst3_bridge_restart_component_callback, vst3_bridge);
  
  ags_vst_iedit_controller_set_component_handler(vst3_bridge->iedit_controller,
						 vst3_bridge->icomponent_handler);

  g_strfreev(parameter_name);
  g_free(value);

  vst3_bridge->iedit_controller_host_editing = g_value_get_pointer(value + 4);

  /*  */
  gtk_list_store_clear(GTK_LIST_STORE(gtk_combo_box_get_model(GTK_COMBO_BOX(vst3_bridge->program))));

  iunit_info = NULL;
  val = ags_vst_funknown_query_interface(vst3_plugin->iedit_controller,
					 ags_vst_iunit_info_get_iid(), &iunit_info);

  if(iunit_info != NULL){
    program =
      start_program = g_hash_table_get_keys(vst3_plugin->program);
  
    /* load ports */
    model = gtk_list_store_new(6,
			       G_TYPE_STRING,
			       G_TYPE_STRING,
			       G_TYPE_STRING,
			       G_TYPE_UINT,
			       G_TYPE_UINT,
			       G_TYPE_UINT);
  
    while(program != NULL){
      i_stop = ags_vst_iunit_info_get_program_list_count(iunit_info);

      for(i = 0; i < i_stop; i++){
	AgsVstProgramListInfo *program_list_info;

	gchar *program_list_info_name;      

	AgsVstProgramListID program_list_info_id;

	program_list_info = ags_vst_program_list_info_alloc();

	ags_vst_iunit_info_get_program_list_info(iunit_info,
						 i,
						 program_list_info);

	j_stop = ags_vst_program_list_info_get_program_count(program_list_info);

	program_list_info_id = ags_vst_program_list_info_get_id(program_list_info);

	program_list_info_name = ags_vst_program_list_info_get_name(program_list_info);
      
	for(j = 0; j < j_stop; j++){
	  gchar *program_name;

	  program_name = NULL;
	  ags_vst_iunit_info_get_program_name(iunit_info,
					      program_list_info_id, j,
					      &program_name);

	  gtk_list_store_append(model, &iter);

	  gtk_list_store_set(model, &iter,
			     0, program->data,
			     1, program_list_info_name,
			     2, program_name,
			     3, GPOINTER_TO_UINT(g_hash_table_lookup(vst3_plugin->program, program->data)),
			     4, program_list_info_id,
			     5, j,
			     -1);
	}
      }
    
      program = program->next;
    }

    g_list_free(start_program);

    gtk_combo_box_set_model(GTK_COMBO_BOX(vst3_bridge->program),
			    GTK_TREE_MODEL(model));
  }

  parameter_count = ags_vst_iedit_controller_get_parameter_count(vst3_bridge->iedit_controller);

  vst3_bridge->flags |= AGS_VST3_BRIDGE_NO_UPDATE;
  
  for(i = 0; i < parameter_count; i++){
    AgsVstParameterInfo *info;
    AgsVstParamID param_id;
    
    guint flags;
    gdouble default_normalized_value;
    
    info = ags_vst_parameter_info_alloc();
    
    ags_vst_iedit_controller_get_parameter_info(vst3_bridge->iedit_controller,
						i, info);

    flags = ags_vst_parameter_info_get_flags(info);

    if((AGS_VST_KIS_PROGRAM_CHANGE & (flags)) != 0){
      ags_vst_parameter_info_free(info);
      
      continue;
    }

    param_id = ags_vst_parameter_info_get_param_id(info);
    
    default_normalized_value = ags_vst_parameter_info_get_default_normalized_value(info);

    if(vst3_bridge->iedit_controller_host_editing != NULL){
      ags_vst_iedit_controller_host_editing_begin_edit_from_host(vst3_bridge->iedit_controller_host_editing,
								 param_id);
    }
    
    ags_vst_iedit_controller_set_param_normalized(vst3_bridge->iedit_controller,
						  param_id,
						  default_normalized_value);

    if(vst3_bridge->iedit_controller_host_editing != NULL){
      ags_vst_iedit_controller_host_editing_end_edit_from_host(vst3_bridge->iedit_controller_host_editing,
							       param_id);
    }
    
    ags_vst_parameter_info_free(info);
  }
  
  vst3_bridge->flags &= (~AGS_VST3_BRIDGE_NO_UPDATE);
}

void
ags_vst3_bridge_reload_port(AgsVst3Bridge *vst3_bridge)
{
  AgsVst3Plugin *vst3_plugin;
  AgsPluginPort *plugin_port;

  GList *start_bulk_member, *bulk_member;
  
  guint i;
  gint32 parameter_count;

  GRecMutex *base_plugin_mutex;

  if(!AGS_IS_VST3_BRIDGE(vst3_bridge)){
    return;
  }

  vst3_plugin = vst3_bridge->vst3_plugin;

  base_plugin_mutex = AGS_BASE_PLUGIN_GET_OBJ_MUTEX(vst3_plugin);

  start_bulk_member = gtk_container_get_children(AGS_EFFECT_BULK(AGS_EFFECT_BRIDGE(AGS_MACHINE(vst3_bridge)->bridge)->bulk_input)->grid);

  parameter_count = ags_vst_iedit_controller_get_parameter_count(vst3_bridge->iedit_controller);

  vst3_bridge->flags |= AGS_VST3_BRIDGE_NO_UPDATE;  

  for(i = 0; i < parameter_count; i++){
    AgsVstParameterInfo *info;
    AgsVstParamID param_id;
    
    gchar *specifier;
    
    guint flags;
    gdouble current_normalized_value;
    gdouble value;
    
    info = ags_vst_parameter_info_alloc();
    
    ags_vst_iedit_controller_get_parameter_info(vst3_bridge->iedit_controller,
						i, info);

    flags = ags_vst_parameter_info_get_flags(info);

    if((AGS_VST_KIS_PROGRAM_CHANGE & (flags)) != 0){
      ags_vst_parameter_info_free(info);
      
      continue;
    }

    param_id = ags_vst_parameter_info_get_param_id(info);

    g_rec_mutex_lock(base_plugin_mutex);
  
    plugin_port = g_hash_table_lookup(vst3_plugin->plugin_port,
				      GINT_TO_POINTER(param_id));  

    g_rec_mutex_unlock(base_plugin_mutex);

    if(plugin_port == NULL){
      ags_vst_parameter_info_free(info);
      
      continue;
    }

    specifier = NULL;
    
    g_object_get(plugin_port,
		 "port-name", &specifier,
		 NULL);
    
    if(vst3_bridge->iedit_controller_host_editing != NULL){
      ags_vst_iedit_controller_host_editing_begin_edit_from_host(vst3_bridge->iedit_controller_host_editing,
								 param_id);
    }

    current_normalized_value = ags_vst_iedit_controller_get_param_normalized(vst3_bridge->iedit_controller,
									     param_id);

    value = ags_vst_iedit_controller_normalized_param_to_plain(vst3_bridge->iedit_controller,
							       param_id,
							       current_normalized_value);

    bulk_member = start_bulk_member;
  
    while(bulk_member != NULL){
      if(AGS_IS_BULK_MEMBER(bulk_member->data) &&
	 !g_strcmp0(AGS_BULK_MEMBER(bulk_member->data)->specifier, specifier)){
	GtkWidget *child_widget;

	gchar *block_scope;

	child_widget = ags_bulk_member_get_widget(bulk_member->data);

	if((block_scope = g_hash_table_lookup(vst3_bridge->block_control, child_widget)) == NULL ||
	   !g_strcmp0(block_scope, AGS_VST3_BRIDGE_BLOCK_CONTROL_VST3_UI) == FALSE){
	  g_hash_table_insert(vst3_bridge->block_control,
			      child_widget,
			      AGS_VST3_BRIDGE_BLOCK_CONTROL_VST3_UI);
  
	  if(AGS_IS_DIAL(child_widget)){
	    ags_dial_set_value((AgsDial *) child_widget,
			       value);
	  }else if(GTK_IS_SCALE(child_widget)){
	    gtk_range_set_value((GtkRange *) child_widget,
				value);
	  }else if(GTK_IS_TOGGLE_BUTTON(child_widget)){
	    gboolean active;

	    active = (value != 0.0) ? TRUE: FALSE;
	
	    gtk_toggle_button_set_active((GtkToggleButton *) child_widget,
					 active);
	  }else if(GTK_IS_BUTTON(child_widget)){
	    g_signal_emit_by_name((GtkButton *) child_widget,
				  "clicked");
	  }

	  g_hash_table_insert(vst3_bridge->block_control,
			      child_widget,
			      NULL);
	}
      
	break;
      }
    
      bulk_member = bulk_member->next;
    }

    if(vst3_bridge->iedit_controller_host_editing != NULL){
      ags_vst_iedit_controller_host_editing_end_edit_from_host(vst3_bridge->iedit_controller_host_editing,
							       param_id);
    }
    
    ags_vst_parameter_info_free(info);
  }
  
  vst3_bridge->flags &= (~AGS_VST3_BRIDGE_NO_UPDATE);

  g_list_free(start_bulk_member);  
}

/**
 * ags_vst3_bridge_new:
 * @soundcard: the assigned soundcard.
 * @filename: the plugin.so
 * @effect: the effect
 *
 * Create a new instance of #AgsVst3Bridge
 *
 * Returns: the new #AgsVst3Bridge
 *
 * Since: 3.10.5
 */
AgsVst3Bridge*
ags_vst3_bridge_new(GObject *soundcard,
		    gchar *filename,
		    gchar *effect)
{
  AgsVst3Bridge *vst3_bridge;

  vst3_bridge = (AgsVst3Bridge *) g_object_new(AGS_TYPE_VST3_BRIDGE,
					       NULL);

  if(soundcard != NULL){
    g_object_set(G_OBJECT(AGS_MACHINE(vst3_bridge)->audio),
		 "output-soundcard", soundcard,
		 NULL);
  }

  g_object_set(vst3_bridge,
	       "filename", filename,
	       "effect", effect,
	       NULL);

  return(vst3_bridge);
}
