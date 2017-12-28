/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2017 Joël Krähemann
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

#include <ags/X/editor/ags_vautomation_edit_box.h>

void ags_vautomation_edit_box_class_init(AgsVAutomationEditBoxClass *vautomation_edit_box);
void ags_vautomation_edit_box_init(AgsVAutomationEditBox *vautomation_edit_box);

/**
 * SECTION:ags_vautomation_edit_box
 * @short_description: horizontal box widget
 * @title: AgsVAutomationEditBox
 * @section_id:
 * @include: ags/widget/ags_vautomation_edit_box.h
 *
 * The #AgsVAutomationEditBox is an horizontal box widget containing #AgsScale.
 */

GType
ags_vautomation_edit_box_get_type(void)
{
  static GType ags_type_vautomation_edit_box = 0;

  if(!ags_type_vautomation_edit_box){
    static const GTypeInfo ags_vautomation_edit_box_info = {
      sizeof (AgsVAutomationEditBoxClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_vautomation_edit_box_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsVAutomationEditBox),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_vautomation_edit_box_init,
    };

    ags_type_vautomation_edit_box = g_type_register_static(AGS_TYPE_AUTOMATION_EDIT_BOX,
							   "AgsVAutomationEditBox", &ags_vautomation_edit_box_info,
							   0);
  }
  
  return(ags_type_vautomation_edit_box);
}

void
ags_vautomation_edit_box_class_init(AgsVAutomationEditBoxClass *vautomation_edit_box)
{
}

void
ags_vautomation_edit_box_init(AgsVAutomationEditBox *vautomation_edit_box)
{
  gtk_orientable_set_orientation(GTK_ORIENTABLE(vautomation_edit_box),
				 GTK_ORIENTATION_VERTICAL);
}

/**
 * ags_vautomation_edit_box_new:
 * 
 * Create a new instance of #AgsVAutomationEditBox.
 * 
 * Returns: the new #AgsVAutomationEditBox instance
 * 
 * Since: 1.3.0
 */
AgsVAutomationEditBox*
ags_vautomation_edit_box_new()
{
  AgsVAutomationEditBox *vautomation_edit_box;

  vautomation_edit_box = (AgsVAutomationEditBox *) g_object_new(AGS_TYPE_VAUTOMATION_EDIT_BOX,
								NULL);
  
  return(vautomation_edit_box);
}
