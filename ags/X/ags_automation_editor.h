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

#ifndef __AGS_AUTOMATION_EDITOR_H__
#define __AGS_AUTOMATION_EDITOR_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/X/editor/ags_automation_toolbar.h>
#include <ags/X/editor/ags_notebook.h>
#include <ags/X/editor/ags_machine_selector.h>
#include <ags/X/editor/ags_scale.h>
#include <ags/X/editor/ags_automation_edit.h>

#define AGS_TYPE_AUTOMATION_EDITOR                (ags_automation_editor_get_type())
#define AGS_AUTOMATION_EDITOR(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_AUTOMATION_EDITOR, AgsAutomationEditor))
#define AGS_AUTOMATION_EDITOR_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_AUTOMATION_EDITOR, AgsAutomationEditorClass))
#define AGS_IS_AUTOMATION_EDITOR(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_AUTOMATION_EDITOR))
#define AGS_IS_AUTOMATION_EDITOR_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_AUTOMATION_EDITOR))
#define AGS_AUTOMATION_EDITOR_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS (obj, AGS_TYPE_AUTOMATION_EDITOR, AgsAutomationEditorClass))

#define AGS_AUTOMATION_EDITOR_CHILD(ptr) ((AgsAutomationEditorChild *)(ptr))

#define AGS_AUTOMATION_EDITOR_DEFAULT_VERSION "0.7.29\0"
#define AGS_AUTOMATION_EDITOR_DEFAULT_BUILD_ID "Mon Jun 13 11:50:58 UTC 2016\0"

typedef struct _AgsAutomationEditor AgsAutomationEditor;
typedef struct _AgsAutomationEditorClass AgsAutomationEditorClass;
typedef struct _AgsAutomationEditorChild AgsAutomationEditorChild;
  
typedef enum{
  AGS_AUTOMATION_EDITOR_CONNECTED    = 1,
}AgsAutomationEditorFlags;

struct _AgsAutomationEditor
{
  GtkVBox vbox;

  guint flags;
  
  gchar *version;
  gchar *build_id;

  GObject *soundcard;
  
  AgsMachineSelector *machine_selector;
  AgsMachine *selected_machine;

  AgsAutomationToolbar *automation_toolbar;

  GList *automation_editor_child;
  GtkTable *audio_table;
  GtkTable *output_table;
  GtkTable *input_table;
  
  AgsScale *current_audio_scale;
  GtkWidget *current_audio_automation_edit;

  AgsNotebook *current_output_notebook;
  AgsScale *current_output_scale;
  GtkWidget *current_output_automation_edit;

  AgsNotebook *current_input_notebook;
  AgsScale *current_input_scale;
  GtkWidget *current_input_automation_edit;

  guint tact_counter;
  gdouble current_tact;
};

struct _AgsAutomationEditorClass
{
  GtkVBoxClass vbox;

  void (*machine_changed)(AgsAutomationEditor *automation_editor,
			  AgsMachine *machine);
};

struct _AgsAutomationEditorChild
{
  AgsMachine *machine;
  
  AgsScale *audio_scale;
  AgsAutomationEdit *audio_automation_edit;

  AgsScale *output_scale;
  AgsNotebook *output_notebook;
  AgsAutomationEdit *output_automation_edit;

  AgsScale *input_scale;
  AgsNotebook *input_notebook;
  AgsAutomationEdit *input_automation_edit;
};
  
GType ags_automation_editor_get_type(void);

AgsAutomationEditorChild* ags_automation_editor_child_alloc(AgsMachine *machine,
							    AgsScale *audio_scale, AgsAutomationEdit *audio_automation_edit,
							    AgsNotebook *output_notebook, AgsScale *output_scale, AgsAutomationEdit *output_automation_edit,
							    AgsNotebook *input_notebook, AgsScale *input_scale, AgsAutomationEdit *input_automation_edit);

void ags_automation_editor_machine_changed(AgsAutomationEditor *automation_editor,
					   AgsMachine *machine);

void ags_automation_editor_select_all(AgsAutomationEditor *automation_editor);

void ags_automation_editor_paste(AgsAutomationEditor *automation_editor);
void ags_automation_editor_copy(AgsAutomationEditor *automation_editor);
void ags_automation_editor_cut(AgsAutomationEditor *automation_editor);
void ags_automation_editor_invert(AgsAutomationEditor *automation_editor);

AgsAutomationEditor* ags_automation_editor_new();

#endif /*__AGS_AUTOMATION_EDITOR_H__*/
