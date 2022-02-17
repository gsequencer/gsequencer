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

#ifndef __AGS_VAUTOMATION_EDIT_BOX_H__
#define __AGS_VAUTOMATION_EDIT_BOX_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#include <ags/GSequencer/editor/ags_automation_edit_box.h>

G_BEGIN_DECLS

#define AGS_TYPE_VAUTOMATION_EDIT_BOX                (ags_vautomation_edit_box_get_type())
#define AGS_VAUTOMATION_EDIT_BOX(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_VAUTOMATION_EDIT_BOX, AgsVAutomationEditBox))
#define AGS_VAUTOMATION_EDIT_BOX_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_VAUTOMATION_EDIT_BOX, AgsVAutomationEditBoxClass))
#define AGS_IS_VAUTOMATION_EDIT_BOX(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_VAUTOMATION_EDIT_BOX))
#define AGS_IS_VAUTOMATION_EDIT_BOX_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_VAUTOMATION_EDIT_BOX))
#define AGS_VAUTOMATION_EDIT_BOX_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS (obj, AGS_TYPE_VAUTOMATION_EDIT_BOX, AgsVAutomationEditBoxClass))

typedef struct _AgsVAutomationEditBox AgsVAutomationEditBox;
typedef struct _AgsVAutomationEditBoxClass AgsVAutomationEditBoxClass;

struct _AgsVAutomationEditBox
{
  AgsAutomationEditBox automation_edit_box;
};

struct _AgsVAutomationEditBoxClass
{
  AgsAutomationEditBoxClass automation_edit_box;
};

GType ags_vautomation_edit_box_get_type(void);

AgsVAutomationEditBox* ags_vautomation_edit_box_new();

G_END_DECLS

#endif /*__AGS_VAUTOMATION_EDIT_BOX_H__*/
