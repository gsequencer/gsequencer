/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2018 Joël Krähemann
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

#ifndef __AGS_SHEET_EDITOR_H__
#define __AGS_SHEET_EDITOR_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#include <ags/X/ags_machine.h>

#include <ags/X/editor/ags_notation_toolbar.h>
#include <ags/X/editor/ags_machine_selector.h>
//#include <ags/X/editor/ags_notation_page.h>

#define AGS_TYPE_SHEET_EDITOR                (ags_sheet_editor_get_type ())
#define AGS_SHEET_EDITOR(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_SHEET_EDITOR, AgsSheetEditor))
#define AGS_SHEET_EDITOR_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_SHEET_EDITOR, AgsSheetEditorClass))
#define AGS_IS_SHEET_EDITOR(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_SHEET_EDITOR))
#define AGS_IS_SHEET_EDITOR_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_SHEET_EDITOR))
#define AGS_SHEET_EDITOR_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_SHEET_EDITOR, AgsSheetEditorClass))
  
#define AGS_SHEET_EDITOR_DEFAULT_VERSION "2.0.0"
#define AGS_SHEET_EDITOR_DEFAULT_BUILD_ID "Sat Feb  3 16:15:50 UTC 2018"

#define AGS_SHEET_EDITOR_MAX_CONTROLS (16 * 16 * 1200)

typedef struct _AgsSheetEditor AgsSheetEditor;
typedef struct _AgsSheetEditorClass AgsSheetEditorClass;

typedef enum{
  AGS_SHEET_EDITOR_CONNECTED                    = 1,
  AGS_SHEET_EDITOR_PATTERN_MODE                 = 1 <<  1,
  AGS_SHEET_EDITOR_PASTE_MATCH_AUDIO_CHANNEL    = 1 <<  2,
  AGS_SHEET_EDITOR_PASTE_NO_DUPLICATES          = 1 <<  3,
}AgsSheetEditorFlags;

struct _AgsSheetEditor
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
  
  //  AgsNotationPage *notation_page;
};

struct _AgsSheetEditorClass
{
  GtkVBoxClass vbox;

  void (*machine_changed)(AgsSheetEditor *sheet_editor, AgsMachine *machine);
};

GType ags_sheet_editor_get_type(void);

void ags_sheet_editor_machine_changed(AgsSheetEditor *sheet_editor,
					AgsMachine *machine);

AgsSheetEditor* ags_sheet_editor_new();

#endif /*__AGS_SHEET_EDITOR_H__*/
