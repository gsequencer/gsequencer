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

#include <ags/X/machine/ags_vst3_bridge.h>
#include <ags/X/machine/ags_vst3_bridge_callbacks.h>

#include <ags/X/ags_ui_provider.h>
#include <ags/X/ags_window.h>
#include <ags/X/ags_navigation.h>
#include <ags/X/ags_effect_bridge.h>
#include <ags/X/ags_effect_bulk.h>
#include <ags/X/ags_bulk_member.h>

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
 * @include: ags/X/ags_vst3_bridge.h
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

  AgsAudio *audio;

  g_signal_connect_after((GObject *) vst3_bridge, "parent-set",
			 G_CALLBACK(ags_vst3_bridge_parent_set_callback), (gpointer) vst3_bridge);

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

  ags_machine_popup_add_edit_options((AgsMachine *) vst3_bridge,
				     (AGS_MACHINE_POPUP_ENVELOPE));
				     
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
  gtk_box_pack_start(vbox,
		     (GtkWidget *) hbox,
		     FALSE, FALSE,
		     0);

  label = (GtkLabel *) gtk_label_new(i18n("program"));
  gtk_box_pack_start(hbox,
		     (GtkWidget *) label,
		     FALSE, FALSE,
		     0);

  vst3_bridge->program = (GtkComboBoxText *) gtk_combo_box_text_new();
  gtk_box_pack_start(hbox,
		     (GtkWidget *) vst3_bridge->program,
		     FALSE, FALSE,
		     0);

  /* effect bridge */
  AGS_MACHINE(vst3_bridge)->bridge = (GtkContainer *) ags_effect_bridge_new(audio);
  gtk_box_pack_start(vbox,
		     (GtkWidget *) AGS_MACHINE(vst3_bridge)->bridge,
		     FALSE, FALSE,
		     0);
  
  grid = (GtkGrid *) gtk_grid_new();
  gtk_box_pack_start((GtkBox *) AGS_EFFECT_BRIDGE(AGS_MACHINE(vst3_bridge)->bridge),
		     (GtkWidget *) grid,
		     FALSE, FALSE,
		     0);

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

	  window = (AgsWindow *) gtk_widget_get_toplevel((GtkWidget *) vst3_bridge);

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

  if((AGS_MACHINE_CONNECTED & (AGS_MACHINE(connectable)->flags)) != 0){
    return;
  }

  ags_vst3_bridge_parent_connectable_interface->connect(connectable);

  vst3_bridge = AGS_VST3_BRIDGE(connectable);

  g_signal_connect_after(G_OBJECT(vst3_bridge->program), "changed",
			 G_CALLBACK(ags_vst3_bridge_program_changed_callback), vst3_bridge);
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
  //TODO:JK: implement me
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
