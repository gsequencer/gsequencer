/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
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

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#include <ags/X/ags_machine.h>

#include <ags/X/editor/ags_automation_toolbar.h>
#include <ags/X/editor/ags_machine_selector.h>
#include <ags/X/editor/ags_scrolled_automation_edit_box.h>
#include <ags/X/editor/ags_automation_edit.h>
#include <ags/X/editor/ags_automation_meta.h>

G_BEGIN_DECLS

#define AGS_TYPE_AUTOMATION_EDITOR                (ags_automation_editor_get_type())
#define AGS_AUTOMATION_EDITOR(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_AUTOMATION_EDITOR, AgsAutomationEditor))
#define AGS_AUTOMATION_EDITOR_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_AUTOMATION_EDITOR, AgsAutomationEditorClass))
#define AGS_IS_AUTOMATION_EDITOR(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_AUTOMATION_EDITOR))
#define AGS_IS_AUTOMATION_EDITOR_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_AUTOMATION_EDITOR))
#define AGS_AUTOMATION_EDITOR_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS (obj, AGS_TYPE_AUTOMATION_EDITOR, AgsAutomationEditorClass))

#define AGS_AUTOMATION_EDITOR_CHILD(ptr) ((AgsAutomationEditorChild *)(ptr))

#define AGS_AUTOMATION_EDITOR_MAX_VALUE_COUNT (64 * 16 * 16 * 1200)
#define AGS_AUTOMATION_EDITOR_MAX_CONTROLS (64 * 16 * 16 * 1200)

#define AGS_AUTOMATION_EDITOR_DEFAULT_VERSION "1.3.0"
#define AGS_AUTOMATION_EDITOR_DEFAULT_BUILD_ID "Wed Dec 27 03:43:55 UTC 2017"

typedef struct _AgsAutomationEditor AgsAutomationEditor;
typedef struct _AgsAutomationEditorClass AgsAutomationEditorClass;
  
typedef enum{
  AGS_AUTOMATION_EDITOR_CONNECTED                 = 1,
  AGS_AUTOMATION_EDITOR_RESET_AUDIO_HSCROLLBAR    = 1 <<  1,
  AGS_AUTOMATION_EDITOR_RESET_OUTPUT_HSCROLLBAR   = 1 <<  2,
  AGS_AUTOMATION_EDITOR_RESET_INPUT_HSCROLLBAR    = 1 <<  3,
  AGS_AUTOMATION_EDITOR_PASTE_MATCH_LINE          = 1 <<  4,
  AGS_AUTOMATION_EDITOR_PASTE_NO_DUPLICATES       = 1 <<  5,
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

  GtkHPaned *paned;
  
  AgsMachineSelector *machine_selector;
  AgsMachine *selected_machine;

  AgsAutomationToolbar *automation_toolbar;

  GtkNotebook *notebook;

  AgsRuler *audio_ruler;
  AgsScrolledScaleBox *audio_scrolled_scale_box;
  AgsScrolledAutomationEditBox *audio_scrolled_automation_edit_box;
  GtkVScrollbar *audio_vscrollbar;
  GtkHScrollbar *audio_hscrollbar;
  
  AgsNotebook *output_notebook;
  AgsRuler *output_ruler;
  AgsScrolledScaleBox *output_scrolled_scale_box;
  AgsScrolledAutomationEditBox *output_scrolled_automation_edit_box;
  GtkVScrollbar *output_vscrollbar;
  GtkHScrollbar *output_hscrollbar;

  AgsNotebook *input_notebook;
  AgsRuler *input_ruler;
  AgsScrolledScaleBox *input_scrolled_scale_box;
  AgsScrolledAutomationEditBox *input_scrolled_automation_edit_box;
  GtkVScrollbar *input_vscrollbar;
  GtkHScrollbar *input_hscrollbar;

  AgsAutomationEdit *focused_automation_edit;

  AgsAutomationMeta *automation_meta;
};

struct _AgsAutomationEditorClass
{
  GtkVBoxClass vbox;

  void (*machine_changed)(AgsAutomationEditor *automation_editor,
			  AgsMachine *machine);
};

GType ags_automation_editor_get_type(void);

void ags_automation_editor_reset_audio_scrollbar(AgsAutomationEditor *automation_editor);
void ags_automation_editor_reset_output_scrollbar(AgsAutomationEditor *automation_editor);
void ags_automation_editor_reset_input_scrollbar(AgsAutomationEditor *automation_editor);

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

G_END_DECLS

#endif /*__AGS_AUTOMATION_EDITOR_H__*/
