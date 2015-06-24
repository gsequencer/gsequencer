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

#ifndef __AGS_EDITOR_H__
#define __AGS_EDITOR_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/audio/ags_devout.h>
#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_note.h>

#include <ags/X/ags_machine.h>

#include <ags/X/editor/ags_toolbar.h>
#include <ags/X/editor/ags_machine_selector.h>
#include <ags/X/editor/ags_notebook.h>
#include <ags/X/editor/ags_meter.h>

#define AGS_TYPE_EDITOR                (ags_editor_get_type ())
#define AGS_EDITOR(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_EDITOR, AgsEditor))
#define AGS_EDITOR_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_EDITOR, AgsEditorClass))
#define AGS_IS_EDITOR(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_EDITOR))
#define AGS_IS_EDITOR_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_EDITOR))
#define AGS_EDITOR_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_EDITOR, AgsEditorClass))

#define AGS_EDITOR_CHILD(ptr) ((AgsEditorChild *)(ptr))
  
#define AGS_EDITOR_DEFAULT_VERSION "0.4.2\0"
#define AGS_EDITOR_DEFAULT_BUILD_ID "CEST 02-10-2014 19:36\0"

#define AGS_EDITOR_DEFAULT "default\0"

typedef struct _AgsEditor AgsEditor;
typedef struct _AgsEditorClass AgsEditorClass;

typedef struct _AgsEditorChild AgsEditorChild;

struct _AgsEditor
{
  GtkVBox vbox;

  guint flags;

  gchar *version;
  gchar *build_id;

  AgsDevout *devout;

  AgsMachineSelector *machine_selector;
  AgsMachine *selected_machine;

  AgsToolbar *toolbar;

  GList *editor_child;
  GtkTable *table;
  
  AgsNotebook *current_notebook;
  AgsMeter *current_meter;
  GtkWidget *current_edit_widget;

  guint tact_counter;
};

struct _AgsEditorClass
{
  GtkVBoxClass vbox;

  void (*machine_changed)(AgsEditor *editor, AgsMachine *machine);
};

struct _AgsEditorChild
{
  AgsMachine *machine;

  AgsNotebook *notebook;
  AgsMeter *meter;
  GtkWidget *edit_widget;
};

GType ags_editor_get_type(void);

AgsEditorChild* ags_editor_child_alloc(AgsMachine *machine, AgsNotebook *notebook, AgsMeter *meter, GtkWidget *edit_widget);

void ags_editor_machine_changed(AgsEditor *editor, AgsMachine *machine);

void ags_editor_select_all(AgsEditor *editor);

void ags_editor_paste(AgsEditor *editor);
void ags_editor_copy(AgsEditor *editor);
void ags_editor_cut(AgsEditor *editor);
void ags_editor_invert(AgsEditor *editor);

AgsEditor* ags_editor_new();

#endif /*__AGS_EDITOR_H__*/
