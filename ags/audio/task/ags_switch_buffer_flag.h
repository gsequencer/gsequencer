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

#ifndef __AGS_SWITCH_BUFFER_FLAG_H__
#define __AGS_SWITCH_BUFFER_FLAG_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/thread/ags_task.h>

#define AGS_TYPE_SWITCH_BUFFER_FLAG                (ags_switch_buffer_flag_get_type())
#define AGS_SWITCH_BUFFER_FLAG(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_SWITCH_BUFFER_FLAG, AgsSwitchBufferFlag))
#define AGS_SWITCH_BUFFER_FLAG_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_SWITCH_BUFFER_FLAG, AgsSwitchBufferFlagClass))
#define AGS_IS_SWITCH_BUFFER_FLAG(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_SWITCH_BUFFER_FLAG))
#define AGS_IS_SWITCH_BUFFER_FLAG_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_SWITCH_BUFFER_FLAG))
#define AGS_SWITCH_BUFFER_FLAG_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_SWITCH_BUFFER_FLAG, AgsSwitchBufferFlagClass))

typedef struct _AgsSwitchBufferFlag AgsSwitchBufferFlag;
typedef struct _AgsSwitchBufferFlagClass AgsSwitchBufferFlagClass;

struct _AgsSwitchBufferFlag
{
  AgsTask task;

  GObject *devout;
};

struct _AgsSwitchBufferFlagClass
{
  AgsTaskClass task;
};

GType ags_switch_buffer_flag_get_type();

AgsSwitchBufferFlag* ags_switch_buffer_flag_new(GObject *devout);

#endif /*__AGS_SWITCH_BUFFER_FLAG_H__*/

