/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2015 Joël Krähemann
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

#ifndef __AGS_HISTORY_H__
#define __AGS_HISTORY_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/audio/ags_task.h>

#define AGS_TYPE_HISTORY                (ags_history_get_type())
#define AGS_HISTORY(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_HISTORY, AgsHistory))
#define AGS_HISTORY_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_HISTORY, AgsHistory))
#define AGS_IS_HISTORY(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_HISTORY))
#define AGS_IS_HISTORY_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_HISTORY))
#define AGS_HISTORY_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_HISTORY, AgsHistoryClass))

typedef struct _AgsHistory AgsHistory;
typedef struct _AgsHistoryClass AgsHistoryClass;
typedef struct _AgsHistoryAppend AgsHistoryAppend;

struct _AgsHistory
{
  GObject gobject;
  
  GList *task;
};

struct _AgsHistoryClass
{
  GObjectClass gobject;
};

GType ags_history_get_type();

AgsHistory* ags_history_new();

#endif /*__AGS_HISTORY_H__*/
