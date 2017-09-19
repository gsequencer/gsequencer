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

#include <ags/X/ags_sequencer_editor_callbacks.h>

#include <ags/object/ags_sequencer.h>

#include <ags/thread/ags_mutex_manager.h>
#include <ags/thread/ags_task.h>

#include <ags/audio/ags_sound_provider.h>
#include <ags/audio/ags_midiin.h>

#include <ags/audio/thread/ags_audio_loop.h>

#include <ags/audio/jack/ags_jack_server.h>
#include <ags/audio/jack/ags_jack_midiin.h>

#include <ags/audio/task/ags_set_input_device.h>

#include <ags/X/ags_xorg_application_context.h>
#include <ags/X/ags_window.h>
#include <ags/X/ags_preferences.h>

#include <ags/X/thread/ags_gui_thread.h>

#include <ags/X/task/ags_add_sequencer_editor_jack.h>
#include <ags/X/task/ags_remove_sequencer_editor_jack.h>

#include <ags/config.h>

void
ags_sequencer_editor_backend_changed_callback(GtkComboBox *combo,
					      AgsSequencerEditor *sequencer_editor)
{
  gchar *str;

  str = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(combo));

  if(str != NULL){
    if(!g_ascii_strncasecmp(str,
			    "jack",
			    5)){
      ags_sequencer_editor_load_jack_card(sequencer_editor);

      gtk_widget_show_all((GtkWidget *) sequencer_editor->jack_hbox);
    }else if(!g_ascii_strncasecmp(str,
				  "alsa",
				  5)){
      ags_sequencer_editor_load_alsa_card(sequencer_editor);

      gtk_widget_hide((GtkWidget *) sequencer_editor->jack_hbox);
    }else if(!g_ascii_strncasecmp(str,
				  "oss",
				  4)){
      ags_sequencer_editor_load_oss_card(sequencer_editor);

      gtk_widget_hide((GtkWidget *) sequencer_editor->jack_hbox);
    }
  }
}

void
ags_sequencer_editor_card_changed_callback(GtkComboBox *combo,
					   AgsSequencerEditor *sequencer_editor)
{
  AgsWindow *window;

  AgsSetInputDevice *set_input_device;

  AgsMutexManager *mutex_manager;
  AgsThread *main_loop;
  AgsGuiThread *gui_thread;

  AgsApplicationContext *application_context;
  GObject *sequencer;

  GtkTreeIter current;
  
  gchar *str;
  gchar *card;

  gboolean use_alsa;
  guint channels, channels_min, channels_max;
  guint rate, rate_min, rate_max;
  guint buffer_size, buffer_size_min, buffer_size_max;

  GError *error;

  pthread_mutex_t *application_mutex;

  window = AGS_WINDOW(AGS_PREFERENCES(gtk_widget_get_ancestor(GTK_WIDGET(sequencer_editor),
							      AGS_TYPE_PREFERENCES))->window);
  sequencer = sequencer_editor->sequencer;

  application_context = (AgsApplicationContext *) window->application_context;

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);
  
  /* get audio loop */
  pthread_mutex_lock(application_mutex);

  main_loop = (AgsThread *) application_context->main_loop;

  pthread_mutex_unlock(application_mutex);

  /* get task and sequencer thread */
  gui_thread = (AgsGuiThread *) ags_thread_find_type(main_loop,
						       AGS_TYPE_GUI_THREAD);
  
  /*  */
  use_alsa = FALSE;

  str = NULL;
  
  if(AGS_IS_JACK_MIDIIN(sequencer)){
    str = "jack";
  }else if(AGS_IS_MIDIIN(sequencer)){
    if((AGS_MIDIIN_ALSA & (AGS_MIDIIN(sequencer)->flags)) != 0){
      str = "alsa";
    }else if((AGS_MIDIIN_OSS & (AGS_MIDIIN(sequencer)->flags)) != 0){
      str = "oss";
    }
  }

  if(str != NULL &&
     !g_ascii_strncasecmp(str,
			  "alsa",
			  5)){
    use_alsa = TRUE;
  }

  card = gtk_combo_box_text_get_active_text(sequencer_editor->card);
  
  if(card != NULL &&
     use_alsa){
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
  if(card != NULL){
    set_input_device = ags_set_input_device_new(sequencer,
						  card);
    ags_gui_thread_schedule_task(gui_thread,
				 set_input_device);
  }
}

void
ags_sequencer_editor_add_jack_callback(GtkWidget *button,
				       AgsSequencerEditor *sequencer_editor)
{
  AgsWindow *window;
  AgsAddSequencerEditorJack *add_sequencer_editor_jack;

  AgsMutexManager *mutex_manager;
  AgsThread *main_loop;
  AgsGuiThread *gui_thread;

  AgsApplicationContext *application_context;

  pthread_mutex_t *application_mutex;  

  window = AGS_WINDOW(AGS_PREFERENCES(gtk_widget_get_ancestor(GTK_WIDGET(sequencer_editor),
							      AGS_TYPE_PREFERENCES))->window);
  application_context = (AgsApplicationContext *) window->application_context;

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);
  
  /* get audio loop */
  pthread_mutex_lock(application_mutex);

  main_loop = (AgsThread *) application_context->main_loop;

  pthread_mutex_unlock(application_mutex);

  /* get task and sequencer thread */
  gui_thread = (AgsGuiThread *) ags_thread_find_type(main_loop,
						       AGS_TYPE_GUI_THREAD);

  /* create set input device task */
  add_sequencer_editor_jack = ags_add_sequencer_editor_jack_new(sequencer_editor);

  /* append AgsSetAudioChannels */
  ags_gui_thread_schedule_task(gui_thread,
			       add_sequencer_editor_jack);
}

void
ags_sequencer_editor_remove_jack_callback(GtkWidget *button,
					  AgsSequencerEditor *sequencer_editor)
{
  AgsWindow *window;
  AgsRemoveSequencerEditorJack *remove_sequencer_editor_jack;

  AgsMutexManager *mutex_manager;
  AgsThread *main_loop;
  AgsGuiThread *gui_thread;

  AgsApplicationContext *application_context;

  pthread_mutex_t *application_mutex;  

  window = AGS_WINDOW(AGS_PREFERENCES(gtk_widget_get_ancestor(GTK_WIDGET(sequencer_editor),
							      AGS_TYPE_PREFERENCES))->window);
  application_context = (AgsApplicationContext *) window->application_context;

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);
  
  /* get audio loop */
  pthread_mutex_lock(application_mutex);

  main_loop = (AgsThread *) application_context->main_loop;

  pthread_mutex_unlock(application_mutex);

  /* get task and sequencer thread */
  gui_thread = (AgsGuiThread *) ags_thread_find_type(main_loop,
						       AGS_TYPE_GUI_THREAD);

  /* create set input device task */
  remove_sequencer_editor_jack = ags_remove_sequencer_editor_jack_new(sequencer_editor,
								      gtk_combo_box_text_get_active_text(sequencer_editor->card));

  /* append AgsSetAudioChannels */
  ags_gui_thread_schedule_task(gui_thread,
			       remove_sequencer_editor_jack);
}
