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

#include <ags/audio/ags_recall_audio_run.h>

#include <ags/object/ags_marshal.h>
#include <ags-lib/object/ags_connectable.h>
#include <ags/object/ags_packable.h>
#include <ags/object/ags_dynamic_connectable.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_recall_audio.h>
#include <ags/audio/ags_recall_channel_run.h>
#include <ags/audio/ags_recall_container.h>

#include <ags/audio/recall/ags_copy_pattern_audio_run.h>

void ags_recall_audio_run_class_init(AgsRecallAudioRunClass *recall_audio_run);
void ags_recall_audio_run_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_recall_audio_run_packable_interface_init(AgsPackableInterface *packable);
void ags_recall_audio_run_dynamic_connectable_interface_init(AgsDynamicConnectableInterface *dynamic_connectable);
void ags_recall_audio_run_init(AgsRecallAudioRun *recall_audio_run);
void ags_recall_audio_run_set_property(GObject *gobject,
				       guint prop_id,
				       const GValue *value,
				       GParamSpec *param_spec);
void ags_recall_audio_run_get_property(GObject *gobject,
				       guint prop_id,
				       GValue *value,
				       GParamSpec *param_spec);
void ags_recall_audio_run_connect(AgsConnectable *connectable);
void ags_recall_audio_run_disconnect(AgsConnectable *connectable);
gboolean ags_recall_audio_run_pack(AgsPackable *packable, GObject *container);
gboolean ags_recall_audio_run_unpack(AgsPackable *packable);
void ags_recall_audio_run_connect_dynamic(AgsDynamicConnectable *dynamic_connectable);
void ags_recall_audio_run_disconnect_dynamic(AgsDynamicConnectable *dynamic_connectable);
void ags_recall_audio_run_finalize(GObject *gobject);

AgsRecall* ags_recall_audio_run_duplicate(AgsRecall *recall,
					  AgsRecallID *recall_id,
					  guint *n_params, GParameter *parameter);
/**
 * SECTION:ags_recall_audio_run
 * @Short_description: audio context of dynamic recall
 * @Title: AgsRecallAudioRun
 *
 * #AgsRecallAudioRun acts as dynamic audio recall.
 */

enum{
  LAST_SIGNAL,
};

enum{
  PROP_0,
  PROP_RECALL_AUDIO,
};

static gpointer ags_recall_audio_run_parent_class = NULL;
static AgsConnectableInterface* ags_recall_audio_run_parent_connectable_interface;
static AgsPackableInterface* ags_recall_audio_run_parent_packable_interface;
static AgsDynamicConnectableInterface *ags_recall_audio_run_parent_dynamic_connectable_interface;
static guint recall_audio_run_signals[LAST_SIGNAL];

GType
ags_recall_audio_run_get_type()
{
  static GType ags_type_recall_audio_run = 0;

  if(!ags_type_recall_audio_run){
    static const GTypeInfo ags_recall_audio_run_info = {
      sizeof (AgsRecallAudioRunClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_recall_audio_run_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsRecallAudioRun),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_recall_audio_run_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_recall_audio_run_dynamic_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_packable_interface_info = {
      (GInterfaceInitFunc) ags_recall_audio_run_packable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_dynamic_connectable_interface_info = {
      (GInterfaceInitFunc) ags_recall_audio_run_dynamic_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_recall_audio_run = g_type_register_static(AGS_TYPE_RECALL,
						       "AgsRecallAudioRun\0",
						       &ags_recall_audio_run_info,
						       0);

    g_type_add_interface_static(ags_type_recall_audio_run,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_recall_audio_run,
				AGS_TYPE_PACKABLE,
				&ags_packable_interface_info);

    g_type_add_interface_static(ags_type_recall_audio_run,
				AGS_TYPE_DYNAMIC_CONNECTABLE,
				&ags_dynamic_connectable_interface_info);
  }

  return(ags_type_recall_audio_run);
}

void
ags_recall_audio_run_class_init(AgsRecallAudioRunClass *recall_audio_run)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;
  GParamSpec *param_spec;

  ags_recall_audio_run_parent_class = g_type_class_peek_parent(recall_audio_run);

  /* GObjectClass */
  gobject = (GObjectClass *) recall_audio_run;

  gobject->set_property = ags_recall_audio_run_set_property;
  gobject->get_property = ags_recall_audio_run_get_property;

  gobject->finalize = ags_recall_audio_run_finalize;

  /* properties */
  /**
   * AgsRecallAudioRun:recall-audio:
   *
   * The recall audio belonging to.
   * 
   * Since: 0.4.0
   */
  param_spec = g_param_spec_object("recall-audio\0",
				   "AgsRecallAudio of this recall\0",
				   "The AgsRecallAudio which this recall needs\0",
				   AGS_TYPE_RECALL_AUDIO,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_RECALL_AUDIO,
				  param_spec);

  /* AgsRecallClass */
  recall = (AgsRecallClass *) recall_audio_run;

  recall->duplicate = ags_recall_audio_run_duplicate;

  /* AgsRecallAudioRunClass */
}

void
ags_recall_audio_runconnectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_recall_audio_run_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_recall_audio_run_connect;
  connectable->disconnect = ags_recall_audio_run_disconnect;
}

void
ags_recall_audio_run_packable_interface_init(AgsPackableInterface *packable)
{
  ags_recall_audio_run_parent_packable_interface = g_type_interface_peek_parent(packable);

  packable->pack = ags_recall_audio_run_pack;
  packable->unpack = ags_recall_audio_run_unpack;
}

void
ags_recall_audio_run_dynamic_connectable_interface_init(AgsDynamicConnectableInterface *dynamic_connectable)
{
  ags_recall_audio_run_parent_dynamic_connectable_interface = g_type_interface_peek_parent(dynamic_connectable);

  dynamic_connectable->connect_dynamic = ags_recall_audio_run_connect_dynamic;
  dynamic_connectable->disconnect_dynamic = ags_recall_audio_run_disconnect_dynamic;
}

void
ags_recall_audio_run_init(AgsRecallAudioRun *recall_audio_run)
{
  recall_audio_run->recall_audio = NULL;
}


void
ags_recall_audio_run_set_property(GObject *gobject,
				  guint prop_id,
				  const GValue *value,
				  GParamSpec *param_spec)
{
  AgsRecallAudioRun *recall_audio_run;

  recall_audio_run = AGS_RECALL_AUDIO_RUN(gobject);

  switch(prop_id){
  case PROP_RECALL_AUDIO:
    {
      AgsRecallAudio *recall_audio;

      recall_audio = (AgsRecallAudio *) g_value_get_object(value);

      if(recall_audio_run->recall_audio == recall_audio)
	return;

      if(recall_audio_run->recall_audio != NULL){
	g_object_unref(G_OBJECT(recall_audio_run->recall_audio));
      }

      if(recall_audio != NULL){
	g_object_ref(G_OBJECT(recall_audio));
      }

      recall_audio_run->recall_audio = recall_audio;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  };
}

void
ags_recall_audio_run_get_property(GObject *gobject,
				  guint prop_id,
				  GValue *value,
				  GParamSpec *param_spec)
{
  AgsRecallAudioRun *recall_audio_run;

  recall_audio_run = AGS_RECALL_AUDIO_RUN(gobject);

  switch(prop_id){
  case PROP_RECALL_AUDIO:
    {
      g_value_set_object(value, recall_audio_run->recall_audio);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  };
}

void
ags_recall_audio_run_finalize(GObject *gobject)
{
  AgsRecallAudioRun *recall_audio_run;

  recall_audio_run = AGS_RECALL_AUDIO_RUN(gobject);

  if(recall_audio_run->recall_audio != NULL)
    g_object_unref(G_OBJECT(recall_audio_run->recall_audio));

  G_OBJECT_CLASS(ags_recall_audio_run_parent_class)->finalize(gobject);
}

void
ags_recall_audio_run_connect(AgsConnectable *connectable)
{
  ags_recall_audio_run_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_recall_audio_run_disconnect(AgsConnectable *connectable)
{
  ags_recall_audio_run_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_recall_audio_run_connect_dynamic(AgsDynamicConnectable *dynamic_connectable)
{
  ags_recall_audio_run_parent_dynamic_connectable_interface->connect_dynamic(dynamic_connectable);

  /* empty */
}

void
ags_recall_audio_run_disconnect_dynamic(AgsDynamicConnectable *dynamic_connectable)
{
  ags_recall_audio_run_parent_dynamic_connectable_interface->disconnect_dynamic(dynamic_connectable);

  /* empty */
}

gboolean
ags_recall_audio_run_pack(AgsPackable *packable, GObject *container)
{
  AgsRecallAudioRun *recall_audio_run;
  AgsRecallContainer *recall_container;
  GList *list;
  AgsRecallID *recall_id;

  if(ags_recall_audio_run_parent_packable_interface->pack(packable, container))
    return(TRUE);

  recall_audio_run = AGS_RECALL_AUDIO_RUN(packable);
  recall_container = AGS_RECALL_CONTAINER(container);

  /* set AgsRecallAudio */
  g_object_set(G_OBJECT(recall_audio_run),
	       "recall-audio\0", recall_container->recall_audio,
	       NULL);

  /* set in AgsRecallChannelRun */
  list = recall_container->recall_channel_run;

  if(AGS_RECALL(packable)->recall_id != NULL){
    recall_id = AGS_RECALL(packable)->recall_id;

    while((list = ags_recall_find_recycling_container(list, recall_id->recycling_container)) != NULL){
      g_object_set(G_OBJECT(list->data),
		   "recall-audio-run\0", AGS_RECALL_AUDIO_RUN(packable),
		   NULL);

      list= list->next;
    }
  }else if((AGS_RECALL_TEMPLATE & (AGS_RECALL(packable)->flags)) != 0){
    while((list = ags_recall_find_template(list)) != NULL){
      g_object_set(G_OBJECT(list->data),
		   "recall-audio-run\0", AGS_RECALL_AUDIO_RUN(packable),
		   NULL);

      list= list->next;
    }
  }

  g_object_set(G_OBJECT(recall_container),
	       "recall-audio-run\0", AGS_RECALL(packable),
	       NULL);

  return(FALSE);
}

gboolean
ags_recall_audio_run_unpack(AgsPackable *packable)
{
  AgsRecallAudioRun *recall_audio_run;
  AgsRecall *recall;
  AgsRecallContainer *recall_container;
  GList *list;
  AgsRecallID *recall_id;

  recall = AGS_RECALL(packable);

  if(recall == NULL)
    return(TRUE);

  recall_container = AGS_RECALL_CONTAINER(recall->container);
 
  if(recall_container == NULL)
    return(TRUE); 
  
  /* ref */
  g_object_ref(recall);
  g_object_ref(recall_container);
  
  recall_audio_run = AGS_RECALL_AUDIO_RUN(packable);

  /* unset AgsRecallAudio */
  g_object_set(G_OBJECT(recall_audio_run),
	       "recall_audio\0", NULL,
	       NULL);

  /* unset in AgsRecallChannelRun */
  list = recall_container->recall_channel_run;

  if(AGS_RECALL(packable)->recall_id != NULL){
    recall_id = AGS_RECALL(packable)->recall_id;

    while((list = ags_recall_find_recycling_container(list, recall_id->recycling_container)) != NULL){
      g_object_set(G_OBJECT(list->data),
		   "recall_audio_run\0", NULL,
		   NULL);

      list= list->next;
    }
  }else if((AGS_RECALL_TEMPLATE & (AGS_RECALL(packable)->flags)) != 0){
    while((list = ags_recall_find_template(list)) != NULL){
      g_object_set(G_OBJECT(list->data),
		   "recall_audio_run\0", NULL,
		   NULL);

      list= list->next;
    }
  }

  /* call parent */
  if(ags_recall_audio_run_parent_packable_interface->unpack(packable)){
    g_object_unref(recall);
    g_object_unref(recall_container);

    return(TRUE);
  }

  /* remove from list */
  recall_container->recall_audio_run = g_list_remove(recall_container->recall_audio_run,
						     recall);

  /* unref */
  g_object_unref(recall);
  g_object_unref(recall_container);

  return(FALSE);
}

AgsRecall*
ags_recall_audio_run_duplicate(AgsRecall *recall,
			       AgsRecallID *recall_id,
			       guint *n_params, GParameter *parameter)
{
  AgsRecallAudioRun *recall_audio_run, *copy;

  recall_audio_run = AGS_RECALL_AUDIO_RUN(recall);

  copy = AGS_RECALL_AUDIO_RUN(AGS_RECALL_CLASS(ags_recall_audio_run_parent_class)->duplicate(recall,
											     recall_id,
											     n_params, parameter));

  g_object_set(G_OBJECT(copy),
	       "recall_audio\0", recall_audio_run->recall_audio,
	       NULL);

  return((AgsRecall *) copy);
}

/**
 * ags_recall_audio_run_new:
 *
 * Creates an #AgsRecallAudioRun.
 *
 * Returns: a new #AgsRecallAudioRun.
 */
AgsRecallAudioRun*
ags_recall_audio_run_new()
{
  AgsRecallAudioRun *recall_audio_run;

  recall_audio_run = (AgsRecallAudioRun *) g_object_new(AGS_TYPE_RECALL_AUDIO_RUN,
							NULL);

  return(recall_audio_run);
}
