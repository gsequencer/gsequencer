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

#ifndef __AGS_START_DEVOUT_H__
#define __AGS_START_DEVOUT_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/audio/ags_task.h>
#include <ags/audio/ags_devout.h>

#define AGS_TYPE_START_DEVOUT                (ags_start_devout_get_type())
#define AGS_START_DEVOUT(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_START_DEVOUT, AgsStartDevout))
#define AGS_START_DEVOUT_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_START_DEVOUT, AgsStartDevoutClass))
#define AGS_IS_START_DEVOUT(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_START_DEVOUT))
#define AGS_IS_START_DEVOUT_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_START_DEVOUT))
#define AGS_START_DEVOUT_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_START_DEVOUT, AgsStartDevoutClass))

typedef struct _AgsStartDevout AgsStartDevout;
typedef struct _AgsStartDevoutClass AgsStartDevoutClass;

struct _AgsStartDevout
{
  AgsTask task;

  AgsDevout *devout;
};

struct _AgsStartDevoutClass
{
  AgsTaskClass task;
};

GType ags_start_devout_get_type();

AgsStartDevout* ags_start_devout_new(AgsDevout *devout);

#endif /*__AGS_START_DEVOUT_H__*/

