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

#include <ags/audio/ags_wave.h>

#include <ags/libags.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_audio_buffer_util.h>

#include <pthread.h>

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

static pthread_mutex_t ags_wave_class_mutex = PTHREAD_MUTEX_INITIALIZER;

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
   * Since: 2.0.0
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
   * Since: 2.0.0
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
   * Since: 2.0.0
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
   * Since: 2.0.0
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
   * Since: 2.0.0
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
   * AgsPattern:timestamp:
   *
   * The pattern's timestamp.
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
   * AgsWave:buffer:
   *
   * The assigned #AgsBuffer
   * 
   * Since: 2.0.0
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

  pthread_mutex_t *mutex;
  pthread_mutexattr_t *attr;

  wave->flags = 0;

  /* add wave mutex */
  wave->obj_mutexattr = 
    attr = (pthread_mutexattr_t *) malloc(sizeof(pthread_mutexattr_t));
  pthread_mutexattr_init(attr);
  pthread_mutexattr_settype(attr,
			    PTHREAD_MUTEX_RECURSIVE);

#ifdef __linux__
  pthread_mutexattr_setprotocol(attr,
				PTHREAD_PRIO_INHERIT);
#endif

  wave->obj_mutex = 
    mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(mutex,
		     attr);  

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

  pthread_mutex_t *wave_mutex;

  wave = AGS_WAVE(gobject);

  /* get wave mutex */
  pthread_mutex_lock(ags_wave_get_class_mutex());
  
  wave_mutex = wave->obj_mutex;
  
  pthread_mutex_unlock(ags_wave_get_class_mutex());

  switch(prop_id){
  case PROP_AUDIO:
    {
      AgsAudio *audio;

      audio = (AgsAudio *) g_value_get_object(value);

      pthread_mutex_lock(wave_mutex);

      if(wave->audio == (GObject *) audio){
	pthread_mutex_unlock(wave_mutex);
	
	return;
      }

      if(wave->audio != NULL){
	g_object_unref(wave->audio);
      }

      if(audio != NULL){
	g_object_ref(audio);
      }

      wave->audio = (GObject *) audio;

      pthread_mutex_unlock(wave_mutex);
    }
    break;
  case PROP_LINE:
    {
      guint line;

      line = g_value_get_uint(value);

      pthread_mutex_lock(wave_mutex);

      wave->line = line;

      pthread_mutex_unlock(wave_mutex);
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

      pthread_mutex_lock(wave_mutex);

      if(timestamp == wave->timestamp){
	pthread_mutex_unlock(wave_mutex);
	
	return;
      }

      if(wave->timestamp != NULL){
	g_object_unref(G_OBJECT(wave->timestamp));
      }

      if(timestamp != NULL){
	g_object_ref(G_OBJECT(timestamp));
      }

      wave->timestamp = timestamp;

      pthread_mutex_unlock(wave_mutex);
    }
    break;
  case PROP_BUFFER:
    {
      AgsBuffer *buffer;

      buffer = (AgsBuffer *) g_value_get_pointer(value);

      pthread_mutex_lock(wave_mutex);

      if(buffer == NULL ||
	 g_list_find(wave->buffer, buffer) != NULL){
	pthread_mutex_unlock(wave_mutex);

	return;
      }

      pthread_mutex_unlock(wave_mutex);

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

  pthread_mutex_t *wave_mutex;

  wave = AGS_WAVE(gobject);

  /* get wave mutex */
  pthread_mutex_lock(ags_wave_get_class_mutex());
  
  wave_mutex = wave->obj_mutex;
  
  pthread_mutex_unlock(ags_wave_get_class_mutex());

  switch(prop_id){
  case PROP_AUDIO:
    {
      pthread_mutex_lock(wave_mutex);

      g_value_set_object(value, wave->audio);

      pthread_mutex_unlock(wave_mutex);
    }
    break;
  case PROP_LINE:
    {
      pthread_mutex_lock(wave_mutex);

      g_value_set_uint(value, wave->line);

      pthread_mutex_unlock(wave_mutex);
    }
    break;
  case PROP_SAMPLERATE:
    {
      pthread_mutex_lock(wave_mutex);

      g_value_set_uint(value, wave->samplerate);

      pthread_mutex_unlock(wave_mutex);
    }
    break;
  case PROP_BUFFER_SIZE:
    {
      pthread_mutex_lock(wave_mutex);

      g_value_set_uint(value, wave->buffer_size);

      pthread_mutex_unlock(wave_mutex);
    }
    break;
  case PROP_FORMAT:
    {
      pthread_mutex_lock(wave_mutex);

      g_value_set_uint(value, wave->format);

      pthread_mutex_unlock(wave_mutex);
    }
    break;
  case PROP_TIMESTAMP:
    {
      pthread_mutex_lock(wave_mutex);

      g_value_set_object(value, wave->timestamp);

      pthread_mutex_unlock(wave_mutex);
    }
    break;
  case PROP_BUFFER:
    {
      pthread_mutex_lock(wave_mutex);

      g_value_set_pointer(value, g_list_copy(wave->buffer));

      pthread_mutex_unlock(wave_mutex);
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

  pthread_mutex_destroy(wave->obj_mutex);
  free(wave->obj_mutex);

  pthread_mutexattr_destroy(wave->obj_mutexattr);
  free(wave->obj_mutexattr);

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
 * ags_wave_get_class_mutex:
 * 
 * Use this function's returned mutex to access mutex fields.
 *
 * Returns: the class mutex
 * 
 * Since: 2.0.0
 */
pthread_mutex_t*
ags_wave_get_class_mutex()
{
  return(&ags_wave_class_mutex);
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
 * Since: 2.0.0
 */
gboolean
ags_wave_test_flags(AgsWave *wave, guint flags)
{
  gboolean retval;
  
  pthread_mutex_t *wave_mutex;

  if(!AGS_IS_WAVE(wave)){
    return(FALSE);
  }
      
  /* get wave mutex */
  pthread_mutex_lock(ags_wave_get_class_mutex());
  
  wave_mutex = wave->obj_mutex;
  
  pthread_mutex_unlock(ags_wave_get_class_mutex());

  /* test */
  pthread_mutex_lock(wave_mutex);

  retval = (flags & (wave->flags)) ? TRUE: FALSE;
  
  pthread_mutex_unlock(wave_mutex);

  return(retval);
}

/**
 * ags_wave_set_flags:
 * @wave: the #AgsWave
 * @flags: the flags
 * 
 * Set @flags on @wave.
 * 
 * Since: 2.0.0
 */
void
ags_wave_set_flags(AgsWave *wave, guint flags)
{
  pthread_mutex_t *wave_mutex;

  if(!AGS_IS_WAVE(wave)){
    return;
  }
      
  /* get wave mutex */
  pthread_mutex_lock(ags_wave_get_class_mutex());
  
  wave_mutex = wave->obj_mutex;
  
  pthread_mutex_unlock(ags_wave_get_class_mutex());

  /* set */
  pthread_mutex_lock(wave_mutex);

  wave->flags |= flags;
  
  pthread_mutex_unlock(wave_mutex);
}

/**
 * ags_wave_unset_flags:
 * @wave: the #AgsWave
 * @flags: the flags
 * 
 * Unset @flags on @wave.
 * 
 * Since: 2.0.0
 */
void
ags_wave_unset_flags(AgsWave *wave, guint flags)
{
  pthread_mutex_t *wave_mutex;

  if(!AGS_IS_WAVE(wave)){
    return;
  }
      
  /* get wave mutex */
  pthread_mutex_lock(ags_wave_get_class_mutex());
  
  wave_mutex = wave->obj_mutex;
  
  pthread_mutex_unlock(ags_wave_get_class_mutex());

  /* set */
  pthread_mutex_lock(wave_mutex);

  wave->flags &= (~flags);
  
  pthread_mutex_unlock(wave_mutex);
}

/**
 * ags_wave_set_samplerate:
 * @wave: the #AgsWave
 * @samplerate: the samplerate
 * 
 * Set samplerate. 
 * 
 * Since: 2.0.0
 */
void
ags_wave_set_samplerate(AgsWave *wave,
			guint samplerate)
{
  GList *start_list, *list;

  void *data, *resampled_data;

  guint64 x;
  guint buffer_length;
  guint new_buffer_length;
  guint buffer_size;
  guint old_samplerate;
  guint format;
  guint offset;
  guint copy_mode;
  guint i;    

  pthread_mutex_t *wave_mutex;

  if(!AGS_IS_WAVE(wave)){
    return;
  }
  
  /* get wave mutex */
  pthread_mutex_lock(ags_wave_get_class_mutex());
  
  wave_mutex = wave->obj_mutex;
  
  pthread_mutex_unlock(ags_wave_get_class_mutex());

  /* apply samplerate */
  pthread_mutex_lock(wave_mutex);

  x = ags_timestamp_get_ags_offset(wave->timestamp);

  buffer_size = wave->buffer_size;
  old_samplerate = wave->samplerate;
  format = wave->format;
  
  wave->samplerate = samplerate;

  start_list = g_list_copy(wave->buffer);
  
  pthread_mutex_unlock(wave_mutex);

  data = NULL;

  buffer_length = g_list_length(start_list);

  switch(format){
  case AGS_SOUNDCARD_SIGNED_8_BIT:
    {
      data = (gint8 *) malloc(buffer_length * buffer_size * sizeof(gint8));

      copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_S8_TO_S8;
    }
    break;
  case AGS_SOUNDCARD_SIGNED_16_BIT:
    {
      data = (gint16 *) malloc(buffer_length * buffer_size * sizeof(gint16));

      copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_S16_TO_S16;
    }
    break;
  case AGS_SOUNDCARD_SIGNED_24_BIT:
    {
      data = (gint32 *) malloc(buffer_length * buffer_size * sizeof(gint32));

      copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_S32_TO_S32;
    }
    break;
  case AGS_SOUNDCARD_SIGNED_32_BIT:
    {
      data = (gint32 *) malloc(buffer_length * buffer_size * sizeof(gint32));

      copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_S32_TO_S32;
    }
    break;
  case AGS_SOUNDCARD_SIGNED_64_BIT:
    {
      data = (gint64 *) malloc(buffer_length * buffer_size * sizeof(gint64));

      copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_S64_TO_S64;
    }
    break;
  case AGS_SOUNDCARD_FLOAT:
    {
      data = (gfloat *) malloc(buffer_length * buffer_size * sizeof(gfloat));

      copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT_TO_FLOAT;
    }
    break;
  case AGS_SOUNDCARD_DOUBLE:
    {
      data = (gdouble *) malloc(buffer_length * buffer_size * sizeof(gdouble));

      copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_DOUBLE_TO_DOUBLE;
    }
    break;
  default:
    g_warning("ags_audio_signal_set_buffer_size() - unsupported format");
  }

  list = start_list;

  while(list != NULL){
    pthread_mutex_t *buffer_mutex;
  
    /* get buffer mutex */
    pthread_mutex_lock(ags_buffer_get_class_mutex());
  
    buffer_mutex = AGS_BUFFER(list->data)->obj_mutex;
  
    pthread_mutex_unlock(ags_buffer_get_class_mutex());

    /*  */
    pthread_mutex_lock(buffer_mutex);
    
    ags_audio_buffer_util_copy_buffer_to_buffer(data, 1, offset,
						AGS_BUFFER(list->data)->data, 1, 0,
						buffer_size, copy_mode);

    pthread_mutex_unlock(buffer_mutex);

    /* iterate */
    list = list->next;

    offset += buffer_size;
  }

  resampled_data = ags_audio_buffer_util_resample(data, 1,
						  ags_audio_buffer_util_format_from_soundcard(format), old_samplerate,
						  buffer_length * buffer_size,
						  samplerate);

  if(data != NULL){
    free(data);
  }

  new_buffer_length = (guint) ceil((samplerate * (buffer_length * buffer_size / old_samplerate)) / buffer_size);
    
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
  
  while(list != NULL && offset < buffer_length * buffer_size){
    pthread_mutex_t *buffer_mutex;
  
    /* get buffer mutex */
    pthread_mutex_lock(ags_buffer_get_class_mutex());
  
    buffer_mutex = AGS_BUFFER(list->data)->obj_mutex;
  
    pthread_mutex_unlock(ags_buffer_get_class_mutex());

    /*  */
    ags_buffer_set_samplerate(list->data,
			      samplerate);

    pthread_mutex_lock(buffer_mutex);
    
    if(offset + buffer_size < buffer_length * buffer_size){
      ags_audio_buffer_util_copy_buffer_to_buffer(AGS_BUFFER(list->data)->data, 1, 0,
						  resampled_data, 1, offset,
						  buffer_size, copy_mode);
    }else{
      guint end_offset;

      end_offset = (buffer_length * buffer_size) - offset;
      
      ags_audio_buffer_util_copy_buffer_to_buffer(AGS_BUFFER(list->data)->data, 1, 0,
						  resampled_data, 1, offset,
						  end_offset - offset, copy_mode);

      ags_audio_buffer_util_clear_buffer(AGS_BUFFER(list->data)->data + end_offset, 1,
					 buffer_size - (end_offset - offset), format);
    }
    
    pthread_mutex_unlock(buffer_mutex);

    /* iterate */
    list = list->next;

    offset += buffer_size;
  }

  g_list_free(start_list);

  if(resampled_data != NULL){
    free(resampled_data);
  }
}

/**
 * ags_wave_set_buffer_size:
 * @wave: the #AgsWave
 * @buffer_size: the buffer size
 * 
 * Set buffer size.
 * 
 * Since: 2.0.0
 */
void
ags_wave_set_buffer_size(AgsWave *wave,
			 guint buffer_size)
{
  GList *start_list, *list;

  void *data;

  guint64 x;
  guint buffer_length;
  guint new_buffer_length;
  guint offset;
  guint format;
  guint old_buffer_size;
  guint word_size;
  guint copy_mode;
  guint i;
  
  pthread_mutex_t *wave_mutex;

  if(!AGS_IS_WAVE(wave)){
    return;
  }
  
  /* get wave mutex */
  pthread_mutex_lock(ags_wave_get_class_mutex());
  
  wave_mutex = wave->obj_mutex;
  
  pthread_mutex_unlock(ags_wave_get_class_mutex());

  /* apply buffer size */
  pthread_mutex_lock(wave_mutex);

  old_buffer_size = wave->buffer_size;
  format = wave->format;
  
  wave->buffer_size = buffer_size;

  start_list = g_list_copy(wave->buffer);
  
  pthread_mutex_unlock(wave_mutex);

  /* resize buffer */
  data = NULL;
  
  buffer_length = g_list_length(start_list);
  word_size = 1;  
  
  switch(format){
  case AGS_SOUNDCARD_SIGNED_8_BIT:
    {
      data = (gint8 *) malloc(buffer_length * old_buffer_size * sizeof(gint8));

      copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_S8_TO_S8;
    }
    break;
  case AGS_SOUNDCARD_SIGNED_16_BIT:
    {
      data = (gint16 *) malloc(buffer_length * old_buffer_size * sizeof(gint16));

      copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_S16_TO_S16;
    }
    break;
  case AGS_SOUNDCARD_SIGNED_24_BIT:
    {
      data = (gint32 *) malloc(buffer_length * old_buffer_size * sizeof(gint32));

      copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_S32_TO_S32;
    }
    break;
  case AGS_SOUNDCARD_SIGNED_32_BIT:
    {
      data = (gint32 *) malloc(buffer_length * old_buffer_size * sizeof(gint32));

      copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_S32_TO_S32;
    }
    break;
  case AGS_SOUNDCARD_SIGNED_64_BIT:
    {
      data = (gint64 *) malloc(buffer_length * old_buffer_size * sizeof(gint64));

      copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_S64_TO_S64;
    }
    break;
  case AGS_SOUNDCARD_FLOAT:
    {
      data = (gfloat *) malloc(buffer_length * old_buffer_size * sizeof(gfloat));

      copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT_TO_FLOAT;
    }
    break;
  case AGS_SOUNDCARD_DOUBLE:
    {
      data = (gdouble *) malloc(buffer_length * old_buffer_size * sizeof(gdouble));

      copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_DOUBLE_TO_DOUBLE;
    }
    break;
  default:
    g_warning("ags_wave_set_buffer_size() - unsupported format");
  }

  list = start_list;

  offset = 0;
  
  while(list != NULL){
    pthread_mutex_t *buffer_mutex;
  
    /* get buffer mutex */
    pthread_mutex_lock(ags_buffer_get_class_mutex());
  
    buffer_mutex = AGS_BUFFER(list->data)->obj_mutex;
  
    pthread_mutex_unlock(ags_buffer_get_class_mutex());

    /*  */
    pthread_mutex_lock(buffer_mutex);
    
    ags_audio_buffer_util_copy_buffer_to_buffer(data, 1, offset,
						AGS_BUFFER(list->data)->data, 1, 0,
						old_buffer_size, copy_mode);

    pthread_mutex_unlock(buffer_mutex);

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
  
  while(list != NULL){
    pthread_mutex_t *buffer_mutex;
  
    /* get buffer mutex */
    pthread_mutex_lock(ags_buffer_get_class_mutex());
  
    buffer_mutex = AGS_BUFFER(list->data)->obj_mutex;
  
    pthread_mutex_unlock(ags_buffer_get_class_mutex());

    /*  */
    g_object_set(list->data,
		 "x", x + offset,
		 NULL);
    ags_buffer_set_buffer_size(list->data,
			       buffer_size);

    pthread_mutex_lock(buffer_mutex);
    
    if(offset + buffer_size < buffer_length * buffer_size){
      ags_audio_buffer_util_copy_buffer_to_buffer(AGS_BUFFER(list->data)->data, 1, 0,
						  data, 1, offset,
						  buffer_size, copy_mode);
    }else{
      guint end_offset;

      end_offset = (buffer_length * buffer_size) - offset;
      
      ags_audio_buffer_util_copy_buffer_to_buffer(AGS_BUFFER(list->data)->data, 1, 0,
						  data, 1, offset,
						  end_offset - offset, copy_mode);

      ags_audio_buffer_util_clear_buffer(AGS_BUFFER(list->data)->data + end_offset, 1,
					 buffer_size - (end_offset - offset), format);
    }
    
    pthread_mutex_unlock(buffer_mutex);

    /* iterate */
    list = list->next;

    offset += buffer_size;
  }

  g_list_free(start_list);

  if(data != NULL){
    free(data);
  }
}

/**
 * ags_wave_set_format:
 * @wave: the #AgsWave
 * @format: the format
 * 
 * Set format. 
 * 
 * Since: 2.0.0
 */
void
ags_wave_set_format(AgsWave *wave,
		    guint format)
{
  GList *list_start, *list;

  pthread_mutex_t *wave_mutex;

  if(!AGS_IS_WAVE(wave)){
    return;
  }
  
  /* get wave mutex */
  pthread_mutex_lock(ags_wave_get_class_mutex());
  
  wave_mutex = wave->obj_mutex;
  
  pthread_mutex_unlock(ags_wave_get_class_mutex());

  /* apply format */
  pthread_mutex_lock(wave_mutex);
  
  wave->format = format;

  list =
    list_start = g_list_copy(wave->buffer);
  
  pthread_mutex_unlock(wave_mutex);
  
  while(list != NULL){
    ags_buffer_set_format(list->data,
			  format);

    list = list->next;
  }

  g_list_free(list_start);
}

/**
 * ags_wave_find_near_timestamp:
 * @wave: a #GList containing #AgsWave
 * @line: the matching audio channel
 * @timestamp: the matching #AgsTimestamp, or %NULL to match any timestamp
 *
 * Retrieve appropriate wave for timestamp.
 *
 * Returns: Next matching #GList-struct or %NULL if not found
 *
 * Since: 2.0.0
 */
GList*
ags_wave_find_near_timestamp(GList *wave, guint line,
			     AgsTimestamp *timestamp)
{
  AgsTimestamp *current_timestamp;

  guint current_line;

  while(wave != NULL){
    guint64 relative_offset;
    guint samplerate;
    
    g_object_get(wave->data,
		 "line", &current_line,
		 "samplerate", &samplerate,
		 NULL);

    relative_offset = AGS_WAVE_DEFAULT_BUFFER_LENGTH * samplerate;
    
    if(current_line != line){
      wave = wave->next;
      
      continue;
    }

    if(timestamp == NULL){
      return(wave);
    }
    
    g_object_get(wave->data,
		 "timestamp", &current_timestamp,
		 NULL);
    
    if(current_timestamp != NULL){
      if(ags_timestamp_test_flags(timestamp,
				  AGS_TIMESTAMP_OFFSET) &&
	 ags_timestamp_test_flags(current_timestamp,
				  AGS_TIMESTAMP_OFFSET)){
	if(ags_timestamp_get_ags_offset(current_timestamp) >= ags_timestamp_get_ags_offset(timestamp) &&
	   ags_timestamp_get_ags_offset(current_timestamp) < ags_timestamp_get_ags_offset(timestamp) + relative_offset){
	  return(wave);
	}
      }else if(ags_timestamp_test_flags(timestamp,
					AGS_TIMESTAMP_UNIX) &&
	       ags_timestamp_test_flags(current_timestamp,
					AGS_TIMESTAMP_UNIX)){
	if(ags_timestamp_get_unix_time(current_timestamp) >= ags_timestamp_get_unix_time(timestamp) &&
	   ags_timestamp_get_unix_time(current_timestamp) < ags_timestamp_get_unix_time(timestamp) + AGS_WAVE_DEFAULT_DURATION){
	  return(wave);
	}
      }
    }
    
    wave = wave->next;
  }
  
  return(NULL);
}

/**
 * ags_wave_add:
 * @wave: the #GList-struct containing #AgsWave
 * @new_wave: the #AgsWave to add
 * 
 * Add @new_wave sorted to @wave
 * 
 * Returns: the new beginning of @wave
 * 
 * Since: 2.0.0
 */
GList*
ags_wave_add(GList *wave,
	     AgsWave *new_wave)
{
  auto gint ags_wave_add_compare(gconstpointer a,
				 gconstpointer b);
  
  gint ags_wave_add_compare(gconstpointer a,
			    gconstpointer b)
  {
    AgsTimestamp *timestamp_a, *timestamp_b;

    g_object_get(a,
		 "timestamp", &timestamp_a,
		 NULL);

    g_object_get(b,
		 "timestamp", &timestamp_b,
		 NULL);
    
    if(ags_timestamp_get_ags_offset(timestamp_a) == ags_timestamp_get_ags_offset(timestamp_b)){
      return(0);
    }else if(ags_timestamp_get_ags_offset(timestamp_a) < ags_timestamp_get_ags_offset(timestamp_b)){
      return(-1);
    }else if(ags_timestamp_get_ags_offset(timestamp_a) > ags_timestamp_get_ags_offset(timestamp_b)){
      return(1);
    }

    return(0);
  }
  
  if(!AGS_IS_WAVE(new_wave)){
    return(wave);
  }
  
  wave = g_list_insert_sorted(wave,
			      new_wave,
			      ags_wave_add_compare);
  
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
 * Since: 2.0.0
 */
void
ags_wave_add_buffer(AgsWave *wave,
		    AgsBuffer *buffer,
		    gboolean use_selection_list)
{
  pthread_mutex_t *wave_mutex;

  if(!AGS_IS_WAVE(wave) ||
     !AGS_IS_BUFFER(buffer)){
    return;
  }

  /* get wave mutex */
  pthread_mutex_lock(ags_wave_get_class_mutex());
  
  wave_mutex = wave->obj_mutex;
  
  pthread_mutex_unlock(ags_wave_get_class_mutex());

  /* insert sorted */
  g_object_ref(buffer);
  
  pthread_mutex_lock(wave_mutex);

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

  pthread_mutex_unlock(wave_mutex);
}

/**
 * ags_wave_remove_buffer:
 * @wave: the #AgsWave
 * @buffer: the #AgsBuffer to remove
 * @use_selection_list: if %TRUE remove from selection, else from default wave
 *
 * Removes @buffer from @wave.
 *
 * Since: 2.0.0
 */
void
ags_wave_remove_buffer(AgsWave *wave,
		       AgsBuffer *buffer,
		       gboolean use_selection_list)
{
  pthread_mutex_t *wave_mutex;

  if(!AGS_IS_WAVE(wave) ||
     !AGS_IS_BUFFER(buffer)){
    return;
  }

  /* get wave mutex */
  pthread_mutex_lock(ags_wave_get_class_mutex());
  
  wave_mutex = wave->obj_mutex;
  
  pthread_mutex_unlock(ags_wave_get_class_mutex());

  /* remove if found */
  pthread_mutex_lock(wave_mutex);
  
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

  pthread_mutex_unlock(wave_mutex);
}

/**
 * ags_wave_get_selection:
 * @wave: the #AgsWave
 *
 * Retrieve selection.
 *
 * Returns: the selection.
 *
 * Since: 2.0.0
 */
GList*
ags_wave_get_selection(AgsWave *wave)
{
  GList *selection;

  pthread_mutex_t *wave_mutex;

  if(!AGS_IS_WAVE(wave)){
    return(NULL);
  }

  /* get wave mutex */
  pthread_mutex_lock(ags_wave_get_class_mutex());
  
  wave_mutex = wave->obj_mutex;
  
  pthread_mutex_unlock(ags_wave_get_class_mutex());

  /* selection */
  pthread_mutex_lock(wave_mutex);

  selection = wave->selection;
  
  pthread_mutex_unlock(wave_mutex);
  
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
 * Since: 2.0.0
 */
gboolean
ags_wave_is_buffer_selected(AgsWave *wave, AgsBuffer *buffer)
{
  GList *selection;

  guint64 x;
  guint64 current_x;
  gboolean retval;
  
  pthread_mutex_t *wave_mutex;

  if(!AGS_IS_WAVE(wave) ||
     !AGS_IS_BUFFER(buffer)){
    return(FALSE);
  }

  /* get wave mutex */
  pthread_mutex_lock(ags_wave_get_class_mutex());
  
  wave_mutex = wave->obj_mutex;
  
  pthread_mutex_unlock(ags_wave_get_class_mutex());

  /* get x */
  g_object_get(buffer,
	       "x", &x,
	       NULL);
  
  /* match buffer */
  pthread_mutex_lock(wave_mutex);

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

  pthread_mutex_unlock(wave_mutex);

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
 * Returns: the matching buffer as #AgsBuffer.
 *
 * Since: 2.0.0
 */
AgsBuffer*
ags_wave_find_point(AgsWave *wave,
		    guint64 x,
		    gboolean use_selection_list)
{
  AgsBuffer *retval;
  
  GList *buffer;
  GList *current_start, *current_end, *current;

  guint64 current_start_x, current_end_x, current_x;
  guint length, position;
  gboolean success;

  pthread_mutex_t *wave_mutex;

  if(!AGS_IS_WAVE(wave)){
    return(NULL);
  }

  /* get wave mutex */
  pthread_mutex_lock(ags_wave_get_class_mutex());
  
  wave_mutex = wave->obj_mutex;
  
  pthread_mutex_unlock(ags_wave_get_class_mutex());

  /* find buffer */
  pthread_mutex_lock(wave_mutex);

  if(use_selection_list){
    buffer = wave->selection;
  }else{
    buffer = wave->buffer;
  }
  
  current_start = buffer;
  current_end = g_list_last(buffer);
  
  length = g_list_length(buffer);
  position = length / 2;

  current = g_list_nth(current_start,
		       position);
  
  retval = NULL;
  success = FALSE;
  
  while(!success && current != NULL){
    g_object_get(current_start->data,
		 "x", &current_start_x,
		 NULL);

    if(current_start_x == x){
      retval = current_start;

      break;
    }else if(x < current_start_x){
      break;
    }

    if(current_start == current_end){
      break;
    }
    
    g_object_get(current_end->data,
		 "x", &current_end_x,
		 NULL);

    if(current_end_x == x){
      retval = current_end;

      break;
    }else if(x > current_end_x){
      break;
    }

    if(current == current_end){
      break;
    }

    g_object_get(current->data,
		 "x", &current_x,
		 NULL);
    
    if(current_x == x){
      retval = current;
      
      break;
    }

    if(position <= 1){
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

  pthread_mutex_unlock(wave_mutex);

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
 * Returns: the matching buffers as #GList.
 *
 * Since: 2.0.0
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
  
  pthread_mutex_t *wave_mutex;

  if(!AGS_IS_WAVE(wave)){
    return(NULL);
  }

  /* get wave mutex */
  pthread_mutex_lock(ags_wave_get_class_mutex());
  
  wave_mutex = wave->obj_mutex;
  
  pthread_mutex_unlock(ags_wave_get_class_mutex());

  if(x0 > x1){
    guint tmp;

    tmp = x1;
    x1 = x0;
    x0 = x1;
  }
  
  /* find buffer */
  pthread_mutex_lock(wave_mutex);

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

  pthread_mutex_unlock(wave_mutex);

  region = g_list_reverse(region);

  return(region);
}

/**
 * ags_wave_free_selection:
 * @wave: the #AgsWave
 *
 * Clear selection.
 *
 * Since: 2.0.0
 */
void
ags_wave_free_selection(AgsWave *wave)
{
  AgsBuffer *buffer;

  GList *list_start, *list;

  pthread_mutex_t *wave_mutex;

  if(!AGS_IS_WAVE(wave)){
    return;
  }

  /* get wave mutex */
  pthread_mutex_lock(ags_wave_get_class_mutex());
  
  wave_mutex = wave->obj_mutex;
  
  pthread_mutex_unlock(ags_wave_get_class_mutex());

  /* free selection */
  pthread_mutex_lock(wave_mutex);

  list =
    list_start = wave->selection;
  
  while(list != NULL){
    ags_buffer_unset_flags(list->data,
			   AGS_BUFFER_IS_SELECTED);
    
    list = list->next;
  }

  wave->selection = NULL;

  pthread_mutex_unlock(wave_mutex);
  
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
 * Since: 2.0.0
 */
void
ags_wave_add_region_to_selection(AgsWave *wave,
				 guint64 x0, guint64 x1,
				 gboolean replace_current_selection)
{
  AgsBuffer *buffer;

  GList *region, *list;

  pthread_mutex_t *wave_mutex;

  if(!AGS_IS_WAVE(wave)){
    return;
  }

  /* get wave mutex */
  pthread_mutex_lock(ags_wave_get_class_mutex());
  
  wave_mutex = wave->obj_mutex;
  
  pthread_mutex_unlock(ags_wave_get_class_mutex());

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
	selection_x0 = current_x + ((current_x + buffer_size) - x1);
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
    pthread_mutex_lock(wave_mutex);
     
    wave->selection = region;

    pthread_mutex_unlock(wave_mutex);
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
	selection_x0 = current_x + ((current_x + buffer_size) - x1);
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
		     "selection-x0", current_selection_x0,
		     "selection-x1", current_selection_x1,
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
 * Since: 2.0.0
 */ 
void
ags_wave_remove_region_from_selection(AgsWave *wave,
				      guint64 x0, guint64 x1)
{
  AgsBuffer *buffer;

  GList *region;
  GList *list;
  
  pthread_mutex_t *wave_mutex;

  if(!AGS_IS_WAVE(wave)){
    return;
  }

  /* get wave mutex */
  pthread_mutex_lock(ags_wave_get_class_mutex());
  
  wave_mutex = wave->obj_mutex;
  
  pthread_mutex_unlock(ags_wave_get_class_mutex());

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
    pthread_mutex_lock(wave_mutex);

    wave->selection = g_list_remove(wave->selection,
				    list->data);

    pthread_mutex_unlock(wave_mutex);

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
 * Since: 2.0.0
 */
void
ags_wave_add_all_to_selection(AgsWave *wave)
{
  GList *list;
  
  pthread_mutex_t *wave_mutex;

  if(!AGS_IS_WAVE(wave)){
    return;
  }

  /* get wave mutex */
  pthread_mutex_lock(ags_wave_get_class_mutex());
  
  wave_mutex = wave->obj_mutex;
  
  pthread_mutex_unlock(ags_wave_get_class_mutex());

  /* select all */
  pthread_mutex_lock(wave_mutex);

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

  pthread_mutex_unlock(wave_mutex);
}

/**
 * ags_wave_copy_selection:
 * @wave: the #AgsWave
 *
 * Copy selection to clipboard.
 *
 * Returns: the selection as XML.
 *
 * Since: 2.0.0
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

  pthread_mutex_t *wave_mutex;

  if(!AGS_IS_WAVE(wave)){
    return(NULL);
  }

  /* get wave mutex */
  pthread_mutex_lock(ags_wave_get_class_mutex());
  
  wave_mutex = wave->obj_mutex;
  
  pthread_mutex_unlock(ags_wave_get_class_mutex());

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
  }
  
  /* selection */
  pthread_mutex_lock(wave_mutex);

  selection =
    start_selection = g_list_copy(wave->selection);

  pthread_mutex_unlock(wave_mutex);

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

    pthread_mutex_t *buffer_mutex;

    buffer = AGS_BUFFER(selection->data);

    pthread_mutex_lock(ags_buffer_get_class_mutex());

    buffer_mutex = buffer->obj_mutex;
    
    pthread_mutex_unlock(ags_buffer_get_class_mutex());
    
    current_buffer = xmlNewChild(wave_node,
				 NULL,
				 BAD_CAST "buffer",
				 NULL);

    pthread_mutex_lock(buffer_mutex);

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

    pthread_mutex_unlock(buffer_mutex);

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
 * Returns: the selection as xmlNode
 *
 * Since: 2.0.0
 */
xmlNode*
ags_wave_cut_selection(AgsWave *wave)
{
  xmlNode *wave_node;
  
  GList *selection, *buffer;
  
  pthread_mutex_t *wave_mutex;

  if(!AGS_IS_WAVE(wave)){
    return(NULL);
  }

  /* get wave mutex */
  pthread_mutex_lock(ags_wave_get_class_mutex());
  
  wave_mutex = wave->obj_mutex;
  
  pthread_mutex_unlock(ags_wave_get_class_mutex());

  /* copy selection */
  wave_node = ags_wave_copy_selection(wave);

  /* cut */
  pthread_mutex_lock(wave_mutex);

  selection = wave->selection;

  while(selection != NULL){
    wave->buffer = g_list_remove(wave->buffer,
				 selection->data);
    g_object_unref(selection->data);

    selection = selection->next;
  }

  pthread_mutex_unlock(wave_mutex);

  /* free selection */
  ags_wave_free_selection(wave);

  return(wave_node);
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
 * Since: 2.0.0
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
  
  auto void ags_wave_insert_native_level_from_clipboard_version_1_4_0();
  
  void ags_wave_insert_native_level_from_clipboard_version_1_4_0()
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
	  
	  samplerate = xmlGetProp(node,
				  "samplerate");

	  if(samplerate != NULL){
	    samplerate_val = g_ascii_strtoull(samplerate,
					      NULL,
					      10);
	  }
	  
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

	  if(!match_timestamp ||
	     x_val < ags_timestamp_get_ags_offset(timestamp) + relative_offset){
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
	      target_data = ags_audio_buffer_util_resample(clipboard_data, 1,
							   ags_audio_buffer_util_format_from_soundcard(format_val), samplerate_val,
							   buffer_size_val,
							   wave_samplerate);
	      target_frame_count = ceil((double) frame_count / (double) samplerate_val * (double) wave_samplerate);
	      
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
		target_data = ags_audio_buffer_util_resample(clipboard_data, 1,
							     ags_audio_buffer_util_format_from_soundcard(format_val), samplerate_val,
							     buffer_size_val,
							     wave_samplerate);
	      
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
    
    ags_wave_insert_native_level_from_clipboard_version_1_4_0();
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
 * Since: 2.0.0
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
 * Since: 2.0.0
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
 * @line: the audio channel to be used
 *
 * Creates a new instance of #AgsWave.
 *
 * Returns: a new #AgsWave
 *
 * Since: 2.0.0
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
