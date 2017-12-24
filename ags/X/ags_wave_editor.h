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

#ifndef __AGS_WAVE_EDITOR_H__
#define __AGS_WAVE_EDITOR_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#include <ags/X/editor/ags_wave_toolbar.h>
#include <ags/X/editor/ags_machine_selector.h>
#include <ags/X/editor/ags_level.h>
#include <ags/X/editor/ags_wave_edit.h>

#define AGS_TYPE_WAVE_EDITOR                (ags_wave_editor_get_type())
#define AGS_WAVE_EDITOR(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_WAVE_EDITOR, AgsWaveEditor))
#define AGS_WAVE_EDITOR_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_WAVE_EDITOR, AgsWaveEditorClass))
#define AGS_IS_WAVE_EDITOR(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_WAVE_EDITOR))
#define AGS_IS_WAVE_EDITOR_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_WAVE_EDITOR))
#define AGS_WAVE_EDITOR_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS (obj, AGS_TYPE_WAVE_EDITOR, AgsWaveEditorClass))

#define AGS_WAVE_EDITOR_CHILD(ptr) ((AgsWaveEditorChild *)(ptr))

#define AGS_WAVE_EDITOR_DEFAULT_VERSION "1.2.0"
#define AGS_WAVE_EDITOR_DEFAULT_BUILD_ID "Mon Nov 20 07:28:21 UTC 2017"

typedef struct _AgsWaveEditor AgsWaveEditor;
typedef struct _AgsWaveEditorClass AgsWaveEditorClass;
  
typedef enum{
  AGS_WAVE_EDITOR_CONNECTED    = 1,
}AgsWaveEditorFlags;

struct _AgsWaveEditor
{
  GtkVBox vbox;

  guint flags;
  
  gchar *version;
  gchar *build_id;

  GObject *soundcard;
  
  AgsMachineSelector *machine_selector;
  AgsMachine *selected_machine;

  AgsWaveToolbar *wave_toolbar;

  GtkTable *table;

  AgsNotebook *notebook;
  AgsLevel *level;
  GtkWidget *wave_edit;

  guint tact_counter;
  gdouble current_tact;
};

struct _AgsWaveEditorClass
{
  GtkVBoxClass vbox;

  void (*machine_changed)(AgsWaveEditor *wave_editor,
			  AgsMachine *machine);
};

GType ags_wave_editor_get_type(void);

void ags_wave_editor_machine_changed(AgsWaveEditor *wave_editor,
					   AgsMachine *machine);

void ags_wave_editor_select_all(AgsWaveEditor *wave_editor);

void ags_wave_editor_paste(AgsWaveEditor *wave_editor);
void ags_wave_editor_copy(AgsWaveEditor *wave_editor);
void ags_wave_editor_cut(AgsWaveEditor *wave_editor);
void ags_wave_editor_invert(AgsWaveEditor *wave_editor);

AgsWaveEditor* ags_wave_editor_new();

#endif /*__AGS_WAVE_EDITOR_H__*/
