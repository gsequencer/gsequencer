/* This file is part of GSequencer.
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

#include <ags/audio/task/ags_add_recall.h>

#include <ags-lib/object/ags_connectable.h>

#include <ags/object/ags_dynamic_connectable.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_recall_audio.h>
#include <ags/audio/ags_recall_channel.h>

void ags_add_recall_class_init(AgsAddRecallClass *add_recall);
void ags_add_recall_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_add_recall_init(AgsAddRecall *add_recall);
void ags_add_recall_connect(AgsConnectable *connectable);
void ags_add_recall_disconnect(AgsConnectable *connectable);
void ags_add_recall_finalize(GObject *gobject);

void ags_add_recall_launch(AgsTask *task);

/**
 * SECTION:ags_add_recall
 * @short_description: add recall object to context
 * @title: AgsAddRecall
 * @section_id:
 * @include: ags/audio/task/ags_add_recall.h
 *
 * The #AgsAddRecall task adds #AgsRecall to context.
 */

static gpointer ags_add_recall_parent_class = NULL;
static AgsConnectableInterface *ags_add_recall_parent_connectable_interface;

GType
ags_add_recall_get_type()
{
  static GType ags_type_add_recall = 0;

  if(!ags_type_add_recall){
    static const GTypeInfo ags_add_recall_info = {
      sizeof (AgsAddRecallClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_add_recall_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsAddRecall),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_add_recall_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_add_recall_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_add_recall = g_type_register_static(AGS_TYPE_TASK,
						 "AgsAddRecall\0",
						 &ags_add_recall_info,
						 0);

    g_type_add_interface_static(ags_type_add_recall,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_add_recall);
}

void
ags_add_recall_class_init(AgsAddRecallClass *add_recall)
{
  GObjectClass *gobject;
  AgsTaskClass *task;

  ags_add_recall_parent_class = g_type_class_peek_parent(add_recall);

  /* gobject */
  gobject = (GObjectClass *) add_recall;

  gobject->finalize = ags_add_recall_finalize;

  /* task */
  task = (AgsTaskClass *) add_recall;

  task->launch = ags_add_recall_launch;
}

void
ags_add_recall_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_add_recall_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_add_recall_connect;
  connectable->disconnect = ags_add_recall_disconnect;
}

void
ags_add_recall_init(AgsAddRecall *add_recall)
{
  add_recall->context = NULL;
  add_recall->recall = NULL;
  add_recall->is_play = FALSE;
}

void
ags_add_recall_connect(AgsConnectable *connectable)
{
  ags_add_recall_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_add_recall_disconnect(AgsConnectable *connectable)
{
  ags_add_recall_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_add_recall_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_add_recall_parent_class)->finalize(gobject);

  /* empty */
}

void
ags_add_recall_launch(AgsTask *task)
{
  AgsAddRecall *add_recall;
  
  add_recall = AGS_ADD_RECALL(task);

  if(AGS_IS_AUDIO(add_recall->context)){
    AgsRecall *current;
    GList *recall_id;

    if(g_list_find(AGS_AUDIO(add_recall->context)->container,
		   add_recall->recall->container) == NULL){
      ags_audio_add_recall_container(AGS_AUDIO(add_recall->context),
				     add_recall->recall->container);
    }

    ags_audio_add_recall(AGS_AUDIO(add_recall->context),
			 (GObject *) add_recall->recall,
			 add_recall->is_play);

    /* set up playback, sequencer or notation */
    recall_id = AGS_AUDIO(add_recall->context)->recall_id;
    if(!AGS_IS_RECALL_AUDIO(add_recall->recall)){
      while(recall_id != NULL){
	if((!add_recall->is_play && AGS_RECALL_ID(recall_id->data)->recycling_container->parent == NULL) ||
	   (add_recall->is_play && AGS_RECALL_ID(recall_id->data)->recycling_container->parent != NULL)){
	  recall_id = recall_id->next;
	
	  continue;
	}

	current = ags_recall_duplicate(add_recall->recall, recall_id->data);

	/* set appropriate flag */
	if((AGS_RECALL_ID_PLAYBACK & (AGS_RECALL_ID(recall_id->data)->flags)) != 0){
	  ags_recall_set_flags(current, AGS_RECALL_PLAYBACK);
	}else if((AGS_RECALL_ID_SEQUENCER & (AGS_RECALL_ID(recall_id->data)->flags)) != 0){
	  ags_recall_set_flags(current, AGS_RECALL_SEQUENCER);
	}else if((AGS_RECALL_ID_NOTATION & (AGS_RECALL_ID(recall_id->data)->flags)) != 0){
	  ags_recall_set_flags(current, AGS_RECALL_NOTATION);
	}

	/* append to AgsAudio */
	if(add_recall->is_play)
	  AGS_AUDIO(add_recall->context)->play = g_list_append(AGS_AUDIO(add_recall->context)->play, current);
	else
	  AGS_AUDIO(add_recall->context)->recall = g_list_append(AGS_AUDIO(add_recall->context)->recall, current);

	/* connect */
	ags_connectable_connect(AGS_CONNECTABLE(current));

	/* notify run */
	ags_recall_notify_dependency(current, AGS_RECALL_NOTIFY_RUN, 1);

	/* resolve */
	ags_recall_resolve_dependencies(current);

	/* init */
	ags_dynamic_connectable_connect_dynamic(AGS_DYNAMIC_CONNECTABLE(current));
      
	current->flags &= (~AGS_RECALL_HIDE);
	ags_recall_run_init_pre(current);
	current->flags &= (~AGS_RECALL_REMOVE);
      
	ags_recall_run_init_inter(current);
	ags_recall_run_init_post(current);

	/* iterate */
	recall_id = recall_id->next;
      }
    }
  }else if(AGS_IS_CHANNEL(add_recall->context)){
    AgsRecall *current;
    GList *recall_id;

    if(g_list_find(AGS_CHANNEL(add_recall->context)->container,
		   add_recall->recall->container) == NULL){
      ags_channel_add_recall_container(AGS_CHANNEL(add_recall->context),
				       add_recall->recall->container);
    }

    ags_channel_add_recall(AGS_CHANNEL(add_recall->context),
			   add_recall->recall,
			   add_recall->is_play);

      /* set up playback, sequencer or notation */
    recall_id = AGS_CHANNEL(add_recall->context)->recall_id;

    if(!AGS_IS_RECALL_CHANNEL(add_recall->recall)){
      while(recall_id != NULL){
	if((!add_recall->is_play && AGS_RECALL_ID(recall_id->data)->recycling_container->parent == NULL) ||
	   (add_recall->is_play && AGS_RECALL_ID(recall_id->data)->recycling_container->parent != NULL)){
	  recall_id = recall_id->next;
	
	  continue;
	}
      
	current = ags_recall_duplicate(add_recall->recall, recall_id->data);

	/* set appropriate flag */
	if((AGS_RECALL_ID_PLAYBACK & (AGS_RECALL_ID(recall_id->data)->flags)) != 0){
	  ags_recall_set_flags(current, AGS_RECALL_PLAYBACK);
	}else if((AGS_RECALL_ID_SEQUENCER & (AGS_RECALL_ID(recall_id->data)->flags)) != 0){
	  ags_recall_set_flags(current, AGS_RECALL_SEQUENCER);
	}else if((AGS_RECALL_ID_NOTATION & (AGS_RECALL_ID(recall_id->data)->flags)) != 0){
	  ags_recall_set_flags(current, AGS_RECALL_NOTATION);
	}

	/* append to AgsAudio */
	if(add_recall->is_play)
	  AGS_CHANNEL(add_recall->context)->play = g_list_append(AGS_CHANNEL(add_recall->context)->play, current);
	else
	  AGS_CHANNEL(add_recall->context)->recall = g_list_append(AGS_CHANNEL(add_recall->context)->recall, current);

	/* connect */
	ags_connectable_connect(AGS_CONNECTABLE(current));

	/* notify run */
	ags_recall_notify_dependency(current, AGS_RECALL_NOTIFY_RUN, 1);

	/* resolve */
	ags_recall_resolve_dependencies(current);

	/* init */
	ags_dynamic_connectable_connect_dynamic(AGS_DYNAMIC_CONNECTABLE(current));
      
	current->flags &= (~AGS_RECALL_HIDE);
	ags_recall_run_init_pre(current);
	current->flags &= (~AGS_RECALL_REMOVE);
      
	ags_recall_run_init_inter(current);
	ags_recall_run_init_post(current);

	/* iterate */
	recall_id = recall_id->next;
      }
    }
  }else if(AGS_IS_RECALL(add_recall->context)){
    ags_recall_add_child(AGS_RECALL(add_recall->context),
			 add_recall->recall);
  }

  ags_connectable_connect(AGS_CONNECTABLE(add_recall->recall));
}

/**
 * ags_add_recall_new:
 * @context: may be #AgsAudio, #AgsChannel or #AgsRecall
 * @recall: the #AgsRecall to add
 * @is_play: if %TRUE non-complex recall
 *
 * Creates an #AgsAddRecall.
 *
 * Returns: an new #AgsAddRecall.
 *
 * Since: 0.4
 */
AgsAddRecall*
ags_add_recall_new(GObject *context,
		   AgsRecall *recall,
		   gboolean is_play)
{
  AgsAddRecall *add_recall;

  add_recall = (AgsAddRecall *) g_object_new(AGS_TYPE_ADD_RECALL,
					     NULL);

  add_recall->context = context;
  add_recall->recall = recall;
  add_recall->is_play = is_play;

  return(add_recall);
}
