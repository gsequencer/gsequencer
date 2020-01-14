/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
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

#include <ags/audio/ags_generic_recall_channel_run.h>
#include <ags/audio/ags_generic_recall_recycling.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_recycling.h>
#include <ags/audio/ags_recall_id.h>

#include <libxml/tree.h>

#include <ags/i18n.h>

void ags_generic_recall_channel_run_class_init(AgsGenericRecallChannelRunClass *generic_recall_channel_run);
void ags_generic_recall_channel_run_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_generic_recall_channel_run_init(AgsGenericRecallChannelRun *generic_recall_channel_run);
void ags_generic_recall_channel_run_set_property(GObject *gobject,
						 guint prop_id,
						 const GValue *value,
						 GParamSpec *param_spec);
void ags_generic_recall_channel_run_get_property(GObject *gobject,
						 guint prop_id,
						 GValue *value,
						 GParamSpec *param_spec);
void ags_generic_recall_channel_run_finalize(GObject *gobject);

void ags_generic_recall_channel_run_connect(AgsConnectable *connectable);
void ags_generic_recall_channel_run_disconnect(AgsConnectable *connectable);

AgsRecall* ags_generic_recall_channel_run_duplicate(AgsRecall *recall,
						    AgsRecallID *recall_id,
						    guint *n_params, gchar **parameter_name, GValue *value);

/**
 * SECTION:ags_generic_recall_channel_run
 * @short_description: generic channel context of recall
 * @title: AgsGenericRecallChannelRun
 * @section_id:
 * @include: ags/audio/ags_generic_recall_channel_run.h
 *
 * #AgsGenericRecallChannelRun acts as generic channel recall.
 */

static gpointer ags_generic_recall_channel_run_parent_class = NULL;
static AgsConnectableInterface *ags_generic_recall_channel_run_parent_connectable_interface;

enum{
  PROP_0,
  PROP_GENERIC_RECALL_RECYCLING_CHILD_TYPE,
};

GType
ags_generic_recall_channel_run_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_generic_recall_channel_run = 0;

    static const GTypeInfo ags_generic_recall_channel_run_info = {
      sizeof (AgsGenericRecallChannelRunClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_generic_recall_channel_run_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsGenericRecallChannelRun),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_generic_recall_channel_run_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_generic_recall_channel_run_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_generic_recall_channel_run = g_type_register_static(AGS_TYPE_RECALL_CHANNEL_RUN,
								 "AgsGenericRecallChannelRun",
								 &ags_generic_recall_channel_run_info,
								 0);

    g_type_add_interface_static(ags_type_generic_recall_channel_run,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_generic_recall_channel_run);
  }

  return g_define_type_id__volatile;
}

void
ags_generic_recall_channel_run_class_init(AgsGenericRecallChannelRunClass *generic_recall_channel_run)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;

  GParamSpec *param_spec;

  ags_generic_recall_channel_run_parent_class = g_type_class_peek_parent(generic_recall_channel_run);

  /* GObjectClass */
  gobject = (GObjectClass *) generic_recall_channel_run;

  gobject->set_property = ags_generic_recall_channel_run_set_property;
  gobject->get_property = ags_generic_recall_channel_run_get_property;

  gobject->finalize = ags_generic_recall_channel_run_finalize;

  /* properties */
  /**
   * AgsGenericRecallChannelRun:generic-recall-recycling-child-type:
   *
   * The type of child #AgsRecall.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_gtype("generic-recall-recycling-child-type",
				  i18n_pspec("generic recall recycling child type"),
				  i18n_pspec("The generic recall recycling type of child that will be added"),
				  AGS_TYPE_RECALL,
				  G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_GENERIC_RECALL_RECYCLING_CHILD_TYPE,
				  param_spec);
  
  /* AgsRecallClass */
  recall = (AgsRecallClass *) generic_recall_channel_run;

  recall->duplicate = ags_generic_recall_channel_run_duplicate;
}

void
ags_generic_recall_channel_run_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_generic_recall_channel_run_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_generic_recall_channel_run_connect;
  connectable->disconnect = ags_generic_recall_channel_run_disconnect;
}

void
ags_generic_recall_channel_run_init(AgsGenericRecallChannelRun *generic_recall_channel_run)
{
  AGS_RECALL(generic_recall_channel_run)->name = "ags-generic";
  AGS_RECALL(generic_recall_channel_run)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(generic_recall_channel_run)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(generic_recall_channel_run)->xml_type = "ags-generic-recall-channel-run";
  AGS_RECALL(generic_recall_channel_run)->port = NULL;

  AGS_RECALL(generic_recall_channel_run)->ability_flags |= (AGS_SOUND_ABILITY_PLAYBACK  |
							    AGS_SOUND_ABILITY_NOTATION  |
							    AGS_SOUND_ABILITY_SEQUENCER |
							    AGS_SOUND_ABILITY_WAVE      |
							    AGS_SOUND_ABILITY_MIDI);
  AGS_RECALL(generic_recall_channel_run)->child_type = AGS_TYPE_GENERIC_RECALL_RECYCLING;

  generic_recall_channel_run->generic_recall_recycling_child_type = G_TYPE_NONE;
}

void
ags_generic_recall_channel_run_set_property(GObject *gobject,
					    guint prop_id,
					    const GValue *value,
					    GParamSpec *param_spec)
{
  AgsGenericRecallChannelRun *generic_recall_channel_run;

  GRecMutex *recall_mutex;

  generic_recall_channel_run = AGS_GENERIC_RECALL_CHANNEL_RUN(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(generic_recall_channel_run);

  switch(prop_id){
  case PROP_GENERIC_RECALL_RECYCLING_CHILD_TYPE:
    {
      g_rec_mutex_lock(recall_mutex);

      generic_recall_channel_run->generic_recall_recycling_child_type = g_value_get_gtype(value);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_generic_recall_channel_run_get_property(GObject *gobject,
					    guint prop_id,
					    GValue *value,
					    GParamSpec *param_spec)
{
  AgsGenericRecallChannelRun *generic_recall_channel_run;

  GRecMutex *recall_mutex;

  generic_recall_channel_run = AGS_GENERIC_RECALL_CHANNEL_RUN(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(generic_recall_channel_run);

  switch(prop_id){
  case PROP_GENERIC_RECALL_RECYCLING_CHILD_TYPE:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_gtype(value,
			generic_recall_channel_run->generic_recall_recycling_child_type);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}
      
void
ags_generic_recall_channel_run_finalize(GObject *gobject)
{
  /* empty */

  /* call parent */
  G_OBJECT_CLASS(ags_generic_recall_channel_run_parent_class)->finalize(gobject);
}

void
ags_generic_recall_channel_run_connect(AgsConnectable *connectable)
{
  AgsRecallChannelRun *recall_channel_run;
  GObject *gobject;

  /* call parent */
  ags_generic_recall_channel_run_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_generic_recall_channel_run_disconnect(AgsConnectable *connectable)
{
  /* call parent */
  ags_generic_recall_channel_run_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

AgsRecall*
ags_generic_recall_channel_run_duplicate(AgsRecall *recall,
					 AgsRecallID *recall_id,
					 guint *n_params, gchar **parameter_name, GValue *value)
{
  AgsGenericRecallChannelRun *generic_recall_channel_run, *copy_generic_recall_channel_run;

  GList *list_start, *list;

  GType generic_recall_recycling_child_type;
  
  GRecMutex *recall_mutex;
  
  generic_recall_channel_run = (AgsGenericRecallChannelRun *) recall;  

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(generic_recall_channel_run);

  /* get some fields */
  g_rec_mutex_lock(recall_mutex);

  generic_recall_recycling_child_type = generic_recall_channel_run->generic_recall_recycling_child_type;

  g_rec_mutex_unlock(recall_mutex);

  /* duplicate */
  copy_generic_recall_channel_run = AGS_RECALL_CLASS(ags_generic_recall_channel_run_parent_class)->duplicate(recall,
													     recall_id,
													     n_params, parameter_name, value);
  
  g_object_set(copy_generic_recall_channel_run,
	       "generic-recall-recycling-child-type", generic_recall_recycling_child_type,
	       NULL);

  /* set child type on AgsRecallRecycling */
  g_object_get(copy_generic_recall_channel_run,
	       "child", &list_start,
	       NULL);

  list = list_start;
  
  while(list != NULL){
    g_object_set(list->data,
		 "child-type", generic_recall_recycling_child_type,
		 NULL);
    
    list = list->next;
  }

  g_list_free_full(list_start,
		   g_object_unref);
  
  return((AgsRecall *) copy_generic_recall_channel_run);
}

/**
 * ags_generic_recall_channel_run_new:
 * @source: the source #AgsChannel
 * @child_type: child type
 * @generic_recall_recycling_child_type: recycling child type
 *
 * Creates an #AgsGenericRecallChannelRun.
 *
 * Returns: a new #AgsGenericRecallChannelRun.
 *
 * Since: 3.0.0
 */
AgsGenericRecallChannelRun*
ags_generic_recall_channel_run_new(AgsChannel *source,
				   GType child_type,
				   GType generic_recall_recycling_child_type)
{
  AgsGenericRecallChannelRun *generic_recall_channel_run;

  generic_recall_channel_run = (AgsGenericRecallChannelRun *) g_object_new(AGS_TYPE_GENERIC_RECALL_CHANNEL_RUN,
									   "source", source,
									   "child-type", child_type,
									   "generic-recall-recycling-child-type", generic_recall_recycling_child_type,
									   NULL);
  
  return(generic_recall_channel_run);
}
