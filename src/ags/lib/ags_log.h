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
#include <stdio.h>

#define AGS_TYPE_LOG                (ags_log_get_type())
#define AGS_LOG(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_LOG, AgsLog))
#define AGS_LOG_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_LOG, AgsLog))
#define AGS_IS_LOG(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_LOG))
#define AGS_IS_LOG_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_LOG))
#define AGS_LOG_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_LOG, AgsLogClass))

extern struct _AgsLog *ags_default_log;

#define ags_default_log ags_default_log

typedef struct _AgsLog AgsLog;
typedef struct _AgsLogClass AgsLogClass;
typedef struct _AgsLogMessage AgsLogMessage;
typedef struct _AgsLogFormatedMessage AgsLogFormatedMessage;

typedef enum{
  AGS_LOG_STARTING                    = 1,
  AGS_LOG_RUNNING                     = 1 <<  1,
  AGS_LOG_TIMER_SLEEP                 = 1 <<  2,
  AGS_LOG_OUTPUT_SUSPEND              = 1 <<  3,
  AGS_LOG_OUTPUT_WAITS_FOR_QUEUE      = 1 <<  4,
  AGS_LOG_QUEUE_SUSPEND               = 1 <<  5,
  AGS_LOG_SUSPEND                     = 1 <<  6,
  AGS_LOG_COPY_FROM_QUEUE_TO_OUTPUT   = 1 <<  7,
  AGS_LOG_OMMIT_DEBUG                 = 1 <<  8,
}AgsLogFlags;

struct _AgsLog
{
  GObject object;

  guint flags;

  FILE *file;
  pthread_mutex_t start_mutex;
  pthread_cond_t start_wait_cond;

  struct timespec *log_interval;
  struct timespec *free_float;
  pthread_t timer_thread;
  pthread_attr_t timer_thread_attr;
  pthread_mutex_t timer_mutex;
  pthread_mutexattr_t timer_mutex_attr;
  gboolean timer_started;
  gboolean timer_active;
  gboolean timer_awaken;
  pthread_cond_t timer_wait_cond;

  pthread_t broker_thread;
  pthread_attr_t broker_thread_attr;
  pthread_mutex_t broker_mutex;
  pthread_mutexattr_t broker_mutex_attr;
  gboolean broker_started;
  gboolean broker_active;
  gboolean broker_awaken;
  gboolean signaled_timer;
  gboolean signaled_log;
  gboolean signaled_queue;
  gboolean signaled_output;
  pthread_cond_t broker_wait_cond;

  GList *output_formated_message;
  pthread_t output_thread;
  pthread_attr_t output_thread_attr;
  pthread_mutex_t output_mutex;
  pthread_mutexattr_t output_mutex_attr;
  gboolean output_started;
  gboolean output_active;
  gboolean output_awaken;
  pthread_cond_t output_wait_cond;

  GList *queue_formated_message;
  GList *queue_message;
  pthread_t queue_thread;
  pthread_attr_t queue_thread_attr;
  pthread_mutex_t queue_mutex;
  pthread_mutexattr_t queue_mutex_attr;
  gboolean queue_started;
  gboolean queue_active;
  gboolean queue_awaken;
  pthread_cond_t queue_wait_cond;

  GList *log;
  guint active_logs;
  guint suspended_logs;
  pthread_cond_t log_wait_cond;
};

struct _AgsLogClass
{
  GObjectClass object;
};

struct _AgsLogMessage
{
  gboolean debug;

  struct timespec *time;
  char *format;
  va_list args;
};

struct _AgsLogFormatedMessage
{
  int length;
  char *message;
};

void ags_log_start_queue(AgsLog *log);
void ags_log_stop_queue(AgsLog *log);

void ags_log_debug(AgsLog *log, char *format, ...);
void ags_log_message(AgsLog *log, char *format, ...);

#endif /*__AGS_LOG_H__*/
