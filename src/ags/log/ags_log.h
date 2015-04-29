/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2005-2011,2015 Joël Krähemann
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

#ifndef __AGS_LOG_H__
#define __AGS_LOG_H__

#include <glib.h>
#include <glib-object.h>

#include <pthread.h>
#include <time.h>
#include <stdio.h>

#define AGS_TYPE_LOG                (ags_log_get_type())
#define AGS_LOG(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_LOG, AgsLog))
#define AGS_LOG_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_LOG, AgsLog))
#define AGS_IS_LOG(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_LOG))
#define AGS_IS_LOG_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_LOG))
#define AGS_LOG_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_LOG, AgsLogClass))

#define AGS_LOG_MESSAGE_LENGTH (4096)
#define AGS_LOG_MESSAGE_DATE_LENGTH (19)

typedef struct _AgsLog AgsLog;
typedef struct _AgsLogClass AgsLogClass;
typedef struct _AgsLogMessage AgsLogMessage;
typedef struct _AgsLogFormatedMessage AgsLogFormatedMessage;

typedef enum{
  AGS_LOG_TRACK     = 1,
}AgsLogFLags;

typedef enum{
  AGS_LOG_DEEBUG     = 1,
  AGS_LOG_MESSAGE    = 1 << 1,
  AGS_LOG_WARN       = 1 << 2,
  AGS_LOG_ERROR      = 1 << 3,
}AgsLogMode;

struct _AgsLog
{
  GObject object;

  guint flags;
  guint mode;
  
  GObject *application_context;
  pthread_mutex_t *application_mutex;
  
  FILE *file;

  pthread_mutex_t *mutex;
  
  volatile guint queued;
  volatile guint pending;
  
  volatile GList *queued_formated_message;
  volatile GList *formated_message;
  
  GList *log;
};

struct _AgsLogClass
{
  GObjectClass object;
};

struct _AgsLogMessage
{
  guint mode;
  
  struct timespec *timestamp;
  char *format;
  va_list args;
};

struct _AgsLogFormatedMessage
{
  guint length;
  char *message;
};

AgsLogMessage* ags_log_message_alloc(struct timespec timestamp, char *format, va_list args);
void ags_log_message_free(AgsLogMessage *log_message);

AgsLogFormatedMessage* ags_log_formated_message_alloc(guint length, char *message);
void ags_log_formated_message_free(AgsLogFormatedMessage *formated_message);

void ags_log_debug(AgsLog *log, char *format, ...);
void ags_log_message(AgsLog *log, char *format, ...);
void ags_log_warn(AgsLog *log, char *format, ...);
void ags_log_error(AgsLog *log, char *format, ...);

void ags_log_add_queue(AgsLog *log, AgsLogMessage *log_message);

void ags_log_output(AgsLog *log,
		    GError **error);

AgsLog* ags_log_new(GObject *application_context);

#endif /*__AGS_LOG_H__*/
