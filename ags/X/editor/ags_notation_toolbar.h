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

#ifndef __AGS_NOTATION_TOOLBAR_H__
#define __AGS_NOTATION_TOOLBAR_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

G_BEGIN_DECLS

#define AGS_TYPE_NOTATION_TOOLBAR                (ags_notation_toolbar_get_type())
#define AGS_NOTATION_TOOLBAR(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_NOTATION_TOOLBAR, AgsNotationToolbar))
#define AGS_NOTATION_TOOLBAR_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_NOTATION_TOOLBAR, AgsNotationToolbarClass))
#define AGS_IS_NOTATION_TOOLBAR(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_NOTATION_TOOLBAR))
#define AGS_IS_NOTATION_TOOLBAR_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_NOTATION_TOOLBAR))
#define AGS_NOTATION_TOOLBAR_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS (obj, AGS_TYPE_NOTATION_TOOLBAR, AgsNotationToolbarClass))

typedef struct _AgsNotationToolbar AgsNotationToolbar;
typedef struct _AgsNotationToolbarClass AgsNotationToolbarClass;

typedef enum{
  AGS_NOTATION_TOOLBAR_CONNECTED   = 1,
}AgsNotationToolbarFlags;

struct _AgsNotationToolbar
{
  GtkToolbar toolbar;

  guint flags;
  
  GtkToggleToolButton *selected_edit_mode;
  GtkToggleToolButton *position;
  GtkToggleToolButton *edit;
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
  GtkDialog *position_notation_cursor;
  
  guint zoom_history;
  GtkComboBoxText *zoom;

  GtkSpinButton *opacity;
};

struct _AgsNotationToolbarClass
{
  GtkToolbarClass toolbar;
};

GType ags_notation_toolbar_get_type(void);

GtkMenu* ags_notation_toolbar_tool_popup_new(AgsNotationToolbar *notation_toolbar);

AgsNotationToolbar* ags_notation_toolbar_new();

G_END_DECLS

#endif /*__AGS_NOTATION_TOOLBAR_H__*/
