/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2021 Joël Krähemann
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

#include <ags/X/machine/ags_live_vst3_bridge.h>
#include <ags/X/machine/ags_live_vst3_bridge_callbacks.h>

#include <ags/X/ags_ui_provider.h>
#include <ags/X/ags_window.h>
#include <ags/X/ags_navigation.h>
#include <ags/X/ags_effect_bridge.h>
#include <ags/X/ags_effect_bulk.h>
#include <ags/X/ags_bulk_member.h>

#include <ags/i18n.h>

void ags_live_vst3_bridge_class_init(AgsLiveVst3BridgeClass *live_vst3_bridge);
void ags_live_vst3_bridge_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_live_vst3_bridge_init(AgsLiveVst3Bridge *live_vst3_bridge);
void ags_live_vst3_bridge_set_property(GObject *gobject,
				       guint prop_id,
				       const GValue *value,
				       GParamSpec *param_spec);
void ags_live_vst3_bridge_get_property(GObject *gobject,
				       guint prop_id,
				       GValue *value,
				       GParamSpec *param_spec);
void ags_live_vst3_bridge_finalize(GObject *gobject);

void ags_live_vst3_bridge_connect(AgsConnectable *connectable);
void ags_live_vst3_bridge_disconnect(AgsConnectable *connectable);

void ags_live_vst3_bridge_resize_audio_channels(AgsMachine *machine,
						guint audio_channels, guint audio_channels_old,
						gpointer data);
void ags_live_vst3_bridge_resize_pads(AgsMachine *machine, GType type,
				      guint pads, guint pads_old,
				      gpointer data);

void ags_live_vst3_bridge_map_recall(AgsMachine *machine);

/**
 * SECTION:ags_live_vst3_bridge
 * @short_description: A composite widget to visualize a bunch of #AgsChannel
 * @title: AgsLiveVst3Bridge
 * @section_id:
 * @include: ags/X/machine/ags_live_vst3_bridge.h
 *
 * #AgsLiveVst3Bridge is a composite widget to visualize all #AgsChannel. It should be
 * packed by an #AgsMachine.
 */

enum{
  PROP_0,
  PROP_FILENAME,
  PROP_EFFECT,
  PROP_INDEX,
};

static gpointer ags_live_vst3_bridge_parent_class = NULL;
static AgsConnectableInterface* ags_live_vst3_bridge_parent_connectable_interface;

extern GHashTable *ags_effect_bulk_indicator_queue_draw;

GType
ags_live_vst3_bridge_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_live_vst3_bridge = 0;

    static const GTypeInfo ags_live_vst3_bridge_info = {
      sizeof(AgsLiveVst3BridgeClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_live_vst3_bridge_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsLiveVst3Bridge),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_live_vst3_bridge_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_live_vst3_bridge_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_live_vst3_bridge = g_type_register_static(AGS_TYPE_MACHINE,
						       "AgsLiveVst3Bridge", &ags_live_vst3_bridge_info,
						       0);

    g_type_add_interface_static(ags_type_live_vst3_bridge,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_live_vst3_bridge);
  }

  return g_define_type_id__volatile;
}

void
ags_live_vst3_bridge_class_init(AgsLiveVst3BridgeClass *live_vst3_bridge)
{
  AgsMachineClass *machine;
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_live_vst3_bridge_parent_class = g_type_class_peek_parent(live_vst3_bridge);

  /* GObjectClass */
  gobject = G_OBJECT_CLASS(live_vst3_bridge);

  gobject->set_property = ags_live_vst3_bridge_set_property;
  gobject->get_property = ags_live_vst3_bridge_get_property;

  gobject->finalize = ags_live_vst3_bridge_finalize;
  
  /* properties */
  /**
   * AgsRecallLiveVst3:filename:
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
   * AgsRecallLiveVst3:effect:
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
   * AgsRecallLiveVst3:index:
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
  machine = (AgsMachineClass *) live_vst3_bridge;

  machine->map_recall = ags_live_vst3_bridge_map_recall;
}

void
ags_live_vst3_bridge_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_live_vst3_bridge_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_live_vst3_bridge_connect;
  connectable->disconnect = ags_live_vst3_bridge_disconnect;
}

void
ags_live_vst3_bridge_init(AgsLiveVst3Bridge *live_vst3_bridge)
{
  GtkBox *vbox;
  GtkBox *hbox;
  GtkLabel *label;
  GtkGrid *grid;
  GtkCellRenderer *cell_renderer;

  AgsAudio *audio;

  g_signal_connect_after((GObject *) live_vst3_bridge, "parent-set",
			 G_CALLBACK(ags_live_vst3_bridge_parent_set_callback), (gpointer) live_vst3_bridge);

  audio = AGS_MACHINE(live_vst3_bridge)->audio;
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
  
  AGS_MACHINE(live_vst3_bridge)->flags |= (AGS_MACHINE_IS_SYNTHESIZER |
					   AGS_MACHINE_REVERSE_NOTATION);

  ags_machine_popup_add_connection_options((AgsMachine *) live_vst3_bridge,
					   (AGS_MACHINE_POPUP_MIDI_DIALOG));

  g_signal_connect_after(G_OBJECT(live_vst3_bridge), "resize-audio-channels",
			 G_CALLBACK(ags_live_vst3_bridge_resize_audio_channels), NULL);

  g_signal_connect_after(G_OBJECT(live_vst3_bridge), "resize-pads",
			 G_CALLBACK(ags_live_vst3_bridge_resize_pads), NULL);
    
  live_vst3_bridge->flags = 0;

  live_vst3_bridge->name = NULL;

  live_vst3_bridge->version = AGS_LIVE_VST3_BRIDGE_DEFAULT_VERSION;
  live_vst3_bridge->build_id = AGS_LIVE_VST3_BRIDGE_DEFAULT_BUILD_ID;

  live_vst3_bridge->xml_type = "ags-live-vst3-bridge";
  
  live_vst3_bridge->mapped_output_pad = 0;
  live_vst3_bridge->mapped_input_pad = 0;

  live_vst3_bridge->vst3_play_container = ags_recall_container_new();
  live_vst3_bridge->vst3_recall_container = ags_recall_container_new();

  live_vst3_bridge->envelope_play_container = ags_recall_container_new();
  live_vst3_bridge->envelope_recall_container = ags_recall_container_new();

  live_vst3_bridge->buffer_play_container = ags_recall_container_new();
  live_vst3_bridge->buffer_recall_container = ags_recall_container_new();

  live_vst3_bridge->filename = NULL;
  live_vst3_bridge->effect = NULL;
  live_vst3_bridge->effect_index = 0;
  
  vbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
				0);
  gtk_container_add((GtkContainer *) gtk_bin_get_child((GtkBin *) live_vst3_bridge),
		    (GtkWidget *) vbox);

  /* program */
  hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				0);
  gtk_box_pack_start(vbox,
		     (GtkWidget *) hbox,
		     FALSE, FALSE,
		     0);

  label = (GtkLabel *) gtk_label_new(i18n("program"));
  gtk_box_pack_start(hbox,
		     (GtkWidget *) label,
		     FALSE, FALSE,
		     0);

  live_vst3_bridge->program = (GtkComboBoxText *) gtk_combo_box_text_new();

  cell_renderer = gtk_cell_renderer_text_new();
  gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(live_vst3_bridge->program),
			     cell_renderer,
			     FALSE);
  gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(live_vst3_bridge->program), cell_renderer,
				 "text", 0,
				 NULL);

  cell_renderer = gtk_cell_renderer_text_new();
  gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(live_vst3_bridge->program),
			     cell_renderer,
			     FALSE);
  gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(live_vst3_bridge->program), cell_renderer,
				 "text", 1,
				 NULL);

  cell_renderer = gtk_cell_renderer_text_new();
  gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(live_vst3_bridge->program),
			     cell_renderer,
			     FALSE);
  gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(live_vst3_bridge->program), cell_renderer,
				 "text", 2,
				 NULL);

  gtk_box_pack_start(hbox,
		     (GtkWidget *) live_vst3_bridge->program,
		     FALSE, FALSE,
		     0);

  /* effect bridge */
  AGS_MACHINE(live_vst3_bridge)->bridge = (GtkContainer *) ags_effect_bridge_new(audio);
  gtk_box_pack_start(vbox,
		     (GtkWidget *) AGS_MACHINE(live_vst3_bridge)->bridge,
		     FALSE, FALSE,
		     0);
  
  grid = (GtkGrid *) gtk_grid_new();
  gtk_box_pack_start((GtkBox *) AGS_EFFECT_BRIDGE(AGS_MACHINE(live_vst3_bridge)->bridge),
		     (GtkWidget *) grid,
		     FALSE, FALSE,
		     0);

  AGS_EFFECT_BRIDGE(AGS_MACHINE(live_vst3_bridge)->bridge)->bulk_input = (GtkWidget *) ags_effect_bulk_new(audio,
													   AGS_TYPE_INPUT);
  AGS_EFFECT_BULK(AGS_EFFECT_BRIDGE(AGS_MACHINE(live_vst3_bridge)->bridge)->bulk_input)->flags |= (AGS_EFFECT_BULK_HIDE_BUTTONS |
												   AGS_EFFECT_BULK_HIDE_ENTRIES |
												   AGS_EFFECT_BULK_SHOW_LABELS);

  gtk_widget_set_valign((GtkWidget *) AGS_EFFECT_BRIDGE(AGS_MACHINE(live_vst3_bridge)->bridge)->bulk_input,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) AGS_EFFECT_BRIDGE(AGS_MACHINE(live_vst3_bridge)->bridge)->bulk_input,
			GTK_ALIGN_FILL);
  
  gtk_grid_attach(grid,
		  (GtkWidget *) AGS_EFFECT_BRIDGE(AGS_MACHINE(live_vst3_bridge)->bridge)->bulk_input,
		  0, 0,
		  1, 1);
}

void
ags_live_vst3_bridge_set_property(GObject *gobject,
				  guint prop_id,
				  const GValue *value,
				  GParamSpec *param_spec)
{
  AgsLiveVst3Bridge *live_vst3_bridge;

  live_vst3_bridge = AGS_LIVE_VST3_BRIDGE(gobject);

  switch(prop_id){
  case PROP_FILENAME:
    {
      gchar *str;
      gchar *filename;

      filename = g_value_get_string(value);

      if(filename == live_vst3_bridge->filename){
	return;
      }

      if(live_vst3_bridge->filename != NULL){
	g_free(live_vst3_bridge->filename);
      }

      if(filename != NULL){
	if(!g_file_test(filename,
			G_FILE_TEST_EXISTS)){
	  AgsWindow *window;

	  window = (AgsWindow *) gtk_widget_get_toplevel((GtkWidget *) live_vst3_bridge);

	  str = g_strdup_printf("%s %s",
				i18n("Plugin file not present"),
				filename);
	  ags_window_show_error(window,
				str);

	  g_free(str);
	}
      }

      live_vst3_bridge->filename = g_strdup(filename);
    }
    break;
  case PROP_EFFECT:
    {
      gchar *effect;
      
      effect = g_value_get_string(value);

      if(effect == live_vst3_bridge->effect){
	return;
      }

      if(live_vst3_bridge->effect != NULL){
	g_free(live_vst3_bridge->effect);
      }

      live_vst3_bridge->effect = g_strdup(effect);
    }
    break;
  case PROP_INDEX:
    {
      unsigned long effect_index;
      
      effect_index = (unsigned long) g_value_get_uint(value);

      if(effect_index == live_vst3_bridge->effect_index){
	return;
      }

      live_vst3_bridge->effect_index = effect_index;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_live_vst3_bridge_get_property(GObject *gobject,
				  guint prop_id,
				  GValue *value,
				  GParamSpec *param_spec)
{
  AgsLiveVst3Bridge *live_vst3_bridge;

  live_vst3_bridge = AGS_LIVE_VST3_BRIDGE(gobject);

  switch(prop_id){
  case PROP_FILENAME:
    {
      g_value_set_string(value, live_vst3_bridge->filename);
    }
    break;
  case PROP_EFFECT:
    {
      g_value_set_string(value, live_vst3_bridge->effect);
    }
    break;
  case PROP_INDEX:
    {
      g_value_set_uint(value, (guint) live_vst3_bridge->effect_index);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_live_vst3_bridge_finalize(GObject *gobject)
{
  AgsLiveVst3Bridge *live_vst3_bridge;

  live_vst3_bridge = (AgsLiveVst3Bridge *) gobject;
  
  g_object_disconnect(G_OBJECT(live_vst3_bridge),
		      "any_signal::resize-audio-channels",
		      G_CALLBACK(ags_live_vst3_bridge_resize_audio_channels),
		      NULL,
		      "any_signal::resize-pads",
		      G_CALLBACK(ags_live_vst3_bridge_resize_pads),
		      NULL,
		      NULL);

  g_free(live_vst3_bridge->filename);
  g_free(live_vst3_bridge->effect);
  
  /* call parent */
  G_OBJECT_CLASS(ags_live_vst3_bridge_parent_class)->finalize(gobject);
}

void
ags_live_vst3_bridge_connect(AgsConnectable *connectable)
{
  AgsLiveVst3Bridge *live_vst3_bridge;

  if((AGS_MACHINE_CONNECTED & (AGS_MACHINE(connectable)->flags)) != 0){
    return;
  }

  ags_live_vst3_bridge_parent_connectable_interface->connect(connectable);

  live_vst3_bridge = AGS_LIVE_VST3_BRIDGE(connectable);

  g_signal_connect_after(G_OBJECT(live_vst3_bridge->program), "changed",
			 G_CALLBACK(ags_live_vst3_bridge_program_changed_callback), live_vst3_bridge);
}

void
ags_live_vst3_bridge_disconnect(AgsConnectable *connectable)
{
  AgsLiveVst3Bridge *live_vst3_bridge;

  if((AGS_MACHINE_CONNECTED & (AGS_MACHINE(connectable)->flags)) == 0){
    return;
  }

  ags_live_vst3_bridge_parent_connectable_interface->connect(connectable);

  live_vst3_bridge = AGS_LIVE_VST3_BRIDGE(connectable);

  g_object_disconnect(G_OBJECT(live_vst3_bridge->program),
		      "any_signal::changed",
		      G_CALLBACK(ags_live_vst3_bridge_program_changed_callback),
		      live_vst3_bridge,
		      NULL);
}

void
ags_live_vst3_bridge_resize_audio_channels(AgsMachine *machine,
					   guint audio_channels, guint audio_channels_old,
					   gpointer data)
{
  AgsLiveVst3Bridge *live_vst3_bridge;
  
  live_vst3_bridge = (AgsLiveVst3Bridge *) machine;

  if(audio_channels > audio_channels_old){
    /* recall */
    if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0){
      ags_live_vst3_bridge_input_map_recall(live_vst3_bridge,
					    audio_channels_old,
					    0);

      ags_live_vst3_bridge_output_map_recall(live_vst3_bridge,
					     audio_channels_old,
					     0);
    }
  }
}

void
ags_live_vst3_bridge_resize_pads(AgsMachine *machine, GType channel_type,
				 guint pads, guint pads_old,
				 gpointer data)
{
  AgsLiveVst3Bridge *live_vst3_bridge;

  gboolean grow;

  live_vst3_bridge = (AgsLiveVst3Bridge *) machine;

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
	ags_live_vst3_bridge_input_map_recall(live_vst3_bridge,
					      0,
					      pads_old);
      }
    }else{
      live_vst3_bridge->mapped_input_pad = pads;
    }
  }else{
    if(grow){
      /* recall */
      if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0){
	ags_live_vst3_bridge_output_map_recall(live_vst3_bridge,
					       0,
					       pads_old);
      }
    }else{
      live_vst3_bridge->mapped_output_pad = pads;
    }
  }
}

void
ags_live_vst3_bridge_map_recall(AgsMachine *machine)
{  
  AgsNavigation *navigation;
  AgsLiveVst3Bridge *live_vst3_bridge;
  
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

  live_vst3_bridge = (AgsLiveVst3Bridge *) machine;

  audio = machine->audio;

  position = 0;

  /* add new controls */
  ags_effect_bulk_add_plugin(AGS_EFFECT_BULK(AGS_EFFECT_BRIDGE(machine->bridge)->bulk_input),
			     NULL,
			     live_vst3_bridge->vst3_play_container, live_vst3_bridge->vst3_recall_container,
			     "ags-fx-vst3",
			     live_vst3_bridge->filename,
			     live_vst3_bridge->effect,
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
				       live_vst3_bridge->envelope_play_container, live_vst3_bridge->envelope_recall_container,
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
				       live_vst3_bridge->buffer_play_container, live_vst3_bridge->buffer_recall_container,
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
  ags_live_vst3_bridge_input_map_recall(live_vst3_bridge,
					0,
					0);

  /* depending on destination */
  ags_live_vst3_bridge_output_map_recall(live_vst3_bridge,
					 0,
					 0);

  /* call parent */
  AGS_MACHINE_CLASS(ags_live_vst3_bridge_parent_class)->map_recall(machine);
}

void
ags_live_vst3_bridge_input_map_recall(AgsLiveVst3Bridge *live_vst3_bridge,
				      guint audio_channel_start,
				      guint input_pad_start)
{
  AgsAudio *audio;

  GList *start_recall;

  gint position;
  guint input_pads;
  guint audio_channels;

  if(live_vst3_bridge->mapped_input_pad > input_pad_start){
    return;
  }

  audio = AGS_MACHINE(live_vst3_bridge)->audio;

  position = 0;

  input_pads = 0;
  audio_channels = 0;

  /* get some fields */
  g_object_get(audio,
	       "input-pads", &input_pads,
	       "audio-channels", &audio_channels,
	       NULL);

  /* add to effect bridge */
  ags_effect_bulk_add_plugin((AgsEffectBulk *) AGS_EFFECT_BRIDGE(AGS_MACHINE(live_vst3_bridge)->bridge)->bulk_input,
			     NULL,
			     live_vst3_bridge->vst3_play_container, live_vst3_bridge->vst3_recall_container,
			     "ags-fx-vst3",
			     live_vst3_bridge->filename,
			     live_vst3_bridge->effect,
			     audio_channel_start, audio_channels,
			     input_pad_start, input_pads,
			     position,
			     (AGS_FX_FACTORY_LIVE | AGS_FX_FACTORY_REMAP | AGS_FX_FACTORY_INPUT), 0);

  /* source */
  if((AGS_MACHINE_IS_SYNTHESIZER & (AGS_MACHINE(live_vst3_bridge)->flags)) != 0){
    /* ags-fx-envelope */
    start_recall = ags_fx_factory_create(audio,
					 live_vst3_bridge->envelope_play_container, live_vst3_bridge->envelope_recall_container,
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
					 live_vst3_bridge->buffer_play_container, live_vst3_bridge->buffer_recall_container,
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

  live_vst3_bridge->mapped_input_pad = input_pads;
}

void
ags_live_vst3_bridge_output_map_recall(AgsLiveVst3Bridge *live_vst3_bridge,
				       guint audio_channel_start,
				       guint output_pad_start)
{
  AgsAudio *audio;

  guint output_pads;
  
  if(live_vst3_bridge->mapped_output_pad > output_pad_start){
    return;
  }

  audio = AGS_MACHINE(live_vst3_bridge)->audio;

  output_pads = 0;

  /* get some fields */
  g_object_get(audio,
	       "output-pads", &output_pads,
	       NULL);

  live_vst3_bridge->mapped_output_pad = output_pads;
}

void
ags_live_vst3_bridge_load(AgsLiveVst3Bridge *live_vst3_bridge)
{
  GtkListStore *model;

  GtkTreeIter iter;

  AgsVst3Plugin *vst3_plugin;

  AgsVstIUnitInfo *iunit_info;    

  GList *start_program, *program;

  guint i, i_stop;
  guint j, j_stop;
  AgsVstTResult val;
  
  /* load plugin */
  vst3_plugin = ags_vst3_manager_find_vst3_plugin(ags_vst3_manager_get_instance(),
						  live_vst3_bridge->filename,
						  live_vst3_bridge->effect);

  if(vst3_plugin == NULL){
    return;
  }

  /*  */
  gtk_list_store_clear(GTK_LIST_STORE(gtk_combo_box_get_model(GTK_COMBO_BOX(live_vst3_bridge->program))));

  iunit_info = NULL;
  val = ags_vst_funknown_query_interface(vst3_plugin->iedit_controller,
					 ags_vst_iunit_info_get_iid(), &iunit_info);

  if(iunit_info == NULL){
    return;
  }

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

  gtk_combo_box_set_model(GTK_COMBO_BOX(live_vst3_bridge->program),
			  GTK_TREE_MODEL(model));
}

/**
 * ags_live_vst3_bridge_new:
 * @soundcard: the assigned soundcard.
 * @filename: the plugin.so
 * @effect: the effect
 *
 * Create a new instance of #AgsLiveVst3Bridge
 *
 * Returns: the new #AgsLiveVst3Bridge
 *
 * Since: 3.10.5
 */
AgsLiveVst3Bridge*
ags_live_vst3_bridge_new(GObject *soundcard,
			 gchar *filename,
			 gchar *effect)
{
  AgsLiveVst3Bridge *live_vst3_bridge;

  live_vst3_bridge = (AgsLiveVst3Bridge *) g_object_new(AGS_TYPE_LIVE_VST3_BRIDGE,
							NULL);

  if(soundcard != NULL){
    g_object_set(G_OBJECT(AGS_MACHINE(live_vst3_bridge)->audio),
		 "output-soundcard", soundcard,
		 NULL);
  }

  g_object_set(live_vst3_bridge,
	       "filename", filename,
	       "effect", effect,
	       NULL);

  return(live_vst3_bridge);
}