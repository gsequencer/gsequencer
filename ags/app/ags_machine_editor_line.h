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

#ifndef __AGS_MACHINE_EDITOR_LINE_H__
#define __AGS_MACHINE_EDITOR_LINE_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#include <ags/app/ags_link_editor.h>
#include <ags/app/ags_line_member_editor.h>

G_BEGIN_DECLS

#define AGS_TYPE_MACHINE_EDITOR_LINE                (ags_machine_editor_line_get_type())
#define AGS_MACHINE_EDITOR_LINE(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_MACHINE_EDITOR_LINE, AgsMachineEditorLine))
#define AGS_MACHINE_EDITOR_LINE_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_MACHINE_EDITOR_LINE, AgsMachineEditorLineClass))
#define AGS_IS_MACHINE_EDITOR_LINE(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_MACHINE_EDITOR_LINE))
#define AGS_IS_MACHINE_EDITOR_LINE_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_MACHINE_EDITOR_LINE))
#define AGS_MACHINE_EDITOR_LINE_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_MACHINE_EDITOR_LINE, AgsMachineEditorLineClass))

typedef struct _AgsMachineEditorLine AgsMachineEditorLine;
typedef struct _AgsMachineEditorLineClass AgsMachineEditorLineClass;

struct _AgsMachineEditorLine
{
  GtkBox box;

  guint connectable_flags;
  
  AgsChannel *channel;
  
  AgsLinkEditor *link_editor;
  AgsLineMemberEditor *line_member_editor;  
};

struct _AgsMachineEditorLineClass
{
  GtkBoxClass box;
};

GType ags_machine_editor_line_get_type(void);

AgsMachineEditorLine* ags_machine_editor_line_new(AgsChannel *channel);

G_END_DECLS

#endif /*__AGS_MACHINE_EDITOR_LINE_H__*/
