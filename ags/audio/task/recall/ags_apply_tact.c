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

#include <ags/audio/task/recall/ags_apply_tact.h>

#include <ags/object/ags_connectable.h>
#include <ags/object/ags_tactable.h>

#include <ags/object/ags_soundcard.h>
#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_recall.h>

#include <math.h>

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
						 "AgsApplyTact\0",
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

  gobject->finalize = ags_apply_tact_finalize;

  /* properties */
  /**
   * AgsApplyTact:scope:
   *
   * The assigned #GObject as scope.
   * 
   * Since: 0.7.117
   */
  param_spec = g_param_spec_object("scope\0",
				   "scope of set buffer size\0",
				   "The scope of set buffer size\0",
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
   * Since: 0.7.117
   */
  param_spec = g_param_spec_double("tact\0",
				   "tact\0",
				   "The tact to apply\0",
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
ags_apply_tact_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_apply_tact_parent_class)->finalize(gobject);

  /* empty */
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
    g_warning("AgsApplyTact: Not supported scope\0");
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
  GList *list;

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
}

void
ags_apply_tact_audio(AgsApplyTact *apply_tact, AgsAudio *audio)
{
  AgsChannel *channel;
  GList *list;

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

  /* AgsChannel */
  channel = audio->output;

  while(channel != NULL){
    ags_apply_tact_channel(apply_tact, channel);

    channel = channel->next;
  }

  channel = audio->input;

  while(channel != NULL){
    ags_apply_tact_channel(apply_tact, channel);

    channel = channel->next;
  }
}


void
ags_apply_tact_soundcard(AgsApplyTact *apply_tact, GObject *soundcard)
{
  GList *list;

  ags_soundcard_set_delay_factor(AGS_SOUNDCARD(soundcard), apply_tact->tact);

  /* AgsAudio */
  list = ags_soundcard_get_audio(AGS_SOUNDCARD(soundcard));

  while(list != NULL){
    ags_apply_tact_audio(apply_tact,
			 AGS_AUDIO(list->data));

    list = list->next;
  }
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
 * Since: 0.4
 */
AgsApplyTact*
ags_apply_tact_new(GObject *scope,
		   gdouble tact)
{
  AgsApplyTact *apply_tact;
  
  apply_tact = (AgsApplyTact *) g_object_new(AGS_TYPE_APPLY_TACT,
					     NULL);
  
  apply_tact->scope = scope;
  apply_tact->tact = tact;

  return(apply_tact);
}
