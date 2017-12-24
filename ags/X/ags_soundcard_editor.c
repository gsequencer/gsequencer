/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2017 Joël Krähemann
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

#include <ags/X/ags_soundcard_editor.h>
#include <ags/X/ags_soundcard_editor_callbacks.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <ags/X/ags_xorg_application_context.h>
#include <ags/X/ags_window.h>
#include <ags/X/ags_preferences.h>

#include <ags/config.h>
#include <ags/i18n.h>

void ags_soundcard_editor_class_init(AgsSoundcardEditorClass *soundcard_editor);
void ags_soundcard_editor_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_soundcard_editor_applicable_interface_init(AgsApplicableInterface *applicable);
void ags_soundcard_editor_init(AgsSoundcardEditor *soundcard_editor);
void ags_soundcard_editor_connect(AgsConnectable *connectable);
void ags_soundcard_editor_disconnect(AgsConnectable *connectable);
void ags_soundcard_editor_set_update(AgsApplicable *applicable, gboolean update);
void ags_soundcard_editor_apply(AgsApplicable *applicable);
void ags_soundcard_editor_reset(AgsApplicable *applicable);
static void ags_soundcard_editor_finalize(GObject *gobject);

/**
 * SECTION:ags_soundcard_editor
 * @short_description: A composite widget to configure soundcard
 * @title: AgsSoundcardEditor
 * @section_id: 
 * @include: ags/X/ags_soundcard_editor.h
 *
 * #AgsSoundcardEditor enables you to make preferences of soundcard, audio channels,
 * samplerate and buffer size.
 */

static gpointer ags_soundcard_editor_parent_class = NULL;

GType
ags_soundcard_editor_get_type(void)
{
  static GType ags_type_soundcard_editor = 0;

  if(!ags_type_soundcard_editor){
    static const GTypeInfo ags_soundcard_editor_info = {
      sizeof (AgsSoundcardEditorClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_soundcard_editor_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsSoundcardEditor),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_soundcard_editor_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_soundcard_editor_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_applicable_interface_info = {
      (GInterfaceInitFunc) ags_soundcard_editor_applicable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    ags_type_soundcard_editor = g_type_register_static(GTK_TYPE_VBOX,
						       "AgsSoundcardEditor", &ags_soundcard_editor_info,
						       0);
    
    g_type_add_interface_static(ags_type_soundcard_editor,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_soundcard_editor,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);
  }

  return(ags_type_soundcard_editor);
}

void
ags_soundcard_editor_class_init(AgsSoundcardEditorClass *soundcard_editor)
{
  GObjectClass *gobject;
  GtkWidgetClass *widget;

  ags_soundcard_editor_parent_class = g_type_class_peek_parent(soundcard_editor);

  /* GtkObjectClass */
  gobject = (GObjectClass *) soundcard_editor;

  gobject->finalize = ags_soundcard_editor_finalize;
}

void
ags_soundcard_editor_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_soundcard_editor_connect;
  connectable->disconnect = ags_soundcard_editor_disconnect;
}

void
ags_soundcard_editor_applicable_interface_init(AgsApplicableInterface *applicable)
{
  applicable->set_update = ags_soundcard_editor_set_update;
  applicable->apply = ags_soundcard_editor_apply;
  applicable->reset = ags_soundcard_editor_reset;
}

void
ags_soundcard_editor_init(AgsSoundcardEditor *soundcard_editor)
{
  GtkTable *table;
  GtkLabel *label;

  soundcard_editor->flags = 0;
  
  soundcard_editor->soundcard = NULL;
  soundcard_editor->soundcard_thread = NULL;
  
  table = (GtkTable *) gtk_table_new(3, 8, FALSE);
  gtk_box_pack_start(GTK_BOX(soundcard_editor),
		     GTK_WIDGET(table),
		     FALSE, FALSE,
		     2);

  /* backend */
  label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				    "label", i18n("backend"),
				    "xalign", 0.0,
				    NULL);
  gtk_table_attach(table,
		   GTK_WIDGET(label),
		   0, 1,
		   0, 1,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  soundcard_editor->backend = (GtkComboBoxText *) gtk_combo_box_text_new();
  gtk_table_attach(table,
		   GTK_WIDGET(soundcard_editor->backend),
		   1, 2,
		   0, 1,
		   GTK_FILL, GTK_FILL,
		   0, 0);

#ifdef AGS_WITH_CORE_AUDIO
  gtk_combo_box_text_append_text(soundcard_editor->backend,
				 "core-audio");
#endif

#ifdef AGS_WITH_PULSE
  gtk_combo_box_text_append_text(soundcard_editor->backend,
				 "pulse");
#endif

#ifdef AGS_WITH_JACK
  gtk_combo_box_text_append_text(soundcard_editor->backend,
				 "jack");
#endif
  
#ifdef AGS_WITH_ALSA
  gtk_combo_box_text_append_text(soundcard_editor->backend,
				 "alsa");
#endif
  
#ifdef AGS_WITH_OSS
  gtk_combo_box_text_append_text(soundcard_editor->backend,
				 "oss");
#endif

  gtk_combo_box_set_active(GTK_COMBO_BOX(soundcard_editor->backend),
			   0);
  
  /* sound card */
  label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				    "label", i18n("soundcard"),
				    "xalign", 0.0,
				    NULL);
  gtk_table_attach(table,
		   GTK_WIDGET(label),
		   0, 1,
		   1, 2,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  soundcard_editor->card = (GtkComboBoxText *) gtk_combo_box_text_new();
  gtk_table_attach(table,
		   GTK_WIDGET(soundcard_editor->card),
		   1, 2,
		   1, 2,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  /* sink */
  soundcard_editor->sink_hbox = NULL;
  soundcard_editor->add_sink = NULL;
  soundcard_editor->remove_sink = NULL;

  soundcard_editor->sink_hbox = (GtkHBox *) gtk_hbox_new(FALSE,
							 0);
  gtk_table_attach(table,
		   GTK_WIDGET(soundcard_editor->sink_hbox),
		   2, 3,
		   1, 2,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  soundcard_editor->add_sink = (GtkButton *) gtk_button_new_from_stock(GTK_STOCK_ADD);
  gtk_box_pack_start((GtkBox *) soundcard_editor->sink_hbox,
		     (GtkWidget *) soundcard_editor->add_sink,
		     FALSE, FALSE,
		     0);
  
  soundcard_editor->remove_sink = (GtkButton *) gtk_button_new_from_stock(GTK_STOCK_REMOVE);
  gtk_box_pack_start((GtkBox *) soundcard_editor->sink_hbox,
		     (GtkWidget *) soundcard_editor->remove_sink,
		     FALSE, FALSE,
		     0);
  
  /* audio channels */
  label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				    "label", i18n("audio channels"),
				    "xalign", 0.0,
				    NULL);
  gtk_table_attach(table,
		   GTK_WIDGET(label),
		   0, 1,
		   2, 3,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  soundcard_editor->audio_channels = (GtkSpinButton *) gtk_spin_button_new_with_range(1.0, 24.0, 1.0);
  gtk_spin_button_set_value(soundcard_editor->audio_channels,
			    2);
  gtk_table_attach(table,
		   GTK_WIDGET(soundcard_editor->audio_channels),
		   1, 2,
		   2, 3,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  /* samplerate */
  label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				    "label", i18n("samplerate"),
				    "xalign", 0.0,
				    NULL);
  gtk_table_attach(table,
		   GTK_WIDGET(label),
		   0, 1,
		   3, 4,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  soundcard_editor->samplerate = (GtkSpinButton *) gtk_spin_button_new_with_range(1.0, 192000.0, 1.0);
  gtk_spin_button_set_value(soundcard_editor->samplerate, 44100);
  gtk_table_attach(table,
		   GTK_WIDGET(soundcard_editor->samplerate),
		   1, 2,
		   3, 4,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  /* buffer size */
  label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				    "label", i18n("buffer size"),
				    "xalign", 0.0,
				    NULL);
  gtk_table_attach(table,
		   GTK_WIDGET(label),
		   0, 1,
		   4, 5,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  soundcard_editor->buffer_size = (GtkSpinButton *) gtk_spin_button_new_with_range(1.0, 65535.0, 1.0);
  gtk_spin_button_set_value(soundcard_editor->buffer_size, 512);
  gtk_table_attach(table,
		   GTK_WIDGET(soundcard_editor->buffer_size),
		   1, 2,
		   4, 5,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  /* format */
  label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				    "label", i18n("format"),
				    "xalign", 0.0,
				    NULL);
  gtk_table_attach(table,
		   GTK_WIDGET(label),
		   0, 1,
		   5, 6,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  soundcard_editor->format = (GtkComboBoxText *) gtk_combo_box_text_new();
  gtk_table_attach(table,
		   GTK_WIDGET(soundcard_editor->format),
		   1, 2,
		   5, 6,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  gtk_combo_box_text_append_text(soundcard_editor->format,
				 "8");
  gtk_combo_box_text_append_text(soundcard_editor->format,
				 "16");
  gtk_combo_box_text_append_text(soundcard_editor->format,
				 "24");
  gtk_combo_box_text_append_text(soundcard_editor->format,
				 "32");
  gtk_combo_box_text_append_text(soundcard_editor->format,
				 "64");
  
  gtk_combo_box_set_active(GTK_COMBO_BOX(soundcard_editor->format),
			   1);

  /*  */
  //  soundcard_editor->remove = NULL;

  soundcard_editor->remove = (GtkButton *) gtk_button_new_from_stock(GTK_STOCK_REMOVE);
  gtk_table_attach(table,
		   GTK_WIDGET(soundcard_editor->remove),
		   2, 3,
		   7, 8,
		   GTK_FILL, GTK_FILL,
		   0, 0);
}

void
ags_soundcard_editor_connect(AgsConnectable *connectable)
{
  AgsSoundcardEditor *soundcard_editor;

  soundcard_editor = AGS_SOUNDCARD_EDITOR(connectable);

  if((AGS_SOUNDCARD_EDITOR_CONNECTED & (soundcard_editor->flags)) != 0){
    return;
  }

  soundcard_editor->flags |= AGS_SOUNDCARD_EDITOR_CONNECTED;
  
  /* backend and card */
  g_signal_connect(G_OBJECT(soundcard_editor->backend), "changed",
		   G_CALLBACK(ags_soundcard_editor_backend_changed_callback), soundcard_editor);

  g_signal_connect(G_OBJECT(soundcard_editor->card), "changed",
		   G_CALLBACK(ags_soundcard_editor_card_changed_callback), soundcard_editor);

  /* add / remove sink */
  g_signal_connect(G_OBJECT(soundcard_editor->add_sink), "clicked",
		   G_CALLBACK(ags_soundcard_editor_add_sink_callback), soundcard_editor);

  g_signal_connect(G_OBJECT(soundcard_editor->remove_sink), "clicked",
		   G_CALLBACK(ags_soundcard_editor_remove_sink_callback), soundcard_editor);

  /* presets */
  g_signal_connect(G_OBJECT(soundcard_editor->audio_channels), "changed",
		   G_CALLBACK(ags_soundcard_editor_audio_channels_changed_callback), soundcard_editor);

  g_signal_connect(G_OBJECT(soundcard_editor->samplerate), "changed",
		   G_CALLBACK(ags_soundcard_editor_samplerate_changed_callback), soundcard_editor);

  g_signal_connect(G_OBJECT(soundcard_editor->buffer_size), "changed",
		   G_CALLBACK(ags_soundcard_editor_buffer_size_changed_callback), soundcard_editor);

  g_signal_connect(G_OBJECT(soundcard_editor->format), "changed",
		   G_CALLBACK(ags_soundcard_editor_format_changed_callback), soundcard_editor);
}

void
ags_soundcard_editor_disconnect(AgsConnectable *connectable)
{
  AgsSoundcardEditor *soundcard_editor;

  soundcard_editor = AGS_SOUNDCARD_EDITOR(connectable);

  if((AGS_SOUNDCARD_EDITOR_CONNECTED & (soundcard_editor->flags)) == 0){
    return;
  }

  soundcard_editor->flags &= (~AGS_SOUNDCARD_EDITOR_CONNECTED);

  /* backend and card */
  g_object_disconnect(G_OBJECT(soundcard_editor->backend),
		      "any_signal::changed",
		      G_CALLBACK(ags_soundcard_editor_backend_changed_callback),
		      soundcard_editor,
		      NULL);

  g_object_disconnect(G_OBJECT(soundcard_editor->card),
		      "any_signal::changed",
		      G_CALLBACK(ags_soundcard_editor_card_changed_callback),
		      soundcard_editor,
		      NULL);

  /* add / remove sink */
  g_object_disconnect(G_OBJECT(soundcard_editor->add_sink),
		      "any_signal::clicked",
		      G_CALLBACK(ags_soundcard_editor_add_sink_callback),
		      soundcard_editor,
		      NULL);

  g_object_disconnect(G_OBJECT(soundcard_editor->remove_sink),
		      "any_signal::clicked",
		      G_CALLBACK(ags_soundcard_editor_remove_sink_callback),
		      soundcard_editor,
		      NULL);

  /* presets */
  g_object_disconnect(G_OBJECT(soundcard_editor->audio_channels),
		      "any_signal::changed",
		      G_CALLBACK(ags_soundcard_editor_audio_channels_changed_callback),
		      soundcard_editor,
		      NULL);

  g_object_disconnect(G_OBJECT(soundcard_editor->samplerate),
		      "any_signal::changed",
		      G_CALLBACK(ags_soundcard_editor_samplerate_changed_callback),
		      soundcard_editor,
		      NULL);

  g_object_disconnect(G_OBJECT(soundcard_editor->buffer_size),
		      "any_signal::changed",
		      G_CALLBACK(ags_soundcard_editor_buffer_size_changed_callback),
		      soundcard_editor,
		      NULL);

  g_object_disconnect(G_OBJECT(soundcard_editor->format),
		      "any_signal::changed",
		      G_CALLBACK(ags_soundcard_editor_format_changed_callback),
		      soundcard_editor,
		      NULL);
}

static void
ags_soundcard_editor_finalize(GObject *gobject)
{
  //TODO:JK: implement me
  
  G_OBJECT_CLASS(ags_soundcard_editor_parent_class)->finalize(gobject);
}

void
ags_soundcard_editor_set_update(AgsApplicable *applicable, gboolean update)
{
  //TODO:JK: implement me
}

void
ags_soundcard_editor_apply(AgsApplicable *applicable)
{
  AgsAudioPreferences *audio_preferences;
  AgsSoundcardEditor *soundcard_editor;
  
  GtkListStore *model;
  GtkTreeIter current;
    
  AgsConfig *config;

  GList *tasks;
  GList *list;	

  gchar *soundcard_group;
  gchar *backend;
  char *device, *str;

  gint nth;
  guint channels;
  guint samplerate;
  guint buffer_size;
  guint format;
  gboolean use_core_audio, use_pulse, use_jack, use_alsa, use_oss;
  
  GValue value =  {0,};

  pthread_mutex_t *application_mutex;

  soundcard_editor = AGS_SOUNDCARD_EDITOR(applicable);
  audio_preferences = (AgsAudioPreferences *) gtk_widget_get_ancestor(GTK_WIDGET(soundcard_editor),
								      AGS_TYPE_AUDIO_PREFERENCES);

  config = ags_config_get_instance();

  list = gtk_container_get_children((GtkContainer *) audio_preferences->soundcard_editor);
  nth = g_list_index(list,
		     soundcard_editor);

  if(nth < 0){
    return;
  }
  
  soundcard_group = g_strdup_printf("%s-%d",
				    AGS_CONFIG_SOUNDCARD,
				    nth);
  g_list_free(list);
  
  /* backend */
#ifdef AGS_WITH_CORE_AUDIO
  use_core_audio = TRUE;
#else
  use_pulse = TRUE;
#endif
  
  use_jack = FALSE;
  use_alsa = FALSE;
  use_oss = FALSE;

  backend = gtk_combo_box_text_get_active_text(soundcard_editor->backend);
  ags_config_set_value(config,
		       soundcard_group,
		       "backend",
		       backend);

  if(backend != NULL){
    if(!g_ascii_strncasecmp(backend,
			    "core-audio",
			    11)){
      use_pulse = TRUE;
    }else if(!g_ascii_strncasecmp(backend,
			    "pulse",
			    6)){
      use_pulse = TRUE;
    }else if(!g_ascii_strncasecmp(backend,
			    "jack",
			    5)){
      use_jack = TRUE;
    }else if(!g_ascii_strncasecmp(backend,
			    "alsa",
			    5)){
      use_alsa = TRUE;
    }else if(!g_ascii_strncasecmp(backend,
				  "oss",
				  4)){
      use_oss = TRUE;
    }
  }

  /* buffer size */
  buffer_size = gtk_spin_button_get_value(soundcard_editor->buffer_size);
  str = g_strdup_printf("%u",
			buffer_size);
  ags_config_set_value(config,
		       soundcard_group,
		       "buffer-size",
		       str);
  g_free(str);

  /* pcm channels */
  channels = gtk_spin_button_get_value(soundcard_editor->audio_channels);
  str = g_strdup_printf("%u",
			channels);
  ags_config_set_value(config,
		       soundcard_group,
		       "pcm-channels",
		       str);
  g_free(str);

  /* format */
  format = 0;
  
  switch(gtk_combo_box_get_active(GTK_COMBO_BOX(soundcard_editor->format))){
  case 0:
    format = AGS_SOUNDCARD_SIGNED_8_BIT;
    break;
  case 1:
    format = AGS_SOUNDCARD_SIGNED_16_BIT;
    break;
  case 2:
    format = AGS_SOUNDCARD_SIGNED_24_BIT;
    break;
  case 3:
    format = AGS_SOUNDCARD_SIGNED_32_BIT;
    break;
  case 4:
    format = AGS_SOUNDCARD_SIGNED_64_BIT;
    break;
  }

  str = g_strdup_printf("%u",
			format);
  ags_config_set_value(config,
		       soundcard_group,
		       "format",
		       str);
  g_free(str);

  /* samplerate */
  samplerate = gtk_spin_button_get_value(soundcard_editor->samplerate);
  str = g_strdup_printf("%u",
			samplerate);
  ags_config_set_value(config,
		       soundcard_group,
		       "samplerate",
		       str);
  g_free(str);
  
  /* device */
  model = GTK_LIST_STORE(gtk_combo_box_get_model(GTK_COMBO_BOX(soundcard_editor->card)));
  
  if(gtk_combo_box_get_active_iter(GTK_COMBO_BOX(soundcard_editor->card),
				   &current)){
    gtk_tree_model_get_value(model,
			     &current,
			     0,
			     &value);
    device = g_value_get_string(&value);
  }else{
    device = NULL;
  }

  /* handle */
  g_message("%s", device);

  if(use_core_audio){
    ags_config_set_value(config,
			 soundcard_group,
			 "device",
			 device);
  }else if(use_pulse){
    ags_config_set_value(config,
			 soundcard_group,
			 "device",
			 device);
  }else if(use_jack){
    ags_config_set_value(config,
			 soundcard_group,
			 "device",
			 device);
  }else if(use_alsa){
    ags_config_set_value(config,
			 soundcard_group,
			 "device",
			 device);
  }else if(use_oss){
    ags_config_set_value(config,
			 soundcard_group,
			 "device",
			 device);
  }
}

void
ags_soundcard_editor_reset(AgsApplicable *applicable)
{
  AgsWindow *window;
  AgsPreferences *preferences;
  AgsSoundcardEditor *soundcard_editor;

  GObject *soundcard;

  GList *card_id, *card_id_start, *card_name, *card_name_start;

  gchar *backend, *device, *str, *tmp;

  guint nth;
  guint channels, channels_min, channels_max;
  guint samplerate, samplerate_min, samplerate_max;
  guint buffer_size, buffer_size_min, buffer_size_max;
  guint format;
  gboolean use_alsa;
  gboolean found_card;
  
  GError *error;
  
  soundcard_editor = AGS_SOUNDCARD_EDITOR(applicable);

  if((AGS_SOUNDCARD_EDITOR_BLOCK_RESET & (soundcard_editor->flags)) != 0){
    return;
  }

  soundcard_editor->flags |= AGS_SOUNDCARD_EDITOR_BLOCK_RESET;
  
  soundcard = soundcard_editor->soundcard;
  
  /* refresh */
  use_alsa = FALSE;

  backend = NULL;
  
  if(AGS_IS_CORE_AUDIO_DEVOUT(soundcard)){
    backend = "core-audio";
  }else if(AGS_IS_PULSE_DEVOUT(soundcard)){
    backend = "pulse";
  }else if(AGS_IS_JACK_DEVOUT(soundcard)){
    backend = "jack";
  }else if(AGS_IS_DEVOUT(soundcard)){
    if((AGS_DEVOUT_ALSA & (AGS_DEVOUT(soundcard)->flags)) != 0){
      backend = "alsa";
    }else if((AGS_DEVOUT_OSS & (AGS_DEVOUT(soundcard)->flags)) != 0){
      backend = "oss";
    }
  }

  if(backend != NULL){
    if(!g_ascii_strncasecmp(backend,
			    "core-audio",
			    11)){
      gtk_combo_box_set_active(GTK_COMBO_BOX(soundcard_editor->backend),
			       0);
      
      //      ags_soundcard_editor_load_core_audio_card(soundcard_editor);
    }else if(!g_ascii_strncasecmp(backend,
			    "pulse",
			    6)){
      gtk_combo_box_set_active(GTK_COMBO_BOX(soundcard_editor->backend),
			       0);
      
      //      ags_soundcard_editor_load_pulse_card(soundcard_editor);
    }else if(!g_ascii_strncasecmp(backend,
			    "jack",
			    5)){
      gtk_combo_box_set_active(GTK_COMBO_BOX(soundcard_editor->backend),
			       1);
      
      //      ags_soundcard_editor_load_jack_card(soundcard_editor);
    }else if(!g_ascii_strncasecmp(backend,
				  "alsa",
				  5)){
      use_alsa = TRUE;

#ifdef AGS_WITH_ALSA
      gtk_combo_box_set_active(GTK_COMBO_BOX(soundcard_editor->backend),
			       2);
#endif
      
      //      ags_soundcard_editor_load_alsa_card(soundcard_editor);
    }else if(!g_ascii_strncasecmp(backend,
				  "oss",
				  4)){
#ifdef AGS_WITH_OSS
      
#ifdef AGS_WITH_ALSA
      gtk_combo_box_set_active(GTK_COMBO_BOX(soundcard_editor->backend),
			       3);
      
#else
      gtk_combo_box_set_active(GTK_COMBO_BOX(soundcard_editor->backend),
			       2);
#endif
      
#else
      gtk_combo_box_set_active(GTK_COMBO_BOX(soundcard_editor->backend),
			       -1);
#endif
      
      //      ags_soundcard_editor_load_oss_card(soundcard_editor);
    }
  }

  /*  */
  device = ags_soundcard_get_device(AGS_SOUNDCARD(soundcard));
  
  card_id = NULL;
  card_name = NULL;

  ags_soundcard_list_cards(AGS_SOUNDCARD(soundcard),
			   &card_id_start, &card_name_start);

  card_id = card_id_start;
  card_name = card_name_start;
  
  nth = 0;
  found_card = FALSE;

  gtk_list_store_clear(GTK_LIST_STORE(gtk_combo_box_get_model(GTK_COMBO_BOX(soundcard_editor->card))));

  while(card_id != NULL){
    //FIXME:JK: work-around for alsa-handle
    tmp = card_id->data;
        
    if(tmp != NULL &&
       device != NULL &&
       !g_ascii_strcasecmp(tmp,
			   device)){
      found_card = TRUE;
    }

    if(tmp != NULL){
      gtk_combo_box_text_append_text(soundcard_editor->card,
				     tmp);
    }
        
    if(!found_card){
      nth++;
    }
    
    card_id = card_id->next;
    card_name = card_name->next;
  }
  
  if(!found_card){
    nth = 0;
  }

  gtk_combo_box_set_active(GTK_COMBO_BOX(soundcard_editor->card),
			   nth);

  if(card_id_start != NULL){
    g_list_free_full(card_id_start,
		     g_free);
    g_list_free_full(card_name_start,
		     g_free);
  }
  
  /*  */
  ags_soundcard_get_presets(AGS_SOUNDCARD(soundcard),
			    &channels,
			    &samplerate,
			    &buffer_size,
			    &format);

  /*  */
  gtk_spin_button_set_value(soundcard_editor->audio_channels,
			    (gdouble) channels);
  gtk_spin_button_set_value(soundcard_editor->samplerate,
			    (gdouble) samplerate);
  gtk_spin_button_set_value(soundcard_editor->buffer_size,
			    (gdouble) buffer_size);

  switch(format){
  case AGS_SOUNDCARD_SIGNED_8_BIT:
    gtk_combo_box_set_active(GTK_COMBO_BOX(soundcard_editor->format),
			     0);
    break;
  case AGS_SOUNDCARD_SIGNED_16_BIT:
    gtk_combo_box_set_active(GTK_COMBO_BOX(soundcard_editor->format),
			     1);
    break;
  case AGS_SOUNDCARD_SIGNED_24_BIT:
    gtk_combo_box_set_active(GTK_COMBO_BOX(soundcard_editor->format),
			     2);
    break;
  case AGS_SOUNDCARD_SIGNED_32_BIT:
    gtk_combo_box_set_active(GTK_COMBO_BOX(soundcard_editor->format),
			     3);
    break;
  case AGS_SOUNDCARD_SIGNED_64_BIT:
    gtk_combo_box_set_active(GTK_COMBO_BOX(soundcard_editor->format),
			     4);
    break;
  }

  /*  */
  if(device != NULL &&
     soundcard != NULL){
    error = NULL;
    ags_soundcard_pcm_info(AGS_SOUNDCARD(soundcard),
			   device,
			   &channels_min, &channels_max,
			   &samplerate_min, &samplerate_max,
			   &buffer_size_min, &buffer_size_max,
			   &error);

    if(error != NULL){
      channels_min = 0.0;
      channels_max = 24.0;
      samplerate_min = 8000.0;
      samplerate_max = 192000.0;
      buffer_size_min = 2.0;
      buffer_size_max = 65535.0;
    }
  }else{
    channels_min = 0.0;
    channels_max = 24.0;
    samplerate_min = 8000.0;
    samplerate_max = 192000.0;
    buffer_size_min = 2.0;
    buffer_size_max = 65535.0;
  }
  
  gtk_spin_button_set_range(soundcard_editor->audio_channels,
			    channels_min, channels_max);
  gtk_spin_button_set_range(soundcard_editor->samplerate,
			    samplerate_min, samplerate_max);
  gtk_spin_button_set_range(soundcard_editor->buffer_size,
			    buffer_size_min, buffer_size_max);

  soundcard_editor->flags &= (~AGS_SOUNDCARD_EDITOR_BLOCK_RESET);
}

void
ags_soundcard_editor_add_sink(AgsSoundcardEditor *soundcard_editor,
			      gchar *device)
{
  AgsWindow *window;
  AgsPreferences *preferences;

  AgsPulseServer *pulse_server;
  AgsPulseDevout *pulse_devout;

  AgsJackServer *jack_server;
  AgsJackDevout *jack_devout;

  AgsCoreAudioServer *core_audio_server;
  AgsCoreAudioDevout *core_audio_devout;

  AgsNotifySoundcard *notify_soundcard;
  
  AgsMutexManager *mutex_manager;
  AgsThread *main_loop;
  AgsThread *soundcard_thread;
  AgsThread *export_thread;
  
  AgsApplicationContext *application_context;

  GObject *soundcard;
  
  GType server_type;
  
  GList *distributed_manager;
  GList *card_name, *card_uri;

  gchar *backend;
  
  gboolean use_core_audio, use_pulse, use_jack;
  gboolean initial_soundcard;
  
  pthread_mutex_t *application_mutex;
  pthread_mutex_t *mutex;

  preferences = (AgsPreferences *) gtk_widget_get_ancestor(GTK_WIDGET(soundcard_editor),
							   AGS_TYPE_PREFERENCES);
  window = AGS_WINDOW(preferences->window);

  application_context = (AgsApplicationContext *) window->application_context;
  application_mutex = window->application_mutex;

  mutex_manager = ags_mutex_manager_get_instance();

  core_audio_server = NULL;
  core_audio_devout = NULL;
  
  pulse_server = NULL;
  pulse_devout = NULL;
  
  jack_server = NULL;
  jack_devout = NULL;

  soundcard = NULL;
    
  server_type = G_TYPE_NONE;
  
  backend = NULL;

  use_core_audio = FALSE;
  use_pulse = FALSE;
  use_jack = FALSE;

  initial_soundcard = FALSE;

  /* determine backend */
  backend = gtk_combo_box_text_get_active_text(soundcard_editor->backend);

  if(backend != NULL){
    if(!g_ascii_strncasecmp(backend,
			    "core-audio",
			    11)){
      server_type = AGS_TYPE_CORE_AUDIO_SERVER;
      
      use_core_audio = TRUE;
    }else if(!g_ascii_strncasecmp(backend,
			    "pulse",
			    6)){
      server_type = AGS_TYPE_PULSE_SERVER;
      
      use_pulse = TRUE;
    }else if(!g_ascii_strncasecmp(backend,
			    "jack",
			    5)){
      server_type = AGS_TYPE_JACK_SERVER;

      use_jack = TRUE;
    }
  }
  
  /* create soundcard */
  pthread_mutex_lock(application_mutex);

  distributed_manager = ags_sound_provider_get_distributed_manager(AGS_SOUND_PROVIDER(application_context));

  if((distributed_manager = ags_list_util_find_type(distributed_manager,
						    server_type)) != NULL){
    if(use_core_audio){
      core_audio_server = AGS_CORE_AUDIO_SERVER(distributed_manager->data);

      soundcard = 
	core_audio_devout = (AgsCoreAudioDevout *) ags_distributed_manager_register_soundcard(AGS_DISTRIBUTED_MANAGER(core_audio_server),
											      TRUE);
    }else if(use_pulse){
      pulse_server = AGS_PULSE_SERVER(distributed_manager->data);

      soundcard = 
	pulse_devout = (AgsPulseDevout *) ags_distributed_manager_register_soundcard(AGS_DISTRIBUTED_MANAGER(pulse_server),
										     TRUE);
    }else if(use_jack){
      jack_server = AGS_JACK_SERVER(distributed_manager->data);

      soundcard = 
	jack_devout = (AgsJackDevout *) ags_distributed_manager_register_soundcard(AGS_DISTRIBUTED_MANAGER(jack_server),
										   TRUE);
    }
  }else{
    g_warning("distributed manager not found");

    pthread_mutex_unlock(application_mutex);
    
    return;
  }
  
  pthread_mutex_unlock(application_mutex);

  if(soundcard == NULL){
    return;
  }

  pthread_mutex_lock(application_mutex);

  mutex = ags_mutex_manager_lookup(mutex_manager,
				   (GObject *) soundcard);
  

  /* add new */
  main_loop = (AgsThread *) application_context->main_loop;
  
  if(ags_sound_provider_get_soundcard(AGS_SOUND_PROVIDER(application_context)) == NULL){
    initial_soundcard = TRUE;
  }

  soundcard_editor->soundcard = soundcard;

  ags_sound_provider_set_soundcard(AGS_SOUND_PROVIDER(application_context),
				   g_list_append(ags_sound_provider_get_soundcard(AGS_SOUND_PROVIDER(application_context)),
						 soundcard));

  /* add AgsAudio to AgsSoundcard */
  if(initial_soundcard){
    GList *machine, *machine_start;
    GList *list;
    
    machine = 
      machine_start = gtk_container_get_children(window->machines);

    while(machine != NULL){
      g_object_ref(G_OBJECT(AGS_MACHINE(machine->data)->audio));
  
      list = ags_soundcard_get_audio(AGS_SOUNDCARD(soundcard));
      list = g_list_prepend(list,
			    AGS_MACHINE(machine->data)->audio);
      ags_soundcard_set_audio(AGS_SOUNDCARD(soundcard),
			      list);

      machine = machine->next;
    }

    g_list_free(machine_start);
  }
  
  pthread_mutex_unlock(application_mutex);  
    
  g_object_ref(soundcard);

  soundcard_thread = (AgsThread *) ags_soundcard_thread_new(soundcard);
  soundcard_editor->soundcard_thread = (GObject *) soundcard_thread;
  
  ags_thread_add_child_extended(main_loop,
				soundcard_thread,
				TRUE, TRUE);

  /* notify soundcard */
  notify_soundcard = ags_notify_soundcard_new(soundcard_thread);
  AGS_TASK(notify_soundcard)->task_thread = application_context->task_thread;

  if(use_core_audio){
    core_audio_devout->notify_soundcard = notify_soundcard;
  }else if(use_pulse){
    pulse_devout->notify_soundcard = notify_soundcard;
  }else if(use_jack){
    jack_devout->notify_soundcard = notify_soundcard;
  }
  
  ags_task_thread_append_cyclic_task(application_context->task_thread,
				     notify_soundcard);

  if(ags_sound_provider_get_default_soundcard_thread(AGS_SOUND_PROVIDER(application_context)) == NULL){
    ags_sound_provider_set_default_soundcard_thread(AGS_SOUND_PROVIDER(application_context),
						    (GObject *) soundcard_thread);
  }

  /* export thread */
  export_thread = (AgsThread *) ags_export_thread_new(soundcard,
						      NULL);
  ags_thread_add_child_extended(main_loop,
				(AgsThread *) export_thread,
				TRUE, TRUE);

  pthread_mutex_unlock(application_mutex);

  /*  */
  pthread_mutex_lock(mutex);

  card_name = NULL;
  card_uri = NULL;
  ags_soundcard_list_cards(AGS_SOUNDCARD(soundcard),
			   &card_uri, &card_name);

  pthread_mutex_unlock(mutex);

  gtk_list_store_clear(GTK_LIST_STORE(gtk_combo_box_get_model(GTK_COMBO_BOX(soundcard_editor->card))));

  while(card_uri != NULL){
    if(card_uri->data != NULL){
      gtk_combo_box_text_append_text(soundcard_editor->card,
				     card_uri->data);
    }
    
    card_uri = card_uri->next;
  }
}

void
ags_soundcard_editor_remove_sink(AgsSoundcardEditor *soundcard_editor,
				 gchar *device)
{
  AgsWindow *window;
  AgsPreferences *preferences;
  GtkDialog *dialog;
  
  AgsCoreAudioServer *core_audio_server;
  AgsCoreAudioDevout *core_audio_devout;

  AgsPulseServer *pulse_server;
  AgsPulseDevout *pulse_devout;

  AgsJackServer *jack_server;
  AgsJackDevout *jack_devout;

  AgsApplicationContext *application_context;
  AgsThread *main_loop;

  GObject *server;
  GObject *soundcard;

  GType server_type;

  GList *machine, *machine_start;
  GList *distributed_manager;
  GList *list;
  GList *card_id;

  gchar *backend;
  
  gboolean use_core_audio, use_pulse, use_jack;

  pthread_mutex_t *application_mutex;

  preferences = (AgsPreferences *) gtk_widget_get_ancestor(GTK_WIDGET(soundcard_editor),
							   AGS_TYPE_PREFERENCES);
  window = AGS_WINDOW(preferences->window);
  application_context = (AgsApplicationContext *) window->application_context;

  application_mutex = window->application_mutex;

  core_audio_server = NULL;
  core_audio_devout = NULL;
  
  pulse_server = NULL;
  pulse_devout = NULL;
  
  jack_server = NULL;
  jack_devout = NULL;

  server = NULL;
  soundcard = NULL;

  server_type = G_TYPE_NONE;
  
  backend = NULL;

  use_core_audio = FALSE;
  use_pulse = FALSE;
  use_jack = FALSE;

  /* determine backend */
  backend = gtk_combo_box_text_get_active_text(soundcard_editor->backend);

  if(backend != NULL){
    if(!g_ascii_strncasecmp(backend,
			    "core-audio",
			    6)){
      server_type = AGS_TYPE_CORE_AUDIO_SERVER;
      
      use_core_audio = TRUE;
    }else if(!g_ascii_strncasecmp(backend,
			    "pulse",
			    6)){
      server_type = AGS_TYPE_PULSE_SERVER;
      
      use_pulse = TRUE;
    }else if(!g_ascii_strncasecmp(backend,
			    "jack",
			    5)){
      server_type = AGS_TYPE_JACK_SERVER;

      use_jack = TRUE;
    }
  }
  
  /* destroy soundcard */
  pthread_mutex_lock(application_mutex);

  main_loop = application_context->main_loop;
  distributed_manager = ags_sound_provider_get_distributed_manager(AGS_SOUND_PROVIDER(application_context));

  if((distributed_manager = ags_list_util_find_type(distributed_manager,
						    server_type)) != NULL){
    if(use_core_audio){
      server = 
	core_audio_server = AGS_CORE_AUDIO_SERVER(distributed_manager->data);
    }else if(use_pulse){
      server = 
	pulse_server = AGS_PULSE_SERVER(distributed_manager->data);
    }else if(use_jack){
      server = 
	jack_server = AGS_JACK_SERVER(distributed_manager->data);
    }
  }else{
    g_warning("distributed manager not found");

    pthread_mutex_unlock(application_mutex);
    
    return;
  }

  list = ags_sound_provider_get_soundcard(AGS_SOUND_PROVIDER(application_context));
  
  while(list != NULL){
    if(use_core_audio){
      if(AGS_IS_CORE_AUDIO_DEVOUT(list->data) &&
	 !g_ascii_strcasecmp(ags_soundcard_get_device(AGS_SOUNDCARD(list->data)),
			     device)){
	soundcard = 
	  core_audio_devout = list->data;
	break;
      }
    }else if(use_pulse){
      if(AGS_IS_PULSE_DEVOUT(list->data) &&
	 !g_ascii_strcasecmp(ags_soundcard_get_device(AGS_SOUNDCARD(list->data)),
			     device)){
	soundcard = 
	  pulse_devout = list->data;
	break;
      }
    }else if(use_jack){
      if(AGS_IS_JACK_DEVOUT(list->data) &&
	 !g_ascii_strcasecmp(ags_soundcard_get_device(AGS_SOUNDCARD(list->data)),
			     device)){
	soundcard = 
	  jack_devout = list->data;
	break;
      }
    }
    
    list = list->next;
  }

  pthread_mutex_unlock(application_mutex);

  if(soundcard == NULL){
    return;
  }

  /* remove AgsSoundcard from AgsAudio */
#if 0
  machine = 
    machine_start = gtk_container_get_children(window->machines);
  
  while(machine != NULL){
    if(AGS_MACHINE(machine->data)->audio->soundcard == soundcard){
      g_object_set(AGS_MACHINE(machine->data)->audio,
		   "soundcard", NULL,
		   NULL);

      g_object_unref(G_OBJECT(AGS_MACHINE(machine->data)->audio));
    }
    
    machine = machine->next;
  }

  ags_soundcard_set_audio(AGS_SOUNDCARD(soundcard),
			  NULL);
  g_list_free(machine_start);
#endif
  
  /*  */
  gtk_list_store_clear(GTK_LIST_STORE(gtk_combo_box_get_model(GTK_COMBO_BOX(soundcard_editor->card))));
  gtk_combo_box_set_active(GTK_COMBO_BOX(soundcard_editor->backend),
			   -1);

#if 0
  if(server != NULL){
    ags_distributed_manager_unregister_soundcard(AGS_DISTRIBUTED_MANAGER(server),
						 soundcard);
  }
#endif
  
  /* remove */
  if(soundcard == soundcard_editor->soundcard){
    soundcard_editor->soundcard = NULL;
  }
    
#if 0  
  if(soundcard != NULL){
    ags_sound_provider_set_soundcard(AGS_SOUND_PROVIDER(application_context),
				     g_list_remove(ags_sound_provider_get_soundcard(AGS_SOUND_PROVIDER(application_context)),
						   soundcard));
    g_object_unref(soundcard);
  }
  
  if(soundcard_editor->soundcard_thread != NULL){
    ags_thread_stop((AgsThread *) soundcard_editor->soundcard_thread);

    ags_thread_remove_child(main_loop,
			    (AgsThread *) soundcard_editor->soundcard_thread);
    
    //    g_object_unref(soundcard_editor->soundcard_thread);
    
    soundcard_editor->soundcard_thread = NULL;
  }
#endif

  /* notify user about restarting GSequencer */
  dialog = gtk_message_dialog_new(preferences,
				  GTK_DIALOG_MODAL,
				  GTK_MESSAGE_INFO,
				  GTK_BUTTONS_OK,
				  "After finished your modifications you should safe your file and restart GSequencer");
  g_signal_connect(dialog, "response",
		   G_CALLBACK(gtk_widget_destroy), NULL);
  gtk_widget_show_all(dialog);
}

void
ags_soundcard_editor_add_soundcard(AgsSoundcardEditor *soundcard_editor,
				   GObject *soundcard)
{
  AgsWindow *window;
  AgsPreferences *preferences;

  AgsNotifySoundcard *notify_soundcard;
  
  AgsThread *main_loop;
  AgsThread *soundcard_thread;
  AgsThread *export_thread;
  
  AgsApplicationContext *application_context;

  gboolean initial_soundcard;
  
  pthread_mutex_t *application_mutex;

  if(soundcard == NULL ||
     AGS_IS_CORE_AUDIO_DEVOUT(soundcard) ||
     AGS_IS_PULSE_DEVOUT(soundcard) ||
     AGS_IS_JACK_DEVOUT(soundcard)){
    return;
  }

  if((AGS_SOUNDCARD_EDITOR_BLOCK_ADD & (soundcard_editor->flags)) != 0){
    return;
  }

  soundcard_editor->flags |= AGS_SOUNDCARD_EDITOR_BLOCK_ADD;

  preferences = (AgsPreferences *) gtk_widget_get_ancestor(GTK_WIDGET(soundcard_editor),
							   AGS_TYPE_PREFERENCES);
  window = AGS_WINDOW(preferences->window);
  
  application_context = (AgsApplicationContext *) window->application_context;
  application_mutex = window->application_mutex;

  initial_soundcard = FALSE;
  
  if(AGS_IS_DEVOUT(soundcard)){
    if((AGS_DEVOUT_ALSA & (AGS_DEVOUT(soundcard)->flags)) != 0){
      ags_soundcard_set_device(AGS_SOUNDCARD(soundcard),
			       "hw:0,0");
    }else if((AGS_DEVOUT_OSS & (AGS_DEVOUT(soundcard)->flags)) != 0){
      ags_soundcard_set_device(AGS_SOUNDCARD(soundcard),
			       "/dev/dsp0");
    }else{
      g_warning("unknown soundcard implementation");
    }
  }else{
    g_warning("unknown soundcard implementation");
  }
  
  /*  */
  pthread_mutex_lock(application_mutex);

  main_loop = (AgsThread *) application_context->main_loop;

  if(g_list_find(ags_sound_provider_get_soundcard(AGS_SOUND_PROVIDER(application_context)),
		 soundcard) != NULL){
    pthread_mutex_unlock(application_mutex);  
  
    soundcard_editor->flags &= (~AGS_SOUNDCARD_EDITOR_BLOCK_ADD);
    
    return;
  }

  if(ags_sound_provider_get_soundcard(AGS_SOUND_PROVIDER(application_context)) == NULL){
    initial_soundcard = TRUE;
  }
  
  soundcard_editor->soundcard = soundcard;

  ags_sound_provider_set_soundcard(AGS_SOUND_PROVIDER(application_context),
				   g_list_append(ags_sound_provider_get_soundcard(AGS_SOUND_PROVIDER(application_context)),
						 soundcard));

  /* add AgsAudio to AgsSoundcard */
  if(initial_soundcard){
    GList *machine, *machine_start;
    GList *list;
    
    machine = 
      machine_start = gtk_container_get_children(window->machines);

    while(machine != NULL){
      g_object_ref(G_OBJECT(AGS_MACHINE(machine->data)->audio));
  
      list = ags_soundcard_get_audio(AGS_SOUNDCARD(soundcard));
      list = g_list_prepend(list,
			    AGS_MACHINE(machine->data)->audio);
      ags_soundcard_set_audio(AGS_SOUNDCARD(soundcard),
			      list);

      g_object_set(AGS_MACHINE(machine->data)->audio,
		   "soundcard", soundcard,
		   NULL);
      
      machine = machine->next;
    }

  
    g_list_free(machine_start);
  }
  
  pthread_mutex_unlock(application_mutex);  
  
  g_object_ref(soundcard);

  soundcard_thread = (AgsThread *) ags_soundcard_thread_new(soundcard);
  soundcard_editor->soundcard_thread = (GObject *) soundcard_thread;
  
  ags_thread_add_child_extended(main_loop,
				soundcard_thread,
				TRUE, TRUE);

  /* notify soundcard */
  notify_soundcard = ags_notify_soundcard_new(soundcard_thread);
  AGS_TASK(notify_soundcard)->task_thread = application_context->task_thread;
  
  if(AGS_IS_DEVOUT(soundcard)){
    AGS_DEVOUT(soundcard)->notify_soundcard = notify_soundcard;
  }
  
  ags_task_thread_append_cyclic_task(application_context->task_thread,
				     notify_soundcard);

  if(ags_sound_provider_get_default_soundcard_thread(AGS_SOUND_PROVIDER(application_context)) == NULL){
    ags_sound_provider_set_default_soundcard_thread(AGS_SOUND_PROVIDER(application_context),
						    (GObject *) soundcard_thread);
  }
  
  /* export thread */
  export_thread = (AgsThread *) ags_export_thread_new(soundcard,
						      NULL);
  ags_thread_add_child_extended(main_loop,
				(AgsThread *) export_thread,
				TRUE, TRUE);

  soundcard_editor->flags &= (~AGS_SOUNDCARD_EDITOR_BLOCK_ADD);
}

void
ags_soundcard_editor_remove_soundcard(AgsSoundcardEditor *soundcard_editor,
				      GObject *soundcard)
{
  AgsWindow *window;
  AgsPreferences *preferences;
  GtkDialog *dialog;
  
  AgsThread *main_loop;
  AgsThread *soundcard_thread;

  AgsApplicationContext *application_context;

  GList *machine, *machine_start;

  pthread_mutex_t *application_mutex;

  if(soundcard == NULL ||
     AGS_IS_CORE_AUDIO_DEVOUT(soundcard) ||
     AGS_IS_PULSE_DEVOUT(soundcard) ||
     AGS_IS_JACK_DEVOUT(soundcard)){
    return;
  }
  
  preferences = (AgsPreferences *) gtk_widget_get_ancestor(GTK_WIDGET(soundcard_editor),
							   AGS_TYPE_PREFERENCES);
  window = AGS_WINDOW(preferences->window);
  application_context = (AgsApplicationContext *) window->application_context;  

  application_mutex = window->application_mutex;
  
  pthread_mutex_lock(application_mutex);

  main_loop = (AgsThread *) application_context->main_loop;

  /* remove AgsSoundcard from AgsAudio */
#if 0
  machine = 
    machine_start = gtk_container_get_children(window->machines);
  
  while(machine != NULL){
    if(AGS_MACHINE(machine->data)->audio->soundcard == soundcard){
      g_object_set(AGS_MACHINE(machine->data)->audio,
		   "soundcard", NULL,
		   NULL);

      g_object_unref(G_OBJECT(AGS_MACHINE(machine->data)->audio));
    }
    
    machine = machine->next;
  }

  ags_soundcard_set_audio(AGS_SOUNDCARD(soundcard),
			  NULL);
  g_list_free(machine_start);
#endif
  
  /*  */  
  if(soundcard == soundcard_editor->soundcard){
    soundcard_editor->soundcard = NULL;
  }
  
#if 0
  if(soundcard != NULL){
    ags_sound_provider_set_soundcard(AGS_SOUND_PROVIDER(application_context),
				     g_list_remove(ags_sound_provider_get_soundcard(AGS_SOUND_PROVIDER(application_context)),
						   soundcard));
    g_object_unref(soundcard);
  }
#endif

#if 0
  if(soundcard_editor->soundcard_thread != NULL){
    ags_thread_stop((AgsThread *) soundcard_editor->soundcard_thread);

    ags_thread_remove_child(main_loop,
			    (AgsThread *) soundcard_editor->soundcard_thread);
    
    //    g_object_unref(soundcard_editor->soundcard_thread);
    
    soundcard_editor->soundcard_thread = NULL;
  }
#endif

  pthread_mutex_unlock(application_mutex);

  /* notify user about restarting GSequencer */
  dialog = gtk_message_dialog_new(preferences,
				  GTK_DIALOG_MODAL,
				  GTK_MESSAGE_INFO,
				  GTK_BUTTONS_OK,
				  "After finished your modifications you should safe your file and restart GSequencer");
  g_signal_connect(dialog, "response",
		   G_CALLBACK(gtk_widget_destroy), NULL);
  gtk_widget_show_all(dialog);
}

void
ags_soundcard_editor_load_core_audio_card(AgsSoundcardEditor *soundcard_editor)
{
  AgsWindow *window;
  AgsPreferences *preferences;

  AgsCoreAudioDevout *core_audio_devout;

  AgsApplicationContext *application_context;

  GList *distributed_manager;
  GList *soundcard;
  GList *card_id;

  pthread_mutex_t *application_mutex;

  preferences = (AgsPreferences *) gtk_widget_get_ancestor(GTK_WIDGET(soundcard_editor),
							   AGS_TYPE_PREFERENCES);
  window = AGS_WINDOW(preferences->window);
  application_context = (AgsApplicationContext *) window->application_context;

  application_mutex = window->application_mutex;

  /* create soundcard */
  pthread_mutex_lock(application_mutex);

  distributed_manager = ags_sound_provider_get_distributed_manager(AGS_SOUND_PROVIDER(application_context));

  if(distributed_manager == NULL){
    g_warning("distributed manager not found");

    pthread_mutex_unlock(application_mutex);

    return;
  }

  soundcard = ags_sound_provider_get_soundcard(AGS_SOUND_PROVIDER(application_context));
  core_audio_devout = NULL;
  
  while(soundcard != NULL){
    if(AGS_IS_CORE_AUDIO_DEVOUT(soundcard->data)){
      core_audio_devout = soundcard->data;
      break;
    }
    
    soundcard = soundcard->next;
  }

  card_id = NULL;
  ags_soundcard_list_cards(AGS_SOUNDCARD(core_audio_devout),
			   &card_id, NULL);

  gtk_list_store_clear(GTK_LIST_STORE(gtk_combo_box_get_model(GTK_COMBO_BOX(soundcard_editor->card))));

  while(card_id != NULL){
    if(card_id->data != NULL){
      gtk_combo_box_text_append_text(soundcard_editor->card,
				     card_id->data);
    }
    
    card_id = card_id->next;
  }

  /*  */
  gtk_widget_set_sensitive((GtkWidget *) soundcard_editor->buffer_size,
			   FALSE);
  
  gtk_widget_set_sensitive((GtkWidget *) soundcard_editor->samplerate,
			   FALSE);
  pthread_mutex_unlock(application_mutex);
}

void
ags_soundcard_editor_load_pulse_card(AgsSoundcardEditor *soundcard_editor)
{
  AgsWindow *window;
  AgsPreferences *preferences;

  AgsPulseDevout *pulse_devout;

  AgsApplicationContext *application_context;

  GList *distributed_manager;
  GList *soundcard;
  GList *card_id;

  pthread_mutex_t *application_mutex;

  preferences = (AgsPreferences *) gtk_widget_get_ancestor(GTK_WIDGET(soundcard_editor),
							   AGS_TYPE_PREFERENCES);
  window = AGS_WINDOW(preferences->window);
  application_context = (AgsApplicationContext *) window->application_context;

  application_mutex = window->application_mutex;

  /* create soundcard */
  pthread_mutex_lock(application_mutex);

  distributed_manager = ags_sound_provider_get_distributed_manager(AGS_SOUND_PROVIDER(application_context));

  if(distributed_manager == NULL){
    g_warning("distributed manager not found");

    pthread_mutex_unlock(application_mutex);

    return;
  }

  soundcard = ags_sound_provider_get_soundcard(AGS_SOUND_PROVIDER(application_context));
  pulse_devout = NULL;
  
  while(soundcard != NULL){
    if(AGS_IS_PULSE_DEVOUT(soundcard->data)){
      pulse_devout = soundcard->data;
      break;
    }
    
    soundcard = soundcard->next;
  }

  card_id = NULL;
  ags_soundcard_list_cards(AGS_SOUNDCARD(pulse_devout),
			   &card_id, NULL);

  gtk_list_store_clear(GTK_LIST_STORE(gtk_combo_box_get_model(GTK_COMBO_BOX(soundcard_editor->card))));

  while(card_id != NULL){
    if(card_id->data != NULL){
      gtk_combo_box_text_append_text(soundcard_editor->card,
				     card_id->data);
    }
    
    card_id = card_id->next;
  }

  /*  */
  gtk_widget_set_sensitive((GtkWidget *) soundcard_editor->buffer_size,
			   FALSE);
  
  gtk_widget_set_sensitive((GtkWidget *) soundcard_editor->samplerate,
			   FALSE);
  pthread_mutex_unlock(application_mutex);
}

void
ags_soundcard_editor_load_jack_card(AgsSoundcardEditor *soundcard_editor)
{
  AgsWindow *window;
  AgsPreferences *preferences;

  AgsJackDevout *jack_devout;

  AgsApplicationContext *application_context;

  GList *distributed_manager;
  GList *soundcard;
  GList *card_id;

  pthread_mutex_t *application_mutex;

  preferences = (AgsPreferences *) gtk_widget_get_ancestor(GTK_WIDGET(soundcard_editor),
							   AGS_TYPE_PREFERENCES);
  window = AGS_WINDOW(preferences->window);
  application_context = (AgsApplicationContext *) window->application_context;

  application_mutex = window->application_mutex;

  /* create soundcard */
  pthread_mutex_lock(application_mutex);

  distributed_manager = ags_sound_provider_get_distributed_manager(AGS_SOUND_PROVIDER(application_context));

  if(distributed_manager == NULL){
    g_warning("distributed manager not found");

    pthread_mutex_unlock(application_mutex);

    return;
  }

  soundcard = ags_sound_provider_get_soundcard(AGS_SOUND_PROVIDER(application_context));
  jack_devout = NULL;
  
  while(soundcard != NULL){
    if(AGS_IS_JACK_DEVOUT(soundcard->data)){
      jack_devout = soundcard->data;
      break;
    }
    
    soundcard = soundcard->next;
  }

  card_id = NULL;
  ags_soundcard_list_cards(AGS_SOUNDCARD(jack_devout),
			   &card_id, NULL);

  gtk_list_store_clear(GTK_LIST_STORE(gtk_combo_box_get_model(GTK_COMBO_BOX(soundcard_editor->card))));

  while(card_id != NULL){
    if(card_id->data != NULL){
      gtk_combo_box_text_append_text(soundcard_editor->card,
				     card_id->data);
    }
    
    card_id = card_id->next;
  }

  /*  */
  gtk_widget_set_sensitive((GtkWidget *) soundcard_editor->buffer_size,
			   FALSE);
  
  gtk_widget_set_sensitive((GtkWidget *) soundcard_editor->samplerate,
			   FALSE);
  pthread_mutex_unlock(application_mutex);
}

void
ags_soundcard_editor_load_alsa_card(AgsSoundcardEditor *soundcard_editor)
{
  AgsWindow *window;
  AgsPreferences *preferences;

  AgsDevout *devout;

  AgsApplicationContext *application_context;

  GList *list;
  GList *card_id;

  pthread_mutex_t *application_mutex;

  if((AGS_SOUNDCARD_EDITOR_BLOCK_LOAD & (soundcard_editor->flags)) != 0){
    return;
  }

  soundcard_editor->flags |= AGS_SOUNDCARD_EDITOR_BLOCK_LOAD;

  preferences = (AgsPreferences *) gtk_widget_get_ancestor(GTK_WIDGET(soundcard_editor),
							   AGS_TYPE_PREFERENCES);
  window = AGS_WINDOW(preferences->window);
  application_context = (AgsApplicationContext *) window->application_context;

  application_mutex = window->application_mutex;
  
  /*  */
  pthread_mutex_lock(application_mutex);

  devout = g_object_new(AGS_TYPE_DEVOUT,
			NULL);
  devout->flags &= (~AGS_DEVOUT_OSS);
  devout->flags |= AGS_DEVOUT_ALSA;
  g_object_set(devout,
	       "application-context", application_context,
	       NULL);

  card_id = NULL;
  ags_soundcard_list_cards(AGS_SOUNDCARD(devout),
			   &card_id, NULL);

  gtk_list_store_clear(GTK_LIST_STORE(gtk_combo_box_get_model(GTK_COMBO_BOX(soundcard_editor->card))));

  while(card_id != NULL){
    if(card_id != NULL){
      gtk_combo_box_text_append_text(soundcard_editor->card,
				     card_id->data);
    }
    
    card_id = card_id->next;
  }

  /* remove previous */
  if(soundcard_editor->soundcard != NULL){
    ags_soundcard_editor_remove_soundcard(soundcard_editor,
					  (GObject *) soundcard_editor->soundcard);
  }

  /* add new */
  ags_soundcard_editor_add_soundcard(soundcard_editor,
				     (GObject *) devout);

  /*  */
  list = gtk_container_get_children((GtkContainer *) GTK_WIDGET(soundcard_editor)->parent);
  
  if(list->data == soundcard_editor){
    gtk_widget_set_sensitive((GtkWidget *) soundcard_editor->buffer_size,
			     TRUE);
  }

  g_list_free(list);
  
  gtk_widget_set_sensitive((GtkWidget *) soundcard_editor->samplerate,
			   TRUE);

  pthread_mutex_unlock(application_mutex);

  soundcard_editor->flags &= (~AGS_SOUNDCARD_EDITOR_BLOCK_LOAD);
}

void
ags_soundcard_editor_load_oss_card(AgsSoundcardEditor *soundcard_editor)
{
  AgsWindow *window;
  AgsPreferences *preferences;

  AgsDevout *devout;

  AgsApplicationContext *application_context;

  GList *list;
  GList *card_id;

  pthread_mutex_t *application_mutex;
  
  preferences = (AgsPreferences *) gtk_widget_get_ancestor(GTK_WIDGET(soundcard_editor),
							   AGS_TYPE_PREFERENCES);
  window = AGS_WINDOW(preferences->window);
  application_context = (AgsApplicationContext *) window->application_context;

  application_mutex = window->application_mutex;
  
  /*  */  
  pthread_mutex_lock(application_mutex);

  devout = g_object_new(AGS_TYPE_DEVOUT,
			NULL);
  devout->flags &= (~AGS_DEVOUT_ALSA);
  devout->flags |= AGS_DEVOUT_OSS;
  g_object_set(devout,
	       "application-context", application_context,
	       NULL);

  card_id = NULL;
  ags_soundcard_list_cards(AGS_SOUNDCARD(devout),
			   &card_id, NULL);

  gtk_list_store_clear(GTK_LIST_STORE(gtk_combo_box_get_model(GTK_COMBO_BOX(soundcard_editor->card))));

  while(card_id != NULL){
    if(card_id != NULL){
      gtk_combo_box_text_append_text(soundcard_editor->card,
				     card_id->data);
    }
    
    card_id = card_id->next;
  }

  /* remove previous */
  if(soundcard_editor->soundcard != NULL){
    ags_soundcard_editor_remove_soundcard(soundcard_editor,
					  (GObject *) soundcard_editor->soundcard);
  }

  /* add new */
  ags_soundcard_editor_add_soundcard(soundcard_editor,
				     (GObject *) devout);

  /*  */
  list = gtk_container_get_children((GtkContainer *) GTK_WIDGET(soundcard_editor)->parent);
  
  if(list->data == soundcard_editor){
    gtk_widget_set_sensitive((GtkWidget *) soundcard_editor->buffer_size,
			     TRUE);
  }

  g_list_free(list);

  gtk_widget_set_sensitive((GtkWidget *) soundcard_editor->samplerate,
			   TRUE);

  pthread_mutex_unlock(application_mutex);
}

/**
 * ags_soundcard_editor_new:
 *
 * Creates an #AgsSoundcardEditor
 *
 * Returns: a new #AgsSoundcardEditor
 *
 * Since: 1.0.0
 */
AgsSoundcardEditor*
ags_soundcard_editor_new()
{
  AgsSoundcardEditor *soundcard_editor;

  soundcard_editor = (AgsSoundcardEditor *) g_object_new(AGS_TYPE_SOUNDCARD_EDITOR,
							 NULL);
  
  return(soundcard_editor);
}

