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

#ifndef __AGS_THREAD_POOL_H__
#define __AGS_THREAD_POOL_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/thread/ags_thread_pool.h>

#define AGS_TYPE_THREAD_POOL                (ags_thread_pool_get_type())
#define AGS_THREAD_POOL(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_THREAD_POOL, AgsThreadPool))
#define AGS_THREAD_POOL_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_THREAD_POOL, AgsThreadPoolClass))
#define AGS_IS_THREAD_POOL(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_THREAD_POOL))
#define AGS_IS_THREAD_POOL_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_THREAD_POOL))
#define AGS_THREAD_POOL_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_THREAD_POOL, AgsThreadPoolClass))

typedef struct _AgsThreadPool AgsThreadPool;
typedef struct _AgsThreadPoolClass AgsThreadPoolClass;

struct _AgsThreadPool
{
  GObject object;

  GList *returnable_thread;
};

struct _AgsThreadPoolClass
{
  GObjectClass object;
};

GType ags_thread_pool_get_type();

AgsThreadPool* ags_thread_pool_new();

#endif /*__AGS_THREAD_POOL_H__*/
