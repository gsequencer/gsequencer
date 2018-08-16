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

#include <ags/audio/ags_input.h>

#include <ags/libags.h>

#include <ags/audio/ags_recycling.h>
#include <ags/audio/ags_audio_signal.h>
#include <ags/audio/ags_synth_generator.h>

#include <ags/audio/file/ags_audio_file_link.h>
#include <ags/audio/file/ags_audio_container.h>
#include <ags/audio/file/ags_audio_file.h>
#include <ags/audio/file/ags_sound_container.h>
#include <ags/audio/file/ags_sound_resource.h>

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
   * Since: 2.0.0
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
   * The synth generators assigned with this input.
   * 
   * Since: 2.0.0
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
      AgsSynthGenerator *synth_generator;

      synth_generator = (AgsSynthGenerator *) g_value_get_pointer(value);

      if(g_list_find(input->synth_generator, synth_generator) != NULL){
	return;
      }
      
      ags_input_add_synth_generator(input, synth_generator);
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
      g_value_set_pointer(value, g_list_copy(input->synth_generator));
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
  AgsRecycling *first_recycling, *last_recycling;
  AgsRecycling *recycling, *end_region;
  AgsAudioSignal *audio_signal;
  AgsRecyclingContext *active_context;
  
  GList *list_start, *list;
  
  pthread_mutex_t *channel_mutex;
  pthread_mutex_t *recycling_mutex;
  pthread_mutex_t *audio_signal_mutex;
  
  if(!AGS_IS_INPUT(input) ||
     !AGS_IS_RECYCLING_CONTEXT(recycling_context)){
    return(FALSE);
  }
  
  channel = input;

  /* get channel mutex */
  pthread_mutex_lock(ags_channel_get_class_mutex());
  
  channel_mutex = channel->obj_mutex;
  
  pthread_mutex_unlock(ags_channel_get_class_mutex());
    
  /* get recycling */
  pthread_mutex_lock(channel_mutex);
  
  first_recycling = channel->first_recycling;
  last_recycling = channel->last_recycling;

  pthread_mutex_unlock(channel_mutex);
  
  if(first_recycling == NULL){
    return(FALSE);
  }

  /* get active recycling context */
  active_context = recycling_context;
  
  /* get recycling mutex */
  pthread_mutex_lock(ags_recycling_get_class_mutex());
  
  recycling_mutex = last_recycling->obj_mutex;
  
  pthread_mutex_unlock(ags_recycling_get_class_mutex());

  /* get end region */  
  pthread_mutex_lock(recycling_mutex);

  end_region = last_recycling->next;

  pthread_mutex_lock(recycling_mutex);

  /* check if active */
  recycling = first_recycling;

  while(recycling != end_region){
    /* get recycling mutex */
    pthread_mutex_lock(ags_recycling_get_class_mutex());
  
    recycling_mutex = recycling->obj_mutex;
  
    pthread_mutex_unlock(ags_recycling_get_class_mutex());
    
    /* get audio signal */
    pthread_mutex_lock(recycling_mutex);

    list = 
      list_start = g_list_copy(recycling->audio_signal);

    pthread_mutex_unlock(recycling_mutex);  

    while(list != NULL){
      AgsRecallID *current_recall_id;
      AgsRecyclingContext *current_recycling_context;

      pthread_mutex_t *current_recall_id_mutex;

      audio_signal = list->data;

      /* get audio signal mutex */
      pthread_mutex_lock(ags_audio_signal_get_class_mutex());
  
      audio_signal_mutex = audio_signal->obj_mutex;
  
      pthread_mutex_unlock(ags_audio_signal_get_class_mutex());

      /* get recall id */
      pthread_mutex_lock(audio_signal_mutex);
      
      current_recall_id = audio_signal->recall_id;

      pthread_mutex_unlock(audio_signal_mutex);
      
      if(current_recall_id == NULL){
	list = list->next;
	
	continue; 
      }

      /* get recall id mutex */
      pthread_mutex_lock(ags_recall_id_get_class_mutex());
  
      current_recall_id_mutex = current_recall_id->obj_mutex;
  
      pthread_mutex_unlock(ags_recall_id_get_class_mutex());

      /* get recycling context */
      pthread_mutex_lock(current_recall_id_mutex);
      
      current_recycling_context = current_recall_id->recycling_context;

      pthread_mutex_unlock(current_recall_id_mutex);      
      
      if(current_recycling_context == active_context) {
	g_list_free(list_start);
	
	return(TRUE);
      }

      list = list->next;
    }

    g_list_free(list_start);
    
    /* iterate */
    pthread_mutex_lock(recycling_mutex);

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
  AgsRecycling *first_recycling, *last_recycling;
  AgsRecycling *recycling, *end_region;
  AgsAudioSignal *audio_signal;  
  AgsRecyclingContext *active_context;

  GList *list_start, *list;
  
  pthread_mutex_t *channel_mutex;
  pthread_mutex_t *recycling_mutex;
  pthread_mutex_t *audio_signal_mutex;

  if(!AGS_IS_INPUT(input) ||
     !AGS_IS_RECYCLING_CONTEXT(recycling_context)){
    return(NULL);
  }

  channel = input;

  /* check if active */
  while(channel != NULL){
    /* get channel mutex */
    pthread_mutex_lock(ags_channel_get_class_mutex());
  
    channel_mutex = channel->obj_mutex;
  
    pthread_mutex_unlock(ags_channel_get_class_mutex());
    
    /* get recycling */
    pthread_mutex_lock(channel_mutex);
  
    first_recycling = channel->first_recycling;
    last_recycling = channel->last_recycling;

    pthread_mutex_unlock(channel_mutex);
  
    if(first_recycling == NULL){
      /* iterate */
      pthread_mutex_lock(channel_mutex);

      channel = channel->next_pad;
    
      pthread_mutex_unlock(channel_mutex);
      
      continue;
    }

    /* get active recycling context */
    active_context = recycling_context;
  
    /* get recycling mutex */
    pthread_mutex_lock(ags_recycling_get_class_mutex());
  
    recycling_mutex = last_recycling->obj_mutex;
  
    pthread_mutex_unlock(ags_recycling_get_class_mutex());

    /* get end region */  
    pthread_mutex_lock(recycling_mutex);

    end_region = last_recycling->next;

    pthread_mutex_lock(recycling_mutex);
    
    /* check recycling */
    recycling = first_recycling;
    
    while(recycling != end_region){
      /* get recycling mutex */
      pthread_mutex_lock(ags_recycling_get_class_mutex());
  
      recycling_mutex = recycling->obj_mutex;
  
      pthread_mutex_unlock(ags_recycling_get_class_mutex());
    
      /* get audio signal */
      pthread_mutex_lock(recycling_mutex);

      list = 
	list_start = g_list_copy(recycling->audio_signal);

      pthread_mutex_unlock(recycling_mutex);  

      /* check audio signal */
      while(list != NULL){
	AgsRecallID *current_recall_id;
	AgsRecyclingContext *current_recycling_context;

	pthread_mutex_t *current_recall_id_mutex;
      
	audio_signal = list->data;

	/* get audio signal mutex */
	pthread_mutex_lock(ags_audio_signal_get_class_mutex());
  
	audio_signal_mutex = audio_signal->obj_mutex;
  
	pthread_mutex_unlock(ags_audio_signal_get_class_mutex());

	/* get recall id */
	pthread_mutex_lock(audio_signal_mutex);
      
	current_recall_id = audio_signal->recall_id;

	pthread_mutex_unlock(audio_signal_mutex);
      
	if(current_recall_id == NULL){
	  list = list->next;
	
	  continue; 
	}

	/* get recall id mutex */
	pthread_mutex_lock(ags_recall_id_get_class_mutex());
  
	current_recall_id_mutex = current_recall_id->obj_mutex;
  
	pthread_mutex_unlock(ags_recall_id_get_class_mutex());

	/* get recycling context */
	pthread_mutex_lock(current_recall_id_mutex);
      
	current_recycling_context = current_recall_id->recycling_context;

	pthread_mutex_unlock(current_recall_id_mutex);      
      
	if(current_recycling_context == active_context) {
	  g_list_free(list_start);
	
	  return(channel);
	}

	list = list->next;
      }

      g_list_free(list_start);

      /* iterate */
      pthread_mutex_lock(recycling_mutex);

      recycling = recycling->next;
    
      pthread_mutex_unlock(recycling_mutex);  
    }

    /* iterate */
    pthread_mutex_lock(channel_mutex);

    channel = channel->next_pad;
    
    pthread_mutex_unlock(channel_mutex);
  }
  
  return(NULL);
}

/**
 * ags_input_add_synth_generator:
 * @input: the #AgsInput
 * @synth_generator: the #AgsSynthGenerator
 * 
 * Add @synth_generator to @input.
 * 
 * Since: 2.0.0
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
 * Since: 2.0.0
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
      ags_recycling_add_audio_signal(AGS_CHANNEL(input)->first_recycling,
				     AGS_AUDIO_SIGNAL(audio_signal->data));
    }else{
      success = FALSE;
    }
  }
  
  return(success);
}

void
ags_input_apply_synth(AgsInput *input)
{
  if(!AGS_IS_INPUT(input)){
    return;
  }

  //TODO:JK: implement me
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
