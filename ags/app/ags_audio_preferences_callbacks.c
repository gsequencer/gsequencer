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

#include <ags/app/ags_audio_preferences_callbacks.h>

#include <ags/app/ags_window.h>
#include <ags/app/ags_preferences.h>
#include <ags/app/ags_soundcard_editor.h>

#include <ags/config.h>
#include <ags/i18n.h>

void
ags_audio_preferences_notify_parent_callback(GObject *gobject,
					     GParamSpec *pspec,
					     gpointer user_data)
{
  AgsAudioPreferences *audio_preferences;

  audio_preferences = (AgsAudioPreferences *) gobject;
  
  if(audio_preferences->add == NULL) {
    AgsPreferences *preferences;

    preferences = (AgsPreferences *) gtk_widget_get_ancestor(GTK_WIDGET(audio_preferences),
							     AGS_TYPE_PREFERENCES);
    
    audio_preferences->add = (GtkButton *) gtk_button_new_from_icon_name("list-add");
    gtk_dialog_add_action_widget((GtkDialog *) preferences,
				 (GtkWidget *) audio_preferences->add,
				 GTK_RESPONSE_NONE);
  }
}

void
ags_audio_preferences_add_callback(GtkWidget *widget, AgsAudioPreferences *audio_preferences)
{
  AgsSoundcardEditor *soundcard_editor;

  AgsThread *main_loop;
  
  AgsApplicationContext *application_context;

  GObject *soundcard;

  GList *start_list, *list;

  application_context = ags_application_context_get_instance();

  main_loop = ags_concurrency_provider_get_main_loop(AGS_CONCURRENCY_PROVIDER(application_context));

  /* retrieve first soundcard */
  soundcard = NULL;

  list =
    start_list = ags_sound_provider_get_soundcard(AGS_SOUND_PROVIDER(application_context));
  
  if(list != NULL){
    soundcard = list->data;
  }

  g_list_free_full(start_list,
		   g_object_unref);
  
  /* soundcard editor */
  soundcard_editor = ags_soundcard_editor_new();

  if(soundcard != NULL){
    soundcard_editor->soundcard = soundcard;
    soundcard_editor->soundcard_thread = (GObject *) ags_thread_find_type(main_loop,
									  AGS_TYPE_SOUNDCARD_THREAD);
   }
  
  if(audio_preferences->soundcard_editor != NULL){
    gtk_widget_set_sensitive((GtkWidget *) soundcard_editor->buffer_size,
			     FALSE);
  }

  audio_preferences->soundcard_editor = g_list_prepend(audio_preferences->soundcard_editor,
						       soundcard_editor);
  
  gtk_box_append((GtkBox *) audio_preferences->soundcard_editor_box,
		 (GtkWidget *) soundcard_editor);
  
  ags_applicable_reset(AGS_APPLICABLE(soundcard_editor));
  ags_connectable_connect(AGS_CONNECTABLE(soundcard_editor));

  g_signal_connect(soundcard_editor->remove, "clicked",
		   G_CALLBACK(ags_audio_preferences_remove_soundcard_editor_callback), audio_preferences);

  gtk_widget_show((GtkWidget *) soundcard_editor);

  /* reset default card */  
  g_object_unref(main_loop);  
}

void
ags_audio_preferences_remove_soundcard_editor_callback(GtkWidget *button,
						       AgsAudioPreferences *audio_preferences)
{
  AgsSoundcardEditor *soundcard_editor;

  GList *start_list, *list;

  soundcard_editor = (AgsSoundcardEditor *) gtk_widget_get_ancestor(button,
								    AGS_TYPE_SOUNDCARD_EDITOR);

  if(!AGS_IS_JACK_DEVOUT(soundcard_editor->soundcard)){
    ags_soundcard_editor_remove_soundcard(soundcard_editor,
					  soundcard_editor->soundcard);
  }

  ags_audio_preferences_remove_soundcard_editor(audio_preferences,
						soundcard_editor);
  
  /* reset default card */
  if(audio_preferences->soundcard_editor != NULL){
    gtk_widget_set_sensitive((GtkWidget *) AGS_SOUNDCARD_EDITOR(audio_preferences->soundcard_editor->data)->buffer_size,
			     TRUE);
  }
}

void
ags_audio_preferences_start_jack_callback(GtkButton *button,
					  AgsAudioPreferences *audio_preferences)
{
  //TODO:JK: implement me
}

void
ags_audio_preferences_stop_jack_callback(GtkButton *button,
					 AgsAudioPreferences *audio_preferences)
{
  //TODO:JK: implement me
}
