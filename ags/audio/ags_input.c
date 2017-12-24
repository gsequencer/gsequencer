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

#include <ags/audio/ags_input.h>

#include <ags/object/ags_application_context.h>
#include <ags/object/ags_connectable.h>

#include <ags/thread/ags_mutex_manager.h>

#include <ags/file/ags_file_link.h>

#include <ags/audio/ags_playable.h>
#include <ags/audio/ags_recycling.h>
#include <ags/audio/ags_audio_signal.h>
#include <ags/audio/ags_synth_generator.h>

#include <ags/audio/file/ags_audio_file_link.h>
#include <ags/audio/file/ags_audio_file.h>
#include <ags/audio/file/ags_ipatch.h>

#include <ags/i18n.h>

#include <math.h>

void ags_input_class_init (AgsInputClass *input_class);
void ags_input_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_input_set_property(GObject *gobject,
			    guint prop_id,
			    const GValue *value,
			    GParamSpec *param_spec);
void ags_input_get_property(GObject *gobject,
			    guint prop_id,
			    GValue *value,
			    GParamSpec *param_spec);
void ags_input_init (AgsInput *input);
void ags_input_dispose(GObject *gobject);
void ags_input_finalize(GObject *gobject);
void ags_input_connect(AgsConnectable *connectable);
void ags_input_disconnect(AgsConnectable *connectable);

/**
 * SECTION:ags_input
 * @short_description: Input of #AgsAudio
 * @title: AgsInput
 * @section_id:
 * @include: ags/audio/ags_input.h
 *
 * #AgsInput represents an input channel of #AgsAudio.
 */

static gpointer ags_input_parent_class = NULL;
static AgsConnectableInterface *ags_input_parent_connectable_interface;

enum{
  PROP_0,
  PROP_FILE_LINK,
  PROP_SYNTH_GENERATOR,
};

GType
ags_input_get_type (void)
{
  static GType ags_type_input = 0;

  if(!ags_type_input){
    static const GTypeInfo ags_input_info = {
      sizeof (AgsInputClass),
      (GBaseInitFunc) NULL, /* base_init */
      (GBaseFinalizeFunc) NULL, /* base_finalize */
      (GClassInitFunc) ags_input_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsInput),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_input_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_input_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_input = g_type_register_static(AGS_TYPE_CHANNEL,
					    "AgsInput",
					    &ags_input_info,
					    0);

    g_type_add_interface_static(ags_type_input,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return (ags_type_input);
}

void
ags_input_class_init(AgsInputClass *input)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_input_parent_class = g_type_class_peek_parent(input);
  
  gobject = (GObjectClass *) input;

  gobject->set_property = ags_input_set_property;
  gobject->get_property = ags_input_get_property;

  gobject->dispose = ags_input_dispose;
  gobject->finalize = ags_input_finalize;
  
  /* properties */
  /**
   * AgsInput:file-link:
   *
   * The file containing audio data.
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_object("file-link",
				   i18n_pspec("file link assigned to"),
				   i18n_pspec("The file link to read from"),
				   AGS_TYPE_FILE_LINK,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FILE_LINK,
				  param_spec);

  /**
   * AgsInput:synth-generator:
   *
   * An optional synth generator that might be used.
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_object("synth-generator",
				   i18n_pspec("the synth generator"),
				   i18n_pspec("The synth generator to be used"),
				   AGS_TYPE_SYNTH_GENERATOR,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_GENERATOR,
				  param_spec);
}

void
ags_input_connectable_interface_init(AgsConnectableInterface *connectable)
{
  AgsConnectableInterface *ags_input_connectable_parent_interface;

  ags_input_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_input_connect;
  connectable->disconnect = ags_input_disconnect;
}

void
ags_input_init(AgsInput *input)
{
  input->file_link = NULL;
  input->synth_generator = NULL;
}

void
ags_input_set_property(GObject *gobject,
		       guint prop_id,
		       const GValue *value,
		       GParamSpec *param_spec)
{
  AgsInput *input;

  input = AGS_INPUT(gobject);

  switch(prop_id){
  case PROP_FILE_LINK:
    {
      AgsFileLink *file_link;

      file_link = (AgsFileLink *) g_value_get_object(value);

      if(input->file_link == (GObject *) file_link){
	return;
      }
      
      if(input->file_link != NULL){
	g_object_unref(G_OBJECT(input->file_link));
      }

      if(file_link != NULL){
	g_object_ref(G_OBJECT(file_link));
      }

      input->file_link = (GObject *) file_link;
    }
    break;
  case PROP_SYNTH_GENERATOR:
    {
      AgsFileLink *synth_generator;

      synth_generator = (AgsFileLink *) g_value_get_object(value);

      if(input->synth_generator == (GObject *) synth_generator){
	return;
      }
      
      if(input->synth_generator != NULL){
	g_object_unref(G_OBJECT(input->synth_generator));
      }

      if(synth_generator != NULL){
	g_object_ref(G_OBJECT(synth_generator));
      }

      input->synth_generator = (GObject *) synth_generator;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_input_get_property(GObject *gobject,
		       guint prop_id,
		       GValue *value,
		       GParamSpec *param_spec)
{
  AgsInput *input;

  input = AGS_INPUT(gobject);

  switch(prop_id){
  case PROP_FILE_LINK:
    {
      g_value_set_object(value, input->file_link);
    }
    break;
  case PROP_SYNTH_GENERATOR:
    {
      g_value_set_object(value, input->synth_generator);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_input_dispose(GObject *gobject)
{
  AgsInput *input;

  input = AGS_INPUT(gobject);

  /* file link */
  if(input->file_link != NULL){
    g_object_run_dispose(G_OBJECT(input->file_link));

    g_object_unref(G_OBJECT(input->file_link));
    
    input->file_link = NULL;
  }

  /* synth generator */
  if(input->synth_generator != NULL){
    g_object_run_dispose(G_OBJECT(input->synth_generator));

    g_object_unref(G_OBJECT(input->synth_generator));
    
    input->synth_generator = NULL;
  }

  /* finalize */
  G_OBJECT_CLASS(ags_input_parent_class)->dispose(gobject);
}

void
ags_input_finalize(GObject *gobject)
{
  AgsInput *input;

  input = AGS_INPUT(gobject);

  /* file link */
  if(input->file_link != NULL){
    g_object_unref(G_OBJECT(input->file_link));
  }

  /* synth generator */
  if(input->synth_generator != NULL){
    g_object_unref(G_OBJECT(input->synth_generator));
  }

  /* finalize */
  G_OBJECT_CLASS(ags_input_parent_class)->finalize(gobject);
}

void
ags_input_connect(AgsConnectable *connectable)
{
  ags_input_parent_connectable_interface->connect(connectable);
}

void
ags_input_disconnect(AgsConnectable *connectable)
{
  ags_input_parent_connectable_interface->disconnect(connectable);
}

/**
 * ags_input_open_file:
 * @input: the #AgsInput
 * @filename: the filename as string
 * @preset: the preset to open
 * @instrument: the instrument to open
 * @sample: the sample to open
 * @audio_channel: the audio channel to read
 * 
 * Open @filename and assign @audio_channel's data as AGS_AUDIO_SIGNAL_TEMPLATE
 * audio signal to @input's own recycling.
 * 
 * Returns: %TRUE if open was successful, else %FALSE
 * 
 * Since: 1.0.0
 */
gboolean
ags_input_open_file(AgsInput *input,
		    gchar *filename,
		    gchar *preset,
		    gchar *instrument,
		    gchar *sample,
		    guint audio_channel)
{
  AgsFileLink *file_link;
  
  GList *audio_signal;

  gboolean success;

  if(!AGS_IS_INPUT(input)){
    return(FALSE);
  }
  
  audio_signal = NULL;
  success = FALSE;

  if(ags_audio_file_check_suffix(filename)){
    AgsAudioFile *audio_file;

    success = TRUE;

    preset = NULL;
    instrument = NULL;
    
    /* open audio file and read audio signal */
    audio_file = ags_audio_file_new(filename,
				    AGS_CHANNEL(input)->soundcard,
				    audio_channel, 1);
    
    ags_audio_file_open(audio_file);
    ags_audio_file_read_audio_signal(audio_file);

    audio_signal = audio_file->audio_signal;
    //    g_object_unref(audio_file);
  }else if(ags_ipatch_check_suffix(filename)){
    AgsIpatch *ipatch;

    AgsPlayable *playable;

    GError *error;
    
    success = TRUE;
    
    ipatch = g_object_new(AGS_TYPE_IPATCH,
			  "soundcard", AGS_CHANNEL(input)->soundcard,
			  "mode", AGS_IPATCH_READ,
			  "filename", filename,
			  NULL);

    playable = AGS_PLAYABLE(ipatch);
  
    error = NULL;
    ags_playable_level_select(playable,
			      0, filename,
			      &error);

    /* select first - preset */
    ipatch->nth_level = 1;
  
    error = NULL;
    ags_playable_level_select(playable,
			      1, preset,
			      &error);

    if(error != NULL){
      g_object_unref(ipatch);

      return(FALSE);
    }

    /* select second - instrument */
    ipatch->nth_level = 2;

    error = NULL;
    ags_playable_level_select(playable,
			      2, instrument,
			      &error);

    if(error != NULL){
      g_object_unref(ipatch);

      return(FALSE);
    }

    /* select third - sample */
    ipatch->nth_level = 3;

    error = NULL;
    ags_playable_level_select(playable,
			      3, sample,
			      &error);

    if(error != NULL){
      g_object_unref(ipatch);

      return(FALSE);
    }

    /* read audio signal */
    audio_signal = ags_playable_read_audio_signal(playable,
						  AGS_CHANNEL(input)->soundcard,
						  audio_channel, 1);
  }

  if(success){
    if(audio_signal != NULL){
      file_link = g_object_new(AGS_TYPE_AUDIO_FILE_LINK,
			       "filename", filename,
			       "preset", preset,
			       "instrument", instrument,
			       "sample", sample,
			       "audio-channel", audio_channel,
			       NULL);
      g_object_set(input,
		   "file-link", file_link,
		   NULL);
    
      /* mark as template */
      AGS_AUDIO_SIGNAL(audio_signal->data)->flags |= AGS_AUDIO_SIGNAL_TEMPLATE;
    
      /* add as template */
      ags_recycling_add_audio_signal(AGS_CHANNEL(input)->first_recycling,
				     AGS_AUDIO_SIGNAL(audio_signal->data));
    }else{
      success = FALSE;
    }
  }
  
  return(success);
}

/**
 * ags_input_apply_synth:
 * @input: the #AgsInput
 * @oscillator: the oscillator to use
 * @frequency: the frequency to use
 * @phase: the phase to use
 * @volume: the volume to use
 * @n_frames: compute n_frames count of frames
 * 
 * Apply synth using specified parameters to input.
 * 
 * Returns: %TRUE if successful applied, else %FALSE
 * 
 * Since: 1.0.0
 */
gboolean
ags_input_apply_synth(AgsInput *input,
		      guint oscillator,
		      gdouble frequency,
		      gdouble phase,
		      gdouble volume,
		      guint n_frames)
{
  return(ags_input_apply_synth_extended(input,
					oscillator,
					frequency,
					phase,
					volume,
					n_frames,
					0,
					AGS_INPUT_SYNTH_BASE_NOTE,
					NULL, NULL,
					AGS_SYNTH_GENERATOR_COMPUTE_FIXED_LENGTH));
}

/**
 * ags_input_apply_synth_extended:
 * @input: the #AgsInput
 * @oscillator: the oscillator to use
 * @frequency: the frequency to use
 * @phase: the phase to use
 * @volume: the volume to use
 * @n_frames: compute n_frames count of frames
 * @attack: the attack to use
 * @base_note: the base note to ramp up from
 * @sync_start: safe to be %NULL
 * @sync_end: safe to be %NULL
 * @compute_flags: the flags to use to compute the synth
 * 
 * Apply synth using specified parameters to input.
 * 
 * Returns: %TRUE if successful applied, else %FALSE
 * 
 * Since: 1.0.0
 */
gboolean
ags_input_apply_synth_extended(AgsInput *input,
			       guint oscillator,
			       gdouble frequency,
			       gdouble phase,
			       gdouble volume,
			       guint n_frames,
			       guint attack,
			       gdouble base_note,
			       AgsComplex *sync_start, AgsComplex *sync_end,
			       guint compute_flags)
{
  AgsAudioSignal *audio_signal;

  if(!AGS_IS_INPUT(input)){
    return(FALSE);
  }

  audio_signal = ags_audio_signal_get_template(AGS_CHANNEL(input)->first_recycling->audio_signal);

  if(audio_signal == NULL){
    audio_signal = g_object_new(AGS_TYPE_AUDIO_SIGNAL,
				"soundcard", AGS_CHANNEL(input)->soundcard,
				NULL);
    
    /* mark as template */
    audio_signal->flags |= AGS_AUDIO_SIGNAL_TEMPLATE;
      
    ags_audio_signal_stream_resize(audio_signal,
				   (guint) ceil((attack + n_frames) / audio_signal->buffer_size));

    /* add as template */
    ags_recycling_add_audio_signal(AGS_CHANNEL(input)->first_recycling,
				   audio_signal);
  }
  
  /* creat synth generator if needed */
  if(input->synth_generator == NULL){
    input->synth_generator = (GObject *) ags_synth_generator_new();

    g_object_set(input->synth_generator,
		 "samplerate", audio_signal->samplerate,
		 "buffer-size", audio_signal->buffer_size,
		 "format", audio_signal->format,
		 NULL);
  }

  /* set properties */
  g_object_set(input->synth_generator,
	       "n-frames", n_frames,
	       "attack", attack,
	       "oscillator", oscillator,
	       "frequency", frequency,
	       "phase", phase,
	       "volume", volume,
	       NULL);

  /* compute audio signal */
  ags_synth_generator_compute_with_audio_signal(input->synth_generator,
						audio_signal,
						base_note,
						sync_start, sync_end,
						compute_flags);

  return(TRUE);
}

/**
 * ags_input_is_active:
 * @input: the #AgsInput
 * @recycling_context: the #AgsRecyclingContext to check
 * 
 * Check if @input is active and needs processing.
 * 
 * Returns: %TRUE if has a need to be processed, else %FALSE
 * 
 * Since: 1.0.0
 */
gboolean
ags_input_is_active(AgsInput *input,
		    GObject *recycling_context)
{
  AgsChannel *channel;
  AgsRecycling *recycling, *end_region;
  AgsAudioSignal *audio_signal;
  AgsRecyclingContext *active_context;
  
  AgsMutexManager *mutex_manager;

  GList *list;
  
  pthread_mutex_t *application_mutex;
  pthread_mutex_t *input_mutex;
  pthread_mutex_t *recycling_mutex;
  
  channel = input;

  if(input == NULL ||
     channel->first_recycling == NULL ||
     recycling_context == NULL){
    return(FALSE);
  }

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  /* lookup mutex */
  pthread_mutex_lock(application_mutex);
  
  input_mutex = ags_mutex_manager_lookup(mutex_manager,
					 (GObject *) channel);
  
  pthread_mutex_unlock(application_mutex);

  /* get recycling */
  pthread_mutex_lock(input_mutex);

  recycling = channel->first_recycling;
  end_region = channel->last_recycling->next;

  if(channel->link == NULL){
    active_context = AGS_RECYCLING_CONTEXT(recycling_context)->parent;
  }else{
    active_context = recycling_context;
  }
  
  pthread_mutex_unlock(input_mutex);

  /* check if active */
  while(recycling != end_region){
    /* lookup mutex */
    pthread_mutex_lock(application_mutex);
  
    recycling_mutex = ags_mutex_manager_lookup(mutex_manager,
					       (GObject *) recycling);
  
    pthread_mutex_unlock(application_mutex);

    pthread_mutex_lock(recycling_mutex);

    list = recycling->audio_signal;

    while(list != NULL){
      audio_signal = list->data;
      
      if(audio_signal->recall_id != NULL &&
	 AGS_RECALL_ID(audio_signal->recall_id)->recycling_context != NULL &&
	 AGS_RECYCLING_CONTEXT(AGS_RECALL_ID(audio_signal->recall_id)->recycling_context)->parent == active_context){
	pthread_mutex_unlock(recycling_mutex);
      
	return(TRUE);
      }

      list = list->next;
    }

    /* iterate */
    recycling = recycling->next;
    
    pthread_mutex_unlock(recycling_mutex);  
  }
  
  return(FALSE);
}

/**
 * ags_input_next_active:
 * @input: the #AgsInput
 * @recycling_context: the #AgsRecyclingContext to check
 * 
 * Find next #AgsInput needed to be processed.
 * 
 * Returns: next active #AgsInput, else %NULL if non available
 * 
 * Since: 1.0.0
 */
AgsInput*
ags_input_next_active(AgsInput *input,
		      GObject *recycling_context)
{
  AgsChannel *channel;
  AgsRecycling *recycling, *end_region;
  AgsAudioSignal *audio_signal;  
  AgsRecyclingContext *active_context;
  
  AgsMutexManager *mutex_manager;

  GList *list;
  
  pthread_mutex_t *application_mutex;
  pthread_mutex_t *input_mutex;
  pthread_mutex_t *recycling_mutex;

  channel = input;
  
  if(input == NULL ||
     recycling_context == NULL){
    return(NULL);
  }

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  /* check if active */
  while(channel != NULL){
    /* lookup mutex */
    pthread_mutex_lock(application_mutex);
  
    input_mutex = ags_mutex_manager_lookup(mutex_manager,
					   (GObject *) channel);
  
    pthread_mutex_unlock(application_mutex);

    /* get recycling */
    pthread_mutex_lock(input_mutex);

    recycling = channel->first_recycling;
    end_region = channel->last_recycling->next;
  
    if(channel->link == NULL){
      active_context = AGS_RECYCLING_CONTEXT(recycling_context)->parent;
    }else{
      active_context = recycling_context;
    }
    
    pthread_mutex_unlock(input_mutex);
    
    /* check recycling */
    while(recycling != end_region){
      /* lookup mutex */
      pthread_mutex_lock(application_mutex);
  
      recycling_mutex = ags_mutex_manager_lookup(mutex_manager,
						 (GObject *) recycling);
  
      pthread_mutex_unlock(application_mutex);

      /* check audio signal */
      pthread_mutex_lock(recycling_mutex);

      list = recycling->audio_signal;

      while(list != NULL){
	audio_signal = list->data;
      
	if(audio_signal->recall_id != NULL &&
	   AGS_RECALL_ID(audio_signal->recall_id)->recycling_context != NULL &&
	   AGS_RECYCLING_CONTEXT(AGS_RECALL_ID(audio_signal->recall_id)->recycling_context)->parent == active_context){
	  pthread_mutex_unlock(recycling_mutex);
      
	  return(channel);
	}

	list = list->next;
      }

      /* iterate */
      recycling = recycling->next;
    
      pthread_mutex_unlock(recycling_mutex);  
    }

    /* iterate */
    pthread_mutex_lock(input_mutex);

    channel = channel->next_pad;
    
    pthread_mutex_unlock(input_mutex);
  }
  
  return(NULL);
}

/**
 * ags_input_new:
 * @audio: the #AgsAudio
 *
 * Creates a #AgsInput, linking tree to @audio.
 *
 * Returns: a new #AgsInput
 *
 * Since: 1.0.0
 */
AgsInput*
ags_input_new(GObject *audio)
{
  AgsInput *input;

  input = (AgsInput *) g_object_new(AGS_TYPE_INPUT,
				    "audio", audio,
				    NULL);

  return(input);
}
