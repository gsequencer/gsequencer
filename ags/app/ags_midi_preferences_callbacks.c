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

#include <ags/app/ags_midi_preferences_callbacks.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_window.h>
#include <ags/app/ags_preferences.h>
#include <ags/app/ags_sequencer_editor.h>

#include <ags/i18n.h>

#include <ags/config.h>

void
ags_midi_preferences_notify_parent_callback(GObject *gobject,
					    GParamSpec *pspec,
					    gpointer user_data)
{
  //empty
}

void
ags_midi_preferences_add_input_sequencer_callback(GAction *action, GVariant *parameter,
						  AgsMidiPreferences *midi_preferences)
{
  AgsSequencerEditor *sequencer_editor;
  
  AgsPulseServer *pulse_server;

  AgsJackServer *jack_server;
  AgsJackMidiin *jack_midiin;
  
  AgsCoreAudioServer *core_audio_server;
  AgsCoreAudioMidiin *core_audio_midiin;

  AgsAlsaMidiin *alsa_midiin;

  AgsOssMidiin *oss_midiin;
  
  AgsThread *main_loop;
  AgsThread *sequencer_thread, *default_sequencer_thread;
  
  AgsApplicationContext *application_context;

  GObject *sequencer;

  GType server_type;
  
  GList *start_sound_server, *sound_server;
  GList *start_list, *list;
  GList *start_card_id, *card_id;
  GList *start_card_name, *card_name;
  
  gchar *backend;
  gchar *tmp;
  
  gboolean use_alsa, use_oss, use_core_audio, use_pulse, use_jack, use_wasapi;
  gboolean is_output;
  gboolean initial_sequencer;

  application_context = ags_application_context_get_instance();

  main_loop = ags_concurrency_provider_get_main_loop(AGS_CONCURRENCY_PROVIDER(application_context));

  core_audio_server = NULL;
  core_audio_midiin = NULL;
  
  pulse_server = NULL;
  
  jack_server = NULL;
  jack_midiin = NULL;

  alsa_midiin = NULL;

  oss_midiin = NULL;

  /* sequencer */
  sequencer = NULL;

  server_type = G_TYPE_NONE;
  
  backend = NULL;

  use_alsa = FALSE;
  use_oss = FALSE;
  use_core_audio = FALSE;
  use_pulse = FALSE;
  use_jack = FALSE;
  use_wasapi = FALSE;

  is_output = FALSE;
  
  initial_sequencer = FALSE;
  
  /* sequencer editor */
  sequencer_editor = ags_sequencer_editor_new();

  /* determine backend */
  backend = gtk_combo_box_text_get_active_text(sequencer_editor->backend);

  if(backend != NULL){
    if(!g_ascii_strncasecmp(backend,
			    "alsa",
			    5)){
      use_alsa = TRUE;
    }else if(!g_ascii_strncasecmp(backend,
				  "oss",
				  4)){
      use_oss = TRUE;
    }else if(!g_ascii_strncasecmp(backend,
				  "core-midi",
				  10)){
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
    }else if(!g_ascii_strncasecmp(backend,
				  "wasapi",
				  7)){
      use_wasapi = TRUE;
    }
  }

  /* create sequencer */
  sound_server =
    start_sound_server = ags_sound_provider_get_sound_server(AGS_SOUND_PROVIDER(application_context));

  if((sound_server = ags_list_util_find_type(start_sound_server,
					     server_type)) != NULL){
    if(use_core_audio){
      GValue *param_value = NULL;
	
      gchar **param_strv = NULL;
      
      core_audio_server = AGS_CORE_AUDIO_SERVER(sound_server->data);

      core_audio_midiin = (AgsCoreAudioMidiin *) ags_sound_server_register_sequencer_with_params(AGS_SOUND_SERVER(core_audio_server),
												 is_output,
												 (gchar **) param_strv, param_value);
      sequencer = (GObject *) core_audio_midiin;

      g_strfreev(param_strv);
      g_free(param_value);
    }else if(use_pulse){
      pulse_server = AGS_PULSE_SERVER(sound_server->data);
      
      //TODO:JK: implement me
    }else if(use_jack){
      jack_server = AGS_JACK_SERVER(sound_server->data);

      jack_midiin = (AgsJackMidiin *) ags_sound_server_register_sequencer(AGS_SOUND_SERVER(jack_server),
									  is_output);
      sequencer = (GObject *) jack_midiin;
    }else{
      //TODO:JK: implement me
    }
  }else{
    if(use_alsa){
      alsa_midiin = ags_alsa_midiin_new();

      sequencer = (GObject *) alsa_midiin;      
    }else if(use_oss){
      oss_midiin = ags_oss_midiin_new();

      sequencer = (GObject *) oss_midiin;
    }else if(use_wasapi){
      //TODO:JK: implement me
    }else{
      //TODO:JK: implement me
    }
  }
  
  if(sequencer != NULL){
    sequencer_editor->sequencer = sequencer;
    sequencer_editor->sequencer_thread = (GObject *) ags_thread_find_type(main_loop,
									  AGS_TYPE_SEQUENCER_THREAD);

    if((start_list = ags_sound_provider_get_sequencer(AGS_SOUND_PROVIDER(application_context))) == NULL){
      initial_sequencer = TRUE;
    }

    g_object_ref(sequencer);
    start_list = g_list_append(start_list,
			       sequencer);
    
    ags_sound_provider_set_sequencer(AGS_SOUND_PROVIDER(application_context),
				     start_list);

    g_list_foreach(start_list,
		   (GFunc) g_object_unref,
		   NULL);

    sequencer_thread = (AgsThread *) ags_sequencer_thread_new(sequencer);
    sequencer_editor->sequencer_thread = (GObject *) sequencer_thread;
  
    ags_thread_add_child_extended(main_loop,
				  sequencer_thread,
				  TRUE, TRUE);
  }
  
  ags_midi_preferences_add_sequencer_editor(midi_preferences,
					    sequencer_editor);

  /*  */
  start_card_id = NULL;
  start_card_name = NULL;
  
  ags_sequencer_list_cards(AGS_SEQUENCER(sequencer),
			   &start_card_id, &start_card_name);

  gtk_list_store_clear(GTK_LIST_STORE(gtk_combo_box_get_model(GTK_COMBO_BOX(sequencer_editor->card))));

  card_id = start_card_id;
  card_name = start_card_name;
  
  while(card_id != NULL){
#ifdef AGS_WITH_CORE_AUDIO
    tmp = card_name->data;
#else
    tmp = card_id->data;
#endif
    
    if(tmp != NULL){
      gtk_combo_box_text_append_text(sequencer_editor->card,
				     tmp);
    }
    
    card_id = card_id->next;
    card_name = card_name->next;
  }
  
  ags_applicable_reset(AGS_APPLICABLE(sequencer_editor));
  ags_connectable_connect(AGS_CONNECTABLE(sequencer_editor));

  g_signal_connect(sequencer_editor->remove, "clicked",
		   G_CALLBACK(ags_midi_preferences_remove_sequencer_editor_callback), midi_preferences);

  gtk_widget_show((GtkWidget *) sequencer_editor);

  /* unref */
  g_list_free_full(start_card_id,
		   (GDestroyNotify) g_free);
  
  g_list_free_full(start_card_name,
		   (GDestroyNotify) g_free);
  
  /* reset default card */  
  g_object_unref(main_loop);  
}

void
ags_midi_preferences_remove_sequencer_editor_callback(GtkWidget *button,
						      AgsMidiPreferences *midi_preferences)
{
  AgsSequencerEditor *sequencer_editor;
  
  sequencer_editor = (AgsSequencerEditor *) gtk_widget_get_ancestor(button,
								    AGS_TYPE_SEQUENCER_EDITOR);

  ags_midi_preferences_remove_sequencer_editor(midi_preferences,
					       sequencer_editor);
}
