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

#include <ags/audio/task/recall/ags_set_muted.h>

#include <ags-lib/object/ags_connectable.h>
#include <ags/object/ags_mutable.h>

#include <ags/audio/ags_devout.h>
#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_recall.h>

#include <math.h>

void ags_set_muted_class_init(AgsSetMutedClass *set_muted);
void ags_set_muted_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_set_muted_init(AgsSetMuted *set_muted);
void ags_set_muted_connect(AgsConnectable *connectable);
void ags_set_muted_disconnect(AgsConnectable *connectable);
void ags_set_muted_finalize(GObject *gobject);

void ags_set_muted_launch(AgsTask *task);

void ags_set_muted_recall(AgsSetMuted *set_muted, AgsRecall *recall);
void ags_set_muted_channel(AgsSetMuted *set_muted, AgsChannel *channel);
void ags_set_muted_audio(AgsSetMuted *set_muted, AgsAudio *audio);

static gpointer ags_set_muted_parent_class = NULL;
static AgsConnectableInterface *ags_set_muted_parent_connectable_interface;

GType
ags_set_muted_get_type()
{
  static GType ags_type_set_muted = 0;

  if(!ags_type_set_muted){
    static const GTypeInfo ags_set_muted_info = {
      sizeof (AgsSetMutedClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_set_muted_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsSetMuted),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_set_muted_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_set_muted_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_set_muted = g_type_register_static(AGS_TYPE_TASK,
						 "AgsSetMuted\0",
						 &ags_set_muted_info,
						 0);
    
    g_type_add_interface_static(ags_type_set_muted,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_set_muted);
}

void
ags_set_muted_class_init(AgsSetMutedClass *set_muted)
{
  GObjectClass *gobject;
  AgsTaskClass *task;

  ags_set_muted_parent_class = g_type_class_peek_parent(set_muted);

  /* GObjectClass */
  gobject = (GObjectClass *) set_muted;

  gobject->finalize = ags_set_muted_finalize;

  /* AgsTaskClass */
  task = (AgsTaskClass *) set_muted;
  
  task->launch = ags_set_muted_launch;
}

void
ags_set_muted_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_set_muted_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_set_muted_connect;
  connectable->disconnect = ags_set_muted_disconnect;
}

void
ags_set_muted_init(AgsSetMuted *set_muted)
{
  set_muted->gobject = NULL;
  set_muted->muted = FALSE;
}

void
ags_set_muted_connect(AgsConnectable *connectable)
{
  ags_set_muted_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_set_muted_disconnect(AgsConnectable *connectable)
{
  ags_set_muted_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_set_muted_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_set_muted_parent_class)->finalize(gobject);

  /* empty */
}

void
ags_set_muted_launch(AgsTask *task)
{
  AgsSetMuted *set_muted;

  set_muted = AGS_SET_MUTED(task);

  if(AGS_IS_AUDIO(set_muted->gobject)){
    AgsAudio *audio;

    audio = AGS_AUDIO(set_muted->gobject);

    ags_set_muted_audio(set_muted, audio);
  }else if(AGS_IS_CHANNEL(set_muted->gobject)){
    AgsChannel *channel;

    channel = AGS_CHANNEL(set_muted->gobject);

    ags_set_muted_channel(set_muted, channel);
  }else if(AGS_IS_RECALL(set_muted->gobject)){
    AgsRecall *recall;

    recall = AGS_RECALL(set_muted->gobject);

    ags_set_muted_recall(set_muted, recall);
  }else{
    g_warning("AgsSetMuted: Not supported gobject\0");
  }
}

void
ags_set_muted_recall(AgsSetMuted *set_muted, AgsRecall *recall)
{
  if(AGS_IS_MUTABLE(recall)){
    ags_mutable_set_muted(AGS_MUTABLE(recall), set_muted->muted);
  }
}

void
ags_set_muted_channel(AgsSetMuted *set_muted, AgsChannel *channel)
{
  GList *list;

  list = channel->play;

  while(list != NULL){
    ags_set_muted_recall(set_muted, AGS_RECALL(list->data));

    list = list->next;
  }

  list = channel->recall;

  while(list != NULL){
    ags_set_muted_recall(set_muted, AGS_RECALL(list->data));

    list = list->next;
  }
}

void
ags_set_muted_audio(AgsSetMuted *set_muted, AgsAudio *audio)
{
  AgsChannel *channel;
  GList *list;

  /* AgsRecall */
  list = audio->play;

  while(list != NULL){
    ags_set_muted_recall(set_muted, AGS_RECALL(list->data));

    list = list->next;
  }

  list = audio->recall;

  while(list != NULL){
    ags_set_muted_recall(set_muted, AGS_RECALL(list->data));

    list = list->next;
  }

  /* AgsChannel */
  channel = audio->output;

  while(channel != NULL){
    ags_set_muted_channel(set_muted, channel);

    channel = channel->next;
  }

  channel = audio->input;

  while(channel != NULL){
    ags_set_muted_channel(set_muted, channel);

    channel = channel->next;
  }
}

AgsSetMuted*
ags_set_muted_new(GObject *gobject,
		  gboolean muted)
{
  AgsSetMuted *set_muted;
  
  set_muted = (AgsSetMuted *) g_object_new(AGS_TYPE_SET_MUTED,
					   NULL);
  
  set_muted->gobject = gobject;
  set_muted->muted = muted;

  return(set_muted);
}
