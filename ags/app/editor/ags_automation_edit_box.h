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

#ifndef __AGS_AUTOMATION_EDIT_BOX_H__
#define __AGS_AUTOMATION_EDIT_BOX_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#include <ags/app/editor/ags_automation_edit.h>

G_BEGIN_DECLS

#define AGS_TYPE_AUTOMATION_EDIT_BOX                (ags_automation_edit_box_get_type())
#define AGS_AUTOMATION_EDIT_BOX(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_AUTOMATION_EDIT_BOX, AgsAutomationEditBox))
#define AGS_AUTOMATION_EDIT_BOX_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_AUTOMATION_EDIT_BOX, AgsAutomationEditBoxClass))
#define AGS_IS_AUTOMATION_EDIT_BOX(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_AUTOMATION_EDIT_BOX))
#define AGS_IS_AUTOMATION_EDIT_BOX_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_AUTOMATION_EDIT_BOX))
#define AGS_AUTOMATION_EDIT_BOX_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS (obj, AGS_TYPE_AUTOMATION_EDIT_BOX, AgsAutomationEditBoxClass))

#define AGS_AUTOMATION_EDIT_BOX_DEFAULT_FIXED_EDIT_HEIGHT (128)
#define AGS_AUTOMATION_EDIT_BOX_DEFAULT_SPACING (6)

typedef struct _AgsAutomationEditBox AgsAutomationEditBox;
typedef struct _AgsAutomationEditBoxClass AgsAutomationEditBoxClass;

struct _AgsAutomationEditBox
{
  GtkBox box;

  /* private */  
  GList *automation_edit;
};

struct _AgsAutomationEditBoxClass
{
  GtkBoxClass box;

  void (*child_width_request)(AgsAutomationEditBox *automation_edit_box,
			      GtkWidget *automation_edit,
			      gint width_request);
  void (*child_height_request)(AgsAutomationEditBox *automation_edit_box,
			       GtkWidget *automation_edit,
			       gint height_request);
};

GType ags_automation_edit_box_get_type(void);

GList* ags_automation_edit_box_get_automation_edit(AgsAutomationEditBox *automation_edit_box);
void ags_automation_edit_box_add_automation_edit(AgsAutomationEditBox *automation_edit_box,
						 AgsAutomationEdit *automation_edit);
void ags_automation_edit_box_remove_automation_edit(AgsAutomationEditBox *automation_edit_box,
						    AgsAutomationEdit *automation_edit);

void ags_automation_edit_box_child_width_request(AgsAutomationEditBox *automation_edit_box,
						 GtkWidget *automation_edit,
						 gint width_request);
void ags_automation_edit_box_child_height_request(AgsAutomationEditBox *automation_edit_box,
						  GtkWidget *automation_edit,
						  gint height_request);

AgsAutomationEditBox* ags_automation_edit_box_new(GtkOrientation orientation);

G_END_DECLS

#endif /*__AGS_AUTOMATION_EDIT_BOX_H__*/
