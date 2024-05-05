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

#ifndef __AGS_POSITION_AUTOMATION_CURSOR_POPOVER_H__
#define __AGS_POSITION_AUTOMATION_CURSOR_POPOVER_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

G_BEGIN_DECLS

#define AGS_TYPE_POSITION_AUTOMATION_CURSOR_POPOVER                (ags_position_automation_cursor_popover_get_type())
#define AGS_POSITION_AUTOMATION_CURSOR_POPOVER(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_POSITION_AUTOMATION_CURSOR_POPOVER, AgsPositionAutomationCursorPopover))
#define AGS_POSITION_AUTOMATION_CURSOR_POPOVER_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_POSITION_AUTOMATION_CURSOR_POPOVER, AgsPositionAutomationCursorPopoverClass))
#define AGS_IS_POSITION_AUTOMATION_CURSOR_POPOVER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_POSITION_AUTOMATION_CURSOR_POPOVER))
#define AGS_IS_POSITION_AUTOMATION_CURSOR_POPOVER_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_POSITION_AUTOMATION_CURSOR_POPOVER))
#define AGS_POSITION_AUTOMATION_CURSOR_POPOVER_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS (obj, AGS_TYPE_POSITION_AUTOMATION_CURSOR_POPOVER, AgsPositionAutomationCursorPopoverClass))

#define AGS_POSITION_AUTOMATION_CURSOR_MAX_BEATS (16 * 1200)

typedef struct _AgsPositionAutomationCursorPopover AgsPositionAutomationCursorPopover;
typedef struct _AgsPositionAutomationCursorPopoverClass AgsPositionAutomationCursorPopoverClass;

struct _AgsPositionAutomationCursorPopover
{
  GtkPopover popover;

  guint connectable_flags;
  
  GtkCheckButton *set_focus;

  GtkSpinButton *position_x;

  GtkBox *action_area;

  GtkButton *activate_button;  
};

struct _AgsPositionAutomationCursorPopoverClass
{
  GtkPopoverClass popover;
};

GType ags_position_automation_cursor_popover_get_type(void);

AgsPositionAutomationCursorPopover* ags_position_automation_cursor_popover_new();

G_END_DECLS

#endif /*__AGS_POSITION_AUTOMATION_CURSOR_POPOVER_H__*/
