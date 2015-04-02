/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2015 Joël Krähemann
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

#include <ags/audio/task/ags_add_recall_container.h>

#include <ags-lib/object/ags_connectable.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_channel.h>

#include <math.h>

void ags_add_recall_container_class_init(AgsAddRecallContainerClass *add_recall_container);
void ags_add_recall_container_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_add_recall_container_init(AgsAddRecallContainer *add_recall_container);
void ags_add_recall_container_connect(AgsConnectable *connectable);
void ags_add_recall_container_disconnect(AgsConnectable *connectable);
void ags_add_recall_container_finalize(GObject *gobject);

void ags_add_recall_container_launch(AgsTask *task);

/**
 * SECTION:ags_add_recall_container
 * @short_description: add recall object to context
 * @title: AgsAddRecallContainer
 * @section_id:
 * @include: ags/audio/task/ags_add_recall_container.h
 *
 * The #AgsAddRecallContainer task adds #AgsRecallContainer of audio.
 */

static gpointer ags_add_recall_container_parent_class = NULL;
static AgsConnectableInterface *ags_add_recall_container_parent_connectable_interface;

GType
ags_add_recall_container_get_type()
{
  static GType ags_type_add_recall_container = 0;

  if(!ags_type_add_recall_container){
    static const GTypeInfo ags_add_recall_container_info = {
      sizeof (AgsAddRecallContainerClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_add_recall_container_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsAddRecallContainer),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_add_recall_container_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_add_recall_container_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_add_recall_container = g_type_register_static(AGS_TYPE_TASK,
							   "AgsAddRecallContainer\0",
							   &ags_add_recall_container_info,
							   0);

    g_type_add_interface_static(ags_type_add_recall_container,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_add_recall_container);
}

void
ags_add_recall_container_class_init(AgsAddRecallContainerClass *add_recall_container)
{
  GObjectClass *gobject;
  AgsTaskClass *task;

  ags_add_recall_container_parent_class = g_type_class_peek_parent(add_recall_container);

  /* GObjectClass */
  gobject = (GObjectClass *) add_recall_container;

  gobject->finalize = ags_add_recall_container_finalize;

  /* AgsTaskClass */
  task = (AgsTaskClass *) add_recall_container;

  task->launch = ags_add_recall_container_launch;
}

void
ags_add_recall_container_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_add_recall_container_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_add_recall_container_connect;
  connectable->disconnect = ags_add_recall_container_disconnect;
}

void
ags_add_recall_container_init(AgsAddRecallContainer *add_recall_container)
{
  add_recall_container->audio = NULL;
  add_recall_container->recall_container = NULL;
}

void
ags_add_recall_container_connect(AgsConnectable *connectable)
{
  ags_add_recall_container_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_add_recall_container_disconnect(AgsConnectable *connectable)
{
  ags_add_recall_container_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_add_recall_container_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_add_recall_container_parent_class)->finalize(gobject);

  /* empty */
}

void
ags_add_recall_container_launch(AgsTask *task)
{
  AgsAddRecallContainer *add_recall_container;

  add_recall_container = AGS_ADD_RECALL_CONTAINER(task);

  ags_audio_add_recall_container(add_recall_container->audio,
				 add_recall_container->recall_container);
}

/**
 * ags_add_recall_container_new:
 * @context: the #AgsAudio
 * @recall: the #AgsRecallContainer to add
 *
 * Creates an #AgsAddRecallContainer.
 *
 * Returns: an new #AgsAddRecallContainer.
 *
 * Since: 0.4
 */
AgsAddRecallContainer*
ags_add_recall_container_new(GObject *audio,
			     AgsRecallContainer *recall_container)
{
  AgsAddRecallContainer *add_recall_container;

  add_recall_container = (AgsAddRecallContainer *) g_object_new(AGS_TYPE_ADD_RECALL_CONTAINER,
								NULL);  
  
  add_recall_container->audio = audio;
  add_recall_container->recall_container = recall_container;

  return(add_recall_container);
}
