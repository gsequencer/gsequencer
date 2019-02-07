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

#include <ags/audio/ags_midi.h>

#include <ags/libags.h>

#include <ags/audio/ags_audio.h>

#include <ags/i18n.h>

#include <errno.h>

void ags_midi_class_init(AgsMidiClass *midi);
void ags_midi_init(AgsMidi *midi);
void ags_midi_set_property(GObject *gobject,
			   guint prop_id,
			   const GValue *value,
			   GParamSpec *param_spec);
void ags_midi_get_property(GObject *gobject,
			   guint prop_id,
			   GValue *value,
			   GParamSpec *param_spec);
void ags_midi_dispose(GObject *gobject);
void ags_midi_finalize(GObject *gobject);

/**
 * SECTION:ags_midi
 * @short_description: Midi class supporting selection and clipboard.
 * @title: AgsMidi
 * @section_id:
 * @include: ags/audio/ags_midi.h
 *
 * #AgsMidi acts as a container of #AgsTrack.
 */

enum{
  PROP_0,
  PROP_AUDIO,
  PROP_AUDIO_CHANNEL,
  PROP_TRACK,
  PROP_TIMESTAMP,
};

static gpointer ags_midi_parent_class = NULL;

static pthread_mutex_t ags_midi_class_mutex = PTHREAD_MUTEX_INITIALIZER;

GType
ags_midi_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_midi = 0;

    static const GTypeInfo ags_midi_info = {
      sizeof(AgsMidiClass),
      NULL,
      NULL,
      (GClassInitFunc) ags_midi_class_init,
      NULL,
      NULL,
      sizeof(AgsMidi),
      0,
      (GInstanceInitFunc) ags_midi_init,
    };

    ags_type_midi = g_type_register_static(G_TYPE_OBJECT,
					   "AgsMidi",
					   &ags_midi_info,
					   0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_midi);
  }

  return g_define_type_id__volatile;
}

void 
ags_midi_class_init(AgsMidiClass *midi)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_midi_parent_class = g_type_class_peek_parent(midi);

  gobject = (GObjectClass *) midi;

  gobject->set_property = ags_midi_set_property;
  gobject->get_property = ags_midi_get_property;

  gobject->dispose = ags_midi_dispose;
  gobject->finalize = ags_midi_finalize;

  /* properties */
  /**
   * AgsMidi:audio:
   *
   * The assigned #AgsAudio
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("audio",
				   i18n_pspec("audio of midi"),
				   i18n_pspec("The audio of midi"),
				   AGS_TYPE_AUDIO,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUDIO,
				  param_spec);

  /**
   * AgsMidi:audio-channel:
   *
   * The midi's audio-channel.
   * 
   * Since: 2.0.0
   */
  param_spec =  g_param_spec_uint("audio-channel",
				  i18n_pspec("audio-channel of midi"),
				  i18n_pspec("The numerical audio-channel of midi"),
				  0,
				  G_MAXUINT32,
				  0,
				  G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUDIO_CHANNEL,
				  param_spec);  
  
  /**
   * AgsPattern:timestamp:
   *
   * The midi's timestamp.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("timestamp",
				   i18n_pspec("timestamp of pattern"),
				   i18n_pspec("The timestamp of pattern"),
				   AGS_TYPE_TIMESTAMP,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_TIMESTAMP,
				  param_spec);

  /**
   * AgsMidi:track:
   *
   * The assigned #AgsTrack
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_pointer("track",
				    i18n_pspec("track of midi"),
				    i18n_pspec("The track of midi"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_TRACK,
				  param_spec);
}

void
ags_midi_init(AgsMidi *midi)
{
  pthread_mutex_t *mutex;
  pthread_mutexattr_t *attr;

  midi->flags = 0;

  /* add midi mutex */
  midi->obj_mutexattr = 
    attr = (pthread_mutexattr_t *) malloc(sizeof(pthread_mutexattr_t));
  pthread_mutexattr_init(attr);
  pthread_mutexattr_settype(attr,
			    PTHREAD_MUTEX_RECURSIVE);

#ifdef __linux__
  pthread_mutexattr_setprotocol(attr,
				PTHREAD_PRIO_INHERIT);
#endif

  midi->obj_mutex = 
    mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(mutex,
		     attr);  

  /* fields */  
  midi->audio = NULL;
  midi->audio_channel = 0;

  midi->timestamp = ags_timestamp_new();

  midi->timestamp->flags &= (~AGS_TIMESTAMP_UNIX);
  midi->timestamp->flags |= AGS_TIMESTAMP_OFFSET;

  midi->timestamp->timer.ags_offset.offset = 0;

  g_object_ref(midi->timestamp);  
  
  midi->track = NULL;
  midi->selection = NULL;
}

void
ags_midi_set_property(GObject *gobject,
		      guint prop_id,
		      const GValue *value,
		      GParamSpec *param_spec)
{
  AgsMidi *midi;

  pthread_mutex_t *midi_mutex;

  midi = AGS_MIDI(gobject);

  /* get midi mutex */
  pthread_mutex_lock(ags_midi_get_class_mutex());
  
  midi_mutex = midi->obj_mutex;
  
  pthread_mutex_unlock(ags_midi_get_class_mutex());

  switch(prop_id){
  case PROP_AUDIO:
    {
      AgsAudio *audio;

      audio = (AgsAudio *) g_value_get_object(value);

      pthread_mutex_lock(midi_mutex);

      if(midi->audio == (GObject *) audio){
	pthread_mutex_unlock(midi_mutex);

	return;
      }

      if(midi->audio != NULL){
	g_object_unref(midi->audio);
      }

      if(audio != NULL){
	g_object_ref(audio);
      }

      midi->audio = (GObject *) audio;

      pthread_mutex_unlock(midi_mutex);
    }
    break;
  case PROP_AUDIO_CHANNEL:
    {
      guint audio_channel;

      audio_channel = g_value_get_uint(value);

      pthread_mutex_lock(midi_mutex);

      midi->audio_channel = audio_channel;

      pthread_mutex_unlock(midi_mutex);
    }
    break;
  case PROP_TIMESTAMP:
    {
      AgsTimestamp *timestamp;

      timestamp = (AgsTimestamp *) g_value_get_object(value);

      pthread_mutex_lock(midi_mutex);

      if(timestamp == (AgsTimestamp *) midi->timestamp){
	pthread_mutex_unlock(midi_mutex);

	return;
      }

      if(midi->timestamp != NULL){
	g_object_unref(G_OBJECT(midi->timestamp));
      }

      if(timestamp != NULL){
	g_object_ref(G_OBJECT(timestamp));
      }

      midi->timestamp = timestamp;

      pthread_mutex_unlock(midi_mutex);
    }
    break;
  case PROP_TRACK:
    {
      AgsTrack *track;

      track = (AgsTrack *) g_value_get_pointer(value);

      pthread_mutex_lock(midi_mutex);

      if(track == NULL ||
	 g_list_find(midi->track, track) != NULL){
	pthread_mutex_unlock(midi_mutex);
	
	return;
      }

      pthread_mutex_unlock(midi_mutex);

      ags_midi_add_track(midi,
			 track,
			 FALSE);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_midi_get_property(GObject *gobject,
		      guint prop_id,
		      GValue *value,
		      GParamSpec *param_spec)
{
  AgsMidi *midi;

  pthread_mutex_t *midi_mutex;

  midi = AGS_MIDI(gobject);

  /* get midi mutex */
  pthread_mutex_lock(ags_midi_get_class_mutex());
  
  midi_mutex = midi->obj_mutex;
  
  pthread_mutex_unlock(ags_midi_get_class_mutex());

  switch(prop_id){
  case PROP_AUDIO:
    {
      pthread_mutex_lock(midi_mutex);

      g_value_set_object(value, midi->audio);

      pthread_mutex_unlock(midi_mutex);
    }
    break;
  case PROP_AUDIO_CHANNEL:
    {
      pthread_mutex_lock(midi_mutex);

      g_value_set_uint(value, midi->audio_channel);

      pthread_mutex_unlock(midi_mutex);
    }
    break;
  case PROP_TIMESTAMP:
    {
      pthread_mutex_lock(midi_mutex);

      g_value_set_object(value, midi->timestamp);

      pthread_mutex_unlock(midi_mutex);
    }
    break;
  case PROP_TRACK:
    {
      pthread_mutex_lock(midi_mutex);

      g_value_set_pointer(value, g_list_copy_deep(midi->track,
						  (GCopyFunc) g_object_ref,
						  NULL));

      pthread_mutex_unlock(midi_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_midi_dispose(GObject *gobject)
{
  AgsMidi *midi;

  GList *list;
  
  midi = AGS_MIDI(gobject);

  /* audio */
  if(midi->audio != NULL){
    g_object_unref(midi->audio);

    midi->audio = NULL;
  }
  
  /* timestamp */
  if(midi->timestamp != NULL){
    g_object_unref(midi->timestamp);

    midi->timestamp = NULL;
  }
    
  /* track and selection */
  list = midi->track;

  while(list != NULL){
    g_object_run_dispose(G_OBJECT(list->data));
    
    list = list->next;
  }
  
  g_list_free_full(midi->track,
		   g_object_unref);
  g_list_free_full(midi->selection,
		   g_object_unref);

  midi->track = NULL;
  midi->selection = NULL;
    
  /* call parent */
  G_OBJECT_CLASS(ags_midi_parent_class)->dispose(gobject);
}

void
ags_midi_finalize(GObject *gobject)
{
  AgsMidi *midi;

  midi = AGS_MIDI(gobject);

  pthread_mutex_destroy(midi->obj_mutex);
  free(midi->obj_mutex);

  pthread_mutexattr_destroy(midi->obj_mutexattr);
  free(midi->obj_mutexattr);

  /* audio */
  if(midi->audio != NULL){
    g_object_unref(midi->audio);
  }
  
  /* timestamp */
  if(midi->timestamp != NULL){
    g_object_unref(midi->timestamp);
  }

  /* track and selection */
  g_list_free_full(midi->track,
		   g_object_unref);

  g_list_free_full(midi->selection,
		   g_object_unref);
  
  /* call parent */
  G_OBJECT_CLASS(ags_midi_parent_class)->finalize(gobject);
}

/**
 * ags_midi_get_class_mutex:
 * 
 * Use this function's returned mutex to access mutex fields.
 *
 * Returns: the class mutex
 * 
 * Since: 2.0.0
 */
pthread_mutex_t*
ags_midi_get_class_mutex()
{
  return(&ags_midi_class_mutex);
}

/**
 * ags_midi_test_flags:
 * @midi: the #AgsMidi
 * @flags: the flags
 * 
 * Test @flags to be set on @midi.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 * 
 * Since: 2.0.0
 */
gboolean
ags_midi_test_flags(AgsMidi *midi, guint flags)
{
  gboolean retval;
  
  pthread_mutex_t *midi_mutex;

  if(!AGS_IS_MIDI(midi)){
    return(FALSE);
  }
      
  /* get midi mutex */
  pthread_mutex_lock(ags_midi_get_class_mutex());
  
  midi_mutex = midi->obj_mutex;
  
  pthread_mutex_unlock(ags_midi_get_class_mutex());

  /* test */
  pthread_mutex_lock(midi_mutex);

  retval = (flags & (midi->flags)) ? TRUE: FALSE;
  
  pthread_mutex_unlock(midi_mutex);

  return(retval);
}

/**
 * ags_midi_set_flags:
 * @midi: the #AgsMidi
 * @flags: the flags
 * 
 * Set @flags on @midi.
 * 
 * Since: 2.0.0
 */
void
ags_midi_set_flags(AgsMidi *midi, guint flags)
{
  pthread_mutex_t *midi_mutex;

  if(!AGS_IS_MIDI(midi)){
    return;
  }
      
  /* get midi mutex */
  pthread_mutex_lock(ags_midi_get_class_mutex());
  
  midi_mutex = midi->obj_mutex;
  
  pthread_mutex_unlock(ags_midi_get_class_mutex());

  /* set */
  pthread_mutex_lock(midi_mutex);

  midi->flags |= flags;
  
  pthread_mutex_unlock(midi_mutex);
}

/**
 * ags_midi_unset_flags:
 * @midi: the #AgsMidi
 * @flags: the flags
 * 
 * Unset @flags on @midi.
 * 
 * Since: 2.0.0
 */
void
ags_midi_unset_flags(AgsMidi *midi, guint flags)
{
  pthread_mutex_t *midi_mutex;

  if(!AGS_IS_MIDI(midi)){
    return;
  }
      
  /* get midi mutex */
  pthread_mutex_lock(ags_midi_get_class_mutex());
  
  midi_mutex = midi->obj_mutex;
  
  pthread_mutex_unlock(ags_midi_get_class_mutex());

  /* set */
  pthread_mutex_lock(midi_mutex);

  midi->flags &= (~flags);
  
  pthread_mutex_unlock(midi_mutex);
}

/**
 * ags_midi_find_near_timestamp:
 * @midi: a #GList containing #AgsMidi
 * @audio_channel: the matching audio channel
 * @timestamp: (allow-none): the matching timestamp, or %NULL to match any timestamp
 *
 * Retrieve appropriate midi for timestamp.
 *
 * Returns: Next match.
 *
 * Since: 2.0.0
 */
GList*
ags_midi_find_near_timestamp(GList *midi, guint audio_channel,
			     AgsTimestamp *timestamp)
{
  AgsTimestamp *current_start_timestamp, *current_end_timestamp, *current_timestamp;

  GList *retval;
  GList *current_start, *current_end, *current;

  guint current_audio_channel;
  guint64 current_x, x;
  guint length, position;
  gboolean use_ags_offset;
  gboolean success;

  if(midi == NULL){
    return(NULL);
  }

  current_start = midi;
  current_end = g_list_last(midi);
  
  length = g_list_length(midi);
  position = length / 2;

  current = g_list_nth(current_start,
		       position);

  if(ags_timestamp_test_flags(timestamp,
			      AGS_TIMESTAMP_OFFSET)){
    x = ags_timestamp_get_ags_offset(timestamp);

    use_ags_offset = TRUE;
  }else if(ags_timestamp_test_flags(timestamp,
				    AGS_TIMESTAMP_UNIX)){
    x = ags_timestamp_get_unix_time(timestamp);

    use_ags_offset = FALSE;
  }else{
    return(NULL);
  }
  
  retval = NULL;
  success = FALSE;
  
  while(!success && current != NULL){
    current_x = 0;
    
    /* check current - start */
    g_object_get(current_start->data,
		 "audio-channel", &current_audio_channel,
		 NULL);
    
    if(current_audio_channel == audio_channel){
      if(timestamp == NULL){
	retval = current_start;
	
	break;
      }

      g_object_get(current_start->data,
		   "timestamp", &current_timestamp,
		   NULL);
      
      if(current_timestamp != NULL){
	if(use_ags_offset){
	  current_x = ags_timestamp_get_ags_offset(current_timestamp);

	  g_object_unref(current_timestamp);
	  
	  if(current_x > x){
	    break;
	  }
	}else{
	  current_x = ags_timestamp_get_unix_time(current_timestamp);
	  
	  g_object_unref(current_timestamp);

	  if(current_x > x){
	    break;
	  }
	}
	
	if(use_ags_offset){
	  if(current_x >= x &&
	     current_x < x + AGS_MIDI_DEFAULT_OFFSET){
	    retval = current_start;
	    
	    break;
	  }
	}else{
	  if(current_x >= x &&
	     current_x < x + AGS_MIDI_DEFAULT_DURATION){
	    retval = current_start;
	    
	    break;
	  }
	}
      }else{
	g_warning("inconsistent data");
      }
    }

    /* check current - end */
    g_object_get(current_end->data,
		 "audio-channel", &current_audio_channel,
		 NULL);
    
    if(current_audio_channel == audio_channel){
      if(timestamp == NULL){
	retval = current_end;
	
	break;
      }

      g_object_get(current_end->data,
		   "timestamp", &current_timestamp,
		   NULL);
      
      if(current_timestamp != NULL){
	if(use_ags_offset){
	  current_x = ags_timestamp_get_ags_offset(current_timestamp);

	  g_object_unref(current_timestamp);

	  if(current_x < x){
	    break;
	  }
	}else{
	  current_x = ags_timestamp_get_unix_time(current_timestamp);
	  
	  g_object_unref(current_timestamp);

	  if(current_x < x){
	    break;
	  }
	}

	if(use_ags_offset){
	  if(current_x >= x &&
	     current_x < x + AGS_MIDI_DEFAULT_OFFSET){
	    retval = current_end;
	    
	    break;
	  }
	}else{
	  if(current_x >= x &&
	     current_x < x + AGS_MIDI_DEFAULT_DURATION){
	    retval = current_end;
	    
	    break;
	  }
	}
      }else{
	g_warning("inconsistent data");
      }
    }

    /* check current - center */
    g_object_get(current->data,
		 "audio-channel", &current_audio_channel,
		 NULL);
    
    if(current_audio_channel == audio_channel){
      if(timestamp == NULL){
	retval = current;
	
	break;
      }

      g_object_get(current->data,
		   "timestamp", &current_timestamp,
		   NULL);

      if(current_timestamp != NULL){
	if(use_ags_offset){
	  current_x = ags_timestamp_get_ags_offset(current_timestamp);

	  g_object_unref(current_timestamp);

	  if(current_x >= x &&
	     current_x < x + AGS_MIDI_DEFAULT_OFFSET){
	    retval = current;
	    
	    break;
	  }
	}else{
	  current_x = ags_timestamp_get_unix_time(current_timestamp);
	  
	  g_object_unref(current_timestamp);

	  if(current_x >= x &&
	     current_x < x + AGS_MIDI_DEFAULT_DURATION){
	    retval = current;
	    
	    break;
	  }
	}
      }else{
	g_warning("inconsistent data");
      }
    }
    
    if(position == 0){
      break;
    }

    position = position / 2;

    if(current_x < x){
      current_start = current->next;
      current_end = current_end->prev;
    }else{
      current_start = current_start->next;
      current_end = current->prev;
    }    

    current = g_list_nth(current_start,
			 position);
  }

  return(retval);
}

/**
 * ags_midi_add:
 * @midi: the #GList-struct containing #AgsMidi
 * @new_midi: the midi to add
 * 
 * Add @new_midi sorted to @midi
 * 
 * Returns: the new beginning of @midi
 * 
 * Since: 2.0.0
 */
GList*
ags_midi_add(GList *midi,
	     AgsMidi *new_midi)
{
  auto gint ags_midi_add_compare(gconstpointer a,
				 gconstpointer b);
  
  gint ags_midi_add_compare(gconstpointer a,
			    gconstpointer b)
  {
    AgsTimestamp *timestamp_a, *timestamp_b;

    guint64 offset_a, offset_b;

    g_object_get(a,
		 "timestamp", &timestamp_a,
		 NULL);

    g_object_get(b,
		 "timestamp", &timestamp_b,
		 NULL);
    
    offset_a = ags_timestamp_get_ags_offset(timestamp_a);
    offset_b = ags_timestamp_get_ags_offset(timestamp_b);

    g_object_unref(timestamp_a);
    g_object_unref(timestamp_b);
    
    if(offset_a == offset_b){
      return(0);
    }else if(offset_a < offset_b){
      return(-1);
    }else if(offset_a > offset_b){
      return(1);
    }

    return(0);
  }
  
  if(!AGS_IS_MIDI(new_midi)){
    return(midi);
  }
  
  midi = g_list_insert_sorted(midi,
			      new_midi,
			      ags_midi_add_compare);
  
  return(midi);
}

/**
 * ags_midi_add_track:
 * @midi: an #AgsMidi
 * @track: the #AgsTrack to add
 * @use_selection_list: if %TRUE add to selection, else to default midi
 *
 * Adds a track to midi.
 *
 * Since: 2.0.0
 */
void
ags_midi_add_track(AgsMidi *midi,
		    AgsTrack *track,
		    gboolean use_selection_list)
{
  pthread_mutex_t *midi_mutex;

  if(!AGS_IS_MIDI(midi) ||
     !AGS_IS_TRACK(track)){
    return;
  }

  /* get midi mutex */
  pthread_mutex_lock(ags_midi_get_class_mutex());
  
  midi_mutex = midi->obj_mutex;
  
  pthread_mutex_unlock(ags_midi_get_class_mutex());

  /* insert sorted */
  g_object_ref(track);
  
  pthread_mutex_lock(midi_mutex);

  if(use_selection_list){
    midi->selection = g_list_insert_sorted(midi->selection,
					   track,
					   (GCompareFunc) ags_track_sort_func);
    ags_track_set_flags(track,
			 AGS_TRACK_IS_SELECTED);
  }else{
    midi->track = g_list_insert_sorted(midi->track,
					track,
					(GCompareFunc) ags_track_sort_func);
  }

  pthread_mutex_unlock(midi_mutex);
}

/**
 * ags_midi_remove_track:
 * @midi: an #AgsMidi
 * @track: the #AgsTrack to remove
 * @use_selection_list: if %TRUE remove from selection, else from default midi
 *
 * Removes a track from midi.
 *
 * Since: 2.0.0
 */
void
ags_midi_remove_track(AgsMidi *midi,
		       AgsTrack *track,
		       gboolean use_selection_list)
{
  pthread_mutex_t *midi_mutex;

  if(!AGS_IS_MIDI(midi) ||
     !AGS_IS_TRACK(track)){
    return;
  }

  /* get midi mutex */
  pthread_mutex_lock(ags_midi_get_class_mutex());
  
  midi_mutex = midi->obj_mutex;
  
  pthread_mutex_unlock(ags_midi_get_class_mutex());

  /* remove if found */
  pthread_mutex_lock(midi_mutex);
  
  if(!use_selection_list){
    if(g_list_find(midi->track,
		   track) != NULL){
      midi->track = g_list_remove(midi->track,
				   track);
      g_object_unref(track);
    }
  }else{
    if(g_list_find(midi->selection,
		   track) != NULL){
      midi->selection = g_list_remove(midi->selection,
				      track);
      g_object_unref(track);
    }
  }

  pthread_mutex_unlock(midi_mutex);
}

/**
 * ags_midi_new:
 * @audio: the assigned #AgsAudio
 * @audio_channel: the audio channel to be used
 *
 * Creates a new instance of #AgsMidi.
 *
 * Returns: the new #AgsMidi
 *
 * Since: 2.0.0
 */
AgsMidi*
ags_midi_new(GObject *audio,
	     guint audio_channel)
{
  AgsMidi *midi;
  
  midi = (AgsMidi *) g_object_new(AGS_TYPE_MIDI,
				  "audio", audio,
				  "audio-channel", audio_channel,
				  NULL);
  
  return(midi);
}
