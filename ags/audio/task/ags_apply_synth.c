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

#include <ags/audio/task/ags_apply_synth.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_input.h>
#include <ags/audio/ags_audio_signal.h>
#include <ags/audio/ags_synth_generator.h>
#include <ags/audio/ags_audio_buffer_util.h>
#include <ags/audio/ags_synth_util.h>

#include <math.h>

#include <ags/i18n.h>

void ags_apply_synth_class_init(AgsApplySynthClass *apply_synth);
void ags_apply_synth_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_apply_synth_init(AgsApplySynth *apply_synth);
void ags_apply_synth_set_property(GObject *gobject,
				  guint prop_id,
				  const GValue *value,
				  GParamSpec *param_spec);
void ags_apply_synth_get_property(GObject *gobject,
				  guint prop_id,
				  GValue *value,
				  GParamSpec *param_spec);
void ags_apply_synth_connect(AgsConnectable *connectable);
void ags_apply_synth_disconnect(AgsConnectable *connectable);
void ags_apply_synth_dispose(GObject *gobject);
void ags_apply_synth_finalize(GObject *gobject);

void ags_apply_synth_launch(AgsTask *task);

/**
 * SECTION:ags_apply_synth
 * @short_description: apply synth to channel
 * @title: AgsApplySynth
 * @section_id:
 * @include: ags/audio/task/ags_apply_synth.h
 *
 * The #AgsApplySynth task apply the specified synth to channel.
 */

static gpointer ags_apply_synth_parent_class = NULL;
static AgsConnectableInterface *ags_apply_synth_parent_connectable_interface;

enum{
  PROP_0,
  PROP_START_CHANNEL,
  PROP_COUNT,
  PROP_FIXED_LENGTH,
  PROP_WAVE,
  PROP_BASE_NOTE,
  PROP_FREQUENCY,
  PROP_VOLUME,
  PROP_PHASE,
  PROP_START_FREQUENCY,
  PROP_DELAY,
  PROP_ATTACK,
  PROP_FRAME_COUNT,
  PROP_LOOP_START,
  PROP_LOOP_END,
  PROP_DO_SYNC,
  PROP_SYNC_MODE,
  PROP_SYNC_POINT,
  PROP_SYNC_POINT_COUNT,
};

GType
ags_apply_synth_get_type()
{
  static GType ags_type_apply_synth = 0;

  if(!ags_type_apply_synth){
    static const GTypeInfo ags_apply_synth_info = {
      sizeof (AgsApplySynthClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_apply_synth_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsApplySynth),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_apply_synth_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_apply_synth_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_apply_synth = g_type_register_static(AGS_TYPE_TASK,
						  "AgsApplySynth",
						  &ags_apply_synth_info,
						  0);

    g_type_add_interface_static(ags_type_apply_synth,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_apply_synth);
}

void
ags_apply_synth_class_init(AgsApplySynthClass *apply_synth)
{
  GObjectClass *gobject;
  AgsTaskClass *task;
  GParamSpec *param_spec;

  ags_apply_synth_parent_class = g_type_class_peek_parent(apply_synth);

  /* GObjectClass */
  gobject = (GObjectClass *) apply_synth;

  gobject->set_property = ags_apply_synth_set_property;
  gobject->get_property = ags_apply_synth_get_property;

  gobject->dispose = ags_apply_synth_dispose;
  gobject->finalize = ags_apply_synth_finalize;

  /* properties */
  /**
   * AgsApplySynth:start-channel:
   *
   * The assigned #AgsChannel
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_object("start-channel",
				   i18n_pspec("start channel of apply synth"),
				   i18n_pspec("The start channel of apply synth task"),
				   AGS_TYPE_CHANNEL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_START_CHANNEL,
				  param_spec);
  
  /**
   * AgsApplySynth:count:
   *
   * The count of channels to apply.
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_uint("count",
				 i18n_pspec("count of channels"),
				 i18n_pspec("The count of channels to apply"),
				 0,
				 G_MAXUINT,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_COUNT,
				  param_spec);

  /**
   * AgsApplySynth:fixed-length:
   *
   * If apply fixed length
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_boolean("fixed-length",
				    i18n_pspec("fixed length"),
				    i18n_pspec("Use fixed length to creat audio data"),
				    TRUE,
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FIXED_LENGTH,
				  param_spec);

  /**
   * AgsApplySynth:wave:
   *
   * The wave of wave.
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_uint("wave",
				 i18n_pspec("wave"),
				 i18n_pspec("The wave"),
				 0,
				 G_MAXUINT,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_WAVE,
				  param_spec);

  /**
   * AgsApplySynth:base-note:
   *
   * The base-note to ramp up from.
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_double("base-note",
				   i18n_pspec("base note"),
				   i18n_pspec("The base note to ramp up from"),
				   -78.0,
				   78.0,
				   -48.0,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_BASE_NOTE,
				  param_spec);

  /**
   * AgsApplySynth:frequency:
   *
   * The frequency of wave.
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_double("frequency",
				   i18n_pspec("frequency of wave"),
				   i18n_pspec("The frequency of wave"),
				   0.0,
				   G_MAXDOUBLE,
				   0.0,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FREQUENCY,
				  param_spec);

  /**
   * AgsApplySynth:volume:
   *
   * The volume of wave.
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_double("volume",
				   i18n_pspec("volume of wave"),
				   i18n_pspec("The volume of wave"),
				   0.0,
				   G_MAXDOUBLE,
				   0.0,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_VOLUME,
				  param_spec);
  /**
   * AgsApplySynth:phase:
   *
   * The phase of wave.
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_double("phase",
				   i18n_pspec("phase of wave"),
				   i18n_pspec("The phase of wave"),
				   0.0,
				   G_MAXDOUBLE,
				   0.0,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PHASE,
				  param_spec);
  
  /**
   * AgsApplySynth:start-frequency:
   *
   * The start frequency as base of wave.
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_double("start-frequency",
				   i18n_pspec("start frequency"),
				   i18n_pspec("The start frequency"),
				   0.0,
				   G_MAXDOUBLE,
				   0.0,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_START_FREQUENCY,
				  param_spec);

  /**
   * AgsApplySynth:delay:
   *
   * The delay.
   * 
   * Since: 1.1.0
   */
  param_spec = g_param_spec_double("delay",
				   i18n_pspec("delay"),
				   i18n_pspec("The delay"),
				   0.0,
				   G_MAXDOUBLE,
				   0.0,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DELAY,
				  param_spec);

  /**
   * AgsApplySynth:attack:
   *
   * The attack of audio data.
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_uint("attack",
				 i18n_pspec("attack of audio data"),
				 i18n_pspec("The attack of audio data"),
				 0,
				 G_MAXUINT,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_ATTACK,
				  param_spec);

  /**
   * AgsApplySynth:frame-count:
   *
   * The frame count of audio data.
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_uint("frame-count",
				 i18n_pspec("frame count of audio data"),
				 i18n_pspec("The frame count of audio data"),
				 0,
				 G_MAXUINT,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FRAME_COUNT,
				  param_spec);

  
  /**
   * AgsApplySynth:loop-start:
   *
   * The loop start of audio data.
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_uint("loop-start",
				 i18n_pspec("loop start of audio data"),
				 i18n_pspec("The loop start of audio data"),
				 0,
				 G_MAXUINT,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_LOOP_START,
				  param_spec);

  /**
   * AgsApplySynth:loop-end:
   *
   * The loop end of audio data.
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_uint("loop-end",
				 i18n_pspec("loop end of audio data"),
				 i18n_pspec("The loop end of audio data"),
				 0,
				 G_MAXUINT,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_LOOP_END,
				  param_spec);

  /**
   * AgsApplySynth:do-sync:
   *
   * If do sync
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_boolean("do-sync",
				    i18n_pspec("do sync"),
				    i18n_pspec("Do sync by zero-cross detection"),
				    TRUE,
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DO_SYNC,
				  param_spec);

  /**
   * AgsApplySynth:sync-mode:
   *
   * The sync mode to use.
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_uint("sync-mode",
				 i18n_pspec("sync mode"),
				 i18n_pspec("The sync mode to use"),
				 0,
				 G_MAXUINT,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNC_MODE,
				  param_spec);
  
  /**
   * AgsApplySynth:sync-point:
   *
   * The sync point to use.
   * 
   * Since: 1.1.0
   */
  param_spec = g_param_spec_pointer("sync-point",
				    i18n_pspec("sync point"),
				    i18n_pspec("The sync point to use"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNC_POINT,
				  param_spec);

  /**
   * AgsApplySynth:sync-point-count:
   *
   * The sync point count of audio data.
   * 
   * Since: 1.1.0
   */
  param_spec = g_param_spec_uint("sync-point-count",
				 i18n_pspec("sync point count of audio data"),
				 i18n_pspec("The sync point count of audio data"),
				 0,
				 G_MAXUINT,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNC_POINT_COUNT,
				  param_spec);

  
  /* AgsTaskClass */
  task = (AgsTaskClass *) apply_synth;

  task->launch = ags_apply_synth_launch;
}

void
ags_apply_synth_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_apply_synth_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_apply_synth_connect;
  connectable->disconnect = ags_apply_synth_disconnect;
}

void
ags_apply_synth_init(AgsApplySynth *apply_synth)
{
  apply_synth->start_channel = NULL;
  apply_synth->count = 0;

  apply_synth->fixed_length = TRUE;

  apply_synth->wave = AGS_APPLY_SYNTH_INVALID;
  apply_synth->base_note = -48.0;

  apply_synth->frequency = 0.0;
  apply_synth->volume = 1.0;
  
  apply_synth->phase = 0.0;
  apply_synth->start_frequency = 0.0;

  apply_synth->delay = 0.0;
  apply_synth->attack = 0;
  
  apply_synth->frame_count = 0;
  apply_synth->loop_start = 0;
  apply_synth->loop_end = 0;

  apply_synth->do_sync = FALSE;
  apply_synth->sync_mode = 0;

  apply_synth->sync_point = NULL;
  apply_synth->sync_point_count = 0;
}

void
ags_apply_synth_set_property(GObject *gobject,
			     guint prop_id,
			     const GValue *value,
			     GParamSpec *param_spec)
{
  AgsApplySynth *apply_synth;

  apply_synth = AGS_APPLY_SYNTH(gobject);

  switch(prop_id){
  case PROP_START_CHANNEL:
    {
      AgsChannel *start_channel;

      start_channel = (AgsChannel *) g_value_get_object(value);

      if(apply_synth->start_channel == (GObject *) start_channel){
	return;
      }

      if(apply_synth->start_channel != NULL){
	g_object_unref(apply_synth->start_channel);
      }

      if(start_channel != NULL){
	g_object_ref(start_channel);
      }

      apply_synth->start_channel = (GObject *) start_channel;
    }
    break;
  case PROP_COUNT:
    {
      apply_synth->count = g_value_get_uint(value);
    }
    break;
  case PROP_FIXED_LENGTH:
    {
      apply_synth->fixed_length = g_value_get_boolean(value);
    }
    break;
  case PROP_WAVE:
    {
      apply_synth->wave = g_value_get_uint(value);
    }
    break;
  case PROP_BASE_NOTE:
    {
      apply_synth->base_note = g_value_get_double(value);
    }
    break;
  case PROP_FREQUENCY:
    {
      apply_synth->frequency = g_value_get_double(value);
    }
    break;
  case PROP_VOLUME:
    {
      apply_synth->volume = g_value_get_double(value);
    }
    break;
  case PROP_PHASE:
    {
      apply_synth->phase = g_value_get_double(value);
    }
    break;
  case PROP_START_FREQUENCY:
    {
      apply_synth->start_frequency = g_value_get_double(value);
    }
    break;
  case PROP_DELAY:
    {
      apply_synth->delay = g_value_get_double(value);
    }
    break;
  case PROP_ATTACK:
    {
      apply_synth->attack = g_value_get_uint(value);
    }
    break;
  case PROP_FRAME_COUNT:
    {
      apply_synth->frame_count = g_value_get_uint(value);
    }
    break;
  case PROP_LOOP_START:
    {
      apply_synth->loop_start = g_value_get_uint(value);
    }
    break;
  case PROP_LOOP_END:
    {
      apply_synth->loop_end = g_value_get_uint(value);
    }
    break;
  case PROP_DO_SYNC:
    {
      apply_synth->do_sync = g_value_get_boolean(value);
    }
    break;
  case PROP_SYNC_MODE:
    {
      apply_synth->sync_mode = g_value_get_uint(value);
    }
    break;
  case PROP_SYNC_POINT:
    {
      gpointer sync_point;
      
      sync_point = g_value_get_pointer(value);


      if(apply_synth->sync_point != NULL){
	guint i;

	for(i = 0; i < apply_synth->sync_point_count; i++){
	  ags_complex_free(apply_synth->sync_point[i]);
	}
	
	g_free(apply_synth->sync_point);
      }
      
      apply_synth->sync_point = sync_point;
    }
    break;
  case PROP_SYNC_POINT_COUNT:
    {
      apply_synth->sync_point_count = g_value_get_uint(value);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_apply_synth_get_property(GObject *gobject,
			     guint prop_id,
			     GValue *value,
			     GParamSpec *param_spec)
{
  AgsApplySynth *apply_synth;

  apply_synth = AGS_APPLY_SYNTH(gobject);

  switch(prop_id){
  case PROP_START_CHANNEL:
    {
      g_value_set_object(value, apply_synth->start_channel);
    }
    break;
  case PROP_COUNT:
    {
      g_value_set_uint(value, apply_synth->count);
    }
    break;
  case PROP_FIXED_LENGTH:
    {
      g_value_set_boolean(value, apply_synth->fixed_length);
    }
    break;
  case PROP_WAVE:
    {
      g_value_set_uint(value, apply_synth->wave);
    }
    break;
  case PROP_BASE_NOTE:
    {
      g_value_set_double(value, apply_synth->base_note);
    }
    break;
  case PROP_FREQUENCY:
    {
      g_value_set_double(value, apply_synth->frequency);
    }
    break;
  case PROP_VOLUME:
    {
      g_value_set_double(value, apply_synth->volume);
    }
    break;
  case PROP_PHASE:
    {
      g_value_set_double(value, apply_synth->phase);
    }
    break;
  case PROP_START_FREQUENCY:
    {
      g_value_set_double(value, apply_synth->start_frequency);
    }
    break;
  case PROP_DELAY:
    {
      g_value_set_double(value, apply_synth->delay);
    }
    break;
  case PROP_ATTACK:
    {
      g_value_set_uint(value, apply_synth->attack);
    }
    break;
  case PROP_FRAME_COUNT:
    {
      g_value_set_uint(value, apply_synth->frame_count);
    }
    break;
  case PROP_LOOP_START:
    {
      g_value_set_uint(value, apply_synth->loop_start);
    }
    break;
  case PROP_LOOP_END:
    {
      g_value_set_uint(value, apply_synth->loop_end);
    }
    break;
  case PROP_DO_SYNC:
    {
      g_value_set_boolean(value, apply_synth->do_sync);
    }
    break;
  case PROP_SYNC_MODE:
    {
      g_value_set_uint(value, apply_synth->sync_mode);
    }
    break;
  case PROP_SYNC_POINT:
    {
      g_value_set_uint(value, apply_synth->sync_point_count);
    }
    break;
  case PROP_SYNC_POINT_COUNT:
    {
      g_value_set_pointer(value, apply_synth->sync_point);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_apply_synth_connect(AgsConnectable *connectable)
{
  ags_apply_synth_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_apply_synth_disconnect(AgsConnectable *connectable)
{
  ags_apply_synth_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_apply_synth_dispose(GObject *gobject)
{
  AgsApplySynth *apply_synth;

  apply_synth = AGS_APPLY_SYNTH(gobject);

  if(apply_synth->start_channel != NULL){
    g_object_unref(apply_synth->start_channel);
    
    apply_synth->start_channel = NULL;
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_apply_synth_parent_class)->dispose(gobject);
}

void
ags_apply_synth_finalize(GObject *gobject)
{
  AgsApplySynth *apply_synth;

  apply_synth = AGS_APPLY_SYNTH(gobject);

  if(apply_synth->start_channel != NULL){
    g_object_unref(apply_synth->start_channel);    
  }

  if(apply_synth->sync_point != NULL){
    guint i;

    for(i = 0; i < apply_synth->sync_point_count; i++){
      ags_complex_free(apply_synth->sync_point[i]);
    }
	
    g_free(apply_synth->sync_point);
  }

  /* call parent */
  G_OBJECT_CLASS(ags_apply_synth_parent_class)->finalize(gobject);
}

void
ags_apply_synth_launch(AgsTask *task)
{
  AgsApplySynth *apply_synth;

  AgsAudio *audio;
  AgsChannel *channel, *input;
  AgsRecycling *first_recycling;
  AgsAudioSignal *audio_signal;
  AgsSynthGenerator *synth_generator;

  AgsMutexManager *mutex_manager;
  
  GList *stream;

  gchar *str;
  
  gint wave;
  guint oscillator;

  guint samplerate;
  guint format;
  guint buffer_size;
  gdouble note;
  double phase, frequency, volume;
  gdouble delay;
  guint attack;
  guint frame_count;
  gboolean do_sync;
  AgsComplex **sync_point;
  guint sync_point_count;

  guint audio_flags;
  guint i;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *audio_mutex;
  pthread_mutex_t *channel_mutex;
  pthread_mutex_t *input_mutex;
  pthread_mutex_t *recycling_mutex;

  /* get mutex manager and application mutex */
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);
  
  apply_synth = AGS_APPLY_SYNTH(task);

  channel = apply_synth->start_channel;

  /* some settings */  
  frame_count = apply_synth->frame_count;

  delay = apply_synth->delay;
  attack = apply_synth->attack;

  wave = (gint) apply_synth->wave;

  switch(wave){
  case AGS_APPLY_SYNTH_SIN:
    oscillator = AGS_SYNTH_GENERATOR_OSCILLATOR_SIN;
    break;
  case AGS_APPLY_SYNTH_SAW:
    oscillator = AGS_SYNTH_GENERATOR_OSCILLATOR_SAWTOOTH;
    break;
  case AGS_APPLY_SYNTH_TRIANGLE:
    oscillator = AGS_SYNTH_GENERATOR_OSCILLATOR_TRIANGLE;
    break;
  case AGS_APPLY_SYNTH_SQUARE:
    oscillator = AGS_SYNTH_GENERATOR_OSCILLATOR_SQUARE;
    break;
  default:
    g_warning("ags_apply_synth_launch_write: warning no wave selected\n");
  }

#ifdef AGS_DEBUG
  g_message("wave = %d\n", wave);
#endif
  
  frequency = apply_synth->frequency;
  phase = apply_synth->phase;  
  volume = apply_synth->volume;

  sync_point = apply_synth->sync_point;
  sync_point_count = apply_synth->sync_point_count;

  /* get channel mutex */
  pthread_mutex_lock(application_mutex);

  channel_mutex = ags_mutex_manager_lookup(mutex_manager,
					 (GObject *) channel);

  pthread_mutex_unlock(application_mutex);

  /* get some fields */
  pthread_mutex_lock(channel_mutex);

  audio = channel->audio;

  pthread_mutex_unlock(channel_mutex);

  /* get audio mutex */
  pthread_mutex_lock(application_mutex);

  audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					 (GObject *) audio);

  pthread_mutex_unlock(application_mutex);

  /* get some fields */
  pthread_mutex_lock(audio_mutex);

  audio_flags = audio->flags;
  
  pthread_mutex_unlock(audio_mutex);

  /* fill */
  if(AGS_IS_INPUT(channel)){
    if((AGS_AUDIO_HAS_NOTATION & (audio_flags)) != 0){      
      /* compute */
      channel = apply_synth->start_channel;
      
      for(i = 0; channel != NULL && i < apply_synth->count; i++){
	/* get channel mutex */
	pthread_mutex_lock(application_mutex);

	channel_mutex = ags_mutex_manager_lookup(mutex_manager,
						 (GObject *) channel);

	pthread_mutex_unlock(application_mutex);

	/* get some fields */
	pthread_mutex_lock(channel_mutex);

	first_recycling = channel->first_recycling;
	
	synth_generator = AGS_INPUT(channel)->synth_generator;
	
	pthread_mutex_unlock(channel_mutex);

	/* get recycling mutex */
	pthread_mutex_lock(application_mutex);

	recycling_mutex = ags_mutex_manager_lookup(mutex_manager,
						 (GObject *) first_recycling);

	pthread_mutex_unlock(application_mutex);

	/* creat synth generator if needed */
	if(synth_generator == NULL){
	  guint samplerate;
	  guint buffer_size;
	  guint format;
	  
	  synth_generator = (GObject *) ags_synth_generator_new();

	  /* get template */
	  pthread_mutex_lock(recycling_mutex);
	  
	  audio_signal = ags_audio_signal_get_template(first_recycling->audio_signal);

	  samplerate = audio_signal->samplerate;
	  buffer_size = audio_signal->buffer_size;
	  format = audio_signal->format;
	  
	  pthread_mutex_unlock(recycling_mutex);

	  /* set properties */
	  g_object_set(synth_generator,
		       "samplerate", samplerate,
		       "buffer-size", buffer_size,
		       "format", format,
		       NULL);

	  /* set synth generator */
	  pthread_mutex_lock(channel_mutex);
	
	  AGS_INPUT(channel)->synth_generator = synth_generator;
	  
	  pthread_mutex_unlock(channel_mutex);
	}

	/* set properties */
	pthread_mutex_lock(channel_mutex);
	
	g_object_set(synth_generator,
		     "frame-count", frame_count,
		     "delay", delay,
		     "attack", attack,
		     "oscillator", oscillator,
		     "frequency", frequency,
		     "phase", phase,
		     "volume", volume,
		     NULL);

	pthread_mutex_unlock(channel_mutex);
	
	/* get template */
	pthread_mutex_lock(recycling_mutex);
	
	audio_signal = ags_audio_signal_get_template(first_recycling->audio_signal);

	pthread_mutex_unlock(recycling_mutex);
	  
	/* compute audio signal */
	note = (apply_synth->base_note) + (gdouble) i;
	  
	pthread_mutex_lock(channel_mutex);

	ags_synth_generator_compute_extended(synth_generator,
					     audio_signal,
					     note,
					     sync_point,
					     sync_point_count);

	pthread_mutex_unlock(channel_mutex);

	/* iterate */
	pthread_mutex_lock(channel_mutex);
	
	channel = channel->next;

	pthread_mutex_unlock(channel_mutex);
      }
    }else{
      /* compute */
      channel = apply_synth->start_channel;
	
      for(i = 0; channel != NULL && i < apply_synth->count; i++){
	/* get channel mutex */
	pthread_mutex_lock(application_mutex);

	channel_mutex = ags_mutex_manager_lookup(mutex_manager,
						 (GObject *) channel);

	pthread_mutex_unlock(application_mutex);

	/* get some fields */
	pthread_mutex_lock(channel_mutex);

	first_recycling = channel->first_recycling;
	
	synth_generator = AGS_INPUT(channel)->synth_generator;
	
	pthread_mutex_unlock(channel_mutex);

	/* get recycling mutex */
	pthread_mutex_lock(application_mutex);

	recycling_mutex = ags_mutex_manager_lookup(mutex_manager,
						 (GObject *) first_recycling);

	pthread_mutex_unlock(application_mutex);

	/* creat synth generator if needed */
	if(synth_generator == NULL){
	  guint samplerate;
	  guint buffer_size;
	  guint format;

	  synth_generator = (GObject *) ags_synth_generator_new();

	  /* get template */
	  pthread_mutex_lock(recycling_mutex);

	  audio_signal = ags_audio_signal_get_template(first_recycling->audio_signal);

	  samplerate = audio_signal->samplerate;
	  buffer_size = audio_signal->buffer_size;
	  format = audio_signal->format;
	  
	  pthread_mutex_unlock(recycling_mutex);

	  /* set properties */
	  g_object_set(synth_generator,
		       "samplerate", samplerate,
		       "buffer-size", buffer_size,
		       "format", format,
		       NULL);

	  /* set synth generator */
	  pthread_mutex_lock(channel_mutex);
	
	  AGS_INPUT(channel)->synth_generator = synth_generator;
	  
	  pthread_mutex_unlock(channel_mutex);
	}

	/* set properties */
	pthread_mutex_lock(channel_mutex);

	g_object_set(synth_generator,
		     "frame-count", frame_count,
		     "delay", delay,
		     "attack", attack,
		     "oscillator", oscillator,
		     "frequency", frequency,
		     "phase", phase,
		     "volume", volume,
		     NULL);

	pthread_mutex_unlock(channel_mutex);
	
	/* get template */
	pthread_mutex_lock(recycling_mutex);
	
	audio_signal = ags_audio_signal_get_template(first_recycling->audio_signal);

	pthread_mutex_unlock(recycling_mutex);

	/* compute audio signal */
	note = (apply_synth->base_note);
	  
	pthread_mutex_lock(channel_mutex);

	ags_synth_generator_compute_extended(AGS_INPUT(channel)->synth_generator,
					     audio_signal,
					     note,
					     sync_point,
					     sync_point_count);

	pthread_mutex_unlock(channel_mutex);

	/* iterate */
	pthread_mutex_lock(channel_mutex);
	
	channel = channel->next;

	pthread_mutex_unlock(channel_mutex);
      }
    }
  }else{
    if((AGS_AUDIO_HAS_NOTATION & (audio_flags)) == 0){
      pthread_mutex_lock(audio_mutex);

      input = audio->input;

      pthread_mutex_unlock(audio_mutex);
	
      while(input != NULL){
	/* get channel mutex */
	pthread_mutex_lock(application_mutex);

	input_mutex = ags_mutex_manager_lookup(mutex_manager,
					       (GObject *) input);

	pthread_mutex_unlock(application_mutex);
	
	/* get some fields */
	pthread_mutex_lock(input_mutex);

	first_recycling = input->first_recycling;
	
	synth_generator = AGS_INPUT(input)->synth_generator;
	
	pthread_mutex_unlock(input_mutex);

	/* get recycling mutex */
	pthread_mutex_lock(application_mutex);

	recycling_mutex = ags_mutex_manager_lookup(mutex_manager,
						 (GObject *) first_recycling);

	pthread_mutex_unlock(application_mutex);

	/* creat synth generator if needed */
	if(synth_generator == NULL){
	  synth_generator = (GObject *) ags_synth_generator_new();

	  /* get template */
	  pthread_mutex_lock(recycling_mutex);
	  
	  audio_signal = ags_audio_signal_get_template(first_recycling->audio_signal);

	  samplerate = audio_signal->samplerate;
	  buffer_size = audio_signal->buffer_size;
	  format = audio_signal->format;
	  
	  pthread_mutex_unlock(recycling_mutex);

	  /* set properties */
	  g_object_set(synth_generator,
		       "samplerate", samplerate,
		       "buffer-size", buffer_size,
		       "format", format,
		       NULL);

	  /* set synth generator */
	  pthread_mutex_lock(input_mutex);
	
	  AGS_INPUT(input)->synth_generator = synth_generator;
	  
	  pthread_mutex_unlock(input_mutex);
	}

	/* set properties */
	g_object_set(AGS_INPUT(input)->synth_generator,
		     "frame-count", frame_count,
		     "delay", delay, 
		     "attack", attack,
		     "oscillator", oscillator,
		     "frequency", frequency,
		     "phase", phase,
		     "volume", volume,
		     NULL);

	/* compute audio signal */
	channel = apply_synth->start_channel;
	
	for(i = 0; channel != NULL && i < apply_synth->count; i++){
	  /* get channel mutex */
	  pthread_mutex_lock(application_mutex);

	  channel_mutex = ags_mutex_manager_lookup(mutex_manager,
						   (GObject *) channel);

	  pthread_mutex_unlock(application_mutex);

	  /* get some fields */
	  pthread_mutex_lock(channel_mutex);

	  first_recycling = channel->first_recycling;
	
	  pthread_mutex_unlock(channel_mutex);

	  /* get template */
	  pthread_mutex_lock(recycling_mutex);
	
	  audio_signal = ags_audio_signal_get_template(first_recycling->audio_signal);

	  pthread_mutex_unlock(recycling_mutex);

	  /* compute audio signal */
 	  note = (apply_synth->base_note) + (gdouble) i;
	  
	  pthread_mutex_lock(channel_mutex);
	  
	  ags_synth_generator_compute_extended(synth_generator,
					       audio_signal,
					       note,
					       sync_point,
					       sync_point_count);
	  
	  pthread_mutex_unlock(channel_mutex);

	  /* iterate */
	  pthread_mutex_lock(channel_mutex);
	
	  channel = channel->next;

	  pthread_mutex_unlock(channel_mutex);
	}
    
	/* iterate */
	pthread_mutex_lock(input_mutex);
	
	input = input->next;

	pthread_mutex_unlock(input_mutex);
      }
    }      
  }  
}

/**
 * ags_apply_synth_new:
 * @start_channel: the start channel
 * @count: the count of lines
 * @wave: the selected wave
 * @attack: the attack
 * @frame_count: frame count
 * @frequency: frequency
 * @phase: the phase
 * @start_frequency: base frequency
 * @volume: volume
 * @loop_start: loop start
 * @loop_end: loop end
 *
 * Creates an #AgsApplySynth.
 *
 * Returns: an new #AgsApplySynth.
 *
 * Since: 1.0.0
 */
AgsApplySynth*
ags_apply_synth_new(AgsChannel *start_channel, guint count,
		    guint wave,
		    guint attack, guint frame_count,
		    gdouble frequency, gdouble phase, gdouble start_frequency,
		    gdouble volume,
		    guint loop_start, guint loop_end)
{
  AgsApplySynth *apply_synth;

  apply_synth = (AgsApplySynth *) g_object_new(AGS_TYPE_APPLY_SYNTH,
					       "start-channel", start_channel,
					       "count", count,
					       "wave", wave,
					       "attack", attack,
					       "frame-count", frame_count,
					       "frequency", frequency,
					       "phase", phase,
					       "start-frequency", start_frequency,
					       "volume", volume,
					       "loop-start", loop_start,
					       "loop-end", loop_end,
					       NULL);


  return(apply_synth);
}
