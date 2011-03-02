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

#include <ags/object/ags_connectable.h>
#include <ags/object/ags_packable.h>
#include <ags/object/ags_run_connectable.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_recall_audio.h>
#include <ags/audio/ags_recall_container.h>

void ags_recall_audio_run_class_init(AgsRecallAudioRunClass *recall_audio_run);
void ags_recall_audio_runconnectable_interface_init(AgsConnectableInterface *connectable);
void ags_recall_audio_run_packable_interface_init(AgsPackableInterface *packable);
void ags_recall_audio_run_run_connectable_interface_init(AgsRunConnectableInterface *run_connectable);
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
gboolean ags_recall_audio_run_pack(AgsPackable *packable, GObject *container);
gboolean ags_recall_audio_run_unpack(AgsPackable *packable);
void ags_recall_audio_run_disconnect(AgsConnectable *connectable);
void ags_recall_audio_run_run_connect(AgsRunConnectable *run_connectable);
void ags_recall_audio_run_run_disconnect(AgsRunConnectable *run_connectable);
void ags_recall_audio_run_finalize(GObject *gobject);

AgsRecall* ags_recall_audio_run_duplicate(AgsRecall *recall,
					  AgsRecallID *recall_id);

enum{
  PROP_0,
  PROP_RECALL_AUDIO,
};

static gpointer ags_recall_audio_run_parent_class = NULL;
static AgsConnectableInterface* ags_recall_audio_run_parent_connectable_interface;
static AgsPackableInterface* ags_recall_audio_run_parent_packable_interface;
static AgsRunConnectableInterface *ags_recall_audio_run_parent_run_connectable_interface;

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
      (GInterfaceInitFunc) ags_recall_audio_runconnectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_packable_interface_info = {
      (GInterfaceInitFunc) ags_recall_audio_run_packable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_run_connectable_interface_info = {
      (GInterfaceInitFunc) ags_recall_audio_run_run_connectable_interface_init,
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
				AGS_TYPE_RUN_CONNECTABLE,
				&ags_run_connectable_interface_info);
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
  param_spec = g_param_spec_object("recall_audio\0",
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
ags_recall_audio_run_run_connectable_interface_init(AgsRunConnectableInterface *run_connectable)
{
  ags_recall_audio_run_parent_run_connectable_interface = g_type_interface_peek_parent(run_connectable);

  run_connectable->connect = ags_recall_audio_run_run_connect;
  run_connectable->disconnect = ags_recall_audio_run_run_disconnect;
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

gboolean
ags_recall_audio_run_pack(AgsPackable *packable, GObject *container)
{
  AgsRecallContainer *recall_container;

  if(ags_recall_audio_run_parent_packable_interface->pack(packable, container))
    return(TRUE);

  recall_container = AGS_RECALL_CONTAINER(container);

  recall_container->recall_audio_run = g_list_prepend(recall_container->recall_audio_run,
						      AGS_RECALL(packable));

  return(FALSE);
}

gboolean
ags_recall_audio_run_unpack(AgsPackable *packable)
{
  AgsRecall *recall;
  AgsRecallContainer *recall_container;

  recall = AGS_RECALL(packable);

  if(recall == NULL)
    return(TRUE);
  
  recall_container = AGS_RECALL_CONTAINER(recall->container);
 
  if(recall_container == NULL)
    return(TRUE); 
  
  /* ref */
  g_object_ref(recall);
  g_object_ref(recall_container);

  /* call parent */
  if(ags_recall_audio_run_parent_packable_interface->unpack(packable))
    return(TRUE);

  /* remove from list */
  recall_container->recall_audio_run = g_list_remove(recall_container->recall_audio_run,
						     recall);

  /* unref */
  g_object_unref(recall);
  g_object_unref(recall_container);

  return(FALSE);
}

void
ags_recall_audio_run_disconnect(AgsConnectable *connectable)
{
  ags_recall_audio_run_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_recall_audio_run_run_connect(AgsConnectable *connectable)
{
  ags_recall_audio_run_parent_run_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_recall_audio_run_run_disconnect(AgsConnectable *connectable)
{
  ags_recall_audio_run_parent_run_connectable_interface->disconnect(connectable);

  /* empty */
}

AgsRecall*
ags_recall_audio_run_duplicate(AgsRecall *recall,
			       AgsRecallID *recall_id)
{
  AgsRecallAudioRun *recall_audio_run, *copy;
  GValue recall_audio_value = {0,};

  recall_audio_run = AGS_RECALL_AUDIO_RUN(recall);
  copy = AGS_RECALL_AUDIO_RUN(AGS_RECALL_CLASS(ags_recall_audio_run_parent_class)->duplicate(recall,
											     recall_id));

  g_value_init(&recall_audio_value, G_TYPE_OBJECT);
  g_value_set_object(&recall_audio_value,
		     G_OBJECT(recall_audio_run->recall_audio));
  g_object_set_property(G_OBJECT(copy),
			"recall_audio\0",
			&recall_audio_value);
  g_value_unset(&recall_audio_value);

  return((AgsRecall *) copy);
}

AgsRecallAudioRun*
ags_recall_audio_run_new()
{
  AgsRecallAudioRun *recall_audio_run;

  recall_audio_run = (AgsRecallAudioRun *) g_object_new(AGS_TYPE_RECALL_AUDIO_RUN,
							NULL);

  return(recall_audio_run);
}
