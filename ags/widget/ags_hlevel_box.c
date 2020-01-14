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

#include <ags/widget/ags_hlevel_box.h>

void ags_hlevel_box_class_init(AgsHLevelBoxClass *hlevel_box);
void ags_hlevel_box_init(AgsHLevelBox *hlevel_box);

/**
 * SECTION:ags_hlevel_box
 * @short_description: horizontal box widget
 * @title: AgsHLevelBox
 * @section_id:
 * @include: ags/widget/ags_hlevel_box.h
 *
 * The #AgsHLevelBox is an horizontal box widget containing #AgsLevel.
 */

static gpointer ags_hlevel_box_parent_class = NULL;

GType
ags_hlevel_box_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_hlevel_box = 0;

    static const GTypeInfo ags_hlevel_box_info = {
      sizeof (AgsHLevelBoxClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_hlevel_box_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsHLevelBox),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_hlevel_box_init,
    };

    ags_type_hlevel_box = g_type_register_static(AGS_TYPE_LEVEL_BOX,
						 "AgsHLevelBox", &ags_hlevel_box_info,
						 0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_hlevel_box);
  }

  return g_define_type_id__volatile;
}

void
ags_hlevel_box_class_init(AgsHLevelBoxClass *hlevel_box)
{
}

void
ags_hlevel_box_init(AgsHLevelBox *hlevel_box)
{
  gtk_orientable_set_orientation(GTK_ORIENTABLE(hlevel_box),
				 GTK_ORIENTATION_HORIZONTAL);
}

/**
 * ags_hlevel_box_new:
 * 
 * Create a new instance of #AgsHLevelBox.
 * 
 * Returns: the new #AgsHLevelBox instance
 * 
 * Since: 3.0.0
 */
AgsHLevelBox*
ags_hlevel_box_new()
{
  AgsHLevelBox *hlevel_box;

  hlevel_box = (AgsHLevelBox *) g_object_new(AGS_TYPE_HLEVEL_BOX,
					     NULL);
  
  return(hlevel_box);
}
