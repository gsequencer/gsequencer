/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2023 Joël Krähemann
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
void ags_apply_synth_init(AgsApplySynth *apply_synth);
void ags_apply_synth_set_property(GObject *gobject,
				  guint prop_id,
				  const GValue *value,
				  GParamSpec *param_spec);
void ags_apply_synth_get_property(GObject *gobject,
				  guint prop_id,
				  GValue *value,
				  GParamSpec *param_spec);
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

enum{
  PROP_0,
  PROP_SYNTH_GENERATOR,
  PROP_START_CHANNEL,
  PROP_BASE_NOTE,
  PROP_COUNT,
  PROP_REQUESTED_FRAME_COUNT,
};

GType
ags_apply_synth_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_apply_synth = 0;

    static const GTypeInfo ags_apply_synth_info = {
      sizeof(AgsApplySynthClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_apply_synth_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsApplySynth),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_apply_synth_init,
    };

    ags_type_apply_synth = g_type_register_static(AGS_TYPE_TASK,
						  "AgsApplySynth",
						  &ags_apply_synth_info,
						  0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_apply_synth);
  }

  return g_define_type_id__volatile;
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
   * AgsApplySynth:synth-generator:
   *
   * The assigned #AgsSynthGenerator
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("synth-generator",
				   i18n_pspec("synth generator"),
				   i18n_pspec("The synth generator to apply"),
				   AGS_TYPE_SYNTH_GENERATOR,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_GENERATOR,
				  param_spec);

  /**
   * AgsApplySynth:start-channel:
   *
   * The assigned #AgsChannel
   * 
   * Since: 3.0.0
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
   * AgsApplySynth:base-note:
   *
   * The base-note to ramp up from.
   * 
   * Since: 3.0.0
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
   * AgsApplySynth:count:
   *
   * The count of channels to apply.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_uint("count",
				 i18n_pspec("count of channels"),
				 i18n_pspec("The count of channels to apply"),
				 0,
				 G_MAXUINT32,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_COUNT,
				  param_spec);

  /**
   * AgsApplySynth:requested-frame-count:
   *
   * The frame count of audio signal to apply.
   * 
   * Since: 3.3.0
   */
  param_spec = g_param_spec_uint("requested-frame-count",
				 i18n_pspec("requested frame count"),
				 i18n_pspec("The requested frame count to apply"),
				 0,
				 G_MAXUINT32,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_REQUESTED_FRAME_COUNT,
				  param_spec);
  
  /* AgsTaskClass */
  task = (AgsTaskClass *) apply_synth;

  task->launch = ags_apply_synth_launch;
}

void
ags_apply_synth_init(AgsApplySynth *apply_synth)
{
  apply_synth->synth_generator = NULL;

  apply_synth->start_channel = NULL;
  apply_synth->count = 0;

  apply_synth->requested_frame_count = 0;
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
  case PROP_SYNTH_GENERATOR:
    {
      AgsSynthGenerator *synth_generator;

      synth_generator = (AgsSynthGenerator *) g_value_get_object(value);

      if(apply_synth->synth_generator == synth_generator){
	return;
      }

      if(apply_synth->synth_generator != NULL){
	g_object_unref(apply_synth->synth_generator);
      }

      if(synth_generator != NULL){
	g_object_ref(synth_generator);
      }

      apply_synth->synth_generator = synth_generator;
    }
    break;
  case PROP_START_CHANNEL:
    {
      AgsChannel *start_channel;

      start_channel = (AgsChannel *) g_value_get_object(value);

      if(apply_synth->start_channel == start_channel){
	return;
      }

      if(apply_synth->start_channel != NULL){
	g_object_unref(apply_synth->start_channel);
      }

      if(start_channel != NULL){
	g_object_ref(start_channel);
      }

      apply_synth->start_channel = start_channel;
    }
    break;
  case PROP_BASE_NOTE:
    {
      apply_synth->base_note = g_value_get_double(value);
    }
    break;
  case PROP_COUNT:
    {
      apply_synth->count = g_value_get_uint(value);
    }
    break;
  case PROP_REQUESTED_FRAME_COUNT:
    {
      apply_synth->requested_frame_count = g_value_get_uint(value);
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
  case PROP_SYNTH_GENERATOR:
    {
      g_value_set_object(value, apply_synth->synth_generator);
    }
    break;
  case PROP_START_CHANNEL:
    {
      g_value_set_object(value, apply_synth->start_channel);
    }
    break;
  case PROP_BASE_NOTE:
    {
      g_value_set_double(value, apply_synth->base_note);
    }
    break;
  case PROP_COUNT:
    {
      g_value_set_uint(value, apply_synth->count);
    }
    break;
  case PROP_REQUESTED_FRAME_COUNT:
    {
      g_value_set_uint(value, apply_synth->requested_frame_count);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_apply_synth_dispose(GObject *gobject)
{
  AgsApplySynth *apply_synth;

  apply_synth = AGS_APPLY_SYNTH(gobject);

  if(apply_synth->synth_generator != NULL){
    g_object_unref(apply_synth->synth_generator);
    
    apply_synth->synth_generator = NULL;
  }
  
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

  if(apply_synth->synth_generator != NULL){
    g_object_unref(apply_synth->synth_generator);    
  }

  if(apply_synth->start_channel != NULL){
    g_object_unref(apply_synth->start_channel);    
  }

  /* call parent */
  G_OBJECT_CLASS(ags_apply_synth_parent_class)->finalize(gobject);
}

void
ags_apply_synth_launch(AgsTask *task)
{
  AgsApplySynth *apply_synth;

  AgsAudio *audio;
  AgsChannel *channel, *next_channel, *input;
  AgsRecycling *first_recycling;
  AgsAudioSignal *audio_signal;
  AgsSynthGenerator *synth_generator;

  GObject *output_soundcard;
  
  GList *list_start;
  GList *rt_template_start, *rt_template;

  gchar *str;
  
  gdouble base_note;
  gdouble note;
  guint count;
  guint requested_frame_count;
  guint buffer_size;
  guint i;
  
  apply_synth = AGS_APPLY_SYNTH(task);

  g_return_if_fail(AGS_IS_CHANNEL(apply_synth->start_channel));
  g_return_if_fail(AGS_IS_SYNTH_GENERATOR(apply_synth->synth_generator));
  
  channel = apply_synth->start_channel;
  
  synth_generator = apply_synth->synth_generator;

  base_note = apply_synth->base_note;
  count = apply_synth->count;

  requested_frame_count = apply_synth->requested_frame_count;

  buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
  
  /* get some fields */
  g_object_get(channel,
	       "audio", &audio,
	       NULL);

  /* compute */
  channel = apply_synth->start_channel;

  if(channel != NULL){
    g_object_ref(channel);

    for(i = 0; channel != NULL && i < apply_synth->count; i++){
      /* get some fields */
      g_object_get(channel,
		   "first-recycling", &first_recycling,
		   NULL);
	
      /* get template */
      g_object_get(first_recycling,
		   "output-soundcard", &output_soundcard,
		   "audio-signal", &list_start,
		   "buffer-size", &buffer_size,
		   NULL);
	
      audio_signal = ags_audio_signal_get_default_template(list_start);

      if(audio_signal == NULL){
	audio_signal = ags_audio_signal_new(output_soundcard,
					    (GObject *) first_recycling,
					    NULL);
	g_object_ref(audio_signal);
	
	ags_audio_signal_set_flags(audio_signal,
				   AGS_AUDIO_SIGNAL_TEMPLATE);

	ags_recycling_create_audio_signal_with_frame_count(first_recycling,
							   audio_signal,
							   requested_frame_count,
							   0.0, 0);
	
	ags_recycling_add_audio_signal(first_recycling,
				       audio_signal);
      }
	
      /* compute audio signal */
      note = apply_synth->base_note + i;

      g_object_get(audio_signal,
		   "buffer-size", &buffer_size,
		   NULL);
      
      g_object_set(audio_signal,
		   "length", (guint) ceil(requested_frame_count / buffer_size),
		   "frame-count", requested_frame_count,
		   NULL);
	
      ags_synth_generator_compute(synth_generator,
				  (GObject *) audio_signal,
				  note);

      rt_template = 
	rt_template_start = ags_audio_signal_get_rt_template(list_start);

      while(rt_template != NULL){
	g_object_get(rt_template->data,
		     "buffer-size", &buffer_size,
		     NULL);
	
	g_object_set(rt_template->data,
		     "length", (guint) ceil(requested_frame_count / buffer_size),
		     "frame-count", requested_frame_count,
		     NULL);

	ags_synth_generator_compute(synth_generator,
				    rt_template->data,
				    note);

	rt_template = rt_template->next;
      }

      g_list_free_full(rt_template_start,
		       g_object_unref);
    
      g_list_free_full(list_start,
		       g_object_unref);
	
      g_object_unref(output_soundcard);
      g_object_unref(first_recycling);
      g_object_unref(audio_signal);
      
      /* iterate */
      next_channel = ags_channel_next(channel);
    
      g_object_unref(channel);

      channel = next_channel;
    }
  }

  if(audio != NULL){
    g_object_unref(audio);
  }
}

/**
 * ags_apply_synth_new:
 * @synth_generator: the #AgsSynthGenerator
 * @start_channel: the start #AgsChannel
 * @base_note: the base note
 * @count: the count of lines
 *
 * Creates an #AgsApplySynth.
 *
 * Returns: an new #AgsApplySynth.
 *
 * Since: 3.0.0
 */
AgsApplySynth*
ags_apply_synth_new(AgsSynthGenerator *synth_generator,
		    AgsChannel *start_channel,
		    gdouble base_note, guint count)
{
  AgsApplySynth *apply_synth;

  apply_synth = (AgsApplySynth *) g_object_new(AGS_TYPE_APPLY_SYNTH,
					       "synth-generator", synth_generator,
					       "start-channel", start_channel,
					       "base-note", base_note,
					       "count", count,
					       NULL);


  return(apply_synth);
}
