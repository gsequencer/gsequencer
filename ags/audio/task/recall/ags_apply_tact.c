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

#include <ags/audio/task/recall/ags_apply_tact.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_recall.h>

#include <math.h>

#include <ags/i18n.h>

void ags_apply_tact_class_init(AgsApplyTactClass *apply_tact);
void ags_apply_tact_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_apply_tact_init(AgsApplyTact *apply_tact);
void ags_apply_tact_set_property(GObject *gobject,
				 guint prop_id,
				 const GValue *value,
				 GParamSpec *param_spec);
void ags_apply_tact_get_property(GObject *gobject,
				 guint prop_id,
				 GValue *value,
				 GParamSpec *param_spec);
void ags_apply_tact_connect(AgsConnectable *connectable);
void ags_apply_tact_disconnect(AgsConnectable *connectable);
void ags_apply_tact_dispose(GObject *gobject);
void ags_apply_tact_finalize(GObject *gobject);

void ags_apply_tact_launch(AgsTask *task);

void ags_apply_tact_recall(AgsApplyTact *apply_tact, AgsRecall *recall);
void ags_apply_tact_channel(AgsApplyTact *apply_tact, AgsChannel *channel);
void ags_apply_tact_audio(AgsApplyTact *apply_tact, AgsAudio *audio);
void ags_apply_tact_soundcard(AgsApplyTact *apply_tact, GObject *soundcard);

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
static AgsConnectableInterface *ags_apply_tact_parent_connectable_interface;

GType
ags_apply_tact_get_type()
{
  static GType ags_type_apply_tact = 0;

  if(!ags_type_apply_tact){
    static const GTypeInfo ags_apply_tact_info = {
      sizeof (AgsApplyTactClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_apply_tact_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsApplyTact),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_apply_tact_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_apply_tact_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_apply_tact = g_type_register_static(AGS_TYPE_TASK,
						 "AgsApplyTact",
						 &ags_apply_tact_info,
						 0);
    
    g_type_add_interface_static(ags_type_apply_tact,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_apply_tact);
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
   * AgsApplyTact:tact:
   *
   * The tact to apply to scope.
   * 
   * Since: 1.0.0
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
ags_apply_tact_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_apply_tact_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_apply_tact_connect;
  connectable->disconnect = ags_apply_tact_disconnect;
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
ags_apply_tact_connect(AgsConnectable *connectable)
{
  ags_apply_tact_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_apply_tact_disconnect(AgsConnectable *connectable)
{
  ags_apply_tact_parent_connectable_interface->disconnect(connectable);

  /* empty */
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

 if(AGS_IS_SOUNDCARD(apply_tact->scope)){
    GObject *soundcard;

    soundcard = apply_tact->scope;

    ags_apply_tact_soundcard(apply_tact, soundcard);
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

  /* apply tact */
  pthread_mutex_lock(channel_mutex);

  list = channel->play;

  while(list != NULL){
    ags_apply_tact_recall(apply_tact, AGS_RECALL(list->data));

    list = list->next;
  }

  list = channel->recall;

  while(list != NULL){
    ags_apply_tact_recall(apply_tact, AGS_RECALL(list->data));

    list = list->next;
  }

  pthread_mutex_unlock(channel_mutex);
}

void
ags_apply_tact_audio(AgsApplyTact *apply_tact, AgsAudio *audio)
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
    ags_apply_tact_recall(apply_tact, AGS_RECALL(list->data));

    list = list->next;
  }

  list = audio->recall;

  while(list != NULL){
    ags_apply_tact_recall(apply_tact, AGS_RECALL(list->data));

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

    /* apply tact */
    ags_apply_tact_channel(apply_tact, channel);

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

    /* apply tact */
    ags_apply_tact_channel(apply_tact, channel);

    /* iterate */
    pthread_mutex_lock(channel_mutex);

    channel = channel->next;
    
    pthread_mutex_unlock(channel_mutex);    
  }
}


void
ags_apply_tact_soundcard(AgsApplyTact *apply_tact, GObject *soundcard)
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

  /* set delay factor and get audio */
  pthread_mutex_lock(soundcard);

  ags_soundcard_set_delay_factor(AGS_SOUNDCARD(soundcard), apply_tact->tact);

  list =
    list_start = g_list_copy(ags_soundcard_get_audio(AGS_SOUNDCARD(soundcard)));

  pthread_mutex_unlock(soundcard);

  /* AgsAudio */
  list = ags_soundcard_get_audio(AGS_SOUNDCARD(soundcard));

  while(list != NULL){
    ags_apply_tact_audio(apply_tact,
			 AGS_AUDIO(list->data));

    list = list->next;
  }

  g_list_free(list_start);
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
 * Since: 1.0.0
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
