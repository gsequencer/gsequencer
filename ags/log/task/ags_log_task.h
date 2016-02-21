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

#ifndef __AGS_LOG_TASK_H__
#define __AGS_LOG_TASK_H__

#include <glib-object.h>

#include <ags/thread/ags_task.h>
#include <ags/log/ags_log.h>

#define AGS_TYPE_LOG_TASK                (ags_log_task_get_type())
#define AGS_LOG_TASK(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_LOG_TASK, AgsLogTask))
#define AGS_LOG_TASK_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_LOG_TASK, AgsLogTaskClass))
#define AGS_IS_LOG_TASK(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_LOG_TASK))
#define AGS_IS_LOG_TASK_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_LOG_TASK))
#define AGS_LOG_TASK_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_LOG_TASK, AgsLogTaskClass))

typedef struct _AgsLogTask AgsLogTask;
typedef struct _AgsLogTaskClass AgsLogTaskClass;

struct _AgsLogTask
{
  AgsTask task;

  AgsLog *log;
  AgsLogMessage *log_message;
};

struct _AgsLogTaskClass
{
  AgsTaskClass task;
};

GType ags_log_task_get_type();

AgsLogTask* ags_log_task_new(AgsLog *log,
			     AgsLogMessage *log_message);

#endif /*__AGS_LOG_TASK_H__*/
