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

#include <ags/audio/task/ags_apply_bpm.h>

#include <ags/audio/ags_sound_provider.h>
#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_recall.h>

#include <math.h>

#include <ags/i18n.h>

void ags_apply_bpm_class_init(AgsApplyBpmClass *apply_bpm);
void ags_apply_bpm_init(AgsApplyBpm *apply_bpm);
void ags_apply_bpm_set_property(GObject *gobject,
				guint prop_id,
				const GValue *value,
				GParamSpec *param_spec);
void ags_apply_bpm_get_property(GObject *gobject,
				guint prop_id,
				GValue *value,
				GParamSpec *param_spec);
void ags_apply_bpm_dispose(GObject *gobject);
void ags_apply_bpm_finalize(GObject *gobject);

void ags_apply_bpm_launch(AgsTask *task);

void ags_apply_bpm_recall(AgsApplyBpm *apply_bpm, AgsRecall *recall);
void ags_apply_bpm_channel(AgsApplyBpm *apply_bpm, AgsChannel *channel);
void ags_apply_bpm_audio(AgsApplyBpm *apply_bpm, AgsAudio *audio);
void ags_apply_bpm_soundcard(AgsApplyBpm *apply_bpm, GObject *soundcard);
void ags_apply_bpm_sequencer(AgsApplyBpm *apply_bpm, GObject *sequencer);
void ags_apply_bpm_application_context(AgsApplyBpm *apply_bpm, AgsApplicationContext *application_context);

/**
 * SECTION:ags_apply_bpm
 * @short_description: apply bpm
 * @title: AgsApplyBpm
 * @section_id:
 * @include: ags/audio/task/recall/ags_apply_bpm.h
 *
 * The #AgsApplyBpm task applys bpm to #AgsDelayAudio.
 */

enum{
  PROP_0,
  PROP_SCOPE,
  PROP_BPM,
};

static gpointer ags_apply_bpm_parent_class = NULL;

GType
ags_apply_bpm_get_type()
{
  static gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_apply_bpm = 0;

    static const GTypeInfo ags_apply_bpm_info = {
      sizeof (AgsApplyBpmClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_apply_bpm_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsApplyBpm),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_apply_bpm_init,
    };

    ags_type_apply_bpm = g_type_register_static(AGS_TYPE_TASK,
						"AgsApplyBpm",
						&ags_apply_bpm_info,
						0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_apply_bpm);
  }

  return g_define_type_id__volatile;
}

void
ags_apply_bpm_class_init(AgsApplyBpmClass *apply_bpm)
{
  GObjectClass *gobject;
  AgsTaskClass *task;

  GParamSpec *param_spec;

  ags_apply_bpm_parent_class = g_type_class_peek_parent(apply_bpm);

  /* GObjectClass */
  gobject = (GObjectClass *) apply_bpm;

  gobject->set_property = ags_apply_bpm_set_property;
  gobject->get_property = ags_apply_bpm_get_property;

  gobject->dispose = ags_apply_bpm_dispose;
  gobject->finalize = ags_apply_bpm_finalize;

  /* properties */
  /**
   * AgsApplyBpm:scope:
   *
   * The assigned #GObject as scope.
   * 
   * Since: 3.0.0
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
   * AgsApplyBpm:bpm:
   *
   * The bpm to apply to scope.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_double("bpm",
				   i18n_pspec("bpm"),
				   i18n_pspec("The bpm to apply"),
				   0.0,
				   G_MAXDOUBLE,
				   AGS_SOUNDCARD_DEFAULT_BPM,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_BPM,
				  param_spec);

  /* AgsTaskClass */
  task = (AgsTaskClass *) apply_bpm;
  
  task->launch = ags_apply_bpm_launch;
}

void
ags_apply_bpm_init(AgsApplyBpm *apply_bpm)
{
  apply_bpm->scope = NULL;
  apply_bpm->bpm = AGS_SOUNDCARD_DEFAULT_BPM;
}

void
ags_apply_bpm_set_property(GObject *gobject,
			   guint prop_id,
			   const GValue *value,
			   GParamSpec *param_spec)
{
  AgsApplyBpm *apply_bpm;

  apply_bpm = AGS_APPLY_BPM(gobject);

  switch(prop_id){
  case PROP_SCOPE:
    {
      GObject *scope;

      scope = (GObject *) g_value_get_object(value);

      if(apply_bpm->scope == (GObject *) scope){
	return;
      }

      if(apply_bpm->scope != NULL){
	g_object_unref(apply_bpm->scope);
      }

      if(scope != NULL){
	g_object_ref(scope);
      }

      apply_bpm->scope = (GObject *) scope;
    }
    break;
  case PROP_BPM:
    {
      apply_bpm->bpm = g_value_get_double(value);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_apply_bpm_get_property(GObject *gobject,
			   guint prop_id,
			   GValue *value,
			   GParamSpec *param_spec)
{
  AgsApplyBpm *apply_bpm;

  apply_bpm = AGS_APPLY_BPM(gobject);

  switch(prop_id){
  case PROP_SCOPE:
    {
      g_value_set_object(value, apply_bpm->scope);
    }
    break;
  case PROP_BPM:
    {
      g_value_set_double(value, apply_bpm->bpm);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_apply_bpm_dispose(GObject *gobject)
{
  AgsApplyBpm *apply_bpm;

  apply_bpm = AGS_APPLY_BPM(gobject);
  
  if(apply_bpm->scope != NULL){
    g_object_unref(apply_bpm->scope);

    apply_bpm->scope = NULL;
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_apply_bpm_parent_class)->dispose(gobject);
}

void
ags_apply_bpm_finalize(GObject *gobject)
{
  AgsApplyBpm *apply_bpm;

  apply_bpm = AGS_APPLY_BPM(gobject);

  if(apply_bpm->scope != NULL){
    g_object_unref(apply_bpm->scope);
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_apply_bpm_parent_class)->finalize(gobject);
}

void
ags_apply_bpm_launch(AgsTask *task)
{
  AgsApplyBpm *apply_bpm;

  AgsMessageDelivery *message_delivery;

  GList *start_message_queue;
  
  apply_bpm = AGS_APPLY_BPM(task);
  
  if(AGS_IS_APPLICATION_CONTEXT(apply_bpm->scope)){
    AgsApplicationContext *application_context;

    application_context = (AgsApplicationContext *) apply_bpm->scope;

    ags_apply_bpm_application_context(apply_bpm, application_context);

    /* emit message */
    message_delivery = ags_message_delivery_get_instance();

    start_message_queue = ags_message_delivery_find_sender_namespace(message_delivery,
								     "libags-audio");

    if(start_message_queue != NULL){
      AgsMessageEnvelope *message;

      xmlDoc *doc;
      xmlNode *root_node;

      /* specify message body */
      doc = xmlNewDoc("1.0");

      root_node = xmlNewNode(NULL,
			     "ags-command");
      xmlDocSetRootElement(doc, root_node);    

      xmlNewProp(root_node,
		 "method",
		 "AgsApplyBpm::launch");

      /* add message */
      message = ags_message_envelope_new((GObject *) apply_bpm,
					 (GObject *) application_context,
					 doc);

      /* set parameter */
      message->n_params = 2;

      message->parameter_name = (gchar **) malloc(3 * sizeof(gchar *));
      message->value = g_new0(GValue,
			      2);

      /* application-context */
      message->parameter_name[0] = "application-context";
    
      g_value_init(&(message->value[0]),
		   G_TYPE_OBJECT);
      g_value_set_object(&(message->value[0]),
			 application_context);

      /* bpm */
      message->parameter_name[1] = "bpm";
    
      g_value_init(&(message->value[1]),
		   G_TYPE_DOUBLE);
      g_value_set_double(&(message->value[1]),
			 apply_bpm->bpm);
    
      /* terminate string vector */
      message->parameter_name[2] = NULL;
    
      /* add message */
      ags_message_delivery_add_message_envelope(message_delivery,
						"libags-audio",
						(GObject *) message);

      g_list_free_full(start_message_queue,
		       (GDestroyNotify) g_object_unref);
    }
  }else if(AGS_IS_SOUNDCARD(apply_bpm->scope)){
    GObject *soundcard;

    soundcard = apply_bpm->scope;

    ags_apply_bpm_soundcard(apply_bpm, soundcard);
  }else if(AGS_IS_SEQUENCER(apply_bpm->scope)){
    GObject *sequencer;

    sequencer = apply_bpm->scope;

    ags_apply_bpm_sequencer(apply_bpm, sequencer);
  }else if(AGS_IS_AUDIO(apply_bpm->scope)){
    AgsAudio *audio;

    audio = AGS_AUDIO(apply_bpm->scope);
    
    ags_apply_bpm_audio(apply_bpm, audio);
  }else if(AGS_IS_CHANNEL(apply_bpm->scope)){
    AgsChannel *channel;

    channel = AGS_CHANNEL(apply_bpm->scope);

    ags_apply_bpm_channel(apply_bpm, channel);
  }else if(AGS_IS_RECALL(apply_bpm->scope)){
    AgsRecall *recall;

    recall = AGS_RECALL(apply_bpm->scope);

    ags_apply_bpm_recall(apply_bpm, recall);
  }else{
    g_warning("AgsApplyBpm: Not supported scope");
  }
}

void
ags_apply_bpm_recall(AgsApplyBpm *apply_bpm, AgsRecall *recall)
{
  if(AGS_IS_TACTABLE(recall)){
    ags_tactable_change_bpm(AGS_TACTABLE(recall),
			    apply_bpm->bpm,
			    ags_tactable_get_bpm(AGS_TACTABLE(recall)));
  }
}

void
ags_apply_bpm_channel(AgsApplyBpm *apply_bpm, AgsChannel *channel)
{
  GList *list_start, *list;

  /* apply bpm - play */
  g_object_get(channel,
	       "play", &list_start,
	       NULL);

  list = list_start;
  
  while(list != NULL){
    ags_apply_bpm_recall(apply_bpm, AGS_RECALL(list->data));
    
    list = list->next;
  }

  g_list_free_full(list_start,
		   g_object_unref);
  
  /* apply bpm - recall */
  g_object_get(channel,
	       "recall", &list_start,
	       NULL);

  list = list_start;
  
  while(list != NULL){
    ags_apply_bpm_recall(apply_bpm, AGS_RECALL(list->data));
    
    list = list->next;
  }

  g_list_free_full(list_start,
		   g_object_unref);
}

void
ags_apply_bpm_audio(AgsApplyBpm *apply_bpm, AgsAudio *audio)
{
  AgsChannel *input, *output;
  AgsChannel *channel, *next_channel;

  GList *list_start, *list;

  /* get some fields */
  g_object_get(audio,
	       "output", &output,
	       "input", &input,
	       NULL);

  /* set bpm */
  g_object_set(audio,
	       "bpm", apply_bpm->bpm,
	       NULL);
  
  /* apply bpm - play */
  g_object_get(audio,
	       "play", &list_start,
	       NULL);

  list = list_start;
  
  while(list != NULL){
    ags_apply_bpm_recall(apply_bpm, AGS_RECALL(list->data));
    
    list = list->next;
  }

  g_list_free_full(list_start,
		   g_object_unref);
  
  /* apply bpm - recall */
  g_object_get(audio,
	       "recall", &list_start,
	       NULL);

  list = list_start;
  
  while(list != NULL){
    ags_apply_bpm_recall(apply_bpm, AGS_RECALL(list->data));
    
    list = list->next;
  }

  g_list_free_full(list_start,
		   g_object_unref);
  
  /* AgsChannel - output */
  if(output != NULL){
    channel = output;
    g_object_ref(channel);
  
    while(channel != NULL){
      /* apply bpm */
      ags_apply_bpm_channel(apply_bpm, channel);

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
      /* apply bpm */
      ags_apply_bpm_channel(apply_bpm, channel);

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
ags_apply_bpm_soundcard(AgsApplyBpm *apply_bpm, GObject *soundcard)
{
  ags_soundcard_set_bpm(AGS_SOUNDCARD(soundcard), apply_bpm->bpm);
}

void
ags_apply_bpm_sequencer(AgsApplyBpm *apply_bpm, GObject *sequencer)
{
  ags_sequencer_set_bpm(AGS_SEQUENCER(sequencer), apply_bpm->bpm);
}

void
ags_apply_bpm_application_context(AgsApplyBpm *apply_bpm, AgsApplicationContext *application_context)
{
  GList *list_start, *list;

  /* soundcard */
  list =
    list_start = ags_sound_provider_get_soundcard(AGS_SOUND_PROVIDER(application_context));

  while(list != NULL){
    ags_apply_bpm_soundcard(apply_bpm, list->data);

    list = list->next;
  }

  g_list_free_full(list_start,
		   g_object_unref);

  /* sequencer */
  list =
    list_start = ags_sound_provider_get_sequencer(AGS_SOUND_PROVIDER(application_context));

  while(list != NULL){
    ags_apply_bpm_sequencer(apply_bpm, list->data);

    list = list->next;
  }

  g_list_free_full(list_start,
		   g_object_unref);

  /* audio */
  list =
    list_start = ags_sound_provider_get_audio(AGS_SOUND_PROVIDER(application_context));

  while(list != NULL){
    ags_apply_bpm_audio(apply_bpm, list->data);

    list = list->next;
  }

  g_list_free_full(list_start,
		   g_object_unref);
}

/**
 * ags_apply_bpm_new:
 * @scope: the #GObject
 * @bpm: the bpm to apply
 *
 * Create a new instance of #AgsApplyBpm.
 *
 * Returns: the new #AgsApplyBpm
 *
 * Since: 3.0.0
 */
AgsApplyBpm*
ags_apply_bpm_new(GObject *scope,
		  gdouble bpm)
{
  AgsApplyBpm *apply_bpm;

  apply_bpm = (AgsApplyBpm *) g_object_new(AGS_TYPE_APPLY_BPM,
					   "scope", scope,
					   "bpm", bpm,
					   NULL);

  return(apply_bpm);
}
