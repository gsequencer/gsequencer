/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2020 Joël Krähemann
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

#include <ags/audio/fx/ags_fx_playback_audio_signal.h>

#include <ags/audio/ags_audio_buffer_util.h>

#include <ags/audio/fx/ags_fx_playback_audio.h>
#include <ags/audio/fx/ags_fx_playback_audio_processor.h>
#include <ags/audio/fx/ags_fx_playback_channel_processor.h>
#include <ags/audio/fx/ags_fx_playback_recycling.h>

#include <ags/i18n.h>

void ags_fx_playback_audio_signal_class_init(AgsFxPlaybackAudioSignalClass *fx_playback_audio_signal);
void ags_fx_playback_audio_signal_init(AgsFxPlaybackAudioSignal *fx_playback_audio_signal);
void ags_fx_playback_audio_signal_dispose(GObject *gobject);
void ags_fx_playback_audio_signal_finalize(GObject *gobject);

void ags_fx_playback_audio_signal_real_run_inter(AgsRecall *recall);

/**
 * SECTION:ags_fx_playback_audio_signal
 * @short_description: fx playback audio signal
 * @title: AgsFxPlaybackAudioSignal
 * @section_id:
 * @include: ags/audio/fx/ags_fx_playback_audio_signal.h
 *
 * The #AgsFxPlaybackAudioSignal class provides ports to the effect processor.
 */

static gpointer ags_fx_playback_audio_signal_parent_class = NULL;

static const gchar *ags_fx_playback_audio_signal_plugin_name = "ags-fx-playback";

GType
ags_fx_playback_audio_signal_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_fx_playback_audio_signal = 0;

    static const GTypeInfo ags_fx_playback_audio_signal_info = {
      sizeof (AgsFxPlaybackAudioSignalClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_fx_playback_audio_signal_class_init,
      NULL, /* class_finalize */
      NULL, /* class_audio_signal */
      sizeof (AgsFxPlaybackAudioSignal),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_fx_playback_audio_signal_init,
    };

    ags_type_fx_playback_audio_signal = g_type_register_static(AGS_TYPE_RECALL_AUDIO_SIGNAL,
							       "AgsFxPlaybackAudioSignal",
							       &ags_fx_playback_audio_signal_info,
							       0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_fx_playback_audio_signal);
  }

  return g_define_type_id__volatile;
}

void
ags_fx_playback_audio_signal_class_init(AgsFxPlaybackAudioSignalClass *fx_playback_audio_signal)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;

  ags_fx_playback_audio_signal_parent_class = g_type_class_peek_parent(fx_playback_audio_signal);

  /* GObjectClass */
  gobject = (GObjectClass *) fx_playback_audio_signal;

  gobject->dispose = ags_fx_playback_audio_signal_dispose;
  gobject->finalize = ags_fx_playback_audio_signal_finalize;

  /* AgsRecallClass */
  recall = (AgsRecallClass *) fx_playback_audio_signal;
  
  recall->run_inter = ags_fx_playback_audio_signal_real_run_inter;
}

void
ags_fx_playback_audio_signal_init(AgsFxPlaybackAudioSignal *fx_playback_audio_signal)
{
  AGS_RECALL(fx_playback_audio_signal)->name = "ags-fx-playback";
  AGS_RECALL(fx_playback_audio_signal)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(fx_playback_audio_signal)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(fx_playback_audio_signal)->xml_type = "ags-fx-playback-audio-signal";
}

void
ags_fx_playback_audio_signal_dispose(GObject *gobject)
{
  AgsFxPlaybackAudioSignal *fx_playback_audio_signal;
  
  fx_playback_audio_signal = AGS_FX_PLAYBACK_AUDIO_SIGNAL(gobject);
  
  /* call parent */
  G_OBJECT_CLASS(ags_fx_playback_audio_signal_parent_class)->dispose(gobject);
}

void
ags_fx_playback_audio_signal_finalize(GObject *gobject)
{
  AgsFxPlaybackAudioSignal *fx_playback_audio_signal;
  
  fx_playback_audio_signal = AGS_FX_PLAYBACK_AUDIO_SIGNAL(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_fx_playback_audio_signal_parent_class)->finalize(gobject);
}

void
ags_fx_playback_audio_signal_real_run_inter(AgsRecall *recall)
{
  AgsAudioSignal *source;

  GObject *output_soundcard;

  gpointer buffer;
  gpointer audio_signal_data;
  
  guint pcm_channels;
  guint output_soundcard_channel;
  guint samplerate, target_samplerate;
  guint buffer_size, target_buffer_size;
  guint format, target_format;
  guint copy_mode;
  gboolean is_done;
  
  GRecMutex *source_stream_mutex;

  output_soundcard = NULL;

  source = NULL;
  
  g_object_get(recall,
	       "source", &source,
	       NULL);

  if(!ags_audio_signal_test_flags(source, AGS_AUDIO_SIGNAL_MASTER) &&
     !ags_audio_signal_test_flags(source, AGS_AUDIO_SIGNAL_FEED)){
    ags_recall_done(recall);

    g_object_unref(source);
    
    /* call parent */
    AGS_RECALL_CLASS(ags_fx_playback_audio_signal_parent_class)->run_inter(recall);

    return;
  }
  
  g_object_get(recall,
	       "output-soundcard", &output_soundcard,
	       "output-soundcard-channel", &output_soundcard_channel,
	       NULL);

  samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
  buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
  format = AGS_SOUNDCARD_DEFAULT_FORMAT;
  
  g_object_get(source,
	       "samplerate", &samplerate,
	       "buffer-size", &buffer_size,
	       "format", &format,
	       NULL);
  
  /* get presets */
  source_stream_mutex = AGS_AUDIO_SIGNAL_GET_STREAM_MUTEX(source);

  ags_soundcard_get_presets(AGS_SOUNDCARD(output_soundcard),
			    &pcm_channels,
			    &target_samplerate,
			    &target_buffer_size,
			    &target_format);

  buffer = ags_soundcard_get_buffer(AGS_SOUNDCARD(output_soundcard));

  copy_mode = ags_audio_buffer_util_get_copy_mode(ags_audio_buffer_util_format_from_soundcard(target_format),
						  ags_audio_buffer_util_format_from_soundcard(format));
  
  if(buffer != NULL &&
     source != NULL &&
     source->stream_current != NULL){    
#ifdef AGS_DEBUG
    g_message("ags-fx-playback 0x%x", source);
#endif
    
    if(samplerate == target_samplerate){
      /* copy */
      g_rec_mutex_lock(source_stream_mutex);
      
      ags_soundcard_lock_buffer(AGS_SOUNDCARD(output_soundcard),
				buffer);
            
      ags_audio_buffer_util_copy_buffer_to_buffer(buffer, pcm_channels, output_soundcard_channel,
						  source->stream_current->data, 1, 0,
						  target_buffer_size, copy_mode);

      ags_soundcard_unlock_buffer(AGS_SOUNDCARD(output_soundcard),
				  buffer);
      g_rec_mutex_unlock(source_stream_mutex);    
    }else{
      audio_signal_data = ags_stream_alloc(target_buffer_size,
					   format);
      
      g_rec_mutex_lock(source_stream_mutex);

      ags_audio_buffer_util_resample_with_buffer(source->stream_current->data, 1,
						 target_format, target_samplerate,
						 target_buffer_size,
						 samplerate,
						 buffer_size,
						 audio_signal_data);
      
      ags_soundcard_lock_buffer(AGS_SOUNDCARD(output_soundcard),
				buffer);
            
      ags_audio_buffer_util_copy_buffer_to_buffer(buffer, pcm_channels, output_soundcard_channel,
						  audio_signal_data, 1, 0,
						  target_buffer_size, copy_mode);

      ags_soundcard_unlock_buffer(AGS_SOUNDCARD(output_soundcard),
				  buffer);

      g_rec_mutex_unlock(source_stream_mutex);

      ags_stream_free(audio_signal_data);
    }
  }

  g_rec_mutex_lock(source_stream_mutex);

  is_done = (source == NULL || source->stream_current == NULL) ? TRUE: FALSE;
  
  g_rec_mutex_unlock(source_stream_mutex);

  if(is_done){
    if(ags_audio_signal_test_flags(source, AGS_AUDIO_SIGNAL_FEED)){
      AgsFxPlaybackAudio *fx_playback_audio;
      AgsFxPlaybackChannelProcessor *fx_playback_channel_processor;
      AgsFxPlaybackRecycling *fx_playback_recycling;

      fx_playback_audio = NULL;

      fx_playback_channel_processor = NULL;

      fx_playback_recycling = NULL;
      
      g_object_get(recall,
		   "parent", &fx_playback_recycling,
		   NULL);

      g_object_get(fx_playback_recycling,
		   "parent", &fx_playback_channel_processor,
		   NULL);

      g_object_get(fx_playback_channel_processor,
		   "recall-audio", &fx_playback_audio,
		   NULL);

      ags_fx_playback_audio_remove_feed_audio_signal(fx_playback_audio,
						     source);
    }
    
    ags_recall_done(recall);
  }

  if(ags_audio_signal_test_flags(source, AGS_AUDIO_SIGNAL_MASTER)){
    g_rec_mutex_lock(source_stream_mutex);

    ags_audio_buffer_util_clear_buffer(source->stream_current->data, 1,
				       buffer_size, ags_audio_buffer_util_format_from_soundcard(format));
    
    g_rec_mutex_unlock(source_stream_mutex);
  }

  if(ags_audio_signal_test_flags(source, AGS_AUDIO_SIGNAL_STREAM)){
    g_rec_mutex_lock(source_stream_mutex);

    if(source->stream_current != NULL){
      source->stream_current = source->stream_current->next;
    }
    
    g_rec_mutex_unlock(source_stream_mutex);
  }

  /* unref */
  if(output_soundcard != NULL){
    g_object_unref(output_soundcard);
  }

  if(source != NULL){
    g_object_unref(source);
  }
  
  /* call parent */
  AGS_RECALL_CLASS(ags_fx_playback_audio_signal_parent_class)->run_inter(recall);
}

/**
 * ags_fx_playback_audio_signal_new:
 * @audio_signal: the #AgsAudioSignal
 *
 * Create a new instance of #AgsFxPlaybackAudioSignal
 *
 * Returns: the new #AgsFxPlaybackAudioSignal
 *
 * Since: 3.3.0
 */
AgsFxPlaybackAudioSignal*
ags_fx_playback_audio_signal_new(AgsAudioSignal *audio_signal)
{
  AgsFxPlaybackAudioSignal *fx_playback_audio_signal;

  fx_playback_audio_signal = (AgsFxPlaybackAudioSignal *) g_object_new(AGS_TYPE_FX_PLAYBACK_AUDIO_SIGNAL,
								       "source", audio_signal,
								       NULL);

  return(fx_playback_audio_signal);
}
