/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2024 Joël Krähemann
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

#include <ags/audio/ags_buffer.h>

#include <ags/audio/ags_audio_signal.h>

#include <ags/i18n.h>

#include <stdlib.h>
#include <string.h>

void ags_buffer_class_init(AgsBufferClass *buffer);
void ags_buffer_init(AgsBuffer *buffer);
void ags_buffer_set_property(GObject *gobject,
			     guint prop_id,
			     const GValue *value,
			     GParamSpec *param_spec);
void ags_buffer_get_property(GObject *gobject,
			     guint prop_id,
			     GValue *value,
			     GParamSpec *param_spec);
void ags_buffer_finalize(GObject *gobject);

/**
 * SECTION:ags_buffer
 * @short_description: Buffer class
 * @title: AgsBuffer
 * @section_id:
 * @include: ags/audio/ags_buffer.h
 *
 * #AgsBuffer represents audio data. If you want to modify `data` property you
 * have to lock the buffer using ags_buffer_lock() and when you are done release it
 * using ags_buffer_unlock().
 *
 * The `x` property is the offset in frames matching samplerate.
 */

enum{
  PROP_0,
  PROP_X,
  PROP_SELECTION_X0,
  PROP_SELECTION_X1,
  PROP_SAMPLERATE,
  PROP_BUFFER_SIZE,
  PROP_FORMAT,
  PROP_DATA,
};

static gpointer ags_buffer_parent_class = NULL;

GType
ags_buffer_get_type()
{
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
    GType ags_type_buffer = 0;

    static const GTypeInfo ags_buffer_info = {
      sizeof(AgsBufferClass),
      NULL,
      NULL,
      (GClassInitFunc) ags_buffer_class_init,
      NULL,
      NULL,
      sizeof(AgsBuffer),
      0,
      (GInstanceInitFunc) ags_buffer_init,
    };

    ags_type_buffer = g_type_register_static(G_TYPE_OBJECT,
					     "AgsBuffer",
					     &ags_buffer_info,
					     0);

    g_once_init_leave(&g_define_type_id__static, ags_type_buffer);
  }

  return(g_define_type_id__static);
}

GType
ags_buffer_flags_get_type()
{
  static gsize g_flags_type_id__static;

  if(g_once_init_enter(&g_flags_type_id__static)){
    static const GFlagsValue values[] = {
      { AGS_BUFFER_IS_SELECTED, "AGS_BUFFER_IS_SELECTED", "buffer-is-selected" },
      { 0, NULL, NULL }
    };

    GType g_flags_type_id = g_flags_register_static(g_intern_static_string("AgsBufferFlags"), values);

    g_once_init_leave(&g_flags_type_id__static, g_flags_type_id);
  }
  
  return(g_flags_type_id__static);
}

void 
ags_buffer_class_init(AgsBufferClass *buffer)
{
  GObjectClass *gobject;

  GParamSpec *param_spec;

  ags_buffer_parent_class = g_type_class_peek_parent(buffer);

  gobject = (GObjectClass *) buffer;

  gobject->set_property = ags_buffer_set_property;
  gobject->get_property = ags_buffer_get_property;

  gobject->finalize = ags_buffer_finalize;

  /* properties */
  /**
   * AgsBuffer:x:
   *
   * Buffer's x offset.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_uint64("x",
				   i18n_pspec("offset x"),
				   i18n_pspec("The x offset"),
				   0,
				   G_MAXUINT64,
				   0,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_X,
				  param_spec);

  /**
   * AgsBuffer:selection-x0:
   *
   * Buffer's selection x0 offset.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_uint64("selection-x0",
				   i18n_pspec("selection offset x0"),
				   i18n_pspec("The selection's x0 offset"),
				   0,
				   G_MAXUINT64,
				   0,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SELECTION_X0,
				  param_spec);

  /**
   * AgsBuffer:selection-x1:
   *
   * Buffer's selection x1 offset.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_uint64("selection-x1",
				   i18n_pspec("selection offset x1"),
				   i18n_pspec("The selection's x1 offset"),
				   0,
				   G_MAXUINT64,
				   0,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SELECTION_X1,
				  param_spec);

  /**
   * AgsBuffer:samplerate:
   *
   * Buffer's audio data samplerate.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_uint("samplerate",
				 i18n_pspec("audio data samplerate"),
				 i18n_pspec("The samplerate of audio data"),
				 0,
				 G_MAXUINT32,
				 AGS_SOUNDCARD_DEFAULT_SAMPLERATE,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SAMPLERATE,
				  param_spec);

  /**
   * AgsBuffer:buffer-size:
   *
   * Buffer's audio data buffer size.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_uint("buffer-size",
				 i18n_pspec("audio data's buffer size"),
				 i18n_pspec("The buffer size of audio data"),
				 0,
				 G_MAXUINT32,
				 AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_BUFFER_SIZE,
				  param_spec);

  /**
   * AgsBuffer:format:
   *
   * Buffer's audio data format.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_uint("format",
				 i18n_pspec("audio data format"),
				 i18n_pspec("The format of audio data"),
				 0,
				 G_MAXUINT32,
				 AGS_SOUNDCARD_DEFAULT_FORMAT,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FORMAT,
				  param_spec);

  /**
   * AgsBuffer:data:
   *
   * Buffer's audio data.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_pointer("data",
				    i18n_pspec("audio data"),
				    i18n_pspec("The audio data"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DATA,
				  param_spec);
}

void
ags_buffer_init(AgsBuffer *buffer)
{  
  AgsConfig *config;

  buffer->flags = 0;

  /* add buffer mutex */
  g_rec_mutex_init(&(buffer->obj_mutex));
  
  /* config */
  config = ags_config_get_instance();

  /* fields */
  buffer->x = 0;

  buffer->selection_x0 = 0;
  buffer->selection_x1 = 0;

  buffer->samplerate = (guint) ags_soundcard_helper_config_get_samplerate(config);
  buffer->buffer_size = (guint) ags_soundcard_helper_config_get_buffer_size(config);
  buffer->format = (guint) ags_soundcard_helper_config_get_format(config);

  buffer->data = ags_stream_alloc(buffer->buffer_size,
				  buffer->format);
}

void
ags_buffer_set_property(GObject *gobject,
			guint prop_id,
			const GValue *value,
			GParamSpec *param_spec)
{
  AgsBuffer *buffer;

  GRecMutex *buffer_mutex;

  buffer = AGS_BUFFER(gobject);

  /* get buffer mutex */
  buffer_mutex = AGS_BUFFER_GET_OBJ_MUTEX(buffer);

  switch(prop_id){
  case PROP_X:
    {
      g_rec_mutex_lock(buffer_mutex);

      buffer->x = g_value_get_uint64(value);

      g_rec_mutex_unlock(buffer_mutex);
    }
    break;
  case PROP_SELECTION_X0:
    {
      g_rec_mutex_lock(buffer_mutex);

      buffer->selection_x0 = g_value_get_uint64(value);

      g_rec_mutex_unlock(buffer_mutex);
    }
    break;
  case PROP_SELECTION_X1:
    {
      g_rec_mutex_lock(buffer_mutex);

      buffer->selection_x1 = g_value_get_uint64(value);

      g_rec_mutex_unlock(buffer_mutex);
    }
    break;
  case PROP_SAMPLERATE:
    {
      guint samplerate;
      
      samplerate = g_value_get_uint(value);

      ags_buffer_set_samplerate(buffer,
				samplerate);
    }
    break;
  case PROP_BUFFER_SIZE:
    {
      guint buffer_size;
      
      buffer_size = g_value_get_uint(value);

      ags_buffer_set_buffer_size(buffer,
				 buffer_size);
    }
    break;
  case PROP_FORMAT:
    {
      AgsSoundcardFormat format;
      
      format = g_value_get_uint(value);

      ags_buffer_set_format(buffer,
			    format);
    }
    break;
  case PROP_DATA:
    {
      g_rec_mutex_lock(buffer_mutex);

      buffer->data = g_value_get_pointer(value);

      g_rec_mutex_unlock(buffer_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_buffer_get_property(GObject *gobject,
			guint prop_id,
			GValue *value,
			GParamSpec *param_spec)
{
  AgsBuffer *buffer;

  GRecMutex *buffer_mutex;

  buffer = AGS_BUFFER(gobject);

  /* get buffer mutex */
  buffer_mutex = AGS_BUFFER_GET_OBJ_MUTEX(buffer);

  switch(prop_id){
  case PROP_X:
    {
      g_rec_mutex_lock(buffer_mutex);

      g_value_set_uint64(value,
			 buffer->x);

      g_rec_mutex_unlock(buffer_mutex);
    }
    break;
  case PROP_SELECTION_X0:
    {
      g_rec_mutex_lock(buffer_mutex);

      g_value_set_uint64(value,
			 buffer->selection_x0);

      g_rec_mutex_unlock(buffer_mutex);
    }
    break;
  case PROP_SELECTION_X1:
    {
      g_rec_mutex_lock(buffer_mutex);

      g_value_set_uint64(value,
			 buffer->selection_x1);

      g_rec_mutex_unlock(buffer_mutex);
    }
    break;
  case PROP_SAMPLERATE:
    {
      g_rec_mutex_lock(buffer_mutex);

      g_value_set_uint(value,
		       buffer->samplerate);

      g_rec_mutex_unlock(buffer_mutex);
    }
    break;
  case PROP_BUFFER_SIZE:
    {
      g_rec_mutex_lock(buffer_mutex);

      g_value_set_uint(value,
		       buffer->buffer_size);

      g_rec_mutex_unlock(buffer_mutex);
    }
    break;
  case PROP_FORMAT:
    {
      g_rec_mutex_lock(buffer_mutex);

      g_value_set_uint(value,
		       buffer->format);

      g_rec_mutex_unlock(buffer_mutex);
    }
    break;
  case PROP_DATA:
    {
      g_rec_mutex_lock(buffer_mutex);

      g_value_set_pointer(value,
			  buffer->data);

      g_rec_mutex_unlock(buffer_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_buffer_finalize(GObject *gobject)
{
  AgsBuffer *buffer;

  buffer = AGS_BUFFER(gobject);

  if(buffer->data != NULL){
    g_free(buffer->data);
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_buffer_parent_class)->finalize(gobject);
}

/**
 * ags_buffer_get_obj_mutex:
 * @buffer: the #AgsBuffer
 * 
 * Get object mutex.
 * 
 * Returns: the #GRecMutex to lock @buffer
 * 
 * Since: 3.1.0
 */
GRecMutex*
ags_buffer_get_obj_mutex(AgsBuffer *buffer)
{
  if(!AGS_IS_BUFFER(buffer)){
    return(NULL);
  }

  return(AGS_BUFFER_GET_OBJ_MUTEX(buffer));
}

/**
 * ags_buffer_lock:
 * @buffer: the #AgsBuffer
 * 
 * Lock object mutex.
 * 
 * Since: 3.1.0
 */
void
ags_buffer_lock(AgsBuffer *buffer)
{
  if(!AGS_IS_BUFFER(buffer)){
    return;
  }

  g_rec_mutex_lock(AGS_BUFFER_GET_OBJ_MUTEX(buffer));
}

/**
 * ags_buffer_unlock:
 * @buffer: the #AgsBuffer
 * 
 * Unlock object mutex.
 * 
 * Since: 3.1.0
 */
void
ags_buffer_unlock(AgsBuffer *buffer)
{
  if(!AGS_IS_BUFFER(buffer)){
    return;
  }

  g_rec_mutex_unlock(AGS_BUFFER_GET_OBJ_MUTEX(buffer));
}

/**
 * ags_buffer_test_flags:
 * @buffer: the #AgsBuffer
 * @flags: the flags
 * 
 * Test @flags to be set on @buffer.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 * 
 * Since: 3.0.0
 */
gboolean
ags_buffer_test_flags(AgsBuffer *buffer, guint flags)
{
  gboolean retval;
  
  GRecMutex *buffer_mutex;

  if(!AGS_IS_BUFFER(buffer)){
    return(FALSE);
  }
      
  /* get buffer mutex */  
  buffer_mutex = AGS_BUFFER_GET_OBJ_MUTEX(buffer);

  /* test */
  g_rec_mutex_lock(buffer_mutex);

  retval = (flags & (buffer->flags)) ? TRUE: FALSE;
  
  g_rec_mutex_unlock(buffer_mutex);

  return(retval);
}

/**
 * ags_buffer_set_flags:
 * @buffer: the #AgsBuffer
 * @flags: the flags
 * 
 * Set @flags on @buffer.
 * 
 * Since: 3.0.0
 */
void
ags_buffer_set_flags(AgsBuffer *buffer, guint flags)
{
  GRecMutex *buffer_mutex;

  if(!AGS_IS_BUFFER(buffer)){
    return;
  }
      
  /* get buffer mutex */
  buffer_mutex = AGS_BUFFER_GET_OBJ_MUTEX(buffer);

  /* set */
  g_rec_mutex_lock(buffer_mutex);

  buffer->flags |= flags;
  
  g_rec_mutex_unlock(buffer_mutex);
}

/**
 * ags_buffer_unset_flags:
 * @buffer: the #AgsBuffer
 * @flags: the flags
 * 
 * Unset @flags on @buffer.
 * 
 * Since: 3.0.0
 */
void
ags_buffer_unset_flags(AgsBuffer *buffer, guint flags)
{
  GRecMutex *buffer_mutex;

  if(!AGS_IS_BUFFER(buffer)){
    return;
  }
      
  /* get buffer mutex */
  buffer_mutex = AGS_BUFFER_GET_OBJ_MUTEX(buffer);

  /* unset */
  g_rec_mutex_lock(buffer_mutex);

  buffer->flags &= (~flags);
  
  g_rec_mutex_unlock(buffer_mutex);
}

/**
 * ags_buffer_sort_func:
 * @a: an #AgsBuffer
 * @b: an other #AgsBuffer
 * 
 * Sort buffers.
 * 
 * Returns: 0 if equal, -1 if smaller and 1 if bigger offset
 *
 * Since: 3.0.0
 */
gint
ags_buffer_sort_func(gconstpointer a,
		     gconstpointer b)
{
  guint64 a_x, b_x ;

  if(a == NULL || b == NULL){
    return(0);
  }
    
  g_object_get(a,
	       "x", &a_x,
	       NULL);
    
  g_object_get(b,
	       "x", &b_x,
	       NULL);

  if(a_x == b_x){
    return(0);
  }

  if(a_x < b_x){
    return(-1);
  }else{
    return(1);
  }  
}

/**
 * ags_buffer_get_x:
 * @buffer: the #AgsBuffer
 *
 * Gets x.
 * 
 * Returns: the x
 * 
 * Since: 3.1.0
 */
guint64
ags_buffer_get_x(AgsBuffer *buffer)
{
  guint64 x;
  
  if(!AGS_IS_BUFFER(buffer)){
    return(0);
  }

  g_object_get(buffer,
	       "x", &x,
	       NULL);

  return(x);
}

/**
 * ags_buffer_set_x:
 * @buffer: the #AgsBuffer
 * @x: the x
 *
 * Sets x.
 * 
 * Since: 3.1.0
 */
void
ags_buffer_set_x(AgsBuffer *buffer, guint64 x)
{
  if(!AGS_IS_BUFFER(buffer)){
    return;
  }

  g_object_set(buffer,
	       "x", x,
	       NULL);
}

/**
 * ags_buffer_get_samplerate:
 * @buffer: the #AgsBuffer
 *
 * Gets samplerate.
 * 
 * Returns: the samplerate
 * 
 * Since: 3.1.0
 */
guint
ags_buffer_get_samplerate(AgsBuffer *buffer)
{
  guint samplerate;
  
  if(!AGS_IS_BUFFER(buffer)){
    return(0);
  }

  g_object_get(buffer,
	       "samplerate", &samplerate,
	       NULL);

  return(samplerate);
}

/**
 * ags_buffer_set_samplerate:
 * @buffer: the #AgsBuffer
 * @samplerate: the samplerate
 * 
 * Set samplerate.
 * 
 * Since: 3.0.0
 */
void
ags_buffer_set_samplerate(AgsBuffer *buffer,
			  guint samplerate)
{
  GRecMutex *buffer_mutex;

  if(!AGS_IS_BUFFER(buffer)){
    return;
  }

  /* get buffer mutex */
  buffer_mutex = AGS_BUFFER_GET_OBJ_MUTEX(buffer);

  /* set samplerate */
  g_rec_mutex_lock(buffer_mutex);

  //TODO:JK: resample data?
  buffer->samplerate = samplerate;
  
  g_rec_mutex_unlock(buffer_mutex);
}

/**
 * ags_buffer_get_buffer_size:
 * @buffer: the #AgsBuffer
 *
 * Gets buffer size.
 * 
 * Returns: the buffer size
 * 
 * Since: 3.1.0
 */
guint
ags_buffer_get_buffer_size(AgsBuffer *buffer)
{
  guint buffer_size;
  
  if(!AGS_IS_BUFFER(buffer)){
    return(0);
  }

  g_object_get(buffer,
	       "buffer-size", &buffer_size,
	       NULL);

  return(buffer_size);
}

/**
 * ags_buffer_set_buffer_size:
 * @buffer: the #AgsBuffer
 * @buffer_size: the buffer size
 * 
 * Set buffer size.
 * 
 * Since: 3.0.0
 */
void
ags_buffer_set_buffer_size(AgsBuffer *buffer,
			   guint buffer_size)
{
  guint old_buffer_size;
  guint word_size;
  
  GRecMutex *buffer_mutex;

  if(!AGS_IS_BUFFER(buffer)){
    return;
  }
      
  /* get buffer mutex */
  buffer_mutex = AGS_BUFFER_GET_OBJ_MUTEX(buffer);

  /* set buffer size */
  g_rec_mutex_lock(buffer_mutex);

  old_buffer_size = buffer->buffer_size;
  
  buffer->buffer_size = buffer_size;

  if(old_buffer_size == buffer->buffer_size){
    g_rec_mutex_unlock(buffer_mutex);    

    return;
  }
  
  switch(buffer->format){
  case AGS_SOUNDCARD_SIGNED_8_BIT:
  {
    buffer->data = (gint8 *) g_realloc(buffer->data,
				       buffer_size * sizeof(gint8));
    word_size = sizeof(gint8);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_16_BIT:
  {
    buffer->data = (gint16 *) g_realloc(buffer->data,
					buffer_size * sizeof(gint16));
    word_size = sizeof(gint16);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_24_BIT:
  {
    buffer->data = (gint32 *) g_realloc(buffer->data,
					buffer_size * sizeof(gint32));
    //NOTE:JK: The 24-bit linear samples use 32-bit physical space
    word_size = sizeof(gint32);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_32_BIT:
  {
    buffer->data = (gint32 *) g_realloc(buffer->data,
					buffer_size * sizeof(gint32));
    word_size = sizeof(gint32);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_64_BIT:
  {
    buffer->data = (gint64 *) g_realloc(buffer->data,
					buffer_size * sizeof(gint64));
    word_size = sizeof(gint64);
    }
    break;
  default:
    g_rec_mutex_unlock(buffer_mutex);
    
    g_warning("ags_buffer_set_buffer_size(): unsupported word size");

    return;
  }

  if(old_buffer_size < buffer_size){
    memset(buffer->data + old_buffer_size, 0, (buffer_size - old_buffer_size) * word_size);
  }

  g_rec_mutex_unlock(buffer_mutex);
}

/**
 * ags_buffer_get_format:
 * @buffer: the #AgsBuffer
 *
 * Gets format.
 * 
 * Returns: the format
 * 
 * Since: 3.1.0
 */
AgsSoundcardFormat
ags_buffer_get_format(AgsBuffer *buffer)
{
  AgsSoundcardFormat format;
  
  if(!AGS_IS_BUFFER(buffer)){
    return(0);
  }

  g_object_get(buffer,
	       "format", &format,
	       NULL);

  return(format);
}

/**
 * ags_buffer_set_format:
 * @buffer: the #AgsBuffer
 * @format: the format
 * 
 * Set format.
 * 
 * Since: 3.0.0
 */
void
ags_buffer_set_format(AgsBuffer *buffer,
		      AgsSoundcardFormat format)
{
  void *data;

  guint copy_mode;

  GRecMutex *buffer_mutex;

  if(!AGS_IS_BUFFER(buffer)){
    return;
  }
      
  /* get buffer mutex */
  buffer_mutex = AGS_BUFFER_GET_OBJ_MUTEX(buffer);

  /* set format */
  g_rec_mutex_lock(buffer_mutex);

  if(buffer->format == format){
    g_rec_mutex_unlock(buffer_mutex);

    return;
  }
  
  data = ags_stream_alloc(buffer->buffer_size,
			  format);

  buffer->audio_buffer_util = ags_audio_buffer_util_alloc();
  
  copy_mode = ags_audio_buffer_util_get_copy_mode_from_format(buffer->audio_buffer_util,
							      ags_audio_buffer_util_format_from_soundcard(buffer->audio_buffer_util,
													  format),
							      ags_audio_buffer_util_format_from_soundcard(buffer->audio_buffer_util,
													  buffer->format));
  
  ags_audio_buffer_util_copy_buffer_to_buffer(buffer->audio_buffer_util,
					      data, 1, 0,
					      buffer->data, 1, 0,
					      buffer->buffer_size, copy_mode);

  g_free(buffer->data);

  buffer->data = data;

  buffer->format = format;

  g_rec_mutex_unlock(buffer_mutex);
}

/**
 * ags_buffer_get_data:
 * @buffer: the #AgsBuffer
 *
 * Gets data.
 * 
 * Returns: the data
 * 
 * Since: 3.1.0
 */
gpointer
ags_buffer_get_data(AgsBuffer *buffer)
{
  gpointer data;
  
  GRecMutex *buffer_mutex;

  if(!AGS_IS_BUFFER(buffer)){
    return(NULL);
  }
      
  /* get buffer mutex */
  buffer_mutex = AGS_BUFFER_GET_OBJ_MUTEX(buffer);

  /* set format */
  g_rec_mutex_lock(buffer_mutex);

  data = buffer->data;
  
  g_rec_mutex_unlock(buffer_mutex);

  return(data);
}

/**
 * ags_buffer_find_range_x:
 * @buffer: (element-type AgsAudio.Buffer) (transfer none): the #GList-struct containing #AgsBuffer
 * @start_x: start x offset
 * @end_x: end x offset
 * 
 * Find within range of @start_x and @end_x.
 *
 * Returns: (element-type AgsAudio.Buffer) (transfer none): the matching entry as #GList-struct, else %NULL
 *
 * Since: 8.0.11
 */
GList*
ags_buffer_find_range_x(GList *buffer,
			guint64 start_x, guint64 end_x)
{
  GList *bisect_start, *bisect_end, *bisect_center;
  GList *bisect_match;
  
  gint length;
  gboolean has_more;

  if(buffer == NULL){
    return(NULL);
  }
  
  bisect_start = buffer;
  bisect_end = g_list_last(buffer);

  length = g_list_length(buffer);
  
  bisect_center = g_list_nth(bisect_start,
			     length / 2);

  bisect_match = NULL;
  
  has_more = TRUE;
  
  while(has_more &&
	bisect_start != NULL &&
	bisect_end != NULL &&
	bisect_center != NULL){
    GList *current_match;
    
    guint bisect_start_x, bisect_end_x, bisect_center_x;

    gboolean bisect_head;

    bisect_head = TRUE;

    bisect_start_x = ags_buffer_get_x(bisect_start->data);
    bisect_end_x = ags_buffer_get_x(bisect_end->data);    
    bisect_center_x = ags_buffer_get_x(bisect_center->data);

    if(bisect_end_x >= start_x &&
       bisect_end_x <= end_x){
      current_match = bisect_end;

      bisect_head = FALSE;
    }

    if(bisect_center_x >= start_x &&
       bisect_center_x <= center_x){
      current_match = bisect_center;

      bisect_head = TRUE;
    }

    if(bisect_center_x < start_x){
      bisect_head = FALSE;
    }
    
    if(bisect_start_x >= start_x &&
       bisect_start_x <= start_x){
      current_match = bisect_start;

      bisect_head = TRUE;
    }

    if(current_match != NULL){
      if(bisect_match != NULL){
	if(ags_buffer_get_x(current_match->data) < ags_buffer_get_x(bisect_match->data)){
	  bisect_match = current_match;
	}
      }else{
	bisect_match = current_match;
      }
    }
    
    /* iterate */
    //NOTE:JK: bisect all, because multiple occurances of x possible
    //    if(bisect_end_x < start_x){
    //      has_more = FALSE;
    //    }
    
    if(length <= 3){
      has_more = FALSE;
    }

    if(has_more){
      if(bisect_head){
	bisect_start = bisect_start->next;
	bisect_end = bisect_center->prev;

	length = 0;

	if(bisect_start != NULL &&
	   bisect_end != NULL){
	  length = g_list_position(bisect_start,
				   bisect_end);

	  length++;
	}
	
	bisect_center = g_list_nth(bisect_start,
				   length / 2);
      }else{
	bisect_start = bisect_center->next;
	bisect_end = bisect_end->prev;

	length = 0;
	
	if(bisect_start != NULL &&
	   bisect_end != NULL){
	  length = g_list_position(bisect_start,
				   bisect_end);

	  length++;
	}

	bisect_center = g_list_nth(bisect_start,
				   length / 2);
      }
    }
  }
  
  return(bisect_match);
}

/**
 * ags_buffer_duplicate:
 * @buffer: an #AgsBuffer
 * 
 * Duplicate a buffer.
 *
 * Returns: (transfer full): the duplicated #AgsBuffer.
 *
 * Since: 3.0.0
 */
AgsBuffer*
ags_buffer_duplicate(AgsBuffer *buffer)
{
  AgsBuffer *buffer_copy;

  guint copy_mode;

  GRecMutex *buffer_mutex;

  if(!AGS_IS_BUFFER(buffer)){
    return(NULL);
  }
  
  /* get buffer mutex */
  buffer_mutex = AGS_BUFFER_GET_OBJ_MUTEX(buffer);

  /* instantiate buffer */  
  buffer_copy = ags_buffer_new();

  buffer_copy->flags = 0;

  g_rec_mutex_lock(buffer_mutex);

  buffer_copy->x = buffer->x;

  g_object_set(buffer_copy,
	       "samplerate", buffer->samplerate,
	       "buffer-size", buffer->buffer_size,
	       "format", buffer->format,
	       NULL);

  copy_mode = ags_audio_buffer_util_get_copy_mode_from_format(buffer->audio_buffer_util,
							      ags_audio_buffer_util_format_from_soundcard(buffer->audio_buffer_util,
													  buffer_copy->format),
							      ags_audio_buffer_util_format_from_soundcard(buffer->audio_buffer_util,
													  buffer->format));
  
  ags_audio_buffer_util_copy_buffer_to_buffer(buffer->audio_buffer_util,
					      buffer_copy->data, 1, 0,
					      buffer->data, 1, 0,
					      buffer_copy->buffer_size, copy_mode);

  g_rec_mutex_unlock(buffer_mutex);

  return(buffer_copy);
}

/**
 * ags_buffer_new:
 *
 * Creates a new instance of #AgsBuffer
 *
 * Returns: the new #AgsBuffer
 *
 * Since: 3.0.0
 */
AgsBuffer*
ags_buffer_new()
{
  AgsBuffer *buffer;

  buffer = (AgsBuffer *) g_object_new(AGS_TYPE_BUFFER,
				      NULL);

  return(buffer);
}
