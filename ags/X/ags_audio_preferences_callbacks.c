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

#include <ags/X/ags_audio_preferences_callbacks.h>

#include <ags/object/ags_connectable.h>

#include <ags/audio/ags_sound_provider.h>
#include <ags/audio/jack/ags_jack_server.h>

#include <ags/X/ags_xorg_application_context.h>
#include <ags/X/ags_window.h>
#include <ags/X/ags_preferences.h>
#include <ags/X/ags_soundcard_editor.h>

#include <ags/config.h>

int
ags_audio_preferences_parent_set_callback(GtkWidget *widget, GtkObject *old_parent, AgsAudioPreferences *audio_preferences)
{
  AgsWindow *window;

  AgsSoundcard *soundcard;
  GtkListStore *model;

  GtkTreeIter iter;
  GList *card_id, *card_name;
  
  if(old_parent != NULL){
    return(0);
  }

  return(0);
}

void
ags_audio_preferences_connect_jack_callback(GtkWidget *widget, AgsAudioPreferences *audio_preferences)
{
  AgsWindow *window;
  AgsPreferences *preferences;

  AgsJackServer *jack_server;
  
  AgsApplicationContext *application_context;

  GList *list;
  
  pthread_mutex_t *application_mutex;

  preferences = (AgsPreferences *) gtk_widget_get_ancestor(GTK_WIDGET(audio_preferences),
							   AGS_TYPE_PREFERENCES);
  window = preferences->window;

  application_context = window->application_context;
  application_mutex = window->application_mutex;

  list = ags_sound_provider_get_distributed_manager(AGS_SOUND_PROVIDER(application_context));

  if(list != NULL){
    jack_server = list->data;

    ags_jack_server_connect_client(jack_server);
  }
}

void
ags_audio_preferences_add_callback(GtkWidget *widget, AgsAudioPreferences *audio_preferences)
{
  AgsWindow *window;
  AgsPreferences *preferences;
  AgsSoundcardEditor *soundcard_editor;

  AgsApplicationContext *application_context;

  GList *list;
  GObject *soundcard;

  pthread_mutex_t *application_mutex;

  preferences = (AgsPreferences *) gtk_widget_get_ancestor(GTK_WIDGET(audio_preferences),
							   AGS_TYPE_PREFERENCES);
  window = preferences->window;

  application_context = window->application_context;
  application_mutex = window->application_mutex;

  soundcard_editor = ags_soundcard_editor_new();
  gtk_box_pack_start(audio_preferences->soundcard_editor,
		     soundcard_editor,
		     FALSE, FALSE,
		     0);
  ags_connectable_connect(AGS_CONNECTABLE(soundcard_editor));
  g_signal_connect(soundcard_editor->remove, "clicked\0",
		   G_CALLBACK(ags_audio_preferences_remove_soundcard_editor_callback), audio_preferences);
  gtk_widget_show_all(soundcard_editor);

  /* reset default card */
  soundcard = NULL;
  
  pthread_mutex_lock(application_mutex);

  list = ags_sound_provider_get_soundcard(AGS_SOUND_PROVIDER(application_context));
  
  if(list != NULL){
    soundcard = list->data;
  }

  pthread_mutex_unlock(application_mutex);
  
  g_object_set(window,
	       "soundcard\0", soundcard,
	       NULL);
}

void
ags_audio_preferences_remove_soundcard_editor_callback(GtkWidget *button,
						       AgsAudioPreferences *audio_preferences)
{
  AgsWindow *window;
  AgsPreferences *preferences;
  AgsSoundcardEditor *soundcard_editor;

  AgsApplicationContext *application_context;

  GList *list;
  GObject *soundcard;

  pthread_mutex_t *application_mutex;
  
  preferences = (AgsPreferences *) gtk_widget_get_ancestor(GTK_WIDGET(audio_preferences),
							   AGS_TYPE_PREFERENCES);
  window = preferences->window;

  application_context = window->application_context;
  application_mutex = window->application_mutex;

  soundcard_editor = gtk_widget_get_ancestor(button,
					     AGS_TYPE_SOUNDCARD_EDITOR);
  ags_soundcard_editor_remove_soundcard(soundcard_editor,
					gtk_combo_box_text_get_active_text(soundcard_editor->card));
  gtk_widget_destroy(soundcard_editor);

  /* reset default card */
  soundcard = NULL;
  
  pthread_mutex_lock(application_mutex);

  list = ags_sound_provider_get_soundcard(AGS_SOUND_PROVIDER(application_context));
  
  if(list != NULL){
    soundcard = list->data;
  }

  pthread_mutex_unlock(application_mutex);
  
  g_object_set(window,
	       "soundcard\0", soundcard,
	       NULL);
}

void
ags_audio_preferences_start_jack_callback(GtkButton *button,
					  AgsAudioPreferences *audio_preferences)
{
  AgsWindow *window;

  AgsJackServer *jack_server;
  
  AgsMutexManager *mutex_manager;

  AgsApplicationContext *application_context;

  jackctl_driver_t *driver;
  
  GList *list;
  JSList *jslist;

  gchar *driver_name;
  
  pthread_mutex_t *application_mutex;

  window = AGS_WINDOW(AGS_PREFERENCES(gtk_widget_get_ancestor(GTK_WIDGET(audio_preferences),
									 AGS_TYPE_PREFERENCES))->window);

  application_context = window->application_context;

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  pthread_mutex_lock(application_mutex);

  //TODO:JK: improve me
  list = ags_sound_provider_get_distributed_manager(AGS_SOUND_PROVIDER(application_context));
  
  while(list != NULL){
    if(AGS_IS_JACK_SERVER(list->data)){
      jack_server = list->data;

      break;
    }
    
    list = list->next;
  }
  
  pthread_mutex_unlock(application_mutex);

  /* find driver */
  driver = NULL;
  
  driver_name = gtk_combo_box_text_get_active_text(audio_preferences->jack_driver);

  jslist = NULL;
  //  jslist = jackctl_server_get_drivers_list(jack_server->jackctl);

  while(jslist != NULL){
    //    if(!g_ascii_strcasecmp(driver_name,
    //			   jackctl_driver_get_name(jslist->data))){
    //      driver = jslist->data;

    //      break;
    //    }
  }

  /* start server */
  if(jack_server != NULL){
    //    jackctl_server_start(jack_server->jackctl);
  }
}

void
ags_audio_preferences_stop_jack_callback(GtkButton *button,
					 AgsAudioPreferences *audio_preferences)
{
  AgsWindow *window;

  AgsJackServer *jack_server;
  
  AgsMutexManager *mutex_manager;

  AgsApplicationContext *application_context;

  GList *list;

  pthread_mutex_t *application_mutex;

  window = AGS_WINDOW(AGS_PREFERENCES(gtk_widget_get_ancestor(GTK_WIDGET(audio_preferences),
									 AGS_TYPE_PREFERENCES))->window);

  application_context = window->application_context;

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  pthread_mutex_lock(application_mutex);

  //TODO:JK: improve me
  list = ags_sound_provider_get_distributed_manager(AGS_SOUND_PROVIDER(application_context));
  
  while(list != NULL){
    if(AGS_IS_JACK_SERVER(list->data)){
      jack_server = list->data;

      break;
    }
    
    list = list->next;
  }
  
  pthread_mutex_unlock(application_mutex);

  if(jack_server != NULL){
    //    jackctl_server_stop(jack_server->jackctl);
  }
}
