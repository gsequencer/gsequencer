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

#include <ags/app/machine/ags_fm_syncsynth.h>
#include <ags/app/machine/ags_fm_syncsynth_callbacks.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_window.h>
#include <ags/app/ags_composite_editor.h>
#include <ags/app/ags_machine.h>
#include <ags/app/ags_pad.h>
#include <ags/app/ags_line.h>

#include <math.h>

#include <ags/i18n.h>

void ags_fm_syncsynth_class_init(AgsFMSyncsynthClass *fm_syncsynth);
void ags_fm_syncsynth_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_fm_syncsynth_init(AgsFMSyncsynth *fm_syncsynth);
void ags_fm_syncsynth_finalize(GObject *gobject);

void ags_fm_syncsynth_connect(AgsConnectable *connectable);
void ags_fm_syncsynth_disconnect(AgsConnectable *connectable);

void ags_fm_syncsynth_show(GtkWidget *widget);

void ags_fm_syncsynth_resize_audio_channels(AgsMachine *machine,
					    guint audio_channels, guint audio_channels_old,
					    gpointer data);
void ags_fm_syncsynth_resize_pads(AgsMachine *machine, GType channel_type,
				  guint pads, guint pads_old,
				  gpointer data);

void ags_fm_syncsynth_map_recall(AgsMachine *machine);

void ags_fm_syncsynth_input_map_recall(AgsFMSyncsynth *fm_syncsynth,
				       guint audio_channel_start,
				       guint input_pad_start);
void ags_fm_syncsynth_output_map_recall(AgsFMSyncsynth *fm_syncsynth,
					guint audio_channel_start,
					guint output_pad_start);

/**
 * SECTION:ags_fm_syncsynth
 * @short_description: fm syncsynth
 * @title: AgsFMSyncsynth
 * @section_id:
 * @include: ags/app/machine/ags_fm_syncsynth.h
 *
 * The #AgsFMSyncsynth is a composite widget to act as fm syncsynth.
 */

static gpointer ags_fm_syncsynth_parent_class = NULL;
static AgsConnectableInterface *ags_fm_syncsynth_parent_connectable_interface;

GType
ags_fm_syncsynth_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_fm_syncsynth = 0;

    static const GTypeInfo ags_fm_syncsynth_info = {
      sizeof(AgsFMSyncsynthClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_fm_syncsynth_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsFMSyncsynth),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_fm_syncsynth_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_fm_syncsynth_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    ags_type_fm_syncsynth = g_type_register_static(AGS_TYPE_MACHINE,
						   "AgsFMSyncsynth", &ags_fm_syncsynth_info,
						   0);
    
    g_type_add_interface_static(ags_type_fm_syncsynth,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_fm_syncsynth);
  }

  return g_define_type_id__volatile;
}

void
ags_fm_syncsynth_class_init(AgsFMSyncsynthClass *fm_syncsynth)
{
  GObjectClass *gobject;
  AgsMachineClass *machine;

  ags_fm_syncsynth_parent_class = g_type_class_peek_parent(fm_syncsynth);

  /* GObjectClass */
  gobject = (GObjectClass *) fm_syncsynth;

  gobject->finalize = ags_fm_syncsynth_finalize;

  /* AgsMachineClass */
  machine = (AgsMachineClass *) fm_syncsynth;

  machine->map_recall = ags_fm_syncsynth_map_recall;
}

void
ags_fm_syncsynth_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_fm_syncsynth_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_fm_syncsynth_connect;
  connectable->disconnect = ags_fm_syncsynth_disconnect;
}

void
ags_fm_syncsynth_init(AgsFMSyncsynth *fm_syncsynth)
{
  AgsWindow *window;
  AgsCompositeEditor *composite_editor;
  GtkBox *hbox;
  GtkBox *vbox;
  GtkGrid *grid;
  GtkFrame *frame;
  GtkBox *volume_hbox;
  GtkLabel *label;
  AgsFMOscillator *fm_oscillator;
  
  AgsAudio *audio;

  AgsApplicationContext *application_context;   

  AgsConfig *config;

  AgsMachineCounterManager *machine_counter_manager;
  AgsMachineCounter *machine_counter;
  
  gchar *machine_name;

  gint position;
  gdouble gui_scale_factor;
  guint samplerate;

  application_context = ags_application_context_get_instance();
  
  /* machine counter */
  machine_counter_manager = ags_machine_counter_manager_get_instance();

  machine_counter = ags_machine_counter_manager_find_machine_counter(machine_counter_manager,
								     AGS_TYPE_FM_SYNCSYNTH);

  machine_name = NULL;

  if(machine_counter != NULL){
    machine_name = g_strdup_printf("Default %d",
				   machine_counter->counter);
  
    ags_machine_counter_increment(machine_counter);
  }
  
  g_object_set(fm_syncsynth,
	       "machine-name", machine_name,
	       NULL);

  g_free(machine_name);

  /* machine selector */
  window = ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));

  composite_editor = ags_ui_provider_get_composite_editor(AGS_UI_PROVIDER(application_context));

  position = g_list_length(window->machine);
  
  ags_machine_selector_popup_insert_machine(composite_editor->machine_selector,
					    position,
					    fm_syncsynth);

  application_context = ags_application_context_get_instance();

  config = ags_config_get_instance();

  /* scale factor */
  gui_scale_factor = ags_ui_provider_get_gui_scale_factor(AGS_UI_PROVIDER(application_context));

  samplerate = ags_soundcard_helper_config_get_samplerate(config);
  
  audio = AGS_MACHINE(fm_syncsynth)->audio;
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

  AGS_MACHINE(fm_syncsynth)->flags |= (AGS_MACHINE_IS_SYNTHESIZER |
				       AGS_MACHINE_REVERSE_NOTATION);
  AGS_MACHINE(fm_syncsynth)->mapping_flags |= AGS_MACHINE_MONO;

  AGS_MACHINE(fm_syncsynth)->input_pad_type = G_TYPE_NONE;
  AGS_MACHINE(fm_syncsynth)->input_line_type = G_TYPE_NONE;
  AGS_MACHINE(fm_syncsynth)->output_pad_type = G_TYPE_NONE;
  AGS_MACHINE(fm_syncsynth)->output_line_type = G_TYPE_NONE;

  /* audio resize */
  g_signal_connect(fm_syncsynth, "samplerate-changed",
		   G_CALLBACK(ags_fm_syncsynth_samplerate_changed_callback), NULL);

  g_signal_connect_after(G_OBJECT(fm_syncsynth), "resize-audio-channels",
			 G_CALLBACK(ags_fm_syncsynth_resize_audio_channels), NULL);

  g_signal_connect_after(G_OBJECT(fm_syncsynth), "resize-pads",
			 G_CALLBACK(ags_fm_syncsynth_resize_pads), NULL);
  
  /* create widgets */
  fm_syncsynth->flags = 0;

  /* mapped IO */
  fm_syncsynth->mapped_input_pad = 0;
  fm_syncsynth->mapped_output_pad = 0;

  fm_syncsynth->playback_play_container = ags_recall_container_new();
  fm_syncsynth->playback_recall_container = ags_recall_container_new();

  fm_syncsynth->notation_play_container = ags_recall_container_new();
  fm_syncsynth->notation_recall_container = ags_recall_container_new();

  fm_syncsynth->volume_play_container = ags_recall_container_new();
  fm_syncsynth->volume_recall_container = ags_recall_container_new();

  fm_syncsynth->envelope_play_container = ags_recall_container_new();
  fm_syncsynth->envelope_recall_container = ags_recall_container_new();

  fm_syncsynth->buffer_play_container = ags_recall_container_new();
  fm_syncsynth->buffer_recall_container = ags_recall_container_new();
   
  /* name and xml type */
  fm_syncsynth->name = NULL;
  fm_syncsynth->xml_type = "ags-fm_syncsynth";
 
  /* create widgets */
  hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				0);

  gtk_box_set_spacing(hbox,
		      AGS_UI_PROVIDER_DEFAULT_SPACING);

  gtk_frame_set_child(AGS_MACHINE(fm_syncsynth)->frame,
		      (GtkWidget *) hbox);

  fm_syncsynth->fm_oscillator = NULL;
  
  fm_syncsynth->fm_oscillator_box = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
							   AGS_UI_PROVIDER_DEFAULT_SPACING);

  gtk_box_append(hbox,
		 (GtkWidget *) fm_syncsynth->fm_oscillator_box);

  /* add fm oscillator */
  fm_oscillator = ags_fm_oscillator_new();
  ags_fm_syncsynth_add_fm_oscillator(fm_syncsynth,
				     fm_oscillator);
  
  /* add and remove buttons */
  vbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
				0);

  gtk_box_set_spacing(vbox,
		      AGS_UI_PROVIDER_DEFAULT_SPACING);

  gtk_box_append(hbox,
		 (GtkWidget *) vbox);

  fm_syncsynth->add = (GtkButton *) gtk_button_new_from_icon_name("list-add");
  gtk_box_append(vbox,
		 (GtkWidget *) fm_syncsynth->add);

  fm_syncsynth->remove = (GtkButton *) gtk_button_new_from_icon_name("list-remove");
  gtk_box_append(vbox,
		 (GtkWidget *) fm_syncsynth->remove);
  
  /* update */
  vbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
				0);

  gtk_box_set_spacing(vbox,
		      AGS_UI_PROVIDER_DEFAULT_SPACING);

  gtk_box_append(hbox,
		 (GtkWidget *) vbox);

  fm_syncsynth->auto_update = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("auto update"));
  gtk_box_append(vbox,
		 (GtkWidget *) fm_syncsynth->auto_update);

  fm_syncsynth->update = (GtkButton *) gtk_button_new_with_label(i18n("update"));
  gtk_box_append(vbox,
		 (GtkWidget *) fm_syncsynth->update);

  /* grid */
  grid = (GtkGrid *) gtk_grid_new();

  gtk_grid_set_column_spacing(grid,
			      AGS_UI_PROVIDER_DEFAULT_COLUMN_SPACING);
  gtk_grid_set_row_spacing(grid,
			   AGS_UI_PROVIDER_DEFAULT_ROW_SPACING);

  gtk_box_append(vbox,
		 (GtkWidget *) grid);
  
  /* lower - frequency */  
  label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				    "label", i18n("lower"),
				    "xalign", 0.0,
				    NULL);

  gtk_widget_set_valign((GtkWidget *) label,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_FILL);
  
  gtk_grid_attach(grid,
		  GTK_WIDGET(label),
		  0, 0,
		  1, 1);

  fm_syncsynth->lower = (GtkSpinButton *) gtk_spin_button_new_with_range(AGS_FM_SYNCSYNTH_BASE_NOTE_MIN,
									 AGS_FM_SYNCSYNTH_BASE_NOTE_MAX,
									 1.0);
  gtk_spin_button_set_digits(fm_syncsynth->lower,
			     2);
  gtk_spin_button_set_value(fm_syncsynth->lower, -48.0);

  gtk_widget_set_valign((GtkWidget *) fm_syncsynth->lower,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) fm_syncsynth->lower,
			GTK_ALIGN_FILL);

  gtk_grid_attach(grid,
		  (GtkWidget *) fm_syncsynth->lower,
		  1, 0,
		  1, 1);

  /* loop start */
  label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				    "label", i18n("loop start"),
				    "xalign", 0.0,
				    NULL);

  gtk_widget_set_valign((GtkWidget *) label,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_FILL);

  gtk_grid_attach(grid,
		  (GtkWidget *) label,
		  0, 1,
		  1, 1);

  fm_syncsynth->loop_start = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0,
									      AGS_FM_OSCILLATOR_DEFAULT_FRAME_COUNT * ((gdouble) samplerate / AGS_FM_OSCILLATOR_DEFAULT_SAMPLERATE),
									      1.0);

  gtk_widget_set_valign((GtkWidget *) fm_syncsynth->loop_start,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) fm_syncsynth->loop_start,
			GTK_ALIGN_FILL);

  gtk_grid_attach(grid,
		  GTK_WIDGET(fm_syncsynth->loop_start),
		  1, 1,
		  1, 1);

  /* loop end */
  label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				    "label", i18n("loop end"),
				    "xalign", 0.0,
				    NULL);

  gtk_widget_set_valign((GtkWidget *) label,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_FILL);

  gtk_grid_attach(grid,
		  (GtkWidget *) label,
		   0, 2,
		   1, 1);

  fm_syncsynth->loop_end = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0,
									    AGS_FM_OSCILLATOR_DEFAULT_FRAME_COUNT * ((gdouble) samplerate / AGS_FM_OSCILLATOR_DEFAULT_SAMPLERATE),
									    1.0);

  gtk_widget_set_valign((GtkWidget *) fm_syncsynth->loop_end,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) fm_syncsynth->loop_end,
			GTK_ALIGN_FILL);

  gtk_grid_attach(grid,
		  (GtkWidget *) fm_syncsynth->loop_end,
		  1, 2,
		  1, 1);

  /* volume */
  frame = (GtkFrame *) gtk_frame_new(i18n("volume"));

  gtk_widget_set_valign((GtkWidget *) frame,
			GTK_ALIGN_START);
  gtk_widget_set_halign((GtkWidget *) frame,
			GTK_ALIGN_START);
    
  gtk_box_append(hbox,
		 (GtkWidget *) frame);

  volume_hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				       0);

  gtk_box_set_spacing(volume_hbox,
		      AGS_UI_PROVIDER_DEFAULT_SPACING);

  gtk_frame_set_child(frame,
		      (GtkWidget *) volume_hbox);
  
  fm_syncsynth->volume = (GtkScale *) gtk_scale_new_with_range(GTK_ORIENTATION_VERTICAL,
							       0.0,
							       2.0,
							       0.025);

  gtk_widget_set_size_request(fm_syncsynth->volume,
			      (gint) (gui_scale_factor * 16.0), (gint) (gui_scale_factor * 100.0));

  gtk_widget_set_valign((GtkWidget *) fm_syncsynth->volume,
			GTK_ALIGN_START);
  
  gtk_box_append(volume_hbox,
		 (GtkWidget *) fm_syncsynth->volume);

  gtk_scale_set_digits(fm_syncsynth->volume,
		       3);

  gtk_range_set_increments(GTK_RANGE(fm_syncsynth->volume),
			   0.025, 0.1);
  gtk_range_set_value(GTK_RANGE(fm_syncsynth->volume),
		      1.0);
  gtk_range_set_inverted(GTK_RANGE(fm_syncsynth->volume),
			 TRUE);  
}

void
ags_fm_syncsynth_finalize(GObject *gobject)
{
  /* call parent */
  G_OBJECT_CLASS(ags_fm_syncsynth_parent_class)->finalize(gobject);
}

void
ags_fm_syncsynth_connect(AgsConnectable *connectable)
{
  AgsFMSyncsynth *fm_syncsynth;

  GList *start_list, *list;
  
  if((AGS_CONNECTABLE_CONNECTED & (AGS_MACHINE(connectable)->connectable_flags)) != 0){
    return;
  }

  ags_fm_syncsynth_parent_connectable_interface->connect(connectable);

  /* AgsFMSyncsynth */
  fm_syncsynth = AGS_FM_SYNCSYNTH(connectable);

  list =
    start_list = ags_fm_syncsynth_get_fm_oscillator(fm_syncsynth);

  while(list != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(list->data));
    
    g_signal_connect((GObject *) list->data, "control-changed",
		     G_CALLBACK(ags_fm_syncsynth_fm_oscillator_control_changed_callback), (gpointer) fm_syncsynth);
    
    list = list->next;
  }

  g_list_free(start_list);

  g_signal_connect((GObject *) fm_syncsynth->add, "clicked",
		   G_CALLBACK(ags_fm_syncsynth_add_callback), (gpointer) fm_syncsynth);

  g_signal_connect((GObject *) fm_syncsynth->remove, "clicked",
		   G_CALLBACK(ags_fm_syncsynth_remove_callback), (gpointer) fm_syncsynth);

  g_signal_connect((GObject *) fm_syncsynth->auto_update, "toggled",
		   G_CALLBACK(ags_fm_syncsynth_auto_update_callback), fm_syncsynth);

  g_signal_connect((GObject *) fm_syncsynth->update, "clicked",
		   G_CALLBACK(ags_fm_syncsynth_update_callback), (gpointer) fm_syncsynth);

  g_signal_connect((GObject *) fm_syncsynth->volume, "value-changed",
		   G_CALLBACK(ags_fm_syncsynth_volume_callback), (gpointer) fm_syncsynth);
}

void
ags_fm_syncsynth_disconnect(AgsConnectable *connectable)
{
  AgsFMSyncsynth *fm_syncsynth;

  GList *start_list, *list;
  
  if((AGS_CONNECTABLE_CONNECTED & (AGS_MACHINE(connectable)->connectable_flags)) == 0){
    return;
  }

  ags_fm_syncsynth_parent_connectable_interface->disconnect(connectable);

  /* AgsFMSyncsynth */
  fm_syncsynth = AGS_FM_SYNCSYNTH(connectable);

  list =
    start_list = ags_fm_syncsynth_get_fm_oscillator(fm_syncsynth);

  while(list != NULL){
    ags_connectable_disconnect(AGS_CONNECTABLE(list->data));
    
    g_object_disconnect((GObject *) list->data,
			"any_signal::control-changed",
			G_CALLBACK(ags_fm_syncsynth_fm_oscillator_control_changed_callback),
			(gpointer) fm_syncsynth,
			NULL);
    
    list = list->next;
  }

  g_list_free(start_list);

  g_object_disconnect((GObject *) fm_syncsynth->add,
		      "any_signal::clicked",
		      G_CALLBACK(ags_fm_syncsynth_add_callback),
		      (gpointer) fm_syncsynth,
		      NULL);
  
  g_object_disconnect((GObject *) fm_syncsynth->remove,
		      "any_signal::clicked",
		      G_CALLBACK(ags_fm_syncsynth_remove_callback),
		      (gpointer) fm_syncsynth,
		      NULL);

  g_object_disconnect((GObject *) fm_syncsynth->auto_update,
		      "any_signal::toggled",
		      G_CALLBACK(ags_fm_syncsynth_auto_update_callback),
		      fm_syncsynth,
		      NULL);
  
  g_object_disconnect((GObject *) fm_syncsynth->update,
		      "any_signal::clicked",
		      G_CALLBACK(ags_fm_syncsynth_update_callback),
		      (gpointer) fm_syncsynth,
		      NULL);

  g_object_disconnect((GObject *) fm_syncsynth->volume,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_fm_syncsynth_volume_callback),
		      (gpointer) fm_syncsynth,
		      NULL);
}

void
ags_fm_syncsynth_resize_audio_channels(AgsMachine *machine,
				       guint audio_channels, guint audio_channels_old,
				       gpointer data)
{
  AgsFMSyncsynth *fm_syncsynth;

  fm_syncsynth = (AgsFMSyncsynth *) machine;

  if(audio_channels > audio_channels_old){
    /* recall */
    if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0){
      ags_fm_syncsynth_input_map_recall(fm_syncsynth,
					audio_channels_old,
					0);
      
      ags_fm_syncsynth_output_map_recall(fm_syncsynth,
					 audio_channels_old,
					 0);
    }
  }
}

void
ags_fm_syncsynth_resize_pads(AgsMachine *machine, GType type,
			     guint pads, guint pads_old,
			     gpointer data)
{
  AgsFMSyncsynth *fm_syncsynth;

  gboolean grow;

  if(pads == pads_old){
    return;
  }

  fm_syncsynth = (AgsFMSyncsynth *) machine;
  
  if(pads_old < pads){
    grow = TRUE;
  }else{
    grow = FALSE;
  }
  
  if(type == AGS_TYPE_INPUT){
    if(grow){
      if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0){
	/* depending on destination */
	ags_fm_syncsynth_input_map_recall(fm_syncsynth,
					  0,
					  pads_old);
      }
    }else{
      fm_syncsynth->mapped_input_pad = pads;
    }
  }else{
    if(grow){
      if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0){
	ags_fm_syncsynth_output_map_recall(fm_syncsynth,
					   0,
					   pads_old);
      }
    }else{
      fm_syncsynth->mapped_output_pad = pads;
    }
  }
}

void
ags_fm_syncsynth_map_recall(AgsMachine *machine)
{
  AgsFMSyncsynth *fm_syncsynth;
  
  AgsAudio *audio;

  GList *start_recall, *recall;

  gint position;

  if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0 ||
     (AGS_MACHINE_PREMAPPED_RECALL & (machine->flags)) != 0){
    return;
  }

  fm_syncsynth = AGS_FM_SYNCSYNTH(machine);

  audio = machine->audio;
  
  position = 0;
  
  /* ags-fx-playback */
  start_recall = ags_fx_factory_create(audio,
				       fm_syncsynth->playback_play_container, fm_syncsynth->playback_recall_container,
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

  /* ags-fx-notation */
  start_recall = ags_fx_factory_create(audio,
				       fm_syncsynth->notation_play_container, fm_syncsynth->notation_recall_container,
				       "ags-fx-notation",
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
				       fm_syncsynth->volume_play_container, fm_syncsynth->volume_recall_container,
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
				       fm_syncsynth->envelope_play_container, fm_syncsynth->envelope_recall_container,
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
				       fm_syncsynth->buffer_play_container, fm_syncsynth->buffer_recall_container,
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
  ags_fm_syncsynth_input_map_recall(fm_syncsynth,
				    0,
				    0);

  /* depending on destination */
  ags_fm_syncsynth_output_map_recall(fm_syncsynth,
				     0,
				     0);

  /* call parent */
  AGS_MACHINE_CLASS(ags_fm_syncsynth_parent_class)->map_recall(machine);  
}

void
ags_fm_syncsynth_input_map_recall(AgsFMSyncsynth *fm_syncsynth,
				  guint audio_channel_start,
				  guint input_pad_start)
{
  AgsAudio *audio;

  GList *start_recall;

  gint position;
  guint input_pads;
  guint audio_channels;

  if(fm_syncsynth->mapped_input_pad > input_pad_start){
    return;
  }

  audio = AGS_MACHINE(fm_syncsynth)->audio;

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
				       fm_syncsynth->playback_play_container, fm_syncsynth->playback_recall_container,
				       "ags-fx-playback",
				       NULL,
				       NULL,
				       audio_channel_start, audio_channels,
				       input_pad_start, input_pads,
				       position,
				       (AGS_FX_FACTORY_REMAP | AGS_FX_FACTORY_INPUT), 0);

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);

  /* ags-fx-notation */
  start_recall = ags_fx_factory_create(audio,
				       fm_syncsynth->notation_play_container, fm_syncsynth->notation_recall_container,
				       "ags-fx-notation",
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
				       fm_syncsynth->volume_play_container, fm_syncsynth->volume_recall_container,
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
				       fm_syncsynth->envelope_play_container, fm_syncsynth->envelope_recall_container,
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
				       fm_syncsynth->buffer_play_container, fm_syncsynth->buffer_recall_container,
				       "ags-fx-buffer",
				       NULL,
				       NULL,
				       audio_channel_start, audio_channels,
				       input_pad_start, input_pads,
				       position,
				       (AGS_FX_FACTORY_REMAP | AGS_FX_FACTORY_INPUT), 0);

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);

  fm_syncsynth->mapped_input_pad = input_pads;
}

void
ags_fm_syncsynth_output_map_recall(AgsFMSyncsynth *fm_syncsynth,
				   guint audio_channel_start,
				   guint output_pad_start)
{
  AgsAudio *audio;

  guint output_pads;

  if(fm_syncsynth->mapped_output_pad > output_pad_start){
    return;
  }

  audio = AGS_MACHINE(fm_syncsynth)->audio;

  /* get some fields */
  g_object_get(audio,
	       "output-pads", &output_pads,
	       NULL);
  
  fm_syncsynth->mapped_output_pad = output_pads;
}

/**
 * ags_fm_syncsynth_test_flags:
 * @fm_syncsynth: the #AgsFMSyncsynth
 * @flags: the flags
 * 
 * Test @flags of @fm_syncsynth.
 * 
 * Returns: %TRUE on success, otherwise %FALSE
 * 
 * Since: 3.2.15
 */
gboolean
ags_fm_syncsynth_test_flags(AgsFMSyncsynth *fm_syncsynth, guint flags)
{
  gboolean success;
  
  if(!AGS_IS_FM_SYNCSYNTH(fm_syncsynth)){
    return(FALSE);
  }

  success = ((flags & (fm_syncsynth->flags))) ? TRUE: FALSE;

  return(success);
}

/**
 * ags_fm_syncsynth_set_flags:
 * @fm_syncsynth: the #AgsFMSyncsynth
 * @flags: the flags
 * 
 * Set @flags of @fm_syncsynth.
 * 
 * Since: 3.2.15
 */
void
ags_fm_syncsynth_set_flags(AgsFMSyncsynth *fm_syncsynth, guint flags)
{
  if(!AGS_IS_FM_SYNCSYNTH(fm_syncsynth)){
    return;
  }

  fm_syncsynth->flags |= flags;
}

/**
 * ags_fm_syncsynth_unset_flags:
 * @fm_syncsynth: the #AgsFMSyncsynth
 * @flags: the flags
 * 
 * Unset @flags of @fm_syncsynth.
 * 
 * Since: 3.2.15
 */
void
ags_fm_syncsynth_unset_flags(AgsFMSyncsynth *fm_syncsynth, guint flags)
{
  if(!AGS_IS_FM_SYNCSYNTH(fm_syncsynth)){
    return;
  }

  fm_syncsynth->flags &= (~flags);
}

/**
 * ags_fm_syncsynth_get_fm_oscillator:
 * @fm_syncsynth: the #AgsFMSyncsynth
 * 
 * Get bulk member of @fm_syncsynth.
 * 
 * Returns: the #GList-struct containing #AgsFMOscillator
 *
 * Since: 4.0.0
 */
GList*
ags_fm_syncsynth_get_fm_oscillator(AgsFMSyncsynth *fm_syncsynth)
{
  g_return_val_if_fail(AGS_IS_FM_SYNCSYNTH(fm_syncsynth), NULL);

  return(g_list_reverse(g_list_copy(fm_syncsynth->fm_oscillator)));
}

/**
 * ags_fm_syncsynth_add_fm_oscillator:
 * @fm_syncsynth: the #AgsFMSyncsynth
 * @fm_oscillator: the #AgsFMOscillator
 * 
 * Add @fm_oscillator to @fm_syncsynth.
 * 
 * Since: 3.0.0
 */
void
ags_fm_syncsynth_add_fm_oscillator(AgsFMSyncsynth *fm_syncsynth,
				   AgsFMOscillator *fm_oscillator)
{
  g_return_if_fail(AGS_IS_FM_SYNCSYNTH(fm_syncsynth));
  g_return_if_fail(AGS_IS_FM_OSCILLATOR(fm_oscillator));

  if(g_list_find(fm_syncsynth->fm_oscillator, fm_oscillator) == NULL){
    fm_syncsynth->fm_oscillator = g_list_prepend(fm_syncsynth->fm_oscillator,
						 fm_oscillator);

    gtk_box_append(fm_syncsynth->fm_oscillator_box,
		   (GtkWidget *) fm_oscillator);
  }
}

/**
 * ags_fm_syncsynth_remove_fm_oscillator:
 * @fm_syncsynth: the #AgsFMSyncsynth
 * @fm_oscillator: the #AgsFMOscillator
 * 
 * Remove nth fm_oscillator.
 * 
 * Since: 3.0.0
 */
void
ags_fm_syncsynth_remove_fm_oscillator(AgsFMSyncsynth *fm_syncsynth,
				      AgsFMOscillator *fm_oscillator)
{
  g_return_if_fail(AGS_IS_FM_SYNCSYNTH(fm_syncsynth));
  g_return_if_fail(AGS_IS_FM_OSCILLATOR(fm_oscillator));
  
  if(g_list_find(fm_syncsynth->fm_oscillator, fm_oscillator) != NULL){
    fm_syncsynth->fm_oscillator = g_list_remove(fm_syncsynth->fm_oscillator,
						fm_oscillator);
    
    gtk_box_remove(fm_syncsynth->fm_oscillator_box,
		   fm_oscillator);
  }
}

/**
 * ags_fm_syncsynth_reset_loop:
 * @fm_syncsynth: the #AgsFMSyncsynth
 * 
 * Reset loop spin buttons.
 * 
 * Since: 3.0.0
 */
void
ags_fm_syncsynth_reset_loop(AgsFMSyncsynth *fm_syncsynth)
{
  GList *start_list, *list;
  
  gdouble loop_upper, tmp0, tmp1;

  loop_upper = 0.0;

  list =
    start_list = ags_fm_syncsynth_get_fm_oscillator(fm_syncsynth);
  
  while(list != NULL){
    tmp0 = gtk_spin_button_get_value(AGS_FM_OSCILLATOR(list->data)->frame_count);
    tmp1 = gtk_spin_button_get_value(AGS_FM_OSCILLATOR(list->data)->attack);

    if(tmp0 + tmp1 > loop_upper){
      loop_upper = tmp0 + tmp1;
    }
    
    list = list->next;
  }

  g_list_free(start_list);
  
  gtk_spin_button_set_range(fm_syncsynth->loop_start,
			    0.0, loop_upper);
  gtk_spin_button_set_range(fm_syncsynth->loop_end,
			    0.0, loop_upper);
}

/**
 * ags_fm_syncsynth_update:
 * @fm_syncsynth: the #AgsFMSyncsynth
 * 
 * Update audio data.
 * 
 * Since: 3.0.0
 */
void
ags_fm_syncsynth_update(AgsFMSyncsynth *fm_syncsynth)
{
  AgsFMOscillator *fm_oscillator;
  
  AgsAudio *audio;
  AgsChannel *start_input;
  AgsChannel *channel, *next_channel;

  AgsClearAudioSignal *clear_audio_signal;
  AgsApplySynth *apply_synth;

  AgsApplicationContext *application_context;
  
  GList *start_list, *list;
  GList *start_synth_generator, *synth_generator;    
  GList *task;
  
  guint input_lines;
  guint requested_frame_count;
  guint buffer_size;
  guint format;
  guint attack, frame_count;
  guint loop_start, loop_end;
  gdouble frequency, phase, start_frequency;
  gdouble volume;
  gdouble fm_lfo_frequency, fm_lfo_depth;
  gdouble fm_tuning;

  guint sync_point_count;

  application_context = ags_application_context_get_instance();

  audio = AGS_MACHINE(fm_syncsynth)->audio;

  /*  */
  start_frequency = (gdouble) gtk_spin_button_get_value(fm_syncsynth->lower);

  /* clear input */
  g_object_get(audio,
	       "input", &start_input,
	       NULL);

  channel = start_input;

  if(channel != NULL){
    g_object_ref(channel);
  }
  
  next_channel = NULL;

  task = NULL;

  while(channel != NULL){
    AgsRecycling *first_recycling;
    AgsAudioSignal *template;

    GList *start_list;
    
    g_object_get(channel,
		 "first-recycling", &first_recycling,
		 NULL);

    g_object_get(first_recycling,
		 "audio-signal", &start_list,
		 NULL);
    
    /* clear task */
    template = ags_audio_signal_get_template(start_list);

    clear_audio_signal = ags_clear_audio_signal_new(template);
    task = g_list_prepend(task,
			  clear_audio_signal);

    g_list_free_full(start_list,
		     g_object_unref);
    
    g_object_unref(first_recycling);
    g_object_unref(template);
    
    /* iterate */
    next_channel = ags_channel_next(channel);

    g_object_unref(channel);

    channel = next_channel;
  }

  if(next_channel != NULL){
    g_object_unref(next_channel);
  }

  /* write input */
  list =
    start_list = ags_fm_syncsynth_get_fm_oscillator(fm_syncsynth);

  /* get some fields */
  g_object_get(audio,
	       "input-lines", &input_lines,
	       "synth-generator", &start_synth_generator,
	       NULL);

  g_object_get(start_input,
	       "buffer-size", &buffer_size,
	       "format", &format,
	       NULL);

  loop_start = (guint) gtk_spin_button_get_value_as_int(fm_syncsynth->loop_start);
  loop_end = (guint) gtk_spin_button_get_value_as_int(fm_syncsynth->loop_end);

  requested_frame_count = 0;

  while(list != NULL){
    guint current_frame_count;

    fm_oscillator = AGS_FM_OSCILLATOR(list->data);

    current_frame_count = gtk_spin_button_get_value(fm_oscillator->attack) + gtk_spin_button_get_value(fm_oscillator->frame_count);

    if(requested_frame_count < current_frame_count){
      requested_frame_count = current_frame_count;
    }
    
    list = list->next;
  }

  list = start_list;
  
  synth_generator = start_synth_generator;

  while(list != NULL){
    guint i;
    gboolean do_sync;
    
    /* do it so */
    fm_oscillator = AGS_FM_OSCILLATOR(list->data);

    attack = (guint) gtk_spin_button_get_value_as_int(fm_oscillator->attack);
    frame_count = (guint) gtk_spin_button_get_value_as_int(fm_oscillator->frame_count);
    phase = (gdouble) gtk_spin_button_get_value(fm_oscillator->phase);
    frequency = (gdouble) gtk_spin_button_get_value(fm_oscillator->frequency);
    volume = (gdouble) gtk_spin_button_get_value(fm_oscillator->volume);

    fm_lfo_frequency = gtk_spin_button_get_value(fm_oscillator->fm_lfo_frequency);
    fm_lfo_depth = gtk_spin_button_get_value(fm_oscillator->fm_lfo_depth);

    fm_tuning = gtk_spin_button_get_value(fm_oscillator->fm_tuning);

    g_object_set(synth_generator->data,
		 "format", format,
		 "delay", (gdouble) attack / buffer_size,
		 "attack", attack,
		 "frame-count", frame_count,
		 "loop-start", loop_start,
		 "loop-end", loop_end,
		 "oscillator", gtk_combo_box_get_active(fm_oscillator->wave),
		 "frequency", frequency,
		 "phase", phase,
		 "volume", volume,
		 "do-fm-synth", TRUE,
		 "fm-lfo-oscillator", gtk_combo_box_get_active(fm_oscillator->fm_lfo_wave),
		 "fm-lfo-frequency", fm_lfo_frequency,
		 "fm-lfo-depth", fm_lfo_depth,
		 "fm-tuning", fm_tuning,
		 NULL);

    do_sync = gtk_check_button_get_active(fm_oscillator->do_sync);

    if(do_sync){
      sync_point_count = fm_oscillator->sync_point_count;
      
      /* free previous sync point */
      if(AGS_SYNTH_GENERATOR(synth_generator->data)->sync_point != NULL){
	for(i = 0; i < AGS_SYNTH_GENERATOR(synth_generator->data)->sync_point_count; i++){
	  ags_complex_free(AGS_SYNTH_GENERATOR(synth_generator->data)->sync_point[i]);
	}

	free(AGS_SYNTH_GENERATOR(synth_generator->data)->sync_point);
      }

      /* set new sync point */
      if(sync_point_count > 0){
	AGS_SYNTH_GENERATOR(synth_generator->data)->sync_point = (AgsComplex **) malloc(sync_point_count * sizeof(AgsComplex *));
      }else{
	AGS_SYNTH_GENERATOR(synth_generator->data)->sync_point = NULL;
      }

      AGS_SYNTH_GENERATOR(synth_generator->data)->sync_point_count = sync_point_count;

      for(i = 0; i < sync_point_count; i++){
	AGS_SYNTH_GENERATOR(synth_generator->data)->sync_point[i] = ags_complex_alloc();

	AGS_SYNTH_GENERATOR(synth_generator->data)->sync_point[i][0].real = gtk_spin_button_get_value(fm_oscillator->sync_point[2 * i]);
	AGS_SYNTH_GENERATOR(synth_generator->data)->sync_point[i][0].imag = gtk_spin_button_get_value(fm_oscillator->sync_point[2 * i + 1]);
      }
    }else{
      for(i = 0; i < AGS_SYNTH_GENERATOR(synth_generator->data)->sync_point_count; i++){
	ags_complex_free(AGS_SYNTH_GENERATOR(synth_generator->data)->sync_point[i]);
      }

      free(AGS_SYNTH_GENERATOR(synth_generator->data)->sync_point);
      
      AGS_SYNTH_GENERATOR(synth_generator->data)->sync_point = NULL;
      AGS_SYNTH_GENERATOR(synth_generator->data)->sync_point_count = 0;
    }
					
    apply_synth = ags_apply_synth_new(synth_generator->data,
				      start_input,
				      start_frequency, input_lines);
    g_object_set(apply_synth,
		 "requested-frame-count", requested_frame_count,
		 NULL);
        
    task = g_list_prepend(task,
			  apply_synth);

    /* iterate */
    synth_generator = synth_generator->next;
    
    list = list->next;
  }

  g_list_free_full(start_synth_generator,
		   g_object_unref);
  
  g_list_free(start_list);

  ags_ui_provider_schedule_task_all(AGS_UI_PROVIDER(application_context),
				    g_list_reverse(task));
}

/**
 * ags_fm_syncsynth_new:
 * @soundcard: the assigned soundcard.
 *
 * Create a new instance of #AgsFMSyncsynth
 *
 * Returns: the new #AgsFMSyncsynth
 *
 * Since: 3.0.0
 */
AgsFMSyncsynth*
ags_fm_syncsynth_new(GObject *soundcard)
{
  AgsFMSyncsynth *fm_syncsynth;

  fm_syncsynth = (AgsFMSyncsynth *) g_object_new(AGS_TYPE_FM_SYNCSYNTH,
						 NULL);

  g_object_set(AGS_MACHINE(fm_syncsynth)->audio,
	       "output-soundcard", soundcard,
	       NULL);

  return(fm_syncsynth);
}
