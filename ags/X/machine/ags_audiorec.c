/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2020 Joël Krähemann
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

#include <ags/X/machine/ags_audiorec.h>
#include <ags/X/machine/ags_audiorec_callbacks.h>

#include <ags/X/ags_ui_provider.h>
#include <ags/X/ags_window.h>

#include <ags/i18n.h>

void ags_audiorec_class_init(AgsAudiorecClass *audiorec);
void ags_audiorec_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_audiorec_init(AgsAudiorec *audiorec);
void ags_audiorec_finalize(GObject *gobject);

void ags_audiorec_connect(AgsConnectable *connectable);
void ags_audiorec_disconnect(AgsConnectable *connectable);

void ags_audiorec_show(GtkWidget *widget);
void ags_audiorec_show_all(GtkWidget *widget);

void ags_audiorec_map_recall(AgsMachine *machine);

void ags_audiorec_resize_audio_channels(AgsMachine *machine,
					guint audio_channels, guint audio_channels_old,
					gpointer data);
void ags_audiorec_resize_pads(AgsMachine *machine, GType type,
			      guint pads, guint pads_old,
			      gpointer data);

void ags_audiorec_output_map_recall(AgsAudiorec *audiorec,
				    guint audio_channel_start,
				    guint output_pad_start);
void ags_audiorec_input_map_recall(AgsAudiorec *audiorec,
				   guint audio_channel_start,
				   guint input_pad_start);

/**
 * SECTION:ags_audiorec
 * @short_description: record audio data
 * @title: AgsAudiorec
 * @section_id:
 * @include: ags/X/machine/ags_audiorec.h
 *
 * The #AgsAudiorec is a composite widget to act as capture source.
 */

static gpointer ags_audiorec_parent_class = NULL;
static AgsConnectableInterface *ags_audiorec_parent_connectable_interface;

GHashTable *ags_audiorec_wave_loader_completed = NULL;
GHashTable *ags_audiorec_indicator_queue_draw = NULL;

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
  widget->show_all = ags_audiorec_show_all;

  /* AgsMachineClass */
  machine = (AgsMachineClass *) audiorec;

  machine->map_recall = ags_audiorec_map_recall;
}

void
ags_audiorec_connectable_interface_init(AgsConnectableInterface *connectable)
{
  AgsConnectableInterface *ags_audiorec_connectable_parent_interface;

  ags_audiorec_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_audiorec_connect;
}

void
ags_audiorec_init(AgsAudiorec *audiorec)
{
  GtkHBox *hbox;
  GtkHBox *vbox;
  GtkHBox *filename_hbox;
  GtkHBox *radio_hbox;
  GtkFrame *frame;
  GtkLabel *label;

  AgsAudio *audio;
  AgsPlaybackDomain *playback_domain;

  guint i;

  static const guint staging_program[] = {
    (AGS_SOUND_STAGING_AUTOMATE | AGS_SOUND_STAGING_RUN_INTER | AGS_SOUND_STAGING_FX),
  };
    
  g_signal_connect_after((GObject *) audiorec, "parent_set",
			 G_CALLBACK(ags_audiorec_parent_set_callback), (gpointer) audiorec);

  audio = AGS_MACHINE(audiorec)->audio;

  playback_domain = NULL;

  ags_audio_set_flags(audio, (AGS_AUDIO_SYNC |
			      AGS_AUDIO_OUTPUT_HAS_RECYCLING |
			      AGS_AUDIO_INPUT_HAS_RECYCLING));
  ags_audio_set_ability_flags(audio, (AGS_SOUND_ABILITY_WAVE));

  g_object_get(audio,
	       "playback-domain", &playback_domain,
	       NULL);

  if(playback_domain != NULL){
    for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
      AgsThread *audio_thread;
      
      audio_thread = ags_playback_domain_get_audio_thread(playback_domain,
							  i);
      
      if(audio_thread != NULL){
	ags_audio_thread_set_do_fx_staging(audio_thread, TRUE);
	ags_audio_thread_set_staging_program(audio_thread,
					     staging_program,
					     1);

	g_object_unref(audio_thread);
      }
    }
    
    g_object_unref(playback_domain);
  }
  
  AGS_MACHINE(audiorec)->flags |= (AGS_MACHINE_IS_WAVE_PLAYER);

  /* audio resize */
  g_signal_connect_after(G_OBJECT(audiorec), "resize-audio-channels",
			 G_CALLBACK(ags_audiorec_resize_audio_channels), NULL);

  g_signal_connect_after(G_OBJECT(audiorec), "resize-pads",
			 G_CALLBACK(ags_audiorec_resize_pads), NULL);
  
  /* mapped IO */
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

  /* context menu */
  ags_machine_popup_add_connection_options((AgsMachine *) audiorec,
					   (AGS_MACHINE_POPUP_CONNECTION_EDITOR));

  AGS_MACHINE(audiorec)->connection_flags |= AGS_MACHINE_SHOW_AUDIO_INPUT_CONNECTION;
  
  ags_machine_popup_add_export_options((AgsMachine *) audiorec,
				       (AGS_MACHINE_POPUP_WAVE_EXPORT));

  /* hbox */
  hbox = (GtkHBox *) gtk_hbox_new(FALSE,
				  0);
  gtk_container_add((GtkContainer *) (gtk_bin_get_child((GtkBin *) audiorec)),
		    (GtkWidget *) hbox);

  /* frame - filename and open */
  frame = (GtkFrame *) gtk_frame_new(i18n("file"));
  gtk_box_pack_start((GtkBox *) hbox,
		     (GtkWidget *) frame,
		     FALSE, FALSE,
		     0);

  vbox = (GtkHBox *) gtk_vbox_new(FALSE,
				  0);
  gtk_container_add((GtkContainer *) frame,
		    (GtkWidget *) vbox);

  /* filename */
  filename_hbox = (GtkHBox *) gtk_hbox_new(FALSE,
					   0);
  gtk_box_pack_start((GtkBox *) vbox,
		     (GtkWidget *) filename_hbox,
		     FALSE, FALSE,
		     0);
  
  label = (GtkLabel *) gtk_label_new(i18n("filename: "));
  gtk_box_pack_start((GtkBox *) filename_hbox,
		     (GtkWidget *) label,
		     FALSE, FALSE,
		     0);

  audiorec->filename = (GtkEntry *) gtk_entry_new();
  gtk_box_pack_start((GtkBox *) filename_hbox,
		     (GtkWidget *) audiorec->filename,
		     FALSE, FALSE,
		     0);

  audiorec->open = (GtkButton *) gtk_button_new_from_stock(GTK_STOCK_OPEN);
  gtk_box_pack_start((GtkBox *) filename_hbox,
		     (GtkWidget *) audiorec->open,
		     FALSE, FALSE,
		     0);

  audiorec->wave_loader = NULL;

  audiorec->position = -1;

  audiorec->loading = (GtkLabel *) gtk_label_new(i18n("loading ...  "));
  gtk_box_pack_start((GtkBox *) filename_hbox,
		     (GtkWidget *) audiorec->loading,
		     FALSE, FALSE,
		     0);
  gtk_widget_set_no_show_all((GtkWidget *) audiorec->loading,
			     TRUE);
  gtk_widget_hide((GtkWidget *) audiorec->loading);
  
  /* radio */
  radio_hbox = (GtkHBox *) gtk_hbox_new(FALSE,
					0);
  gtk_box_pack_start((GtkBox *) vbox,
		     (GtkWidget *) radio_hbox,
		     FALSE, FALSE,
		     0);

  audiorec->keep_data = (GtkRadioButton *) gtk_radio_button_new_with_label_from_widget(NULL,
										       "keep");
  gtk_box_pack_start((GtkBox *) radio_hbox,
		     (GtkWidget *) audiorec->keep_data,
		     FALSE, FALSE,
		     0);

  audiorec->replace_data = (GtkRadioButton *) gtk_radio_button_new_with_label_from_widget(audiorec->keep_data,
											  "replace");
  gtk_box_pack_start((GtkBox *) radio_hbox,
		     (GtkWidget *) audiorec->replace_data,
		     FALSE, FALSE,
		     0);
  
  audiorec->mix_data = (GtkRadioButton *) gtk_radio_button_new_with_label_from_widget(audiorec->keep_data,
										      "mix");
  gtk_box_pack_start((GtkBox *) radio_hbox,
		     (GtkWidget *) audiorec->mix_data,
		     FALSE, FALSE,
		     0);
  
  /* frame - hindicator */
  frame = (GtkFrame *) gtk_frame_new(i18n("input"));
  gtk_box_pack_start((GtkBox *) hbox,
		     (GtkWidget *) frame,
		     FALSE, FALSE,
		     0);

  audiorec->hindicator_vbox = (GtkVBox *) gtk_vbox_new(FALSE,
						       0);
  gtk_container_add((GtkContainer *) frame,
		    (GtkWidget *) audiorec->hindicator_vbox);

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
  AgsWindow *window;
  AgsAudiorec *audiorec;

  GList *list;

  if((AGS_MACHINE_CONNECTED & (AGS_MACHINE(connectable)->flags)) != 0){
    return;
  }

  ags_audiorec_parent_connectable_interface->connect(connectable);

  audiorec = AGS_AUDIOREC(connectable);

  /* filename */
  g_signal_connect(audiorec->open, "clicked",
		   G_CALLBACK(ags_audiorec_open_callback), audiorec);

  /* mode */
  g_signal_connect_after(audiorec->keep_data, "clicked",
			 G_CALLBACK(ags_audiorec_keep_data_callback), audiorec);

  g_signal_connect_after(audiorec->mix_data, "clicked",
			 G_CALLBACK(ags_audiorec_mix_data_callback), audiorec);

  g_signal_connect_after(audiorec->replace_data, "clicked",
			 G_CALLBACK(ags_audiorec_replace_data_callback), audiorec);
}

void
ags_audiorec_disconnect(AgsConnectable *connectable)
{
  AgsAudiorec *audiorec;

  GList *list;

  if((AGS_MACHINE_CONNECTED & (AGS_MACHINE(connectable)->flags)) == 0){
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
		      "any_signal::clicked",
		      G_CALLBACK(ags_audiorec_keep_data_callback),
		      audiorec,
		      NULL);

  g_object_disconnect(audiorec->replace_data,
		      "any_signal::clicked",
		      G_CALLBACK(ags_audiorec_replace_data_callback),
		      audiorec,
		      NULL);

  g_object_disconnect(audiorec->mix_data,
		      "any_signal::clicked",
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
ags_audiorec_show_all(GtkWidget *widget)
{  
  /* call parent */
  GTK_WIDGET_CLASS(ags_audiorec_parent_class)->show_all(widget);
}


void
ags_audiorec_resize_audio_channels(AgsMachine *machine,
				   guint audio_channels, guint audio_channels_old,
				   gpointer data)
{
  AgsAudiorec *audiorec;
  
  AgsAudio *audio;
  AgsPlayback *playback;
  AgsChannel *start_output;
  AgsChannel *start_input;
  AgsChannel *channel, *next_pad, *next_channel;

  AgsConfig *config;

  GList *start_recall;
  
  gchar *str;

  gdouble gui_scale_factor;  
  guint output_pads, input_pads;
  guint i;
  
  static const guint staging_program[] = {
    (AGS_SOUND_STAGING_AUTOMATE | AGS_SOUND_STAGING_RUN_INTER | AGS_SOUND_STAGING_FX),
  };
  
  config = ags_config_get_instance();

  audiorec = AGS_AUDIOREC(machine);
  
  audio = machine->audio;

  start_output = NULL;
  start_input = NULL;
  
  output_pads = 0;
  input_pads = 0;

  g_object_get(audio,
	       "output", &start_output,
	       "input", &start_input,
	       "input-pads", &input_pads,
	       "output-pads", &output_pads,
	       NULL);

  /* scale factor */
  gui_scale_factor = 1.0;
  
  str = ags_config_get_value(config,
			     AGS_CONFIG_GENERIC,
			     "gui-scale");

  if(str != NULL){	  
    gui_scale_factor = g_ascii_strtod(str,
				      NULL);

    g_free(str);
  }
      
  if(audio_channels > audio_channels_old){
    /* AgsOutput */
    channel = start_output;

    if(channel != NULL){
      g_object_ref(channel);
    }

    next_pad = NULL;
    
    while(channel != NULL){      
      /* get some fields */
      next_pad = ags_channel_next_pad(channel),
      
      next_channel = ags_channel_pad_nth(channel,
					 audio_channels_old);

      if(channel != NULL){
	g_object_unref(channel);
      }
      
      channel = next_channel;
      
      while(channel != next_pad){
	/* ability */
	ags_channel_set_ability_flags(channel, (AGS_SOUND_ABILITY_WAVE));

	/* fx engine */
	g_object_get(channel,
		     "playback", &playback,
		     NULL);

	if(playback != NULL){
	  for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
	    AgsThread *channel_thread;
	  
	    channel_thread = ags_playback_get_channel_thread(playback,
							     i);

	    if(channel_thread != NULL){
	      ags_channel_thread_set_do_fx_staging(channel_thread, TRUE);
	      ags_channel_thread_set_staging_program(channel_thread,
						     staging_program,
						     1);
	    
	      g_object_unref(channel_thread);
	    }
	  }
	
	  g_object_unref(playback);
	}
	
	/* iterate */
	next_channel = ags_channel_next(channel);

	g_object_unref(channel);

	channel = next_channel;
      }

      if(next_pad != NULL){
	g_object_unref(next_pad);
      }
    }

    if(channel != NULL){
      g_object_unref(channel);
    }

    /* AgsInput */
    channel = start_input;

    if(channel != NULL){
      g_object_ref(channel);
    }

    next_pad = NULL;
    
    while(channel != NULL){      
      /* get some fields */
      next_pad = ags_channel_next_pad(channel),
      
      next_channel = ags_channel_pad_nth(channel,
					 audio_channels_old);

      if(channel != NULL){
	g_object_unref(channel);
      }
      
      channel = next_channel;
      
      while(channel != next_pad){
	/* ability */
	ags_channel_set_ability_flags(channel, (AGS_SOUND_ABILITY_WAVE));

	/* fx engine */
	g_object_get(channel,
		     "playback", &playback,
		     NULL);

	if(playback != NULL){
	  for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
	    AgsThread *channel_thread;
	  
	    channel_thread = ags_playback_get_channel_thread(playback,
							     i);

	    if(channel_thread != NULL){
	      ags_channel_thread_set_do_fx_staging(channel_thread, TRUE);
	      ags_channel_thread_set_staging_program(channel_thread,
						     staging_program,
						     1);
	    
	      g_object_unref(channel_thread);
	    }
	  }
	
	  g_object_unref(playback);
	}
	
	/* iterate */
	next_channel = ags_channel_next(channel);

	g_object_unref(channel);

	channel = next_channel;
      }

      if(next_pad != NULL){
	g_object_unref(next_pad);
      }
    }

    if(channel != NULL){
      g_object_unref(channel);
    }
    
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
      AgsHIndicator *hindicator;
	
      hindicator = ags_hindicator_new();
      g_object_set(hindicator,
		   "segment-width", (guint) (gui_scale_factor * AGS_HINDICATOR_DEFAULT_SEGMENT_WIDTH),
		   "segment-height", (guint) (gui_scale_factor * AGS_HINDICATOR_DEFAULT_SEGMENT_HEIGHT),
		   "segment-padding", (guint) (gui_scale_factor * AGS_INDICATOR_DEFAULT_SEGMENT_PADDING),
		   NULL);
      gtk_box_pack_start((GtkBox *) audiorec->hindicator_vbox,
			 (GtkWidget *) hindicator,
			 FALSE, FALSE,
			 8);
    }

    gtk_widget_show_all((GtkWidget *) audiorec->hindicator_vbox);
  }else{
    GList *list, *list_start;

    list_start =
      list = gtk_container_get_children((GtkContainer *) audiorec->hindicator_vbox);

    list = g_list_nth(list_start,
		      audio_channels);
    
    while(list != NULL){
      gtk_widget_destroy((GtkWidget *) list->data);
      
      list = list->next;
    }

    g_list_free(list_start);
  }

  /* unref */
  if(start_output != NULL){
    g_object_unref(start_output);
  }
}

void
ags_audiorec_resize_pads(AgsMachine *machine,
			 GType type,
			 guint pads, guint pads_old,
			 gpointer data)
{
  AgsAudiorec *audiorec;

  AgsAudio *audio;
  AgsPlayback *playback;
  AgsChannel *start_output;
  AgsChannel *start_input;
  AgsChannel *channel, *next_channel;

  AgsConfig *config;    

  gchar *str;

  gdouble gui_scale_factor;  
  guint audio_channels;
  guint i;
  
  static const guint staging_program[] = {
    (AGS_SOUND_STAGING_AUTOMATE | AGS_SOUND_STAGING_RUN_INTER | AGS_SOUND_STAGING_FX),
  };

  config = ags_config_get_instance();

  audiorec = AGS_AUDIOREC(machine);

  audio = machine->audio;

  start_output = NULL;
  start_input = NULL;
  
  audio_channels = 0;
  
  /* scale factor */
  gui_scale_factor = 1.0;
  
  str = ags_config_get_value(config,
			     AGS_CONFIG_GENERIC,
			     "gui-scale");

  if(str != NULL){
    gui_scale_factor = g_ascii_strtod(str,
				      NULL);

    g_free(str);
  }

  if(g_type_is_a(type, AGS_TYPE_INPUT)){
    if(pads > pads_old){
      /* get some fields */
      g_object_get(audio,
		   "input", &start_input,
		   "audio-channels", &audio_channels,
		   NULL);

      /* AgsOutput */
      channel = ags_channel_pad_nth(start_input,
				    pads_old);
      
      while(channel != NULL){
	ags_channel_set_ability_flags(channel, (AGS_SOUND_ABILITY_WAVE));

	/* fx engine */
	g_object_get(channel,
		     "playback", &playback,
		     NULL);

	if(playback != NULL){
	  for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
	    AgsThread *channel_thread;
	  
	    channel_thread = ags_playback_get_channel_thread(playback,
							     i);

	    if(channel_thread != NULL){
	      ags_channel_thread_set_do_fx_staging(channel_thread, TRUE);
	      ags_channel_thread_set_staging_program(channel_thread,
						     staging_program,
						     1);
	    
	      g_object_unref(channel_thread);
	    }
	  }
	
	  g_object_unref(playback);
	}
	
	/* iterate */
	next_channel = ags_channel_next(channel);

	g_object_unref(channel);

	channel = next_channel;
      }

      if(start_input != NULL){
	g_object_unref(start_input);
      }

      if(channel != NULL){
	g_object_unref(channel);
      }
      
      for(i = 0; i < audio_channels; i++){
	AgsHIndicator *hindicator;

	hindicator = ags_hindicator_new();
	g_object_set(hindicator,
		     "segment-width", (guint) (gui_scale_factor * AGS_HINDICATOR_DEFAULT_SEGMENT_WIDTH),
		     "segment-height", (guint) (gui_scale_factor * AGS_HINDICATOR_DEFAULT_SEGMENT_HEIGHT),
		     "segment-padding", (guint) (gui_scale_factor * AGS_INDICATOR_DEFAULT_SEGMENT_PADDING),
		     NULL);
	gtk_box_pack_start((GtkBox *) audiorec->hindicator_vbox,
			   (GtkWidget *) hindicator,
			   FALSE, FALSE,
			   8);
      }

      gtk_widget_show_all((GtkWidget *) audiorec->hindicator_vbox);

      /* depending on destination */
      ags_audiorec_input_map_recall(audiorec,
				    0,
				    pads_old);
    }else{
      GList *list, *list_start;

      list_start = 
	list = gtk_container_get_children((GtkContainer *) audiorec->hindicator_vbox);

      while(list != NULL){
	gtk_widget_destroy((GtkWidget *) list->data);
      
	list = list->next;
      }

      g_list_free(list_start);

      audiorec->mapped_input_pad = pads;
    }
  }else{
    if(pads > pads_old){
      /* get some fields */
      g_object_get(audio,
		   "output", &start_output,
		   "audio-channels", &audio_channels,
		   NULL);

      /* AgsOutput */
      channel = ags_channel_pad_nth(start_output,
				    pads_old);
      
      while(channel != NULL){
	ags_channel_set_ability_flags(channel, (AGS_SOUND_ABILITY_WAVE));

	/* fx engine */
	g_object_get(channel,
		     "playback", &playback,
		     NULL);

	if(playback != NULL){
	  for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
	    AgsThread *channel_thread;
	  
	    channel_thread = ags_playback_get_channel_thread(playback,
							     i);

	    if(channel_thread != NULL){
	      ags_channel_thread_set_do_fx_staging(channel_thread, TRUE);
	      ags_channel_thread_set_staging_program(channel_thread,
						     staging_program,
						     1);
	    
	      g_object_unref(channel_thread);
	    }
	  }
	
	  g_object_unref(playback);
	}
	
	/* iterate */
	next_channel = ags_channel_next(channel);

	g_object_unref(channel);

	channel = next_channel;
      }

      if(start_input != NULL){
	g_object_unref(start_input);
      }

      if(channel != NULL){
	g_object_unref(channel);
      }

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
  AgsAudio *audio;

  GList *start_recall, *recall;

  gint position;

  if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0 ||
     (AGS_MACHINE_PREMAPPED_RECALL & (machine->flags)) != 0){
    return;
  }

  audio = machine->audio;

  position = 0;
  
  /* ags-fx-playback */
  start_recall = ags_fx_factory_create(audio,
				       AGS_AUDIOREC(machine)->playback_play_container, AGS_AUDIOREC(machine)->playback_recall_container,
				       "ags-fx-playback",
				       NULL,
				       NULL,
				       0, 0,
				       0, 0,
				       position,
				       (AGS_FX_FACTORY_ADD),
				       0);

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);

  /* ags-fx-peak */
  start_recall = ags_fx_factory_create(audio,
				       AGS_AUDIOREC(machine)->peak_play_container, AGS_AUDIOREC(machine)->peak_recall_container,
				       "ags-fx-peak",
				       NULL,
				       NULL,
				       0, 0,
				       0, 0,
				       position,
				       (AGS_FX_FACTORY_ADD),
				       0);

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);

  /* ags-fx-buffer */
  start_recall = ags_fx_factory_create(audio,
				       AGS_AUDIOREC(machine)->buffer_play_container, AGS_AUDIOREC(machine)->buffer_recall_container,
				       "ags-fx-buffer",
				       NULL,
				       NULL,
				       0, 0,
				       0, 0,
				       position,
				       (AGS_FX_FACTORY_ADD),
				       0);

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
  
  /* depending on destination */
  ags_audiorec_input_map_recall(machine,
				0,
				0);

  /* depending on destination */
  ags_audiorec_output_map_recall(machine,
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

  if(audiorec->mapped_output_pad > output_pad_start){
    return;
  }

  audio = AGS_MACHINE(audiorec)->audio;
  
  /* get some fields */
  g_object_get(audio,
	       "output-pads", &output_pads,
	       NULL);
  
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

  if(audiorec->mapped_input_pad > input_pad_start){
    return;
  }

  audio = AGS_MACHINE(audiorec)->audio;

  /* get some fields */
  g_object_get(audio,
	       "input-pads", &input_pads,
	       "audio-channels", &audio_channels,
	       NULL);

  position = 0;

  /* ags-fx-playback */
  start_recall = ags_fx_factory_create(audio,
				       audiorec->playback_play_container, audiorec->playback_recall_container,
				       "ags-fx-playback",
				       NULL,
				       NULL,
				       audio_channel_start, audio_channels,
				       input_pad_start, input_pads,
				       position,
				       (AGS_FX_FACTORY_REMAP),
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
				       audio_channel_start, audio_channels,
				       input_pad_start, input_pads,
				       position,
				       (AGS_FX_FACTORY_REMAP),
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
				       audio_channel_start, audio_channels,
				       input_pad_start, input_pads,
				       position,
				       (AGS_FX_FACTORY_REMAP),
				       0);

  /* unref */
  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
  
  audiorec->mapped_input_pad = input_pads;
}

void
ags_audiorec_open_filename(AgsAudiorec *audiorec,
			   gchar *filename)
{
  AgsWaveLoader *wave_loader;
  
  if(!AGS_IS_AUDIOREC(audiorec) ||
     filename == NULL){
    return;
  }

  audiorec->wave_loader = 
    wave_loader = ags_wave_loader_new(AGS_MACHINE(audiorec)->audio,
				      filename,
				      TRUE);

  ags_wave_loader_start(wave_loader);
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
	g_object_run_dispose((GObject *) audiorec->wave_loader);
	g_object_unref(audiorec->wave_loader);

	audiorec->wave_loader = NULL;

	audiorec->position = -1;
	gtk_widget_hide((GtkWidget *) audiorec->loading);
      }else{
	if(audiorec->position == -1){
	  audiorec->position = 0;

	  gtk_widget_show((GtkWidget *) audiorec->loading);
	}

	switch(audiorec->position){
	case 0:
	  {
	    audiorec->position = 1;
	    
	    gtk_label_set_label(audiorec->loading,
				"loading ...  ");
	  }
	  break;
	case 1:
	  {
	    audiorec->position = 2;

	    gtk_label_set_label(audiorec->loading,
				"loading  ... ");
	  }
	  break;
	case 2:
	  {
	    audiorec->position = 0;

	    gtk_label_set_label(audiorec->loading,
				"loading   ...");
	  }
	  break;
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
    
    GList *list, *list_start;

    guint i;

    audio = AGS_MACHINE(audiorec)->audio;
    g_object_get(audio,
		 "input", &start_channel,
		 NULL);
    
    list_start = 
      list = gtk_container_get_children((GtkContainer *) audiorec->hindicator_vbox);
    
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

    if(next_channel != NULL){
      g_object_unref(next_channel);
    }
    
    g_list_free(list_start);
    
    return(TRUE);
  }else{
    return(FALSE);
  }
}

/**
 * ags_audiorec_new:
 * @soundcard: the assigned soundcard.
 *
 * Create a new instance of #AgsAudiorec
 *
 * Returns: the new #AgsAudiorec
 *
 * Since: 3.0.0
 */
AgsAudiorec*
ags_audiorec_new(GObject *soundcard)
{
  AgsAudiorec *audiorec;

  audiorec = (AgsAudiorec *) g_object_new(AGS_TYPE_AUDIOREC,
					  NULL);

  g_object_set(G_OBJECT(AGS_MACHINE(audiorec)->audio),
	       "output-soundcard", soundcard,
	       NULL);

  return(audiorec);
}
