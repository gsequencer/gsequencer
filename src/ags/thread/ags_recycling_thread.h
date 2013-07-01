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

#ifndef __AGS_RECYCLING_THREAD_H__
#define __AGS_RECYCLING_THREAD_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/thread/ags_thread.h>

#include <ags/audio/ags_recycling.h>

#define AGS_TYPE_RECYCLING_THREAD                (ags_recycling_thread_get_type())
#define AGS_RECYCLING_THREAD(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_RECYCLING_THREAD, AgsRecyclingThread))
#define AGS_RECYCLING_THREAD_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_RECYCLING_THREAD, AgsRecyclingThread))
#define AGS_IS_RECYCLING_THREAD(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_RECYCLING_THREAD))
#define AGS_IS_RECYCLING_THREAD_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_RECYCLING_THREAD))
#define AGS_RECYCLING_THREAD_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_RECYCLING_THREAD, AgsRecyclingThreadClass))

typedef struct _AgsRecyclingThread AgsRecyclingThread;
typedef struct _AgsRecyclingThreadClass AgsRecyclingThreadClass;

typedef enum{
  AGS_RECYCLING_THREAD_WAIT      = 1,
  AGS_RECYCLING_THREAD_DONE      = 1 << 1,
}AgsRecyclingThreadFlags;

struct _AgsRecyclingThread
{
  AgsThread thread;

  guint flags;

  GObject *recycling;
  pthread_mutex_t iteration_mutex;
  pthread_cond_t iteration_cond;

  guint stage;
};

struct _AgsRecyclingThreadClass
{
  AgsThreadClass thread;

  void (*iterate)(AgsThread *thread, guint stage);

  void (*run_init_pre)(AgsRecyclingThread *recycling_thread);
  void (*run_init_inter)(AgsRecyclingThread *recycling_thread);
  void (*run_init_post)(AgsRecyclingThread *recycling_thread);

  void (*run_pre)(AgsRecyclingThread *recycling_thread);
  void (*run_inter)(AgsRecyclingThread *recycling_thread);
  void (*run_post)(AgsRecyclingThread *recycling_thread);
};

GType ags_recycling_thread_get_type();

void ags_recycling_thread_iterate(AgsRecyclingThread *recycling_thread);

void ags_recycling_thread_run_init_pre(AgsRecyclingThread *recycling_thread);
void ags_recycling_thread_run_init_inter(AgsRecyclingThread *recycling_thread);
void ags_recycling_thread_run_init_post(AgsRecyclingThread *recycling_thread);

void ags_recycling_thread_run_pre(AgsRecyclingThread *recycling_thread);
void ags_recycling_thread_run_inter(AgsRecyclingThread *recycling_thread);
void ags_recycling_thread_run_post(AgsRecyclingThread *recycling_thread);

AgsRecyclingThread* ags_recycling_thread_new(GObject *recycling);

#endif /*__AGS_RECYCLING_THREAD_H__*/
