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
#include <ags/audio/ags_devout.h>

#include <ags/audio/jack/ags_jack_server.h>
#include <ags/audio/jack/ags_jack_devout.h>

#include <ags/audio/thread/ags_audio_loop.h>

#include <ags/audio/task/ags_set_output_device.h>
#include <ags/audio/task/ags_set_audio_channels.h>
#include <ags/audio/task/ags_set_buffer_size.h>
#include <ags/audio/task/ags_set_samplerate.h>
#include <ags/audio/task/ags_set_format.h>

#include <ags/X/ags_xorg_application_context.h>
#include <ags/X/ags_window.h>
#include <ags/X/ags_preferences.h>

#include <ags/X/task/ags_add_soundcard_editor_jack.h>
#include <ags/X/task/ags_remove_soundcard_editor_jack.h>

#include <ags/config.h>

void
ags_soundcard_editor_backend_changed_callback(GtkComboBox *combo,
					      AgsSoundcardEditor *soundcard_editor)
{
  gchar *str;

  str = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(combo));

  if(str != NULL){
    if(!g_ascii_strncasecmp(str,
			    "jack\0",
			    5)){
      ags_soundcard_editor_load_jack_card(soundcard_editor);

      gtk_widget_show_all((GtkWidget *) soundcard_editor->jack_hbox);
    }else if(!g_ascii_strncasecmp(str,
				  "alsa\0",
				  5)){
      ags_soundcard_editor_load_alsa_card(soundcard_editor);

      gtk_widget_hide((GtkWidget *) soundcard_editor->jack_hbox);
    }else if(!g_ascii_strncasecmp(str,
				  "oss\0",
				  4)){
      ags_soundcard_editor_load_oss_card(soundcard_editor);

      gtk_widget_hide((GtkWidget *) soundcard_editor->jack_hbox);
    }
  }
}

void
ags_soundcard_editor_card_changed_callback(GtkComboBox *combo,
					   AgsSoundcardEditor *soundcard_editor)
{
  AgsWindow *window;

  AgsSetOutputDevice *set_output_device;

  AgsMutexManager *mutex_manager;
  AgsThread *main_loop;
  AgsTaskThread *task_thread;

  AgsApplicationContext *application_context;
  AgsConfig *config;
  GObject *soundcard;

  GtkTreeIter current;
  
  gchar *str;
  gchar *card;

  gboolean use_jack, use_alsa, use_oss;
  guint channels, channels_min, channels_max;
  guint rate, rate_min, rate_max;
  guint buffer_size, buffer_size_min, buffer_size_max;

  GValue value =  {0,};

  GError *error;

  pthread_mutex_t *application_mutex;

  window = AGS_WINDOW(AGS_PREFERENCES(gtk_widget_get_ancestor(GTK_WIDGET(soundcard_editor),
							      AGS_TYPE_PREFERENCES))->window);
  soundcard = soundcard_editor->soundcard;

  application_context = (AgsApplicationContext *) window->application_context;

  mutex_manager = ags_mutex_manager_get_instance(mutex_manager);
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);
  
  /* get audio loop */
  pthread_mutex_lock(application_mutex);

  main_loop = (AgsThread *) application_context->main_loop;

  pthread_mutex_unlock(application_mutex);

  /* get task and soundcard thread */
  task_thread = (AgsTaskThread *) ags_thread_find_type(main_loop,
						       AGS_TYPE_TASK_THREAD);
  
  /*  */
  use_jack = TRUE;
  use_alsa = FALSE;
  use_alsa = FALSE;

  config = ags_config_get_instance();

  str = NULL;
  
  if(AGS_IS_JACK_DEVOUT(soundcard)){
    str = "jack\0";
  }else if(AGS_IS_DEVOUT(soundcard)){
    if((AGS_DEVOUT_ALSA & (AGS_DEVOUT(soundcard)->flags)) != 0){
      str = "alsa\0";
    }else if((AGS_DEVOUT_OSS & (AGS_DEVOUT(soundcard)->flags)) != 0){
      str = "oss\0";
    }
  }
  
  if(str != NULL &&
     !g_ascii_strncasecmp(str,
			  "jack\0",
			  5)){
    use_jack = TRUE;
  }else if(str != NULL &&
	   !g_ascii_strncasecmp(str,
				"alsa\0",
				5)){
    use_jack = FALSE;
    use_alsa = TRUE;
  }else if(str != NULL &&
	   !g_ascii_strncasecmp(str,
				"oss\0",
				4)){
    use_jack = FALSE;
    use_oss = TRUE;
  }

  card = gtk_combo_box_text_get_active_text(soundcard_editor->card);

  if(card == NULL){
    return;
  }
  
  if(use_alsa){
    if(index(card,
	     ',') != NULL){
      str = g_strndup(card,
		      index(card,
			    ',') - card);

      g_free(card);      
      card = str;
    }
  }
  
  /* reset dialog */
  error = NULL;
  ags_soundcard_pcm_info(AGS_SOUNDCARD(soundcard),
			 card,
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

  set_output_device = ags_set_output_device_new(soundcard,
						card);

  gtk_spin_button_set_range(soundcard_editor->audio_channels,
			    channels_min, channels_max);
  gtk_spin_button_set_range(soundcard_editor->samplerate,
			    rate_min, rate_max);
  gtk_spin_button_set_range(soundcard_editor->buffer_size,
			    buffer_size_min, buffer_size_max);
}

void
ags_soundcard_editor_add_jack_callback(GtkWidget *button,
				       AgsSoundcardEditor *soundcard_editor)
{
  AgsWindow *window;
  AgsAddSoundcardEditorJack *add_soundcard_editor_jack;

  AgsMutexManager *mutex_manager;
  AgsThread *main_loop;
  AgsTaskThread *task_thread;

  AgsApplicationContext *application_context;

  pthread_mutex_t *application_mutex;  

  window = AGS_WINDOW(AGS_PREFERENCES(gtk_widget_get_ancestor(GTK_WIDGET(soundcard_editor),
							      AGS_TYPE_PREFERENCES))->window);
  application_context = (AgsApplicationContext *) window->application_context;

  mutex_manager = ags_mutex_manager_get_instance(mutex_manager);
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);
  
  /* get audio loop */
  pthread_mutex_lock(application_mutex);

  main_loop = (AgsThread *) application_context->main_loop;

  pthread_mutex_unlock(application_mutex);

  /* get task and soundcard thread */
  task_thread = (AgsTaskThread *) ags_thread_find_type(main_loop,
						       AGS_TYPE_TASK_THREAD);

  /* create set output device task */
  add_soundcard_editor_jack = ags_add_soundcard_editor_jack_new(soundcard_editor);

  /* append AgsSetAudioChannels */
  ags_task_thread_append_task(task_thread,
			      AGS_TASK(add_soundcard_editor_jack));
}

void
ags_soundcard_editor_remove_jack_callback(GtkWidget *button,
					  AgsSoundcardEditor *soundcard_editor)
{
  AgsWindow *window;
  AgsRemoveSoundcardEditorJack *remove_soundcard_editor_jack;

  AgsMutexManager *mutex_manager;
  AgsThread *main_loop;
  AgsTaskThread *task_thread;

  AgsApplicationContext *application_context;

  pthread_mutex_t *application_mutex;  

  window = AGS_WINDOW(AGS_PREFERENCES(gtk_widget_get_ancestor(GTK_WIDGET(soundcard_editor),
							      AGS_TYPE_PREFERENCES))->window);
  application_context = (AgsApplicationContext *) window->application_context;

  mutex_manager = ags_mutex_manager_get_instance(mutex_manager);
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);
  
  /* get audio loop */
  pthread_mutex_lock(application_mutex);

  main_loop = (AgsThread *) application_context->main_loop;

  pthread_mutex_unlock(application_mutex);

  /* get task and soundcard thread */
  task_thread = (AgsTaskThread *) ags_thread_find_type(main_loop,
						       AGS_TYPE_TASK_THREAD);

  /* create set output device task */
  remove_soundcard_editor_jack = ags_remove_soundcard_editor_jack_new(soundcard_editor,
								      gtk_combo_box_text_get_active_text(soundcard_editor->card));

  /* append AgsSetAudioChannels */
  ags_task_thread_append_task(task_thread,
			      AGS_TASK(remove_soundcard_editor_jack));
}

void
ags_soundcard_editor_audio_channels_changed_callback(GtkSpinButton *spin_button,
						     AgsSoundcardEditor *soundcard_editor)
{
  AgsWindow *window;
  GObject *soundcard;
  AgsSetAudioChannels *set_audio_channels;

  AgsMutexManager *mutex_manager;
  AgsThread *main_loop;
  AgsTaskThread *task_thread;

  AgsApplicationContext *application_context;

  pthread_mutex_t *application_mutex;  

  window = AGS_WINDOW(AGS_PREFERENCES(gtk_widget_get_ancestor(GTK_WIDGET(soundcard_editor),
							      AGS_TYPE_PREFERENCES))->window);
  soundcard = window->soundcard;

  application_context = (AgsApplicationContext *) window->application_context;

  mutex_manager = ags_mutex_manager_get_instance(mutex_manager);
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);
  
  /* get audio loop */
  pthread_mutex_lock(application_mutex);

  main_loop = (AgsThread *) application_context->main_loop;

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
ags_soundcard_editor_samplerate_changed_callback(GtkSpinButton *spin_button,
						 AgsSoundcardEditor *soundcard_editor)
{
  AgsWindow *window;
  GObject *soundcard;
  AgsSetSamplerate *set_samplerate;

  AgsMutexManager *mutex_manager;
  AgsThread *main_loop;
  AgsTaskThread *task_thread;

  AgsApplicationContext *application_context;

  pthread_mutex_t *application_mutex;
  
  window = AGS_WINDOW(AGS_PREFERENCES(gtk_widget_get_ancestor(GTK_WIDGET(soundcard_editor),
							      AGS_TYPE_PREFERENCES))->window);
  soundcard = window->soundcard;

  application_context = (AgsApplicationContext *) window->application_context;

  mutex_manager = ags_mutex_manager_get_instance(mutex_manager);
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);
  
  /* get audio loop */
  pthread_mutex_lock(application_mutex);

  main_loop = (AgsThread *) application_context->main_loop;

  pthread_mutex_unlock(application_mutex);

  /* get task and soundcard thread */
  task_thread = (AgsTaskThread *) ags_thread_find_type(main_loop,
						       AGS_TYPE_TASK_THREAD);

  /* create set output device task */
  set_samplerate = ags_set_samplerate_new(soundcard,
					  (guint) gtk_spin_button_get_value(spin_button));

  /* append AgsSetSamplerate */
  ags_task_thread_append_task(task_thread,
			      AGS_TASK(set_samplerate));
}

void
ags_soundcard_editor_buffer_size_changed_callback(GtkSpinButton *spin_button,
						  AgsSoundcardEditor *soundcard_editor)
{
  AgsWindow *window;
  GObject *soundcard;
  AgsSetBufferSize *set_buffer_size;

  AgsMutexManager *mutex_manager;
  AgsThread *main_loop;
  AgsTaskThread *task_thread;

  AgsApplicationContext *application_context;

  pthread_mutex_t *application_mutex;
  
  window = AGS_WINDOW(AGS_PREFERENCES(gtk_widget_get_ancestor(GTK_WIDGET(soundcard_editor),
							      AGS_TYPE_PREFERENCES))->window);
  soundcard = window->soundcard;

  application_context = (AgsApplicationContext *) window->application_context;

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);
  
  /* get audio loop */
  pthread_mutex_lock(application_mutex);

  main_loop = (AgsThread *) application_context->main_loop;

  pthread_mutex_unlock(application_mutex);

  /* get task and soundcard thread */
  task_thread = (AgsTaskThread *) ags_thread_find_type(main_loop,
						       AGS_TYPE_TASK_THREAD);

  /* create set output device task */
  set_buffer_size = ags_set_buffer_size_new(soundcard,
					    (guint) gtk_spin_button_get_value(spin_button));

  /* append AgsSetBufferSize */
  ags_task_thread_append_task(task_thread,
			      AGS_TASK(set_buffer_size));
}

void
ags_soundcard_editor_format_changed_callback(GtkComboBox *combo_box,
					     AgsSoundcardEditor *soundcard_editor)
{
  AgsWindow *window;
  GObject *soundcard;
  AgsSetFormat *set_format;

  AgsMutexManager *mutex_manager;
  AgsThread *main_loop;
  AgsTaskThread *task_thread;

  AgsApplicationContext *application_context;

  guint format;
  
  pthread_mutex_t *application_mutex;
  
  window = AGS_WINDOW(AGS_PREFERENCES(gtk_widget_get_ancestor(GTK_WIDGET(soundcard_editor),
							      AGS_TYPE_PREFERENCES))->window);
  soundcard = window->soundcard;

  application_context = (AgsApplicationContext *) window->application_context;

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);
  
  /* get audio loop */
  pthread_mutex_lock(application_mutex);

  main_loop = (AgsThread *) application_context->main_loop;

  pthread_mutex_unlock(application_mutex);

  /* get task and soundcard thread */
  task_thread = (AgsTaskThread *) ags_thread_find_type(main_loop,
						       AGS_TYPE_TASK_THREAD);

  /* format */
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

  /* create set output device task */
  set_format = ags_set_format_new((GObject *) soundcard,
				  format);

  /* append AgsSetBufferSize */
  ags_task_thread_append_task(task_thread,
			      AGS_TASK(set_format));
}
