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

#ifndef __AGS_TIME_STRETCH_BUFFER_POPOVER_H__
#define __AGS_TIME_STRETCH_BUFFER_POPOVER_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

G_BEGIN_DECLS

#define AGS_TYPE_TIME_STRETCH_BUFFER_POPOVER                (ags_time_stretch_buffer_popover_get_type())
#define AGS_TIME_STRETCH_BUFFER_POPOVER(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_TIME_STRETCH_BUFFER_POPOVER, AgsTimeStretchBufferPopover))
#define AGS_TIME_STRETCH_BUFFER_POPOVER_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_TIME_STRETCH_BUFFER_POPOVER, AgsTimeStretchBufferPopoverClass))
#define AGS_IS_TIME_STRETCH_BUFFER_POPOVER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_TIME_STRETCH_BUFFER_POPOVER))
#define AGS_IS_TIME_STRETCH_BUFFER_POPOVER_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_TIME_STRETCH_BUFFER_POPOVER))
#define AGS_TIME_STRETCH_BUFFER_POPOVER_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS (obj, AGS_TYPE_TIME_STRETCH_BUFFER_POPOVER, AgsTimeStretchBufferPopoverClass))

#define AGS_TIME_STRETCH_BUFFER_MAX_FREQUENCY (22050.0)
#define AGS_TIME_STRETCH_BUFFER_DEFAULT_FREQUENCY (440.0)

#define AGS_TIME_STRETCH_BUFFER_MAX_BPM (1920.0)
#define AGS_TIME_STRETCH_BUFFER_DEFAULT_BPM (120.0)

typedef struct _AgsTimeStretchBufferPopover AgsTimeStretchBufferPopover;
typedef struct _AgsTimeStretchBufferPopoverClass AgsTimeStretchBufferPopoverClass;

struct _AgsTimeStretchBufferPopover
{
  GtkPopover popover;

  guint connectable_flags;

  GtkSpinButton *frequency;

  GtkSpinButton *orig_bpm;
  GtkSpinButton *new_bpm;

  GtkBox *action_area;

  GtkButton *activate_button;  
};

struct _AgsTimeStretchBufferPopoverClass
{
  GtkPopoverClass popover;
};

GType ags_time_stretch_buffer_popover_get_type(void);

AgsTimeStretchBufferPopover* ags_time_stretch_buffer_popover_new();

G_END_DECLS

#endif /*__AGS_TIME_STRETCH_BUFFER_POPOVER_H__*/
