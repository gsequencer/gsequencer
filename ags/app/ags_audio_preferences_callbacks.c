/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2026 Joël Krähemann
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

#include <ags/app/ags_ui_provider.h>
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
  //empty
}

void
ags_audio_preferences_add_output_soundcard_callback(GAction *action, GVariant *parameter,
						    AgsAudioPreferences *audio_preferences)
{
  AgsSoundcardEditor *soundcard_editor;

  AgsThread *main_loop;
  
  AgsApplicationContext *application_context;

  GObject *soundcard;

  GList *start_list, *list;

  application_context = ags_application_context_get_instance();

  main_loop = ags_concurrency_provider_get_main_loop(AGS_CONCURRENCY_PROVIDER(application_context));

  /* soundcard */
  soundcard = NULL;
  
  /* soundcard editor */
  soundcard_editor = ags_soundcard_editor_new();

  if(soundcard != NULL){
    soundcard_editor->soundcard = soundcard;
    soundcard_editor->soundcard_thread = (GObject *) ags_thread_find_type(main_loop,
									  AGS_TYPE_SOUNDCARD_THREAD);
   }
  
  ags_audio_preferences_add_soundcard_editor(audio_preferences,
					     soundcard_editor);
  
  ags_applicable_reset(AGS_APPLICABLE(soundcard_editor));
  ags_connectable_connect(AGS_CONNECTABLE(soundcard_editor));

  g_signal_connect(soundcard_editor->remove, "clicked",
		   G_CALLBACK(ags_audio_preferences_remove_soundcard_editor_callback), audio_preferences);

  gtk_widget_show((GtkWidget *) soundcard_editor);

  /* reset default card */  
  g_object_unref(main_loop);  
}

void
ags_audio_preferences_add_input_soundcard_callback(GAction *action, GVariant *parameter,
						   AgsAudioPreferences *audio_preferences)
{
  AgsSoundcardEditor *soundcard_editor;

  AgsThread *main_loop;
  
  AgsApplicationContext *application_context;

  GObject *soundcard;

  GList *start_list, *list;

  application_context = ags_application_context_get_instance();

  main_loop = ags_concurrency_provider_get_main_loop(AGS_CONCURRENCY_PROVIDER(application_context));

  /* soundcard */
  soundcard = NULL;
  
  /* soundcard editor */
  soundcard_editor = ags_soundcard_editor_new();

  if(soundcard != NULL){
    soundcard_editor->soundcard = soundcard;
    soundcard_editor->soundcard_thread = (GObject *) ags_thread_find_type(main_loop,
									  AGS_TYPE_SOUNDCARD_THREAD);
   }
  
  ags_audio_preferences_add_soundcard_editor(audio_preferences,
					     soundcard_editor);
  
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

  GList *start_list;
  
  soundcard_editor = (AgsSoundcardEditor *) gtk_widget_get_ancestor(button,
								    AGS_TYPE_SOUNDCARD_EDITOR);

  if(!AGS_IS_JACK_DEVOUT(soundcard_editor->soundcard)){
    ags_soundcard_editor_remove_soundcard(soundcard_editor,
					  soundcard_editor->soundcard);
  }

  ags_audio_preferences_remove_soundcard_editor(audio_preferences,
						soundcard_editor);
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
