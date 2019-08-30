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

#include <ags/audio/ags_frequency_map.h>

#include <ags/libags.h>

#include <ags/audio/ags_synth_enums.h>
#include <ags/audio/ags_synth_util.h>

#include <math.h>
#include <complex.h>

#include <ags/i18n.h>

void ags_frequency_map_class_init(AgsFrequencyMapClass *frequency_map_class);
void ags_frequency_map_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_frequency_map_init(AgsFrequencyMap *frequency_map);
void ags_frequency_map_set_property(GObject *gobject,
				    guint prop_id,
				    const GValue *value,
				    GParamSpec *param_spec);
void ags_frequency_map_get_property(GObject *gobject,
				    guint prop_id,
				    GValue *value,
				    GParamSpec *param_spec);
void ags_frequency_map_dispose(GObject *gobject);
void ags_frequency_map_finalize(GObject *gobject);

void ags_frequency_map_real_process(AgsFrequencyMap *frequency_map);
void ags_frequency_map_real_factorize(AgsFrequencyMap *frequency_map,
				      AgsFrequencyMap *factorized_frequency_map);
void ags_frequency_map_real_compute_max_likelihood(AgsFrequencyMap *frequency_map,
						   AgsComplex *source,
						   AgsComplex **retval);


/**
 * SECTION:ags_frequency_map
 * @short_description: Contains the audio data for a given frequency
 * @title: AgsFrequencyMap
 * @section_id:
 * @include: ags/audio/ags_frequency_map.h
 *
 * #AgsFrequencyMap stores a buffer of complex numbers. Representing an discrete sampled
 * oscillator wave at a giving frequency.
 */

enum{
  PROP_0,
  PROP_Z_INDEX,
  PROP_WINDOW_COUNT,
  PROP_SAMPLERATE,
  PROP_BUFFER_SIZE,
  PROP_FRAME_COUNT,
  PROP_ATTACK,
  PROP_OSCILLATOR_MODE,
  PROP_FREQ,
};

enum{
  PROCESS,
  FACTORIZE,
  COMPUTE_MAX_LIKELIHOOD,
  LAST_SIGNAL,
};

static gpointer ags_frequency_map_parent_class = NULL;
static guint frequency_map_signals[LAST_SIGNAL];

static pthread_mutex_t ags_frequency_map_class_mutex = PTHREAD_MUTEX_INITIALIZER;

GType
ags_frequency_map_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_frequency_map = 0;

    static const GTypeInfo ags_frequency_map_info = {
      sizeof (AgsFrequencyMapClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_frequency_map_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsFrequencyMap),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_frequency_map_init,
    };

    ags_type_frequency_map = g_type_register_static(G_TYPE_OBJECT,
						    "AgsFrequencyMap",
						    &ags_frequency_map_info,
						    0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_frequency_map);
  }

  return g_define_type_id__volatile;
}

void
ags_frequency_map_class_init(AgsFrequencyMapClass *frequency_map)
{
  GObjectClass *gobject;

  GParamSpec *param_spec;

  ags_frequency_map_parent_class = g_type_class_peek_parent(frequency_map);

  /* GObjectClass */
  gobject = (GObjectClass *) frequency_map;

  gobject->set_property = ags_frequency_map_set_property;
  gobject->get_property = ags_frequency_map_get_property;

  gobject->dispose = ags_frequency_map_dispose;
  gobject->finalize = ags_frequency_map_finalize;

  /* properties */
  /**
   * AgsFrequencyMap:z-index:
   *
   * The z-index as position of this frequency map.
   * 
   * Since: 2.3.0
   */
  param_spec = g_param_spec_uint("z-index",
				 i18n_pspec("z-index as position"),
				 i18n_pspec("The z-index of this frequency map"),
				 0,
				 G_MAXUINT32,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_Z_INDEX,
				  param_spec);

  /**
   * AgsFrequencyMap:window-count:
   *
   * The the window count of total z-indexes.
   * 
   * Since: 2.3.0
   */
  param_spec = g_param_spec_uint("window-count",
				 i18n_pspec("window count"),
				 i18n_pspec("The window count of indexes"),
				 0,
				 G_MAXUINT32,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_WINDOW_COUNT,
				  param_spec);


  /**
   * AgsFrequencyMap:samplerate:
   *
   * The samplerate to be used.
   * 
   * Since: 2.3.0
   */
  param_spec = g_param_spec_uint("samplerate",
				 i18n_pspec("using samplerate"),
				 i18n_pspec("The samplerate to be used"),
				 0,
				 G_MAXUINT32,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SAMPLERATE,
				  param_spec);

  /**
   * AgsFrequencyMap:buffer-size:
   *
   * The buffer size to be used.
   * 
   * Since: 2.3.0
   */
  param_spec = g_param_spec_uint("buffer-size",
				 i18n_pspec("using buffer size"),
				 i18n_pspec("The buffer size to be used"),
				 0,
				 G_MAXUINT32,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_BUFFER_SIZE,
				  param_spec);

  /**
   * AgsFrequencyMap:frame-count:
   *
   * The initial size of audio data.
   * 
   * Since: 2.3.0
   */
  param_spec = g_param_spec_uint("frame-count",
				 i18n_pspec("frame count of audio data"),
				 i18n_pspec("The initial frame count of audio data"),
				 0,
				 G_MAXUINT32,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FRAME_COUNT,
				  param_spec);

  /**
   * AgsFrequencyMap:attack:
   *
   * The attack to be used.
   * 
   * Since: 2.3.0
   */
  param_spec = g_param_spec_uint("attack",
				 i18n_pspec("using attack"),
				 i18n_pspec("The attack to be used"),
				 0,
				 G_MAXUINT32,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_ATTACK,
				  param_spec);

  /**
   * AgsFrequencyMap:oscillator-mode:
   *
   * The oscillator mode to be used.
   * 
   * Since: 2.3.0
   */
  param_spec = g_param_spec_uint("oscillator-mode",
				 i18n_pspec("using oscillator mode"),
				 i18n_pspec("The oscillator mode to be used"),
				 0,
				 G_MAXUINT32,
				 AGS_SYNTH_OSCILLATOR_SIN,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_OSCILLATOR_MODE,
				  param_spec);

  /**
   * AgsFrequencyMap:freq:
   *
   * The freq to be used.
   * 
   * Since: 2.3.0
   */
  param_spec = g_param_spec_double("freq",
				   i18n_pspec("using frequency"),
				   i18n_pspec("The frequency to be used"),
				   -1.0,
				   G_MAXDOUBLE,
				   AGS_FREQUENCY_MAP_DEFAULT_FREQ,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FREQ,
				  param_spec);

  /* AgsFrequencyMapClass */
  frequency_map->process = ags_frequency_map_real_process;

  frequency_map->factorize = ags_frequency_map_real_factorize;

  frequency_map->compute_max_likelihood = ags_frequency_map_real_compute_max_likelihood;

  /* signals */
  /**
   * AgsFrequencyMap::process:
   * @frequency_map: the #AgsFrequencyMap
   *
   * The ::process signal notifies about processed buffer.
   * 
   * Since: 2.3.0
   */
  frequency_map_signals[PROCESS] =
    g_signal_new("process",
		 G_TYPE_FROM_CLASS(frequency_map),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsFrequencyMapClass, process),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);

  /**
   * AgsFrequencyMap::factorize:
   * @frequency_map: the #AgsFrequencyMap
   * @factorized_frequency_map: the factorized #AgsFrequencyMap
   *
   * The ::factorize signal notifies about factorizing map.
   * 
   * Since: 2.3.0
   */
  frequency_map_signals[FACTORIZE] =
    g_signal_new("factorize",
		 G_TYPE_FROM_CLASS(frequency_map),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsFrequencyMapClass, factorize),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__OBJECT,
		 G_TYPE_NONE, 1,
		 G_TYPE_OBJECT);

  /**
   * AgsFrequencyMap::compute-max-likelihood:
   * @frequency_map: the #AgsFrequencyMap
   * @source: the source buffer
   * @retval: return location pointer to buffer
   *
   * The ::compute-max-likelihood signal notifies about max likelihooded commputed.
   * 
   * Since: 2.3.0
   */
  frequency_map_signals[COMPUTE_MAX_LIKELIHOOD] =
    g_signal_new("compute-max-likelihood",
		 G_TYPE_FROM_CLASS(frequency_map),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsFrequencyMapClass, compute_max_likelihood),
		 NULL, NULL,
		 ags_cclosure_marshal_VOID__POINTER_POINTER,
		 G_TYPE_NONE, 2,
		 G_TYPE_POINTER,
		 G_TYPE_POINTER);
}

void
ags_frequency_map_init(AgsFrequencyMap *frequency_map)
{
  pthread_mutex_t *mutex;
  pthread_mutexattr_t *attr;

  frequency_map->flags = 0;

  /* add audio signal mutex */
  frequency_map->obj_mutexattr = 
    attr = (pthread_mutexattr_t *) malloc(sizeof(pthread_mutexattr_t));
  pthread_mutexattr_init(attr);
  pthread_mutexattr_settype(attr,
			    PTHREAD_MUTEX_RECURSIVE);

#ifdef __linux__
  pthread_mutexattr_setprotocol(attr,
				PTHREAD_PRIO_INHERIT);
#endif

  frequency_map->obj_mutex = 
    mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(mutex,
		     attr);  

  /* uuid */
#if 0
  frequency_map->uuid = ags_uuid_alloc();
  ags_uuid_generate(frequency_map->uuid);
#else
  frequency_map->uuid = NULL;  
#endif

  frequency_map->z_index = 0;
  frequency_map->window_count = 0;

  /* presets */
  frequency_map->samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
  frequency_map->buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;

  frequency_map->frame_count = 0;

  frequency_map->attack = 0;

  frequency_map->oscillator_mode = AGS_SYNTH_OSCILLATOR_SIN;

  frequency_map->freq = AGS_FREQUENCY_MAP_DEFAULT_FREQ;

  frequency_map->buffer = ags_stream_alloc(AGS_SOUNDCARD_COMPLEX,
					   frequency_map->buffer_size);
}

void
ags_frequency_map_set_property(GObject *gobject,
			       guint prop_id,
			       const GValue *value,
			       GParamSpec *param_spec)
{
  AgsFrequencyMap *frequency_map;

  pthread_mutex_t *frequency_map_mutex;

  frequency_map = AGS_FREQUENCY_MAP(gobject);

  /* get audio signal mutex */
  frequency_map_mutex = AGS_FREQUENCY_MAP_GET_OBJ_MUTEX(frequency_map);
  
  switch(prop_id){
  case PROP_Z_INDEX:
  {
    guint z_index;

    z_index = g_value_get_uint(value);

    pthread_mutex_lock(frequency_map_mutex);

    frequency_map->z_index = z_index;

    pthread_mutex_unlock(frequency_map_mutex);
  }
  break;
  case PROP_WINDOW_COUNT:
  {
    guint window_count;

    window_count = g_value_get_uint(value);

    pthread_mutex_lock(frequency_map_mutex);

    frequency_map->window_count = window_count;

    pthread_mutex_unlock(frequency_map_mutex);
  }
  break;
  case PROP_SAMPLERATE:
  {
    guint samplerate;

    samplerate = g_value_get_uint(value);

    pthread_mutex_lock(frequency_map_mutex);

    frequency_map->samplerate = samplerate;

    pthread_mutex_unlock(frequency_map_mutex);
  }
  break;
  case PROP_BUFFER_SIZE:
  {
    guint buffer_size;

    buffer_size = g_value_get_uint(value);

    pthread_mutex_lock(frequency_map_mutex);

    if(frequency_map->buffer_size != buffer_size){
      frequency_map->buffer_size = buffer_size;

      ags_stream_free(frequency_map->buffer);
      
      frequency_map->buffer = ags_stream_alloc(AGS_SOUNDCARD_COMPLEX,
					       frequency_map->buffer_size);
    }
    
    pthread_mutex_unlock(frequency_map_mutex);
  }
  break;
  case PROP_FRAME_COUNT:
  {
    guint frame_count;

    frame_count = g_value_get_uint(value);

    pthread_mutex_lock(frequency_map_mutex);

    frequency_map->frame_count = frame_count;

    pthread_mutex_unlock(frequency_map_mutex);
  }
  break;
  case PROP_ATTACK:
  {
    guint attack;

    attack = g_value_get_uint(value);

    pthread_mutex_lock(frequency_map_mutex);

    frequency_map->attack = attack;

    pthread_mutex_unlock(frequency_map_mutex);
  }
  break;
  case PROP_OSCILLATOR_MODE:
  {
    guint oscillator_mode;

    oscillator_mode = g_value_get_uint(value);

    pthread_mutex_lock(frequency_map_mutex);

    frequency_map->oscillator_mode = oscillator_mode;

    pthread_mutex_unlock(frequency_map_mutex);
  }
  break;
  case PROP_FREQ:
  {
    gdouble freq;

    freq = g_value_get_double(value);

    pthread_mutex_lock(frequency_map_mutex);

    frequency_map->freq = freq;

    pthread_mutex_unlock(frequency_map_mutex);
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_frequency_map_get_property(GObject *gobject,
			       guint prop_id,
			       GValue *value,
			       GParamSpec *param_spec)
{
  AgsFrequencyMap *frequency_map;

  pthread_mutex_t *frequency_map_mutex;
  pthread_mutex_t *stream_mutex;

  frequency_map = AGS_FREQUENCY_MAP(gobject);

  /* get audio signal mutex */
  frequency_map_mutex = AGS_FREQUENCY_MAP_GET_OBJ_MUTEX(frequency_map);

  switch(prop_id){
  case PROP_Z_INDEX:
  {
    pthread_mutex_lock(frequency_map_mutex);

    g_value_set_uint(value, frequency_map->z_index);

    pthread_mutex_unlock(frequency_map_mutex);
  }
  break;
  case PROP_WINDOW_COUNT:
  {
    pthread_mutex_lock(frequency_map_mutex);

    g_value_set_uint(value, frequency_map->window_count);

    pthread_mutex_unlock(frequency_map_mutex);
  }
  break;
  case PROP_SAMPLERATE:
  {
    pthread_mutex_lock(frequency_map_mutex);

    g_value_set_uint(value, frequency_map->samplerate);

    pthread_mutex_unlock(frequency_map_mutex);
  }
  break;
  case PROP_BUFFER_SIZE:
  {
    pthread_mutex_lock(frequency_map_mutex);

    g_value_set_uint(value, frequency_map->buffer_size);

    pthread_mutex_unlock(frequency_map_mutex);
  }
  break;
  case PROP_FRAME_COUNT:
  {
    pthread_mutex_lock(frequency_map_mutex);

    g_value_set_uint(value, frequency_map->frame_count);

    pthread_mutex_unlock(frequency_map_mutex);
  }
  break;
  case PROP_ATTACK:
  {
    pthread_mutex_lock(frequency_map_mutex);

    g_value_set_uint(value, frequency_map->attack);

    pthread_mutex_unlock(frequency_map_mutex);
  }
  break;
  case PROP_OSCILLATOR_MODE:
  {
    pthread_mutex_lock(frequency_map_mutex);

    g_value_set_uint(value, frequency_map->oscillator_mode);

    pthread_mutex_unlock(frequency_map_mutex);
  }
  break;
  case PROP_FREQ:
  {
    pthread_mutex_lock(frequency_map_mutex);

    g_value_set_double(value, frequency_map->freq);

    pthread_mutex_unlock(frequency_map_mutex);
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_frequency_map_dispose(GObject *gobject)
{
  AgsFrequencyMap *frequency_map;

  frequency_map = AGS_FREQUENCY_MAP(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_frequency_map_parent_class)->dispose(gobject);
}

void
ags_frequency_map_finalize(GObject *gobject)
{
  AgsFrequencyMap *frequency_map;

  frequency_map = AGS_FREQUENCY_MAP(gobject);


  pthread_mutex_destroy(frequency_map->obj_mutex);
  free(frequency_map->obj_mutex);

  pthread_mutexattr_destroy(frequency_map->obj_mutexattr);
  free(frequency_map->obj_mutexattr);

  ags_uuid_free(frequency_map->uuid);

  /* call parent */
  G_OBJECT_CLASS(ags_frequency_map_parent_class)->finalize(gobject);
}

/**
 * ags_frequency_map_sort_func:
 * @a: an #AgsFrequencyMap
 * @b: an #AgsFrequencyMap
 * 
 * Sort frequency maps.
 * 
 * Returns: 0 if equal, -1 if smaller and 1 if bigger offset
 *
 * Since: 2.3.0
 */
gint
ags_frequency_map_sort_func(gconstpointer a,
			    gconstpointer b)
{
  guint a_samplerate, b_samplerate;
  guint a_buffer_size, b_buffer_size;
  gdouble a_freq, b_freq;
  
  if(a == NULL || b == NULL){
    return(0);
  }

  g_object_get(a,
	       "samplerate", &a_samplerate,
	       "buffer-size", &a_buffer_size,
	       "freq", &a_freq,
	       NULL);
  
  g_object_get(b,
	       "samplerate", &b_samplerate,
	       "buffer-size", &b_buffer_size,
	       "freq", &b_freq,
	       NULL);
  
  if(a_samplerate == b_samplerate){
    if(a_buffer_size == b_buffer_size){
      if(a_freq == b_freq){
	return(0);
      }
      
      if(a_freq < b_freq){
	return(-1);
      }else{
	return(1);
      }
    }

    if(a_buffer_size < b_buffer_size){
      return(-1);
    }else{
      return(1);
    }
  }

  if(a_samplerate < b_samplerate){
    return(-1);
  }else{
    return(1);
  }  
}

/**
 * ags_frequency_map_test_flags:
 * @frequency_map: the #AgsFrequencyMap
 * @flags: the flags
 * 
 * Test @flags to be set on @recall.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 * 
 * Since: 2.3.0
 */
gboolean
ags_frequency_map_test_flags(AgsFrequencyMap *frequency_map, guint flags)
{
  gboolean retval;
  
  pthread_mutex_t *frequency_map_mutex;

  if(!AGS_IS_FREQUENCY_MAP(frequency_map)){
    return(FALSE);
  }
  
  /* get base plugin mutex */
  frequency_map_mutex = AGS_FREQUENCY_MAP_GET_OBJ_MUTEX(frequency_map);

  /* test flags */
  pthread_mutex_lock(frequency_map_mutex);

  retval = ((flags & (frequency_map->flags)) != 0) ? TRUE: FALSE;
  
  pthread_mutex_unlock(frequency_map_mutex);

  return(retval);
}

/**
 * ags_frequency_map_set_flags:
 * @frequency_map: the #AgsFrequencyMap
 * @flags: the flags
 *
 * Set flags.
 * 
 * Since: 2.3.0
 */
void
ags_frequency_map_set_flags(AgsFrequencyMap *frequency_map, guint flags)
{
  pthread_mutex_t *frequency_map_mutex;

  if(!AGS_IS_FREQUENCY_MAP(frequency_map)){
    return;
  }
  
  /* get base plugin mutex */
  frequency_map_mutex = AGS_FREQUENCY_MAP_GET_OBJ_MUTEX(frequency_map);

  /* set flags */
  pthread_mutex_lock(frequency_map_mutex);

  frequency_map->flags |= flags;
  
  pthread_mutex_unlock(frequency_map_mutex);
}

/**
 * ags_frequency_map_unset_flags:
 * @frequency_map: the #AgsFrequencyMap
 * @flags: the flags
 *
 * Unset flags.
 * 
 * Since: 2.3.0
 */
void
ags_frequency_map_unset_flags(AgsFrequencyMap *frequency_map, guint flags)
{
  pthread_mutex_t *frequency_map_mutex;

  if(!AGS_IS_FREQUENCY_MAP(frequency_map)){
    return;
  }
  
  /* get base plugin mutex */
  frequency_map_mutex = AGS_FREQUENCY_MAP_GET_OBJ_MUTEX(frequency_map);

  /* unset flags */
  pthread_mutex_lock(frequency_map_mutex);

  frequency_map->flags &= (~flags);
  
  pthread_mutex_unlock(frequency_map_mutex);
}

void
ags_frequency_map_real_process(AgsFrequencyMap *frequency_map)
{
  switch(frequency_map->oscillator_mode){
  case AGS_SYNTH_OSCILLATOR_SIN:
  {
    ags_synth_util_sin_complex(frequency_map->buffer + frequency_map->attack,
			       frequency_map->freq, 0.0, 1.0,
			       frequency_map->samplerate,
			       0, frequency_map->frame_count);
  }
  break;
  case AGS_SYNTH_OSCILLATOR_SAWTOOTH:
  {
    ags_synth_util_sawtooth_complex(frequency_map->buffer + frequency_map->attack,
				    frequency_map->freq, 0.0, 1.0,
				    frequency_map->samplerate,
				    0, frequency_map->frame_count);
  }
  break;
  case AGS_SYNTH_OSCILLATOR_TRIANGLE:
  {
    ags_synth_util_triangle_complex(frequency_map->buffer + frequency_map->attack,
				    frequency_map->freq, 0.0, 1.0,
				    frequency_map->samplerate,
				    0, frequency_map->frame_count);
  }
  break;
  case AGS_SYNTH_OSCILLATOR_SQUARE:
  {
    ags_synth_util_square_complex(frequency_map->buffer + frequency_map->attack,
				  frequency_map->freq, 0.0, 1.0,
				  frequency_map->samplerate,
				  0, frequency_map->frame_count);
  }
  break;
  case AGS_SYNTH_OSCILLATOR_IMPULSE:
  {
    ags_synth_util_impulse_complex(frequency_map->buffer + frequency_map->attack,
				   frequency_map->freq, 0.0, 1.0,
				   frequency_map->samplerate,
				   0, frequency_map->frame_count);
  }
  break;
  }
}

/**
 * ags_frequency_map_process:
 * @frequency_map: the #AgsFrequencyMap
 * 
 * Process @frequency_map.
 * 
 * Since: 2.3.0
 */
void
ags_frequency_map_process(AgsFrequencyMap *frequency_map)
{
  g_return_if_fail(AGS_IS_FREQUENCY_MAP(frequency_map));
  g_object_ref(G_OBJECT(frequency_map));
  g_signal_emit(G_OBJECT(frequency_map),
		frequency_map_signals[PROCESS], 0);
  g_object_unref(G_OBJECT(frequency_map));
}

void
ags_frequency_map_real_factorize(AgsFrequencyMap *frequency_map,
				 AgsFrequencyMap *factorized_frequency_map)
{
  complex z;  
  guint i;

  for(i = frequency_map->attack; i < frequency_map->frame_count; i++){
    z = ags_complex_get(factorized_frequency_map->buffer[i]) * ags_complex_get(frequency_map->buffer[i]);
    
    ags_complex_set(factorized_frequency_map->buffer[i],
		    z);
  }
}

/**
 * ags_frequency_map_factorize:
 * @frequency_map: the #AgsFrequencyMap
 * @factorized_frequency_map: the factorized #AgsFrequencyMap
 * 
 * Factorize @frequency_map.
 * 
 * Since: 2.3.0
 */
void
ags_frequency_map_factorize(AgsFrequencyMap *frequency_map,
			    AgsFrequencyMap *factorized_frequency_map)
{
  g_return_if_fail(AGS_IS_FREQUENCY_MAP(frequency_map) &&
		   AGS_IS_FREQUENCY_MAP(factorized_frequency_map));
  g_object_ref(G_OBJECT(frequency_map));
  g_signal_emit(G_OBJECT(frequency_map),
		frequency_map_signals[FACTORIZE], 0,
		factorized_frequency_map);
  g_object_unref(G_OBJECT(frequency_map));
}

void
ags_frequency_map_real_compute_max_likelihood(AgsFrequencyMap *frequency_map,
					      AgsComplex *source,
					      AgsComplex **retval)
{
  complex z;
  guint i;

  for(i = 0; i < frequency_map->buffer_size; i++){
    z = ags_complex_get(frequency_map->buffer[i]) * ags_complex_get(source[i]);
        
    ags_complex_set(retval[0][i],
		    z);
  }
}

/**
 * ags_frequency_map_compute_max_likelihood:
 * @frequency_map: the #AgsFrequencyMap
 * @source: the source as #AgsComplex buffer
 * @retval: the return location of result as #AgsComplex buffer
 * 
 * Compute maximum likelihood @source and store in @retval.
 * 
 * Since: 2.3.0
 */
void
ags_frequency_map_compute_max_likelihood(AgsFrequencyMap *frequency_map,
					 AgsComplex *source,
					 AgsComplex **retval)
{
  g_return_if_fail(AGS_IS_FREQUENCY_MAP(frequency_map) &&
		   source != NULL &&
		   retval != NULL &&
		   retval[0] != NULL);
  g_object_ref(G_OBJECT(frequency_map));
  g_signal_emit(G_OBJECT(frequency_map),
		frequency_map_signals[COMPUTE_MAX_LIKELIHOOD], 0,
		source,
		retval);
  g_object_unref(G_OBJECT(frequency_map));
}

/**
 * ags_frequency_map_new:
 *
 *
 * Returns: a new #AgsFrequencyMap
 *
 * Since: 2.3.0
 */
AgsFrequencyMap*
ags_frequency_map_new()
{
  AgsFrequencyMap *frequency_map;

  frequency_map = (AgsFrequencyMap *) g_object_new(AGS_TYPE_FREQUENCY_MAP,
						   NULL);

  return(frequency_map);
}
