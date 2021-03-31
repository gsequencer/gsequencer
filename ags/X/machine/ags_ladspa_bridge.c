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

#include <ags/X/machine/ags_ladspa_bridge.h>
#include <ags/X/machine/ags_ladspa_bridge_callbacks.h>

#include <ags/X/ags_ui_provider.h>
#include <ags/X/ags_window.h>
#include <ags/X/ags_effect_bridge.h>
#include <ags/X/ags_effect_bulk.h>
#include <ags/X/ags_bulk_member.h>

#include <ags/i18n.h>

void ags_ladspa_bridge_class_init(AgsLadspaBridgeClass *ladspa_bridge);
void ags_ladspa_bridge_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_ladspa_bridge_init(AgsLadspaBridge *ladspa_bridge);
void ags_ladspa_bridge_set_property(GObject *gobject,
				    guint prop_id,
				    const GValue *value,
				    GParamSpec *param_spec);
void ags_ladspa_bridge_get_property(GObject *gobject,
				    guint prop_id,
				    GValue *value,
				    GParamSpec *param_spec);
void ags_ladspa_bridge_finalize(GObject *gobject);

void ags_ladspa_bridge_connect(AgsConnectable *connectable);
void ags_ladspa_bridge_disconnect(AgsConnectable *connectable);

void ags_ladspa_bridge_resize_audio_channels(AgsMachine *machine,
					     guint audio_channels, guint audio_channels_old,
					     gpointer data);
void ags_ladspa_bridge_resize_pads(AgsMachine *machine, GType channel_type,
				   guint pads, guint pads_old,
				   gpointer data);

void ags_ladspa_bridge_map_recall(AgsMachine *machine);


/**
 * SECTION:ags_ladspa_bridge
 * @short_description: A composite widget to visualize a bunch of #AgsChannel
 * @title: AgsLadspaBridge
 * @section_id:
 * @include: ags/X/machine/ags_ladspa_bridge.h
 *
 * #AgsLadspaBridge is a composite widget to visualize all #AgsChannel. It should be
 * packed by an #AgsMachine.
 */

enum{
  PROP_0,
  PROP_FILENAME,
  PROP_EFFECT,
  PROP_INDEX,
};

static gpointer ags_ladspa_bridge_parent_class = NULL;
static AgsConnectableInterface* ags_ladspa_bridge_parent_connectable_interface;

GType
ags_ladspa_bridge_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_ladspa_bridge = 0;

    static const GTypeInfo ags_ladspa_bridge_info = {
      sizeof(AgsLadspaBridgeClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_ladspa_bridge_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsLadspaBridge),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_ladspa_bridge_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_ladspa_bridge_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_ladspa_bridge = g_type_register_static(AGS_TYPE_MACHINE,
						    "AgsLadspaBridge", &ags_ladspa_bridge_info,
						    0);

    g_type_add_interface_static(ags_type_ladspa_bridge,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_ladspa_bridge);
  }

  return g_define_type_id__volatile;
}

void
ags_ladspa_bridge_class_init(AgsLadspaBridgeClass *ladspa_bridge)
{
  AgsMachineClass *machine;
  GObjectClass *gobject;

  GParamSpec *param_spec;

  ags_ladspa_bridge_parent_class = g_type_class_peek_parent(ladspa_bridge);

  /* GObjectClass */
  gobject = G_OBJECT_CLASS(ladspa_bridge);

  gobject->set_property = ags_ladspa_bridge_set_property;
  gobject->get_property = ags_ladspa_bridge_get_property;

  gobject->finalize = ags_ladspa_bridge_finalize;
  
  /* properties */
  /**
   * AgsRecallLadspa:filename:
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
   * AgsRecallLadspa:effect:
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
   * AgsRecallLadspa:index:
   *
   * The effect's index.
   * 
   * Since: 3.0.0
   */
  param_spec =  g_param_spec_ulong("index",
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
  machine = (AgsMachineClass *) ladspa_bridge;

  machine->map_recall = ags_ladspa_bridge_map_recall;
}

void
ags_ladspa_bridge_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_ladspa_bridge_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_ladspa_bridge_connect;
  connectable->disconnect = ags_ladspa_bridge_disconnect;
}

void
ags_ladspa_bridge_init(AgsLadspaBridge *ladspa_bridge)
{
  GtkGrid *grid;

  AgsAudio *audio;

  g_signal_connect_after((GObject *) ladspa_bridge, "parent-set",
			 G_CALLBACK(ags_ladspa_bridge_parent_set_callback), (gpointer) ladspa_bridge);

  audio = AGS_MACHINE(ladspa_bridge)->audio;
  ags_audio_set_flags(audio, (AGS_AUDIO_SYNC));
  g_object_set(audio,
	       "min-audio-channels", 1,
	       "min-output-pads", 1,
	       "min-input-pads", 1,
	       NULL);

  g_signal_connect_after(G_OBJECT(ladspa_bridge), "resize-audio-channels",
			 G_CALLBACK(ags_ladspa_bridge_resize_audio_channels), NULL);
  
  g_signal_connect_after(G_OBJECT(ladspa_bridge), "resize-pads",
			 G_CALLBACK(ags_ladspa_bridge_resize_pads), NULL);

  ladspa_bridge->flags = 0;

  ladspa_bridge->name = NULL;

  ladspa_bridge->version = AGS_LADSPA_BRIDGE_DEFAULT_VERSION;
  ladspa_bridge->build_id = AGS_LADSPA_BRIDGE_DEFAULT_BUILD_ID;

  ladspa_bridge->xml_type = "ags-ladspa-bridge";
  
  ladspa_bridge->mapped_output_pad = 0;
  ladspa_bridge->mapped_input_pad = 0;

  ladspa_bridge->ladspa_play_container = ags_recall_container_new();
  ladspa_bridge->ladspa_recall_container = ags_recall_container_new();
  
  ladspa_bridge->filename = NULL;
  ladspa_bridge->effect = NULL;
  ladspa_bridge->effect_index = 0;

  AGS_MACHINE(ladspa_bridge)->bridge = (GtkContainer *) ags_effect_bridge_new(audio);
  gtk_container_add((GtkContainer *) gtk_bin_get_child((GtkBin *) ladspa_bridge),
		    (GtkWidget *) AGS_MACHINE(ladspa_bridge)->bridge);

  grid = (GtkGrid *) gtk_grid_new();
  gtk_box_pack_start((GtkBox *) AGS_EFFECT_BRIDGE(AGS_MACHINE(ladspa_bridge)->bridge),
		     (GtkWidget *) grid,
		     FALSE, FALSE,
		     0);

  AGS_EFFECT_BRIDGE(AGS_MACHINE(ladspa_bridge)->bridge)->bulk_input = (GtkWidget *) ags_effect_bulk_new(audio,
													AGS_TYPE_INPUT);
  AGS_EFFECT_BULK(AGS_EFFECT_BRIDGE(AGS_MACHINE(ladspa_bridge)->bridge)->bulk_input)->flags |= (AGS_EFFECT_BULK_HIDE_BUTTONS |
												AGS_EFFECT_BULK_HIDE_ENTRIES |
												AGS_EFFECT_BULK_SHOW_LABELS);

  gtk_widget_set_valign((GtkWidget *) AGS_EFFECT_BRIDGE(AGS_MACHINE(ladspa_bridge)->bridge)->bulk_input,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) AGS_EFFECT_BRIDGE(AGS_MACHINE(ladspa_bridge)->bridge)->bulk_input,
			GTK_ALIGN_FILL);
  
  gtk_grid_attach(grid,
		  (GtkWidget *) AGS_EFFECT_BRIDGE(AGS_MACHINE(ladspa_bridge)->bridge)->bulk_input,
		  0, 0,
		  1, 1);
}

void
ags_ladspa_bridge_set_property(GObject *gobject,
			       guint prop_id,
			       const GValue *value,
			       GParamSpec *param_spec)
{
  AgsLadspaBridge *ladspa_bridge;

  ladspa_bridge = AGS_LADSPA_BRIDGE(gobject);

  switch(prop_id){
  case PROP_FILENAME:
  {
    gchar *filename;

    filename = g_value_get_string(value);

    if(filename == ladspa_bridge->filename){
      return;
    }

    if(ladspa_bridge->filename != NULL){
      g_free(ladspa_bridge->filename);
    }

    if(filename != NULL){
      if(!g_file_test(filename,
		      G_FILE_TEST_EXISTS)){
	AgsWindow *window;

	window = (AgsWindow *) gtk_widget_get_toplevel((GtkWidget *) ladspa_bridge);

	ags_window_show_error(window,
			      g_strdup_printf("Plugin file not present %s",
					      filename));
      }
    }
      
    ladspa_bridge->filename = g_strdup(filename);
  }
  break;
  case PROP_EFFECT:
  {
    gchar *effect;
      
    effect = g_value_get_string(value);

    if(effect == ladspa_bridge->effect){
      return;
    }

    if(ladspa_bridge->effect != NULL){
      g_free(ladspa_bridge->effect);
    }

    ladspa_bridge->effect = g_strdup(effect);
  }
  break;
  case PROP_INDEX:
  {
    unsigned long effect_index;
      
    effect_index = g_value_get_ulong(value);

    if(effect_index == ladspa_bridge->effect_index){
      return;
    }

    ladspa_bridge->effect_index = effect_index;
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_ladspa_bridge_get_property(GObject *gobject,
			       guint prop_id,
			       GValue *value,
			       GParamSpec *param_spec)
{
  AgsLadspaBridge *ladspa_bridge;

  ladspa_bridge = AGS_LADSPA_BRIDGE(gobject);

  switch(prop_id){
  case PROP_FILENAME:
  {
    g_value_set_string(value, ladspa_bridge->filename);
  }
  break;
  case PROP_EFFECT:
  {
    g_value_set_string(value, ladspa_bridge->effect);
  }
  break;
  case PROP_INDEX:
  {
    g_value_set_ulong(value, ladspa_bridge->effect_index);
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_ladspa_bridge_finalize(GObject *gobject)
{
  AgsLadspaBridge *ladspa_bridge;

  ladspa_bridge = (AgsLadspaBridge *) gobject;
  
  g_object_disconnect(G_OBJECT(ladspa_bridge),
		      "any_signal::resize-audio-channels",
		      G_CALLBACK(ags_ladspa_bridge_resize_audio_channels),
		      NULL,
		      "any_signal::resize-pads",
		      G_CALLBACK(ags_ladspa_bridge_resize_pads),
		      NULL,
		      NULL);
  
  g_free(ladspa_bridge->filename);
  g_free(ladspa_bridge->effect);

  /* call parent */
  G_OBJECT_CLASS(ags_ladspa_bridge_parent_class)->finalize(gobject);
}

void
ags_ladspa_bridge_connect(AgsConnectable *connectable)
{
  if((AGS_MACHINE_CONNECTED & (AGS_MACHINE(connectable)->flags)) != 0){
    return;
  }

  ags_ladspa_bridge_parent_connectable_interface->connect(connectable);
}

void
ags_ladspa_bridge_disconnect(AgsConnectable *connectable)
{
  if((AGS_MACHINE_CONNECTED & (AGS_MACHINE(connectable)->flags)) == 0){
    return;
  }

  ags_ladspa_bridge_parent_connectable_interface->disconnect(connectable);
}

void
ags_ladspa_bridge_resize_audio_channels(AgsMachine *machine,
				      guint audio_channels, guint audio_channels_old,
				      gpointer data)
{
  AgsLadspaBridge *ladspa_bridge;

  AgsAudio *audio;
  
  guint output_pads, input_pads;

  ladspa_bridge = (AgsLadspaBridge *) machine;  

  audio = machine->audio;

  /* get some fields */
  g_object_get(audio,
	       "output-pads", &output_pads,
	       "input-pads", &input_pads,
	       NULL);
  
  /* check available */
  if(input_pads == 0 &&
     output_pads == 0){
    return;
  }

  if(audio_channels > audio_channels_old){
    /* recall */
    if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0){
      ags_ladspa_bridge_input_map_recall(ladspa_bridge,
				       audio_channels_old,
				       0);

      ags_ladspa_bridge_output_map_recall(ladspa_bridge,
					audio_channels_old,
					0);
    }
  }
}

void
ags_ladspa_bridge_resize_pads(AgsMachine *machine, GType type,
			    guint pads, guint pads_old,
			    gpointer data)
{
  AgsLadspaBridge *ladspa_bridge;
  
  AgsAudio *audio;
  
  guint audio_channels;
  gboolean grow;

  ladspa_bridge = (AgsLadspaBridge *) machine;

  audio = machine->audio;

  /* get some fields */
  g_object_get(audio,
	       "audio-channels", &audio_channels,
	       NULL);
  
  /* check available */
  if(pads == pads_old ||
     audio_channels == 0){
    return;
  }

  if(pads_old < pads){
    grow = TRUE;
  }else{
    grow = FALSE;
  }
  
  if(g_type_is_a(type, AGS_TYPE_INPUT)){
    if(grow){      
      /* recall */
      if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0){
	ags_ladspa_bridge_input_map_recall(ladspa_bridge,
					 0,
					 pads_old);
      }
    }else{
      ladspa_bridge->mapped_input_pad = pads;
    }
  }else{
    if(grow){
      /* recall */
      if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0){
	ags_ladspa_bridge_output_map_recall(ladspa_bridge,
					  0,
					  pads_old);
      }
    }else{
      ladspa_bridge->mapped_output_pad = pads;
    }
  }
}

void
ags_ladspa_bridge_map_recall(AgsMachine *machine)
{  
  AgsLadspaBridge *ladspa_bridge;
  
  gint position;
  
  if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0 ||
     (AGS_MACHINE_PREMAPPED_RECALL & (machine->flags)) != 0){
    return;
  }

  ladspa_bridge = (AgsLadspaBridge *) machine;

  position = 0;
  
  /* add new controls */
  ags_effect_bulk_add_plugin(AGS_EFFECT_BULK(AGS_EFFECT_BRIDGE(machine->bridge)->bulk_input),
			     NULL,
			     ladspa_bridge->ladspa_play_container, ladspa_bridge->ladspa_recall_container,
			     "ags-fx-ladspa",
			     ladspa_bridge->filename,
			     ladspa_bridge->effect,
			     0, 0,
			     0, 0,
			     position,
			     (AGS_FX_FACTORY_ADD), 0);

  /* depending on destination */
  ags_ladspa_bridge_input_map_recall(ladspa_bridge,
				     0,
				     0);

  /* depending on destination */
  ags_ladspa_bridge_output_map_recall(ladspa_bridge,
				      0,
				      0);

  /* call parent */
  AGS_MACHINE_CLASS(ags_ladspa_bridge_parent_class)->map_recall(machine);
}

void
ags_ladspa_bridge_input_map_recall(AgsLadspaBridge *ladspa_bridge,
				   guint audio_channel_start,
				   guint input_pad_start)
{
  AgsAudio *audio;

  gint position;
  guint input_pads;
  guint audio_channels;

  if(ladspa_bridge->mapped_input_pad > input_pad_start){
    return;
  }

  audio = AGS_MACHINE(ladspa_bridge)->audio;

  position = 0;

  input_pads = 0;
  audio_channels = 0;

  /* get some fields */
  g_object_get(audio,
	       "input-pads", &input_pads,
	       "audio-channels", &audio_channels,
	       NULL);

  /* add to effect bridge */
  ags_effect_bulk_add_plugin((AgsEffectBulk *) AGS_EFFECT_BRIDGE(AGS_MACHINE(ladspa_bridge)->bridge)->bulk_input,
			     NULL,
			     ladspa_bridge->ladspa_play_container, ladspa_bridge->ladspa_recall_container,
			     "ags-fx-ladspa",
			     ladspa_bridge->filename,
			     ladspa_bridge->effect,
			     audio_channel_start, audio_channels,
			     input_pad_start, input_pads,
			     position,
			     (AGS_FX_FACTORY_REMAP), 0);
  
  ladspa_bridge->mapped_input_pad = input_pads;
}

void
ags_ladspa_bridge_output_map_recall(AgsLadspaBridge *ladspa_bridge,
				    guint audio_channel_start,
				    guint output_pad_start)
{
  AgsAudio *audio;

  guint output_pads;

  if(ladspa_bridge->mapped_output_pad > output_pad_start){
    return;
  }

  audio = AGS_MACHINE(ladspa_bridge)->audio;

  /* get some fields */
  g_object_get(audio,
	       "output-pads", &output_pads,
	       NULL);

  ladspa_bridge->mapped_output_pad = output_pads;
}

/**
 * ags_ladspa_bridge_new:
 * @soundcard: the assigned soundcard.
 * @filename: the plugin.so
 * @effect: the effect
 *
 * Creates an #AgsLadspaBridge
 *
 * Returns: a new #AgsLadspaBridge
 *
 * Since: 3.0.0
 */
AgsLadspaBridge*
ags_ladspa_bridge_new(GObject *soundcard,
		      gchar *filename,
		      gchar *effect)
{
  AgsLadspaBridge *ladspa_bridge;

  ladspa_bridge = (AgsLadspaBridge *) g_object_new(AGS_TYPE_LADSPA_BRIDGE,
						   NULL);

  g_object_set(AGS_MACHINE(ladspa_bridge)->audio,
	       "output-soundcard", soundcard,
	       NULL);
  
  g_object_set(ladspa_bridge,
	       "filename", filename,
	       "effect", effect,
	       NULL);

  return(ladspa_bridge);
}
