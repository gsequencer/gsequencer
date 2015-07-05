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

#ifndef __AGS_REMOVE_RECALL_CONTAINER_H__
#define __AGS_REMOVE_RECALL_CONTAINER_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/audio/ags_task.h>

#include <ags/audio/ags_recall_container.h>

#define AGS_TYPE_REMOVE_RECALL_CONTAINER                (ags_remove_recall_container_get_type())
#define AGS_REMOVE_RECALL_CONTAINER(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_REMOVE_RECALL_CONTAINER, AgsRemoveRecallContainer))
#define AGS_REMOVE_RECALL_CONTAINER_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_REMOVE_RECALL_CONTAINER, AgsRemoveRecallContainerClass))
#define AGS_IS_REMOVE_RECALL_CONTAINER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_REMOVE_RECALL_CONTAINER))
#define AGS_IS_REMOVE_RECALL_CONTAINER_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_REMOVE_RECALL_CONTAINER))
#define AGS_REMOVE_RECALL_CONTAINER_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_REMOVE_RECALL_CONTAINER, AgsRemoveRecallContainerClass))

typedef struct _AgsRemoveRecallContainer AgsRemoveRecallContainer;
typedef struct _AgsRemoveRecallContainerClass AgsRemoveRecallContainerClass;

struct _AgsRemoveRecallContainer
{
  AgsTask task;

  GObject *audio;

  AgsRecallContainer *recall_container;
};

struct _AgsRemoveRecallContainerClass
{
  AgsTaskClass task;
};

GType ags_remove_recall_container_get_type();

AgsRemoveRecallContainer* ags_remove_recall_container_new(GObject *audio,
							  AgsRecallContainer *recall_container);

#endif /*__AGS_REMOVE_RECALL_CONTAINER_H__*/
