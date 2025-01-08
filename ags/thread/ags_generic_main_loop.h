/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
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

#ifndef __AGS_GENERIC_MAIN_LOOP_H__
#define __AGS_GENERIC_MAIN_LOOP_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/thread/ags_thread.h>

G_BEGIN_DECLS

#define AGS_TYPE_GENERIC_MAIN_LOOP                (ags_generic_main_loop_get_type())
#define AGS_GENERIC_MAIN_LOOP(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_GENERIC_MAIN_LOOP, AgsGenericMainLoop))
#define AGS_GENERIC_MAIN_LOOP_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_GENERIC_MAIN_LOOP, AgsGenericMainLoopClass))
#define AGS_IS_GENERIC_MAIN_LOOP(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_GENERIC_MAIN_LOOP))
#define AGS_IS_GENERIC_MAIN_LOOP_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_GENERIC_MAIN_LOOP))
#define AGS_GENERIC_MAIN_LOOP_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_GENERIC_MAIN_LOOP, AgsGenericMainLoopClass))

#define AGS_GENERIC_MAIN_LOOP_DEFAULT_JIFFIE (AGS_THREAD_DEFAULT_JIFFIE)

typedef struct _AgsGenericMainLoop AgsGenericMainLoop;
typedef struct _AgsGenericMainLoopClass AgsGenericMainLoopClass;

struct _AgsGenericMainLoop
{
  AgsThread thread;
  
  GRecMutex tree_lock;

  gboolean is_syncing;

  gboolean is_critical_region;
  guint critical_region_ref;
};

struct _AgsGenericMainLoopClass
{
  AgsThreadClass thread;
};

GType ags_generic_main_loop_get_type();

AgsGenericMainLoop* ags_generic_main_loop_new();

G_END_DECLS

#endif /*__AGS_GENERIC_MAIN_LOOP_H__*/
