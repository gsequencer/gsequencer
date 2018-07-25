/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2018 Joël Krähemann
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

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

void
ags_notation_editor_machine_changed_callback(AgsMachineSelector *machine_selector, AgsMachine *machine,
					     AgsNotationEditor *notation_editor)
{
  ags_notation_editor_machine_changed(notation_editor, machine);
}

void
ags_notation_editor_resize_audio_channels_callback(AgsMachine *machine, 
						   guint audio_channels, guint audio_channels_old,
						   AgsNotationEditor *notation_editor)
{
  guint i;
  
  if(audio_channels > audio_channels_old){
    GList *tab;
    
    for(i = audio_channels_old; i < audio_channels; i++){
      ags_notebook_insert_tab(notation_editor->notebook,
			      i);

      tab = notation_editor->notebook->tab;
      gtk_toggle_button_set_active(AGS_NOTEBOOK_TAB(tab->data)->toggle,
				   TRUE);
    }
  }else{
    /* shrink notebook */
    for(i = audio_channels; i < audio_channels_old; i++){
      ags_notebook_remove_tab(notation_editor->notebook,
			      audio_channels);
    }
  }
}

void
ags_notation_editor_resize_pads_callback(AgsMachine *machine, GType channel_type,
					 guint pads, guint pads_old,
					 AgsNotationEditor *notation_editor)
{
  AgsAudio *audio;

  audio = machine->audio;

  /* verify pads */
  if(ags_audio_test_behaviour_flags(audio, AGS_SOUND_BEHAVIOUR_NOTATION_DEFAULT)){
    if(!g_type_is_a(channel_type,
		    AGS_TYPE_INPUT)){
      return;
    }    
  }else{
    if(!g_type_is_a(channel_type,
		    AGS_TYPE_OUTPUT)){
      return;
    }
  }

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
  AgsAudio *audio;
  AgsChannel *channel;
  AgsRecycling *recycling, *last_recycling;
  AgsRecycling *end_recycling;
  AgsAudioSignal *audio_signal;
  AgsPlayback *playback;
  AgsRecallID *recall_id;
  
  GObject *output_soundcard;
  
  gdouble delay;
  guint samplerate;
  
  GValue value = {0,};

  channel = AGS_INIT_CHANNEL(task)->channel;

  /* get some fields */
  g_object_get(channel,
	       "audio", &audio,
	       "output-soundcard", &output_soundcard,
	       "playback", &playback,
	       NULL);

  recall_id = ags_playback_get_recall_id(playback, AGS_SOUND_SCOPE_PLAYBACK);
    
#ifdef AGS_DEBUG
  g_message("launch");
#endif

  if(playback == NULL ||
     recall_id == NULL){
    return;
  }

  /* get presets */
  ags_soundcard_get_presets(AGS_SOUNDCARD(output_soundcard),
			    NULL,
			    &samplerate,
			    NULL,
			    NULL);

  delay = ags_soundcard_get_delay(AGS_SOUNDCARD(output_soundcard));

  /* get some fields */
  g_object_get(channel,
	       "first-recycling", &recycling,
	       "last-recycling", &last_recycling,
	       NULL);

  g_object_get(last_recycling,
	       "next", &end_recycling,
	       NULL);

  /* add audio signal */  
  while(recycling != end_recycling){
    if(!ags_recall_global_get_rt_safe()){
      guint note_x0, note_x1;
      
      audio_signal = ags_audio_signal_new((GObject *) output_soundcard,
					  (GObject *) recycling,
					  (GObject *) recall_id);
      g_object_set(audio_signal,
		   "note", note,
		   NULL);

      /* add audio signal */
      g_object_get(note,
		   "x0", &note_x0,
		   "x1", &note_x1,
		   NULL);

      ags_recycling_create_audio_signal_with_frame_count(recycling,
							 audio_signal,
							 (note_x1 - note_x0) * ((gdouble) samplerate / notation_delay),
							 0.0, 0);
      audio_signal->stream_current = audio_signal->stream_beginning;
      ags_connectable_connect(AGS_CONNECTABLE(audio_signal));
  
      /*
       * emit add_audio_signal on AgsRecycling
       */
      ags_recycling_add_audio_signal(recycling,
				     audio_signal);
    }else{
      GList *start_list, *list;

      g_object_get(recycling,
		   "audio-signal", &start_list,
		   NULL);
      
      audio_signal = NULL;
      list = ags_audio_signal_get_by_recall_id(start_list,
					       recall_id);
	    
      if(list != NULL){
	audio_signal = list->data;

	g_object_set(audio_signal,
		     "note", note,
		     NULL);
      }

      g_list_free(start_list);

      g_object_set(note,
		   "rt-offset", 0,
		   NULL);
    }

    g_object_get(recycling,
		 "next", &recycling,
		 NULL);
  }
}
