/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2024 Joël Krähemann
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

#include <ags/app/machine/ags_stargazer_synth.h>
#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_window.h>
#include <ags/app/ags_composite_editor.h>
#include <ags/app/ags_machine.h>
#include <ags/app/ags_pad.h>
#include <ags/app/ags_line.h>

#include <math.h>

#include <ags/i18n.h>

void ags_stargazer_synth_class_init(AgsStargazerSynthClass *stargazer_synth);
void ags_stargazer_synth_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_stargazer_synth_init(AgsStargazerSynth *stargazer_synth);
void ags_stargazer_synth_finalize(GObject *gobject);

void ags_stargazer_synth_connect(AgsConnectable *connectable);
void ags_stargazer_synth_disconnect(AgsConnectable *connectable);

void ags_stargazer_synth_show(GtkWidget *widget);

void ags_stargazer_synth_resize_audio_channels(AgsMachine *machine,
					       guint audio_channels, guint audio_channels_old,
					       gpointer data);
void ags_stargazer_synth_resize_pads(AgsMachine *machine, GType channel_type,
				     guint pads, guint pads_old,
				     gpointer data);

void ags_stargazer_synth_map_recall(AgsMachine *machine);

void ags_stargazer_synth_input_map_recall(AgsStargazerSynth *stargazer_synth,
					  guint audio_channel_start,
					  guint input_pad_start);
void ags_stargazer_synth_output_map_recall(AgsStargazerSynth *stargazer_synth,
					   guint audio_channel_start,
					   guint output_pad_start);

void ags_stargazer_synth_refresh_port(AgsMachine *machine);

/**
 * SECTION:ags_stargazer_synth
 * @short_description: stargazer synth
 * @title: AgsStargazerSynth
 * @section_id:
 * @include: ags/app/machine/ags_stargazer_synth.h
 *
 * The #AgsStargazerSynth is a composite widget to act as stargazer synth.
 */

static gpointer ags_stargazer_synth_parent_class = NULL;
static AgsConnectableInterface *ags_stargazer_synth_parent_connectable_interface;

GType
ags_stargazer_synth_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_stargazer_synth = 0;

    static const GTypeInfo ags_stargazer_synth_info = {
      sizeof(AgsStargazerSynthClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_stargazer_synth_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsStargazerSynth),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_stargazer_synth_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_stargazer_synth_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    ags_type_stargazer_synth = g_type_register_static(AGS_TYPE_MACHINE,
						      "AgsStargazerSynth", &ags_stargazer_synth_info,
						      0);
    
    g_type_add_interface_static(ags_type_stargazer_synth,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_stargazer_synth);
  }

  return g_define_type_id__volatile;
}

void
ags_stargazer_synth_class_init(AgsStargazerSynthClass *stargazer_synth)
{
  GObjectClass *gobject;
  AgsMachineClass *machine;

  ags_stargazer_synth_parent_class = g_type_class_peek_parent(stargazer_synth);

  /* GObjectClass */
  gobject = (GObjectClass *) stargazer_synth;

  gobject->finalize = ags_stargazer_synth_finalize;

  /* AgsMachineClass */
  machine = (AgsMachineClass *) stargazer_synth;

  machine->map_recall = ags_stargazer_synth_map_recall;

  machine->refresh_port = ags_stargazer_synth_refresh_port;
}

void
ags_stargazer_synth_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_stargazer_synth_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_stargazer_synth_connect;
  connectable->disconnect = ags_stargazer_synth_disconnect;
}

void
ags_stargazer_synth_init(AgsStargazerSynth *stargazer_synth)
{
  AgsWindow *window;
  AgsCompositeEditor *composite_editor;
  GtkBox *vbox;
  GtkBox *hbox;
  GtkBox *osc_vbox;
  GtkGrid *synth_0_grid;
  GtkGrid *synth_1_grid;
  GtkGrid *misc_grid;
  GtkBox *pitch_type_hbox;
  GtkBox *band_hbox;
  GtkGrid *low_pass_grid;
  GtkGrid *high_pass_grid;
  GtkGrid *chorus_grid;
  GtkLabel *label;

  GtkAdjustment *adjustment;
  
  AgsAudio *audio;

  AgsMachineCounterManager *machine_counter_manager;
  AgsMachineCounter *machine_counter;

  AgsApplicationContext *application_context;   
  
  gchar *machine_name;

  gint position;
  gdouble gui_scale_factor;

  const gchar* pitch_type_strv[] = {
    "fluid-interpolate-none",
    "fluid-interpolate-linear",
    "fluid-interpolate-4th-order",
    "fluid-interpolate-7th-order",
    "ags-pitch-2x-alias",    
    "ags-pitch-4x-alias",    
    "ags-pitch-16x-alias",
    NULL,
  };

  application_context = ags_application_context_get_instance();
  
  /* machine counter */
  machine_counter_manager = ags_machine_counter_manager_get_instance();

  machine_counter = ags_machine_counter_manager_find_machine_counter(machine_counter_manager,
								     AGS_TYPE_STARGAZER_SYNTH);

  machine_name = NULL;

  if(machine_counter != NULL){
    machine_name = g_strdup_printf("Default %d",
				   machine_counter->counter);
  
    ags_machine_counter_increment(machine_counter);
  }
  
  g_object_set(stargazer_synth,
	       "machine-name", machine_name,
	       NULL);

  g_free(machine_name);

  /* machine selector */
  window = (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));

  composite_editor = (AgsCompositeEditor *) ags_ui_provider_get_composite_editor(AGS_UI_PROVIDER(application_context));

  position = g_list_length(window->machine);
  
  ags_machine_selector_popup_insert_machine(composite_editor->machine_selector,
					    position,
					    (AgsMachine *) stargazer_synth);

  /* scale factor */
  gui_scale_factor = ags_ui_provider_get_gui_scale_factor(AGS_UI_PROVIDER(application_context));
  
  audio = AGS_MACHINE(stargazer_synth)->audio;
  ags_audio_set_flags(audio, (AGS_AUDIO_SYNC |
			      AGS_AUDIO_ASYNC |
			      AGS_AUDIO_OUTPUT_HAS_RECYCLING |
			      AGS_AUDIO_INPUT_HAS_RECYCLING |
			      AGS_AUDIO_INPUT_HAS_SYNTH));
  ags_audio_set_ability_flags(audio, (AGS_SOUND_ABILITY_PLAYBACK |
				      AGS_SOUND_ABILITY_NOTATION));
  ags_audio_set_behaviour_flags(audio, (AGS_SOUND_BEHAVIOUR_REVERSE_MAPPING |
					AGS_SOUND_BEHAVIOUR_DEFAULTS_TO_INPUT));
  g_object_set(audio,
	       "min-audio-channels", 1,
	       "max-audio-channels", 1,
	       "min-output-pads", 1,
	       "min-input-pads", 1,
	       "max-input-pads", 128,
	       "audio-start-mapping", 0,
	       "audio-end-mapping", 128,
	       "midi-start-mapping", 0,
	       "midi-end-mapping", 128,
	       NULL);

  AGS_MACHINE(stargazer_synth)->flags |= (AGS_MACHINE_IS_SYNTHESIZER |
					  AGS_MACHINE_REVERSE_NOTATION);
  AGS_MACHINE(stargazer_synth)->mapping_flags |= AGS_MACHINE_MONO;

  AGS_MACHINE(stargazer_synth)->input_pad_type = G_TYPE_NONE;
  AGS_MACHINE(stargazer_synth)->input_line_type = G_TYPE_NONE;
  AGS_MACHINE(stargazer_synth)->output_pad_type = G_TYPE_NONE;
  AGS_MACHINE(stargazer_synth)->output_line_type = G_TYPE_NONE;

  /* audio resize */
  g_signal_connect_after(G_OBJECT(stargazer_synth), "resize-audio-channels",
			 G_CALLBACK(ags_stargazer_synth_resize_audio_channels), NULL);

  g_signal_connect_after(G_OBJECT(stargazer_synth), "resize-pads",
			 G_CALLBACK(ags_stargazer_synth_resize_pads), NULL);
  
  /* create widgets */
  stargazer_synth->flags = 0;

  /* mapped IO */
  stargazer_synth->mapped_input_pad = 0;
  stargazer_synth->mapped_output_pad = 0;

  stargazer_synth->playback_play_container = ags_recall_container_new();
  stargazer_synth->playback_recall_container = ags_recall_container_new();

  stargazer_synth->synth_play_container = ags_recall_container_new();
  stargazer_synth->synth_recall_container = ags_recall_container_new();

  stargazer_synth->volume_play_container = ags_recall_container_new();
  stargazer_synth->volume_recall_container = ags_recall_container_new();

  stargazer_synth->envelope_play_container = ags_recall_container_new();
  stargazer_synth->envelope_recall_container = ags_recall_container_new();

  stargazer_synth->buffer_play_container = ags_recall_container_new();
  stargazer_synth->buffer_recall_container = ags_recall_container_new();
   
  /* name and xml type */
  stargazer_synth->name = NULL;
  stargazer_synth->xml_type = "ags-stargazer-synth"; 
}

void
ags_stargazer_synth_finalize(GObject *gobject)
{
  /* call parent */
  G_OBJECT_CLASS(ags_stargazer_synth_parent_class)->finalize(gobject);
}

void
ags_stargazer_synth_connect(AgsConnectable *connectable)
{
  AgsStargazerSynth *stargazer_synth;
  
  if((AGS_CONNECTABLE_CONNECTED & (AGS_MACHINE(connectable)->connectable_flags)) != 0){
    return;
  }

  ags_stargazer_synth_parent_connectable_interface->connect(connectable);
  
  /* AgsStargazerSynth */
  stargazer_synth = AGS_STARGAZER_SYNTH(connectable);
}

void
ags_stargazer_synth_disconnect(AgsConnectable *connectable)
{
  AgsStargazerSynth *stargazer_synth;
  
  if((AGS_CONNECTABLE_CONNECTED & (AGS_MACHINE(connectable)->connectable_flags)) == 0){
    return;
  }

  ags_stargazer_synth_parent_connectable_interface->disconnect(connectable);

  /* AgsStargazerSynth */
  stargazer_synth = AGS_STARGAZER_SYNTH(connectable);
}

void
ags_stargazer_synth_resize_audio_channels(AgsMachine *machine,
					  guint audio_channels, guint audio_channels_old,
					  gpointer data)
{
  AgsStargazerSynth *stargazer_synth;

  stargazer_synth = (AgsStargazerSynth *) machine;

  if(audio_channels > audio_channels_old){    
    /* recall */
    if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0){
      ags_stargazer_synth_input_map_recall(stargazer_synth,
					   audio_channels_old,
					   0);
      
      ags_stargazer_synth_output_map_recall(stargazer_synth,
					    audio_channels_old,
					    0);
    }
  }
}

void
ags_stargazer_synth_resize_pads(AgsMachine *machine, GType type,
				guint pads, guint pads_old,
				gpointer data)
{
  AgsStargazerSynth *stargazer_synth;

  gboolean grow;

  stargazer_synth = (AgsStargazerSynth *) machine;
  
  if(pads_old < pads){
    grow = TRUE;
  }else{
    grow = FALSE;
  }
  
  if(type == AGS_TYPE_INPUT){
    if(grow){
      if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0){
	/* depending on destination */
	ags_stargazer_synth_input_map_recall(stargazer_synth,
					     0,
					     pads_old);
      }
    }else{
      stargazer_synth->mapped_input_pad = pads;
    }
  }else{
    if(grow){
      if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0){
	ags_stargazer_synth_output_map_recall(stargazer_synth,
					      0,
					      pads_old);
      }
    }else{
      stargazer_synth->mapped_output_pad = pads;
    }
  }
}

void
ags_stargazer_synth_map_recall(AgsMachine *machine)
{
  AgsStargazerSynth *stargazer_synth;
  
  AgsAudio *audio;

  GList *start_recall, *recall;

  gint position;

  if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0 ||
     (AGS_MACHINE_PREMAPPED_RECALL & (machine->flags)) != 0){
    return;
  }

  stargazer_synth = AGS_STARGAZER_SYNTH(machine);

  audio = machine->audio;
  
  position = 0;

  /* ags-fx-playback */
  start_recall = ags_fx_factory_create(audio,
				       stargazer_synth->playback_play_container, stargazer_synth->playback_recall_container,
				       "ags-fx-playback",
				       NULL,
				       NULL,
				       0, 0,
				       0, 0,
				       position,
				       (AGS_FX_FACTORY_ADD | AGS_FX_FACTORY_INPUT),
				       0);

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);

  /* ags-fx-synth */
  start_recall = ags_fx_factory_create(audio,
				       stargazer_synth->synth_play_container, stargazer_synth->synth_recall_container,
				       "ags-fx-stargazer-synth",
				       NULL,
				       NULL,
				       0, 0,
				       0, 0,
				       position,
				       (AGS_FX_FACTORY_ADD | AGS_FX_FACTORY_INPUT),
				       0);

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);

  /* ags-fx-volume */
  start_recall = ags_fx_factory_create(audio,
				       stargazer_synth->volume_play_container, stargazer_synth->volume_recall_container,
				       "ags-fx-volume",
				       NULL,
				       NULL,
				       0, 0,
				       0, 0,
				       position,
				       (AGS_FX_FACTORY_ADD | AGS_FX_FACTORY_INPUT),
				       0);

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
  

  /* ags-fx-envelope */
  start_recall = ags_fx_factory_create(audio,
				       stargazer_synth->envelope_play_container, stargazer_synth->envelope_recall_container,
				       "ags-fx-envelope",
				       NULL,
				       NULL,
				       0, 0,
				       0, 0,
				       position,
				       (AGS_FX_FACTORY_ADD | AGS_FX_FACTORY_INPUT),
				       0);

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);

  /* ags-fx-buffer */
  start_recall = ags_fx_factory_create(audio,
				       stargazer_synth->buffer_play_container, stargazer_synth->buffer_recall_container,
				       "ags-fx-buffer",
				       NULL,
				       NULL,
				       0, 0,
				       0, 0,
				       position,
				       (AGS_FX_FACTORY_ADD | AGS_FX_FACTORY_INPUT),
				       0);

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);

  /* depending on destination */
  ags_stargazer_synth_input_map_recall(stargazer_synth,
				       0,
				       0);

  /* depending on destination */
  ags_stargazer_synth_output_map_recall(stargazer_synth,
					0,
					0);

  /* call parent */
  AGS_MACHINE_CLASS(ags_stargazer_synth_parent_class)->map_recall(machine);  
}

void
ags_stargazer_synth_input_map_recall(AgsStargazerSynth *stargazer_synth,
				     guint audio_channel_start,
				     guint input_pad_start)
{
  AgsAudio *audio;

  GList *start_recall;

  gint position;
  guint input_pads;
  guint audio_channels;

  if(stargazer_synth->mapped_input_pad > input_pad_start){
    return;
  }

  audio = AGS_MACHINE(stargazer_synth)->audio;

  position = 0;
  
  input_pads = 0;
  audio_channels = 0;

  /* get some fields */
  g_object_get(audio,
	       "input-pads", &input_pads,
	       "audio-channels", &audio_channels,
	       NULL);
  
  /* ags-fx-playback */
  start_recall = ags_fx_factory_create(audio,
				       stargazer_synth->playback_play_container, stargazer_synth->playback_recall_container,
				       "ags-fx-playback",
				       NULL,
				       NULL,
				       audio_channel_start, audio_channels,
				       input_pad_start, input_pads,
				       position,
				       (AGS_FX_FACTORY_REMAP | AGS_FX_FACTORY_INPUT), 0);

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);

  /* ags-fx-synth */
  start_recall = ags_fx_factory_create(audio,
				       stargazer_synth->synth_play_container, stargazer_synth->synth_recall_container,
				       "ags-fx-stargazer-synth",
				       NULL,
				       NULL,
				       audio_channel_start, audio_channels,
				       input_pad_start, input_pads,
				       position,
				       (AGS_FX_FACTORY_REMAP | AGS_FX_FACTORY_INPUT), 0);

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);

  /* ags-fx-volume */
  start_recall = ags_fx_factory_create(audio,
				       stargazer_synth->volume_play_container, stargazer_synth->volume_recall_container,
				       "ags-fx-volume",
				       NULL,
				       NULL,
				       audio_channel_start, audio_channels,
				       input_pad_start, input_pads,
				       position,
				       (AGS_FX_FACTORY_REMAP | AGS_FX_FACTORY_INPUT), 0);

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);

  /* ags-fx-envelope */
  start_recall = ags_fx_factory_create(audio,
				       stargazer_synth->envelope_play_container, stargazer_synth->envelope_recall_container,
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
				       stargazer_synth->buffer_play_container, stargazer_synth->buffer_recall_container,
				       "ags-fx-buffer",
				       NULL,
				       NULL,
				       audio_channel_start, audio_channels,
				       input_pad_start, input_pads,
				       position,
				       (AGS_FX_FACTORY_REMAP | AGS_FX_FACTORY_INPUT), 0);

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);

  stargazer_synth->mapped_input_pad = input_pads;
}

void
ags_stargazer_synth_output_map_recall(AgsStargazerSynth *stargazer_synth,
				      guint audio_channel_start,
				      guint output_pad_start)
{
  AgsAudio *audio;

  guint output_pads;

  if(stargazer_synth->mapped_output_pad > output_pad_start){
    return;
  }

  audio = AGS_MACHINE(stargazer_synth)->audio;

  /* get some fields */
  g_object_get(audio,
	       "output-pads", &output_pads,
	       NULL);
  
  stargazer_synth->mapped_output_pad = output_pads;
}

void
ags_stargazer_synth_refresh_port(AgsMachine *machine)
{
  AgsStargazerSynth *stargazer_synth;
  
  GList *start_play, *start_recall, *recall;

  stargazer_synth = (AgsStargazerSynth *) machine;
  
  start_play = ags_audio_get_play(machine->audio);
  start_recall = ags_audio_get_recall(machine->audio);

  recall =
    start_recall = g_list_concat(start_play, start_recall);

  machine->flags |= AGS_MACHINE_NO_UPDATE;

  machine->flags &= (~AGS_MACHINE_NO_UPDATE);
}

/**
 * ags_stargazer_synth_new:
 * @soundcard: the assigned soundcard.
 *
 * Create a new instance of #AgsStargazerSynth
 *
 * Returns: the new #AgsStargazerSynth
 *
 * Since: 3.14.0
 */
AgsStargazerSynth*
ags_stargazer_synth_new(GObject *soundcard)
{
  AgsStargazerSynth *stargazer_synth;

  stargazer_synth = (AgsStargazerSynth *) g_object_new(AGS_TYPE_STARGAZER_SYNTH,
						       NULL);

  g_object_set(AGS_MACHINE(stargazer_synth)->audio,
	       "output-soundcard", soundcard,
	       NULL);

  return(stargazer_synth);
}
