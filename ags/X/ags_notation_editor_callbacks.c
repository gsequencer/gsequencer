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

#include <ags/X/ags_notation_editor_callbacks.h>

void
ags_notation_editor_machine_changed_callback(AgsMachineSelector *machine_selector, AgsMachine *machine,
					     AgsNotationEditor *notation_editor)
{
  ags_notation_editor_machine_changed(notation_editor, machine);
}

void
ags_notation_editor_resize_pads_callback(AgsMachine *machine, GType channel_type,
					 guint pads, guint pads_old,
					 AgsNotationEditor *notation_editor)
{
  AgsAudio *audio;
  
  AgsMutexManager *mutex_manager;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *audio_mutex;

  audio = machine->audio;
  
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  /* get audio mutex */
  pthread_mutex_lock(application_mutex);
  
  audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					 (GObject *) audio);
  
  pthread_mutex_unlock(application_mutex);

  /* verify pads */
  pthread_mutex_lock(audio_mutex);
  
  if((AGS_AUDIO_NOTATION_DEFAULT & (audio->flags)) != 0){
    if(!g_type_is_a(channel_type,
		    AGS_TYPE_INPUT)){
      pthread_mutex_unlock(audio_mutex);
      
      return;
    }    
  }else{
    if(!g_type_is_a(channel_type,
		    AGS_TYPE_OUTPUT)){
      pthread_mutex_unlock(audio_mutex);
      
      return;
    }
  }
  
  pthread_mutex_unlock(audio_mutex);

  /*  */
  g_object_set(notation_editor->scrolled_piano->piano,
	       "key-count", pads,
	       NULL);
  gtk_widget_queue_draw(notation_editor->scrolled_piano->piano);

  /*  */
  gtk_widget_queue_draw(notation_editor->notation_edit);
}


void
ags_notation_editor_init_channel_launch_callback(AgsTask *task, AgsNote *note)
{
  GObject *soundcard;
  AgsChannel *channel;
  AgsRecycling *recycling;

  AgsAddAudioSignal *add_audio_signal;

  AgsThread *main_loop;

  AgsApplicationContext *application_context;
  AgsConfig *config;
  
  GList *recall, *tmp;
  GList *delay_audio;
  
  gchar *str;
  gdouble notation_delay;
  guint samplerate;
  
  GValue value = {0,};
  
  channel = AGS_INIT_CHANNEL(task)->channel;

  soundcard = channel->soundcard;
  application_context = ags_soundcard_get_application_context(AGS_SOUNDCARD(soundcard));
  ags_soundcard_get_presets(AGS_SOUNDCARD(soundcard),
			    NULL,
			    &samplerate,
			    NULL,
			    NULL);
  
  main_loop = (AgsThread *) application_context->main_loop;

#ifdef AGS_DEBUG
  g_message("launch");
#endif
  
  if(AGS_PLAYBACK(channel->playback) == NULL ||
     AGS_PLAYBACK(channel->playback)->recall_id[0] == NULL){    
    return;
  }

  /* connect done */
  recall = ags_recall_find_provider_with_recycling_context(channel->play,
							   G_OBJECT(channel),
							   G_OBJECT(AGS_PLAYBACK(channel->playback)->recall_id[0]->recycling_context));
  
  tmp = recall;
  recall = ags_recall_find_type(recall,
				AGS_TYPE_PLAY_CHANNEL_RUN);
  //FIXME:JK: below
  //    g_list_free(tmp);

  /* read notation delay */
  delay_audio = channel->recall;
  delay_audio = ags_recall_find_type(delay_audio,
				     AGS_TYPE_DELAY_AUDIO);
  
  if(delay_audio != NULL){
    g_value_init(&value,
		 G_TYPE_DOUBLE);
    ags_port_safe_read(AGS_DELAY_AUDIO(delay_audio->data)->notation_delay,
		       &value);
    notation_delay = g_value_get_double(&value);
  }else{
    notation_delay = 1.0;
  }
  
  if(recall != NULL){
    AgsAudioSignal *audio_signal;
      
    /* add audio signal */
    recycling = channel->first_recycling;

    while(recycling != channel->last_recycling->next){
      audio_signal = ags_audio_signal_new((GObject *) soundcard,
					  (GObject *) recycling,
					  (GObject *) AGS_RECALL(recall->data)->recall_id);
      /* add audio signal */
      ags_recycling_create_audio_signal_with_frame_count(recycling,
							 audio_signal,
							 (note->x[1] - note->x[0]) * ((gdouble) samplerate / notation_delay),
							 0.0, 0);
      audio_signal->stream_current = audio_signal->stream_beginning;
      ags_connectable_connect(AGS_CONNECTABLE(audio_signal));
  
      /*
       * emit add_audio_signal on AgsRecycling
       */
      ags_recycling_add_audio_signal(recycling,
				     audio_signal);

      recycling = recycling->next;
    }    
  }
}
