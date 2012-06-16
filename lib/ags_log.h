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

#ifndef __AGS_LOG_H__
#define __AGS_LOG_H__

#include <glib.h>
#include <glib-object.h>

#include <pthread.h>
#include <time.h>

#define AGS_TYPE_LOG                (ags_log_get_type())
#define AGS_LOG(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_LOG, AgsLog))
#define AGS_LOG_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_LOG, AgsLog))
#define AGS_IS_LOG(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_LOG))
#define AGS_IS_LOG_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_LOG))
#define AGS_LOG_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_LOG, AgsLogClass))

typedef struct _AgsLog AgsLog;
typedef struct _AgsLogClass AgsLogClass;
typedef struct _AgsLogMessage AgsLogMessage;

typedef enum{
  AGS_LOG_RUNNING                     = 1,
  AGS_LOG_OUTPUT_EMPTY                = 1 <<  1,
  AGS_LOG_QUEUE_EMPTY                 = 1 <<  2,
  AGS_LOG_EMPTY                       = 1 <<  3,
  AGS_LOG_COPY_FROM_LOG_TO_QUEUE      = 1 <<  4,
  AGS_LOG_COPY_FROM_QUEUE_TO_OUTPUT   = 1 <<  5,
}AgsLogFlags;

struct _AgsLog
{
  GObject object;

  guint flags;

  FILE *file;

  struct timespec *log_interval;

  pthread_t broker_thread;
  pthread_attr_t broker_thread_attr;

  GList *output_formated_message;
  pthread_t output_thread;
  pthread_attr_t output_thread_attr;
  pthread_mutex_t output_mutex;
  pthread_mutexattr_t output_mutex_attr;
  pthread_cond_t output_wait_cond;

  GList *queue_formated_message;
  GList *queue_message;
  pthread_t queue_thread;
  pthread_attr_t queue_thread_attr;
  pthread_mutex_t queue_mutex;
  pthread_mutexattr_t queue_mutex_attr;
  pthread_cond_t queue_wait_cond;

  GList *log_message;
  pthread_mutex_t log_mutex;
  pthread_mutexattr_t log_mutex_attr;
  pthread_cond_t log_wait_cond;
  pthread_mutex_t log_add_mutex;
  pthread_mutexattr_t log_add_mutex_attr;
  pthread_cond_t log_add_wait_cond;
};

struct _AgsLogClass
{
  GObjectClass object;
};

struct _AgsLogMessage
{
  struct timespec *time;
  char *format;
  va_list args;
};

void ags_log_start_queue(AgsLog *log);
void ags_log_stop_queue(AgsLog *log);

void ags_log_message(AgsLog *log, char *format, ...);

#endif /*__AGS_LOG_H__*/
