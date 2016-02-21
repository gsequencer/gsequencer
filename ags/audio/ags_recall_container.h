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

#ifndef __AGS_RECALL_CONTAINER_H__
#define __AGS_RECALL_CONTAINER_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/audio/ags_recall.h>

#define AGS_TYPE_RECALL_CONTAINER                (ags_recall_container_get_type())
#define AGS_RECALL_CONTAINER(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_RECALL_CONTAINER, AgsRecallContainer))
#define AGS_RECALL_CONTAINER_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_RECALL_CONTAINER, AgsRecallContainerClass))
#define AGS_IS_RECALL_CONTAINER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_RECALL_CONTAINER))
#define AGS_IS_RECALL_CONTAINER_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_RECALL_CONTAINER))
#define AGS_RECALL_CONTAINER_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_RECALL_CONTAINER, AgsRecallContainerClass))

typedef struct _AgsRecallContainer AgsRecallContainer;
typedef struct _AgsRecallContainerClass AgsRecallContainerClass;

typedef enum{
  AGS_RECALL_CONTAINER_PLAY    =  1,
}AgsRecallContainerFlags;

typedef enum{
  AGS_RECALL_CONTAINER_FIND_TYPE,
  AGS_RECALL_CONTAINER_FIND_TEMPLATE,
  AGS_RECALL_CONTAINER_FIND_RECALL_ID,
}AgsRecallContainerFindFlags;

struct _AgsRecallContainer
{
  GObject object;
  
  guint flags;

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
  GObjectClass object;
};

GType ags_recall_container_get_type();

AgsRecall* ags_recall_container_get_recall_audio(AgsRecallContainer *container);
GList* ags_recall_container_get_recall_audio_run(AgsRecallContainer *container);
GList* ags_recall_container_get_recall_channel(AgsRecallContainer *container);
GList* ags_recall_container_get_recall_channel_run(AgsRecallContainer *container);

GList* ags_recall_container_find(GList *recall_container,
				 GType type,
				 guint find_flags,
				 AgsRecallID *recall_id);

AgsRecallContainer* ags_recall_container_new();

#endif /*__AGS_RECALL_CONTAINER_H__*/
