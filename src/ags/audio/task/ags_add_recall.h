/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2014 Joël Krähemann
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

#ifndef __AGS_ADD_RECALL_H__
#define __AGS_ADD_RECALL_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/thread/ags_task.h>
#include <ags/audio/ags_recall.h>

#define AGS_TYPE_ADD_RECALL                (ags_add_recall_get_type())
#define AGS_ADD_RECALL(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_ADD_RECALL, AgsAddRecall))
#define AGS_ADD_RECALL_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_ADD_RECALL, AgsAddRecallClass))
#define AGS_IS_ADD_RECALL(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_ADD_RECALL))
#define AGS_IS_ADD_RECALL_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_ADD_RECALL))
#define AGS_ADD_RECALL_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_ADD_RECALL, AgsAddRecallClass))

typedef struct _AgsAddRecall AgsAddRecall;
typedef struct _AgsAddRecallClass AgsAddRecallClass;

struct _AgsAddRecall
{
  AgsTask task;

  GObject *context;

  AgsRecall *recall;
  gboolean is_play;
};

struct _AgsAddRecallClass
{
  AgsTaskClass task;
};

GType ags_add_recall_get_type();

AgsAddRecall* ags_add_recall_new(GObject *context,
				 AgsRecall *recall,
				 gboolean is_play);

#endif /*__AGS_ADD_RECALL_H__*/
