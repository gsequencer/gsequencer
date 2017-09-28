/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2015 Joël Krähemann
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

#define AGS_TYPE_AUTOMATION_TOOLBAR                (ags_automation_toolbar_get_type())
#define AGS_AUTOMATION_TOOLBAR(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_AUTOMATION_TOOLBAR, AgsAutomationToolbar))
#define AGS_AUTOMATION_TOOLBAR_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_AUTOMATION_TOOLBAR, AgsAutomationToolbarClass))
#define AGS_IS_AUTOMATION_TOOLBAR(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_AUTOMATION_TOOLBAR))
#define AGS_IS_AUTOMATION_TOOLBAR_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_AUTOMATION_TOOLBAR))
#define AGS_AUTOMATION_TOOLBAR_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS (obj, AGS_TYPE_AUTOMATION_TOOLBAR, AgsAutomationToolbarClass))

#define AGS_AUTOMATION_TOOLBAR_DATA_CHANNEL "ags-channel\0"
#define AGS_AUTOMATION_TOOLBAR_DATA_PORT "ags-port\0"

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
  
  GtkToggleButton *selected_edit_mode;
  GtkToggleButton *position;
  GtkToggleButton *edit;
  GtkToggleButton *clear;
  GtkToggleButton *select;

  GtkButton *copy;
  GtkButton *cut;
  GtkButton *paste;

  GtkMenuToolButton *menu_tool;
  GtkMenu *tool_popup;

  GtkDialog *select_acceleration;
  GtkDialog *ramp_acceleration;

  guint zoom_history;
  GtkComboBox *zoom;

  GtkComboBox *port;
};

struct _AgsAutomationToolbarClass
{
  GtkToolbarClass toolbar;
};

GType ags_automation_toolbar_get_type(void);

void ags_automation_toolbar_load_port(AgsAutomationToolbar *automation_toolbar);
void ags_automation_toolbar_apply_port(AgsAutomationToolbar *automation_toolbar,
				       gchar *control_name);

GtkMenu* ags_automation_toolbar_tool_popup_new(GtkToolbar *automation_toolbar);

AgsAutomationToolbar* ags_automation_toolbar_new();

#endif /*__AGS_AUTOMATION_TOOLBAR_H__*/
