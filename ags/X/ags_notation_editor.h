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

#ifndef __AGS_NOTATION_EDITOR_H__
#define __AGS_NOTATION_EDITOR_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#include <ags/X/ags_machine.h>

#include <ags/X/editor/ags_notation_toolbar.h>
#include <ags/X/editor/ags_machine_selector.h>
#include <ags/X/editor/ags_notation_edit.h>

#define AGS_TYPE_NOTATION_EDITOR                (ags_notation_editor_get_type ())
#define AGS_NOTATION_EDITOR(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_NOTATION_EDITOR, AgsNotationEditor))
#define AGS_NOTATION_EDITOR_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_NOTATION_EDITOR, AgsNotationEditorClass))
#define AGS_IS_NOTATION_EDITOR(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_NOTATION_EDITOR))
#define AGS_IS_NOTATION_EDITOR_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_NOTATION_EDITOR))
#define AGS_NOTATION_EDITOR_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_NOTATION_EDITOR, AgsNotationEditorClass))
  
#define AGS_NOTATION_EDITOR_DEFAULT_VERSION "1.2.0"
#define AGS_NOTATION_EDITOR_DEFAULT_BUILD_ID "Mon Nov 27 08:23:16 UTC 2017"

#define AGS_NOTATION_EDITOR_MAX_CONTROLS (16 * 16 * 1200)

typedef struct _AgsNotationEditor AgsNotationEditor;
typedef struct _AgsNotationEditorClass AgsNotationEditorClass;

typedef enum{
  AGS_NOTATION_EDITOR_CONNECTED        = 1,
  AGS_NOTATION_EDITOR_PATTERN_MODE     = 1 <<  1,
}AgsNotationEditorFlags;

struct _AgsNotationEditor
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

  AgsNotationToolbar *notation_toolbar;
  
  AgsMachineSelector *machine_selector;
  AgsMachine *selected_machine;
  
  AgsNotebook *notebook;

  AgsScrolledPiano *scrolled_piano;
  
  AgsNotationEdit *notation_edit;
};

struct _AgsNotationEditorClass
{
  GtkVBoxClass vbox;

  void (*machine_changed)(AgsNotationEditor *notation_editor, AgsMachine *machine);
};

GType ags_notation_editor_get_type(void);

void ags_notation_editor_machine_changed(AgsNotationEditor *notation_editor,
					 AgsMachine *machine);

void ags_notation_editor_add_note(AgsNotationEditor *notation_editor,
				  AgsNote *note);

void ags_notation_editor_delete_note(AgsNotationEditor *notation_editor,
				     guint x, guint y);

void ags_notation_editor_select_region(AgsNotationEditor *notation_editor,
				       guint x0, guint y0,
				       guint x1, guint y1);

void ags_notation_editor_do_feedback(AgsNotationEditor *notation_editor);

void ags_notation_editor_select_all(AgsNotationEditor *notation_editor);

void ags_notation_editor_paste(AgsNotationEditor *notation_editor);
void ags_notation_editor_copy(AgsNotationEditor *notation_editor);
void ags_notation_editor_cut(AgsNotationEditor *notation_editor);
void ags_notation_editor_invert(AgsNotationEditor *notation_editor);

AgsNotationEditor* ags_notation_editor_new();

#endif /*__AGS_NOTATION_EDITOR_H__*/
