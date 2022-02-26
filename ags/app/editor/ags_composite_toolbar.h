/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2022 Joël Krähemann
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

#ifndef __AGS_COMPOSITE_TOOLBAR_H__
#define __AGS_COMPOSITE_TOOLBAR_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <ags/libags-gui.h>

G_BEGIN_DECLS

#define AGS_TYPE_COMPOSITE_TOOLBAR                (ags_composite_toolbar_get_type())
#define AGS_COMPOSITE_TOOLBAR(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_COMPOSITE_TOOLBAR, AgsCompositeToolbar))
#define AGS_COMPOSITE_TOOLBAR_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_COMPOSITE_TOOLBAR, AgsCompositeToolbarClass))
#define AGS_IS_COMPOSITE_TOOLBAR(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_COMPOSITE_TOOLBAR))
#define AGS_IS_COMPOSITE_TOOLBAR_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_COMPOSITE_TOOLBAR))
#define AGS_COMPOSITE_TOOLBAR_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS (obj, AGS_TYPE_COMPOSITE_TOOLBAR, AgsCompositeToolbarClass))

#define AGS_COMPOSITE_TOOLBAR_DEFAULT_VERSION "4.0.0"
#define AGS_COMPOSITE_TOOLBAR_DEFAULT_BUILD_ID "Sat Feb 26 21:09:50 UTC 2022"

#define AGS_COMPOSITE_TOOLBAR_DIALOG_SCOPE_COUNT (5)

#define AGS_COMPOSITE_TOOLBAR_SCOPE_COMMON "common"
#define AGS_COMPOSITE_TOOLBAR_SCOPE_NOTATION "notation"
#define AGS_COMPOSITE_TOOLBAR_SCOPE_SHEET "sheet"
#define AGS_COMPOSITE_TOOLBAR_SCOPE_AUTOMATION "automation"
#define AGS_COMPOSITE_TOOLBAR_SCOPE_WAVE "wave"

#define AGS_COMPOSITE_TOOLBAR_TOOL_PADDING (6)

typedef struct _AgsCompositeToolbar AgsCompositeToolbar;
typedef struct _AgsCompositeToolbarClass AgsCompositeToolbarClass;

typedef enum{
  AGS_COMPOSITE_TOOLBAR_ADDED_TO_REGISTRY     = 1,
  AGS_COMPOSITE_TOOLBAR_CONNECTED             = 1 << 1,
}AgsCompositeToolbarFlags;

typedef enum{
  AGS_COMPOSITE_TOOLBAR_TOOL_POSITION  = 1,
  AGS_COMPOSITE_TOOLBAR_TOOL_EDIT      = 1 <<  1,
  AGS_COMPOSITE_TOOLBAR_TOOL_CLEAR     = 1 <<  2,
  AGS_COMPOSITE_TOOLBAR_TOOL_SELECT    = 1 <<  3,
}AgsCompositeToolbarTool;

typedef enum{
  AGS_COMPOSITE_TOOLBAR_ACTION_INVERT    = 1,
  AGS_COMPOSITE_TOOLBAR_ACTION_COPY      = 1 <<  1,
  AGS_COMPOSITE_TOOLBAR_ACTION_CUT       = 1 <<  2,
  AGS_COMPOSITE_TOOLBAR_ACTION_PASTE     = 1 <<  3,
}AgsCompositeToolbarAction;

typedef enum{
  AGS_COMPOSITE_TOOLBAR_PASTE_MATCH_AUDIO_CHANNEL  = 1,
  AGS_COMPOSITE_TOOLBAR_PASTE_MATCH_LINE           = 1 <<  1,
  AGS_COMPOSITE_TOOLBAR_PASTE_NO_DUPLICATES        = 1 <<  2,
}AgsCompositeToolbarPasteMode;

typedef enum{
  AGS_COMPOSITE_TOOLBAR_HAS_MENU_TOOL      = 1,
  AGS_COMPOSITE_TOOLBAR_HAS_ZOOM           = 1 <<  1,
  AGS_COMPOSITE_TOOLBAR_HAS_OPACITY        = 1 <<  2,
  AGS_COMPOSITE_TOOLBAR_HAS_PORT           = 1 <<  3,
  AGS_COMPOSITE_TOOLBAR_HAS_BEATS          = 1 <<  4,
  AGS_COMPOSITE_TOOLBAR_HAS_BEATS_TYPE     = 1 <<  5,
}AgsCompositeToolbarOption;

typedef enum{
  AGS_COMPOSITE_TOOLBAR_ZOOM_4_1,
  AGS_COMPOSITE_TOOLBAR_ZOOM_2_1,
  AGS_COMPOSITE_TOOLBAR_ZOOM_1_1,
  AGS_COMPOSITE_TOOLBAR_ZOOM_1_2,
  AGS_COMPOSITE_TOOLBAR_ZOOM_1_4,
  AGS_COMPOSITE_TOOLBAR_ZOOM_1_8,
  AGS_COMPOSITE_TOOLBAR_ZOOM_1_16,
}AgsCompositeToolbarZoom;

typedef enum{
  AGS_COMPOSITE_TOOLBAR_COMMON_DIALOG_ENABLE_ALL_AUDIO_CHANNELS       = 1,
  AGS_COMPOSITE_TOOLBAR_COMMON_DIALOG_DISABLE_ALL_AUDIO_CHANNELS      = 1 <<  1,
  AGS_COMPOSITE_TOOLBAR_COMMON_DIALOG_ENABLE_ALL_LINES                = 1 <<  2,
  AGS_COMPOSITE_TOOLBAR_COMMON_DIALOG_DISABLE_ALL_LINES               = 1 <<  3,
}AgsCompositeToolbarCommonDialog;

typedef enum{
  AGS_COMPOSITE_TOOLBAR_NOTATION_DIALOG_MOVE_NOTE           = 1,
  AGS_COMPOSITE_TOOLBAR_NOTATION_DIALOG_CROP_NOTE           = 1 <<  1,
  AGS_COMPOSITE_TOOLBAR_NOTATION_DIALOG_SELECT_NOTE         = 1 <<  2,
  AGS_COMPOSITE_TOOLBAR_NOTATION_DIALOG_POSITION_CURSOR     = 1 <<  3,
}AgsCompositeToolbarNotationDialog;

typedef enum{
  AGS_COMPOSITE_TOOLBAR_SHEET_DIALOG_POSITION_CURSOR       = 1,
  AGS_COMPOSITE_TOOLBAR_SHEET_DIALOG_ADD_PAGE              = 1 <<  1,
  AGS_COMPOSITE_TOOLBAR_SHEET_DIALOG_REMOVE_PAGE           = 1 <<  2,  
}AgsCompositeToolbarSheetDialog;

typedef enum{
  AGS_COMPOSITE_TOOLBAR_AUTOMATION_DIALOG_SELECT_ACCELERATION   = 1,
  AGS_COMPOSITE_TOOLBAR_AUTOMATION_DIALOG_RAMP_ACCELERATION     = 1 <<  1,
  AGS_COMPOSITE_TOOLBAR_AUTOMATION_DIALOG_POSITION_CURSOR       = 1 <<  2,
}AgsCompositeToolbarAutomationDialog;

typedef enum{
  AGS_COMPOSITE_TOOLBAR_WAVE_DIALOG_SELECT_BUFFER       = 1,
  AGS_COMPOSITE_TOOLBAR_WAVE_DIALOG_POSITION_CURSOR     = 1 <<  1,
  AGS_COMPOSITE_TOOLBAR_WAVE_DIALOG_TIME_STRETCH_BUFFER = 1 <<  2,
}AgsCompositeToolbarWaveDialog;

struct _AgsCompositeToolbar
{
  GtkBox box;

  guint flags;

  guint tool;
  guint action;
  guint option;
  
  gchar *version;
  gchar *build_id;

  AgsUUID *uuid;

  gboolean block_selected_tool;
  GtkButton *selected_tool;

  GtkToggleButton *position;
  GtkToggleButton *edit;
  GtkToggleButton *clear;
  GtkToggleButton *select;

  GtkButton *invert;
  GtkButton *copy;
  GtkButton *cut;

  guint paste_mode;
  GtkMenuButton *paste;
  GMenu *paste_popup;

  gchar **menu_tool_dialog;
  GValue *menu_tool_value;
  GtkMenuButton *menu_tool;
  GMenu *menu_tool_popup;

  GtkComboBox *beats;
  GtkComboBox *beats_type;
  GtkComboBox *note;
  
  GtkComboBox *port;

  gint selected_zoom;
  GtkComboBox *zoom;

  GtkSpinButton *opacity;
  
  GtkDialog *notation_move_note;
  GtkDialog *notation_crop_note;  
  GtkDialog *notation_select_note;
  GtkDialog *notation_position_cursor;
  
  GtkDialog *sheet_position_cursor;
  GtkDialog *sheet_add_page;
  GtkDialog *sheet_remove_page;
  
  GtkDialog *automation_select_acceleration;
  GtkDialog *automation_ramp_acceleration;
  GtkDialog *automation_position_cursor;

  GtkDialog *wave_select_buffer;
  GtkDialog *wave_position_cursor;
  GtkDialog *wave_time_stretch_buffer;
};

struct _AgsCompositeToolbarClass
{
  GtkBoxClass box;
};

GType ags_composite_toolbar_get_type(void);

gboolean ags_composite_toolbar_test_flags(AgsCompositeToolbar *composite_toolbar, guint flags);
void ags_composite_toolbar_set_flags(AgsCompositeToolbar *composite_toolbar, guint flags);
void ags_composite_toolbar_unset_flags(AgsCompositeToolbar *composite_toolbar, guint flags);

gboolean ags_composite_toolbar_test_tool(AgsCompositeToolbar *composite_toolbar, guint tool);
void ags_composite_toolbar_set_tool(AgsCompositeToolbar *composite_toolbar, guint tool);
void ags_composite_toolbar_unset_tool(AgsCompositeToolbar *composite_toolbar, guint tool);

gboolean ags_composite_toolbar_test_action(AgsCompositeToolbar *composite_toolbar, guint action);
void ags_composite_toolbar_set_action(AgsCompositeToolbar *composite_toolbar, guint action);
void ags_composite_toolbar_unset_action(AgsCompositeToolbar *composite_toolbar, guint action);

gboolean ags_composite_toolbar_test_option(AgsCompositeToolbar *composite_toolbar, guint option);
void ags_composite_toolbar_set_option(AgsCompositeToolbar *composite_toolbar, guint option);
void ags_composite_toolbar_unset_option(AgsCompositeToolbar *composite_toolbar, guint option);

GMenu* ags_composite_toolbar_paste_popup_new(guint paste_mode);
GMenu* ags_composite_toolbar_menu_tool_popup_new(gchar **dialog,
						 GValue *value);

void ags_composite_toolbar_load_port(AgsCompositeToolbar *composite_toolbar);

void ags_composite_toolbar_set_selected_tool(AgsCompositeToolbar *composite_toolbar,
					     GtkToggleButton *selected_tool);

void ags_composite_toolbar_scope_create_and_connect(AgsCompositeToolbar *composite_toolbar,
						    gchar *scope);

AgsCompositeToolbar* ags_composite_toolbar_new();

G_END_DECLS

#endif /*__AGS_COMPOSITE_TOOLBAR_H__*/
