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

#ifndef __AGS_SELECT_NOTE_POPOVER_H__
#define __AGS_SELECT_NOTE_POPOVER_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

G_BEGIN_DECLS

#define AGS_TYPE_SELECT_NOTE_POPOVER                (ags_select_note_popover_get_type())
#define AGS_SELECT_NOTE_POPOVER(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_SELECT_NOTE_POPOVER, AgsSelectNotePopover))
#define AGS_SELECT_NOTE_POPOVER_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_SELECT_NOTE_POPOVER, AgsSelectNotePopoverClass))
#define AGS_IS_SELECT_NOTE_POPOVER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_SELECT_NOTE_POPOVER))
#define AGS_IS_SELECT_NOTE_POPOVER_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_SELECT_NOTE_POPOVER))
#define AGS_SELECT_NOTE_POPOVER_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS (obj, AGS_TYPE_SELECT_NOTE_POPOVER, AgsSelectNotePopoverClass))

#define AGS_SELECT_NOTE_MAX_BEATS (16 * 1200)
#define AGS_SELECT_NOTE_MAX_KEYS (1024)

typedef struct _AgsSelectNotePopover AgsSelectNotePopover;
typedef struct _AgsSelectNotePopoverClass AgsSelectNotePopoverClass;

struct _AgsSelectNotePopover
{
  GtkPopover popover;

  guint connectable_flags;

  GtkCheckButton *copy_selection;

  GtkSpinButton *select_x0;
  GtkSpinButton *select_y0;

  GtkSpinButton *select_x1;
  GtkSpinButton *select_y1;

  GtkBox *action_area;

  GtkButton *activate_button;  
};

struct _AgsSelectNotePopoverClass
{
  GtkPopoverClass popover;
};

GType ags_select_note_popover_get_type(void);

AgsSelectNotePopover* ags_select_note_popover_new();

G_END_DECLS

#endif /*__AGS_SELECT_NOTE_POPOVER_H__*/
