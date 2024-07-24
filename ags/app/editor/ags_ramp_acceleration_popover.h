/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2024 Joël Krähemann
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

#ifndef __AGS_RAMP_ACCELERATION_POPOVER_H__
#define __AGS_RAMP_ACCELERATION_POPOVER_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

G_BEGIN_DECLS

#define AGS_TYPE_RAMP_ACCELERATION_POPOVER                (ags_ramp_acceleration_popover_get_type())
#define AGS_RAMP_ACCELERATION_POPOVER(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_RAMP_ACCELERATION_POPOVER, AgsRampAccelerationPopover))
#define AGS_RAMP_ACCELERATION_POPOVER_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_RAMP_ACCELERATION_POPOVER, AgsRampAccelerationPopoverClass))
#define AGS_IS_RAMP_ACCELERATION_POPOVER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_RAMP_ACCELERATION_POPOVER))
#define AGS_IS_RAMP_ACCELERATION_POPOVER_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_RAMP_ACCELERATION_POPOVER))
#define AGS_RAMP_ACCELERATION_POPOVER_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS (obj, AGS_TYPE_RAMP_ACCELERATION_POPOVER, AgsRampAccelerationPopoverClass))

#define AGS_RAMP_ACCELERATION_MAX_BEATS (16 * 16 * 1200)
#define AGS_RAMP_ACCELERATION_MAX_STEPS (128)
#define AGS_RAMP_ACCELERATION_DEFAULT_WIDTH (64)

typedef struct _AgsRampAccelerationPopover AgsRampAccelerationPopover;
typedef struct _AgsRampAccelerationPopoverClass AgsRampAccelerationPopoverClass;

struct _AgsRampAccelerationPopover
{
  GtkPopover popover;

  guint connectable_flags;
  
  GtkComboBoxText *port;

  GtkSpinButton *ramp_x0;
  GtkSpinButton *ramp_y0;

  GtkSpinButton *ramp_x1;
  GtkSpinButton *ramp_y1;

  GtkSpinButton *ramp_step_count;

  GtkBox *action_area;

  GtkButton *activate_button;  
};

struct _AgsRampAccelerationPopoverClass
{
  GtkPopoverClass popover;
};

GType ags_ramp_acceleration_popover_get_type(void);

AgsRampAccelerationPopover* ags_ramp_acceleration_popover_new();

G_END_DECLS

#endif /*__AGS_RAMP_ACCELERATION_POPOVER_H__*/
