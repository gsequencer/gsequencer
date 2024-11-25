/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2023 Joël Krähemann
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

#include <ags/app/machine/ags_syncsynth.h>
#include <ags/app/machine/ags_syncsynth_callbacks.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_window.h>
#include <ags/app/ags_composite_editor.h>
#include <ags/app/ags_machine.h>
#include <ags/app/ags_pad.h>
#include <ags/app/ags_line.h>

#include <math.h>

#include <ags/i18n.h>

void ags_syncsynth_class_init(AgsSyncsynthClass *syncsynth);
void ags_syncsynth_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_syncsynth_init(AgsSyncsynth *syncsynth);
void ags_syncsynth_finalize(GObject *gobject);

void ags_syncsynth_connect(AgsConnectable *connectable);
void ags_syncsynth_disconnect(AgsConnectable *connectable);

void ags_syncsynth_show(GtkWidget *widget);

void ags_syncsynth_resize_audio_channels(AgsMachine *machine,
					 guint audio_channels, guint audio_channels_old,
					 gpointer data);
void ags_syncsynth_resize_pads(AgsMachine *machine, GType channel_type,
			       guint pads, guint pads_old,
			       gpointer data);

void ags_syncsynth_map_recall(AgsMachine *machine);

void ags_syncsynth_input_map_recall(AgsSyncsynth *syncsynth,
				    guint audio_channel_start,
				    guint input_pad_start);
void ags_syncsynth_output_map_recall(AgsSyncsynth *syncsynth,
				     guint audio_channel_start,
				     guint output_pad_start);

/**
 * SECTION:ags_syncsynth
 * @short_description: syncsynth
 * @title: AgsSyncsynth
 * @section_id:
 * @include: ags/app/machine/ags_syncsynth.h
 *
 * The #AgsSyncsynth is a composite widget to act as syncsynth.
 */

static gpointer ags_syncsynth_parent_class = NULL;
static AgsConnectableInterface *ags_syncsynth_parent_connectable_interface;

GType
ags_syncsynth_get_type(void)
{
  static gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_syncsynth = 0;

    static const GTypeInfo ags_syncsynth_info = {
      sizeof(AgsSyncsynthClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_syncsynth_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsSyncsynth),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_syncsynth_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_syncsynth_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    ags_type_syncsynth = g_type_register_static(AGS_TYPE_MACHINE,
						"AgsSyncsynth", &ags_syncsynth_info,
						0);
    
    g_type_add_interface_static(ags_type_syncsynth,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_syncsynth);
  }

  return g_define_type_id__volatile;
}

void
ags_syncsynth_class_init(AgsSyncsynthClass *syncsynth)
{
  GObjectClass *gobject;
  AgsMachineClass *machine;

  ags_syncsynth_parent_class = g_type_class_peek_parent(syncsynth);

  /* GObjectClass */
  gobject = (GObjectClass *) syncsynth;

  gobject->finalize = ags_syncsynth_finalize;

  /* AgsMachineClass */
  machine = (AgsMachineClass *) syncsynth;

  machine->map_recall = ags_syncsynth_map_recall;
}

void
ags_syncsynth_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_syncsynth_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_syncsynth_connect;
  connectable->disconnect = ags_syncsynth_disconnect;
}

void
ags_syncsynth_init(AgsSyncsynth *syncsynth)
{
  AgsWindow *window;
  AgsCompositeEditor *composite_editor;
  GtkBox *hbox;
  GtkBox *vbox;
  GtkGrid *grid;
  GtkLabel *label;
  GtkFrame *frame;
  GtkBox *volume_hbox;
  AgsOscillator *oscillator;
  
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

  config = ags_config_get_instance();
  
  /* machine counter */
  machine_counter_manager = ags_machine_counter_manager_get_instance();

  machine_counter = ags_machine_counter_manager_find_machine_counter(machine_counter_manager,
								     AGS_TYPE_SYNCSYNTH);

  machine_name = NULL;

  if(machine_counter != NULL){
    machine_name = g_strdup_printf("Default %d",
				   machine_counter->counter);
  
    ags_machine_counter_increment(machine_counter);
  }
  
  g_object_set(syncsynth,
	       "machine-name", machine_name,
	       NULL);

  g_free(machine_name);

  /* machine selector */
  window = (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));

  composite_editor = (AgsCompositeEditor *) ags_ui_provider_get_composite_editor(AGS_UI_PROVIDER(application_context));

  position = g_list_length(window->machine);
  
  ags_machine_selector_popup_insert_machine(composite_editor->machine_selector,
					    position,
					    (AgsMachine *) syncsynth);

  /* scale factor */
  gui_scale_factor = ags_ui_provider_get_gui_scale_factor(AGS_UI_PROVIDER(application_context));

  samplerate = ags_soundcard_helper_config_get_samplerate(config);
  
  audio = AGS_MACHINE(syncsynth)->audio;
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

  AGS_MACHINE(syncsynth)->flags |= (AGS_MACHINE_IS_SYNTHESIZER |
				    AGS_MACHINE_REVERSE_NOTATION);
  AGS_MACHINE(syncsynth)->mapping_flags |= AGS_MACHINE_MONO;

  AGS_MACHINE(syncsynth)->input_pad_type = G_TYPE_NONE;
  AGS_MACHINE(syncsynth)->input_line_type = G_TYPE_NONE;
  AGS_MACHINE(syncsynth)->output_pad_type = G_TYPE_NONE;
  AGS_MACHINE(syncsynth)->output_line_type = G_TYPE_NONE;

  /* audio resize */
  g_signal_connect(syncsynth, "samplerate-changed",
		   G_CALLBACK(ags_syncsynth_samplerate_changed_callback), NULL);

  g_signal_connect_after(G_OBJECT(syncsynth), "resize-audio-channels",
			 G_CALLBACK(ags_syncsynth_resize_audio_channels), NULL);

  g_signal_connect_after(G_OBJECT(syncsynth), "resize-pads",
			 G_CALLBACK(ags_syncsynth_resize_pads), NULL);
  
  /* create widgets */
  syncsynth->flags = 0;

  /* mapped IO */
  syncsynth->mapped_input_pad = 0;
  syncsynth->mapped_output_pad = 0;

  syncsynth->playback_play_container = ags_recall_container_new();
  syncsynth->playback_recall_container = ags_recall_container_new();

  syncsynth->notation_play_container = ags_recall_container_new();
  syncsynth->notation_recall_container = ags_recall_container_new();

  syncsynth->volume_play_container = ags_recall_container_new();
  syncsynth->volume_recall_container = ags_recall_container_new();

  syncsynth->envelope_play_container = ags_recall_container_new();
  syncsynth->envelope_recall_container = ags_recall_container_new();

  syncsynth->buffer_play_container = ags_recall_container_new();
  syncsynth->buffer_recall_container = ags_recall_container_new();
   
  /* name and xml type */
  syncsynth->name = NULL;
  syncsynth->xml_type = "ags-syncsynth";
 
  /* create widgets */
  hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				AGS_UI_PROVIDER_DEFAULT_SPACING);

  gtk_frame_set_child(AGS_MACHINE(syncsynth)->frame,
		      (GtkWidget *) hbox);

  syncsynth->oscillator = NULL;  

  syncsynth->oscillator_box = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
						     AGS_UI_PROVIDER_DEFAULT_SPACING);

  gtk_box_append(hbox,
		 (GtkWidget *) syncsynth->oscillator_box);

  /* add 2 oscillators */
  oscillator = ags_oscillator_new();
  ags_syncsynth_add_oscillator(syncsynth,
			       oscillator);

  oscillator = ags_oscillator_new();
  ags_syncsynth_add_oscillator(syncsynth,
			       oscillator);
  
  /* add and remove buttons */
  vbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
				0);

  gtk_box_set_spacing(vbox,
		      AGS_UI_PROVIDER_DEFAULT_SPACING);

  gtk_box_append(hbox,
		 (GtkWidget *) vbox);

  syncsynth->add = (GtkButton *) gtk_button_new_from_icon_name("list-add");
  gtk_box_append(vbox,
		 (GtkWidget *) syncsynth->add);

  syncsynth->remove = (GtkButton *) gtk_button_new_from_icon_name("list-remove");
  gtk_box_append(vbox,
		 (GtkWidget *) syncsynth->remove);
  
  /* update */
  vbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
				0);

  gtk_box_set_spacing(vbox,
		      AGS_UI_PROVIDER_DEFAULT_SPACING);

  gtk_box_append(hbox,
		 (GtkWidget *) vbox);

  syncsynth->auto_update = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("auto update"));
  gtk_box_append(vbox,
		 (GtkWidget *) syncsynth->auto_update);

  syncsynth->update = (GtkButton *) gtk_button_new_with_label(i18n("update"));
  gtk_box_append(vbox,
		 (GtkWidget *) syncsynth->update);

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
				    NULL);

  gtk_widget_set_valign((GtkWidget *) label,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
    
  gtk_grid_attach(grid,
		  (GtkWidget *) label,
		  0, 0,
		  1, 1);

  syncsynth->lower = (GtkSpinButton *) gtk_spin_button_new_with_range(AGS_SYNCSYNTH_BASE_NOTE_MIN,
								      AGS_SYNCSYNTH_BASE_NOTE_MAX,
								      1.0);
  gtk_spin_button_set_value(syncsynth->lower, -48.0);
  gtk_spin_button_set_digits(syncsynth->lower,
			     2);

  gtk_widget_set_valign((GtkWidget *) syncsynth->lower,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) syncsynth->lower,
			GTK_ALIGN_FILL);
  
  gtk_grid_attach(grid,
		  (GtkWidget *) syncsynth->lower,
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

  syncsynth->loop_start = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0,
									   AGS_OSCILLATOR_DEFAULT_FRAME_COUNT * ((gdouble) samplerate / AGS_OSCILLATOR_DEFAULT_SAMPLERATE),
									   1.0);

  gtk_widget_set_valign((GtkWidget *) syncsynth->loop_start,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) syncsynth->loop_start,
			GTK_ALIGN_FILL);
  
  gtk_grid_attach(grid,
		  (GtkWidget *) syncsynth->loop_start,
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

  syncsynth->loop_end = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0,
									 AGS_OSCILLATOR_DEFAULT_FRAME_COUNT * ((gdouble) samplerate / AGS_OSCILLATOR_DEFAULT_SAMPLERATE),
									 1.0);

  gtk_widget_set_valign((GtkWidget *) syncsynth->loop_end,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) syncsynth->loop_end,
			GTK_ALIGN_FILL);
  
  gtk_grid_attach(grid,
		  (GtkWidget *) syncsynth->loop_end,
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
  
  syncsynth->volume = (GtkScale *) gtk_scale_new_with_range(GTK_ORIENTATION_VERTICAL,
							    0.0,
							    2.0,
							    0.025);

  gtk_widget_set_size_request((GtkWidget *) syncsynth->volume,
			      (gint) (gui_scale_factor * 16.0), (gint) (gui_scale_factor * 100.0));

  gtk_widget_set_valign((GtkWidget *) syncsynth->volume,
			GTK_ALIGN_START);
  
  gtk_box_append(volume_hbox,
		 (GtkWidget *) syncsynth->volume);

  gtk_scale_set_digits(syncsynth->volume,
		       3);

  gtk_range_set_increments(GTK_RANGE(syncsynth->volume),
			   0.025, 0.1);
  gtk_range_set_value(GTK_RANGE(syncsynth->volume),
		      1.0);
  gtk_range_set_inverted(GTK_RANGE(syncsynth->volume),
			 TRUE);  
}

void
ags_syncsynth_finalize(GObject *gobject)
{
  /* call parent */
  G_OBJECT_CLASS(ags_syncsynth_parent_class)->finalize(gobject);
}

void
ags_syncsynth_connect(AgsConnectable *connectable)
{
  AgsSyncsynth *syncsynth;

  GList *list_start, *list;
  
  if((AGS_CONNECTABLE_CONNECTED & (AGS_MACHINE(connectable)->connectable_flags)) != 0){
    return;
  }

  ags_syncsynth_parent_connectable_interface->connect(connectable);

  /* AgsSyncsynth */
  syncsynth = AGS_SYNCSYNTH(connectable);

  list =
    list_start = ags_syncsynth_get_oscillator(syncsynth);

  while(list != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(list->data));
    
    g_signal_connect((GObject *) list->data, "control-changed",
		     G_CALLBACK(ags_syncsynth_oscillator_control_changed_callback), (gpointer) syncsynth);
    
    list = list->next;
  }

  g_list_free(list_start);

  g_signal_connect((GObject *) syncsynth->add, "clicked",
		   G_CALLBACK(ags_syncsynth_add_callback), (gpointer) syncsynth);

  g_signal_connect((GObject *) syncsynth->remove, "clicked",
		   G_CALLBACK(ags_syncsynth_remove_callback), (gpointer) syncsynth);

  g_signal_connect((GObject *) syncsynth->auto_update, "toggled",
		   G_CALLBACK(ags_syncsynth_auto_update_callback), syncsynth);

  g_signal_connect((GObject *) syncsynth->update, "clicked",
		   G_CALLBACK(ags_syncsynth_update_callback), (gpointer) syncsynth);

  g_signal_connect((GObject *) syncsynth->volume, "value-changed",
		   G_CALLBACK(ags_syncsynth_volume_callback), (gpointer) syncsynth);
}

void
ags_syncsynth_disconnect(AgsConnectable *connectable)
{
  AgsSyncsynth *syncsynth;

  GList *list_start, *list;
  
  if((AGS_CONNECTABLE_CONNECTED & (AGS_MACHINE(connectable)->connectable_flags)) == 0){
    return;
  }

  ags_syncsynth_parent_connectable_interface->disconnect(connectable);

  /* AgsSyncsynth */
  syncsynth = AGS_SYNCSYNTH(connectable);

  list =
    list_start = ags_syncsynth_get_oscillator(syncsynth);

  while(list != NULL){
    ags_connectable_disconnect(AGS_CONNECTABLE(list->data));
    
    g_object_disconnect((GObject *) list->data,
			"any_signal::control-changed",
			G_CALLBACK(ags_syncsynth_oscillator_control_changed_callback),
			(gpointer) syncsynth,
			NULL);
    
    list = list->next;
  }

  g_list_free(list_start);

  g_object_disconnect((GObject *) syncsynth->add,
		      "any_signal::clicked",
		      G_CALLBACK(ags_syncsynth_add_callback),
		      (gpointer) syncsynth,
		      NULL);
  
  g_object_disconnect((GObject *) syncsynth->remove,
		      "any_signal::clicked",
		      G_CALLBACK(ags_syncsynth_remove_callback),
		      (gpointer) syncsynth,
		      NULL);

  g_object_disconnect((GObject *) syncsynth->auto_update,
		      "any_signal::toggled",
		      G_CALLBACK(ags_syncsynth_auto_update_callback),
		      syncsynth,
		      NULL);
  
  g_object_disconnect((GObject *) syncsynth->update,
		      "any_signal::clicked",
		      G_CALLBACK(ags_syncsynth_update_callback),
		      (gpointer) syncsynth,
		      NULL);

  g_object_disconnect((GObject *) syncsynth->volume,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_syncsynth_volume_callback),
		      (gpointer) syncsynth,
		      NULL);
}

void
ags_syncsynth_resize_audio_channels(AgsMachine *machine,
				    guint audio_channels, guint audio_channels_old,
				    gpointer data)
{
  AgsSyncsynth *syncsynth;

  syncsynth = (AgsSyncsynth *) machine;

  if(audio_channels > audio_channels_old){    
    /* recall */
    if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0){
      ags_syncsynth_input_map_recall(syncsynth,
				     audio_channels_old,
				     0);
      
      ags_syncsynth_output_map_recall(syncsynth,
				      audio_channels_old,
				      0);
    }
  }
}

void
ags_syncsynth_resize_pads(AgsMachine *machine, GType type,
			  guint pads, guint pads_old,
			  gpointer data)
{
  AgsSyncsynth *syncsynth;

  gboolean grow;

  syncsynth = (AgsSyncsynth *) machine;
  
  if(pads_old < pads){
    grow = TRUE;
  }else{
    grow = FALSE;
  }
  
  if(type == AGS_TYPE_INPUT){
    if(grow){
      if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0){
	/* depending on destination */
	ags_syncsynth_input_map_recall(syncsynth,
				       0,
				       pads_old);
      }
    }else{
      syncsynth->mapped_input_pad = pads;
    }
  }else{
    if(grow){
      if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0){
	ags_syncsynth_output_map_recall(syncsynth,
					0,
					pads_old);
      }
    }else{
      syncsynth->mapped_output_pad = pads;
    }
  }
}

void
ags_syncsynth_map_recall(AgsMachine *machine)
{
  AgsSyncsynth *syncsynth;
  
  AgsAudio *audio;

  GList *start_recall, *recall;

  gint position;

  if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0 ||
     (AGS_MACHINE_PREMAPPED_RECALL & (machine->flags)) != 0){
    return;
  }

  syncsynth = AGS_SYNCSYNTH(machine);

  audio = machine->audio;
  
  position = 0;
  
  /* ags-fx-playback */
  start_recall = ags_fx_factory_create(audio,
				       syncsynth->playback_play_container, syncsynth->playback_recall_container,
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
				       syncsynth->notation_play_container, syncsynth->notation_recall_container,
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
				       syncsynth->volume_play_container, syncsynth->volume_recall_container,
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
				       syncsynth->envelope_play_container, syncsynth->envelope_recall_container,
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
				       syncsynth->buffer_play_container, syncsynth->buffer_recall_container,
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
  ags_syncsynth_input_map_recall(syncsynth,
				 0,
				 0);

  /* depending on destination */
  ags_syncsynth_output_map_recall(syncsynth,
				  0,
				  0);

  /* call parent */
  AGS_MACHINE_CLASS(ags_syncsynth_parent_class)->map_recall(machine);  
}

void
ags_syncsynth_input_map_recall(AgsSyncsynth *syncsynth,
			       guint audio_channel_start,
			       guint input_pad_start)
{
  AgsAudio *audio;

  GList *start_recall;

  gint position;
  guint input_pads;
  guint audio_channels;

  if(syncsynth->mapped_input_pad > input_pad_start){
    return;
  }

  audio = AGS_MACHINE(syncsynth)->audio;

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
				       syncsynth->playback_play_container, syncsynth->playback_recall_container,
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
				       syncsynth->notation_play_container, syncsynth->notation_recall_container,
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
				       syncsynth->volume_play_container, syncsynth->volume_recall_container,
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
				       syncsynth->envelope_play_container, syncsynth->envelope_recall_container,
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
				       syncsynth->buffer_play_container, syncsynth->buffer_recall_container,
				       "ags-fx-buffer",
				       NULL,
				       NULL,
				       audio_channel_start, audio_channels,
				       input_pad_start, input_pads,
				       position,
				       (AGS_FX_FACTORY_REMAP | AGS_FX_FACTORY_INPUT), 0);

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);

  syncsynth->mapped_input_pad = input_pads;
}

void
ags_syncsynth_output_map_recall(AgsSyncsynth *syncsynth,
				guint audio_channel_start,
				guint output_pad_start)
{
  AgsAudio *audio;

  guint output_pads;

  if(syncsynth->mapped_output_pad > output_pad_start){
    return;
  }

  audio = AGS_MACHINE(syncsynth)->audio;

  /* get some fields */
  g_object_get(audio,
	       "output-pads", &output_pads,
	       NULL);
  
  syncsynth->mapped_output_pad = output_pads;
}

/**
 * ags_syncsynth_test_flags:
 * @syncsynth: the #AgsSyncsynth
 * @flags: the flags
 * 
 * Test @flags of @syncsynth.
 * 
 * Returns: %TRUE on success, otherwise %FALSE
 * 
 * Since: 3.2.15
 */
gboolean
ags_syncsynth_test_flags(AgsSyncsynth *syncsynth, guint flags)
{
  gboolean success;
  
  if(!AGS_IS_SYNCSYNTH(syncsynth)){
    return(FALSE);
  }

  success = ((flags & (syncsynth->flags))) ? TRUE: FALSE;

  return(success);
}

/**
 * ags_syncsynth_set_flags:
 * @syncsynth: the #AgsSyncsynth
 * @flags: the flags
 * 
 * Set @flags of @syncsynth.
 * 
 * Since: 3.2.15
 */
void
ags_syncsynth_set_flags(AgsSyncsynth *syncsynth, guint flags)
{
  if(!AGS_IS_SYNCSYNTH(syncsynth)){
    return;
  }

  syncsynth->flags |= flags;
}

/**
 * ags_syncsynth_unset_flags:
 * @syncsynth: the #AgsSyncsynth
 * @flags: the flags
 * 
 * Unset @flags of @syncsynth.
 * 
 * Since: 3.2.15
 */
void
ags_syncsynth_unset_flags(AgsSyncsynth *syncsynth, guint flags)
{
  if(!AGS_IS_SYNCSYNTH(syncsynth)){
    return;
  }

  syncsynth->flags &= (~flags);
}

/**
 * ags_syncsynth_get_oscillator:
 * @syncsynth: the #AgsSyncsynth
 * 
 * Get bulk member of @syncsynth.
 * 
 * Returns: the #GList-struct containing #AgsOscillator
 *
 * Since: 4.0.0
 */
GList*
ags_syncsynth_get_oscillator(AgsSyncsynth *syncsynth)
{
  g_return_val_if_fail(AGS_IS_SYNCSYNTH(syncsynth), NULL);

  return(g_list_reverse(g_list_copy(syncsynth->oscillator)));
}

/**
 * ags_syncsynth_add_oscillator:
 * @syncsynth: the #AgsSyncsynth
 * @oscillator: the #AgsOscillator
 * 
 * Add @oscillator to @syncsynth.
 * 
 * Since: 3.0.0
 */
void
ags_syncsynth_add_oscillator(AgsSyncsynth *syncsynth,
			     AgsOscillator *oscillator)
{
  AgsAudio *audio;
  
  g_return_if_fail(AGS_IS_SYNCSYNTH(syncsynth));
  g_return_if_fail(AGS_IS_OSCILLATOR(oscillator));

  if(g_list_find(syncsynth->oscillator, oscillator) == NULL){
    syncsynth->oscillator = g_list_prepend(syncsynth->oscillator,
					   oscillator);
    
    gtk_box_append(syncsynth->oscillator_box,
		   (GtkWidget *) oscillator);

    audio = AGS_MACHINE(syncsynth)->audio;
    ags_audio_add_synth_generator(audio,
				  (GObject *) ags_synth_generator_new());
  }
}

/**
 * ags_syncsynth_remove_oscillator:
 * @syncsynth: the #AgsSyncsynth
 * @oscillator: the #AgsOscillator
 * 
 * Remove nth oscillator.
 * 
 * Since: 3.0.0
 */
void
ags_syncsynth_remove_oscillator(AgsSyncsynth *syncsynth,
				AgsOscillator *oscillator)
{
  g_return_if_fail(AGS_IS_SYNCSYNTH(syncsynth));
  g_return_if_fail(AGS_IS_OSCILLATOR(oscillator));

  if(g_list_find(syncsynth->oscillator, oscillator) != NULL){
    syncsynth->oscillator = g_list_remove(syncsynth->oscillator,
					  oscillator);
    
    gtk_box_remove(syncsynth->oscillator_box,
		   (GtkWidget *) oscillator);
  }
}

/**
 * ags_syncsynth_reset_loop:
 * @syncsynth: the #AgsSyncsynth
 * 
 * Reset loop spin buttons.
 * 
 * Since: 3.0.0
 */
void
ags_syncsynth_reset_loop(AgsSyncsynth *syncsynth)
{
  GList *list, *list_start;
  
  gdouble loop_upper, tmp0, tmp1;

  loop_upper = 0.0;

  list =
    list_start = ags_syncsynth_get_oscillator(syncsynth);
  
  while(list != NULL){
    tmp0 = gtk_spin_button_get_value(AGS_OSCILLATOR(list->data)->frame_count);
    tmp1 = gtk_spin_button_get_value(AGS_OSCILLATOR(list->data)->attack);

    if(tmp0 + tmp1 > loop_upper){
      loop_upper = tmp0 + tmp1;
    }
    
    list = list->next;
  }

  g_list_free(list_start);
  
  gtk_spin_button_set_range(syncsynth->loop_start,
			    0.0, loop_upper);
  gtk_spin_button_set_range(syncsynth->loop_end,
			    0.0, loop_upper);
}

/**
 * ags_syncsynth_update:
 * @syncsynth: the #AgsSyncsynth
 * 
 * Update audio data.
 * 
 * Since: 3.0.0
 */
void
ags_syncsynth_update(AgsSyncsynth *syncsynth)
{
  AgsOscillator *oscillator;
  
  AgsAudio *audio;
  AgsChannel *start_input;
  AgsChannel *channel, *next_channel;

  AgsClearAudioSignal *clear_audio_signal;
  AgsApplySynth *apply_synth;

  AgsApplicationContext *application_context;
  
  GList *list, *list_start;
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

  guint sync_point_count;

  GRecMutex *synth_generator_mutex;

  application_context = ags_application_context_get_instance();

  audio = AGS_MACHINE(syncsynth)->audio;

  /*  */
  start_frequency = (gdouble) gtk_spin_button_get_value(syncsynth->lower);

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
    template = ags_audio_signal_get_default_template(start_list);

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
    list_start = ags_syncsynth_get_oscillator(syncsynth);

  /* get some fields */
  g_object_get(audio,
	       "input-lines", &input_lines,
	       "synth-generator", &start_synth_generator,
	       NULL);

  g_object_get(start_input,
	       "buffer-size", &buffer_size,
	       "format", &format,
	       NULL);

  loop_start = (guint) gtk_spin_button_get_value_as_int(syncsynth->loop_start);
  loop_end = (guint) gtk_spin_button_get_value_as_int(syncsynth->loop_end);

  requested_frame_count = 0;

  while(list != NULL){
    guint current_frame_count;

    oscillator = AGS_OSCILLATOR(list->data);

    current_frame_count = gtk_spin_button_get_value(oscillator->attack) + gtk_spin_button_get_value(oscillator->frame_count);

    if(requested_frame_count < current_frame_count){
      requested_frame_count = current_frame_count;
    }
    
    list = list->next;
  }

  list = list_start;
  
  synth_generator = start_synth_generator;

  /* get synth generator mutex */
  synth_generator_mutex = AGS_SYNTH_GENERATOR_GET_OBJ_MUTEX(synth_generator->data);

  while(list != NULL){
    guint i;
    gboolean do_sync;
    
    /* do it so */
    oscillator = AGS_OSCILLATOR(list->data);

    attack = (guint) gtk_spin_button_get_value_as_int(oscillator->attack);
    frame_count = (guint) gtk_spin_button_get_value_as_int(oscillator->frame_count);
    phase = (gdouble) gtk_spin_button_get_value(oscillator->phase);
    frequency = (gdouble) gtk_spin_button_get_value(oscillator->frequency);
    volume = (gdouble) gtk_spin_button_get_value(oscillator->volume);

    g_object_set(synth_generator->data,
		 "format", format,
		 "delay", (gdouble) attack / buffer_size,
		 "attack", attack,
		 "frame-count", frame_count,
		 "loop-start", loop_start,
		 "loop-end", loop_end,
		 "oscillator", gtk_combo_box_get_active(oscillator->wave),
		 "frequency", frequency,
		 "phase", phase,
		 "volume", volume,
		 NULL);

    do_sync = gtk_check_button_get_active(oscillator->do_sync);

    /* sync point */
    g_rec_mutex_lock(synth_generator_mutex);

    if(do_sync){
      sync_point_count = oscillator->sync_point_count;
      
      /* free previous sync point */
      g_free(AGS_SYNTH_GENERATOR(synth_generator->data)->sync_point);

      /* set new sync point */
      if(sync_point_count > 0){
	AGS_SYNTH_GENERATOR(synth_generator->data)->sync_point = (AgsComplex *) g_malloc(sync_point_count * sizeof(AgsComplex));
      }else{
	AGS_SYNTH_GENERATOR(synth_generator->data)->sync_point = NULL;
      }

      AGS_SYNTH_GENERATOR(synth_generator->data)->sync_point_count = sync_point_count;

      for(i = 0; i < sync_point_count; i++){
	AGS_SYNTH_GENERATOR(synth_generator->data)->sync_point[i].real = gtk_spin_button_get_value(oscillator->sync_point[2 * i]);
	AGS_SYNTH_GENERATOR(synth_generator->data)->sync_point[i].imag = gtk_spin_button_get_value(oscillator->sync_point[2 * i + 1]);
      }
    }else{
      g_free(AGS_SYNTH_GENERATOR(synth_generator->data)->sync_point);
      
      AGS_SYNTH_GENERATOR(synth_generator->data)->sync_point = NULL;
      AGS_SYNTH_GENERATOR(synth_generator->data)->sync_point_count = 0;
    }
    
    g_rec_mutex_unlock(synth_generator_mutex);
					
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
  
  g_list_free(list_start);

  ags_ui_provider_schedule_task_all(AGS_UI_PROVIDER(application_context),
				    g_list_reverse(task));
}

/**
 * ags_syncsynth_new:
 * @soundcard: the assigned soundcard.
 *
 * Create a new instance of #AgsSyncsynth
 *
 * Returns: the new #AgsSyncsynth
 *
 * Since: 3.0.0
 */
AgsSyncsynth*
ags_syncsynth_new(GObject *soundcard)
{
  AgsSyncsynth *syncsynth;

  syncsynth = (AgsSyncsynth *) g_object_new(AGS_TYPE_SYNCSYNTH,
					    NULL);

  g_object_set(AGS_MACHINE(syncsynth)->audio,
	       "output-soundcard", soundcard,
	       NULL);

  return(syncsynth);
}
