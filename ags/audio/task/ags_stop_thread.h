/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2022 Joël Krähemann
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

#ifndef __AGS_STOP_THREAD_H__
#define __AGS_STOP_THREAD_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

G_BEGIN_DECLS

#define AGS_TYPE_STOP_THREAD                (ags_stop_thread_get_type())
#define AGS_STOP_THREAD(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_STOP_THREAD, AgsStopThread))
#define AGS_STOP_THREAD_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_STOP_THREAD, AgsStopThreadClass))
#define AGS_IS_STOP_THREAD(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_STOP_THREAD))
#define AGS_IS_STOP_THREAD_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_STOP_THREAD))
#define AGS_STOP_THREAD_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_STOP_THREAD, AgsStopThreadClass))

typedef struct _AgsStopThread AgsStopThread;
typedef struct _AgsStopThreadClass AgsStopThreadClass;

struct _AgsStopThread
{
  AgsTask task;
};

struct _AgsStopThreadClass
{
  AgsTaskClass task;
};

GType ags_stop_thread_get_type();

AgsStopThread* ags_stop_thread_new();

G_END_DECLS

#endif /*__AGS_STOP_THREAD_H__*/
