/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2015 Joël Krähemann
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

#include <ags/audio/task/recall/ags_apply_bpm.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_recall.h>

#include <math.h>

#include <ags/i18n.h>

void ags_apply_bpm_class_init(AgsApplyBpmClass *apply_bpm);
void ags_apply_bpm_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_apply_bpm_init(AgsApplyBpm *apply_bpm);
void ags_apply_bpm_set_property(GObject *gobject,
				guint prop_id,
				const GValue *value,
				GParamSpec *param_spec);
void ags_apply_bpm_get_property(GObject *gobject,
				guint prop_id,
				GValue *value,
				GParamSpec *param_spec);
void ags_apply_bpm_connect(AgsConnectable *connectable);
void ags_apply_bpm_disconnect(AgsConnectable *connectable);
void ags_apply_bpm_dispose(GObject *gobject);
void ags_apply_bpm_finalize(GObject *gobject);

void ags_apply_bpm_launch(AgsTask *task);

void ags_apply_bpm_recall(AgsApplyBpm *apply_bpm, AgsRecall *recall);
void ags_apply_bpm_channel(AgsApplyBpm *apply_bpm, AgsChannel *channel);
void ags_apply_bpm_audio(AgsApplyBpm *apply_bpm, AgsAudio *audio);
void ags_apply_bpm_soundcard(AgsApplyBpm *apply_bpm, GObject *soundcard);

/**
 * SECTION:ags_apply_bpm
 * @short_description: apply bpm to delay audio
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
static AgsConnectableInterface *ags_apply_bpm_parent_connectable_interface;

GType
ags_apply_bpm_get_type()
{
  static GType ags_type_apply_bpm = 0;

  if(!ags_type_apply_bpm){
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

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_apply_bpm_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_apply_bpm = g_type_register_static(AGS_TYPE_TASK,
						"AgsApplyBpm",
						&ags_apply_bpm_info,
						0);
    
    g_type_add_interface_static(ags_type_apply_bpm,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_apply_bpm);
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
   * Since: 1.0.0
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
   * Since: 1.0.0
   */
  param_spec = g_param_spec_double("bpm",
				   i18n_pspec("bpm"),
				   i18n_pspec("The bpm to apply"),
				   0,
				   G_MAXDOUBLE,
				   0,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_BPM,
				  param_spec);

  /* AgsTaskClass */
  task = (AgsTaskClass *) apply_bpm;
  
  task->launch = ags_apply_bpm_launch;
}

void
ags_apply_bpm_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_apply_bpm_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_apply_bpm_connect;
  connectable->disconnect = ags_apply_bpm_disconnect;
}

void
ags_apply_bpm_init(AgsApplyBpm *apply_bpm)
{
  apply_bpm->scope = NULL;
  apply_bpm->bpm = 0.0;
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
ags_apply_bpm_connect(AgsConnectable *connectable)
{
  ags_apply_bpm_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_apply_bpm_disconnect(AgsConnectable *connectable)
{
  ags_apply_bpm_parent_connectable_interface->disconnect(connectable);

  /* empty */
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
  
  apply_bpm = AGS_APPLY_BPM(task);
  
  if(AGS_IS_SOUNDCARD(apply_bpm->scope)){
    GObject *soundcard;

    soundcard = apply_bpm->scope;

    ags_apply_bpm_soundcard(apply_bpm, soundcard);
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
			    ags_tactable_get_bpm(AGS_TACTABLE(recall)),
			    apply_bpm->bpm);
  }
}

void
ags_apply_bpm_channel(AgsApplyBpm *apply_bpm, AgsChannel *channel)
{
  AgsMutexManager *mutex_manager;

  GList *list;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *channel_mutex;

  /* get mutex manager and application mutex */
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  /* get channel mutex */
  pthread_mutex_lock(application_mutex);
  
  channel_mutex = ags_mutex_manager_lookup(mutex_manager,
					   (GObject *) channel);

  pthread_mutex_unlock(application_mutex);

  /* apply bpm */
  pthread_mutex_lock(channel_mutex);

  list = channel->play;
  
  while(list != NULL){
    ags_apply_bpm_recall(apply_bpm, AGS_RECALL(list->data));
    
    list = list->next;
  }
  
  list = channel->recall;
  
  while(list != NULL){
    ags_apply_bpm_recall(apply_bpm, AGS_RECALL(list->data));
    
    list = list->next;
  }

  pthread_mutex_unlock(channel_mutex);
}

void
ags_apply_bpm_audio(AgsApplyBpm *apply_bpm, AgsAudio *audio)
{
  AgsChannel *input, *output;
  AgsChannel *channel;

  AgsMutexManager *mutex_manager;

  GList *list;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *audio_mutex;
  pthread_mutex_t *channel_mutex;

  /* get mutex manager and application mutex */
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  /* get audio mutex */
  pthread_mutex_lock(application_mutex);

  audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					 (GObject *) audio);

  pthread_mutex_unlock(application_mutex);

  /* get some fields */
  pthread_mutex_lock(audio_mutex);

  output = audio->output;
  input = audio->input;

  /* AgsRecall */
  list = audio->play;
    
  while(list != NULL){
    ags_apply_bpm_recall(apply_bpm, AGS_RECALL(list->data));
    
    list = list->next;
  }
  
  list = audio->recall;
  
  while(list != NULL){
    ags_apply_bpm_recall(apply_bpm, AGS_RECALL(list->data));
    
    list = list->next;
  }

  pthread_mutex_unlock(audio_mutex);
  
  /* AgsChannel */
  channel = output;

  while(channel != NULL){
    /* get channel mutex */
    pthread_mutex_lock(application_mutex);

    channel_mutex = ags_mutex_manager_lookup(mutex_manager,
					     (GObject *) channel);

    pthread_mutex_unlock(application_mutex);

    /* apply bpm */
    ags_apply_bpm_channel(apply_bpm, channel);

    /* iterate */
    pthread_mutex_lock(channel_mutex);

    channel = channel->next;
    
    pthread_mutex_unlock(channel_mutex);    
  }

  channel = input;

  while(channel != NULL){
    /* get channel mutex */
    pthread_mutex_lock(application_mutex);

    channel_mutex = ags_mutex_manager_lookup(mutex_manager,
					     (GObject *) channel);

    pthread_mutex_unlock(application_mutex);

    /* apply bpm */
    ags_apply_bpm_channel(apply_bpm, channel);

    /* iterate */
    pthread_mutex_lock(channel_mutex);

    channel = channel->next;
    
    pthread_mutex_unlock(channel_mutex);    
  }
}

void
ags_apply_bpm_soundcard(AgsApplyBpm *apply_bpm, GObject *soundcard)
{
  AgsMutexManager *mutex_manager;
  
  GList *list_start, *list;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *soundcard_mutex;

  /* get mutex manager and application mutex */
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  /* get soundcard mutex */
  pthread_mutex_lock(application_mutex);
  
  soundcard_mutex = ags_mutex_manager_lookup(mutex_manager,
					     (GObject *) apply_bpm->soundcard);

  pthread_mutex_unlock(application_mutex);

  /* set bpm and get audio */
  pthread_mutex_lock(soundcard);

  ags_soundcard_set_bpm(AGS_SOUNDCARD(soundcard), apply_bpm->bpm);

  list =
    list_start = g_list_copy(ags_soundcard_get_audio(AGS_SOUNDCARD(soundcard)));

  pthread_mutex_unlock(soundcard);

  /* AgsAudio */
  while(list != NULL){
    ags_apply_bpm_audio(apply_bpm,
			AGS_AUDIO(list->data));

    list = list->next;
  }

  g_list_free(list_start);
}

/**
 * ags_apply_bpm_new:
 * @scope: the #GObject
 * @bpm: the bpm to apply
 *
 * Creates an #AgsApplyBpm.
 *
 * Returns: an new #AgsApplyBpm.
 *
 * Since: 1.0.0
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
