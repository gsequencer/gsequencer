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

#include <ags/audio/task/ags_add_audio_signal.h>

#include <ags/i18n.h>

void ags_add_audio_signal_class_init(AgsAddAudioSignalClass *add_audio_signal);
void ags_add_audio_signal_init(AgsAddAudioSignal *add_audio_signal);
void ags_add_audio_signal_set_property(GObject *gobject,
				       guint prop_id,
				       const GValue *value,
				       GParamSpec *param_spec);
void ags_add_audio_signal_get_property(GObject *gobject,
				       guint prop_id,
				       GValue *value,
				       GParamSpec *param_spec);
void ags_add_audio_signal_dispose(GObject *gobject);
void ags_add_audio_signal_finalize(GObject *gobject);

void ags_add_audio_signal_launch(AgsTask *task);

/**
 * SECTION:ags_add_audio_signal
 * @short_description: add audio signal object to recycling
 * @title: AgsAddAudioSignal
 * @section_id:
 * @include: ags/audio/task/ags_add_audio_signal.h
 *
 * The #AgsAddAudioSignal task adds #AgsAudioSignal to #AgsRecycling.
 */

static gpointer ags_add_audio_signal_parent_class = NULL;

enum{
  PROP_0,
  PROP_RECYCLING,
  PROP_AUDIO_SIGNAL,
  PROP_SOUNDCARD,
  PROP_RECALL_ID,
  PROP_AUDIO_SIGNAL_FLAGS,
};

GType
ags_add_audio_signal_get_type()
{
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
    GType ags_type_add_audio_signal = 0;

    static const GTypeInfo ags_add_audio_signal_info = {
      sizeof (AgsAddAudioSignalClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_add_audio_signal_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsAddAudioSignal),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_add_audio_signal_init,
    };

    ags_type_add_audio_signal = g_type_register_static(AGS_TYPE_TASK,
						       "AgsAddAudioSignal",
						       &ags_add_audio_signal_info,
						       0);

    g_once_init_leave(&g_define_type_id__static, ags_type_add_audio_signal);
  }

  return g_define_type_id__static;
}

void
ags_add_audio_signal_class_init(AgsAddAudioSignalClass *add_audio_signal)
{
  GObjectClass *gobject;
  AgsTaskClass *task;
  GParamSpec *param_spec;
  
  ags_add_audio_signal_parent_class = g_type_class_peek_parent(add_audio_signal);

  /* gobject */
  gobject = (GObjectClass *) add_audio_signal;

  gobject->set_property = ags_add_audio_signal_set_property;
  gobject->get_property = ags_add_audio_signal_get_property;

  gobject->dispose = ags_add_audio_signal_dispose;
  gobject->finalize = ags_add_audio_signal_finalize;

  /* properties */
  /**
   * AgsAddAudioSignal:recycling:
   *
   * The assigned #AgsRecycling
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("recycling",
				   i18n_pspec("recycling of add audio signal"),
				   i18n_pspec("The recycling of add audio signal task"),
		 		   AGS_TYPE_RECYCLING,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_RECYCLING,
				  param_spec);

  /**
   * AgsAddAudioSignal:audio-signal:
   *
   * The assigned #AgsAudioSignal
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("audio-signal",
				   i18n_pspec("audio signal of add audio signal"),
				   i18n_pspec("The audio signal of add audio signal task"),
				   AGS_TYPE_AUDIO_SIGNAL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUDIO_SIGNAL,
				  param_spec);

  /**
   * AgsAddAudioSignal:soundcard:
   *
   * The assigned #AgsSoundcard
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("soundcard",
				   i18n_pspec("soundcard of add audio signal"),
				   i18n_pspec("The soundcard of add audio signal task"),
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SOUNDCARD,
				  param_spec);

  /**
   * AgsAddAudioSignal:recall-id:
   *
   * The assigned #AgsRecallID
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("recall-id",
				   i18n_pspec("audio signal of add audio signal"),
				   i18n_pspec("The audio signal of add audio signal task"),
				   AGS_TYPE_RECALL_ID,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_RECALL_ID,
				  param_spec);

  /**
   * AgsAddAudioSignal:audio-signal-flags:
   *
   * The audio signal's flags.
   * 
   * Since: 3.0.0
   */
  param_spec =  g_param_spec_uint("audio-signal-flags",
				  i18n_pspec("audio signal flags of effect"),
				  i18n_pspec("The audio signal's flags to apply"),
				  0,
				  G_MAXUINT,
				  0,
				  G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUDIO_SIGNAL_FLAGS,
				  param_spec);
  
  /* task */
  task = (AgsTaskClass *) add_audio_signal;

  task->launch = ags_add_audio_signal_launch;
}

void
ags_add_audio_signal_init(AgsAddAudioSignal *add_audio_signal)
{
  add_audio_signal->recycling = NULL;
  add_audio_signal->audio_signal = NULL;
  add_audio_signal->soundcard = NULL;
  add_audio_signal->recall_id = NULL;
  add_audio_signal->audio_signal_flags = 0;
}

void
ags_add_audio_signal_set_property(GObject *gobject,
				  guint prop_id,
				  const GValue *value,
				  GParamSpec *param_spec)
{
  AgsAddAudioSignal *add_audio_signal;

  add_audio_signal = AGS_ADD_AUDIO_SIGNAL(gobject);

  switch(prop_id){
  case PROP_RECYCLING:
    {
      AgsRecycling *recycling;

      recycling = (AgsRecycling *) g_value_get_object(value);

      if(add_audio_signal->recycling == recycling){
	return;
      }

      if(add_audio_signal->recycling != NULL){
	g_object_unref(add_audio_signal->recycling);
      }

      if(recycling != NULL){
	g_object_ref(recycling);
      }

      add_audio_signal->recycling = recycling;
    }
    break;
  case PROP_AUDIO_SIGNAL:
    {
      AgsAudioSignal *audio_signal;

      audio_signal = (AgsAudioSignal *) g_value_get_object(value);

      if(add_audio_signal->audio_signal == audio_signal){
	return;
      }

      if(add_audio_signal->audio_signal != NULL){
	g_object_unref(add_audio_signal->audio_signal);
      }

      if(audio_signal != NULL){
	g_object_ref(audio_signal);
      }

      add_audio_signal->audio_signal = audio_signal;
    }
    break;
  case PROP_SOUNDCARD:
    {
      GObject *soundcard;

      soundcard = (GObject *) g_value_get_object(value);

      if(add_audio_signal->soundcard == (GObject *) soundcard){
	return;
      }

      if(add_audio_signal->soundcard != NULL){
	g_object_unref(add_audio_signal->soundcard);
      }

      if(soundcard != NULL){
	g_object_ref(soundcard);
      }

      add_audio_signal->soundcard = (GObject *) soundcard;
    }
    break;
  case PROP_RECALL_ID:
    {
      AgsRecallID *recall_id;

      recall_id = (AgsRecallID *) g_value_get_object(value);

      if(add_audio_signal->recall_id == recall_id){
	return;
      }

      if(add_audio_signal->recall_id != NULL){
	g_object_unref(add_audio_signal->recall_id);
      }

      if(recall_id != NULL){
	g_object_ref(recall_id);
      }

      add_audio_signal->recall_id = recall_id;
    }
    break;
  case PROP_AUDIO_SIGNAL_FLAGS:
    {
      guint audio_signal_flags;

      audio_signal_flags = g_value_get_uint(value);

      add_audio_signal->audio_signal_flags = audio_signal_flags;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_add_audio_signal_get_property(GObject *gobject,
				  guint prop_id,
				  GValue *value,
				  GParamSpec *param_spec)
{
  AgsAddAudioSignal *add_audio_signal;

  add_audio_signal = AGS_ADD_AUDIO_SIGNAL(gobject);

  switch(prop_id){
  case PROP_RECYCLING:
    {
      g_value_set_object(value, add_audio_signal->recycling);
    }
    break;
  case PROP_AUDIO_SIGNAL:
    {
      g_value_set_object(value, add_audio_signal->audio_signal);
    }
    break;
  case PROP_SOUNDCARD:
    {
      g_value_set_object(value, add_audio_signal->soundcard);
    }
    break;
  case PROP_AUDIO_SIGNAL_FLAGS:
    {
      g_value_set_uint(value, add_audio_signal->audio_signal_flags);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_add_audio_signal_dispose(GObject *gobject)
{
  AgsAddAudioSignal *add_audio_signal;
  
  add_audio_signal = AGS_ADD_AUDIO_SIGNAL(gobject);

  if(add_audio_signal->recycling != NULL){
    g_object_unref(add_audio_signal->recycling);

    add_audio_signal->recycling = NULL;
  }

  if(add_audio_signal->audio_signal != NULL){
    g_object_unref(add_audio_signal->audio_signal);

    add_audio_signal->audio_signal = NULL;
  }

  if(add_audio_signal->soundcard != NULL){
    g_object_unref(add_audio_signal->soundcard);

    add_audio_signal->soundcard = NULL;
  }

  if(add_audio_signal->recall_id != NULL){
    g_object_unref(add_audio_signal->recall_id);

    add_audio_signal->recall_id = NULL;
  }

  /* call parent */
  G_OBJECT_CLASS(ags_add_audio_signal_parent_class)->dispose(gobject);
}

void
ags_add_audio_signal_finalize(GObject *gobject)
{
  AgsAddAudioSignal *add_audio_signal;
  
  add_audio_signal = AGS_ADD_AUDIO_SIGNAL(gobject);

  if(add_audio_signal->recycling != NULL){
    g_object_unref(add_audio_signal->recycling);
  }

  if(add_audio_signal->audio_signal != NULL){
    g_object_unref(add_audio_signal->audio_signal);
  }

  if(add_audio_signal->soundcard != NULL){
    g_object_unref(add_audio_signal->soundcard);
  }

  if(add_audio_signal->recall_id != NULL){
    g_object_unref(add_audio_signal->recall_id);
  }

  /* call parent */
  G_OBJECT_CLASS(ags_add_audio_signal_parent_class)->finalize(gobject);
}

void
ags_add_audio_signal_launch(AgsTask *task)
{
  AgsRecycling *recycling;
  AgsAudioSignal *audio_signal, *old_template;
  AgsRecallID *recall_id;

  AgsAddAudioSignal *add_audio_signal;

  GObject *soundcard;

  guint audio_signal_flags;
  gdouble delay;
  guint attack;

  GRecMutex *recycling_mutex;

  add_audio_signal = AGS_ADD_AUDIO_SIGNAL(task);

  g_return_if_fail(AGS_IS_RECYCLING(add_audio_signal->recycling));
  
  soundcard = add_audio_signal->soundcard;
  
  recycling = add_audio_signal->recycling;

  audio_signal = add_audio_signal->audio_signal;
  
  audio_signal_flags = add_audio_signal->audio_signal_flags;

  recall_id = add_audio_signal->recall_id;
  
  /* get recycling mutex */
  recycling_mutex = AGS_RECYCLING_GET_OBJ_MUTEX(recycling);
  
  /* check for template to remove */
  old_template = NULL;
  
  if((AGS_AUDIO_SIGNAL_TEMPLATE & (audio_signal_flags)) != 0){
    g_rec_mutex_lock(recycling_mutex);
    
    old_template = ags_audio_signal_get_default_template(add_audio_signal->recycling->audio_signal);

    g_rec_mutex_unlock(recycling_mutex);
  }

  /* create audio signal */
  if(audio_signal == NULL){
    audio_signal =
      add_audio_signal->audio_signal = ags_audio_signal_new((GObject *) soundcard,
							    (GObject *) recycling,
							    (GObject *) recall_id);
    ags_audio_signal_set_flags(audio_signal,
			       add_audio_signal->audio_signal_flags);
  }

  /* delay and attack */
  //TODO:JK: unclear
  attack = 0; //soundcard->attack[((tic_counter_incr == AGS_NOTATION_TICS_PER_BEAT) ?
    //		   0:
    //			   tic_counter_incr)];
  delay = 0.0; //soundcard->delay[((tic_counter_incr == AGS_NOTATION_TICS_PER_BEAT) ?
  //		 0:
  //			 tic_counter_incr)];
  
  /* add audio signal */
  ags_recycling_create_audio_signal_with_defaults(recycling,
						  audio_signal,
						  delay, attack);
  audio_signal->stream_current = audio_signal->stream;
  ags_connectable_connect(AGS_CONNECTABLE(audio_signal));

  /* remove template */
  if(old_template != NULL){
    ags_recycling_remove_audio_signal(add_audio_signal->recycling,
				      old_template);
  }
  
  /*
   * emit add_audio_signal on AgsRecycling
   */
  ags_recycling_add_audio_signal(add_audio_signal->recycling,
				 audio_signal);

  if(old_template != NULL){
    g_object_unref(old_template);
  }
}

/**
 * ags_add_audio_signal_new:
 * @recycling: the #AgsRecycling
 * @audio_signal: the #AgsAudioSignal to add
 * @soundcard: the #GObject defaulting to
 * @recall_id: the #AgsRecallID, may be %NULL if %AGS_AUDIO_SIGNAL_TEMPLATE set
 * @audio_signal_flags: the flags to set
 *
 * Creates an #AgsAddAudioSignal.
 *
 * Returns: an new #AgsAddAudioSignal.
 *
 * Since: 3.0.0
 */
AgsAddAudioSignal*
ags_add_audio_signal_new(AgsRecycling *recycling,
			 AgsAudioSignal *audio_signal,
			 GObject *soundcard,
			 AgsRecallID *recall_id,
			 guint audio_signal_flags)
{
  AgsAddAudioSignal *add_audio_signal;

  add_audio_signal = (AgsAddAudioSignal *) g_object_new(AGS_TYPE_ADD_AUDIO_SIGNAL,
							"recycling", recycling,
							"audio-signal", audio_signal,
							"soundcard", soundcard,
							"recall-id", recall_id,
							"audio-signal-flags", audio_signal_flags,
							NULL);

  return(add_audio_signal);
}
