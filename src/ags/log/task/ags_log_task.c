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

#include <ags/log/task/ags_log_task.h>

#include <ags/object/ags_connectable.h>

void ags_log_task_class_init(AgsLogTaskClass *log_task);
void ags_log_task_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_log_task_init(AgsLogTask *log_task);
void ags_log_task_connect(AgsConnectable *connectable);
void ags_log_task_disconnect(AgsConnectable *connectable);
void ags_log_task_finalize(GObject *gobject);

void ags_log_task_launch(AgsTask *task);

/**
 * SECTION:ags_log_task
 * @short_description: add audio object to soundcard
 * @title: AgsLogTask
 * @section_id:
 * @include: ags/audio/task/ags_log_task.h
 *
 * The #AgsLogTask task adds #AgsAudio to #AgsSoundcard.
 */

static gpointer ags_log_task_parent_class = NULL;
static AgsConnectableInterface *ags_log_task_parent_connectable_interface;

GType
ags_log_task_get_type()
{
  static GType ags_type_log_task = 0;

  if(!ags_type_log_task){
    static const GTypeInfo ags_log_task_info = {
      sizeof (AgsLogTaskClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_log_task_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsLogTask),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_log_task_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_log_task_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    ags_type_log_task = g_type_register_static(AGS_TYPE_TASK,
						"AgsLogTask\0",
						&ags_log_task_info,
						0);
    
    g_type_add_interface_static(ags_type_log_task,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_log_task);
}

void
ags_log_task_class_init(AgsLogTaskClass *log_task)
{
  GObjectClass *gobject;
  AgsTaskClass *task;

  ags_log_task_parent_class = g_type_class_peek_parent(log_task);

  /* gobject */
  gobject = (GObjectClass *) log_task;

  gobject->finalize = ags_log_task_finalize;

  /* task */
  task = (AgsTaskClass *) log_task;

  task->launch = ags_log_task_launch;
}

void
ags_log_task_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_log_task_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_log_task_connect;
  connectable->disconnect = ags_log_task_disconnect;
}

void
ags_log_task_init(AgsLogTask *log_task)
{
  log_task->log = NULL;
  log_task->log_message = NULL;
}

void
ags_log_task_connect(AgsConnectable *connectable)
{
  ags_log_task_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_log_task_disconnect(AgsConnectable *connectable)
{
  ags_log_task_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_log_task_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_log_task_parent_class)->finalize(gobject);

  /* empty */
}

void
ags_log_task_launch(AgsTask *task)
{
  AgsLogTask *log_task;

  log_task = AGS_LOG_TASK(task);

  //TODO:JK: implement me
}

/**
 * ags_log_task_new:
 * @log: the #AgsLog
 * @log_message: the #AgsLogMessage to queue
 *
 * Creates an #AgsLogTask.
 *
 * Returns: an new #AgsLogTask.
 *
 * Since: 0.4.3
 */
AgsLogTask*
ags_log_task_new(AgsLog *log,
		  AgsLogMessage *log_message)
{
  AgsLogTask *log_task;

  log_task = (AgsLogTask *) g_object_new(AGS_TYPE_LOG_TASK,
					 "log\0", log,
					 "log-message\0", log_message,
					 NULL);

  return(log_task);
}
