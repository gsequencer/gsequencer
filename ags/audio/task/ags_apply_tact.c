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

#include <ags/audio/task/ags_apply_tact.h>

#include <ags/audio/ags_sound_provider.h>
#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_recall.h>

#include <math.h>

#include <ags/i18n.h>

void ags_apply_tact_class_init(AgsApplyTactClass *apply_tact);
void ags_apply_tact_init(AgsApplyTact *apply_tact);
void ags_apply_tact_set_property(GObject *gobject,
				 guint prop_id,
				 const GValue *value,
				 GParamSpec *param_spec);
void ags_apply_tact_get_property(GObject *gobject,
				 guint prop_id,
				 GValue *value,
				 GParamSpec *param_spec);
void ags_apply_tact_dispose(GObject *gobject);
void ags_apply_tact_finalize(GObject *gobject);

void ags_apply_tact_launch(AgsTask *task);

void ags_apply_tact_recall(AgsApplyTact *apply_tact, AgsRecall *recall);
void ags_apply_tact_channel(AgsApplyTact *apply_tact, AgsChannel *channel);
void ags_apply_tact_audio(AgsApplyTact *apply_tact, AgsAudio *audio);
void ags_apply_tact_soundcard(AgsApplyTact *apply_tact, GObject *soundcard);
void ags_apply_tact_sequencer(AgsApplyTact *apply_tact, GObject *sequencer);
void ags_apply_tact_application_context(AgsApplyTact *apply_tact, AgsApplicationContext *application_context);

/**
 * SECTION:ags_apply_tact
 * @short_description: apply tact to delay audio
 * @title: AgsApplyTact
 * @section_id:
 * @include: ags/audio/task/recall/ags_apply_tact.h
 *
 * The #AgsApplyTact task applys tact to #AgsTactable.
 */

enum{
  PROP_0,
  PROP_SCOPE,
  PROP_TACT,
};

static gpointer ags_apply_tact_parent_class = NULL;

GType
ags_apply_tact_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_apply_tact = 0;

    static const GTypeInfo ags_apply_tact_info = {
      sizeof(AgsApplyTactClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_apply_tact_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsApplyTact),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_apply_tact_init,
    };

    ags_type_apply_tact = g_type_register_static(AGS_TYPE_TASK,
						 "AgsApplyTact",
						 &ags_apply_tact_info,
						 0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_apply_tact);
  }

  return g_define_type_id__volatile;
}

void
ags_apply_tact_class_init(AgsApplyTactClass *apply_tact)
{
  GObjectClass *gobject;
  AgsTaskClass *task;

  GParamSpec *param_spec;

  ags_apply_tact_parent_class = g_type_class_peek_parent(apply_tact);

  /* GObjectClass */
  gobject = (GObjectClass *) apply_tact;

  gobject->set_property = ags_apply_tact_set_property;
  gobject->get_property = ags_apply_tact_get_property;

  gobject->dispose = ags_apply_tact_dispose;
  gobject->finalize = ags_apply_tact_finalize;

  /* properties */
  /**
   * AgsApplyTact:scope:
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
   * AgsApplyTact:tact:
   *
   * The tact to apply to scope.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_double("tact",
				   i18n_pspec("tact"),
				   i18n_pspec("The tact to apply"),
				   0,
				   G_MAXDOUBLE,
				   0,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_TACT,
				  param_spec);

  /* AgsTaskClass */
  task = (AgsTaskClass *) apply_tact;
  
  task->launch = ags_apply_tact_launch;
}

void
ags_apply_tact_init(AgsApplyTact *apply_tact)
{
  apply_tact->scope = NULL;
  apply_tact->tact = 0.0;
}

void
ags_apply_tact_set_property(GObject *gobject,
			    guint prop_id,
			    const GValue *value,
			    GParamSpec *param_spec)
{
  AgsApplyTact *apply_tact;

  apply_tact = AGS_APPLY_TACT(gobject);

  switch(prop_id){
  case PROP_SCOPE:
    {
      GObject *scope;

      scope = (GObject *) g_value_get_object(value);

      if(apply_tact->scope == (GObject *) scope){
	return;
      }

      if(apply_tact->scope != NULL){
	g_object_unref(apply_tact->scope);
      }

      if(scope != NULL){
	g_object_ref(scope);
      }

      apply_tact->scope = (GObject *) scope;
    }
    break;
  case PROP_TACT:
    {
      apply_tact->tact = g_value_get_double(value);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_apply_tact_get_property(GObject *gobject,
			    guint prop_id,
			    GValue *value,
			    GParamSpec *param_spec)
{
  AgsApplyTact *apply_tact;

  apply_tact = AGS_APPLY_TACT(gobject);

  switch(prop_id){
  case PROP_SCOPE:
    {
      g_value_set_object(value, apply_tact->scope);
    }
    break;
  case PROP_TACT:
    {
      g_value_set_double(value, apply_tact->tact);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_apply_tact_dispose(GObject *gobject)
{
  AgsApplyTact *apply_tact;

  apply_tact = AGS_APPLY_TACT(gobject);

  if(apply_tact->scope != NULL){
    g_object_unref(apply_tact->scope);

    apply_tact->scope = NULL;
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_apply_tact_parent_class)->dispose(gobject);
}

void
ags_apply_tact_finalize(GObject *gobject)
{
  AgsApplyTact *apply_tact;

  apply_tact = AGS_APPLY_TACT(gobject);

  if(apply_tact->scope != NULL){
    g_object_unref(apply_tact->scope);
  }

  /* call parent */
  G_OBJECT_CLASS(ags_apply_tact_parent_class)->finalize(gobject);
}

void
ags_apply_tact_launch(AgsTask *task)
{
  AgsApplyTact *apply_tact;

  apply_tact = AGS_APPLY_TACT(task);

 if(AGS_IS_APPLICATION_CONTEXT(apply_tact->scope)){
    AgsApplicationContext *application_context;

    application_context = (AgsApplicationContext *) apply_tact->scope;

    ags_apply_tact_application_context(apply_tact, application_context);
  }else if(AGS_IS_SOUNDCARD(apply_tact->scope)){
    GObject *soundcard;

    soundcard = apply_tact->scope;

    ags_apply_tact_soundcard(apply_tact, soundcard);
 }else if(AGS_IS_SEQUENCER(apply_tact->scope)){
    GObject *sequencer;

    sequencer = apply_tact->scope;

    ags_apply_tact_sequencer(apply_tact, sequencer);
 }else if(AGS_IS_AUDIO(apply_tact->scope)){
    AgsAudio *audio;

    audio = AGS_AUDIO(apply_tact->scope);

    ags_apply_tact_audio(apply_tact, audio);
  }else if(AGS_IS_CHANNEL(apply_tact->scope)){
    AgsChannel *channel;

    channel = AGS_CHANNEL(apply_tact->scope);

    ags_apply_tact_channel(apply_tact, channel);
  }else if(AGS_IS_RECALL(apply_tact->scope)){
    AgsRecall *recall;

    recall = AGS_RECALL(apply_tact->scope);

    ags_apply_tact_recall(apply_tact, recall);
  }else{
    g_warning("AgsApplyTact: Not supported scope");
  }
}

void
ags_apply_tact_recall(AgsApplyTact *apply_tact, AgsRecall *recall)
{
  if(AGS_IS_TACTABLE(recall)){
    ags_tactable_change_tact(AGS_TACTABLE(recall),
			     ags_tactable_get_tact(AGS_TACTABLE(recall)),
			     apply_tact->tact);
  }
}

void
ags_apply_tact_channel(AgsApplyTact *apply_tact, AgsChannel *channel)
{
  GList *list_start, *list;

  /* apply tact - play */
  g_object_get(channel,
	       "play", &list_start,
	       NULL);

  list = list_start;
  
  while(list != NULL){
    ags_apply_tact_recall(apply_tact, AGS_RECALL(list->data));
    
    list = list->next;
  }

  g_list_free_full(list_start,
		   g_object_unref);
  
  /* apply tact - recall */
  g_object_get(channel,
	       "recall", &list_start,
	       NULL);

  list = list_start;
  
  while(list != NULL){
    ags_apply_tact_recall(apply_tact, AGS_RECALL(list->data));
    
    list = list->next;
  }

  g_list_free_full(list_start,
		   g_object_unref);
}

void
ags_apply_tact_audio(AgsApplyTact *apply_tact, AgsAudio *audio)
{
  AgsChannel *input, *output;
  AgsChannel *channel, *next_channel;

  GList *list_start, *list;

  g_object_get(audio,
	       "output", &output,
	       "input", &input,
	       NULL);

  /* apply tact - play */
  g_object_get(audio,
	       "play", &list_start,
	       NULL);

  list = list_start;
  
  while(list != NULL){
    ags_apply_tact_recall(apply_tact, AGS_RECALL(list->data));
    
    list = list->next;
  }

  g_list_free_full(list_start,
		   g_object_unref);
  
  /* apply tact - recall */
  g_object_get(audio,
	       "recall", &list_start,
	       NULL);

  list = list_start;
  
  while(list != NULL){
    ags_apply_tact_recall(apply_tact, AGS_RECALL(list->data));
    
    list = list->next;
  }

  g_list_free_full(list_start,
		   g_object_unref);
  
  /* AgsChannel - output */
  if(output != NULL){
    channel = output;
    g_object_ref(channel);

    while(channel != NULL){
      /* apply tact */
      ags_apply_tact_channel(apply_tact, channel);

      /* iterate */
      next_channel = ags_channel_next(channel);
    
      g_object_unref(channel);

      channel = next_channel;
    }

    /* unref */
    g_object_unref(output);
  }

  /* AgsChannel - input */
  if(input != NULL){
    channel = input;
    g_object_ref(channel);

    while(channel != NULL){
      /* apply tact */
      ags_apply_tact_channel(apply_tact, channel);

      /* iterate */
      next_channel = ags_channel_next(channel);
    
      g_object_unref(channel);

      channel = next_channel;
    }

    /* unref */
    g_object_unref(input);
  }
}

void
ags_apply_tact_soundcard(AgsApplyTact *apply_tact, GObject *soundcard)
{
  ags_soundcard_set_delay_factor(AGS_SOUNDCARD(soundcard), apply_tact->tact);
}

void
ags_apply_tact_sequencer(AgsApplyTact *apply_tact, GObject *sequencer)
{
  ags_sequencer_set_delay_factor(AGS_SEQUENCER(sequencer), apply_tact->tact);
}

void
ags_apply_tact_application_context(AgsApplyTact *apply_tact, AgsApplicationContext *application_context)
{
  GList *list_start, *list;

  /* soundcard */
  list =
    list_start = ags_sound_provider_get_soundcard(AGS_SOUND_PROVIDER(application_context));

  while(list != NULL){
    ags_apply_tact_soundcard(apply_tact, list->data);

    list = list->next;
  }

  g_list_free_full(list_start,
		   g_object_unref);

  /* sequencer */
  list =
    list_start = ags_sound_provider_get_sequencer(AGS_SOUND_PROVIDER(application_context));

  while(list != NULL){
    ags_apply_tact_sequencer(apply_tact, list->data);

    list = list->next;
  }

  g_list_free_full(list_start,
		   g_object_unref);

  /* audio */
  list =
    list_start = ags_sound_provider_get_audio(AGS_SOUND_PROVIDER(application_context));

  while(list != NULL){
    ags_apply_tact_audio(apply_tact, list->data);

    list = list->next;
  }

  g_list_free_full(list_start,
		   g_object_unref);
}

/**
 * ags_apply_tact_new:
 * @scope: the #GObject
 * @tact: the tact to apply
 *
 * Creates an #AgsApplyTact.
 *
 * Returns: an new #AgsApplyTact.
 *
 * Since: 2.0.0
 */
AgsApplyTact*
ags_apply_tact_new(GObject *scope,
		   gdouble tact)
{
  AgsApplyTact *apply_tact;
  
  apply_tact = (AgsApplyTact *) g_object_new(AGS_TYPE_APPLY_TACT,
					     "scope", scope,
					     "tact", tact,
					     NULL);
  
  return(apply_tact);
}
