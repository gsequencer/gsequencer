/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2013 Joël Krähemann
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

#ifndef __AGS_RETURNABLE_THREAD_H__
#define __AGS_RETURNABLE_THREAD_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/thread/ags_thread.h>

#define AGS_TYPE_RETURNABLE_THREAD                (ags_returnable_thread_get_type())
#define AGS_RETURNABLE_THREAD(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_RETURNABLE_THREAD, AgsReturnableThread))
#define AGS_RETURNABLE_THREAD_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_RETURNABLE_THREAD, AgsReturnableThreadClass))
#define AGS_IS_RETURNABLE_THREAD(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_RETURNABLE_THREAD))
#define AGS_IS_RETURNABLE_THREAD_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_RETURNABLE_THREAD))
#define AGS_RETURNABLE_THREAD_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_RETURNABLE_THREAD, AgsReturnableThreadClass))

typedef struct _AgsReturnableThread AgsReturnableThread;
typedef struct _AgsReturnableThreadClass AgsReturnableThreadClass;

typedef enum{
  AGS_RETURNABLE_THREAD_IN_USE     = 1,
}AgsReturnableThreadFlags;

struct _AgsReturnableThread
{
  AgsThread thread;

  volatile guint flags;
};

struct _AgsReturnableThreadClass
{
  AgsThreadClass thread;
};

GType ags_returnable_thread_get_type();

AgsReturnableThread* ags_returnable_thread_new();

#endif /*__AGS_RETURNABLE_THREAD_H__*/
