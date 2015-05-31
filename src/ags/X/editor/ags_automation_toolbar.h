/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2014 Joël Krähemann
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

#ifndef __AGS_AUTOMATION_TOOLBAR_H__
#define __AGS_AUTOMATION_TOOLBAR_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include <ags/X/editor/ags_port_selection.h>

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

struct _AgsAutomationToolbar
{
  GtkToolbar toolbar;

  GtkToggleButton *selected_edit_mode;
  GtkToggleButton *position;
  GtkToggleButton *edit;
  GtkToggleButton *clear;
  GtkToggleButton *select;

  GtkMenuItem *copy;
  GtkMenuItem *cut;
  GtkMenuItem *paste;

  GtkComboBox *zoom;

  GtkComboBox *port;
};

struct _AgsAutomationToolbarClass
{
  GtkToolbarClass toolbar;
};

GType ags_automation_toolbar_get_type(void);

void ags_automation_toolbar_load_port(AgsAutomationToolbar *automation_toolbar);
void ags_automation_toolbar_apply_port(AgsAutomationToolbar *automation_toolbar);

AgsAutomationToolbar* ags_automation_toolbar_new();

#endif /*__AGS_AUTOMATION_TOOLBAR_H__*/
