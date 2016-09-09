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

#include <ags/X/ags_soundcard_editor_callbacks.h>

#include <ags/object/ags_soundcard.h>

#include <ags/thread/ags_mutex_manager.h>
#include <ags/thread/ags_task_thread.h>
#include <ags/thread/ags_task.h>

#include <ags/audio/ags_sound_provider.h>

#include <ags/audio/jack/ags_jack_server.h>

#include <ags/audio/thread/ags_audio_loop.h>

#include <ags/audio/task/ags_set_output_device.h>
#include <ags/audio/task/ags_set_audio_channels.h>
#include <ags/audio/task/ags_set_buffer_size.h>
#include <ags/audio/task/ags_set_samplerate.h>

#include <ags/X/ags_xorg_application_context.h>
#include <ags/X/ags_window.h>
#include <ags/X/ags_preferences.h>

#include <ags/config.h>

void
ags_soundcard_editor_launch_change_soundcard_callback(AgsTask *task,
						      AgsSoundcardEditor *soundcard_editor)
{
  AgsWindow *window;
  AgsPreferences *preferences;

  AgsApplicationContext *application_context;

  GObject *soundcard;

  preferences = (AgsPreferences *) gtk_widget_get_ancestor(GTK_WIDGET(soundcard_editor),
							   AGS_TYPE_PREFERENCES);
  window = preferences->window;

  application_context = window->application_context;

  /* reset default card */
  soundcard = AGS_XORG_APPLICATION_CONTEXT(application_context)->soundcard->data;
  g_object_set(window,
	       "soundcard\0", soundcard,
	       NULL);
}

void
ags_soundcard_editor_backend_changed_callback(GtkComboBox *combo,
					      AgsSoundcardEditor *soundcard_editor)
{
  gchar *str;

  str = gtk_combo_box_text_get_active_text(combo);

  if(str != NULL){
    if(!g_ascii_strncasecmp(str,
			    "alsa\0",
			    5)){
      ags_soundcard_editor_load_alsa_card(soundcard_editor);
    }else if(!g_ascii_strncasecmp(str,
				  "oss\0",
				  4)){
      ags_soundcard_editor_load_oss_card(soundcard_editor);
    }
  }
}

void
ags_soundcard_editor_card_changed_callback(GtkComboBox *combo,
					   AgsSoundcardEditor *soundcard_editor)
{
  AgsWindow *window;

  AgsSoundcard *soundcard;
  GtkTreeIter current;

  AgsMutexManager *mutex_manager;
  AgsThread *main_loop;
  AgsTaskThread *task_thread;

  AgsApplicationContext *application_context;
  AgsConfig *config;
  
  gchar *str;

  gboolean use_jack;
  gboolean use_alsa;
  guint channels, channels_min, channels_max;
  guint rate, rate_min, rate_max;
  guint buffer_size, buffer_size_min, buffer_size_max;

  GValue value =  {0,};

  GError *error;

  pthread_mutex_t *application_mutex;
  
  window = AGS_WINDOW(AGS_PREFERENCES(gtk_widget_get_ancestor(GTK_WIDGET(soundcard_editor),
							      AGS_TYPE_PREFERENCES))->window);
  soundcard = AGS_SOUNDCARD(window->soundcard);

  application_context = window->application_context;

  mutex_manager = ags_mutex_manager_get_instance(mutex_manager);
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);
  
  /* get audio loop */
  pthread_mutex_lock(application_mutex);

  main_loop = application_context->main_loop;

  pthread_mutex_unlock(application_mutex);

  /* get task and soundcard thread */
  task_thread = (AgsTaskThread *) ags_thread_find_type(main_loop,
						       AGS_TYPE_TASK_THREAD);

  /*  */
  use_jack = TRUE;
  
#ifdef AGS_WITH_ALSA
  use_alsa = TRUE;
#else
  use_alsa = FALSE;
#endif

  config = ags_config_get_instance();

  str = ags_config_get_value(config,
			     AGS_CONFIG_SOUNDCARD,
			     "jack\0");

  if(!g_ascii_strncasecmp(str,
			  "disabled\0",
			  9)){
    use_jack = FALSE;
  }

  str = ags_config_get_value(config,
			     AGS_CONFIG_SOUNDCARD,
			     "backend\0");

  if(str != NULL &&
     !g_ascii_strncasecmp(str,
			  "oss\0",
			  4)){
    use_alsa = FALSE;
  }

  str = gtk_combo_box_text_get_active_text(soundcard_editor->card);
  
  if(use_alsa && !use_jack){
    str = g_strndup(str,
		    index(str,
			  ',') - str);
  }
    
  /* reset dialog */
  error = NULL;
  ags_soundcard_pcm_info(soundcard,
			 str,
			 &channels_min, &channels_max,
			 &rate_min, &rate_max,
			 &buffer_size_min, &buffer_size_max,
			 &error);

  if(error != NULL){
    GtkMessageDialog *dialog;

    dialog = (GtkMessageDialog *) gtk_message_dialog_new((GtkWindow *) gtk_widget_get_ancestor(GTK_WIDGET(soundcard_editor),
											       AGS_TYPE_PREFERENCES),
							 GTK_DIALOG_MODAL,
							 GTK_MESSAGE_ERROR,
							 GTK_BUTTONS_CLOSE,
							 "%s\0", error->message);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(GTK_WIDGET(dialog));

    gtk_spin_button_set_range(soundcard_editor->audio_channels, 0.0, 24.0);
    gtk_spin_button_set_range(soundcard_editor->samplerate, 1.0, 192000.0);
    gtk_spin_button_set_range(soundcard_editor->buffer_size, 1.0, 65535.0);

    return;
  }

  gtk_spin_button_set_range(soundcard_editor->audio_channels,
			    channels_min, channels_max);
  gtk_spin_button_set_range(soundcard_editor->samplerate,
			    rate_min, rate_max);
  gtk_spin_button_set_range(soundcard_editor->buffer_size,
			    buffer_size_min, buffer_size_max);
}

void
ags_soundcard_editor_audio_channels_changed(GtkSpinButton *spin_button,
					    AgsSoundcardEditor *soundcard_editor)
{
  AgsWindow *window;
  AgsSoundcard *soundcard;
  AgsSetAudioChannels *set_audio_channels;

  AgsMutexManager *mutex_manager;
  AgsThread *main_loop;
  AgsTaskThread *task_thread;

  AgsApplicationContext *application_context;

  pthread_mutex_t *application_mutex;  

  window = AGS_WINDOW(AGS_PREFERENCES(gtk_widget_get_ancestor(GTK_WIDGET(soundcard_editor),
							      AGS_TYPE_PREFERENCES))->window);
  soundcard = AGS_SOUNDCARD(window->soundcard);

  application_context = window->application_context;

  mutex_manager = ags_mutex_manager_get_instance(mutex_manager);
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);
  
  /* get audio loop */
  pthread_mutex_lock(application_mutex);

  main_loop = application_context->main_loop;

  pthread_mutex_unlock(application_mutex);

  /* get task and soundcard thread */
  task_thread = (AgsTaskThread *) ags_thread_find_type(main_loop,
						       AGS_TYPE_TASK_THREAD);

  /* create set output device task */
  set_audio_channels = ags_set_audio_channels_new(soundcard,
						  (guint) gtk_spin_button_get_value(spin_button));

  /* append AgsSetAudioChannels */
  ags_task_thread_append_task(task_thread,
			      AGS_TASK(set_audio_channels));
}

void
ags_soundcard_editor_samplerate_changed(GtkSpinButton *spin_button,
					AgsSoundcardEditor *soundcard_editor)
{
  AgsWindow *window;
  AgsSoundcard *soundcard;
  AgsSetSamplerate *set_samplerate;

  AgsMutexManager *mutex_manager;
  AgsThread *main_loop;
  AgsTaskThread *task_thread;

  AgsApplicationContext *application_context;

  pthread_mutex_t *application_mutex;
  
  window = AGS_WINDOW(AGS_PREFERENCES(gtk_widget_get_ancestor(GTK_WIDGET(soundcard_editor),
							      AGS_TYPE_PREFERENCES))->window);
  soundcard = AGS_SOUNDCARD(window->soundcard);

  application_context = window->application_context;

  mutex_manager = ags_mutex_manager_get_instance(mutex_manager);
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);
  
  /* get audio loop */
  pthread_mutex_lock(application_mutex);

  main_loop = application_context->main_loop;

  pthread_mutex_unlock(application_mutex);

  /* get task and soundcard thread */
  task_thread = (AgsTaskThread *) ags_thread_find_type(main_loop,
						       AGS_TYPE_TASK_THREAD);

  /* create set output device task */
  set_samplerate = ags_set_samplerate_new((GObject *) soundcard,
					  (guint) gtk_spin_button_get_value(spin_button));

  /* append AgsSetSamplerate */
  ags_task_thread_append_task(task_thread,
			      AGS_TASK(set_samplerate));
}

void
ags_soundcard_editor_buffer_size_changed(GtkSpinButton *spin_button,
					 AgsSoundcardEditor *soundcard_editor)
{
  AgsWindow *window;
  AgsSoundcard *soundcard;
  AgsSetBufferSize *set_buffer_size;

  AgsMutexManager *mutex_manager;
  AgsThread *main_loop;
  AgsTaskThread *task_thread;

  AgsApplicationContext *application_context;

  pthread_mutex_t *application_mutex;
  
  window = AGS_WINDOW(AGS_PREFERENCES(gtk_widget_get_ancestor(GTK_WIDGET(soundcard_editor),
							      AGS_TYPE_PREFERENCES))->window);
  soundcard = AGS_SOUNDCARD(window->soundcard);

  application_context = window->application_context;

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);
  
  /* get audio loop */
  pthread_mutex_lock(application_mutex);

  main_loop = application_context->main_loop;

  pthread_mutex_unlock(application_mutex);

  /* get task and soundcard thread */
  task_thread = (AgsTaskThread *) ags_thread_find_type(main_loop,
						       AGS_TYPE_TASK_THREAD);

  /* create set output device task */
  set_buffer_size = ags_set_buffer_size_new((GObject *) soundcard,
					    (guint) gtk_spin_button_get_value(spin_button));

  /* append AgsSetBufferSize */
  ags_task_thread_append_task(task_thread,
			      AGS_TASK(set_buffer_size));
}
