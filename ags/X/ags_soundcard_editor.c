/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2015 Joël Krähemann
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

#include <ags/object/ags_config.h>
#include <ags/object/ags_connectable.h>
#include <ags/object/ags_applicable.h>
#include <ags/object/ags_distributed_manager.h>
#include <ags/object/ags_soundcard.h>

#include <ags/audio/ags_sound_provider.h>
#include <ags/audio/ags_devout.h>

#include <ags/audio/jack/ags_jack_server.h>
#include <ags/audio/jack/ags_jack_devout.h>

#include <ags/X/ags_xorg_application_context.h>
#include <ags/X/ags_window.h>
#include <ags/X/ags_preferences.h>

#include <ags/config.h>

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
						       "AgsSoundcardEditor\0", &ags_soundcard_editor_info,
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

  soundcard_editor->soundcard = NULL;
  soundcard_editor->soundcard_thread = NULL;
  
  table = (GtkTable *) gtk_table_new(3, 8, FALSE);
  gtk_box_pack_start(GTK_BOX(soundcard_editor),
		     GTK_WIDGET(table),
		     FALSE, FALSE,
		     2);

  /* backend */
  label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				    "label\0", "backend\0",
				    "xalign\0", 0.0,
				    NULL);
  gtk_table_attach(table,
		   GTK_WIDGET(label),
		   0, 1,
		   0, 1,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  soundcard_editor->backend = (GtkComboBox *) gtk_combo_box_text_new();
  gtk_table_attach(table,
		   GTK_WIDGET(soundcard_editor->backend),
		   1, 2,
		   0, 1,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  gtk_combo_box_text_append_text(soundcard_editor->backend,
				 "jack\0");
  
#ifdef AGS_WITH_ALSA
  gtk_combo_box_text_append_text(soundcard_editor->backend,
				 "alsa\0");
#endif
  
  gtk_combo_box_text_append_text(soundcard_editor->backend,
				 "oss\0");
  gtk_combo_box_set_active(soundcard_editor->backend,
			   0);
  
  /* sound card */
  label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				    "label\0", "sound card\0",
				    "xalign\0", 0.0,
				    NULL);
  gtk_table_attach(table,
		   GTK_WIDGET(label),
		   0, 1,
		   1, 2,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  soundcard_editor->card = (GtkComboBox *) gtk_combo_box_text_new();
  gtk_table_attach(table,
		   GTK_WIDGET(soundcard_editor->card),
		   1, 2,
		   1, 2,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  /* jack */
  soundcard_editor->jack_hbox = NULL;
  soundcard_editor->add_jack = NULL;
  soundcard_editor->remove_jack = NULL;

  soundcard_editor->jack_hbox = gtk_hbox_new(FALSE,
					     0);
  gtk_table_attach(table,
		   GTK_WIDGET(soundcard_editor->jack_hbox),
		   2, 3,
		   1, 2,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  soundcard_editor->add_jack = gtk_button_new_from_stock(GTK_STOCK_ADD);
  gtk_box_pack_start(soundcard_editor->jack_hbox,
		     soundcard_editor->add_jack,
		     FALSE, FALSE,
		     0);
  
  soundcard_editor->remove_jack = gtk_button_new_from_stock(GTK_STOCK_REMOVE);
  gtk_box_pack_start(soundcard_editor->jack_hbox,
		     soundcard_editor->remove_jack,
		     FALSE, FALSE,
		     0);
  
  /* audio channels */
  label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				    "label\0", "audio channels\0",
				    "xalign\0", 0.0,
				    NULL);
  gtk_table_attach(table,
		   GTK_WIDGET(label),
		   0, 1,
		   2, 3,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  soundcard_editor->audio_channels = (GtkSpinButton *) gtk_spin_button_new_with_range(1.0, 24.0, 1.0);
  gtk_spin_button_set_value(soundcard_editor->audio_channels, 2);
  gtk_table_attach(table,
		   GTK_WIDGET(soundcard_editor->audio_channels),
		   1, 2,
		   2, 3,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  /* samplerate */
  label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				    "label\0", "samplerate\0",
				    "xalign\0", 0.0,
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
				    "label\0", "buffer size\0",
				    "xalign\0", 0.0,
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
				    "label\0", "format\0",
				    "xalign\0", 0.0,
				    NULL);
  gtk_table_attach(table,
		   GTK_WIDGET(label),
		   0, 1,
		   5, 6,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  soundcard_editor->format = (GtkComboBox *) gtk_combo_box_text_new();
  gtk_table_attach(table,
		   GTK_WIDGET(soundcard_editor->format),
		   1, 2,
		   5, 6,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  gtk_combo_box_text_append_text(soundcard_editor->format,
				 "8\0");
  gtk_combo_box_text_append_text(soundcard_editor->format,
				 "16\0");
  gtk_combo_box_text_append_text(soundcard_editor->format,
				 "24\0");
  gtk_combo_box_text_append_text(soundcard_editor->format,
				 "32\0");
  gtk_combo_box_text_append_text(soundcard_editor->format,
				 "64\0");
  
  gtk_combo_box_set_active(soundcard_editor->format,
			   1);

  /*  */
  //  soundcard_editor->remove = NULL;

  soundcard_editor->remove = gtk_button_new_from_stock(GTK_STOCK_REMOVE);
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

  /* backend and card */
  g_signal_connect(G_OBJECT(soundcard_editor->backend), "changed\0",
		   G_CALLBACK(ags_soundcard_editor_backend_changed_callback), soundcard_editor);

  g_signal_connect(G_OBJECT(soundcard_editor->card), "changed\0",
		   G_CALLBACK(ags_soundcard_editor_card_changed_callback), soundcard_editor);

  /* add / remove jack */
  g_signal_connect(G_OBJECT(soundcard_editor->add_jack), "clicked\0",
		   G_CALLBACK(ags_soundcard_editor_add_jack_callback), soundcard_editor);

  g_signal_connect(G_OBJECT(soundcard_editor->remove_jack), "clicked\0",
		   G_CALLBACK(ags_soundcard_editor_remove_jack_callback), soundcard_editor);

  /* presets */
  g_signal_connect(G_OBJECT(soundcard_editor->audio_channels), "changed\0",
		   G_CALLBACK(ags_soundcard_editor_audio_channels_changed_callback), soundcard_editor);

  g_signal_connect(G_OBJECT(soundcard_editor->samplerate), "changed\0",
		   G_CALLBACK(ags_soundcard_editor_samplerate_changed_callback), soundcard_editor);

  g_signal_connect(G_OBJECT(soundcard_editor->buffer_size), "changed\0",
		   G_CALLBACK(ags_soundcard_editor_buffer_size_changed_callback), soundcard_editor);

  g_signal_connect(G_OBJECT(soundcard_editor->format), "changed\0",
		   G_CALLBACK(ags_soundcard_editor_format_changed_callback), soundcard_editor);
}

void
ags_soundcard_editor_disconnect(AgsConnectable *connectable)
{
  /* empty */
}

static void
ags_soundcard_editor_finalize(GObject *gobject)
{
  //TODO:JK: implement me
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
  gboolean use_jack, use_alsa, use_oss;
  gboolean add_jack, add_alsa, add_oss;
  
  GValue value =  {0,};

  pthread_mutex_t *application_mutex;

  soundcard_editor = AGS_SOUNDCARD_EDITOR(applicable);
  audio_preferences = (AgsAudioPreferences *) gtk_widget_get_ancestor(GTK_WIDGET(soundcard_editor),
								      AGS_TYPE_AUDIO_PREFERENCES);

  config = ags_config_get_instance();

  list = gtk_container_get_children(audio_preferences->soundcard_editor);
  nth = g_list_index(list,
		     soundcard_editor);

  if(nth < 0){
    return;
  }
  
  soundcard_group = g_strdup_printf("%s-%d\0",
				    AGS_CONFIG_SOUNDCARD,
				    nth);
  g_list_free(list);
  
  /* backend */
  use_jack = TRUE;
  use_alsa = FALSE;
  use_oss = FALSE;

  backend = gtk_combo_box_text_get_active_text(soundcard_editor->backend);
  ags_config_set_value(config,
		       soundcard_group,
		       "backend\0",
		       backend);

  if(backend != NULL){
    if(!g_ascii_strncasecmp(backend,
			    "jack\0",
			    5)){
      use_jack = TRUE;
    }else if(!g_ascii_strncasecmp(backend,
			    "alsa\0",
			    5)){
      use_alsa = TRUE;
    }else if(!g_ascii_strncasecmp(backend,
				  "oss\0",
				  4)){
      use_oss = TRUE;
    }
  }

  /* buffer size */
  buffer_size = gtk_spin_button_get_value(soundcard_editor->buffer_size);
  str = g_strdup_printf("%u\0",
			buffer_size);
  ags_config_set_value(config,
		       soundcard_group,
		       "buffer-size\0",
		       str);
  g_free(str);

  /* pcm channels */
  channels = gtk_spin_button_get_value(soundcard_editor->audio_channels);
  str = g_strdup_printf("%u\0",
			channels);
  ags_config_set_value(config,
		       soundcard_group,
		       "pcm-channels\0",
		       str);
  g_free(str);

  /* format */
  switch(gtk_combo_box_get_active(soundcard_editor->format)){
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

  str = g_strdup_printf("%u\0",
			format);
  ags_config_set_value(config,
		       soundcard_group,
		       "format\0",
		       str);
  g_free(str);

  /* samplerate */
  samplerate = gtk_spin_button_get_value(soundcard_editor->samplerate);
  str = g_strdup_printf("%u\0",
			samplerate);
  ags_config_set_value(config,
		       soundcard_group,
		       "samplerate\0",
		       str);
  g_free(str);
  
  /* device */
  model = gtk_combo_box_get_model(soundcard_editor->card);
  
  if(gtk_combo_box_get_active_iter(soundcard_editor->card,
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
  g_message("%s\0", device);

  if(use_jack){
    ags_config_set_value(config,
			 soundcard_group,
			 "device\0",
			 device);
  }else if(use_alsa){
    ags_config_set_value(config,
			 soundcard_group,
			 "device\0",
			 device);
  }else if(use_oss){
    ags_config_set_value(config,
			 soundcard_group,
			 "device\0",
			 device);
  }
}

void
ags_soundcard_editor_reset(AgsApplicable *applicable)
{
  AgsWindow *window;
  AgsPreferences *preferences;
  AgsSoundcardEditor *soundcard_editor;
  
  AgsConfig *config;

  GObject *soundcard;

  GList *card_id, *card_id_start, *card_name, *card_name_start;

  gchar *backend, *device, *str, *tmp;

  guint nth;
  guint channels, channels_min, channels_max;
  guint samplerate, samplerate_min, samplerate_max;
  guint buffer_size, buffer_size_min, buffer_size_max;
  guint format;
  gboolean use_jack, use_alsa, use_oss;
  gboolean found_card;
  
  GValue value =  {0,};
  
  GError *error;
  
  soundcard_editor = AGS_SOUNDCARD_EDITOR(applicable);
  soundcard = soundcard_editor->soundcard;
  
  /* refresh */
  use_jack = TRUE;
  use_alsa = FALSE;
  use_oss = FALSE;

  backend = ags_config_get_value(config,
				 AGS_CONFIG_SOUNDCARD,
				 "backend\0");

  if(backend != NULL){
    if(!g_ascii_strncasecmp(backend,
			    "jack\0",
			    5)){
      use_jack = TRUE;
      gtk_combo_box_set_active(soundcard_editor->backend,
			       0);
    }else if(!g_ascii_strncasecmp(backend,
				  "alsa\0",
				  5)){
      use_alsa = TRUE;
      use_jack = FALSE;
      gtk_combo_box_set_active(soundcard_editor->backend,
			       1);
    }else if(!g_ascii_strncasecmp(backend,
				  "oss\0",
				  4)){
      use_oss = TRUE;
      use_jack = FALSE;
      gtk_combo_box_set_active(soundcard_editor->backend,
			       2);
    }
  }

  /*  */
  device = ags_soundcard_get_device(AGS_SOUNDCARD(soundcard));
  
  card_id = NULL;
  card_name = NULL;

  ags_soundcard_list_cards(soundcard,
			   &card_id_start, &card_name_start);

  card_id = card_id_start;
  card_name = card_name_start;
  
  nth = 0;
  found_card = FALSE;

  gtk_list_store_clear(GTK_LIST_STORE(gtk_combo_box_get_model(soundcard_editor->card)));

  while(card_id != NULL){
    //FIXME:JK: work-around for alsa-handle
    tmp = card_id->data;
    
    if(use_alsa){
      tmp = g_strdup_printf("%s,0\0",
			    card_id->data);
    }
    
    if(!g_ascii_strcasecmp(tmp,
			   device)){
      found_card = TRUE;
    }

    gtk_combo_box_text_append_text(soundcard_editor->card,
				   tmp);

    if(use_alsa){
      g_free(tmp);
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

  gtk_combo_box_set_active(soundcard_editor->card,
			   nth);

  g_list_free_full(card_id_start,
		   g_free);
  g_list_free_full(card_name_start,
		   g_free);
  
  /*  */
  ags_soundcard_get_presets(soundcard,
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
    gtk_combo_box_set_active(soundcard_editor->format,
			     0);
    break;
  case AGS_SOUNDCARD_SIGNED_16_BIT:
    gtk_combo_box_set_active(soundcard_editor->format,
			     1);
    break;
  case AGS_SOUNDCARD_SIGNED_24_BIT:
    gtk_combo_box_set_active(soundcard_editor->format,
			     2);
    break;
  case AGS_SOUNDCARD_SIGNED_32_BIT:
    gtk_combo_box_set_active(soundcard_editor->format,
			     3);
    break;
  case AGS_SOUNDCARD_SIGNED_64_BIT:
    gtk_combo_box_set_active(soundcard_editor->format,
			     4);
    break;
  }

  /*  */
  if(str != NULL &&
     soundcard != NULL){
    error = NULL;
    ags_soundcard_pcm_info(AGS_SOUNDCARD(soundcard),
			   str,
			   &channels_min, &channels_max,
			   &samplerate_min, &samplerate_max,
			   &buffer_size_min, &buffer_size_max,
			   &error);
    g_free(str);
  }else{
    channels_min = 0.0;
    channels_max = 24.0;
    samplerate_min = 1.0;
    samplerate_max = 192000.0;
    buffer_size_min = 1.0;
    buffer_size_max = 65535.0;
  }
  
  gtk_spin_button_set_range(soundcard_editor->audio_channels,
			    channels_min, channels_max);
  gtk_spin_button_set_range(soundcard_editor->samplerate,
			    samplerate_min, samplerate_max);
  gtk_spin_button_set_range(soundcard_editor->buffer_size,
			    buffer_size_min, buffer_size_max);
}

void
ags_soundcard_editor_add_jack(AgsSoundcardEditor *soundcard_editor,
			      gchar *device)
{
  AgsWindow *window;
  AgsPreferences *preferences;

  AgsJackServer *jack_server;
  AgsJackDevout *jack_devout;

  AgsMutexManager *mutex_manager;

  AgsApplicationContext *application_context;

  GList *distributed_manager;
  GList *card_name, *card_uri;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *mutex;

  preferences = (AgsPreferences *) gtk_widget_get_ancestor(GTK_WIDGET(soundcard_editor),
							   AGS_TYPE_PREFERENCES);
  window = AGS_WINDOW(preferences->window);

  application_context = window->application_context;
  application_mutex = window->application_mutex;

  mutex_manager = ags_mutex_manager_get_instance();
  
  /* create soundcard */
  pthread_mutex_lock(application_mutex);

  distributed_manager = ags_sound_provider_get_distributed_manager(AGS_SOUND_PROVIDER(application_context));

  if(distributed_manager != NULL){
    jack_server = distributed_manager->data;
  }else{
    g_warning("distributed manager not found\0");

    pthread_mutex_unlock(application_mutex);
    
    return;
  }
  
  jack_devout = ags_distributed_manager_register_soundcard(AGS_DISTRIBUTED_MANAGER(jack_server),
							   TRUE);

  pthread_mutex_unlock(application_mutex);

  if(jack_devout == NULL){
    return;
  }

  pthread_mutex_lock(application_mutex);

  mutex = ags_mutex_manager_lookup(mutex_manager,
				   jack_devout);
  
  pthread_mutex_unlock(application_mutex);

  /*  */
  if(device != NULL){
    //NOTE:JK: don't
    //    ags_soundcard_set_device(AGS_SOUNDCARD(jack_devout),
    //			     device);
  }

  /* add new */
  ags_soundcard_editor_add_soundcard(soundcard_editor,
  				     jack_devout);

  /*  */
  pthread_mutex_lock(mutex);

  card_name = NULL;
  card_uri = NULL;
  ags_soundcard_list_cards(AGS_SOUNDCARD(jack_devout),
			   &card_uri, &card_name);

  pthread_mutex_unlock(mutex);

  gtk_list_store_clear(GTK_LIST_STORE(gtk_combo_box_get_model(soundcard_editor->card)));

  while(card_uri != NULL){
    gtk_combo_box_text_append_text(soundcard_editor->card,
    				   card_uri->data);

    
    card_uri = card_uri->next;
  }
}

void
ags_soundcard_editor_remove_jack(AgsSoundcardEditor *soundcard_editor,
				 gchar *device)
{
  AgsWindow *window;
  AgsPreferences *preferences;

  AgsJackServer *jack_server;
  AgsJackDevout *jack_devout;

  AgsMutexManager *mutex_manager;

  AgsApplicationContext *application_context;

  GList *distributed_manager;
  GList *soundcard;
  GList *card_id;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *mutex;

  preferences = (AgsPreferences *) gtk_widget_get_ancestor(GTK_WIDGET(soundcard_editor),
							   AGS_TYPE_PREFERENCES);
  window = AGS_WINDOW(preferences->window);
  application_context = window->application_context;

  application_mutex = window->application_mutex;
    
  /* create soundcard */
  pthread_mutex_lock(application_mutex);

  distributed_manager = ags_sound_provider_get_distributed_manager(AGS_SOUND_PROVIDER(application_context));

  if(distributed_manager != NULL){
    jack_server = distributed_manager->data;
  }else{
    g_warning("distributed manager not found\0");

    pthread_mutex_unlock(application_mutex);
    
    return;
  }

  soundcard = ags_sound_provider_get_soundcard(AGS_SOUND_PROVIDER(application_context));
  jack_devout = NULL;
  
  while(soundcard != NULL){
    if(AGS_IS_JACK_DEVOUT(soundcard->data) &&
       !g_ascii_strcasecmp(ags_soundcard_get_device(AGS_SOUNDCARD(soundcard->data)),
			   device)){
      jack_devout = soundcard->data;
      break;
    }
    
    soundcard = soundcard->next;
  }

  pthread_mutex_unlock(application_mutex);

  /*  */  
  pthread_mutex_lock(application_mutex);

  mutex = ags_mutex_manager_lookup(mutex_manager,
				   jack_devout);
  
  pthread_mutex_unlock(application_mutex);

  /*  */
  pthread_mutex_lock(mutex);

  card_id = NULL;
  ags_soundcard_list_cards(AGS_SOUNDCARD(jack_devout),
			   &card_id, NULL);

  pthread_mutex_unlock(mutex);

  gtk_list_store_clear(GTK_LIST_STORE(gtk_combo_box_get_model(soundcard_editor->card)));

  while(card_id != NULL){
    gtk_combo_box_text_append_text(soundcard_editor->card,
				   card_id->data);

    
    card_id = card_id->next;
  }

  /* remove */
  ags_soundcard_editor_remove_soundcard(soundcard_editor,
					jack_devout);
}

void
ags_soundcard_editor_add_soundcard(AgsSoundcardEditor *soundcard_editor,
				   GObject *soundcard)
{
  AgsWindow *window;
  AgsPreferences *preferences;

  AgsThread *audio_loop;
  AgsThread *soundcard_thread;

  AgsApplicationContext *application_context;

  pthread_mutex_t *application_mutex;

  if(soundcard == NULL){
    return;
  }
  
  preferences = (AgsPreferences *) gtk_widget_get_ancestor(GTK_WIDGET(soundcard_editor),
							   AGS_TYPE_PREFERENCES);
  window = AGS_WINDOW(preferences->window);
  application_context = window->application_context;
  application_mutex = window->application_mutex;

  /*  */
  pthread_mutex_lock(application_mutex);

  audio_loop = application_context->main_loop;
  
  soundcard_editor->soundcard = soundcard;

  ags_sound_provider_set_soundcard(AGS_SOUND_PROVIDER(application_context),
				   g_list_append(ags_sound_provider_get_soundcard(AGS_SOUND_PROVIDER(application_context)),
						 soundcard));

  pthread_mutex_unlock(application_mutex);  
    
  g_object_ref(soundcard);

  soundcard_editor->soundcard_thread = 
    soundcard_thread = (AgsThread *) ags_soundcard_thread_new(soundcard);

  ags_thread_add_child_extended(AGS_THREAD(audio_loop),
				soundcard_thread,
				TRUE, TRUE);

  if(ags_sound_provider_get_default_soundcard_thread(AGS_SOUND_PROVIDER(application_context)) == NULL){
    ags_sound_provider_set_default_soundcard_thread(AGS_SOUND_PROVIDER(application_context),
						    soundcard_thread);
  }
}

void
ags_soundcard_editor_remove_soundcard(AgsSoundcardEditor *soundcard_editor,
				      GObject *soundcard)
{
  AgsWindow *window;
  AgsPreferences *preferences;

  AgsThread *audio_loop;
  AgsThread *soundcard_thread;

  AgsApplicationContext *application_context;

  pthread_mutex_t *application_mutex;

  preferences = (AgsPreferences *) gtk_widget_get_ancestor(GTK_WIDGET(soundcard_editor),
							   AGS_TYPE_PREFERENCES);
  window = AGS_WINDOW(preferences->window);
  application_context = window->application_context;  

  application_mutex = window->application_mutex;
  
  pthread_mutex_lock(application_mutex);

  audio_loop = application_context->main_loop;
    
  if(soundcard == soundcard_editor->soundcard){
    soundcard_editor->soundcard = NULL;
  }

  if(soundcard != NULL){
    ags_sound_provider_set_soundcard(AGS_SOUND_PROVIDER(application_context),
				     g_list_remove(ags_sound_provider_get_soundcard(AGS_SOUND_PROVIDER(application_context)),
						   soundcard));
    g_object_unref(soundcard);
  }
  
  if(soundcard_editor->soundcard_thread != NULL){
    ags_thread_stop(soundcard_editor->soundcard_thread);
    
    ags_thread_remove_child(AGS_THREAD(audio_loop),
			    soundcard_editor->soundcard_thread);

    g_object_unref(soundcard_editor->soundcard_thread);    
    soundcard_editor->soundcard_thread = NULL;
  }

  pthread_mutex_unlock(application_mutex);
}

void
ags_soundcard_editor_load_jack_card(AgsSoundcardEditor *soundcard_editor)
{
  AgsWindow *window;
  AgsPreferences *preferences;

  AgsJackServer *jack_server;
  AgsJackDevout *jack_devout;

  AgsApplicationContext *application_context;

  GList *distributed_manager;
  GList *soundcard;
  GList *card_id;

  pthread_mutex_t *application_mutex;

  preferences = (AgsPreferences *) gtk_widget_get_ancestor(GTK_WIDGET(soundcard_editor),
							   AGS_TYPE_PREFERENCES);
  window = AGS_WINDOW(preferences->window);
  application_context = window->application_context;

  application_mutex = window->application_mutex;
    
  /* create soundcard */
  pthread_mutex_lock(application_mutex);

  distributed_manager = ags_sound_provider_get_distributed_manager(AGS_SOUND_PROVIDER(application_context));

  if(distributed_manager != NULL){
    jack_server = distributed_manager->data;
  }else{
    g_warning("distributed manager not found\0");

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

  gtk_list_store_clear(GTK_LIST_STORE(gtk_combo_box_get_model(soundcard_editor->card)));

  while(card_id != NULL){
    gtk_combo_box_text_append_text(soundcard_editor->card,
				   card_id->data);

    
    card_id = card_id->next;
  }

  pthread_mutex_unlock(application_mutex);
}

void
ags_soundcard_editor_load_alsa_card(AgsSoundcardEditor *soundcard_editor)
{
  AgsWindow *window;
  AgsPreferences *preferences;

  AgsDevout *devout;

  AgsApplicationContext *application_context;

  GList *card_id;

  pthread_mutex_t *application_mutex;

  preferences = (AgsPreferences *) gtk_widget_get_ancestor(GTK_WIDGET(soundcard_editor),
							   AGS_TYPE_PREFERENCES);
  window = AGS_WINDOW(preferences->window);
  application_context = window->application_context;

  application_mutex = window->application_mutex;
  
  /*  */
  pthread_mutex_lock(application_mutex);

  devout = ags_devout_new(application_context);
  devout->flags &= (~AGS_DEVOUT_OSS);
  devout->flags |= AGS_DEVOUT_ALSA;

  card_id = NULL;
  ags_soundcard_list_cards(AGS_SOUNDCARD(devout),
			   &card_id, NULL);

  gtk_list_store_clear(GTK_LIST_STORE(gtk_combo_box_get_model(soundcard_editor->card)));

  while(card_id != NULL){
    gtk_combo_box_text_append_text(soundcard_editor->card,
				   card_id->data);

    
    card_id = card_id->next;
  }

  /* remove previous */
  if(soundcard_editor->soundcard != NULL){
    ags_soundcard_editor_remove_soundcard(soundcard_editor,
					  soundcard_editor->soundcard);
  }

  /* add new */
  ags_soundcard_editor_add_soundcard(soundcard_editor,
				     devout);

  pthread_mutex_unlock(application_mutex);
}

void
ags_soundcard_editor_load_oss_card(AgsSoundcardEditor *soundcard_editor)
{
  AgsWindow *window;
  AgsPreferences *preferences;

  AgsDevout *devout;

  AgsApplicationContext *application_context;

  GList *card_id;

  pthread_mutex_t *application_mutex;
  
  preferences = (AgsPreferences *) gtk_widget_get_ancestor(GTK_WIDGET(soundcard_editor),
							   AGS_TYPE_PREFERENCES);
  window = AGS_WINDOW(preferences->window);
  application_context = window->application_context;

  application_mutex = window->application_mutex;
  
  /*  */  
  pthread_mutex_lock(application_mutex);

  devout = ags_devout_new(application_context);
  devout->flags &= (~AGS_DEVOUT_ALSA);
  devout->flags |= AGS_DEVOUT_OSS;

  card_id = NULL;
  ags_soundcard_list_cards(AGS_SOUNDCARD(devout),
			   &card_id, NULL);

  gtk_list_store_clear(GTK_LIST_STORE(gtk_combo_box_get_model(soundcard_editor->card)));

  while(card_id != NULL){
    gtk_combo_box_text_append_text(soundcard_editor->card,
				   card_id->data);

    
    card_id = card_id->next;
  }

  /* remove previous */
  if(soundcard_editor->soundcard != NULL){
    ags_soundcard_editor_remove_soundcard(soundcard_editor,
					  soundcard_editor->soundcard);
  }

  /* add new */
  ags_soundcard_editor_add_soundcard(soundcard_editor,
				     devout);

  pthread_mutex_unlock(application_mutex);
}

/**
 * ags_soundcard_editor_new:
 *
 * Creates an #AgsSoundcardEditor
 *
 * Returns: a new #AgsSoundcardEditor
 *
 * Since: 0.7.65
 */
AgsSoundcardEditor*
ags_soundcard_editor_new()
{
  AgsSoundcardEditor *soundcard_editor;

  soundcard_editor = (AgsSoundcardEditor *) g_object_new(AGS_TYPE_SOUNDCARD_EDITOR,
							 NULL);
  
  return(soundcard_editor);
}

