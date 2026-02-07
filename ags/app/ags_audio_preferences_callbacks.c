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

  AgsPulseServer *pulse_server;
  AgsPulseDevout *pulse_devout;

  AgsJackServer *jack_server;
  AgsJackDevout *jack_devout;
  
  AgsCoreAudioServer *core_audio_server;
  AgsCoreAudioDevout *core_audio_devout;
  
  AgsAlsaDevout *alsa_devout;

  AgsOssDevout *oss_devout;
  
  AgsWasapiDevout *wasapi_devout;
  
  AgsThread *main_loop;
  AgsThread *soundcard_thread, *default_soundcard_thread;
  AgsThread *export_thread;
  
  AgsApplicationContext *application_context;

  GObject *soundcard;

  GType server_type;
 
  GList *start_sound_server, *sound_server;
  GList *start_list, *list;
  GList *start_card_id, *card_id;
  GList *start_card_name, *card_name;
  
  gchar *backend;
  gchar *tmp;
  
  guint pcm_channels;
  guint buffer_size;
  AgsSoundcardFormat format;
  guint samplerate;
  gboolean use_alsa, use_oss, use_core_audio, use_pulse, use_jack, use_wasapi;
  gboolean is_output;
  gboolean initial_soundcard;
  
  application_context = ags_application_context_get_instance();

  main_loop = ags_concurrency_provider_get_main_loop(AGS_CONCURRENCY_PROVIDER(application_context));

  core_audio_server = NULL;
  core_audio_devout = NULL;
  
  pulse_server = NULL;
  pulse_devout = NULL;
  
  jack_server = NULL;
  jack_devout = NULL;

  alsa_devout = NULL;

  oss_devout = NULL;
  
  /* soundcard */
  soundcard = NULL;

  server_type = G_TYPE_NONE;
  
  backend = NULL;

  use_alsa = FALSE;
  use_oss = FALSE;
  use_core_audio = FALSE;
  use_pulse = FALSE;
  use_jack = FALSE;
  use_wasapi = FALSE;
  
  is_output = TRUE;
  
  initial_soundcard = FALSE;
  
  /* soundcard editor */
  soundcard_editor = ags_soundcard_editor_new();

  gtk_combo_box_set_active(soundcard_editor->capability,
			   0);
  
  /* determine backend */
  backend = gtk_combo_box_text_get_active_text(soundcard_editor->backend);

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
    }else if(!g_ascii_strncasecmp(backend,
				  "wasapi",
				  7)){
      use_wasapi = TRUE;
    }
  }
  
  /* presets */
  pcm_channels = gtk_spin_button_get_value_as_int(soundcard_editor->audio_channels);
  samplerate = gtk_spin_button_get_value_as_int(soundcard_editor->samplerate);
  buffer_size = gtk_spin_button_get_value_as_int(soundcard_editor->buffer_size);
  format = AGS_SOUNDCARD_DEFAULT_FORMAT;

  switch(gtk_combo_box_get_active(GTK_COMBO_BOX(soundcard_editor->format))){
  case 0:
    {
      format = AGS_SOUNDCARD_SIGNED_8_BIT;
    }
    break;
  case 1:
    {
      format = AGS_SOUNDCARD_SIGNED_16_BIT;
    }
    break;
  case 2:
    {
      format = AGS_SOUNDCARD_SIGNED_24_BIT;
    }
    break;
  case 3:
    {
      format = AGS_SOUNDCARD_SIGNED_32_BIT;
    }
    break;
  case 4:
    {
      format = AGS_SOUNDCARD_SIGNED_64_BIT;
    }
    break;
  case 5:
    {
      format = AGS_SOUNDCARD_FLOAT;
    }
    break;
  case 6:
    {
      format = AGS_SOUNDCARD_DOUBLE;
    }
    break;
  }

  /* create soundcard */
  sound_server =
    start_sound_server = ags_sound_provider_get_sound_server(AGS_SOUND_PROVIDER(application_context));

  if((sound_server = ags_list_util_find_type(start_sound_server,
					     server_type)) != NULL){
    if(use_core_audio){
      GValue *param_value = g_new0(GValue,
				   4);
	
      gchar **param_strv = (gchar **) g_malloc(5 * sizeof(gchar *));

      param_strv[0] = g_strdup("pcm-channels");
      param_strv[1] = g_strdup("buffer-size");
      param_strv[2] = g_strdup("format");
      param_strv[3] = g_strdup("samplerate");
      param_strv[4] = NULL;

      g_value_init(param_value, G_TYPE_UINT);
      g_value_set_uint(param_value,
		       pcm_channels);
	
      g_value_init(param_value + 1, G_TYPE_UINT);
      g_value_set_uint(param_value + 1,
		       buffer_size);

      g_value_init(param_value + 2, G_TYPE_UINT);
      g_value_set_uint(param_value + 2,
		       format);

      g_value_init(param_value + 3, G_TYPE_UINT);
      g_value_set_uint(param_value + 3,
		       samplerate);
      
      core_audio_server = AGS_CORE_AUDIO_SERVER(sound_server->data);

      if(is_output){
	core_audio_devout = (AgsCoreAudioDevout *) ags_sound_server_register_soundcard_with_params(AGS_SOUND_SERVER(core_audio_server),
												   is_output,
												   (gchar **) param_strv, param_value);
	soundcard = (GObject *) core_audio_devout;
      }

      g_strfreev(param_strv);
      g_free(param_value);
    }else if(use_pulse){
      pulse_server = AGS_PULSE_SERVER(sound_server->data);

      pulse_devout = (AgsPulseDevout *) ags_sound_server_register_soundcard(AGS_SOUND_SERVER(pulse_server),
									    is_output);
      soundcard = (GObject *) pulse_devout;
    }else if(use_jack){
      jack_server = AGS_JACK_SERVER(sound_server->data);

      jack_devout = (AgsJackDevout *) ags_sound_server_register_soundcard(AGS_SOUND_SERVER(jack_server),
									  is_output);
      soundcard = (GObject *) jack_devout;
    }else{
      //TODO:JK: implement me
    }
  }else{
    if(use_alsa){
      alsa_devout = ags_alsa_devout_new();

      soundcard = (GObject *) alsa_devout;      
    }else if(use_oss){
      oss_devout = ags_oss_devout_new();

      soundcard = (GObject *) oss_devout;
    }else if(use_wasapi){
      wasapi_devout = ags_wasapi_devout_new();

      soundcard = (GObject *) wasapi_devout;
    }else{
      //TODO:JK: implement me
    }
  }
  
  if(soundcard != NULL){
    soundcard_editor->soundcard = soundcard;
    soundcard_editor->soundcard_thread = (GObject *) ags_thread_find_type(main_loop,
									  AGS_TYPE_SOUNDCARD_THREAD);

    if((start_list = ags_sound_provider_get_soundcard(AGS_SOUND_PROVIDER(application_context))) == NULL){
      initial_soundcard = TRUE;
    }

    g_object_ref(soundcard);
    start_list = g_list_append(start_list,
			       soundcard);
    
    ags_sound_provider_set_soundcard(AGS_SOUND_PROVIDER(application_context),
				     start_list);

    g_list_foreach(start_list,
		   (GFunc) g_object_unref,
		   NULL);

    soundcard_thread = (AgsThread *) ags_soundcard_thread_new(soundcard,
							      ags_soundcard_get_capability(AGS_SOUNDCARD(soundcard)));
    soundcard_editor->soundcard_thread = (GObject *) soundcard_thread;
  
    ags_thread_add_child_extended(main_loop,
				  soundcard_thread,
				  TRUE, TRUE);

    if((default_soundcard_thread = (AgsThread *) ags_sound_provider_get_default_soundcard_thread(AGS_SOUND_PROVIDER(application_context))) == NULL){
      ags_sound_provider_set_default_soundcard_thread(AGS_SOUND_PROVIDER(application_context),
						      (GObject *) soundcard_thread);
    }else{
      g_object_unref(default_soundcard_thread);
    }

    /* export thread */
    export_thread = (AgsThread *) ags_export_thread_new(soundcard,
							NULL);
    ags_thread_add_child_extended(main_loop,
				  (AgsThread *) export_thread,
				  TRUE, TRUE);
  }
  
  ags_audio_preferences_add_soundcard_editor(audio_preferences,
					     soundcard_editor);

  /*  */
  start_card_id = NULL;
  start_card_name = NULL;
  
  ags_soundcard_list_cards(AGS_SOUNDCARD(soundcard),
			   &start_card_id, &start_card_name);

  gtk_list_store_clear(GTK_LIST_STORE(gtk_combo_box_get_model(GTK_COMBO_BOX(soundcard_editor->card))));

  card_id = start_card_id;
  card_name = start_card_name;
  
  while(card_id != NULL){
#ifdef AGS_WITH_CORE_AUDIO
    tmp = card_name->data;
#else
    tmp = card_id->data;
#endif
    
    if(tmp != NULL){
      gtk_combo_box_text_append_text(soundcard_editor->card,
				     tmp);
    }
    
    card_id = card_id->next;
    card_name = card_name->next;
  }

  /* unref */
  g_object_unref(main_loop);
  
  g_list_free_full(start_card_id,
		   (GDestroyNotify) g_free);
  
  g_list_free_full(start_card_name,
		   (GDestroyNotify) g_free);
  
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

  AgsPulseServer *pulse_server;
  AgsPulseDevin *pulse_devin;

  AgsJackServer *jack_server;
  AgsJackDevin *jack_devin;
  
  AgsCoreAudioServer *core_audio_server;
  AgsCoreAudioDevin *core_audio_devin;

  AgsAlsaDevin *alsa_devin;

  AgsOssDevin *oss_devin;
  
  AgsWasapiDevin *wasapi_devin;
  
  AgsThread *main_loop;
  AgsThread *soundcard_thread, *default_soundcard_thread;
  AgsThread *export_thread;
  
  AgsApplicationContext *application_context;

  GObject *soundcard;

  GType server_type;
  
  GList *start_sound_server, *sound_server;
  GList *start_list, *list;
  GList *start_card_id, *card_id;
  GList *start_card_name, *card_name;
  
  gchar *backend;
  gchar *tmp;
  
  guint pcm_channels;
  guint buffer_size;
  AgsSoundcardFormat format;
  guint samplerate;
  gboolean use_alsa, use_oss, use_core_audio, use_pulse, use_jack, use_wasapi;
  gboolean is_output;
  gboolean initial_soundcard;
  
  application_context = ags_application_context_get_instance();

  main_loop = ags_concurrency_provider_get_main_loop(AGS_CONCURRENCY_PROVIDER(application_context));

  core_audio_server = NULL;
  core_audio_devin = NULL;
  
  pulse_server = NULL;
  pulse_devin = NULL;
  
  jack_server = NULL;
  jack_devin = NULL;

  alsa_devin = NULL;

  oss_devin = NULL;

  /* soundcard */
  soundcard = NULL;

  server_type = G_TYPE_NONE;
  
  backend = NULL;

  use_alsa = FALSE;
  use_oss = FALSE;
  use_core_audio = FALSE;
  use_pulse = FALSE;
  use_jack = FALSE;
  use_wasapi = FALSE;

  is_output = FALSE;
  
  initial_soundcard = FALSE;
  
  /* soundcard editor */
  soundcard_editor = ags_soundcard_editor_new();

  gtk_combo_box_set_active(soundcard_editor->capability,
			   1);

  /* determine backend */
  backend = gtk_combo_box_text_get_active_text(soundcard_editor->backend);

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
    }else if(!g_ascii_strncasecmp(backend,
				  "wasapi",
				  7)){
      use_wasapi = TRUE;
    }
  }
  
  /* presets */
  pcm_channels = gtk_spin_button_get_value_as_int(soundcard_editor->audio_channels);
  samplerate = gtk_spin_button_get_value_as_int(soundcard_editor->samplerate);
  buffer_size = gtk_spin_button_get_value_as_int(soundcard_editor->buffer_size);
  format = AGS_SOUNDCARD_DEFAULT_FORMAT;

  switch(gtk_combo_box_get_active(GTK_COMBO_BOX(soundcard_editor->format))){
  case 0:
    {
      format = AGS_SOUNDCARD_SIGNED_8_BIT;
    }
    break;
  case 1:
    {
      format = AGS_SOUNDCARD_SIGNED_16_BIT;
    }
    break;
  case 2:
    {
      format = AGS_SOUNDCARD_SIGNED_24_BIT;
    }
    break;
  case 3:
    {
      format = AGS_SOUNDCARD_SIGNED_32_BIT;
    }
    break;
  case 4:
    {
      format = AGS_SOUNDCARD_SIGNED_64_BIT;
    }
    break;
  case 5:
    {
      format = AGS_SOUNDCARD_FLOAT;
    }
    break;
  case 6:
    {
      format = AGS_SOUNDCARD_DOUBLE;
    }
    break;
  }
  
  /* create soundcard */
  sound_server =
    start_sound_server = ags_sound_provider_get_sound_server(AGS_SOUND_PROVIDER(application_context));

  if((sound_server = ags_list_util_find_type(start_sound_server,
					     server_type)) != NULL){
    if(use_core_audio){
      GValue *param_value = g_new0(GValue,
				   4);
	
      gchar **param_strv = (gchar **) g_malloc(5 * sizeof(gchar *));

      param_strv[0] = g_strdup("pcm-channels");
      param_strv[1] = g_strdup("buffer-size");
      param_strv[2] = g_strdup("format");
      param_strv[3] = g_strdup("samplerate");
      param_strv[4] = NULL;

      g_value_init(param_value, G_TYPE_UINT);
      g_value_set_uint(param_value,
		       pcm_channels);
	
      g_value_init(param_value + 1, G_TYPE_UINT);
      g_value_set_uint(param_value + 1,
		       buffer_size);

      g_value_init(param_value + 2, G_TYPE_UINT);
      g_value_set_uint(param_value + 2,
		       format);

      g_value_init(param_value + 3, G_TYPE_UINT);
      g_value_set_uint(param_value + 3,
		       samplerate);
      
      core_audio_server = AGS_CORE_AUDIO_SERVER(sound_server->data);

      if(!is_output){
	core_audio_devin = (AgsCoreAudioDevin *) ags_sound_server_register_soundcard_with_params(AGS_SOUND_SERVER(core_audio_server),
												 is_output,
												 (gchar **) param_strv, param_value);
	soundcard = (GObject *) core_audio_devin;
      }

      g_strfreev(param_strv);
      g_free(param_value);
    }else if(use_pulse){
      pulse_server = AGS_PULSE_SERVER(sound_server->data);

      pulse_devin = (AgsPulseDevin *) ags_sound_server_register_soundcard(AGS_SOUND_SERVER(pulse_server),
									  is_output);
      soundcard = (GObject *) pulse_devin;
    }else if(use_jack){
      jack_server = AGS_JACK_SERVER(sound_server->data);

      jack_devin = (AgsJackDevin *) ags_sound_server_register_soundcard(AGS_SOUND_SERVER(jack_server),
									is_output);
      soundcard = (GObject *) jack_devin;
    }else{
      //TODO:JK: implement me
    }
  }else{
    if(use_alsa){
      alsa_devin = ags_alsa_devin_new();

      soundcard = (GObject *) alsa_devin;      
    }else if(use_oss){
      oss_devin = ags_oss_devin_new();

      soundcard = (GObject *) oss_devin;
    }else if(use_wasapi){
      wasapi_devin = ags_wasapi_devin_new();

      soundcard = (GObject *) wasapi_devin;
    }else{
      //TODO:JK: implement me
    }
  }
  
  if(soundcard != NULL){
    soundcard_editor->soundcard = soundcard;
    soundcard_editor->soundcard_thread = (GObject *) ags_thread_find_type(main_loop,
									  AGS_TYPE_SOUNDCARD_THREAD);

    if((start_list = ags_sound_provider_get_soundcard(AGS_SOUND_PROVIDER(application_context))) == NULL){
      initial_soundcard = TRUE;
    }

    g_object_ref(soundcard);
    start_list = g_list_append(start_list,
			       soundcard);
    
    ags_sound_provider_set_soundcard(AGS_SOUND_PROVIDER(application_context),
				     start_list);

    g_list_foreach(start_list,
		   (GFunc) g_object_unref,
		   NULL);

    soundcard_thread = (AgsThread *) ags_soundcard_thread_new(soundcard,
							      ags_soundcard_get_capability(AGS_SOUNDCARD(soundcard)));
    soundcard_editor->soundcard_thread = (GObject *) soundcard_thread;
  
    ags_thread_add_child_extended(main_loop,
				  soundcard_thread,
				  TRUE, TRUE);

    if((default_soundcard_thread = (AgsThread *) ags_sound_provider_get_default_soundcard_thread(AGS_SOUND_PROVIDER(application_context))) == NULL){
      ags_sound_provider_set_default_soundcard_thread(AGS_SOUND_PROVIDER(application_context),
						      (GObject *) soundcard_thread);
    }else{
      g_object_unref(default_soundcard_thread);
    }

    /* export thread */
    export_thread = (AgsThread *) ags_export_thread_new(soundcard,
							NULL);
    ags_thread_add_child_extended(main_loop,
				  (AgsThread *) export_thread,
				  TRUE, TRUE);
  }
  
  ags_audio_preferences_add_soundcard_editor(audio_preferences,
					     soundcard_editor);
  
  /*  */
  start_card_id = NULL;
  start_card_name = NULL;
  
  ags_soundcard_list_cards(AGS_SOUNDCARD(soundcard),
			   &start_card_id, &start_card_name);

  gtk_list_store_clear(GTK_LIST_STORE(gtk_combo_box_get_model(GTK_COMBO_BOX(soundcard_editor->card))));

  card_id = start_card_id;
  card_name = start_card_name;
  
  while(card_id != NULL){
#ifdef AGS_WITH_CORE_AUDIO
    tmp = card_name->data;
#else
    tmp = card_id->data;
#endif
    
    if(tmp != NULL){
      gtk_combo_box_text_append_text(soundcard_editor->card,
				     tmp);
    }
    
    card_id = card_id->next;
    card_name = card_name->next;
  }

  ags_applicable_reset(AGS_APPLICABLE(soundcard_editor));
  ags_connectable_connect(AGS_CONNECTABLE(soundcard_editor));

  g_signal_connect(soundcard_editor->remove, "clicked",
		   G_CALLBACK(ags_audio_preferences_remove_soundcard_editor_callback), audio_preferences);

  gtk_widget_show((GtkWidget *) soundcard_editor);

  /* unref */
  g_list_free_full(start_card_id,
		   (GDestroyNotify) g_free);
  
  g_list_free_full(start_card_name,
		   (GDestroyNotify) g_free);
  
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
