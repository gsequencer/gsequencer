/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
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

#include <ags/audio/ags_recycling.h>
#include <ags/audio/ags_audio_signal.h>
#include <ags/audio/ags_synth_generator.h>

#include <ags/audio/file/ags_audio_file_link.h>
#include <ags/audio/file/ags_audio_container.h>
#include <ags/audio/file/ags_audio_file.h>
#include <ags/audio/file/ags_sound_container.h>
#include <ags/audio/file/ags_sound_resource.h>
#include <ags/audio/file/ags_ipatch.h>

#include <ags/i18n.h>

#include <math.h>

void ags_input_class_init (AgsInputClass *input_class);
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

void ags_input_notify_samplerate_callback(GObject *gobject,
					  GParamSpec *pspec,
					  gpointer user_data);
void ags_input_notify_buffer_size_callback(GObject *gobject,
					   GParamSpec *pspec,
					   gpointer user_data);
void ags_input_notify_format_callback(GObject *gobject,
				      GParamSpec *pspec,
				      gpointer user_data);

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

enum{
  PROP_0,
  PROP_FILE_LINK,
  PROP_SYNTH_GENERATOR,
};

GType
ags_input_get_type (void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_input = 0;

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

    ags_type_input = g_type_register_static(AGS_TYPE_CHANNEL,
					    "AgsInput",
					    &ags_input_info,
					    0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_input);
  }

  return g_define_type_id__volatile;
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
   * Since: 3.0.0
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
   * AgsInput:synth-generator: (type GList(AgsSynthGenerator)) (transfer full)
   *
   * The synth generators assigned with this input.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_pointer("synth-generator",
				    i18n_pspec("the synth generator"),
				    i18n_pspec("The synth generator to be used"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_GENERATOR,
				  param_spec);
}

void
ags_input_init(AgsInput *input)
{
  g_signal_connect_after(input, "notify::samplerate",
			 G_CALLBACK(ags_input_notify_samplerate_callback), NULL);
  g_signal_connect_after(input, "notify::buffer-size",
			 G_CALLBACK(ags_input_notify_buffer_size_callback), NULL);
  g_signal_connect_after(input, "notify::format",
			 G_CALLBACK(ags_input_notify_format_callback), NULL);

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
  
  GRecMutex *channel_mutex;

  input = AGS_INPUT(gobject);

  /* get channel mutex */
  channel_mutex = AGS_CHANNEL_GET_OBJ_MUTEX(input);

  switch(prop_id){
  case PROP_FILE_LINK:
    {
      AgsFileLink *file_link;

      file_link = (AgsFileLink *) g_value_get_object(value);

      g_rec_mutex_lock(channel_mutex);

      if(input->file_link == (GObject *) file_link){
	g_rec_mutex_unlock(channel_mutex);

	return;
      }
      
      if(input->file_link != NULL){
	g_object_unref(G_OBJECT(input->file_link));
      }

      if(file_link != NULL){
	g_object_ref(G_OBJECT(file_link));
      }

      input->file_link = (GObject *) file_link;

      g_rec_mutex_unlock(channel_mutex);
    }
    break;
  case PROP_SYNTH_GENERATOR:
    {
      AgsSynthGenerator *synth_generator;

      synth_generator = (AgsSynthGenerator *) g_value_get_pointer(value);

      g_rec_mutex_lock(channel_mutex);

      if(g_list_find(input->synth_generator, synth_generator) != NULL){
	g_rec_mutex_unlock(channel_mutex);
	
	return;
      }

      g_rec_mutex_unlock(channel_mutex);
      
      ags_input_add_synth_generator(input, (GObject *) synth_generator);
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

  GRecMutex *channel_mutex;

  input = AGS_INPUT(gobject);

  /* get channel mutex */
  channel_mutex = AGS_CHANNEL_GET_OBJ_MUTEX(input);

  switch(prop_id){
  case PROP_FILE_LINK:
    {
      g_rec_mutex_lock(channel_mutex);

      g_value_set_object(value, input->file_link);

      g_rec_mutex_unlock(channel_mutex);
    }
    break;
  case PROP_SYNTH_GENERATOR:
    {
      g_rec_mutex_lock(channel_mutex);

      g_value_set_pointer(value, g_list_copy_deep(input->synth_generator,
						  (GCopyFunc) g_object_ref,
						  NULL));

      g_rec_mutex_unlock(channel_mutex);
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

  GList *list, *list_next;
  
  input = AGS_INPUT(gobject);

  /* file link */
  if(input->file_link != NULL){
    g_object_run_dispose(G_OBJECT(input->file_link));

    g_object_unref(G_OBJECT(input->file_link));
    
    input->file_link = NULL;
  }

  /* synth generator */
  if(input->synth_generator != NULL){
    list = input->synth_generator;
    
    while(list != NULL){
      list_next = list->next;

      g_object_run_dispose(list->data);

      list = list_next;
    }

    g_list_free_full(input->synth_generator,
		     g_object_unref);
    
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
  g_list_free_full(input->synth_generator,
		   g_object_unref);

  /* finalize */
  G_OBJECT_CLASS(ags_input_parent_class)->finalize(gobject);
}

void
ags_input_notify_samplerate_callback(GObject *gobject,
				     GParamSpec *pspec,
				     gpointer user_data)
{
  AgsInput *input;

  GList *start_list, *list;
  
  guint samplerate;
  
  input = AGS_INPUT(gobject);
  
  g_object_get(gobject,
	       "samplerate", &samplerate,
	       "synth-generator", &start_list,
	       NULL);

  /* apply to synth generator */
  list = start_list;

  while(list != NULL){
    g_object_set(list->data,
		 "samplerate", samplerate,
		 NULL);

    list = list->next;
  }

  g_list_free(start_list);
}

void
ags_input_notify_buffer_size_callback(GObject *gobject,
				      GParamSpec *pspec,
				      gpointer user_data)
{
  AgsInput *input;

  GList *start_list, *list;
  
  guint buffer_size;
  
  input = AGS_INPUT(gobject);
  
  g_object_get(gobject,
	       "buffer-size", &buffer_size,
	       "synth-generator", &start_list,
	       NULL);

  /* apply to synth generator */
  list = start_list;

  while(list != NULL){
    g_object_set(list->data,
		 "buffer-size", buffer_size,
		 NULL);

    list = list->next;
  }

  g_list_free(start_list);
}

void
ags_input_notify_format_callback(GObject *gobject,
				 GParamSpec *pspec,
				 gpointer user_data)
{
  AgsInput *input;

  GList *start_list, *list;
  
  guint format;
  
  input = AGS_INPUT(gobject);
  
  g_object_get(gobject,
	       "format", &format,
	       "synth-generator", &start_list,
	       NULL);

  /* apply to synth generator */
  list = start_list;

  while(list != NULL){
    g_object_set(list->data,
		 "format", format,
		 NULL);

    list = list->next;
  }

  g_list_free(start_list);
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
 * Since: 3.0.0
 */
gboolean
ags_input_is_active(AgsInput *input,
		    GObject *recycling_context)
{
  AgsChannel *channel;
  AgsRecycling *first_recycling, *last_recycling;
  AgsRecycling *recycling, *next_recycling, *end_recycling;
  AgsAudioSignal *audio_signal;
  AgsRecyclingContext *active_context;
  
  GList *start_list, *list;

  gboolean success;
  
  if(!AGS_IS_INPUT(input) ||
     !AGS_IS_RECYCLING_CONTEXT(recycling_context)){
    return(FALSE);
  }
  
  channel = (AgsChannel *) input;
    
  /* get recycling */
  g_object_get(channel,
	       "first-recycling", &first_recycling,
	       "last-recycling", &last_recycling,
	       NULL);
  
  if(first_recycling == NULL){
    return(FALSE);
  }

  /* get active recycling context */
  active_context = (AgsRecyclingContext *) recycling_context;

  /* get end recycling */
  end_recycling = ags_recycling_next(last_recycling);

  /* check if active */
  recycling = first_recycling;
  g_object_ref(recycling);

  success = FALSE;
  
  while(recycling != end_recycling){
    /* get audio signal */
    g_object_get(recycling,
		 "audio-signal", &start_list,
		 NULL);

    list = start_list;
    
    while(list != NULL){
      AgsRecallID *current_recall_id;
      AgsRecyclingContext *current_recycling_context;

      audio_signal = list->data;

      /* get recall id */
      g_object_get(audio_signal,
		   "recall-id", &current_recall_id,
		   NULL);
      
      if(current_recall_id == NULL){
	list = list->next;
	
	continue; 
      }

      /* get recycling context */
      g_object_get(current_recall_id,
		   "recycling-context", &current_recycling_context,
		   NULL);

      success = (current_recycling_context == active_context) ? TRUE: FALSE;

      if(current_recycling_context != NULL){
	g_object_unref(current_recycling_context);
      }
      
      if(success){
	g_list_free_full(start_list,
			 g_object_unref);
	
	goto ags_input_is_active_END;
      }

      list = list->next;
    }

    g_list_free_full(start_list,
		     g_object_unref);
    
    /* iterate */
    next_recycling = ags_recycling_next(recycling);

    g_object_unref(recycling);

    recycling = next_recycling;
  }

ags_input_is_active_END:
  g_object_unref(first_recycling);
  g_object_unref(last_recycling);

  if(end_recycling != NULL){
    g_object_unref(end_recycling);
  }
  
  return(success);
}

/**
 * ags_input_next_active:
 * @input: the #AgsInput
 * @recycling_context: the #AgsRecyclingContext to check
 * 
 * Find next #AgsInput needed to be processed.
 * 
 * Returns: (transfer full): next active #AgsInput, else %NULL if non available
 * 
 * Since: 3.0.0
 */
AgsInput*
ags_input_next_active(AgsInput *input,
		      GObject *recycling_context)
{
  AgsChannel *channel, *next_pad_channel;
  AgsInput *retval;
  AgsRecycling *first_recycling, *last_recycling;
  AgsRecycling *recycling, *next_recycling, *end_recycling;
  AgsAudioSignal *audio_signal;  
  AgsRecyclingContext *active_context;

  GList *start_list, *list;  

  gboolean success;
  
  if(!AGS_IS_INPUT(input) ||
     !AGS_IS_RECYCLING_CONTEXT(recycling_context)){
    return(NULL);
  }

  channel = (AgsChannel *) input;
  g_object_ref(channel);
  
  retval = NULL;
  
  /* check if active */
  while(channel != NULL){
    /* get recycling */
    g_object_get(channel,
		 "first-recycling", &first_recycling,
		 "last-recycling", &last_recycling,
		 NULL);
  
    if(first_recycling == NULL){
      /* iterate */
      next_pad_channel = ags_channel_next_pad(channel);

      g_object_unref(channel);

      channel = next_pad_channel;
      
      continue;
    }

    /* get active recycling context */
    active_context = (AgsRecyclingContext *) recycling_context;

    /* get end recycling */
    end_recycling = ags_recycling_next(last_recycling);
    
    /* check recycling */
    recycling = first_recycling;
    
    while(recycling != end_recycling){
      /* get audio signal */
      g_object_get(recycling,
		   "audio-signal", &start_list,
		   NULL);

      /* check audio signal */
      list = start_list;
      
      while(list != NULL){
	AgsRecallID *current_recall_id;
	AgsRecyclingContext *current_recycling_context;

	audio_signal = list->data;

	/* get recall id */
	g_object_get(audio_signal,
		     "recall-id", &current_recall_id,
		     NULL);
      
	if(current_recall_id == NULL){
	  list = list->next;
	
	  continue; 
	}

	/* get recycling context */
	g_object_get(current_recall_id,
		     "recycling-context", &current_recycling_context,
		     NULL);

	success = (current_recycling_context == active_context) ? TRUE: FALSE;
	
	if(success){
	  g_list_free_full(start_list,
			   g_object_unref);

	  retval = channel;

	  goto ags_input_next_active_END;
	}

	list = list->next;
      }

      g_list_free_full(start_list,
		       g_object_unref);

      /* iterate */
      next_recycling = ags_recycling_next(recycling);

      g_object_unref(recycling);

      recycling = next_recycling;
    }

  ags_input_next_active_END:
    g_object_unref(first_recycling);
    g_object_unref(last_recycling);

    if(end_recycling != NULL){
      g_object_unref(end_recycling);
    }

    if(retval != NULL){
      break;
    }
    
    /* iterate */
    next_pad_channel = ags_channel_next_pad(channel);

    g_object_unref(channel);

    channel = next_pad_channel;
  }
  
  return(retval);
}

/**
 * ags_input_add_synth_generator:
 * @input: the #AgsInput
 * @synth_generator: the #AgsSynthGenerator
 * 
 * Add @synth_generator to @input.
 * 
 * Since: 3.0.0
 */
void
ags_input_add_synth_generator(AgsInput *input,
			      GObject *synth_generator)
{
  if(!AGS_IS_INPUT(input) ||
     !AGS_IS_SYNTH_GENERATOR(synth_generator)){
    return;
  }

  g_object_ref(synth_generator);
  input->synth_generator = g_list_prepend(input->synth_generator,
					  synth_generator);
}

/**
 * ags_input_remove_synth_generator:
 * @input: the #AgsInput
 * @synth_generator: the #AgsSynthGenerator
 * 
 * Remove @synth_generator from @input.
 * 
 * Since: 3.0.0
 */
void
ags_input_remove_synth_generator(AgsInput *input,
				 GObject *synth_generator)
{
  if(!AGS_IS_INPUT(input) ||
     !AGS_IS_SYNTH_GENERATOR(synth_generator)){
    return;
  }

  input->synth_generator = g_list_remove(input->synth_generator,
					 synth_generator);
  g_object_unref(synth_generator);
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
 * Since: 3.0.0
 */
gboolean
ags_input_open_file(AgsInput *input,
		    gchar *filename,
		    gchar *preset,
		    gchar *instrument,
		    gchar *sample,
		    guint audio_channel)
{
  AgsRecycling *recycling;
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
				    AGS_CHANNEL(input)->output_soundcard,
				    audio_channel);
    
    ags_audio_file_open(audio_file);
    ags_audio_file_read_audio_signal(audio_file);

    audio_signal = audio_file->audio_signal;
    //    g_object_unref(audio_file);
  }else if(ags_ipatch_check_suffix(filename)){
    AgsAudioContainer *audio_container;

    success = TRUE;

    preset = NULL;
    instrument = NULL;
    
    /* open audio container and read audio signal */
    audio_container = ags_audio_container_new(filename,
					      preset,
					      instrument,
					      sample,
					      AGS_CHANNEL(input)->output_soundcard,
					      audio_channel);
    
    ags_audio_container_open(audio_container);
    ags_audio_container_read_audio_signal(audio_container);

    audio_signal = audio_container->audio_signal;
    //    g_object_unref(audio_container);
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
      g_object_get(input,
		   "first-recycling", &recycling,
		   NULL);
      ags_recycling_add_audio_signal(recycling,
				     AGS_AUDIO_SIGNAL(audio_signal->data));

      g_object_unref(recycling);
    }else{
      success = FALSE;
    }
  }
  
  return(success);
}

/**
 * ags_input_new:
 * @audio: the #AgsAudio
 *
 * Creates a #AgsInput, linking tree to @audio.
 *
 * Returns: a new #AgsInput
 *
 * Since: 3.0.0
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
