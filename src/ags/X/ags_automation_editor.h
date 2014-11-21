/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2014 Joël Krähemann
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef __AGS_AUTOMATION_EDITOR_H__
#define __AGS_AUTOMATION_EDITOR_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/X/editor/ags_automation_toolbar.h>
#include <ags/X/editor/ags_notebook.h>
#include <ags/X/editor/ags_machine_selector.h>
#include <ags/X/editor/ags_automation_edit.h>

#define AGS_TYPE_AUTOMATION_EDITOR                (ags_automation_editor_get_type())
#define AGS_AUTOMATION_EDITOR(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_AUTOMATION_EDITOR, AgsAutomationEditor))
#define AGS_AUTOMATION_EDITOR_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_AUTOMATION_EDITOR, AgsAutomationEditorClass))
#define AGS_IS_AUTOMATION_EDITOR(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_AUTOMATION_EDITOR))
#define AGS_IS_AUTOMATION_EDITOR_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_AUTOMATION_EDITOR))
#define AGS_AUTOMATION_EDITOR_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS (obj, AGS_TYPE_AUTOMATION_EDITOR, AgsAutomationEditorClass))

typedef struct _AgsAutomationEditor AgsAutomationEditor;
typedef struct _AgsAutomationEditorClass AgsAutomationEditorClass;

struct _AgsAutomationEditor
{
  GtkDialog dialog;

  gchar *version;
  gchar *build_id;

  GObject *window;

  AgsMachineSelector *machine_selector;
  AgsMachine *selected_machine;
  gulong set_audio_channels_handler;
  gulong set_pads_handler;

  AgsAutomationToolbar *automation_toolbar;

  AgsNotebook *notebook;

  AgsAutomationEdit *automation_edit;
};

struct _AgsAutomationEditorClass
{
  GtkDialogClass dialog;

  void (*machine_changed)(AgsAutomationEditor *automation_editor,
			  AgsMachine *machine);
};

GType ags_automation_editor_get_type(void);

void ags_automation_editor_machine_changed(AgsAutomationEditor *automation_editor,
					   AgsMachine *machine);

AgsAutomationEditor* ags_automation_editor_new(GObject *window);

#endif /*__AGS_AUTOMATION_EDITOR_H__*/
