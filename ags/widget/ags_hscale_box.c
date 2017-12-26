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

#include <ags/widget/ags_hscale_box.h>

void ags_hscale_box_class_init(AgsHScaleBoxClass *hscale_box);
void ags_hscale_box_init(AgsHScaleBox *hscale_box);

/**
 * SECTION:ags_hscale_box
 * @short_description: horizontal box widget
 * @title: AgsHScaleBox
 * @section_id:
 * @include: ags/widget/ags_hscale_box.h
 *
 * The #AgsHScaleBox is an horizontal box widget containing #AgsScale.
 */

static gpointer ags_hscale_box_parent_class = NULL;

GType
ags_hscale_box_get_type(void)
{
  static GType ags_type_hscale_box = 0;

  if(!ags_type_hscale_box){
    static const GTypeInfo ags_hscale_box_info = {
      sizeof (AgsHScaleBoxClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_hscale_box_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsHScaleBox),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_hscale_box_init,
    };

    ags_type_hscale_box = g_type_register_static(AGS_TYPE_SCALE_BOX,
						 "AgsHScaleBox", &ags_hscale_box_info,
						 0);
  }
  
  return(ags_type_hscale_box);
}

void
ags_hscale_box_class_init(AgsHScaleBoxClass *hscale_box)
{
}

void
ags_hscale_box_init(AgsHScaleBox *hscale_box)
{
  gtk_orientable_set_orientation(GTK_ORIENTABLE(hscale_box),
				 GTK_ORIENTATION_HORIZONTAL);
}

/**
 * ags_hscale_box_new:
 * 
 * Create a new instance of #AgsHScaleBox.
 * 
 * Returns: the new #AgsHScaleBox instance
 * 
 * Since: 1.3.0
 */
AgsHScale_Box*
ags_hscale_box_new()
{
  AgsHScaleBox *hscale_box;

  hscale_box = (AgsHScaleBox *) g_object_new(AGS_TYPE_HSCALE_BOX,
					     NULL);
  
  return(hscale_box);
}
