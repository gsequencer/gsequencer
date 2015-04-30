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

#ifndef __AGS_XORG_APPLICATION_CONTEXT_H__
#define __AGS_XORG_APPLICATION_CONTEXT_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include <ags/object/ags_application_context.h>
#include <ags/object/ags_soundcard.h>

#include <ags/thread/ags_concurrency_provider.h>
#ifdef AGS_USE_LINUX_THREADS
#include <ags/thread/ags_thread-kthreads.h>
#else
#include <ags/thread/ags_thread-posix.h>
#endif
#include <ags/thread/ags_single_thread.h>
#include <ags/thread/ags_autosave_thread.h>
#include <ags/thread/ags_task_thread.h>
#include <ags/thread/ags_thread_pool.h>

#include <ags/audio/thread/ags_audio_loop.h>
#include <ags/audio/thread/ags_devout_thread.h>
#include <ags/audio/thread/ags_export_thread.h>

#include <ags/server/ags_server.h>

#include <ags/X/ags_window.h>

#include <ags/X/thread/ags_gui_thread.h>

#define AGS_TYPE_XORG_APPLICATION_CONTEXT                (ags_xorg_application_context_get_type())
#define AGS_XORG_APPLICATION_CONTEXT(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_XORG_APPLICATION_CONTEXT, AgsXorgApplicationContext))
#define AGS_XORG_APPLICATION_CONTEXT_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_XORG_APPLICATION_CONTEXT, AgsXorgApplicationContextClass))
#define AGS_IS_XORG_APPLICATION_CONTEXT(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_XORG_APPLICATION_CONTEXT))
#define AGS_IS_XORG_APPLICATION_CONTEXT_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_XORG_APPLICATION_CONTEXT))
#define AGS_XORG_APPLICATION_CONTEXT_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_XORG_APPLICATION_CONTEXT, AgsXorgApplicationContextClass))

#define AGS_XORG_VERSION "0.4.3\0"
#define AGS_XORG_BUILD_ID "Thu Apr  2 13:04:21 GMT 2015\0"

typedef struct _AgsXorgApplicationContext AgsXorgApplicationContext;
typedef struct _AgsXorgApplicationContextClass AgsXorgApplicationContextClass;

typedef enum{
  AGS_XORG_APPLICATION_CONTEXT_SHOW_GUI      = 1,
}AgsXorgApplicationContextFlags;

struct _AgsXorgApplicationContext
{
  AgsApplicationContext application_context;

  AgsThreadPool *thread_pool;

  AgsThread *devout_thread;
  AgsThread *export_thread;

  AgsThread *gui_thread;
  
  AgsThread *autosave_thread;

  AgsServer *server;
  
  GList *soundcard;
  
  AgsWindow *window;
};

struct _AgsXorgApplicationContextClass
{
  AgsApplicationContextClass application_context;
};

GType ags_xorg_application_context_get_type();

AgsXorgApplicationContext* ags_xorg_application_context_new();

#endif /*__AGS_XORG_APPLICATION_CONTEXT_H__*/
