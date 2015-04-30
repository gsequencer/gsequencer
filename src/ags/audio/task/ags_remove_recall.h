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

#ifndef __AGS_REMOVE_RECALL_H__
#define __AGS_REMOVE_RECALL_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/thread/ags_task.h>

#include <ags/audio/ags_recall.h>

#define AGS_TYPE_REMOVE_RECALL                (ags_remove_recall_get_type())
#define AGS_REMOVE_RECALL(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_REMOVE_RECALL, AgsRemoveRecall))
#define AGS_REMOVE_RECALL_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_REMOVE_RECALL, AgsRemoveRecallClass))
#define AGS_IS_REMOVE_RECALL(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_REMOVE_RECALL))
#define AGS_IS_REMOVE_RECALL_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_REMOVE_RECALL))
#define AGS_REMOVE_RECALL_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_REMOVE_RECALL, AgsRemoveRecallClass))

typedef struct _AgsRemoveRecall AgsRemoveRecall;
typedef struct _AgsRemoveRecallClass AgsRemoveRecallClass;

struct _AgsRemoveRecall
{
  AgsTask task;

  GObject *context;

  AgsRecall *recall;
  gboolean is_play;
  gboolean remove_all;
};

struct _AgsRemoveRecallClass
{
  AgsTaskClass task;
};

GType ags_remove_recall_get_type();

AgsRemoveRecall* ags_remove_recall_new(GObject *context,
				       AgsRecall *recall,
				       gboolean is_play,
				       gboolean remove_all);

#endif /*__AGS_REMOVE_RECALL_H__*/
