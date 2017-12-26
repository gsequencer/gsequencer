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
 * @short_description: horizontal box widget
 * @title: AgsVScaleBox
 * @section_id:
 * @include: ags/widget/ags_vscale_box.h
 *
 * The #AgsVScaleBox is an horizontal box widget containing #AgsScale.
 */

GType
ags_vscale_box_get_type(void)
{
  static GType ags_type_vscale_box = 0;

  if(!ags_type_vscale_box){
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
  }
  
  return(ags_type_vscale_box);
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
 * Since: 1.3.0
 */
AgsVScale_Box*
ags_vscale_box_new()
{
  AgsVScaleBox *vscale_box;

  vscale_box = (AgsVScaleBox *) g_object_new(AGS_TYPE_VSCALE_BOX,
					     NULL);
  
  return(vscale_box);
}
