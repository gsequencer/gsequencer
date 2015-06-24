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

#ifndef __AGS_SET_MUTED_H__
#define __AGS_SET_MUTED_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/audio/ags_task.h>

#define AGS_TYPE_SET_MUTED                (ags_set_muted_get_type())
#define AGS_SET_MUTED(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_SET_MUTED, AgsSetMuted))
#define AGS_SET_MUTED_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_SET_MUTED, AgsSetMutedClass))
#define AGS_IS_SET_MUTED(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_SET_MUTED))
#define AGS_IS_SET_MUTED_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_SET_MUTED))
#define AGS_SET_MUTED_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_SET_MUTED, AgsSetMutedClass))

typedef struct _AgsSetMuted AgsSetMuted;
typedef struct _AgsSetMutedClass AgsSetMutedClass;

struct _AgsSetMuted
{
  AgsTask task;

  GObject *gobject;

  gboolean muted;
};

struct _AgsSetMutedClass
{
  AgsTaskClass task;
};

GType ags_set_muted_get_type();

AgsSetMuted* ags_set_muted_new(GObject *gobject,
			       gboolean muted);

#endif /*__AGS_SET_MUTED_H__*/
