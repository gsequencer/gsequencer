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

#include <ags/widget/ags_vlevel_box.h>

void ags_vlevel_box_class_init(AgsVLevelBoxClass *vlevel_box);
void ags_vlevel_box_init(AgsVLevelBox *vlevel_box);

/**
 * SECTION:ags_vlevel_box
 * @short_description: vertical box widget
 * @title: AgsVLevelBox
 * @section_id:
 * @include: ags/widget/ags_vlevel_box.h
 *
 * The #AgsVLevelBox is an vertical box widget containing #AgsLevel.
 */

GType
ags_vlevel_box_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_vlevel_box = 0;

    static const GTypeInfo ags_vlevel_box_info = {
      sizeof (AgsVLevelBoxClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_vlevel_box_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsVLevelBox),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_vlevel_box_init,
    };

    ags_type_vlevel_box = g_type_register_static(AGS_TYPE_LEVEL_BOX,
						 "AgsVLevelBox", &ags_vlevel_box_info,
						 0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_vlevel_box);
  }

  return g_define_type_id__volatile;
}

void
ags_vlevel_box_class_init(AgsVLevelBoxClass *vlevel_box)
{
}

void
ags_vlevel_box_init(AgsVLevelBox *vlevel_box)
{
  gtk_orientable_set_orientation(GTK_ORIENTABLE(vlevel_box),
				 GTK_ORIENTATION_VERTICAL);
}

/**
 * ags_vlevel_box_new:
 * 
 * Create a new instance of #AgsVLevelBox.
 * 
 * Returns: the new #AgsVLevelBox instance
 * 
 * Since: 2.0.0
 */
AgsVLevelBox*
ags_vlevel_box_new()
{
  AgsVLevelBox *vlevel_box;

  vlevel_box = (AgsVLevelBox *) g_object_new(AGS_TYPE_VLEVEL_BOX,
					     NULL);
  
  return(vlevel_box);
}
