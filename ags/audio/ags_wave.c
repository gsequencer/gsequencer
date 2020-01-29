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

#include <ags/audio/ags_wave.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_audio_buffer_util.h>

#include <ags/i18n.h>

#include <errno.h>

void ags_wave_class_init(AgsWaveClass *wave);
void ags_wave_init(AgsWave *wave);
void ags_wave_set_property(GObject *gobject,
			   guint prop_id,
			   const GValue *value,
			   GParamSpec *param_spec);
void ags_wave_get_property(GObject *gobject,
			   guint prop_id,
			   GValue *value,
			   GParamSpec *param_spec);
void ags_wave_dispose(GObject *gobject);
void ags_wave_finalize(GObject *gobject);
  
void ags_wave_insert_native_level_from_clipboard_version_1_4_0(AgsWave *wave,
							       xmlNode *root_node, char *version,
							       char *x_boundary,
							       gboolean reset_x_offset, guint64 x_offset,
							       gdouble delay, guint attack,
							       gboolean match_line, gboolean do_replace,
							       guint current_line,
							       guint64 relative_offset,
							       guint wave_samplerate,
							       guint wave_buffer_size,
							       guint wave_format,
							       gboolean match_timestamp);

void ags_wave_insert_native_level_from_clipboard(AgsWave *wave,
						 xmlNode *root_node, char *version,
						 char *x_boundary,
						 gboolean reset_x_offset, guint64 x_offset,
						 gdouble delay, guint attack,
						 gboolean match_line, gboolean do_replace);

/**
 * SECTION:ags_wave
 * @short_description: Wave class supporting selection and clipboard.
 * @title: AgsWave
 * @section_id:
 * @include: ags/audio/ags_wave.h
 *
 * #AgsWave acts as a container of #AgsBuffer.
 */

enum{
  PROP_0,
  PROP_AUDIO,
  PROP_LINE,
  PROP_SAMPLERATE,
  PROP_BUFFER_SIZE,
  PROP_FORMAT,
  PROP_TIMESTAMP,
  PROP_BUFFER,
};

static gpointer ags_wave_parent_class = NULL;

GType
ags_wave_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_wave = 0;

    static const GTypeInfo ags_wave_info = {
      sizeof(AgsWaveClass),
      NULL,
      NULL,
      (GClassInitFunc) ags_wave_class_init,
      NULL,
      NULL,
      sizeof(AgsWave),
      0,
      (GInstanceInitFunc) ags_wave_init,
    };

    ags_type_wave = g_type_register_static(G_TYPE_OBJECT,
					   "AgsWave",
					   &ags_wave_info,
					   0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_wave);
  }

  return g_define_type_id__volatile;
}

void 
ags_wave_class_init(AgsWaveClass *wave)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_wave_parent_class = g_type_class_peek_parent(wave);

  gobject = (GObjectClass *) wave;

  gobject->set_property = ags_wave_set_property;
  gobject->get_property = ags_wave_get_property;

  gobject->dispose = ags_wave_dispose;
  gobject->finalize = ags_wave_finalize;

  /* properties */
  /**
   * AgsWave:audio:
   *
   * The assigned #AgsAudio
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("audio",
				   i18n_pspec("audio of wave"),
				   i18n_pspec("The audio of wave"),
				   AGS_TYPE_AUDIO,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUDIO,
				  param_spec);

  /**
   * AgsWave:line:
   *
   * The wave's line.
   * 
   * Since: 3.0.0
   */
  param_spec =  g_param_spec_uint("line",
				  i18n_pspec("line of wave"),
				  i18n_pspec("The numerical line of wave"),
				  0,
				  G_MAXUINT32,
				  0,
				  G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_LINE,
				  param_spec);
  
  /**
   * AgsWave:samplerate:
   *
   * The audio buffer's samplerate.
   * 
   * Since: 3.0.0
   */
  param_spec =  g_param_spec_uint("samplerate",
				  i18n_pspec("samplerate of audio buffer"),
				  i18n_pspec("The samplerate of audio buffer"),
				  0,
				  G_MAXUINT32,
				  AGS_SOUNDCARD_DEFAULT_SAMPLERATE,
				  G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SAMPLERATE,
				  param_spec);

  /**
   * AgsWave:buffer-size:
   *
   * The audio buffer's buffer size.
   * 
   * Since: 3.0.0
   */
  param_spec =  g_param_spec_uint("buffer-size",
				  i18n_pspec("buffer size of audio buffer"),
				  i18n_pspec("The buffer size of audio buffer"),
				  0,
				  G_MAXUINT32,
				  AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE,
				  G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_BUFFER_SIZE,
				  param_spec);

  /**
   * AgsWave:format:
   *
   * The audio buffer's format.
   * 
   * Since: 3.0.0
   */
  param_spec =  g_param_spec_uint("format",
				  i18n_pspec("format of audio buffer"),
				  i18n_pspec("The format of audio buffer"),
				  0,
				  G_MAXUINT32,
				  AGS_SOUNDCARD_DEFAULT_FORMAT,
				  G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FORMAT,
				  param_spec);

  /**
   * AgsWave:timestamp:
   *
   * The pattern's timestamp.
   * 
   * Since: 3.0.0
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
   * AgsWave:buffer: (type GList(AgsBuffer)) (transfer full)
   *
   * The assigned #AgsBuffer
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_pointer("buffer",
				    i18n_pspec("buffer of wave"),
				    i18n_pspec("The buffer of wave"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_BUFFER,
				  param_spec);
}

void
ags_wave_init(AgsWave *wave)
{
  AgsConfig *config;

  wave->flags = 0;

  /* wave mutex */
  g_rec_mutex_init(&(wave->obj_mutex));

  /* config */
  config = ags_config_get_instance();

  /* fields */  
  wave->audio = NULL;
  wave->line = 0;

  wave->samplerate = (guint) ags_soundcard_helper_config_get_samplerate(config);
  wave->buffer_size = (guint) ags_soundcard_helper_config_get_buffer_size(config);
  wave->format = (guint) ags_soundcard_helper_config_get_format(config);
  
  wave->timestamp = ags_timestamp_new();

  wave->timestamp->flags &= (~AGS_TIMESTAMP_UNIX);
  wave->timestamp->flags |= AGS_TIMESTAMP_OFFSET;

  wave->timestamp->timer.ags_offset.offset = 0;

  g_object_ref(wave->timestamp);

  wave->buffer = NULL;
  wave->selection = NULL;
}

void
ags_wave_set_property(GObject *gobject,
		      guint prop_id,
		      const GValue *value,
		      GParamSpec *param_spec)
{
  AgsWave *wave;

  GRecMutex *wave_mutex;

  wave = AGS_WAVE(gobject);

  /* get wave mutex */
  wave_mutex = AGS_WAVE_GET_OBJ_MUTEX(wave);

  switch(prop_id){
  case PROP_AUDIO:
    {
      AgsAudio *audio;

      audio = (AgsAudio *) g_value_get_object(value);

      g_rec_mutex_lock(wave_mutex);

      if(wave->audio == (GObject *) audio){
	g_rec_mutex_unlock(wave_mutex);
	
	return;
      }

      if(wave->audio != NULL){
	g_object_unref(wave->audio);
      }

      if(audio != NULL){
	g_object_ref(audio);
      }

      wave->audio = (GObject *) audio;

      g_rec_mutex_unlock(wave_mutex);
    }
    break;
  case PROP_LINE:
    {
      guint line;

      line = g_value_get_uint(value);

      g_rec_mutex_lock(wave_mutex);

      wave->line = line;

      g_rec_mutex_unlock(wave_mutex);
    }
    break;
  case PROP_SAMPLERATE:
    {
      guint samplerate;

      samplerate = g_value_get_uint(value);

      ags_wave_set_samplerate(wave,
			      samplerate);
    }
    break;
  case PROP_BUFFER_SIZE:
    {
      guint buffer_size;

      buffer_size = g_value_get_uint(value);

      ags_wave_set_buffer_size(wave,
			       buffer_size);
    }
    break;
  case PROP_FORMAT:
    {
      guint format;

      format = g_value_get_uint(value);

      ags_wave_set_format(wave,
			  format);
    }
    break;
  case PROP_TIMESTAMP:
    {
      AgsTimestamp *timestamp;

      timestamp = (AgsTimestamp *) g_value_get_object(value);

      g_rec_mutex_lock(wave_mutex);

      if(timestamp == wave->timestamp){
	g_rec_mutex_unlock(wave_mutex);
	
	return;
      }

      if(wave->timestamp != NULL){
	g_object_unref(G_OBJECT(wave->timestamp));
      }

      if(timestamp != NULL){
	g_object_ref(G_OBJECT(timestamp));
      }

      wave->timestamp = timestamp;

      g_rec_mutex_unlock(wave_mutex);
    }
    break;
  case PROP_BUFFER:
    {
      AgsBuffer *buffer;

      buffer = (AgsBuffer *) g_value_get_pointer(value);

      g_rec_mutex_lock(wave_mutex);

      if(buffer == NULL ||
	 g_list_find(wave->buffer, buffer) != NULL){
	g_rec_mutex_unlock(wave_mutex);

	return;
      }

      g_rec_mutex_unlock(wave_mutex);

      ags_wave_add_buffer(wave,
			  buffer,
			  FALSE);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_wave_get_property(GObject *gobject,
		      guint prop_id,
		      GValue *value,
		      GParamSpec *param_spec)
{
  AgsWave *wave;

  GRecMutex *wave_mutex;

  wave = AGS_WAVE(gobject);

  /* get wave mutex */
  wave_mutex = AGS_WAVE_GET_OBJ_MUTEX(wave);

  switch(prop_id){
  case PROP_AUDIO:
    {
      g_rec_mutex_lock(wave_mutex);

      g_value_set_object(value, wave->audio);

      g_rec_mutex_unlock(wave_mutex);
    }
    break;
  case PROP_LINE:
    {
      g_rec_mutex_lock(wave_mutex);

      g_value_set_uint(value, wave->line);

      g_rec_mutex_unlock(wave_mutex);
    }
    break;
  case PROP_SAMPLERATE:
    {
      g_rec_mutex_lock(wave_mutex);

      g_value_set_uint(value, wave->samplerate);

      g_rec_mutex_unlock(wave_mutex);
    }
    break;
  case PROP_BUFFER_SIZE:
    {
      g_rec_mutex_lock(wave_mutex);

      g_value_set_uint(value, wave->buffer_size);

      g_rec_mutex_unlock(wave_mutex);
    }
    break;
  case PROP_FORMAT:
    {
      g_rec_mutex_lock(wave_mutex);

      g_value_set_uint(value, wave->format);

      g_rec_mutex_unlock(wave_mutex);
    }
    break;
  case PROP_TIMESTAMP:
    {
      g_rec_mutex_lock(wave_mutex);

      g_value_set_object(value, wave->timestamp);

      g_rec_mutex_unlock(wave_mutex);
    }
    break;
  case PROP_BUFFER:
    {
      g_rec_mutex_lock(wave_mutex);

      g_value_set_pointer(value, g_list_copy_deep(wave->buffer,
						  (GCopyFunc) g_object_ref,
						  NULL));

      g_rec_mutex_unlock(wave_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_wave_dispose(GObject *gobject)
{
  AgsWave *wave;

  GList *list;
  
  wave = AGS_WAVE(gobject);

  /* audio */
  if(wave->audio != NULL){
    g_object_unref(wave->audio);

    wave->audio = NULL;
  }

  /* timestamp */
  if(wave->timestamp != NULL){
    g_object_unref(wave->timestamp);

    wave->timestamp = NULL;
  }

  /* buffer and selection */
  list = wave->buffer;

  while(list != NULL){
    g_object_run_dispose(G_OBJECT(list->data));
    
    list = list->next;
  }
  
  g_list_free_full(wave->buffer,
		   g_object_unref);
  g_list_free_full(wave->selection,
		   g_object_unref);

  wave->buffer = NULL;
  wave->selection = NULL;
    
  /* call parent */
  G_OBJECT_CLASS(ags_wave_parent_class)->dispose(gobject);
}

void
ags_wave_finalize(GObject *gobject)
{
  AgsWave *wave;

  wave = AGS_WAVE(gobject);

  /* audio */
  if(wave->audio != NULL){
    g_object_unref(wave->audio);
  }

  /* timestamp */
  if(wave->timestamp != NULL){
    g_object_unref(wave->timestamp);
  }
    
  /* buffer and selection */
  g_list_free_full(wave->buffer,
		   g_object_unref);

  g_list_free_full(wave->selection,
		   g_object_unref);
  
  /* call parent */
  G_OBJECT_CLASS(ags_wave_parent_class)->finalize(gobject);
}

/**
 * ags_wave_get_obj_mutex:
 * @wave: the #AgsWave
 * 
 * Get object mutex.
 * 
 * Returns: the #GRecMutex to lock @wave
 * 
 * Since: 3.1.0
 */
GRecMutex*
ags_wave_get_obj_mutex(AgsWave *wave)
{
  if(!AGS_IS_WAVE(wave)){
    return(NULL);
  }

  return(AGS_WAVE_GET_OBJ_MUTEX(wave));
}

/**
 * ags_wave_test_flags:
 * @wave: the #AgsWave
 * @flags: the flags
 * 
 * Test @flags to be set on @wave.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 * 
 * Since: 3.0.0
 */
gboolean
ags_wave_test_flags(AgsWave *wave, guint flags)
{
  gboolean retval;
  
  GRecMutex *wave_mutex;

  if(!AGS_IS_WAVE(wave)){
    return(FALSE);
  }
      
  /* get wave mutex */
  wave_mutex = AGS_WAVE_GET_OBJ_MUTEX(wave);

  /* test */
  g_rec_mutex_lock(wave_mutex);

  retval = (flags & (wave->flags)) ? TRUE: FALSE;
  
  g_rec_mutex_unlock(wave_mutex);

  return(retval);
}

/**
 * ags_wave_set_flags:
 * @wave: the #AgsWave
 * @flags: the flags
 * 
 * Set @flags on @wave.
 * 
 * Since: 3.0.0
 */
void
ags_wave_set_flags(AgsWave *wave, guint flags)
{
  GRecMutex *wave_mutex;

  if(!AGS_IS_WAVE(wave)){
    return;
  }
      
  /* get wave mutex */
  wave_mutex = AGS_WAVE_GET_OBJ_MUTEX(wave);

  /* set */
  g_rec_mutex_lock(wave_mutex);

  wave->flags |= flags;
  
  g_rec_mutex_unlock(wave_mutex);
}

/**
 * ags_wave_unset_flags:
 * @wave: the #AgsWave
 * @flags: the flags
 * 
 * Unset @flags on @wave.
 * 
 * Since: 3.0.0
 */
void
ags_wave_unset_flags(AgsWave *wave, guint flags)
{
  GRecMutex *wave_mutex;

  if(!AGS_IS_WAVE(wave)){
    return;
  }
      
  /* get wave mutex */
  wave_mutex = AGS_WAVE_GET_OBJ_MUTEX(wave);

  /* set */
  g_rec_mutex_lock(wave_mutex);

  wave->flags &= (~flags);
  
  g_rec_mutex_unlock(wave_mutex);
}

/**
 * ags_wave_find_near_timestamp:
 * @wave: (element-type AgsAudio.Wave) (transfer none): the #GList-struct containing #AgsWave
 * @line: the matching audio channel
 * @timestamp: the matching #AgsTimestamp, or %NULL to match any timestamp
 *
 * Retrieve appropriate wave for timestamp.
 *
 * Returns: (element-type AgsAudio.Wave) (transfer none): Next matching #GList-struct or %NULL if not found
 *
 * Since: 3.0.0
 */
GList*
ags_wave_find_near_timestamp(GList *wave, guint line,
			     AgsTimestamp *timestamp)
{
  AgsTimestamp *current_timestamp;

  GList *retval;
  GList *current_start, *current_end, *current;

  guint current_line;
  guint64 current_x, x;
  guint length, position;
  gboolean use_ags_offset;
  gboolean success;

  if(wave == NULL){
    return(NULL);
  }

  current_start = wave;
  current_end = g_list_last(wave);
  
  length = g_list_length(wave);
  position = (length - 1) / 2;

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

  current_x = 0;
  
  retval = NULL;
  success = FALSE;
  
  while(!success && current != NULL){
    guint64 relative_offset;
    guint samplerate;

    /* check current - start */
    g_object_get(current_start->data,
		 "line", &current_line,
		 NULL);

    if(current_line == line){
      if(timestamp == NULL){
	retval = current_start;
	
	break;
      }

      g_object_get(current_start->data,
		   "samplerate", &samplerate,
		   "timestamp", &current_timestamp,
		   NULL);

      relative_offset = AGS_WAVE_DEFAULT_BUFFER_LENGTH * samplerate;

      if(current_timestamp != NULL){
	if(use_ags_offset){
	  current_x = ags_timestamp_get_ags_offset(current_timestamp);
	  
	  g_object_unref(current_timestamp);

	  if(current_x > x + relative_offset){
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
	     current_x < x + relative_offset){
	    retval = current_start;
	    
	    break;
	  }
	}else{
	  if(current_x >= x &&
	     current_x < x + AGS_WAVE_DEFAULT_DURATION){
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
		 "line", &current_line,
		 NULL);
    
    if(current_line == line){
      if(timestamp == NULL){
	retval = current_end;
	
	break;
      }

      g_object_get(current_end->data,
		   "samplerate", &samplerate,
		   "timestamp", &current_timestamp,
		   NULL);

      relative_offset = AGS_WAVE_DEFAULT_BUFFER_LENGTH * samplerate;
	  
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
	     current_x < x + relative_offset){
	    retval = current_end;

	    break;
	  }
	}else{
	  if(current_x >= x &&
	     current_x < x + AGS_WAVE_DEFAULT_DURATION){
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
		 "line", &current_line,
		 NULL);

    if(current_line == line){
      if(timestamp == NULL){
	retval = current;
	
	break;
      }
    }
    
    g_object_get(current->data,
		 "samplerate", &samplerate,
		 "timestamp", &current_timestamp,
		 NULL);

    relative_offset = AGS_WAVE_DEFAULT_BUFFER_LENGTH * samplerate;

    if(current_timestamp != NULL){
      if(use_ags_offset){
	current_x = ags_timestamp_get_ags_offset(current_timestamp);

	g_object_unref(current_timestamp);
    
	if(current_x >= x &&
	   current_x < x + relative_offset &&
	   current_line == line){
	  retval = current;

	  break;
	}
      }else{
	current_x = ags_timestamp_get_unix_time(current_timestamp);
	  
	g_object_unref(current_timestamp);

	if(current_x >= x &&
	   current_x < x + AGS_WAVE_DEFAULT_DURATION &&
	   current_line == line){
	  retval = current;
	    
	  break;
	}
      }
    }else{
      g_warning("inconsistent data");
    }
    
    if(length <= 3){
      break;
    }

    if(current_x < x){
      current_start = current->next;
      current_end = current_end->prev;
    }else if(current_x > x){
      current_start = current_start->next;
      current_end = current->prev;
    }else{
      current_start = current_start->next;
      //NOTE:JK: we want progression
      //current_end = current_end->prev;
    }

    length = g_list_position(current_start,
			     current_end) + 1;
    position = (length - 1) / 2;

    current = g_list_nth(current_start,
			 position);
  }

  return(retval);
}

/**
 * ags_wave_sort_func:
 * @a: the #AgsWave
 * @b: another #AgsWave
 * 
 * Compare @a and @b.
 * 
 * Returns: 0 if equal, -1 if smaller and 1 if bigger timestamp
 * 
 * Since: 3.0.0
 */
gint
ags_wave_sort_func(gconstpointer a,
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

/**
 * ags_wave_get_audio:
 * @wave: the #AgsWave
 * 
 * Get audio.
 * 
 * Returns: (transfer full): the #AgsAudio
 * 
 * Since: 3.1.0
 */
GObject*
ags_wave_get_audio(AgsWave *wave)
{
  GObject *audio;

  if(!AGS_IS_WAVE(wave)){
    return(NULL);
  }

  g_object_get(wave,
	       "audio", &audio,
	       NULL);

  return(audio);
}

/**
 * ags_wave_set_audio:
 * @wave: the #AgsWave
 * @audio: the #AgsAudio
 * 
 * Set audio.
 * 
 * Since: 3.1.0
 */
void
ags_wave_set_audio(AgsWave *wave, GObject *audio)
{
  if(!AGS_IS_WAVE(wave)){
    return;
  }

  g_object_set(wave,
	       "audio", audio,
	       NULL);
}

/**
 * ags_wave_get_line:
 * @wave: the #AgsWave
 *
 * Gets line.
 * 
 * Returns: the line
 * 
 * Since: 3.1.0
 */
guint
ags_wave_get_line(AgsWave *wave)
{
  guint line;
  
  if(!AGS_IS_WAVE(wave)){
    return(0);
  }

  g_object_get(wave,
	       "line", &line,
	       NULL);

  return(line);
}

/**
 * ags_wave_set_line:
 * @wave: the #AgsWave
 * @line: the line
 *
 * Sets line.
 * 
 * Since: 3.1.0
 */
void
ags_wave_set_line(AgsWave *wave, guint line)
{
  if(!AGS_IS_WAVE(wave)){
    return;
  }

  g_object_set(wave,
	       "line", line,
	       NULL);
}

/**
 * ags_wave_get_samplerate:
 * @wave: the #AgsWave
 *
 * Gets samplerate.
 * 
 * Returns: the samplerate
 * 
 * Since: 3.1.0
 */
guint
ags_wave_get_samplerate(AgsWave *wave)
{
  guint samplerate;
  
  if(!AGS_IS_WAVE(wave)){
    return(0);
  }

  g_object_get(wave,
	       "samplerate", &samplerate,
	       NULL);

  return(samplerate);
}

/**
 * ags_wave_set_samplerate:
 * @wave: the #AgsWave
 * @samplerate: the samplerate
 * 
 * Set samplerate. 
 * 
 * Since: 3.0.0
 */
void
ags_wave_set_samplerate(AgsWave *wave,
			guint samplerate)
{
  GList *start_list, *list;

  void *data, *resampled_data;

  guint64 x;
  guint end_offset;
  guint buffer_length;
  guint new_buffer_length;
  guint buffer_size;
  guint old_samplerate;
  guint format;
  guint offset;
  guint copy_mode;
  guint i;    

  GRecMutex *wave_mutex;

  if(!AGS_IS_WAVE(wave)){
    return;
  }
  
  /* get wave mutex */
  wave_mutex = AGS_WAVE_GET_OBJ_MUTEX(wave);

  /* check resample */
  g_rec_mutex_lock(wave_mutex);

  old_samplerate = wave->samplerate;

  g_rec_mutex_unlock(wave_mutex);

  if(old_samplerate == samplerate){
    return;
  }

  /* apply samplerate */
  g_rec_mutex_lock(wave_mutex);

  x = ags_timestamp_get_ags_offset(wave->timestamp);

  buffer_size = wave->buffer_size;
  format = wave->format;
  
  wave->samplerate = samplerate;

  start_list = g_list_copy(wave->buffer);
  
  g_rec_mutex_unlock(wave_mutex);

  data = NULL;

  buffer_length = g_list_length(start_list);

  new_buffer_length = (guint) ceil((samplerate * (buffer_length * buffer_size / old_samplerate)) / buffer_size);

  copy_mode = G_MAXUINT;
  
  switch(format){
  case AGS_SOUNDCARD_SIGNED_8_BIT:
    {
      data = (gint8 *) malloc(buffer_length * buffer_size * sizeof(gint8));
      memset(data, 0, buffer_length * buffer_size * sizeof(gint8));
      
      copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_S8_TO_S8;
    }
    break;
  case AGS_SOUNDCARD_SIGNED_16_BIT:
    {
      data = (gint16 *) malloc(buffer_length * buffer_size * sizeof(gint16));
      memset(data, 0, buffer_length * buffer_size * sizeof(gint16));

      copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_S16_TO_S16;
    }
    break;
  case AGS_SOUNDCARD_SIGNED_24_BIT:
    {
      data = (gint32 *) malloc(buffer_length * buffer_size * sizeof(gint32));
      memset(data, 0, buffer_length * buffer_size * sizeof(gint32));

      copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_S32_TO_S32;
    }
    break;
  case AGS_SOUNDCARD_SIGNED_32_BIT:
    {
      data = (gint32 *) malloc(buffer_length * buffer_size * sizeof(gint32));
      memset(data, 0, buffer_length * buffer_size * sizeof(gint32));

      copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_S32_TO_S32;
    }
    break;
  case AGS_SOUNDCARD_SIGNED_64_BIT:
    {
      data = (gint64 *) malloc(buffer_length * buffer_size * sizeof(gint64));
      memset(data, 0, buffer_length * buffer_size * sizeof(gint64));

      copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_S64_TO_S64;
    }
    break;
  case AGS_SOUNDCARD_FLOAT:
    {
      data = (gfloat *) malloc(buffer_length * buffer_size * sizeof(gfloat));
      memset(data, 0, buffer_length * buffer_size * sizeof(gfloat));

      copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT_TO_FLOAT;
    }
    break;
  case AGS_SOUNDCARD_DOUBLE:
    {
      data = (gdouble *) malloc(buffer_length * buffer_size * sizeof(gdouble));
      memset(data, 0, buffer_length * buffer_size * sizeof(gdouble));

      copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_DOUBLE_TO_DOUBLE;
    }
    break;
  default:
    g_warning("ags_audio_signal_set_buffer_samplerate() - unsupported format");
  }

  list = start_list;
  offset = 0;
  
  while(list != NULL){
    GRecMutex *buffer_mutex;
  
    /* get buffer mutex */
    buffer_mutex = AGS_BUFFER_GET_OBJ_MUTEX(list->data);

    /*  */
    g_rec_mutex_lock(buffer_mutex);
    
    ags_audio_buffer_util_copy_buffer_to_buffer(data, 1, offset,
						AGS_BUFFER(list->data)->data, 1, 0,
						buffer_size, copy_mode);

    g_rec_mutex_unlock(buffer_mutex);

    /* iterate */
    list = list->next;

    offset += buffer_size;
  }

  resampled_data = ags_stream_alloc(new_buffer_length * buffer_size,
				    format);
  
  ags_audio_buffer_util_resample_with_buffer(data, 1,
					     ags_audio_buffer_util_format_from_soundcard(format), old_samplerate,
					     buffer_length * buffer_size,
					     samplerate,
					     new_buffer_length * buffer_size,
					     resampled_data);

  if(data != NULL){
    free(data);
  }
    
  if(samplerate < old_samplerate){
    list = g_list_nth(start_list,
		      new_buffer_length);
    
    for(i = 0; i < buffer_length - new_buffer_length && list != NULL; i++){
      ags_wave_remove_buffer(wave,
			     list->data,
			     FALSE);

      list = list->next;
    }
  }else{
    for(i = 0; i < new_buffer_length - buffer_length; i++){
      AgsBuffer *current;

      current = ags_buffer_new();
      g_object_set(current,
		   "x", x + i * buffer_size,
		   NULL);
      ags_buffer_set_buffer_size(current,
				 buffer_size);
      
      ags_wave_add_buffer(wave,
			  current,
			  FALSE);
    }
  }

  g_list_free(start_list);

  g_object_get(wave,
	       "buffer", &start_list,
	       NULL);
  
  list = start_list;

  offset = 0;
  end_offset = (buffer_length * buffer_size);    
  
  while(list != NULL && offset < buffer_length * buffer_size){
    GRecMutex *buffer_mutex;
  
    /* get buffer mutex */
    buffer_mutex = AGS_BUFFER_GET_OBJ_MUTEX(list->data);

    /*  */
    ags_buffer_set_samplerate(list->data,
			      samplerate);

    g_rec_mutex_lock(buffer_mutex);
    
    ags_audio_buffer_util_clear_buffer(AGS_BUFFER(list->data)->data, 1,
				       buffer_size, ags_audio_buffer_util_format_from_soundcard(format));

    if(offset + buffer_size < buffer_length * buffer_size){
      ags_audio_buffer_util_copy_buffer_to_buffer(AGS_BUFFER(list->data)->data, 1, 0,
						  resampled_data, 1, offset,
						  buffer_size, copy_mode);
    }else{
      if(end_offset > offset){
	ags_audio_buffer_util_copy_buffer_to_buffer(AGS_BUFFER(list->data)->data, 1, 0,
						    resampled_data, 1, offset,
						    end_offset - offset, copy_mode);

	ags_audio_buffer_util_clear_buffer(AGS_BUFFER(list->data)->data + (end_offset - offset), 1,
					   buffer_size - (end_offset - offset), ags_audio_buffer_util_format_from_soundcard(format));
      }
    }
    
    g_rec_mutex_unlock(buffer_mutex);

    /* iterate */
    list = list->next;

    offset += buffer_size;
  }

  g_list_free_full(start_list,
		   g_object_unref);

  if(resampled_data != NULL){
    free(resampled_data);
  }
}

/**
 * ags_wave_get_buffer_size:
 * @wave: the #AgsWave
 *
 * Gets buffer size.
 * 
 * Returns: the buffer size
 * 
 * Since: 3.1.0
 */
guint
ags_wave_get_buffer_size(AgsWave *wave)
{
  guint buffer_size;
  
  if(!AGS_IS_WAVE(wave)){
    return(0);
  }

  g_object_get(wave,
	       "buffer-size", &buffer_size,
	       NULL);

  return(buffer_size);
}

/**
 * ags_wave_set_buffer_size:
 * @wave: the #AgsWave
 * @buffer_size: the buffer size
 * 
 * Set buffer size.
 * 
 * Since: 3.0.0
 */
void
ags_wave_set_buffer_size(AgsWave *wave,
			 guint buffer_size)
{
  GList *start_list, *list;

  void *data;

  guint64 x;
  guint end_offset;
  guint buffer_length;
  guint new_buffer_length;
  guint offset;
  guint format;
  guint old_buffer_size;
  guint word_size;
  guint copy_mode;
  guint i;
  
  GRecMutex *wave_mutex;

  if(!AGS_IS_WAVE(wave)){
    return;
  }
  
  /* get wave mutex */
  wave_mutex = AGS_WAVE_GET_OBJ_MUTEX(wave);

  g_rec_mutex_lock(wave_mutex);

  old_buffer_size = wave->buffer_size;

  g_rec_mutex_unlock(wave_mutex);

  if(buffer_size == old_buffer_size){
    return;
  }
  
  /* apply buffer size */
  g_rec_mutex_lock(wave_mutex);

  x = ags_timestamp_get_ags_offset(wave->timestamp);

  format = wave->format;
  
  wave->buffer_size = buffer_size;

  start_list = g_list_copy(wave->buffer);
  
  g_rec_mutex_unlock(wave_mutex);

  /* resize buffer */
  data = NULL;
  
  buffer_length = g_list_length(start_list);
  word_size = 1;  

  copy_mode = G_MAXUINT;
  
  switch(format){
  case AGS_SOUNDCARD_SIGNED_8_BIT:
    {
      data = (gint8 *) malloc(buffer_length * old_buffer_size * sizeof(gint8));
      memset(data, 0, buffer_length * old_buffer_size * sizeof(gint8));
      
      copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_S8_TO_S8;
    }
    break;
  case AGS_SOUNDCARD_SIGNED_16_BIT:
    {
      data = (gint16 *) malloc(buffer_length * old_buffer_size * sizeof(gint16));
      memset(data, 0, buffer_length * old_buffer_size * sizeof(gint16));

      copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_S16_TO_S16;
    }
    break;
  case AGS_SOUNDCARD_SIGNED_24_BIT:
    {
      data = (gint32 *) malloc(buffer_length * old_buffer_size * sizeof(gint32));
      memset(data, 0, buffer_length * old_buffer_size * sizeof(gint32));

      copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_S32_TO_S32;
    }
    break;
  case AGS_SOUNDCARD_SIGNED_32_BIT:
    {
      data = (gint32 *) malloc(buffer_length * old_buffer_size * sizeof(gint32));
      memset(data, 0, buffer_length * old_buffer_size * sizeof(gint32));

      copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_S32_TO_S32;
    }
    break;
  case AGS_SOUNDCARD_SIGNED_64_BIT:
    {
      data = (gint64 *) malloc(buffer_length * old_buffer_size * sizeof(gint64));
      memset(data, 0, buffer_length * old_buffer_size * sizeof(gint64));

      copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_S64_TO_S64;
    }
    break;
  case AGS_SOUNDCARD_FLOAT:
    {
      data = (gfloat *) malloc(buffer_length * old_buffer_size * sizeof(gfloat));
      memset(data, 0, buffer_length * old_buffer_size * sizeof(gfloat));

      copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT_TO_FLOAT;
    }
    break;
  case AGS_SOUNDCARD_DOUBLE:
    {
      data = (gdouble *) malloc(buffer_length * old_buffer_size * sizeof(gdouble));
      memset(data, 0, buffer_length * old_buffer_size * sizeof(gdouble));

      copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_DOUBLE_TO_DOUBLE;
    }
    break;
  default:
    g_warning("ags_wave_set_buffer_size() - unsupported format");
  }

  list = start_list;

  offset = 0;
  
  while(list != NULL){
    GRecMutex *buffer_mutex;
  
    /* get buffer mutex */
    buffer_mutex = AGS_BUFFER_GET_OBJ_MUTEX(list->data);

    /*  */
    g_rec_mutex_lock(buffer_mutex);
    
    ags_audio_buffer_util_copy_buffer_to_buffer(data, 1, offset,
						AGS_BUFFER(list->data)->data, 1, 0,
						old_buffer_size, copy_mode);

    g_rec_mutex_unlock(buffer_mutex);

    /* iterate */
    list = list->next;

    offset += old_buffer_size;
  }

  new_buffer_length = (guint) ceil((buffer_length * old_buffer_size) / buffer_size);

  if(old_buffer_size < buffer_size){
    list = g_list_nth(start_list,
		      new_buffer_length);
    
    for(i = 0; i < buffer_length - new_buffer_length && list != NULL; i++){
      ags_wave_remove_buffer(wave,
			     list->data,
			     FALSE);

      list = list->next;
    }
  }else{
    for(i = 0; i < new_buffer_length - buffer_length; i++){
      AgsBuffer *current;

      current = ags_buffer_new();
      g_object_set(current,
		   "x", x + i * buffer_size,
		   NULL);
      ags_buffer_set_buffer_size(current,
				 buffer_size);
      
      ags_wave_add_buffer(wave,
			  current,
			  FALSE);
    }
  }

  g_list_free(start_list);

  g_object_get(wave,
	       "buffer", &start_list,
	       NULL);
  
  list = start_list;

  offset = 0;
  end_offset = (buffer_length * buffer_size);    
  
  while(list != NULL){
    GRecMutex *buffer_mutex;
  
    /* get buffer mutex */
    buffer_mutex = AGS_BUFFER_GET_OBJ_MUTEX(list->data);

    /*  */
    g_object_set(list->data,
		 "x", x + offset,
		 NULL);
    ags_buffer_set_buffer_size(list->data,
			       buffer_size);

    g_rec_mutex_lock(buffer_mutex);

    ags_audio_buffer_util_clear_buffer(AGS_BUFFER(list->data)->data, 1,
				       buffer_size, ags_audio_buffer_util_format_from_soundcard(format));
    
    if(offset + buffer_size < buffer_length * buffer_size){
      ags_audio_buffer_util_copy_buffer_to_buffer(AGS_BUFFER(list->data)->data, 1, 0,
						  data, 1, offset,
						  buffer_size, copy_mode);
    }else{
      if(end_offset > offset){
	ags_audio_buffer_util_copy_buffer_to_buffer(AGS_BUFFER(list->data)->data, 1, 0,
						    data, 1, offset,
						    end_offset - offset, copy_mode);
      }
    }
    
    g_rec_mutex_unlock(buffer_mutex);

    /* iterate */
    list = list->next;

    offset += buffer_size;
  }

  g_list_free_full(start_list,
		   g_object_unref);

  if(data != NULL){
    free(data);
  }
}

/**
 * ags_wave_get_format:
 * @wave: the #AgsWave
 *
 * Gets format.
 * 
 * Returns: the format
 * 
 * Since: 3.1.0
 */
guint
ags_wave_get_format(AgsWave *wave)
{
  guint format;
  
  if(!AGS_IS_WAVE(wave)){
    return(0);
  }

  g_object_get(wave,
	       "format", &format,
	       NULL);

  return(format);
}

/**
 * ags_wave_set_format:
 * @wave: the #AgsWave
 * @format: the format
 * 
 * Set format. 
 * 
 * Since: 3.0.0
 */
void
ags_wave_set_format(AgsWave *wave,
		    guint format)
{
  GList *list_start, *list;

  GRecMutex *wave_mutex;

  if(!AGS_IS_WAVE(wave)){
    return;
  }
  
  /* get wave mutex */
  wave_mutex = AGS_WAVE_GET_OBJ_MUTEX(wave);

  /* apply format */
  g_rec_mutex_lock(wave_mutex);
  
  wave->format = format;

  list =
    list_start = g_list_copy(wave->buffer);
  
  g_rec_mutex_unlock(wave_mutex);
  
  while(list != NULL){
    ags_buffer_set_format(list->data,
			  format);

    list = list->next;
  }

  g_list_free(list_start);
}

/**
 * ags_wave_get_timestamp:
 * @wave: the #AgsWave
 * 
 * Get timestamp.
 * 
 * Returns: (transfer full): the #AgsTimestamp
 * 
 * Since: 3.1.0
 */
AgsTimestamp*
ags_wave_get_timestamp(AgsWave *wave)
{
  AgsTimestamp *timestamp;

  if(!AGS_IS_WAVE(wave)){
    return(NULL);
  }

  g_object_get(wave,
	       "timestamp", &timestamp,
	       NULL);

  return(timestamp);
}

/**
 * ags_wave_set_timestamp:
 * @wave: the #AgsWave
 * @timestamp: the #AgsTimestamp
 * 
 * Set timestamp.
 * 
 * Since: 3.1.0
 */
void
ags_wave_set_timestamp(AgsWave *wave, AgsTimestamp *timestamp)
{
  if(!AGS_IS_WAVE(wave)){
    return;
  }

  g_object_set(wave,
	       "timestamp", timestamp,
	       NULL);
}

/**
 * ags_wave_get_buffer:
 * @wave: the #AgsWave
 * 
 * Get buffer.
 * 
 * Returns: (element-type AgsAudio.Buffer) (transfer full): the #GList-struct containig #AgsBuffer
 * 
 * Since: 3.1.0
 */
GList*
ags_wave_get_buffer(AgsWave *wave)
{
  GList *buffer;

  if(!AGS_IS_WAVE(wave)){
    return(NULL);
  }

  g_object_get(wave,
	       "buffer", &buffer,
	       NULL);

  return(buffer);
}

/**
 * ags_wave_set_buffer:
 * @wave: the #AgsWave
 * @buffer: (element-type AgsAudio.Buffer) (transfer full): the #GList-struct containing #AgsBuffer
 * 
 * Set buffer by replacing existing.
 * 
 * Since: 3.1.0
 */
void
ags_wave_set_buffer(AgsWave *wave, GList *buffer)
{
  GList *start_buffer;
  
  GRecMutex *wave_mutex;

  if(!AGS_IS_WAVE(wave)){
    return;
  }

  /* get wave mutex */
  wave_mutex = AGS_WAVE_GET_OBJ_MUTEX(wave);
    
  g_rec_mutex_lock(wave_mutex);

  start_buffer = wave->buffer;
  wave->buffer = buffer;
  
  g_rec_mutex_unlock(wave_mutex);

  g_list_free_full(start_buffer,
		   (GDestroyNotify) g_object_unref);
}

/**
 * ags_wave_add:
 * @wave: (element-type AgsAudio.Wave) (transfer none): the #GList-struct containing #AgsWave
 * @new_wave: the #AgsWave to add
 * 
 * Add @new_wave sorted to @wave
 * 
 * Returns: (element-type AgsAudio.Wave) (transfer none): the new beginning of @wave
 * 
 * Since: 3.0.0
 */
GList*
ags_wave_add(GList *wave,
	     AgsWave *new_wave)
{
  
  if(!AGS_IS_WAVE(new_wave)){
    return(wave);
  }
  
  wave = g_list_insert_sorted(wave,
			      new_wave,
			      ags_wave_sort_func);
  
  return(wave);
}

/**
 * ags_wave_add_buffer:
 * @wave: the #AgsWave
 * @buffer: the #AgsBuffer to add
 * @use_selection_list: if %TRUE add to selection, else to default wave
 *
 * Add @buffer to @wave.
 *
 * Since: 3.0.0
 */
void
ags_wave_add_buffer(AgsWave *wave,
		    AgsBuffer *buffer,
		    gboolean use_selection_list)
{
  GRecMutex *wave_mutex;

  if(!AGS_IS_WAVE(wave) ||
     !AGS_IS_BUFFER(buffer)){
    return;
  }

  /* get wave mutex */
  wave_mutex = AGS_WAVE_GET_OBJ_MUTEX(wave);

  /* insert sorted */
  g_object_ref(buffer);
  
  g_rec_mutex_lock(wave_mutex);

  if(use_selection_list){
    wave->selection = g_list_insert_sorted(wave->selection,
					   buffer,
					   (GCompareFunc) ags_buffer_sort_func);
    ags_buffer_set_flags(buffer,
			 AGS_BUFFER_IS_SELECTED);
  }else{
    wave->buffer = g_list_insert_sorted(wave->buffer,
					buffer,
					(GCompareFunc) ags_buffer_sort_func);
  }

  g_rec_mutex_unlock(wave_mutex);
}

/**
 * ags_wave_remove_buffer:
 * @wave: the #AgsWave
 * @buffer: the #AgsBuffer to remove
 * @use_selection_list: if %TRUE remove from selection, else from default wave
 *
 * Removes @buffer from @wave.
 *
 * Since: 3.0.0
 */
void
ags_wave_remove_buffer(AgsWave *wave,
		       AgsBuffer *buffer,
		       gboolean use_selection_list)
{
  GRecMutex *wave_mutex;

  if(!AGS_IS_WAVE(wave) ||
     !AGS_IS_BUFFER(buffer)){
    return;
  }

  /* get wave mutex */
  wave_mutex = AGS_WAVE_GET_OBJ_MUTEX(wave);

  /* remove if found */
  g_rec_mutex_lock(wave_mutex);
  
  if(!use_selection_list){
    if(g_list_find(wave->buffer,
		   buffer) != NULL){
      wave->buffer = g_list_remove(wave->buffer,
				   buffer);
      g_object_unref(buffer);
    }
  }else{
    if(g_list_find(wave->selection,
		   buffer) != NULL){
      wave->selection = g_list_remove(wave->selection,
				      buffer);
      g_object_unref(buffer);
    }
  }

  g_rec_mutex_unlock(wave_mutex);
}

/**
 * ags_wave_get_selection:
 * @wave: the #AgsWave
 *
 * Retrieve selection.
 *
 * Returns: (element-type AgsAudio.Buffer) (transfer none): the selection.
 *
 * Since: 3.0.0
 */
GList*
ags_wave_get_selection(AgsWave *wave)
{
  GList *selection;

  GRecMutex *wave_mutex;

  if(!AGS_IS_WAVE(wave)){
    return(NULL);
  }

  /* get wave mutex */
  wave_mutex = AGS_WAVE_GET_OBJ_MUTEX(wave);

  /* selection */
  g_rec_mutex_lock(wave_mutex);

  selection = wave->selection;
  
  g_rec_mutex_unlock(wave_mutex);
  
  return(selection);
}

/**
 * ags_wave_is_buffer_selected:
 * @wave: the #AgsWave
 * @buffer: the #AgsBuffer to check for
 *
 * Check selection for buffer.
 *
 * Returns: %TRUE if selected otherwise %FALSE
 *
 * Since: 3.0.0
 */
gboolean
ags_wave_is_buffer_selected(AgsWave *wave, AgsBuffer *buffer)
{
  GList *selection;

  guint64 x;
  guint64 current_x;
  gboolean retval;
  
  GRecMutex *wave_mutex;

  if(!AGS_IS_WAVE(wave) ||
     !AGS_IS_BUFFER(buffer)){
    return(FALSE);
  }

  /* get wave mutex */
  wave_mutex = AGS_WAVE_GET_OBJ_MUTEX(wave);

  /* get x */
  g_object_get(buffer,
	       "x", &x,
	       NULL);
  
  /* match buffer */
  g_rec_mutex_lock(wave_mutex);

  selection = wave->selection;
  retval = FALSE;
  
  while(selection != NULL){
    /* get current x */
    g_object_get(selection->data,
		 "x", &current_x,
		 NULL);

    if(current_x > x){
      break;
    }
    
    if(selection->data == buffer){
      retval = TRUE;

      break;
    }

    selection = selection->next;
  }

  g_rec_mutex_unlock(wave_mutex);

  return(retval);
}

/**
 * ags_wave_find_point:
 * @wave: the #AgsWave
 * @x: offset
 * @use_selection_list: if %TRUE selection is searched
 *
 * Find buffers by offset.
 *
 * Returns: (transfer none): the matching buffer as #AgsBuffer.
 *
 * Since: 3.0.0
 */
AgsBuffer*
ags_wave_find_point(AgsWave *wave,
		    guint64 x,
		    gboolean use_selection_list)
{
  AgsBuffer *retval;
  
  GList *buffer;
  GList *current_start, *current_end, *current;

  guint buffer_size;
  guint64 current_start_x, current_end_x, current_x;
  guint length, position;
  gboolean success;

  GRecMutex *wave_mutex;

  if(!AGS_IS_WAVE(wave)){
    return(NULL);
  }

  /* get wave mutex */
  wave_mutex = AGS_WAVE_GET_OBJ_MUTEX(wave);

  /* find buffer */
  g_rec_mutex_lock(wave_mutex);

  buffer_size = wave->buffer_size;
  
  if(use_selection_list){
    buffer = wave->selection;
  }else{
    buffer = wave->buffer;
  }
  
  current_start = buffer;
  current_end = g_list_last(buffer);
  
  length = g_list_length(buffer);
  position = (length - 1) / 2;

  current = g_list_nth(current_start,
		       position);
  
  retval = NULL;
  success = FALSE;
  
  while(!success && current != NULL){
    g_object_get(current_start->data,
		 "x", &current_start_x,
		 NULL);

    if(current_start_x > x){
      break;
    }
    
    if(current_start_x <= x &&
       current_start_x + buffer_size > x){
      retval = current_start->data;

      break;
    }
    
    g_object_get(current_end->data,
		 "x", &current_end_x,
		 NULL);

    if(current_end_x + buffer_size < x){
      break;
    }
    
    if(current_end_x <= x &&
       current_end_x + buffer_size > x){
      retval = current_end->data;

      break;
    }

    g_object_get(current->data,
		 "x", &current_x,
		 NULL);
    
    if(current_x <= x &&
       current_x + buffer_size > x){
      retval = current->data;
      
      break;
    }

    if(length <= 3){
      break;
    }

    if(current_x < x){
      current_start = current->next;
      current_end = current_end->prev;
    }else if(current_x > x){
      current_start = current_start->next;
      current_end = current->prev;
    }else{
      current_start = current_start->next;
      //NOTE:JK: we want progression
      //current_end = current_end->prev;
    }

    length = g_list_position(current_start,
			     current_end) + 1;
    position = (length - 1) / 2;

    current = g_list_nth(current_start,
			 position);
  }

  g_rec_mutex_unlock(wave_mutex);

  return(retval);
}

/**
 * ags_wave_find_region:
 * @wave: the #AgsWave
 * @x0: x start offset
 * @x1: x end offset
 * @use_selection_list: if %TRUE selection is searched
 *
 * Find buffers by offset and region.
 *
 * Returns: (element-type AgsAudio.Buffer) (transfer container): the matching buffers as #GList.
 *
 * Since: 3.0.0
 */
GList*
ags_wave_find_region(AgsWave *wave,
		     guint64 x0,
		     guint64 x1,
		     gboolean use_selection_list)
{
  GList *buffer;
  GList *region;

  guint buffer_size;
  guint64 current_x;
  
  GRecMutex *wave_mutex;

  if(!AGS_IS_WAVE(wave)){
    return(NULL);
  }

  /* get wave mutex */
  wave_mutex = AGS_WAVE_GET_OBJ_MUTEX(wave);

  if(x0 > x1){
    guint tmp;

    tmp = x1;
    x1 = x0;
    x0 = x1;
  }
  
  /* find buffer */
  g_rec_mutex_lock(wave_mutex);

  if(use_selection_list){
    buffer = wave->selection;
  }else{
    buffer = wave->buffer;
  }

  while(buffer != NULL){
    g_object_get(buffer->data,
		 "buffer-size", &buffer_size,
		 "x", &current_x,
		 NULL);
    
    if(current_x + buffer_size > x0){
      break;
    }
    
    buffer = buffer->next;
  }

  region = NULL;

  while(buffer != NULL){
    g_object_get(buffer->data,
		 "x", &current_x,
		 NULL);

    if(current_x > x1){
      break;
    }

    region = g_list_prepend(region,
			    buffer->data);

    buffer = buffer->next;
  }

  g_rec_mutex_unlock(wave_mutex);

  region = g_list_reverse(region);

  return(region);
}

/**
 * ags_wave_free_selection:
 * @wave: the #AgsWave
 *
 * Clear selection.
 *
 * Since: 3.0.0
 */
void
ags_wave_free_selection(AgsWave *wave)
{
  AgsBuffer *buffer;

  GList *list_start, *list;

  GRecMutex *wave_mutex;

  if(!AGS_IS_WAVE(wave)){
    return;
  }

  /* get wave mutex */
  wave_mutex = AGS_WAVE_GET_OBJ_MUTEX(wave);

  /* free selection */
  g_rec_mutex_lock(wave_mutex);

  list =
    list_start = wave->selection;
  
  while(list != NULL){
    ags_buffer_unset_flags(list->data,
			   AGS_BUFFER_IS_SELECTED);
    
    list = list->next;
  }

  wave->selection = NULL;

  g_rec_mutex_unlock(wave_mutex);
  
  g_list_free_full(list_start,
		   g_object_unref);
}

/**
 * ags_wave_add_region_to_selection:
 * @wave: the #AgsWave
 * @x0: x start offset
 * @x1: x end offset
 * @replace_current_selection: if %TRUE selection is replaced
 *
 * Add buffer within region to selection.
 *
 * Since: 3.0.0
 */
void
ags_wave_add_region_to_selection(AgsWave *wave,
				 guint64 x0, guint64 x1,
				 gboolean replace_current_selection)
{
  AgsBuffer *buffer;

  GList *region, *list;

  GRecMutex *wave_mutex;

  if(!AGS_IS_WAVE(wave)){
    return;
  }

  /* get wave mutex */
  wave_mutex = AGS_WAVE_GET_OBJ_MUTEX(wave);

  /* find region */
  region = ags_wave_find_region(wave,
				x0,
				x1,
				FALSE);

  if(replace_current_selection){
    ags_wave_free_selection(wave);

    list = region;

    while(list != NULL){
      guint buffer_size;
      guint64 current_x;
      guint64 selection_x0, selection_x1;

      g_object_get(list->data,
		   "buffer-size", &buffer_size,
		   "x", &current_x,
		   NULL);

      if(current_x + buffer_size > x0){
	selection_x0 = current_x;
      }else{
	selection_x0 = x0;
      }

      if(current_x + buffer_size < x1){
	selection_x1 = current_x + buffer_size;
      }else{
	selection_x1 = current_x + ((current_x + buffer_size) - x1);
      }
      
      ags_buffer_set_flags(list->data,
			   AGS_BUFFER_IS_SELECTED);
      g_object_ref(list->data);
      
      g_object_set(list->data,
		   "selection-x0", selection_x0,
		   "selection-x1", selection_x1,
		   NULL);

      list = list->next;
    }

    /* replace */
    g_rec_mutex_lock(wave_mutex);
     
    wave->selection = region;

    g_rec_mutex_unlock(wave_mutex);
  }else{
    list = region;
    
    while(list != NULL){
      guint buffer_size;
      guint64 current_x;
      guint64 selection_x0, selection_x1;

      g_object_get(list->data,
		   "buffer-size", &buffer_size,
		   "x", &current_x,
		   NULL);

      if(current_x + buffer_size > x0){
	selection_x0 = current_x;
      }else{
	selection_x0 = x0;
      }

      if(current_x + buffer_size < x1){
	selection_x1 = current_x + buffer_size;
      }else{
	selection_x1 = current_x + ((current_x + buffer_size) - x1);
      }

      if(!ags_wave_is_buffer_selected(wave, list->data)){
	/* add */
	ags_wave_add_buffer(wave,
			    list->data,
			    TRUE);

	g_object_set(list->data,
		     "selection-x0", selection_x0,
		     "selection-x1", selection_x1,
		     NULL);
      }else{
	guint64 current_selection_x0, current_selection_x1;

	g_object_get(list->data,
		     "selection-x0", &current_selection_x0,
		     "selection-x1", &current_selection_x1,
		     NULL);
	
	if(selection_x0 < current_selection_x0){
	  g_object_set(list->data,
		       "selection-x0", selection_x0,
		       NULL);
	}

	if(selection_x1 > current_selection_x1){
	  g_object_set(list->data,
		       "selection-x1", selection_x1,
		       NULL);
	}
      }
      
      list = list->next;
    }
    
    g_list_free(region);
  }
}

/**
 * ags_wave_remove_region_from_selection:
 * @wave: the #AgsWave
 * @x0: x start offset
 * @x1: x end offset
 *
 * Remove buffers within region of selection.
 *
 * Since: 3.0.0
 */ 
void
ags_wave_remove_region_from_selection(AgsWave *wave,
				      guint64 x0, guint64 x1)
{
  AgsBuffer *buffer;

  GList *region;
  GList *list;
  
  GRecMutex *wave_mutex;

  if(!AGS_IS_WAVE(wave)){
    return;
  }

  /* get wave mutex */
  wave_mutex = AGS_WAVE_GET_OBJ_MUTEX(wave);

  /* find region */
  region = ags_wave_find_region(wave,
				x0,
				x1,
				TRUE);

  list = region;
  
  while(list != NULL){
    ags_buffer_unset_flags(list->data,
			   AGS_BUFFER_IS_SELECTED);

    /* remove */
    g_rec_mutex_lock(wave_mutex);

    wave->selection = g_list_remove(wave->selection,
				    list->data);

    g_rec_mutex_unlock(wave_mutex);

    g_object_unref(list->data);

    /* iterate */
    list = list->next;
  }

  g_list_free(region);
}

/**
 * ags_wave_add_all_to_selection:
 * @wave: the #AgsWave
 *
 * Select all buffer to selection.
 *
 * Since: 3.0.0
 */
void
ags_wave_add_all_to_selection(AgsWave *wave)
{
  GList *list;
  
  GRecMutex *wave_mutex;

  if(!AGS_IS_WAVE(wave)){
    return;
  }

  /* get wave mutex */
  wave_mutex = AGS_WAVE_GET_OBJ_MUTEX(wave);

  /* select all */
  g_rec_mutex_lock(wave_mutex);

  list = wave->buffer;

  while(list != NULL){
    guint buffer_size;
    guint64 current_x;

    g_object_get(list->data,
		 "buffer-size", &buffer_size,
		 "x", &current_x,
		 NULL);
    
    ags_wave_add_buffer(wave,
			list->data, TRUE);
    g_object_set(list->data,
		 "selection-x0", current_x,
		 "selection-x1", current_x + buffer_size,
		 NULL);
    
    list = list->next;
  }

  g_rec_mutex_unlock(wave_mutex);
}

/**
 * ags_wave_copy_selection:
 * @wave: the #AgsWave
 *
 * Copy selection to clipboard.
 *
 * Returns: (transfer none): the selection as XML.
 *
 * Since: 3.0.0
 */
xmlNode*
ags_wave_copy_selection(AgsWave *wave)
{
  AgsBuffer *buffer;

  AgsTimestamp *timestamp;
  
  xmlNode *wave_node, *current_buffer;
  xmlNode *timestamp_node;

  GList *start_selection, *selection;

  xmlChar *str;

  guint format;
  guint64 x_boundary;

  GRecMutex *wave_mutex;

  if(!AGS_IS_WAVE(wave)){
    return(NULL);
  }

  /* get wave mutex */
  wave_mutex = AGS_WAVE_GET_OBJ_MUTEX(wave);

  /* create root node */
  wave_node = xmlNewNode(NULL,
			 BAD_CAST "wave");

  xmlNewProp(wave_node,
	     BAD_CAST "program",
	     BAD_CAST "ags");
  xmlNewProp(wave_node,
	     BAD_CAST "type",
	     BAD_CAST (AGS_WAVE_CLIPBOARD_TYPE));
  xmlNewProp(wave_node,
	     BAD_CAST "version",
	     BAD_CAST (AGS_WAVE_CLIPBOARD_VERSION));
  xmlNewProp(wave_node,
	     BAD_CAST "format",
	     BAD_CAST (AGS_WAVE_CLIPBOARD_FORMAT));
  xmlNewProp(wave_node,
	     BAD_CAST "line",
	     BAD_CAST (g_strdup_printf("%u", wave->line)));

  /* buffer format */
  g_object_get(wave,
	       "format", &format,
	       NULL);
  
  str = NULL;
  
  switch(format){    
  case AGS_SOUNDCARD_SIGNED_8_BIT:
    {
      str = "s8";
    }
    break;
  case AGS_SOUNDCARD_SIGNED_16_BIT:
    {
      str = "s16";
    }
    break;
  case AGS_SOUNDCARD_SIGNED_24_BIT:
    {
      str = "s24";
    }
    break;
  case AGS_SOUNDCARD_SIGNED_32_BIT:
    {
      str = "s32";
    }
    break;
  case AGS_SOUNDCARD_SIGNED_64_BIT:
    {
      str = "s64";
    }
    break;
  case AGS_SOUNDCARD_FLOAT:
    {
      str = "float";
    }
    break;
  case AGS_SOUNDCARD_DOUBLE:
    {
      str = "double";
    }
    break;
  }

  xmlNewProp(wave_node,
	     BAD_CAST "buffer-format",
	     BAD_CAST (str));
  
  /* timestamp */
  g_object_get(wave,
	       "timestamp", &timestamp,
	       NULL);

  if(timestamp != NULL){
    timestamp_node = xmlNewNode(NULL,
				BAD_CAST "timestamp");
    xmlAddChild(wave_node,
		timestamp_node);

    xmlNewProp(timestamp_node,
	       BAD_CAST "offset",
	       BAD_CAST (g_strdup_printf("%lu", ags_timestamp_get_ags_offset(timestamp))));

    g_object_unref(timestamp);
  }
  
  /* selection */
  g_rec_mutex_lock(wave_mutex);

  selection =
    start_selection = g_list_copy(wave->selection);

  g_rec_mutex_unlock(wave_mutex);

  if(selection != NULL){
    g_object_get(selection->data,
		 "selection-x0", &x_boundary,
		 NULL);
    x_boundary = AGS_BUFFER(selection->data)->selection_x0;
  }else{
    x_boundary = 0;
  }

  while(selection != NULL){
    xmlChar *content;
    guchar *cbuffer;

    guint buffer_size;

    GRecMutex *buffer_mutex;

    buffer = AGS_BUFFER(selection->data);

    buffer_mutex = AGS_BUFFER_GET_OBJ_MUTEX(buffer);
    
    current_buffer = xmlNewChild(wave_node,
				 NULL,
				 BAD_CAST "buffer",
				 NULL);

    g_rec_mutex_lock(buffer_mutex);

    xmlNewProp(current_buffer,
	       BAD_CAST "samplerate",
	       BAD_CAST (g_strdup_printf("%u", buffer->samplerate)));

    xmlNewProp(current_buffer,
	       BAD_CAST "buffer-size",
	       BAD_CAST (g_strdup_printf("%u", buffer->buffer_size)));

    switch(buffer->format){    
    case AGS_SOUNDCARD_SIGNED_8_BIT:
      {
	str = "s8";
      }
      break;
    case AGS_SOUNDCARD_SIGNED_16_BIT:
      {
	str = "s16";
      }
      break;
    case AGS_SOUNDCARD_SIGNED_24_BIT:
      {
	str = "s24";
      }
      break;
    case AGS_SOUNDCARD_SIGNED_32_BIT:
      {
	str = "s32";
      }
      break;
    case AGS_SOUNDCARD_SIGNED_64_BIT:
      {
	str = "s64";
      }
      break;
    case AGS_SOUNDCARD_FLOAT:
      {
	str = "float";
      }
      break;
    case AGS_SOUNDCARD_DOUBLE:
      {
	str = "double";
      }
      break;
    }
    
    xmlNewProp(current_buffer,
	       BAD_CAST "format",
	       BAD_CAST (str));

    //    g_message("copy - buffer->x = %lu", buffer->x);
    
    xmlNewProp(current_buffer,
	       BAD_CAST "x",
	       BAD_CAST (g_strdup_printf("%lu", buffer->x)));

    xmlNewProp(current_buffer,
	       BAD_CAST "selection-x0",
	       BAD_CAST (g_strdup_printf("%lu", buffer->selection_x0)));

    xmlNewProp(current_buffer,
	       BAD_CAST "selection-x1",
	       BAD_CAST (g_strdup_printf("%lu", buffer->selection_x1)));
    
    cbuffer = NULL;
    buffer_size = 0;
    
    switch(buffer->format){
    case AGS_SOUNDCARD_SIGNED_8_BIT:
      {
	cbuffer = ags_buffer_util_s8_to_char_buffer((gint8 *) buffer->data,
						    buffer->buffer_size);
	buffer_size = buffer->buffer_size * sizeof(guchar);
      }
      break;
    case AGS_SOUNDCARD_SIGNED_16_BIT:
      {
	cbuffer = ags_buffer_util_s16_to_char_buffer((gint16 *) buffer->data,
						     buffer->buffer_size);
	buffer_size = 2 * buffer->buffer_size * sizeof(guchar);
      }
      break;
    case AGS_SOUNDCARD_SIGNED_24_BIT:
      {
	cbuffer = ags_buffer_util_s24_to_char_buffer((gint32 *) buffer->data,
						     buffer->buffer_size);
	buffer_size = 4 * buffer->buffer_size * sizeof(guchar);
      }
      break;
    case AGS_SOUNDCARD_SIGNED_32_BIT:
      {
	cbuffer = ags_buffer_util_s32_to_char_buffer((gint32 *) buffer->data,
						     buffer->buffer_size);
	buffer_size = 4 * buffer->buffer_size * sizeof(guchar);
      }
      break;
    case AGS_SOUNDCARD_SIGNED_64_BIT:
      {
	cbuffer = ags_buffer_util_s64_to_char_buffer((gint64 *) buffer->data,
						     buffer->buffer_size);
	buffer_size = 8 * buffer->buffer_size * sizeof(guchar);
      }
      break;
    }

    g_rec_mutex_unlock(buffer_mutex);

    //    current_buffer->content = g_base64_encode(cbuffer,
    //					      buffer_size);
    xmlNodeSetContent(current_buffer,
		      g_base64_encode(cbuffer,
    				      buffer_size));
    
    g_free(cbuffer);
    
    selection = selection->next;
  }

  g_list_free(start_selection);
  
  xmlNewProp(wave_node,
	     BAD_CAST "x-boundary",
	     BAD_CAST (g_strdup_printf("%lu", x_boundary)));

  return(wave_node);
}

/**
 * ags_wave_cut_selection:
 * @wave: the #AgsWave
 *
 * Cut selection to clipboard.
 *
 * Returns: (transfer none): the selection as xmlNode
 *
 * Since: 3.0.0
 */
xmlNode*
ags_wave_cut_selection(AgsWave *wave)
{
  xmlNode *wave_node;
  
  GList *selection, *buffer;
  
  GRecMutex *wave_mutex;

  if(!AGS_IS_WAVE(wave)){
    return(NULL);
  }

  /* get wave mutex */
  wave_mutex = AGS_WAVE_GET_OBJ_MUTEX(wave);

  /* copy selection */
  wave_node = ags_wave_copy_selection(wave);

  /* cut */
  g_rec_mutex_lock(wave_mutex);

  selection = wave->selection;

  while(selection != NULL){
    wave->buffer = g_list_remove(wave->buffer,
				 selection->data);
    g_object_unref(selection->data);

    selection = selection->next;
  }

  g_rec_mutex_unlock(wave_mutex);

  /* free selection */
  ags_wave_free_selection(wave);

  return(wave_node);
}

void
ags_wave_insert_native_level_from_clipboard_version_1_4_0(AgsWave *wave,
							  xmlNode *root_node, char *version,
							  char *x_boundary,
							  gboolean reset_x_offset, guint64 x_offset,
							  gdouble delay, guint attack,
							  gboolean match_line, gboolean do_replace,
							  guint current_line,
							  guint64 relative_offset,
							  guint wave_samplerate,
							  guint wave_buffer_size,
							  guint wave_format,
							  gboolean match_timestamp)
{
  AgsBuffer *buffer;

  AgsTimestamp *timestamp;

  xmlNode *node;

  void *clipboard_data;
  unsigned char *clipboard_cdata;

  xmlChar *samplerate;
  xmlChar *buffer_size;
  xmlChar *format;
  xmlChar *x;
  xmlChar *selection_x0, *selection_x1;
  xmlChar *content;
  gchar *offset;
  char *endptr;

  guint64 timestamp_offset;
  guint samplerate_val;
  guint buffer_size_val;
  guint format_val;
  guint64 x_boundary_val;
  guint64 x_val;
  guint64 base_x_difference;
  guint64 selection_x0_val, selection_x1_val;
  guint target_frame_count, frame_count;
  guint word_size;
  gsize clipboard_length;
  gboolean subtract_x;

  node = root_node->children;

  /* retrieve x values for resetting */
  base_x_difference = 0;
  subtract_x = FALSE;

  x_boundary_val = 0;
    
  if(reset_x_offset){
    if(x_boundary != NULL){
      errno = 0;
	
      x_boundary_val = strtoul(x_boundary,
			       &endptr,
			       10);

      if(errno == ERANGE){
	goto dont_reset_x_offset;
      } 
	
      if(x_boundary == endptr){
	goto dont_reset_x_offset;
      }

      if(x_boundary_val < x_offset){
	base_x_difference = x_offset - x_boundary_val;
	subtract_x = FALSE;
      }else{
	base_x_difference = x_boundary_val - x_offset;
	subtract_x = TRUE;
      }
    }else{
    dont_reset_x_offset:
      reset_x_offset = FALSE;
    }
  }

  /* parse */
  while(node != NULL){
    if(node->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp("buffer",
		     node->name,
		     7)){
	void *target_data;

	samplerate_val = 0;
	samplerate = xmlGetProp(node,
				"samplerate");

	if(samplerate != NULL){
	  samplerate_val = g_ascii_strtoull(samplerate,
					    NULL,
					    10);
	}

	buffer_size_val = 0;
	buffer_size = xmlGetProp(node,
				 "buffer-size");

	if(buffer_size != NULL){
	  buffer_size_val = g_ascii_strtoull(buffer_size,
					     NULL,
					     10);
	}
	  
	/* retrieve format */
	format = xmlGetProp(node,
			    "format");

	if(!g_ascii_strncasecmp("s8",
				format,
				3)){
	  format_val = AGS_SOUNDCARD_SIGNED_8_BIT;
	}else if(!g_ascii_strncasecmp("s16",
				      format,
				      4)){
	  format_val = AGS_SOUNDCARD_SIGNED_16_BIT;
	}else if(!g_ascii_strncasecmp("s24",
				      format,
				      4)){
	  format_val = AGS_SOUNDCARD_SIGNED_24_BIT;
	}else if(!g_ascii_strncasecmp("s32",
				      format,
				      4)){
	  format_val = AGS_SOUNDCARD_SIGNED_32_BIT;
	}else if(!g_ascii_strncasecmp("s64",
				      format,
				      4)){
	  format_val = AGS_SOUNDCARD_SIGNED_64_BIT;
	}else if(!g_ascii_strncasecmp("float",
				      format,
				      4)){
	  format_val = AGS_SOUNDCARD_FLOAT;
	}else if(!g_ascii_strncasecmp("double",
				      format,
				      4)){
	  format_val = AGS_SOUNDCARD_DOUBLE;
	}else{
	  node = node->next;
	  
	  continue;
	}
	  
	/* retrieve x offset */
	x = xmlGetProp(node,
		       "x");

	if(x == NULL){
	  node = node->next;
	  
	  continue;
	}
	  
	errno = 0;
	x_val = g_ascii_strtoull(x,
				 &endptr,
				 10);

	if(errno == ERANGE){
	  node = node->next;
	  
	  continue;
	} 

	if(x == endptr){
	  node = node->next;
	  
	  continue;
	}

	/* calculate new offset */
	if(reset_x_offset){
	  errno = 0;

	  if(subtract_x){
	    x_val -= base_x_difference;

	    if(errno != 0){
	      node = node->next;
	      
	      continue;
	    }
	  }else{
	    x_val += base_x_difference;

	    if(errno != 0){
	      node = node->next;
	      
	      continue;
	    }
	  }
	}
	  
	/* selection x0 and x1 */
	selection_x0_val = 0;
	selection_x1_val = wave_buffer_size;
	  
	selection_x0 = xmlGetProp(node,
				  "selection-x0");

	if(selection_x0 != NULL){
	  guint64 tmp;

	  endptr = NULL;
	  errno = 0;
	  tmp = g_ascii_strtoull(selection_x0,
				 &endptr,
				 10);
	    
	  if(errno != ERANGE &&
	     endptr != selection_x0 &&
	     tmp < wave_buffer_size){
	    selection_x0_val = tmp;
	  }
	}
	  
	selection_x1 = xmlGetProp(node,
				  "selection-x1");

	if(selection_x1 != NULL){
	  guint64 tmp;

	  endptr = NULL;
	  errno = 0;
	  tmp = g_ascii_strtoull(selection_x1,
				 &endptr,
				 10);
	    
	  if(errno != ERANGE &&
	     endptr != selection_x1 &&
	     selection_x0_val <= tmp &&
	     tmp <= wave_buffer_size){
	    selection_x1_val = tmp;
	  }
	}

	frame_count = selection_x1_val - selection_x0_val;
	  
	if(frame_count == 0){
	  node = node->next;
	  
	  continue;
	}

	content = xmlNodeGetContent(node);
	//content = node->content;
	  
	if(content == NULL){
	  node = node->next;
	  
	  continue;
	}
	  
	clipboard_cdata = g_base64_decode(content,
					  &clipboard_length);
	  
	switch(format_val){
	case AGS_SOUNDCARD_SIGNED_8_BIT:
	{
	  word_size = 1;

	  clipboard_data = ags_buffer_util_char_buffer_to_s8(clipboard_cdata,
							     clipboard_length);
	}
	break;
	case AGS_SOUNDCARD_SIGNED_16_BIT:
	{
	  word_size = 2;

	  clipboard_data = ags_buffer_util_char_buffer_to_s16(clipboard_cdata,
							      clipboard_length);
	}
	break;
	case AGS_SOUNDCARD_SIGNED_24_BIT:
	{
	  word_size = 4;

	  clipboard_data = ags_buffer_util_char_buffer_to_s32(clipboard_cdata,
							      clipboard_length);
	}
	break;
	case AGS_SOUNDCARD_SIGNED_32_BIT:
	{
	  word_size = 4;

	  clipboard_data = ags_buffer_util_char_buffer_to_s32(clipboard_cdata,
							      clipboard_length);
	}
	break;
	case AGS_SOUNDCARD_SIGNED_64_BIT:
	{
	  word_size = 8;

	  clipboard_data = ags_buffer_util_char_buffer_to_s64(clipboard_cdata,
							      clipboard_length);
	}
	break;
	case AGS_SOUNDCARD_FLOAT:
	{
	  word_size = sizeof(gfloat);

	  clipboard_data = ags_buffer_util_char_buffer_to_float(clipboard_cdata,
								clipboard_length);
	}
	break;
	case AGS_SOUNDCARD_DOUBLE:
	{
	  word_size = sizeof(gdouble);

	  clipboard_data = ags_buffer_util_char_buffer_to_double(clipboard_cdata,
								 clipboard_length);
	}
	break;
	default:
	  node = node->next;
		  
	  continue;
	}

	if(clipboard_length % word_size != 0 ||
	   clipboard_length / word_size != frame_count){
	  g_warning("malformed clipboard");

	  node = node->next;
	    
	  continue;
	}

	/* add buffer */
	g_object_get(wave,
		     "timestamp", &timestamp,
		     NULL);

	timestamp_offset = ags_timestamp_get_ags_offset(timestamp);
	g_object_unref(timestamp);

	if(!match_timestamp ||
	   x_val < timestamp_offset + relative_offset){
	  guint copy_mode;
	    
	  /* find first */
	  buffer = ags_wave_find_point(wave,
				       wave_buffer_size * floor(x_val / wave_buffer_size),
				       FALSE);

	  if(buffer != NULL &&
	     do_replace){
	    void *data;

	    //	      g_message("found %d", x_val);
	      
	    data = buffer->data;

	    if(attack != 0){
	      switch(wave_format){
	      case AGS_SOUNDCARD_SIGNED_8_BIT:
	      {
		data = ((gint8 *) data) + attack;
	      }
	      break;
	      case AGS_SOUNDCARD_SIGNED_16_BIT:
	      {
		data = ((gint16 *) data) + attack;
	      }
	      break;
	      case AGS_SOUNDCARD_SIGNED_24_BIT:
	      {
		data = ((gint32 *) data) + attack;
	      }
	      break;
	      case AGS_SOUNDCARD_SIGNED_32_BIT:
	      {
		data = ((gint32 *) data) + attack;
	      }
	      break;
	      case AGS_SOUNDCARD_SIGNED_64_BIT:
	      {
		data = ((gint64 *) data) + attack;
	      }
	      break;
	      case AGS_SOUNDCARD_FLOAT:
	      {
		data = ((gfloat *) data) + attack;
	      }
	      break;
	      case AGS_SOUNDCARD_DOUBLE:
	      {
		data = ((gdouble *) data) + attack;
	      }
	      break;
	      default:
		g_warning("unknown soundcard format");
		  
		node = node->next;
		  
		continue;
	      }
	    }
		
	    if(attack + frame_count <= wave_buffer_size){
	      if(wave_format == AGS_SOUNDCARD_DOUBLE){
		ags_audio_buffer_util_clear_double(data, 1,
						   frame_count);
	      }else if(wave_format == AGS_SOUNDCARD_FLOAT){
		ags_audio_buffer_util_clear_float(data, 1,
						  frame_count);
	      }else{		
		ags_audio_buffer_util_clear_buffer(data, 1,
						   frame_count, ags_audio_buffer_util_format_from_soundcard(wave_format));
	      }
	    }else{
	      if(wave_format == AGS_SOUNDCARD_DOUBLE){
		ags_audio_buffer_util_clear_double(data, 1,
						   wave_buffer_size);
	      }else if(wave_format == AGS_SOUNDCARD_FLOAT){
		ags_audio_buffer_util_clear_float(data, 1,
						  wave_buffer_size);
	      }else{		
		ags_audio_buffer_util_clear_buffer(data, 1,
						   wave_buffer_size - attack, ags_audio_buffer_util_format_from_soundcard(wave_format));
	      }
	    }
	  }
	    
	  if(buffer == NULL){
	    buffer = ags_buffer_new();
	    g_object_set(buffer,
			 "samplerate", wave_samplerate,
			 "buffer-size", wave_buffer_size,
			 "format", wave_format,
			 NULL);  
	      
	    buffer->x = x_val;
	      
	    //	      g_message("created %d", x_val);
	      
	    ags_wave_add_buffer(wave,
				buffer,
				FALSE);
	  }

	  //	    g_message("insert - buffer->x = %lu", buffer->x);
	  //	    g_message("%d %d", wave_format, format_val);
	  copy_mode = ags_audio_buffer_util_get_copy_mode(ags_audio_buffer_util_format_from_soundcard(wave_format),
							  ags_audio_buffer_util_format_from_soundcard(format_val));

	  if(samplerate_val != wave_samplerate){
	    target_frame_count = ceil((double) frame_count / (double) samplerate_val * (double) wave_samplerate);

	    target_data = ags_stream_alloc(target_frame_count,
					   format_val);

	    ags_audio_buffer_util_resample_with_buffer(clipboard_data, 1,
						       ags_audio_buffer_util_format_from_soundcard(format_val), samplerate_val,
						       buffer_size_val,
						       wave_samplerate,
						       target_frame_count,
						       target_data);
	    
	    if(attack + target_frame_count <= wave_buffer_size){
	      ags_audio_buffer_util_copy_buffer_to_buffer(buffer->data, 1, attack,
							  target_data, 1, 0,
							  target_frame_count, copy_mode);
	    }else{
	      ags_audio_buffer_util_copy_buffer_to_buffer(buffer->data, 1, attack,
							  target_data, 1, 0,
							  wave_buffer_size - attack, copy_mode);
	    }

	    free(target_data);
	  }else{
	    if(attack + frame_count <= wave_buffer_size){
	      ags_audio_buffer_util_copy_buffer_to_buffer(buffer->data, 1, attack,
							  clipboard_data, 1, 0,
							  frame_count, copy_mode);
	    }else{
	      ags_audio_buffer_util_copy_buffer_to_buffer(buffer->data, 1, attack,
							  clipboard_data, 1, 0,
							  wave_buffer_size - attack, copy_mode);
	    }
	  }
	    
	  /* find next */
	  if(attack + frame_count > wave_buffer_size){
	    buffer = ags_wave_find_point(wave,
					 x_val + 1,
					 FALSE);

	    if(buffer != NULL &&
	       do_replace){
	      void *data;

	      data = buffer->data;
		
	      if(wave_format == AGS_SOUNDCARD_DOUBLE){
		ags_audio_buffer_util_clear_double(data, 1,
						   attack);
	      }else if(wave_format == AGS_SOUNDCARD_FLOAT){
		ags_audio_buffer_util_clear_float(data, 1,
						  attack);
	      }else{		
		ags_audio_buffer_util_clear_buffer(data, 1,
						   attack, ags_audio_buffer_util_format_from_soundcard(wave_format));
	      }
	    }
	    
	    if(buffer == NULL){
	      buffer = ags_buffer_new();
	      g_object_set(buffer,
			   "samplerate", wave_samplerate,
			   "buffer-size", wave_buffer_size,
			   "format", wave_format,
			   NULL);  
	      buffer->x = x_val + 1;
	      
	      ags_wave_add_buffer(wave,
				  buffer,
				  FALSE);
	    }

	    //	      g_message("insert - buffer->x = %lu", buffer->x);

	    copy_mode = ags_audio_buffer_util_get_copy_mode(ags_audio_buffer_util_format_from_soundcard(wave_format),
							    ags_audio_buffer_util_format_from_soundcard(format_val));
	      
	    if(samplerate_val != wave_samplerate){
	      target_data = ags_stream_alloc(wave_buffer_size,
					     format_val);
	      
	      ags_audio_buffer_util_resample_with_buffer(clipboard_data, 1,
							 ags_audio_buffer_util_format_from_soundcard(format_val), samplerate_val,
							 buffer_size_val,
							 wave_samplerate,
							 wave_buffer_size,
							 target_data);
	      
	      ags_audio_buffer_util_copy_buffer_to_buffer(buffer->data, 1, 0,
							  target_data, 1, wave_buffer_size - attack,
							  attack, copy_mode);

	      free(target_data);
	    }else{
	      ags_audio_buffer_util_copy_buffer_to_buffer(buffer->data, 1, 0,
							  clipboard_data, 1, wave_buffer_size - attack,
							  attack, copy_mode);
	    }
	  }
	}
      }
    }
      
    node = node->next;
  }
}

/**
 * ags_wave_insert_native_level_from_clipboard:
 * @wave: the #AgsWave
 * @wave_node: the clipboard XML data
 * @version: clipboard version
 * @x_boundary: region start offset
 * @reset_x_offset: if %TRUE @x_offset used as cursor
 * @x_offset: region start cursor offset
 * @delay: the delay to be used
 * @attack: the attack to be used
 * @match_line: only paste if channel matches
 * @do_replace: if %TRUE current data is replaced, otherwise additive mixing is performed 
 *
 * Paste previously copied buffers. 
 *
 * Since: 3.0.0
 */
void
ags_wave_insert_native_level_from_clipboard(AgsWave *wave,
					    xmlNode *root_node, char *version,
					    char *x_boundary,
					    gboolean reset_x_offset, guint64 x_offset,
					    gdouble delay, guint attack,
					    gboolean match_line, gboolean do_replace)
{
  guint current_line;
  guint64 relative_offset;
  guint wave_samplerate;
  guint wave_buffer_size;
  guint wave_format;
  
  gboolean match_timestamp;  

  if(!AGS_IS_WAVE(wave)){
    return;
  }

  match_timestamp = TRUE;
  
  if(!xmlStrncmp("1.4.0",
		 version,
		 6)){
    /* changes contain only optional informations */
    g_object_get(wave,
		 "line", &current_line,
		 "samplerate", &wave_samplerate,
		 "buffer-size", &wave_buffer_size,
		 "format", &wave_format,
		 NULL);

    relative_offset = AGS_WAVE_DEFAULT_BUFFER_LENGTH * wave_samplerate;

    if(match_line &&
       current_line != g_ascii_strtoull(xmlGetProp(root_node,
						   "line"),
					NULL,
					10)){
      //      g_message("line %d", current_line);
      
      return;
    }
    
    ags_wave_insert_native_level_from_clipboard_version_1_4_0(wave,
							      root_node, version,
							      x_boundary,
							      reset_x_offset, x_offset,
							      delay, attack,
							      match_line, do_replace,
							      current_line,
							      relative_offset,
							      wave_samplerate,
							      wave_buffer_size,
							      wave_format,
							      match_timestamp);
  }  
}

/**
 * ags_wave_insert_from_clipboard:
 * @wave: the #AgsWave
 * @wave_node: the clipboard XML data
 * @reset_x_offset: if %TRUE @x_offset used as cursor
 * @x_offset: region start cursor offset
 * @delay: the delay to be used
 * @attack: the attack to be used
 *
 * Paste previously copied buffers. 
 *
 * Since: 3.0.0
 */
void
ags_wave_insert_from_clipboard(AgsWave *wave,
			       xmlNode *wave_node,
			       gboolean reset_x_offset, guint64 x_offset,
			       gdouble delay, guint attack)
{
  ags_wave_insert_from_clipboard_extended(wave,
					  wave_node,
					  reset_x_offset, x_offset,
					  delay, attack,
					  FALSE, FALSE);
}

/**
 * ags_wave_insert_from_clipboard_extended:
 * @wave: the #AgsWave
 * @wave_node: the clipboard XML data
 * @reset_x_offset: if %TRUE @x_offset used as cursor
 * @x_offset: region start cursor offset
 * @delay: the delay to be used
 * @attack: the attack to be used
 * @match_line: only paste if channel matches
 * @do_replace: if %TRUE current data is replaced, otherwise additive mixing is performed 
 * 
 * Paste previously copied buffers. 
 * 
 * Since: 3.0.0
 */
void
ags_wave_insert_from_clipboard_extended(AgsWave *wave,
					xmlNode *wave_node,
					gboolean reset_x_offset, guint64 x_offset,
					gdouble delay, guint attack,
					gboolean match_line, gboolean do_replace)
{
  char *program, *version, *type, *format;
  char *x_boundary;

  if(!AGS_IS_WAVE(wave)){
    return;
  }

  while(wave_node != NULL){
    if(wave_node->type == XML_ELEMENT_NODE && !xmlStrncmp("wave", wave_node->name, 9)){
      break;
    }

    wave_node = wave_node->next;
  }

  if(wave_node != NULL){
    program = xmlGetProp(wave_node, "program");

    if(!xmlStrncmp("ags", program, 4)){
      version = xmlGetProp(wave_node, "version");
      type = xmlGetProp(wave_node, "type");
      format = xmlGetProp(wave_node, "format");

      if(!xmlStrcmp(AGS_WAVE_CLIPBOARD_FORMAT,
		    format)){
	x_boundary = xmlGetProp(wave_node, "x-boundary");

	ags_wave_insert_native_level_from_clipboard(wave,
						    wave_node, version,
						    x_boundary,
						    reset_x_offset, x_offset,
						    delay, attack,
						    match_line, do_replace);
      }
    }
  }
}

/**
 * ags_wave_new:
 * @audio: the assigned #AgsAudio
 * @line: the line to be used
 *
 * Creates a new instance of #AgsWave.
 *
 * Returns: a new #AgsWave
 *
 * Since: 3.0.0
 */
AgsWave*
ags_wave_new(GObject *audio,
	     guint line)
{
  AgsWave *wave;
  
  wave = (AgsWave *) g_object_new(AGS_TYPE_WAVE,
				  "audio", audio,
				  "line", line,
				  NULL);
  
  return(wave);
}
