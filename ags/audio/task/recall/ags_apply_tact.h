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

#ifndef __AGS_APPLY_TACT_H__
#define __AGS_APPLY_TACT_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/thread/ags_task.h>

#define AGS_TYPE_APPLY_TACT                (ags_apply_tact_get_type())
#define AGS_APPLY_TACT(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_APPLY_TACT, AgsApplyTact))
#define AGS_APPLY_TACT_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_APPLY_TACT, AgsApplyTactClass))
#define AGS_IS_APPLY_TACT(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_APPLY_TACT))
#define AGS_IS_APPLY_TACT_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_APPLY_TACT))
#define AGS_APPLY_TACT_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_APPLY_TACT, AgsApplyTactClass))

typedef struct _AgsApplyTact AgsApplyTact;
typedef struct _AgsApplyTactClass AgsApplyTactClass;

struct _AgsApplyTact
{
  AgsTask task;

  GObject *gobject;

  gdouble tact;
};

struct _AgsApplyTactClass
{
  AgsTaskClass task;
};

GType ags_apply_tact_get_type();

AgsApplyTact* ags_apply_tact_new(GObject *gobject,
				 gdouble tact);

#endif /*__AGS_APPLY_TACT_H__*/
