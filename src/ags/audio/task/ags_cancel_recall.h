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

#ifndef __AGS_CANCEL_RECALL_H__
#define __AGS_CANCEL_RECALL_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/thread/ags_task.h>

#include <ags/audio/ags_recall.h>

#define AGS_TYPE_CANCEL_RECALL                (ags_cancel_recall_get_type())
#define AGS_CANCEL_RECALL(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_CANCEL_RECALL, AgsCancelRecall))
#define AGS_CANCEL_RECALL_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_CANCEL_RECALL, AgsCancelRecallClass))
#define AGS_IS_CANCEL_RECALL(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_CANCEL_RECALL))
#define AGS_IS_CANCEL_RECALL_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_CANCEL_RECALL))
#define AGS_CANCEL_RECALL_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_CANCEL_RECALL, AgsCancelRecallClass))

typedef struct _AgsCancelRecall AgsCancelRecall;
typedef struct _AgsCancelRecallClass AgsCancelRecallClass;

struct _AgsCancelRecall
{
  AgsTask task;

  AgsRecall *recall;

  AgsDevoutPlay *play;
};

struct _AgsCancelRecallClass
{
  AgsTaskClass task;
};

GType ags_cancel_recall_get_type();

AgsCancelRecall* ags_cancel_recall_new(AgsRecall *recall,
				       AgsDevoutPlay *play);

#endif /*__AGS_CANCEL_RECALL_H__*/
