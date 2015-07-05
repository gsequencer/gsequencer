/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2005-2011 Joël Krähemann
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

#ifndef __AGS_EDITOR_H__
#define __AGS_EDITOR_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/object/ags_soundcard.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_note.h>

#include <ags/X/ags_machine.h>

#include <ags/X/editor/ags_toolbar.h>
#include <ags/X/editor/ags_machine_selector.h>
#include <ags/X/editor/ags_notebook.h>
#include <ags/X/editor/ags_piano.h>
#include <ags/X/editor/ags_soundset.h>
#include <ags/X/editor/ags_note_edit.h>
#include <ags/X/editor/ags_pattern_edit.h>
#include <ags/X/editor/ags_automation_edit.h>

#define AGS_TYPE_EDITOR                (ags_editor_get_type ())
#define AGS_EDITOR(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_EDITOR, AgsEditor))
#define AGS_EDITOR_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_EDITOR, AgsEditorClass))
#define AGS_IS_EDITOR(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_EDITOR))
#define AGS_IS_EDITOR_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_EDITOR))
#define AGS_EDITOR_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_EDITOR, AgsEditorClass))

#define AGS_EDITOR_DEFAULT_VERSION "0.4.2\0"
#define AGS_EDITOR_DEFAULT_BUILD_ID "CEST 02-10-2014 19:36\0"

#define AGS_EDITOR_DEFAULT "default\0"

typedef struct _AgsEditor AgsEditor;
typedef struct _AgsEditorClass AgsEditorClass;

typedef enum{
  AGS_EDITOR_TOOL_NOTE_EDIT        = 1,
  AGS_EDITOR_TOOL_PATTERN_EDIT     = 1 << 1,
  AGS_EDITOR_TOOL_AUTOMATION_EDIT  = 1 << 2,
};

struct _AgsEditor
{
  GtkVBox vbox;

  guint flags;

  gchar *version;
  gchar *build_id;

  GObject *soundcard;

  GtkTable *table;

  AgsMachineSelector *machine_selector;
  AgsMachine *selected_machine;
  gulong set_audio_channels_handler;
  gulong set_pads_handler;

  AgsToolbar *toolbar;

  AgsNotebook *notebook;

  union AgsMeter{
    AgsPiano *piano;
    AgsSoundset *soundset;
  }meter;

  union AgsEdit{
    AgsNoteEdit *note_edit;
    AgsPatternEdit *pattern_edit;
  }edit;

  guint tact_counter;
};

struct _AgsEditorClass
{
  GtkVBoxClass vbox;

  void (*machine_changed)(AgsEditor *editor,
			  AgsMachine *machine);
};

GType ags_editor_get_type(void);

void ags_editor_machine_changed(AgsEditor *editor,
				AgsMachine *machine);

AgsEditor* ags_editor_new();

#endif /*__AGS_EDITOR_H__*/
