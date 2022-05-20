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

#include <ags/app/machine/ags_audiorec.h>
#include <ags/app/machine/ags_audiorec_callbacks.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_window.h>
#include <ags/app/ags_composite_editor.h>
#include <ags/app/ags_navigation.h>

#include <string.h>

#include <glib.h>
#include <glib/gstdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <ags/i18n.h>

void ags_audiorec_class_init(AgsAudiorecClass *audiorec);
void ags_audiorec_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_audiorec_init(AgsAudiorec *audiorec);
void ags_audiorec_finalize(GObject *gobject);

void ags_audiorec_connect(AgsConnectable *connectable);
void ags_audiorec_disconnect(AgsConnectable *connectable);

void ags_audiorec_show(GtkWidget *widget);

void ags_audiorec_map_recall(AgsMachine *machine);

void ags_audiorec_resize_audio_channels(AgsMachine *machine,
					guint audio_channels, guint audio_channels_old,
					gpointer data);
void ags_audiorec_resize_pads(AgsMachine *machine, GType channel_type,
			      guint pads, guint pads_old,
			      gpointer data);

void ags_audiorec_output_map_recall(AgsAudiorec *audiorec,
				    guint audio_channel_start,
				    guint output_pad_start);
void ags_audiorec_input_map_recall(AgsAudiorec *audiorec,
				   guint audio_channel_start,
				   guint input_pad_start);

void* ags_audiorec_fast_export_run(void *ptr);

/**
 * SECTION:ags_audiorec
 * @short_description: record audio data
 * @title: AgsAudiorec
 * @section_id:
 * @include: ags/app/machine/ags_audiorec.h
 *
 * The #AgsAudiorec is a composite widget to act as capture source.
 */

static gpointer ags_audiorec_parent_class = NULL;
static AgsConnectableInterface *ags_audiorec_parent_connectable_interface;

GHashTable *ags_audiorec_wave_loader_completed = NULL;
GHashTable *ags_audiorec_indicator_queue_draw = NULL;

struct _AgsAudiorecFastExport
{
  AgsAudio *audio;

  guint64 start_frame;
  guint64 end_frame;
  
  AgsAudioFile *audio_file;  
};

GType
ags_audiorec_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_audiorec = 0;

    static const GTypeInfo ags_audiorec_info = {
      sizeof(AgsAudiorecClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_audiorec_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsAudiorec),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_audiorec_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_audiorec_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_audiorec = g_type_register_static(AGS_TYPE_MACHINE,
					       "AgsAudiorec", &ags_audiorec_info,
					       0);
    
    g_type_add_interface_static(ags_type_audiorec,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_audiorec);
  }

  return g_define_type_id__volatile;
}

void
ags_audiorec_class_init(AgsAudiorecClass *audiorec)
{
  GObjectClass *gobject;
  GtkWidgetClass *widget;
  AgsMachineClass *machine;

  ags_audiorec_parent_class = g_type_class_peek_parent(audiorec);

  /* GObjectClass */
  gobject = (GObjectClass *) audiorec;

  gobject->finalize = ags_audiorec_finalize;

  /* GtkWidgetClass */
  widget = (GtkWidgetClass *) audiorec;

  widget->show = ags_audiorec_show;

  /* AgsMachineClass */
  machine = (AgsMachineClass *) audiorec;

  machine->map_recall = ags_audiorec_map_recall;
}

void
ags_audiorec_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_audiorec_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_audiorec_connect;
  connectable->disconnect = ags_audiorec_disconnect;
}

void
ags_audiorec_init(AgsAudiorec *audiorec)
{
  AgsWindow *window;
  AgsCompositeEditor *composite_editor;
  GtkBox *hbox;
  GtkBox *vbox;
  GtkBox *filename_hbox;
  GtkBox *radio_hbox;
  GtkFrame *frame;
  GtkLabel *label;

  AgsAudio *audio;
  AgsPlaybackDomain *playback_domain;

  AgsMachineCounterManager *machine_counter_manager;
  AgsMachineCounter *machine_counter;

  AgsApplicationContext *application_context;
  
  gchar *machine_name;

  gint position;
  guint i;

  static const guint staging_program[] = {
    (AGS_SOUND_STAGING_AUTOMATE | AGS_SOUND_STAGING_RUN_INTER | AGS_SOUND_STAGING_FX),
  };

  application_context = ags_application_context_get_instance();
  
  /* machine counter */
  machine_counter_manager = ags_machine_counter_manager_get_instance();

  machine_counter = ags_machine_counter_manager_find_machine_counter(machine_counter_manager,
								     AGS_TYPE_AUDIOREC);

  machine_name = NULL;

  if(machine_counter != NULL){
    machine_name = g_strdup_printf("Default %d",
				   machine_counter->counter);
  
    ags_machine_counter_increment(machine_counter);
  }
  
  g_object_set(audiorec,
	       "machine-name", machine_name,
	       NULL);

  g_free(machine_name);

  /* machine selector */
  window = ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));

  composite_editor = ags_ui_provider_get_composite_editor(AGS_UI_PROVIDER(application_context));

  position = g_list_length(window->machine);
  
  ags_machine_selector_popup_insert_machine(composite_editor->machine_selector,
					    position,
					    audiorec);
  
  audio = AGS_MACHINE(audiorec)->audio;

  playback_domain = NULL;

  ags_audio_set_flags(audio, (AGS_AUDIO_SYNC |
			      AGS_AUDIO_OUTPUT_HAS_RECYCLING |
			      AGS_AUDIO_INPUT_HAS_RECYCLING));
  ags_audio_set_ability_flags(audio, (AGS_SOUND_ABILITY_NOTATION));

  AGS_MACHINE(audiorec)->flags |= (AGS_MACHINE_IS_WAVE_PLAYER);

  /* audio resize */
  g_signal_connect_after(G_OBJECT(audiorec), "resize-audio-channels",
			 G_CALLBACK(ags_audiorec_resize_audio_channels), NULL);

  g_signal_connect_after(G_OBJECT(audiorec), "resize-pads",
			 G_CALLBACK(ags_audiorec_resize_pads), NULL);
  
  /* mapped IO */
  audiorec->mapped_output_audio_channel = 0;
  audiorec->mapped_input_audio_channel = 0;

  audiorec->mapped_input_pad = 0;
  audiorec->mapped_output_pad = 0;

  /* name and xml type */
  audiorec->name = NULL;
  audiorec->xml_type = "ags-audiorec";

  audiorec->playback_play_container = ags_recall_container_new();
  audiorec->playback_recall_container = ags_recall_container_new();

  audiorec->peak_play_container = ags_recall_container_new();
  audiorec->peak_recall_container = ags_recall_container_new();

  audiorec->buffer_play_container = ags_recall_container_new();
  audiorec->buffer_recall_container = ags_recall_container_new();

  AGS_MACHINE(audiorec)->connection_flags |= AGS_MACHINE_SHOW_AUDIO_INPUT_CONNECTION;

  /* hbox */
  hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				0);
  gtk_frame_set_child(AGS_MACHINE(audiorec)->frame,
		      (GtkWidget *) hbox);

  /* frame - filename and open */
  frame = (GtkFrame *) gtk_frame_new(i18n("file"));
  gtk_box_append(hbox,
		 (GtkWidget *) frame);

  vbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
				0);
  gtk_frame_set_child(frame,
		      (GtkWidget *) vbox);

  /* filename */
  filename_hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
					 0);
  gtk_box_append(vbox,
		 (GtkWidget *) filename_hbox);
  
  label = (GtkLabel *) gtk_label_new(i18n("filename: "));
  gtk_box_append(filename_hbox,
		 (GtkWidget *) label);

  audiorec->filename = (GtkEntry *) gtk_entry_new();
  gtk_box_append(filename_hbox,
		 (GtkWidget *) audiorec->filename);

  audiorec->open = (GtkButton *) gtk_button_new_with_mnemonic(i18n("_Open"));
  gtk_box_append(filename_hbox,
		 (GtkWidget *) audiorec->open);

  audiorec->wave_loader = NULL;

  audiorec->position = -1;

  audiorec->wave_loader_spinner = (GtkSpinner *) gtk_spinner_new();
  gtk_box_append(filename_hbox,
		 (GtkWidget *) audiorec->wave_loader_spinner);
  gtk_widget_hide((GtkWidget *) audiorec->wave_loader_spinner);
  
  /* radio */
  radio_hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				      0);
  gtk_box_append(vbox,
		 (GtkWidget *) radio_hbox);

  audiorec->keep_data = (GtkCheckButton *) gtk_check_button_new_with_label("keep");
  gtk_box_append(radio_hbox,
		 (GtkWidget *) audiorec->keep_data);

  audiorec->replace_data = (GtkCheckButton *) gtk_check_button_new_with_label("replace");
  gtk_check_button_set_group(audiorec->keep_data,
			     audiorec->replace_data);
  gtk_box_append(radio_hbox,
		 (GtkWidget *) audiorec->replace_data);
  
  audiorec->mix_data = (GtkCheckButton *) gtk_check_button_new_with_label("mix");
  gtk_check_button_set_group(audiorec->keep_data,
			     audiorec->mix_data);
  gtk_box_append(radio_hbox,
		 (GtkWidget *) audiorec->mix_data);
  
  /* frame - hindicator */
  frame = (GtkFrame *) gtk_frame_new(i18n("input"));

  gtk_widget_set_halign(frame,
			GTK_ALIGN_START);
  
  gtk_widget_set_hexpand(frame,
			 FALSE);

  gtk_box_append(hbox,
		 (GtkWidget *) frame);

  audiorec->indicator_vbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
						    0);
  gtk_frame_set_child(frame,
		      (GtkWidget *) audiorec->indicator_vbox);

  /* dialog */
  audiorec->open_dialog = NULL;

  /* wave_loader */
  if(ags_audiorec_wave_loader_completed == NULL){
    ags_audiorec_wave_loader_completed = g_hash_table_new_full(g_direct_hash, g_direct_equal,
							       NULL,
							       NULL);
  }

  g_hash_table_insert(ags_audiorec_wave_loader_completed,
		      audiorec, ags_audiorec_wave_loader_completed_timeout);
  g_timeout_add(1000 / 4, (GSourceFunc) ags_audiorec_wave_loader_completed_timeout, (gpointer) audiorec);

  /* indicator */
  if(ags_audiorec_indicator_queue_draw == NULL){
    ags_audiorec_indicator_queue_draw = g_hash_table_new_full(g_direct_hash, g_direct_equal,
							      NULL,
							      NULL);
  }

  g_hash_table_insert(ags_audiorec_indicator_queue_draw,
		      audiorec, ags_audiorec_indicator_queue_draw_timeout);
  g_timeout_add(AGS_UI_PROVIDER_DEFAULT_TIMEOUT * 1000.0, (GSourceFunc) ags_audiorec_indicator_queue_draw_timeout, (gpointer) audiorec);
}

void
ags_audiorec_finalize(GObject *gobject)
{
  AgsAudiorec *audiorec;

  audiorec = (AgsAudiorec *) gobject;

  g_hash_table_remove(ags_audiorec_wave_loader_completed,
		      audiorec);
  
  g_hash_table_remove(ags_audiorec_indicator_queue_draw,
		      audiorec);
  
  /* call parent */
  G_OBJECT_CLASS(ags_audiorec_parent_class)->finalize(gobject);
}

void
ags_audiorec_connect(AgsConnectable *connectable)
{
  AgsAudiorec *audiorec;

  if((AGS_CONNECTABLE_CONNECTED & (AGS_MACHINE(connectable)->connectable_flags)) != 0){
    return;
  }

  ags_audiorec_parent_connectable_interface->connect(connectable);

  audiorec = AGS_AUDIOREC(connectable);

  /* filename */
  g_signal_connect(audiorec->open, "clicked",
		   G_CALLBACK(ags_audiorec_open_callback), audiorec);

  /* mode */
  g_signal_connect_after(audiorec->keep_data, "toggled",
			 G_CALLBACK(ags_audiorec_keep_data_callback), audiorec);

  g_signal_connect_after(audiorec->mix_data, "toggled",
			 G_CALLBACK(ags_audiorec_mix_data_callback), audiorec);

  g_signal_connect_after(audiorec->replace_data, "toggled",
			 G_CALLBACK(ags_audiorec_replace_data_callback), audiorec);
}

void
ags_audiorec_disconnect(AgsConnectable *connectable)
{
  AgsAudiorec *audiorec;

  if((AGS_CONNECTABLE_CONNECTED & (AGS_MACHINE(connectable)->connectable_flags)) == 0){
    return;
  }

  ags_audiorec_parent_connectable_interface->disconnect(connectable);

  audiorec = AGS_AUDIOREC(connectable);

  /* filename */
  g_object_disconnect(audiorec->open,
		      "any_signal::clicked",
		      G_CALLBACK(ags_audiorec_open_callback),
		      audiorec,
		      NULL);

  /* mode */
  g_object_disconnect(audiorec->keep_data,
		      "any_signal::toggled",
		      G_CALLBACK(ags_audiorec_keep_data_callback),
		      audiorec,
		      NULL);

  g_object_disconnect(audiorec->replace_data,
		      "any_signal::toggled",
		      G_CALLBACK(ags_audiorec_replace_data_callback),
		      audiorec,
		      NULL);

  g_object_disconnect(audiorec->mix_data,
		      "any_signal::toggled",
		      G_CALLBACK(ags_audiorec_mix_data_callback),
		      audiorec,
		      NULL);
}

void
ags_audiorec_show(GtkWidget *widget)
{
  /* call parent */
  GTK_WIDGET_CLASS(ags_audiorec_parent_class)->show(widget);
}

void
ags_audiorec_resize_audio_channels(AgsMachine *machine,
				   guint audio_channels, guint audio_channels_old,
				   gpointer data)
{
  AgsAudiorec *audiorec;

  AgsApplicationContext *application_context;

  gdouble gui_scale_factor;
  guint i;
  guint j;
  
  application_context = ags_application_context_get_instance();

  audiorec = AGS_AUDIOREC(machine);
  
  /* scale factor */
  gui_scale_factor = ags_ui_provider_get_gui_scale_factor(AGS_UI_PROVIDER(application_context));
      
  if(audio_channels > audio_channels_old){    
    /* recall */
    if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0){
      ags_audiorec_input_map_recall(audiorec,
				    audio_channels_old,
				    0);

      ags_audiorec_output_map_recall(audiorec,
				     audio_channels_old,
				     0);
    }
    
    /* widgets */
    for(i = audio_channels_old; i < audio_channels; i++){
      AgsIndicator *hindicator;
	
      hindicator = ags_indicator_new(GTK_ORIENTATION_HORIZONTAL,
				     gui_scale_factor * AGS_AUDIOREC_DEFAULT_SEGMENT_WIDTH,
				     gui_scale_factor * AGS_AUDIOREC_DEFAULT_SEGMENT_HEIGHT);
      g_object_set(hindicator,
		   "segment-padding", (guint) (gui_scale_factor * AGS_INDICATOR_DEFAULT_SEGMENT_PADDING),
		   NULL);
      ags_audiorec_add_indicator(audiorec,
				 hindicator);

      gtk_widget_show((GtkWidget *) hindicator);
    }
  }else{
    GList *start_list, *list;
    
    list =
      start_list = ags_audiorec_get_indicator(audiorec);
    
    list = g_list_nth(start_list,
		      audio_channels);
    
    while(list != NULL){
      ags_audiorec_remove_indicator(audiorec,
				    list->data);
      
      g_object_run_dispose(list->data);
      g_object_unref(list->data);
      
      list = list->next;
    }

    g_list_free(start_list);
  }
}

void
ags_audiorec_resize_pads(AgsMachine *machine,
			 GType channel_type,
			 guint pads, guint pads_old,
			 gpointer data)
{
  AgsAudiorec *audiorec;

  AgsApplicationContext *application_context;

  guint i;
  guint j;
  gdouble gui_scale_factor;  
  
  application_context = ags_application_context_get_instance();

  audiorec = AGS_AUDIOREC(machine);
  
  /* scale factor */
  gui_scale_factor = ags_ui_provider_get_gui_scale_factor(AGS_UI_PROVIDER(application_context));
  
  if(g_type_is_a(channel_type, AGS_TYPE_INPUT)){
    if(pads > pads_old){
      /* depending on destination */
      ags_audiorec_input_map_recall(audiorec,
				    0,
				    pads_old);
    }else{
      GList *start_list, *list;

      list =
	start_list = ags_audiorec_get_indicator(audiorec);

      while(list != NULL){
	ags_audiorec_remove_indicator(audiorec,
				      list->data);
      
	g_object_run_dispose(list->data);
	g_object_unref(list->data);
      
	list = list->next;
      }

      g_list_free(start_list);

      audiorec->mapped_input_pad = pads;
    }
  }else{
    if(pads > pads_old){
      /* depending on destination */
      ags_audiorec_output_map_recall(audiorec,
				     0,
				     pads_old);
    }else{
      audiorec->mapped_output_pad = pads;
    }
  }
}

void
ags_audiorec_map_recall(AgsMachine *machine)
{
  AgsNavigation *navigation;
  AgsAudiorec *audiorec;
  
  AgsAudio *audio;

  AgsApplicationContext *application_context;

  GList *start_recall, *recall;

  gint position;

  if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0 ||
     (AGS_MACHINE_PREMAPPED_RECALL & (machine->flags)) != 0){
    return;
  }

  application_context = ags_application_context_get_instance();

  navigation = ags_ui_provider_get_navigation(AGS_UI_PROVIDER(application_context));

  audiorec = AGS_AUDIOREC(machine);
  
  audio = machine->audio;

  position = 0;

  /* ags-fx-playback */
  start_recall = ags_fx_factory_create(audio,
				       audiorec->playback_play_container, audiorec->playback_recall_container,
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

  /* ags-fx-peak */
  start_recall = ags_fx_factory_create(audio,
				       audiorec->peak_play_container, audiorec->peak_recall_container,
				       "ags-fx-peak",
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
				       audiorec->buffer_play_container, audiorec->buffer_recall_container,
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
  ags_audiorec_input_map_recall((AgsAudiorec *) machine,
				0,
				0);

  /* depending on destination */
  ags_audiorec_output_map_recall((AgsAudiorec *) machine,
				 0,
				 0);

  /* call parent */
  AGS_MACHINE_CLASS(ags_audiorec_parent_class)->map_recall(machine);  
}

void
ags_audiorec_output_map_recall(AgsAudiorec *audiorec,
			       guint audio_channel_start,
			       guint output_pad_start)
{  
  AgsAudio *audio;

  guint output_pads;
  guint audio_channels;

  audio = AGS_MACHINE(audiorec)->audio;
  
  /* get some fields */
  output_pads = AGS_MACHINE(audiorec)->output_pads;
  audio_channels = AGS_MACHINE(audiorec)->audio_channels;    
  
  audiorec->mapped_output_audio_channel = audio_channels;
  audiorec->mapped_output_pad = output_pads;
}

void
ags_audiorec_input_map_recall(AgsAudiorec *audiorec,
			      guint audio_channel_start,
			      guint input_pad_start)
{
  AgsAudio *audio;

  GList *start_recall;

  guint input_pads;
  guint audio_channels;
  gint position;
  guint i;
  guint j;
  
  audio = AGS_MACHINE(audiorec)->audio;

  /* get some fields */
  input_pads = AGS_MACHINE(audiorec)->input_pads;
  audio_channels = AGS_MACHINE(audiorec)->audio_channels;    

  position = 0;

  for(i = 0; i < input_pads; i++){
    for(j = 0; j < audio_channels; j++){
      AgsMachineInputLine *input_line;

      input_line = g_list_nth_data(AGS_MACHINE(audiorec)->machine_input_line,
				   (i * audio_channels) + j);

      if(input_line != NULL &&
	 input_line->mapped_recall == FALSE){
	/* ags-fx-playback */
	start_recall = ags_fx_factory_create(audio,
					     audiorec->playback_play_container, audiorec->playback_recall_container,
					     "ags-fx-playback",
					     NULL,
					     NULL,
					     j, j + 1,
					     i, i + 1,
					     position,
					     (AGS_FX_FACTORY_REMAP | AGS_FX_FACTORY_INPUT),
					     0);

	/* unref */
	g_list_free_full(start_recall,
			 (GDestroyNotify) g_object_unref);

	/* ags-fx-peak */
	start_recall = ags_fx_factory_create(audio,
					     audiorec->peak_play_container, audiorec->peak_recall_container,
					     "ags-fx-peak",
					     NULL,
					     NULL,
					     j, j + 1,
					     i, i + 1,
					     position,
					     (AGS_FX_FACTORY_REMAP | AGS_FX_FACTORY_INPUT),
					     0);

	/* unref */
	g_list_free_full(start_recall,
			 (GDestroyNotify) g_object_unref);

	/* ags-fx-buffer */
	start_recall = ags_fx_factory_create(audio,
					     audiorec->buffer_play_container, audiorec->buffer_recall_container,
					     "ags-fx-buffer",
					     NULL,
					     NULL,
					     j, j + 1,
					     i, i + 1,
					     position,
					     (AGS_FX_FACTORY_REMAP | AGS_FX_FACTORY_INPUT),
					     0);

	/* unref */
	g_list_free_full(start_recall,
			 (GDestroyNotify) g_object_unref);

	/* now input line is mapped */
	input_line->mapped_recall = TRUE;	
      }
    }
  }
  
  audiorec->mapped_input_audio_channel = audio_channels;
  audiorec->mapped_input_pad = input_pads;
}

/**
 * ags_audiorec_get_indicator:
 * @audiorec: the #AgsAudiorec
 * 
 * Get indicator.
 * 
 * Returns: the #GList-struct containing #AgsIndicator
 * 
 * Since: 4.0.0
 */
GList*
ags_audiorec_get_indicator(AgsAudiorec *audiorec)
{
  g_return_val_if_fail(AGS_IS_AUDIOREC(audiorec), NULL);

  return(g_list_reverse(g_list_copy(audiorec->indicator)));
}

/**
 * ags_audiorec_add_indicator:
 * @audiorec: the #AgsAudiorec
 * @indicator: the #AgsIndicator
 * 
 * Add @indicator to @audiorec.
 * 
 * Since: 4.0.0
 */
void
ags_audiorec_add_indicator(AgsAudiorec *audiorec,
			   AgsIndicator *indicator)
{
  g_return_if_fail(AGS_IS_AUDIOREC(audiorec));
  g_return_if_fail(AGS_IS_INDICATOR(indicator));

  if(g_list_find(audiorec->indicator, indicator) == NULL){
    audiorec->indicator = g_list_prepend(audiorec->indicator,
					 indicator);
    
    gtk_box_append(audiorec->indicator_vbox,
		   indicator);
  }  
}

/**
 * ags_audiorec_remove_indicator:
 * @audiorec: the #AgsAudiorec
 * @indicator: the #AgsIndicator
 * 
 * Remove @indicator from @audiorec.
 * 
 * Since: 4.0.0
 */
void
ags_audiorec_remove_indicator(AgsAudiorec *audiorec,
			      AgsIndicator *indicator)
{
  g_return_if_fail(AGS_IS_AUDIOREC(audiorec));
  g_return_if_fail(AGS_IS_INDICATOR(indicator));

  if(g_list_find(audiorec->indicator, indicator) != NULL){
    audiorec->indicator = g_list_remove(audiorec->indicator,
					indicator);
    
    gtk_box_remove(audiorec->indicator_vbox,
		   indicator);
  }  
}

/**
 * ags_audiorec_open_filename:
 * @audiorec: the #AgsAudiorec
 * @filename: the filename
 * 
 * Open @filename of @audiorec.
 * 
 * Since: 3.0.0
 */
void
ags_audiorec_open_filename(AgsAudiorec *audiorec,
			   gchar *filename)
{
  AgsWaveLoader *wave_loader;
  
  if(!AGS_IS_AUDIOREC(audiorec) ||
     filename == NULL ||
     strlen(filename) == 0){
    return;
  }
  
  audiorec->wave_loader = 
    wave_loader = ags_wave_loader_new(AGS_MACHINE(audiorec)->audio,
				      filename,
				      TRUE);

  ags_wave_loader_start(wave_loader);
}

void*
ags_audiorec_fast_export_run(void *ptr)
{
  AgsAudio *audio;
  AgsBuffer *buffer;
  AgsAudioFile *audio_file;

  AgsTimestamp *timestamp;

  struct _AgsAudiorecFastExport *fast_export;

  GList *start_wave, *end_wave, *wave;
  
  void *data;
  
  guint64 start_frame, end_frame;  
  guint default_offset;
  guint destination_offset, source_offset;
  guint audio_channels;
  guint copy_mode;
  guint samplerate;
  guint format;
  guint source_format;
  guint buffer_size;
  guint i, j;

  fast_export = ptr;

  audio = fast_export->audio;

  start_frame = fast_export->start_frame;
  end_frame = fast_export->end_frame;

  audio_file = fast_export->audio_file;
  
  g_object_get(audio,
	       "wave", &start_wave,
	       "audio-channels", &audio_channels,
	       "samplerate", &samplerate,
	       "format", &format,
	       "buffer-size", &buffer_size,	       
	       NULL);  
  
  default_offset = AGS_WAVE_DEFAULT_BUFFER_LENGTH * samplerate;

  timestamp = ags_timestamp_new();
  timestamp->flags = AGS_TIMESTAMP_OFFSET;
  
  data = ags_stream_alloc(audio_channels * buffer_size,
			  format);
  
  for(i = start_frame; i + buffer_size < end_frame; ){
    guint current_buffer_size;

    GRecMutex *buffer_mutex;
    
    ags_timestamp_set_ags_offset(timestamp,
				 default_offset * floor((gdouble) i / (gdouble) default_offset));
    
    ags_audio_buffer_util_clear_buffer(data, audio_channels,
				       buffer_size, ags_audio_buffer_util_format_from_soundcard(format));

    current_buffer_size = buffer_size;
    
    if(i == start_frame){
      source_offset = start_frame % buffer_size;

      current_buffer_size -= source_offset;
    }else{
      source_offset = 0;
    }

    if(default_offset * floor((gdouble) i / (gdouble) default_offset) < default_offset * floor((gdouble) (i + current_buffer_size) / (gdouble) default_offset)){
      current_buffer_size = (default_offset * floor((gdouble) (i + current_buffer_size) / (gdouble) default_offset)) - i;
    }
    
    for(j = 0; j < audio_channels; j++){
      wave = ags_wave_find_near_timestamp(start_wave, j,
					  timestamp);

      if(wave == NULL){
	continue;
      }

      buffer = ags_wave_find_point(wave->data,
				   i,
				   FALSE);
      
      if(buffer != NULL){
	g_object_get(buffer,
		     "format", &source_format,
		     NULL);
	
	copy_mode = ags_audio_buffer_util_get_copy_mode(ags_audio_buffer_util_format_from_soundcard(format),
							ags_audio_buffer_util_format_from_soundcard(source_format));
	
	buffer_mutex = AGS_BUFFER_GET_OBJ_MUTEX(buffer);

	destination_offset = j;

	g_rec_mutex_lock(buffer_mutex);
      
	ags_audio_buffer_util_copy_buffer_to_buffer(data, audio_channels, destination_offset,
						    buffer->data, 1, source_offset,
						    current_buffer_size, copy_mode);

	g_rec_mutex_unlock(buffer_mutex);
      }
    }
    
    ags_audio_file_write(audio_file,
			 data,
			 current_buffer_size,
			 format);
    
    i += current_buffer_size;
  }
  
  ags_audio_file_flush(audio_file);
  ags_audio_file_close(audio_file);

  g_thread_exit(NULL);

  return(NULL);
}

/**
 * ags_audiorec_fast_export:
 * @audiorec: the #AgsAudiorec
 * @filename: the filename
 * @start_frame: the start frame
 * @end_frame: the end frame
 * 
 * Fast export @audiorec.
 * 
 * Since: 3.5.7
 */
void
ags_audiorec_fast_export(AgsAudiorec *audiorec,
			 gchar *filename,
			 guint64 start_frame, guint64 end_frame)
{
  AgsAudioFile *audio_file;

  AgsApplicationContext *application_context;
  
  GObject *soundcard;

  GThread *fast_export_thread;
  
  struct _AgsAudiorecFastExport *fast_export;  

  guint audio_channels;
  guint samplerate;
  guint buffer_size;

  if(!AGS_IS_AUDIOREC(audiorec) ||
     filename == NULL){
    return;
  }

  application_context = ags_application_context_get_instance();

  soundcard = ags_sound_provider_get_default_soundcard(AGS_SOUND_PROVIDER(application_context));

  /* get some fields */
  audio_channels = 1;

  samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
  buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
  
  g_object_get(AGS_MACHINE(audiorec)->audio,
	       "audio-channels", &audio_channels,
	       "samplerate", &samplerate,
	       "buffer-size", &buffer_size,	       
	       NULL);

  if(g_file_test(filename,
		 G_FILE_TEST_EXISTS)){
    g_remove(filename);
  }

  audio_file = ags_audio_file_new(filename,
				  soundcard,
				  -1);

  audio_file->file_audio_channels = audio_channels;
  audio_file->file_samplerate = samplerate;  
  
  ags_audio_file_rw_open(audio_file,
			 TRUE);

  fast_export = g_malloc(sizeof(struct _AgsAudiorecFastExport));

  fast_export->audio = AGS_MACHINE(audiorec)->audio;

  fast_export->start_frame = start_frame;
  fast_export->end_frame = end_frame;

  fast_export->audio_file = audio_file;

  fast_export_thread = g_thread_new("Advanced Gtk+ Sequencer - fast export",
				    ags_audiorec_fast_export_run,
				    fast_export);

  g_thread_join(fast_export_thread);
}

/**
 * ags_audiorec_wave_loader_completed_timeout:
 * @audiorec: the #AgsAudiorec
 *
 * Queue draw widget
 *
 * Returns: %TRUE if proceed poll completed, otherwise %FALSE
 *
 * Since: 3.0.0
 */
gboolean
ags_audiorec_wave_loader_completed_timeout(AgsAudiorec *audiorec)
{
  if(g_hash_table_lookup(ags_audiorec_wave_loader_completed,
			 audiorec) != NULL){
    if(audiorec->wave_loader != NULL){
      if(ags_wave_loader_test_flags(audiorec->wave_loader, AGS_WAVE_LOADER_HAS_COMPLETED)){
	g_object_unref(audiorec->wave_loader);

	audiorec->wave_loader = NULL;

	audiorec->position = -1;

	gtk_spinner_stop(audiorec->wave_loader_spinner);
	gtk_widget_hide((GtkWidget *) audiorec->wave_loader_spinner);
      }else{
	if(audiorec->position == -1){
	  audiorec->position = 0;

	  gtk_widget_show((GtkWidget *) audiorec->wave_loader_spinner);
	  gtk_spinner_start(audiorec->wave_loader_spinner);
	}
      }
    }
    
    return(TRUE);
  }else{
    return(FALSE);
  }
}

/**
 * ags_audiorec_indicator_queue_draw_timeout:
 * @audiorec: the #AgsAudiorec
 *
 * Queue draw widget
 *
 * Returns: %TRUE if proceed with redraw, otherwise %FALSE
 *
 * Since: 3.0.0
 */
gboolean
ags_audiorec_indicator_queue_draw_timeout(AgsAudiorec *audiorec)
{
  if(g_hash_table_lookup(ags_audiorec_indicator_queue_draw,
			 audiorec) != NULL){
    AgsAudio *audio;
    AgsChannel *start_channel;
    AgsChannel *channel, *next_channel;
    
    GList *start_list, *list;

    guint i;

    audio = AGS_MACHINE(audiorec)->audio;
    g_object_get(audio,
		 "input", &start_channel,
		 NULL);
    
    list =
      start_list = ags_audiorec_get_indicator(audiorec);
    
    /* check members */
    channel = start_channel;

    if(channel != NULL){
      g_object_ref(channel);
    }

    next_channel = NULL;
    
    for(i = 0; list != NULL; i++){
      GtkAdjustment *adjustment;
      GtkWidget *child;

      AgsPort *current;

      GList *start_port;
      
      gdouble average_peak;
      gdouble peak;
	
      GValue value = {0,};
	
      child = list->data;
      
      average_peak = 0.0;
      
      start_port = ags_channel_collect_all_channel_ports_by_specifier_and_context(channel,
										  "./peak[0]",
										  FALSE);

      current = NULL;

      if(start_port != NULL){
	current = start_port->data;
      }
      
      /* recall port - read value */
      g_value_init(&value, G_TYPE_FLOAT);
      ags_port_safe_read(current,
			 &value);
      
      peak = g_value_get_float(&value);
      g_value_unset(&value);

      /* calculate peak */
      average_peak += peak;
      
      /* apply */
      g_object_get(child,
		   "adjustment", &adjustment,
		   NULL);
	
      gtk_adjustment_set_value(adjustment,
			       10.0 * average_peak);

      /* queue draw */
      gtk_widget_queue_draw(child);

      g_list_free_full(start_port,
		       g_object_unref);
      
      /* iterate */
      list = list->next;

      next_channel = ags_channel_next(channel);

      g_object_unref(channel);

      channel = next_channel;
    }

    if(start_channel != NULL){
      g_object_unref(start_channel);
    }

    if(channel != NULL){
      g_object_unref(channel);
    }
    
    g_list_free(start_list);
    
    return(TRUE);
  }else{
    return(FALSE);
  }
}

/**
 * ags_audiorec_new:
 * @output_soundcard: the assigned output soundcard.
 *
 * Create a new instance of #AgsAudiorec
 *
 * Returns: the new #AgsAudiorec
 *
 * Since: 3.0.0
 */
AgsAudiorec*
ags_audiorec_new(GObject *output_soundcard)
{
  AgsAudiorec *audiorec;

  audiorec = (AgsAudiorec *) g_object_new(AGS_TYPE_AUDIOREC,
					  NULL);

  g_object_set(G_OBJECT(AGS_MACHINE(audiorec)->audio),
	       "output-soundcard", output_soundcard,
	       NULL);

  return(audiorec);
}
