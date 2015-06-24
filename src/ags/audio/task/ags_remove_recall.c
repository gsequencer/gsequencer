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

#include <ags/audio/task/ags_remove_recall.h>

#include <ags-lib/object/ags_connectable.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_recall_container.h>
#include <ags/audio/ags_recall_audio_run.h>
#include <ags/audio/ags_recall_channel_run.h>

#include <math.h>

void ags_remove_recall_class_init(AgsRemoveRecallClass *remove_recall);
void ags_remove_recall_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_remove_recall_init(AgsRemoveRecall *remove_recall);
void ags_remove_recall_connect(AgsConnectable *connectable);
void ags_remove_recall_disconnect(AgsConnectable *connectable);
void ags_remove_recall_finalize(GObject *gobject);

void ags_remove_recall_launch(AgsTask *task);

/**
 * SECTION:ags_remove_recall
 * @short_description: remove recall object to context
 * @title: AgsRemoveRecall
 * @section_id:
 * @include: ags/audio/task/ags_remove_recall.h
 *
 * The #AgsRemoveRecall task removes #AgsRecall to context.
 */

static gpointer ags_remove_recall_parent_class = NULL;
static AgsConnectableInterface *ags_remove_recall_parent_connectable_interface;

GType
ags_remove_recall_get_type()
{
  static GType ags_type_remove_recall = 0;

  if(!ags_type_remove_recall){
    static const GTypeInfo ags_remove_recall_info = {
      sizeof (AgsRemoveRecallClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_remove_recall_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsRemoveRecall),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_remove_recall_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_remove_recall_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_remove_recall = g_type_register_static(AGS_TYPE_TASK,
						  "AgsRemoveRecall\0",
						  &ags_remove_recall_info,
						  0);

    g_type_add_interface_static(ags_type_remove_recall,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_remove_recall);
}

void
ags_remove_recall_class_init(AgsRemoveRecallClass *remove_recall)
{
  GObjectClass *gobject;
  AgsTaskClass *task;

  ags_remove_recall_parent_class = g_type_class_peek_parent(remove_recall);

  /* GObjectClass */
  gobject = (GObjectClass *) remove_recall;

  gobject->finalize = ags_remove_recall_finalize;

  /* AgsTaskClass */
  task = (AgsTaskClass *) remove_recall;

  task->launch = ags_remove_recall_launch;
}

void
ags_remove_recall_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_remove_recall_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_remove_recall_connect;
  connectable->disconnect = ags_remove_recall_disconnect;
}

void
ags_remove_recall_init(AgsRemoveRecall *remove_recall)
{
  remove_recall->context = NULL;
  remove_recall->recall = NULL;
  remove_recall->is_play = FALSE;
  remove_recall->remove_all = FALSE;
}

void
ags_remove_recall_connect(AgsConnectable *connectable)
{
  ags_remove_recall_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_remove_recall_disconnect(AgsConnectable *connectable)
{
  ags_remove_recall_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_remove_recall_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_remove_recall_parent_class)->finalize(gobject);

  /* empty */
}

void
ags_remove_recall_launch(AgsTask *task)
{
  AgsRemoveRecall *remove_recall;

  remove_recall = AGS_REMOVE_RECALL(task);

  if(AGS_IS_AUDIO(remove_recall->context)){
    if(remove_recall->remove_all){
      GList *list;

      if(AGS_IS_RECALL_AUDIO_RUN(remove_recall->recall)){
	list = AGS_RECALL_CONTAINER(remove_recall->recall->container)->recall_audio_run;

	while(list != NULL){
	  if(list->data != remove_recall->recall){
	    ags_audio_remove_recall(AGS_AUDIO(remove_recall->context),
				    (GObject *) list->data,
				    remove_recall->is_play);
	  }
	  
	  list = list->next;	  
	}
      }
    }

    ags_audio_remove_recall(AGS_AUDIO(remove_recall->context),
			    (GObject *) remove_recall->recall,
			    remove_recall->is_play);

  }else if(AGS_IS_CHANNEL(remove_recall->context)){
    if(remove_recall->remove_all){
      GList *list;

      if(AGS_IS_RECALL_CHANNEL_RUN(remove_recall->recall)){
	list = AGS_RECALL_CONTAINER(remove_recall->recall->container)->recall_channel_run;

	while(list != NULL){
	  if(list->data != remove_recall->recall){
	    ags_channel_remove_recall(AGS_CHANNEL(remove_recall->context),
				      (GObject *) list->data,
				      remove_recall->is_play);
	  }
	  
	  list = list->next;
	}
      }
    }

    ags_channel_remove_recall(AGS_CHANNEL(remove_recall->context),
			      (GObject *) remove_recall->recall,
			      remove_recall->is_play);
  }else if(AGS_IS_RECALL(remove_recall->context)){
    ags_recall_remove_child(AGS_RECALL(remove_recall->context),
			    remove_recall->recall);
  }else{
    ags_recall_remove(remove_recall->recall);
  }
}

/**
 * ags_remove_recall_new:
 * @context: may be #AgsAudio, #AgsChannel or #AgsRecall
 * @recall: the #AgsRecall to remove
 * @is_play: if %TRUE non-complex recall
 * @remove_all: if %TRUE all related will be removed
 *
 * Creates an #AgsRemoveRecall.
 *
 * Returns: an new #AgsRemoveRecall.
 *
 * Since: 0.4
 */
AgsRemoveRecall*
ags_remove_recall_new(GObject *context,
		      AgsRecall *recall,
		      gboolean is_play,
		      gboolean remove_all)
{
  AgsRemoveRecall *remove_recall;

  remove_recall = (AgsRemoveRecall *) g_object_new(AGS_TYPE_REMOVE_RECALL,
						   NULL);  

  remove_recall->context = context;
  remove_recall->recall = recall;
  remove_recall->is_play = is_play;
  remove_recall->remove_all = remove_all;

  return(remove_recall);
}
