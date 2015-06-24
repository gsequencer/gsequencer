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

#ifndef __AGS_APPLY_BPM_H__
#define __AGS_APPLY_BPM_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/audio/ags_task.h>

#define AGS_TYPE_APPLY_BPM                (ags_apply_bpm_get_type())
#define AGS_APPLY_BPM(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_APPLY_BPM, AgsApplyBpm))
#define AGS_APPLY_BPM_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_APPLY_BPM, AgsApplyBpmClass))
#define AGS_IS_APPLY_BPM(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_APPLY_BPM))
#define AGS_IS_APPLY_BPM_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_APPLY_BPM))
#define AGS_APPLY_BPM_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_APPLY_BPM, AgsApplyBpmClass))

typedef struct _AgsApplyBpm AgsApplyBpm;
typedef struct _AgsApplyBpmClass AgsApplyBpmClass;

struct _AgsApplyBpm
{
  AgsTask task;

  GObject *gobject;

  gdouble bpm;
};

struct _AgsApplyBpmClass
{
  AgsTaskClass task;
};

GType ags_apply_bpm_get_type();

AgsApplyBpm* ags_apply_bpm_new(GObject *gobject,
			       gdouble bpm);

#endif /*__AGS_APPLY_BPM_H__*/
