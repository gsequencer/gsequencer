/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2017 Joël Krähemann
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

#include <ags/libags-gui.h>

#include <ags/X/ags_machine.h>

#include <ags/X/editor/ags_automation_toolbar.h>
#include <ags/X/editor/ags_machine_selector.h>
#include <ags/X/editor/ags_automation_edit.h>

#define AGS_TYPE_AUTOMATION_EDITOR                (ags_automation_editor_get_type())
#define AGS_AUTOMATION_EDITOR(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_AUTOMATION_EDITOR, AgsAutomationEditor))
#define AGS_AUTOMATION_EDITOR_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_AUTOMATION_EDITOR, AgsAutomationEditorClass))
#define AGS_IS_AUTOMATION_EDITOR(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_AUTOMATION_EDITOR))
#define AGS_IS_AUTOMATION_EDITOR_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_AUTOMATION_EDITOR))
#define AGS_AUTOMATION_EDITOR_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS (obj, AGS_TYPE_AUTOMATION_EDITOR, AgsAutomationEditorClass))

#define AGS_AUTOMATION_EDITOR_CHILD(ptr) ((AgsAutomationEditorChild *)(ptr))

#define AGS_NOTATION_EDITOR_MAX_VALUE_COUNT (64 * 16 * 16 * 1200)

#define AGS_AUTOMATION_EDITOR_DEFAULT_VERSION "1.3.0"
#define AGS_AUTOMATION_EDITOR_DEFAULT_BUILD_ID "Wed Dec 27 03:43:55 UTC 2017"

typedef struct _AgsAutomationEditor AgsAutomationEditor;
typedef struct _AgsAutomationEditorClass AgsAutomationEditorClass;
  
typedef enum{
  AGS_AUTOMATION_EDITOR_CONNECTED    = 1,
}AgsAutomationEditorFlags;

struct _AgsAutomationEditor
{
  GtkVBox vbox;

  guint flags;
  
  gchar *version;
  gchar *build_id;

  guint tact_counter;
  gdouble current_tact;

  guint chunk_duration;

  GObject *soundcard;

  GtkHPaned *paned;
  
  AgsMachineSelector *machine_selector;
  AgsMachine *selected_machine;

  AgsAutomationToolbar *automation_toolbar;

  GtkNotebook *notebook;
  
  AgsScrolledScaleBox *audio_scrolled_scale_box;
  AgsAutomationEditBox *audio_automation_edit_box;

  AgsNotebook *output_notebook;
  AgsScrolledScaleBox *output_scrolled_scale_box;
  AgsAutomationEditBox *output_automation_edit_box;

  AgsNotebook *input_notebook;
  AgsScrolledScaleBox *input_scrolled_scale_box;
  AgsAutomationEditBox *input_automation_edit_box;
};

struct _AgsAutomationEditorClass
{
  GtkVBoxClass vbox;

  void (*machine_changed)(AgsAutomationEditor *automation_editor,
			  AgsMachine *machine);
};

GType ags_automation_editor_get_type(void);

void ags_automation_editor_machine_changed(AgsAutomationEditor *automation_editor,
					   AgsMachine *machine);

void ags_automation_editor_add_acceleration(AgsAutomationEditor *automation_editor,
					    AgsAcceleration *acceleration);

void ags_automation_editor_delete_acceleration(AgsAutomationEditor *automation_editor,
					       guint x, gdouble y);

void ags_automation_editor_select_region(AgsAutomationEditor *automation_editor,
					 guint x0, gdouble y0,
					 guint x1, gdouble y1);

void ags_automation_editor_select_all(AgsAutomationEditor *automation_editor);

void ags_automation_editor_paste(AgsAutomationEditor *automation_editor);
void ags_automation_editor_copy(AgsAutomationEditor *automation_editor);
void ags_automation_editor_cut(AgsAutomationEditor *automation_editor);
void ags_automation_editor_invert(AgsAutomationEditor *automation_editor);

AgsAutomationEditor* ags_automation_editor_new();

#endif /*__AGS_AUTOMATION_EDITOR_H__*/
