/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2005-2011 Joël Krähemann
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <ags/audio/task/recall/ags_apply_tact.h>

#include <ags-lib/object/ags_connectable.h>
#include <ags/object/ags_tactable.h>

#include <ags/audio/ags_devout.h>
#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_recall.h>

#include <math.h>

void ags_apply_tact_class_init(AgsApplyTactClass *apply_tact);
void ags_apply_tact_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_apply_tact_init(AgsApplyTact *apply_tact);
void ags_apply_tact_connect(AgsConnectable *connectable);
void ags_apply_tact_disconnect(AgsConnectable *connectable);
void ags_apply_tact_finalize(GObject *gobject);

void ags_apply_tact_launch(AgsTask *task);

void ags_apply_tact_recall(AgsApplyTact *apply_tact, AgsRecall *recall);
void ags_apply_tact_channel(AgsApplyTact *apply_tact, AgsChannel *channel);
void ags_apply_tact_audio(AgsApplyTact *apply_tact, AgsAudio *audio);

/**
 * SECTION:ags_apply_tact
 * @short_description: apply tact to delay audio
 * @title: AgsApplyTact
 * @section_id:
 * @include: ags/tact/task/ags_apply_tact.h
 *
 * The #AgsApplyTact task applys tact to #AgsTactable.
 */

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

  ags_apply_tact_parent_class = g_type_class_peek_parent(apply_tact);

  /* GObjectClass */
  gobject = (GObjectClass *) apply_tact;

  gobject->finalize = ags_apply_tact_finalize;

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
  apply_tact->gobject = NULL;
  apply_tact->tact = 0.0;
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

  if(AGS_IS_AUDIO(apply_tact->gobject)){
    AgsAudio *audio;

    audio = AGS_AUDIO(apply_tact->gobject);

    ags_apply_tact_audio(apply_tact, audio);
  }else if(AGS_IS_CHANNEL(apply_tact->gobject)){
    AgsChannel *channel;

    channel = AGS_CHANNEL(apply_tact->gobject);

    ags_apply_tact_channel(apply_tact, channel);
  }else if(AGS_IS_RECALL(apply_tact->gobject)){
    AgsRecall *recall;

    recall = AGS_RECALL(apply_tact->gobject);

    ags_apply_tact_recall(apply_tact, recall);
  }else{
    g_warning("AgsApplyTact: Not supported gobject\0");
  }
}

void
ags_apply_tact_recall(AgsApplyTact *apply_tact, AgsRecall *recall)
{
  if(AGS_IS_TACTABLE(recall)){
    ags_tactable_change_tact(AGS_TACTABLE(recall), apply_tact->tact);
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

/**
 * ags_apply_tact_new:
 * @gobject: the #GObject
 * @tact: the tact to apply
 *
 * Creates an #AgsApplyTact.
 *
 * Returns: an new #AgsApplyTact.
 *
 * Since: 0.4
 */
AgsApplyTact*
ags_apply_tact_new(GObject *gobject,
		   gdouble tact)
{
  AgsApplyTact *apply_tact;
  
  apply_tact = (AgsApplyTact *) g_object_new(AGS_TYPE_APPLY_TACT,
					     NULL);
  
  apply_tact->gobject = gobject;
  apply_tact->tact = tact;

  return(apply_tact);
}
