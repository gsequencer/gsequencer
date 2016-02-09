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

#ifndef __AGS_THREAD_APPLICATION_CONTEXT_H__
#define __AGS_THREAD_APPLICATION_CONTEXT_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/object/ags_application_context.h>

#ifdef AGS_USE_LINUX_THREADS
#include <ags/thread/ags_thread-kthreads.h>
#else
#include <ags/thread/ags_thread-posix.h>
#endif 

#include <ags/thread/ags_thread_pool.h>

#define AGS_TYPE_THREAD_APPLICATION_CONTEXT                (ags_thread_application_context_get_type())
#define AGS_THREAD_APPLICATION_CONTEXT(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_THREAD_APPLICATION_CONTEXT, AgsThreadApplicationContext))
#define AGS_THREAD_APPLICATION_CONTEXT_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_THREAD_APPLICATION_CONTEXT, AgsThreadApplicationContextClass))
#define AGS_IS_THREAD_APPLICATION_CONTEXT(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_THREAD_APPLICATION_CONTEXT))
#define AGS_IS_THREAD_APPLICATION_CONTEXT_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_THREAD_APPLICATION_CONTEXT))
#define AGS_THREAD_APPLICATION_CONTEXT_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_THREAD_APPLICATION_CONTEXT, AgsThreadApplicationContextClass))

#define AGS_THREAD_BUILD_ID "Thu Apr  2 13:04:21 GMT 2015\0"
#define AGS_THREAD_DEFAULT_VERSION "0.4.3\0"

typedef struct _AgsThreadApplicationContext AgsThreadApplicationContext;
typedef struct _AgsThreadApplicationContextClass AgsThreadApplicationContextClass;

typedef enum{
  AGS_THREAD_APPLICATION_CONTEXT_SINGLE_THREAD      = 1,
}AgsThreadApplicationContextFlags;

struct _AgsThreadApplicationContext
{
  AgsApplicationContextClass application_contex;

  guint flags;

  AgsThread *main_loop;
  AgsThread *autosave_thread;
  AgsThreadPool *thread_pool;
};

struct _AgsThreadApplicationContextClass
{
  AgsApplicationContextClass application_contex;
};

GType ags_thread_application_context_get_type();

AgsThreadApplicationContext* ags_thread_application_context_new(AgsMainLoop *main_loop,
								AgsConfig *config);

#endif /*__AGS_THREAD_APPLICATION_CONTEXT_H__*/
