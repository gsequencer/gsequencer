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

#ifndef __AGS_SHEET_TOOLBAR_H__
#define __AGS_SHEET_TOOLBAR_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

G_BEGIN_DECLS

#define AGS_TYPE_SHEET_TOOLBAR                (ags_sheet_toolbar_get_type())
#define AGS_SHEET_TOOLBAR(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_SHEET_TOOLBAR, AgsSheetToolbar))
#define AGS_SHEET_TOOLBAR_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_SHEET_TOOLBAR, AgsSheetToolbarClass))
#define AGS_IS_SHEET_TOOLBAR(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_SHEET_TOOLBAR))
#define AGS_IS_SHEET_TOOLBAR_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_SHEET_TOOLBAR))
#define AGS_SHEET_TOOLBAR_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS (obj, AGS_TYPE_SHEET_TOOLBAR, AgsSheetToolbarClass))

typedef struct _AgsSheetToolbar AgsSheetToolbar;
typedef struct _AgsSheetToolbarClass AgsSheetToolbarClass;

typedef enum{
  AGS_SHEET_TOOLBAR_CONNECTED   = 1,
}AgsSheetToolbarFlags;

struct _AgsSheetToolbar
{
  GtkToolbar toolbar;

  guint flags;
  
  GtkToggleToolButton *position;
  GtkMenuToolButton *edit_tool;
  GtkToggleToolButton *clear;
  GtkToggleToolButton *select;

  GtkToolButton *copy;
  GtkToolButton *cut;
  GtkMenuToolButton *paste_tool;
  GtkButton *invert;

  GtkMenuToolButton *menu_tool;
  GtkMenu *tool_popup;

  GtkDialog *move_note;
  GtkDialog *crop_note;  
  GtkDialog *select_note;
  GtkDialog *position_sheet_cursor;
  
  guint zoom_history;
  GtkComboBoxText *zoom;

  GtkSpinButton *opacity;
};

struct _AgsSheetToolbarClass
{
  GtkToolbarClass toolbar;
};

GType ags_sheet_toolbar_get_type(void);

AgsSheetToolbar* ags_sheet_toolbar_new();

G_END_DECLS

#endif /*__AGS_SHEET_TOOLBAR_H__*/
