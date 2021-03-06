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

#include <ags/widget/ags_vscale_box.h>

void ags_vscale_box_class_init(AgsVScaleBoxClass *vscale_box);
void ags_vscale_box_init(AgsVScaleBox *vscale_box);

/**
 * SECTION:ags_vscale_box
 * @short_description: vertical box widget
 * @title: AgsVScaleBox
 * @section_id:
 * @include: ags/widget/ags_vscale_box.h
 *
 * The #AgsVScaleBox is an vertical box widget containing #AgsScale.
 */

GType
ags_vscale_box_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_vscale_box = 0;

    static const GTypeInfo ags_vscale_box_info = {
      sizeof (AgsVScaleBoxClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_vscale_box_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsVScaleBox),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_vscale_box_init,
    };

    ags_type_vscale_box = g_type_register_static(AGS_TYPE_SCALE_BOX,
						 "AgsVScaleBox", &ags_vscale_box_info,
						 0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_vscale_box);
  }

  return g_define_type_id__volatile;
}

void
ags_vscale_box_class_init(AgsVScaleBoxClass *vscale_box)
{
}

void
ags_vscale_box_init(AgsVScaleBox *vscale_box)
{
  gtk_orientable_set_orientation(GTK_ORIENTABLE(vscale_box),
				 GTK_ORIENTATION_VERTICAL);
}

/**
 * ags_vscale_box_new:
 * 
 * Create a new instance of #AgsVScaleBox.
 * 
 * Returns: the new #AgsVScaleBox instance
 * 
 * Since: 3.0.0
 */
AgsVScaleBox*
ags_vscale_box_new()
{
  AgsVScaleBox *vscale_box;

  vscale_box = (AgsVScaleBox *) g_object_new(AGS_TYPE_VSCALE_BOX,
					     NULL);
  
  return(vscale_box);
}
