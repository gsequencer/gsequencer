/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2022 Joël Krähemann
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

#ifndef __AGS_MACHINE_EDITOR_PAD_H__
#define __AGS_MACHINE_EDITOR_PAD_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#include <ags/app/ags_machine_editor_line.h>

G_BEGIN_DECLS

#define AGS_TYPE_MACHINE_EDITOR_PAD                (ags_machine_editor_pad_get_type())
#define AGS_MACHINE_EDITOR_PAD(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_MACHINE_EDITOR_PAD, AgsMachineEditorPad))
#define AGS_MACHINE_EDITOR_PAD_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_MACHINE_EDITOR_PAD, AgsMachineEditorPadClass))
#define AGS_IS_MACHINE_EDITOR_PAD(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_MACHINE_EDITOR_PAD))
#define AGS_IS_MACHINE_EDITOR_PAD_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_MACHINE_EDITOR_PAD))
#define AGS_MACHINE_EDITOR_PAD_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_MACHINE_EDITOR_PAD, AgsMachineEditorPadClass))

typedef struct _AgsMachineEditorPad AgsMachineEditorPad;
typedef struct _AgsMachineEditorPadClass AgsMachineEditorPadClass;

struct _AgsMachineEditorPad
{
  GtkBox box;

  guint connectable_flags;
  
  AgsChannel *channel;
  
  GtkExpander *expander;
  
  GList *line;

  GtkBox *line_box;
};

struct _AgsMachineEditorPadClass
{
  GtkBoxClass box;
};

GType ags_machine_editor_pad_get_type(void);

AgsMachineEditorPad* ags_machine_editor_pad_new(AgsChannel *channel);

G_END_DECLS

#endif /*__AGS_MACHINE_EDITOR_PAD_H__*/
