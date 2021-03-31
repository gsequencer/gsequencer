/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2021 Joël Krähemann
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

#include <ags/X/ags_machine.h>

#include <ags/X/editor/ags_wave_toolbar.h>
#include <ags/X/editor/ags_machine_selector.h>
#include <ags/X/editor/ags_scrolled_wave_edit_box.h>
#include <ags/X/editor/ags_wave_edit.h>
#include <ags/X/editor/ags_wave_meta.h>

G_BEGIN_DECLS

#define AGS_TYPE_WAVE_EDITOR                (ags_wave_editor_get_type())
#define AGS_WAVE_EDITOR(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_WAVE_EDITOR, AgsWaveEditor))
#define AGS_WAVE_EDITOR_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_WAVE_EDITOR, AgsWaveEditorClass))
#define AGS_IS_WAVE_EDITOR(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_WAVE_EDITOR))
#define AGS_IS_WAVE_EDITOR_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_WAVE_EDITOR))
#define AGS_WAVE_EDITOR_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS (obj, AGS_TYPE_WAVE_EDITOR, AgsWaveEditorClass))

#define AGS_WAVE_EDITOR_MAX_VALUE_COUNT (64 * 16 * 16 * 1200)
#define AGS_WAVE_EDITOR_MAX_CONTROLS (64 * 16 * 16 * 1200)

#define AGS_WAVE_EDITOR_DEFAULT_VERSION "1.4.0"
#define AGS_WAVE_EDITOR_DEFAULT_BUILD_ID "Thu Jan  4 19:47:23 UTC 2018"

typedef struct _AgsWaveEditor AgsWaveEditor;
typedef struct _AgsWaveEditorClass AgsWaveEditorClass;
  
typedef enum{
  AGS_WAVE_EDITOR_CONNECTED                 = 1,
  AGS_WAVE_EDITOR_RESET_HSCROLLBAR          = 1 <<  1,
  AGS_WAVE_EDITOR_PASTE_MATCH_LINE          = 1 <<  2,
  AGS_WAVE_EDITOR_PASTE_REPLACE             = 1 <<  3,
}AgsWaveEditorFlags;

struct _AgsWaveEditor
{
  GtkBox box;
  
  guint flags;
  
  gchar *version;
  gchar *build_id;

  guint tact_counter;
  gdouble current_tact;

  GtkPaned *paned;

  AgsWaveToolbar *wave_toolbar;

  AgsMachineSelector *machine_selector;
  AgsMachine *selected_machine;

  AgsNotebook *notebook;
  AgsRuler *ruler;

  AgsScrolledLevelBox *scrolled_level_box;

  AgsScrolledWaveEditBox *scrolled_wave_edit_box;

  GtkScrollbar *vscrollbar;
  GtkScrollbar *hscrollbar;

  AgsWaveEdit *focused_wave_edit;

  AgsWaveMeta *wave_meta;
};

struct _AgsWaveEditorClass
{
  GtkBoxClass box;

  void (*machine_changed)(AgsWaveEditor *wave_editor,
			  AgsMachine *machine);
};

GType ags_wave_editor_get_type(void);

void ags_wave_editor_reset_scrollbar(AgsWaveEditor *wave_editor);

void ags_wave_editor_reset_audio_scrollbar(AgsWaveEditor *wave_editor);
void ags_wave_editor_reset_output_scrollbar(AgsWaveEditor *wave_editor);
void ags_wave_editor_reset_input_scrollbar(AgsWaveEditor *wave_editor);

void ags_wave_editor_machine_changed(AgsWaveEditor *wave_editor,
				     AgsMachine *machine);

void ags_wave_editor_select_region(AgsWaveEditor *wave_editor,
				   guint x0, gdouble y0,
				   guint x1, gdouble y1);

void ags_wave_editor_select_all(AgsWaveEditor *wave_editor);

void ags_wave_editor_paste(AgsWaveEditor *wave_editor);
void ags_wave_editor_copy(AgsWaveEditor *wave_editor);
void ags_wave_editor_cut(AgsWaveEditor *wave_editor);
void ags_wave_editor_invert(AgsWaveEditor *wave_editor);

AgsWaveEditor* ags_wave_editor_new();

G_END_DECLS

#endif /*__AGS_WAVE_EDITOR_H__*/
