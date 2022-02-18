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

#ifndef __AGS_AUTOMATION_TOOLBAR_H__
#define __AGS_AUTOMATION_TOOLBAR_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

G_BEGIN_DECLS

#define AGS_TYPE_AUTOMATION_TOOLBAR                (ags_automation_toolbar_get_type())
#define AGS_AUTOMATION_TOOLBAR(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_AUTOMATION_TOOLBAR, AgsAutomationToolbar))
#define AGS_AUTOMATION_TOOLBAR_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_AUTOMATION_TOOLBAR, AgsAutomationToolbarClass))
#define AGS_IS_AUTOMATION_TOOLBAR(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_AUTOMATION_TOOLBAR))
#define AGS_IS_AUTOMATION_TOOLBAR_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_AUTOMATION_TOOLBAR))
#define AGS_AUTOMATION_TOOLBAR_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS (obj, AGS_TYPE_AUTOMATION_TOOLBAR, AgsAutomationToolbarClass))

#define AGS_AUTOMATION_TOOLBAR_DATA_CHANNEL "ags-channel"
#define AGS_AUTOMATION_TOOLBAR_DATA_PORT "ags-port"

typedef struct _AgsAutomationToolbar AgsAutomationToolbar;
typedef struct _AgsAutomationToolbarClass AgsAutomationToolbarClass;

typedef enum{
  AGS_AUTOMATION_TOOLBAR_CONNECTED    = 1,
  AGS_AUTOMATION_TOOLBAR_RESET_PORT   = 1 << 1,
}AgsAutomationToolbarFlags;

struct _AgsAutomationToolbar
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

  GtkMenuToolButton *menu_tool;
  GtkMenu *tool_popup;

  GtkDialog *select_acceleration;
  GtkDialog *ramp_acceleration;
  GtkDialog *position_automation_cursor;

  guint zoom_history;
  GtkComboBox *zoom;

  GtkComboBox *port;

  GtkSpinButton *opacity;
};

struct _AgsAutomationToolbarClass
{
  GtkToolbarClass toolbar;
};

GType ags_automation_toolbar_get_type(void);

void ags_automation_toolbar_load_port(AgsAutomationToolbar *automation_toolbar);
void ags_automation_toolbar_apply_port(AgsAutomationToolbar *automation_toolbar,
				       GType channel_type, gchar *control_name);

GtkMenu* ags_automation_toolbar_tool_popup_new(AgsAutomationToolbar *automation_toolbar);

AgsAutomationToolbar* ags_automation_toolbar_new();

G_END_DECLS

#endif /*__AGS_AUTOMATION_TOOLBAR_H__*/
