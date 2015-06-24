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

#include <ags/audio/task/ags_remove_recall_container.h>

#include <ags-lib/object/ags_connectable.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_channel.h>

#include <math.h>

void ags_remove_recall_container_class_init(AgsRemoveRecallContainerClass *remove_recall_container);
void ags_remove_recall_container_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_remove_recall_container_init(AgsRemoveRecallContainer *remove_recall_container);
void ags_remove_recall_container_connect(AgsConnectable *connectable);
void ags_remove_recall_container_disconnect(AgsConnectable *connectable);
void ags_remove_recall_container_finalize(GObject *gobject);

void ags_remove_recall_container_launch(AgsTask *task);

/**
 * SECTION:ags_remove_recall_container
 * @short_description: remove recall object to context
 * @title: AgsRemoveRecallContainer
 * @section_id:
 * @include: ags/audio/task/ags_remove_recall_container.h
 *
 * The #AgsRemoveRecallContainer task removes #AgsRecallContainer of audio.
 */

static gpointer ags_remove_recall_container_parent_class = NULL;
static AgsConnectableInterface *ags_remove_recall_container_parent_connectable_interface;

GType
ags_remove_recall_container_get_type()
{
  static GType ags_type_remove_recall_container = 0;

  if(!ags_type_remove_recall_container){
    static const GTypeInfo ags_remove_recall_container_info = {
      sizeof (AgsRemoveRecallContainerClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_remove_recall_container_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsRemoveRecallContainer),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_remove_recall_container_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_remove_recall_container_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_remove_recall_container = g_type_register_static(AGS_TYPE_TASK,
							      "AgsRemoveRecallContainer\0",
							      &ags_remove_recall_container_info,
							      0);

    g_type_add_interface_static(ags_type_remove_recall_container,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_remove_recall_container);
}

void
ags_remove_recall_container_class_init(AgsRemoveRecallContainerClass *remove_recall_container)
{
  GObjectClass *gobject;
  AgsTaskClass *task;

  ags_remove_recall_container_parent_class = g_type_class_peek_parent(remove_recall_container);

  /* GObjectClass */
  gobject = (GObjectClass *) remove_recall_container;

  gobject->finalize = ags_remove_recall_container_finalize;

  /* AgsTaskClass */
  task = (AgsTaskClass *) remove_recall_container;

  task->launch = ags_remove_recall_container_launch;
}

void
ags_remove_recall_container_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_remove_recall_container_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_remove_recall_container_connect;
  connectable->disconnect = ags_remove_recall_container_disconnect;
}

void
ags_remove_recall_container_init(AgsRemoveRecallContainer *remove_recall_container)
{
  remove_recall_container->audio = NULL;
  remove_recall_container->recall_container = NULL;
}

void
ags_remove_recall_container_connect(AgsConnectable *connectable)
{
  ags_remove_recall_container_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_remove_recall_container_disconnect(AgsConnectable *connectable)
{
  ags_remove_recall_container_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_remove_recall_container_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_remove_recall_container_parent_class)->finalize(gobject);

  /* empty */
}

void
ags_remove_recall_container_launch(AgsTask *task)
{
  AgsRemoveRecallContainer *remove_recall_container;

  remove_recall_container = AGS_REMOVE_RECALL_CONTAINER(task);

  ags_audio_remove_recall_container((AgsAudio *) remove_recall_container->audio,
				    (GObject *) remove_recall_container->recall_container);
}

/**
 * ags_remove_recall_container_new:
 * @context: the #AgsAudio
 * @recall: the #AgsRecallContainer to remove
 *
 * Creates an #AgsRemoveRecallContainer.
 *
 * Returns: an new #AgsRemoveRecallContainer.
 *
 * Since: 0.4
 */
AgsRemoveRecallContainer*
ags_remove_recall_container_new(GObject *audio,
				AgsRecallContainer *recall_container)
{
  AgsRemoveRecallContainer *remove_recall_container;

  remove_recall_container = (AgsRemoveRecallContainer *) g_object_new(AGS_TYPE_REMOVE_RECALL_CONTAINER,
								      NULL);  
  
  remove_recall_container->audio = audio;
  remove_recall_container->recall_container = recall_container;

  return(remove_recall_container);
}
