/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2020 Joël Krähemann
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

#ifndef __AGS_COMPOSITE_EDITOR_H__
#define __AGS_COMPOSITE_EDITOR_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <ags/libags-gui.h>

#include <ags/X/ags_machine.h>
#include <ags/X/ags_composite_view.h>

#include <ags/X/editor/ags_toolbar.h>
#include <ags/X/editor/ags_machine_selector.h>

G_BEGIN_DECLS

#define AGS_TYPE_COMPOSITE_EDITOR                (ags_composite_editor_get_type ())
#define AGS_COMPOSITE_EDITOR(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_COMPOSITE_EDITOR, AgsCompositeEditor))
#define AGS_COMPOSITE_EDITOR_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_COMPOSITE_EDITOR, AgsCompositeEditorClass))
#define AGS_IS_COMPOSITE_EDITOR(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_COMPOSITE_EDITOR))
#define AGS_IS_COMPOSITE_EDITOR_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_COMPOSITE_EDITOR))
#define AGS_COMPOSITE_EDITOR_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_COMPOSITE_EDITOR, AgsCompositeEditorClass))
  
#define AGS_COMPOSITE_EDITOR_DEFAULT_VERSION "3.8.0"
#define AGS_COMPOSITE_EDITOR_DEFAULT_BUILD_ID "Mon Dec 14 15:29:38 UTC 2020"

typedef struct _AgsCompositeEditor AgsCompositeEditor;
typedef struct _AgsCompositeEditorClass AgsCompositeEditorClass;

typedef enum{
  AGS_COMPOSITE_EDITOR_ADDED_TO_REGISTRY     = 1,
  AGS_COMPOSITE_EDITOR_CONNECTED             = 1 << 1,
}AgsCompositeEditorFlags;

typedef enum{
  AGS_COMPOSITE_EDITOR_VIEW_NOTATION    = 1,
  AGS_COMPOSITE_EDITOR_VIEW_SHEET       = 1 <<  1,
  AGS_COMPOSITE_EDITOR_VIEW_AUTOMATION  = 1 <<  2,
  AGS_COMPOSITE_EDITOR_VIEW_WAVE        = 1 <<  3,
}AgsCompositeEditorView;

struct _AgsCompositeEditor
{
  GtkBox box;

  guint flags;
  guint view;
  
  gchar *version;
  gchar *build_id;

  AgsUUID *uuid;

  AgsToolbar *toolbar;
  
  GtkHPaned *paned;
  
  AgsMachineSelector *machine_selector;
  AgsMachine *selected_machine;

  AgsCompositeView *selected_view;

  AgsCompositeView *notation_view;
  AgsCompositeView *sheet_view;
  AgsCompositeView *automation_view;
  AgsCompositeView *wave_view;
};

struct _AgsCompositeEditorClass
{
  GtkBoxClass box;

  void (*machine_changed)(AgsCompositeEditor *composite_editor,
			  AgsMachine *machine);
};

GType ags_composite_editor_get_type(void);

gboolean ags_composite_editor_test_flags(AgsCompositeEditor *composite_editor, guint flags);
void ags_composite_editor_set_flags(AgsCompositeEditor *composite_editor, guint flags);
void ags_composite_editor_unset_flags(AgsCompositeEditor *composite_editor, guint flags);

void ags_composite_editor_machine_changed(AgsCompositeEditor *composite_editor,
					  AgsMachine *machine);

/* common */
void ags_composite_editor_paste(AgsCompositeEditor *composite_editor);
void ags_composite_editor_copy(AgsCompositeEditor *composite_editor);
void ags_composite_editor_cut(AgsCompositeEditor *composite_editor);
void ags_composite_editor_invert(AgsCompositeEditor *composite_editor);

/* instantiate */
AgsCompositeEditor* ags_composite_editor_new();

G_END_DECLS

#endif /*__AGS_COMPOSITE_EDITOR_H__*/
