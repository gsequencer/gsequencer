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

#ifndef __AGS_TOOLBAR_H__
#define __AGS_TOOLBAR_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

G_BEGIN_DECLS

#define AGS_TYPE_TOOLBAR                (ags_toolbar_get_type())
#define AGS_TOOLBAR(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_TOOLBAR, AgsToolbar))
#define AGS_TOOLBAR_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_TOOLBAR, AgsToolbarClass))
#define AGS_IS_TOOLBAR(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_TOOLBAR))
#define AGS_IS_TOOLBAR_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_TOOLBAR))
#define AGS_TOOLBAR_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS (obj, AGS_TYPE_TOOLBAR, AgsToolbarClass))

#define AGS_TOOLBAR_DEFAULT_VERSION "3.8.0"
#define AGS_TOOLBAR_DEFAULT_BUILD_ID "Mon Dec 14 15:29:38 UTC 2020"

#define AGS_TOOLBAR_DIALOG_SCOPE_COUNT (5)

#define AGS_TOOLBAR_SCOPE_COMMON "common"
#define AGS_TOOLBAR_SCOPE_NOTATION "notation"
#define AGS_TOOLBAR_SCOPE_SHEET "sheet"
#define AGS_TOOLBAR_SCOPE_AUTOMATION "automation"
#define AGS_TOOLBAR_SCOPE_WAVE "wave"

typedef struct _AgsToolbar AgsToolbar;
typedef struct _AgsToolbarClass AgsToolbarClass;

typedef enum{
  AGS_TOOLBAR_ADDED_TO_REGISTRY     = 1,
  AGS_TOOLBAR_CONNECTED             = 1 << 1,
}AgsToolbarFlags;

typedef enum{
  AGS_TOOLBAR_TOOL_POSITION  = 1,
  AGS_TOOLBAR_TOOL_EDIT      = 1 <<  1,
  AGS_TOOLBAR_TOOL_CLEAR     = 1 <<  2,
  AGS_TOOLBAR_TOOL_SELECT    = 1 <<  3,
}AgsToolbarTool;

typedef enum{
  AGS_TOOLBAR_ACTION_INVERT    = 1,
  AGS_TOOLBAR_ACTION_COPY      = 1 <<  1,
  AGS_TOOLBAR_ACTION_CUT       = 1 <<  2,
  AGS_TOOLBAR_ACTION_PASTE     = 1 <<  3,
}AgsToolbarAction;

typedef enum{
  AGS_TOOLBAR_PASTE_MATCH_AUDIO_CHANNEL  = 1,
  AGS_TOOLBAR_PASTE_MATCH_LINE           = 1 <<  1,
  AGS_TOOLBAR_PASTE_NO_DUPLICATES        = 1 <<  2,
}AgsToolbarPasteMode;

typedef enum{
  AGS_TOOLBAR_HAS_MENU_TOOL      = 1,
  AGS_TOOLBAR_HAS_ZOOM           = 1 <<  1,
  AGS_TOOLBAR_HAS_OPACITY        = 1 <<  2,
  AGS_TOOLBAR_HAS_PORT           = 1 <<  3,
}AgsToolbarOption;

typedef enum{
  AGS_TOOLBAR_ZOOM_4_1,
  AGS_TOOLBAR_ZOOM_2_1,
  AGS_TOOLBAR_ZOOM_1_1,
  AGS_TOOLBAR_ZOOM_1_2,
  AGS_TOOLBAR_ZOOM_1_4,
  AGS_TOOLBAR_ZOOM_1_8,
  AGS_TOOLBAR_ZOOM_1_16,
}AgsToolbarZoom;

typedef enum{
  AGS_TOOLBAR_COMMON_DIALOG_ENABLE_ALL_AUDIO_CHANNELS       = 1,
  AGS_TOOLBAR_COMMON_DIALOG_DISABLE_ALL_AUDIO_CHANNELS      = 1 <<  1,
  AGS_TOOLBAR_COMMON_DIALOG_ENABLE_ALL_LINES                = 1 <<  2,
  AGS_TOOLBAR_COMMON_DIALOG_DISABLE_ALL_LINES               = 1 <<  3,
}AgsToolbarCommonDialog;

typedef enum{
  AGS_TOOLBAR_NOTATION_DIALOG_MOVE_NOTE           = 1,
  AGS_TOOLBAR_NOTATION_DIALOG_CROP_NOTE           = 1 <<  1,
  AGS_TOOLBAR_NOTATION_DIALOG_SELECT_NOTE         = 1 <<  2,
  AGS_TOOLBAR_NOTATION_DIALOG_POSITION_CURSOR     = 1 <<  3,
}AgsToolbarNotationDialog;

typedef enum{
  AGS_TOOLBAR_SHEET_DIALOG_POSITION_CURSOR,
}AgsToolbarSheetDialog;

typedef enum{
  AGS_TOOLBAR_AUTOMATION_DIALOG_SELECT_ACCELERATION   = 1,
  AGS_TOOLBAR_AUTOMATION_DIALOG_RAMP_ACCELERATION     = 1 <<  1,
  AGS_TOOLBAR_AUTOMATION_DIALOG_POSITION_CURSOR       = 1 <<  2,
}AgsToolbarAutomationDialog;

typedef enum{
  AGS_TOOLBAR_WAVE_DIALOG_SELECT_BUFFER       = 1,
  AGS_TOOLBAR_WAVE_DIALOG_POSITION_CURSOR     = 1 <<  1,
}AgsToolbarWaveDialog;

struct _AgsToolbar
{
  GtkToolbar toolbar;

  guint flags;

  guint tool;
  guint action;
  guint option;
  
  gchar *version;
  gchar *build_id;

  AgsUUID *uuid;

  gboolean block_selected_tool;
  GtkToggleToolButton *selected_tool;

  GtkToggleToolButton *position;
  GtkToggleToolButton *edit;
  GtkToggleToolButton *clear;
  GtkToggleToolButton *select;

  GtkToolButton *invert;
  GtkToolButton *copy;
  GtkToolButton *cut;

  guint paste_mode;
  GtkMenuToolButton *paste;
  GtkMenu *paste_popup;

  gchar **menu_tool_dialog;
  GValue *menu_tool_value;
  GtkMenuToolButton *menu_tool;
  GtkMenu *menu_tool_popup;

  GtkComboBox *port;

  gint selected_zoom;
  GtkComboBox *zoom;

  GtkSpinButton *opacity;
  
  GtkDialog *notation_move_note;
  GtkDialog *notation_crop_note;  
  GtkDialog *notation_select_note;
  GtkDialog *notation_position_cursor;
  
  GtkDialog *sheet_position_cursor;
  
  GtkDialog *automation_select_acceleration;
  GtkDialog *automation_ramp_acceleration;
  GtkDialog *automation_position_cursor;

  GtkDialog *wave_select_buffer;
  GtkDialog *wave_position_cursor;
};

struct _AgsToolbarClass
{
  GtkToolbarClass toolbar;
};

GType ags_toolbar_get_type(void);

gboolean ags_toolbar_test_flags(AgsToolbar *toolbar, guint flags);
void ags_toolbar_set_flags(AgsToolbar *toolbar, guint flags);
void ags_toolbar_unset_flags(AgsToolbar *toolbar, guint flags);

gboolean ags_toolbar_test_tool(AgsToolbar *toolbar, guint tool);
void ags_toolbar_set_tool(AgsToolbar *toolbar, guint tool);
void ags_toolbar_unset_tool(AgsToolbar *toolbar, guint tool);

gboolean ags_toolbar_test_action(AgsToolbar *toolbar, guint action);
void ags_toolbar_set_action(AgsToolbar *toolbar, guint action);
void ags_toolbar_unset_action(AgsToolbar *toolbar, guint action);

gboolean ags_toolbar_test_option(AgsToolbar *toolbar, guint option);
void ags_toolbar_set_option(AgsToolbar *toolbar, guint option);
void ags_toolbar_unset_option(AgsToolbar *toolbar, guint option);

GtkMenu* ags_toolbar_paste_popup_new(guint paste_mode);
GtkMenu* ags_toolbar_menu_tool_popup_new(gchar **dialog,
					 GValue *value);

void ags_toolbar_set_selected_tool(AgsToolbar *toolbar,
				   GtkToggleToolButton *selected_tool);

void ags_toolbar_scope_create_and_connect(AgsToolbar *toolbar,
					  gchar *scope);

AgsToolbar* ags_toolbar_new();

G_END_DECLS

#endif /*__AGS_TOOLBAR_H__*/
