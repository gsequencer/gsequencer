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

#ifndef __AGS_APPLY_WAVETABLE_H__
#define __AGS_APPLY_WAVETABLE_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/thread/ags_task.h>

#define AGS_TYPE_APPLY_WAVETABLE                (ags_apply_wavetable_get_type())
#define AGS_APPLY_WAVETABLE(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_APPLY_WAVETABLE, AgsApplyWavetable))
#define AGS_APPLY_WAVETABLE_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_APPLY_WAVETABLE, AgsApplyWavetableClass))
#define AGS_IS_APPLY_WAVETABLE(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_APPLY_WAVETABLE))
#define AGS_IS_APPLY_WAVETABLE_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_APPLY_WAVETABLE))
#define AGS_APPLY_WAVETABLE_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_APPLY_WAVETABLE, AgsApplyWavetableClass))

typedef struct _AgsApplyWavetable AgsApplyWavetable;
typedef struct _AgsApplyWavetableClass AgsApplyWavetableClass;

struct _AgsApplyWavetable
{
  AgsTask task;
};

struct _AgsApplyWavetableClass
{
  AgsTaskClass task;
};

GType ags_apply_wavetable_get_type();

AgsApplyWavetable* ags_apply_wavetable_new();

#endif /*__AGS_APPLY_WAVETABLE_H__*/
