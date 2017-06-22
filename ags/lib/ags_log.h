/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2016 Joël Krähemann
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

#ifndef __AGS_LOG_H__
#define __AGS_LOG_H__

#include <glib.h>
#include <glib-object.h>

#define AGS_TYPE_LOG                (ags_log_get_type())
#define AGS_LOG(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_LOG, AgsLog))
#define AGS_LOG_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_LOG, AgsLogClass))
#define AGS_IS_LOG(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_LOG))
#define AGS_IS_LOG_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_LOG))
#define AGS_LOG_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS (obj, AGS_TYPE_LOG, AgsLogClass))

typedef struct _AgsLog AgsLog;
typedef struct _AgsLogClass AgsLogClass;

struct _AgsLog
{
  GObject gobject;

  guint flags;

  pthread_mutex_t *mutex;
  
  volatile GList *messages;
};

struct _AgsLogClass
{
  GObjectClass gobject;
};

GType ags_log_get_type(void);

void ags_log_add_message(AgsLog *log,
			 gchar *str);
GList* ags_log_get_messages(AgsLog *log);

AgsLog* ags_log_get_instance();

AgsLog* ags_log_new();

#endif /*__AGS_LOG_H__*/
