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

#include <ags/audio/task/ags_apply_sequencer_length.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_recall.h>

#include <math.h>

#include <ags/i18n.h>

void ags_apply_sequencer_length_class_init(AgsApplySequencerLengthClass *apply_sequencer_length);
void ags_apply_sequencer_length_init(AgsApplySequencerLength *apply_sequencer_length);
void ags_apply_sequencer_length_set_property(GObject *gobject,
					     guint prop_id,
					     const GValue *value,
					     GParamSpec *param_spec);
void ags_apply_sequencer_length_get_property(GObject *gobject,
					     guint prop_id,
					     GValue *value,
					     GParamSpec *param_spec);
void ags_apply_sequencer_length_dispose(GObject *gobject);
void ags_apply_sequencer_length_finalize(GObject *gobject);

void ags_apply_sequencer_length_recall(AgsApplySequencerLength *apply_sequencer_length, AgsRecall *recall);
void ags_apply_sequencer_length_channel(AgsApplySequencerLength *apply_sequencer_length, AgsChannel *channel);
void ags_apply_sequencer_length_audio(AgsApplySequencerLength *apply_sequencer_length, AgsAudio *audio);

void ags_apply_sequencer_length_launch(AgsTask *task);

/**
 * SECTION:ags_apply_sequencer_length
 * @short_description: apply sequencer length to delay audio
 * @title: AgsApplySequencerLength
 * @section_id:
 * @include: ags/audio/task/recall/ags_apply_sequencer_length.h
 *
 * The #AgsApplySequencerLength task applys sequencer length to #AgsDelayAudio.
 */

enum{
  PROP_0,
  PROP_SCOPE,
  PROP_SEQUENCER_LENGTH,
};

static gpointer ags_apply_sequencer_length_parent_class = NULL;

GType
ags_apply_sequencer_length_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_apply_sequencer_length = 0;

    static const GTypeInfo ags_apply_sequencer_length_info = {
      sizeof(AgsApplySequencerLengthClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_apply_sequencer_length_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsApplySequencerLength),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_apply_sequencer_length_init,
    };

    ags_type_apply_sequencer_length = g_type_register_static(AGS_TYPE_TASK,
							     "AgsApplySequencerLength",
							     &ags_apply_sequencer_length_info,
							     0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_apply_sequencer_length);
  }

  return g_define_type_id__volatile;
}

void
ags_apply_sequencer_length_class_init(AgsApplySequencerLengthClass *apply_sequencer_length)
{
  GObjectClass *gobject;
  AgsTaskClass *task;

  GParamSpec *param_spec;

  ags_apply_sequencer_length_parent_class = g_type_class_peek_parent(apply_sequencer_length);

  /* GObjectClass */
  gobject = (GObjectClass *) apply_sequencer_length;

  gobject->set_property = ags_apply_sequencer_length_set_property;
  gobject->get_property = ags_apply_sequencer_length_get_property;

  gobject->dispose = ags_apply_sequencer_length_dispose;
  gobject->finalize = ags_apply_sequencer_length_finalize;

  /* properties */
  /**
   * AgsApplySequencerLength:scope:
   *
   * The assigned #GObject as scope.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("scope",
				   i18n_pspec("scope of set buffer size"),
				   i18n_pspec("The scope of set buffer size"),
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SCOPE,
				  param_spec);

  /**
   * AgsApplySequencerLength:sequencer-length:
   *
   * The sequencer length to apply to scope.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_double("sequencer-length",
				   i18n_pspec("sequencer length"),
				   i18n_pspec("The sequence length to apply"),
				   0,
				   G_MAXDOUBLE,
				   0,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SEQUENCER_LENGTH,
				  param_spec);

  /* AgsTaskClass */
  task = (AgsTaskClass *) apply_sequencer_length;
  
  task->launch = ags_apply_sequencer_length_launch;
}

void
ags_apply_sequencer_length_init(AgsApplySequencerLength *apply_sequencer_length)
{
  apply_sequencer_length->scope = NULL;
  apply_sequencer_length->length = 0.0;
}

void
ags_apply_sequencer_length_set_property(GObject *gobject,
					guint prop_id,
					const GValue *value,
					GParamSpec *param_spec)
{
  AgsApplySequencerLength *apply_sequencer_length;

  apply_sequencer_length = AGS_APPLY_SEQUENCER_LENGTH(gobject);

  switch(prop_id){
  case PROP_SCOPE:
    {
      GObject *scope;

      scope = (GObject *) g_value_get_object(value);

      if(apply_sequencer_length->scope == (GObject *) scope){
	return;
      }

      if(apply_sequencer_length->scope != NULL){
	g_object_unref(apply_sequencer_length->scope);
      }

      if(scope != NULL){
	g_object_ref(scope);
      }

      apply_sequencer_length->scope = (GObject *) scope;
    }
    break;
  case PROP_SEQUENCER_LENGTH:
    {
      apply_sequencer_length->length = g_value_get_double(value);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_apply_sequencer_length_get_property(GObject *gobject,
					guint prop_id,
					GValue *value,
					GParamSpec *param_spec)
{
  AgsApplySequencerLength *apply_sequencer_length;

  apply_sequencer_length = AGS_APPLY_SEQUENCER_LENGTH(gobject);

  switch(prop_id){
  case PROP_SCOPE:
    {
      g_value_set_object(value, apply_sequencer_length->scope);
    }
    break;
  case PROP_SEQUENCER_LENGTH:
    {
      g_value_set_double(value, apply_sequencer_length->length);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_apply_sequencer_length_dispose(GObject *gobject)
{
  AgsApplySequencerLength *apply_sequencer_length;

  apply_sequencer_length = AGS_APPLY_SEQUENCER_LENGTH(gobject);
  
  if(apply_sequencer_length->scope != NULL){
    g_object_unref(apply_sequencer_length->scope);

    apply_sequencer_length->scope = NULL;
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_apply_sequencer_length_parent_class)->dispose(gobject);
}

void
ags_apply_sequencer_length_finalize(GObject *gobject)
{
  AgsApplySequencerLength *apply_sequencer_length;

  apply_sequencer_length = AGS_APPLY_SEQUENCER_LENGTH(gobject);

  if(apply_sequencer_length->scope != NULL){
    g_object_unref(apply_sequencer_length->scope);
  }

  /* call parent */
  G_OBJECT_CLASS(ags_apply_sequencer_length_parent_class)->finalize(gobject);
}

void
ags_apply_sequencer_length_launch(AgsTask *task)
{
  AgsApplySequencerLength *apply_sequencer_length;

  apply_sequencer_length = AGS_APPLY_SEQUENCER_LENGTH(task);

  if(AGS_IS_AUDIO(apply_sequencer_length->scope)){
    AgsAudio *audio;

    audio = AGS_AUDIO(apply_sequencer_length->scope);

    ags_apply_sequencer_length_audio(apply_sequencer_length, audio);
  }else if(AGS_IS_CHANNEL(apply_sequencer_length->scope)){
    AgsChannel *channel;

    channel = AGS_CHANNEL(apply_sequencer_length->scope);

    ags_apply_sequencer_length_channel(apply_sequencer_length, channel);
  }else if(AGS_IS_RECALL(apply_sequencer_length->scope)){
    AgsRecall *recall;

    recall = AGS_RECALL(apply_sequencer_length->scope);

    ags_apply_sequencer_length_recall(apply_sequencer_length, recall);
  }else{
    g_warning("AgsApplySequencerLength: Not supported scope");
  }
}

void
ags_apply_sequencer_length_recall(AgsApplySequencerLength *apply_sequencer_length, AgsRecall *recall)
{
  if(AGS_IS_TACTABLE(recall)){
    ags_tactable_change_sequencer_duration(AGS_TACTABLE(recall), apply_sequencer_length->length);
  }
}

void
ags_apply_sequencer_length_channel(AgsApplySequencerLength *apply_sequencer_length, AgsChannel *channel)
{
  GList *list_start, *list;

  /* apply sequencer length - play */
  g_object_get(channel,
	       "play", &list_start,
	       NULL);

  list = list_start;
  
  while(list != NULL){
    ags_apply_sequencer_length_recall(apply_sequencer_length, AGS_RECALL(list->data));
    
    list = list->next;
  }

  g_list_free_full(list_start,
		   g_object_unref);
  
  /* apply sequencer length - recall */
  g_object_get(channel,
	       "recall", &list_start,
	       NULL);

  list = list_start;
  
  while(list != NULL){
    ags_apply_sequencer_length_recall(apply_sequencer_length, AGS_RECALL(list->data));
    
    list = list->next;
  }

  g_list_free_full(list_start,
		   g_object_unref);
}

void
ags_apply_sequencer_length_audio(AgsApplySequencerLength *apply_sequencer_length, AgsAudio *audio)
{
  AgsChannel *input, *output;
  AgsChannel *channel;

  GList *list_start, *list;

  pthread_mutex_t *audio_mutex;
  pthread_mutex_t *channel_mutex;

  /* get audio mutex */
  audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio);

  /* get some fields */
  pthread_mutex_lock(audio_mutex);

  output = audio->output;
  input = audio->input;

  pthread_mutex_unlock(audio_mutex);

  /* apply sequencer_length - play */
  g_object_get(audio,
	       "play", &list_start,
	       NULL);

  list = list_start;
  
  while(list != NULL){
    ags_apply_sequencer_length_recall(apply_sequencer_length, AGS_RECALL(list->data));
    
    list = list->next;
  }
  
  g_list_free_full(list_start,
		   g_object_unref);
  
  /* apply sequencer length - recall */
  g_object_get(audio,
	       "recall", &list_start,
	       NULL);

  list = list_start;
  
  while(list != NULL){
    ags_apply_sequencer_length_recall(apply_sequencer_length, AGS_RECALL(list->data));
    
    list = list->next;
  }

  g_list_free_full(list_start,
		   g_object_unref);
  
  /* AgsChannel - output */
  channel = output;

  while(channel != NULL){
    /* get channel mutex */
    channel_mutex = AGS_CHANNEL_GET_OBJ_MUTEX(channel);

    /* apply sequencer length */
    ags_apply_sequencer_length_channel(apply_sequencer_length, channel);

    /* iterate */
    pthread_mutex_lock(channel_mutex);

    channel = channel->next;
    
    pthread_mutex_unlock(channel_mutex);    
  }

  /* AgsChannel - input */
  channel = input;

  while(channel != NULL){
    /* get channel mutex */
    channel_mutex = AGS_CHANNEL_GET_OBJ_MUTEX(channel);

    /* apply sequencer length */
    ags_apply_sequencer_length_channel(apply_sequencer_length, channel);

    /* iterate */
    pthread_mutex_lock(channel_mutex);

    channel = channel->next;
    
    pthread_mutex_unlock(channel_mutex);    
  }
}

/**
 * ags_apply_sequencer_length_new:
 * @scope: the #GObject
 * @length: the sequencer length to apply
 *
 * Creates an #AgsApplySequencerLength.
 *
 * Returns: an new #AgsApplySequencerLength.
 *
 * Since: 2.0.0
 */
AgsApplySequencerLength*
ags_apply_sequencer_length_new(GObject *scope,
			       gdouble length)
{
  AgsApplySequencerLength *apply_sequencer_length;

  apply_sequencer_length = (AgsApplySequencerLength *) g_object_new(AGS_TYPE_APPLY_SEQUENCER_LENGTH,
								    "scope", scope,
								    "sequencer-length", length,
								    NULL);

  return(apply_sequencer_length);
}
