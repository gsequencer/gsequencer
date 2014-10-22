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

#include <ags/audio/task/recall/ags_apply_bpm.h>

#include <ags-lib/object/ags_connectable.h>
#include <ags/object/ags_tactable.h>

#include <ags/audio/ags_devout.h>
#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_recall.h>

#include <math.h>

void ags_apply_bpm_class_init(AgsApplyBpmClass *apply_bpm);
void ags_apply_bpm_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_apply_bpm_init(AgsApplyBpm *apply_bpm);
void ags_apply_bpm_connect(AgsConnectable *connectable);
void ags_apply_bpm_disconnect(AgsConnectable *connectable);
void ags_apply_bpm_finalize(GObject *gobject);

void ags_apply_bpm_launch(AgsTask *task);

void ags_apply_bpm_recall(AgsApplyBpm *apply_bpm, AgsRecall *recall);
void ags_apply_bpm_channel(AgsApplyBpm *apply_bpm, AgsChannel *channel);
void ags_apply_bpm_audio(AgsApplyBpm *apply_bpm, AgsAudio *audio);
void ags_apply_bpm_devout(AgsApplyBpm *apply_bpm, AgsDevout *devout);

/**
 * SECTION:ags_apply_bpm
 * @short_description: apply bpm to delay audio
 * @title: AgsApplyBpm
 * @section_id:
 * @include: ags/bpm/task/ags_apply_bpm.h
 *
 * The #AgsApplyBpm task applys bpm to #AgsDelayAudio.
 */

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
						"AgsApplyBpm\0",
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

  ags_apply_bpm_parent_class = g_type_class_peek_parent(apply_bpm);

  /* GObjectClass */
  gobject = (GObjectClass *) apply_bpm;

  gobject->finalize = ags_apply_bpm_finalize;

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
  apply_bpm->gobject = NULL;
  apply_bpm->bpm = 0.0;
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
ags_apply_bpm_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_apply_bpm_parent_class)->finalize(gobject);

  /* empty */
}

void
ags_apply_bpm_launch(AgsTask *task)
{
  AgsApplyBpm *apply_bpm;
  
  apply_bpm = AGS_APPLY_BPM(task);
  
  if(AGS_IS_DEVOUT(apply_bpm->gobject)){
    AgsDevout *devout;

    devout = AGS_DEVOUT(apply_bpm->gobject);

    ags_apply_bpm_devout(apply_bpm, devout);
  }else if(AGS_IS_AUDIO(apply_bpm->gobject)){
    AgsAudio *audio;

    audio = AGS_AUDIO(apply_bpm->gobject);
    
    ags_apply_bpm_audio(apply_bpm, audio);
  }else if(AGS_IS_CHANNEL(apply_bpm->gobject)){
    AgsChannel *channel;

    channel = AGS_CHANNEL(apply_bpm->gobject);

    ags_apply_bpm_channel(apply_bpm, channel);
  }else if(AGS_IS_RECALL(apply_bpm->gobject)){
    AgsRecall *recall;

    recall = AGS_RECALL(apply_bpm->gobject);

    ags_apply_bpm_recall(apply_bpm, recall);
  }else{
    g_warning("AgsApplyBpm: Not supported gobject\0");
  }
}

void
ags_apply_bpm_recall(AgsApplyBpm *apply_bpm, AgsRecall *recall)
{
  if(AGS_IS_TACTABLE(recall)){
    ags_tactable_change_bpm(AGS_TACTABLE(recall), apply_bpm->bpm);
  }
}

void
ags_apply_bpm_channel(AgsApplyBpm *apply_bpm, AgsChannel *channel)
{
  GList *list;
    
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
}

void
ags_apply_bpm_audio(AgsApplyBpm *apply_bpm, AgsAudio *audio)
{
  AgsChannel *channel;
  GList *list;

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
  
  /* AgsChannel */
  channel = audio->output;

  while(channel != NULL){
    ags_apply_bpm_channel(apply_bpm, channel);

    channel = channel->next;
  }

  channel = audio->input;

  while(channel != NULL){
    ags_apply_bpm_channel(apply_bpm, channel);

    channel = channel->next;
  }
}

void
ags_apply_bpm_devout(AgsApplyBpm *apply_bpm, AgsDevout *devout)
{
  GList *list;

  //TODO:JK: implement me

  /* AgsAudio */
  list = devout->audio;

  while(list != NULL){
    ags_apply_bpm_audio(apply_bpm,
			AGS_AUDIO(list->data));

    list = list->next;
  }
}

/**
 * ags_apply_bpm_new:
 * @gobject: the #GObject
 * @bpm: the bpm to apply
 *
 * Creates an #AgsApplyBpm.
 *
 * Returns: an new #AgsApplyBpm.
 *
 * Since: 0.4
 */
AgsApplyBpm*
ags_apply_bpm_new(GObject *gobject,
		  gdouble bpm)
{
  AgsApplyBpm *apply_bpm;

  apply_bpm = (AgsApplyBpm *) g_object_new(AGS_TYPE_APPLY_BPM,
					   NULL);

  apply_bpm->gobject = gobject;
  apply_bpm->bpm = bpm;

  return(apply_bpm);
}
