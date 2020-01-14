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

#ifndef __AGS_RECALL_CONTAINER_H__
#define __AGS_RECALL_CONTAINER_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <ags/audio/ags_recall.h>

G_BEGIN_DECLS

#define AGS_TYPE_RECALL_CONTAINER                (ags_recall_container_get_type())
#define AGS_RECALL_CONTAINER(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_RECALL_CONTAINER, AgsRecallContainer))
#define AGS_RECALL_CONTAINER_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_RECALL_CONTAINER, AgsRecallContainerClass))
#define AGS_IS_RECALL_CONTAINER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_RECALL_CONTAINER))
#define AGS_IS_RECALL_CONTAINER_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_RECALL_CONTAINER))
#define AGS_RECALL_CONTAINER_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_RECALL_CONTAINER, AgsRecallContainerClass))

#define AGS_RECALL_CONTAINER_GET_OBJ_MUTEX(obj) (&(((AgsRecallContainer *) obj)->obj_mutex))

typedef struct _AgsRecallContainer AgsRecallContainer;
typedef struct _AgsRecallContainerClass AgsRecallContainerClass;

/**
 * AgsRecallContainerFlags:
 * @AGS_RECALL_CONTAINER_ADDED_TO_REGISTRY: add to registry
 * @AGS_RECALL_CONTAINER_CONNECTED: indicates the recall container was connected by calling #AgsConnectable::connect()
 * @AGS_RECALL_CONTAINER_PLAY: bound to play context
 * 
 * Enum values to control the behavior or indicate internal state of #AgsRecallContainer by
 * enable/disable as flags.
 */
typedef enum{
  AGS_RECALL_CONTAINER_ADDED_TO_REGISTRY   = 1,
  AGS_RECALL_CONTAINER_CONNECTED           = 1 <<  1,
  AGS_RECALL_CONTAINER_PLAY                = 1 <<  2,
}AgsRecallContainerFlags;

/**
 * AgsRecallContainerFindFlags:
 * @AGS_RECALL_CONTAINER_FIND_TYPE: find type
 * @AGS_RECALL_CONTAINER_FIND_TEMPLATE: find template
 * @AGS_RECALL_CONTAINER_FIND_RECALL_ID: find recall id
 * 
 * Enum values to specify find criteria.
 */
typedef enum{
  AGS_RECALL_CONTAINER_FIND_TYPE          = 1,
  AGS_RECALL_CONTAINER_FIND_TEMPLATE      = 1 <<  1,
  AGS_RECALL_CONTAINER_FIND_RECALL_ID     = 1 <<  2,
}AgsRecallContainerFindFlags;

struct _AgsRecallContainer
{
  GObject gobject;
  
  guint flags;

  GRecMutex obj_mutex;

  AgsUUID *uuid;

  GType recall_audio_type;
  AgsRecall *recall_audio;

  GType recall_audio_run_type;
  GList *recall_audio_run;

  GType recall_channel_type;
  GList *recall_channel;

  GType recall_channel_run_type;
  GList *recall_channel_run;
};

struct _AgsRecallContainerClass
{
  GObjectClass gobject;
};

GType ags_recall_container_get_type();

gboolean ags_recall_container_test_flags(AgsRecallContainer *recall_container, guint flags);
void ags_recall_container_set_flags(AgsRecallContainer *recall_container, guint flags);
void ags_recall_container_unset_flags(AgsRecallContainer *recall_container, guint flags);

/* children */
void ags_recall_container_add(AgsRecallContainer *recall_container,
			      AgsRecall *recall);
void ags_recall_container_remove(AgsRecallContainer *recall_container,
				 AgsRecall *recall);

/* query */
AgsRecall* ags_recall_container_get_recall_audio(AgsRecallContainer *recall_container);
GList* ags_recall_container_get_recall_audio_run(AgsRecallContainer *recall_container);
GList* ags_recall_container_get_recall_channel(AgsRecallContainer *recall_container);
GList* ags_recall_container_get_recall_channel_run(AgsRecallContainer *recall_container);

GList* ags_recall_container_find(GList *recall_container,
				 GType type,
				 guint find_flags,
				 AgsRecallID *recall_id);

/* instantiate */
AgsRecallContainer* ags_recall_container_new();

G_END_DECLS

#endif /*__AGS_RECALL_CONTAINER_H__*/
