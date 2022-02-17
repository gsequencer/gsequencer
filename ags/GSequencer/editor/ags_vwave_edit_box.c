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

#include <ags/GSequencer/editor/ags_vwave_edit_box.h>

void ags_vwave_edit_box_class_init(AgsVWaveEditBoxClass *vwave_edit_box);
void ags_vwave_edit_box_init(AgsVWaveEditBox *vwave_edit_box);

/**
 * SECTION:ags_vwave_edit_box
 * @short_description: vertical box widget
 * @title: AgsVWaveEditBox
 * @section_id:
 * @include: ags/widget/ags_vwave_edit_box.h
 *
 * The #AgsVWaveEditBox is an horizontal box widget containing #AgsWaveEdit.
 */

GType
ags_vwave_edit_box_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_vwave_edit_box = 0;

    static const GTypeInfo ags_vwave_edit_box_info = {
      sizeof (AgsVWaveEditBoxClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_vwave_edit_box_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsVWaveEditBox),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_vwave_edit_box_init,
    };

    ags_type_vwave_edit_box = g_type_register_static(AGS_TYPE_WAVE_EDIT_BOX,
						     "AgsVWaveEditBox", &ags_vwave_edit_box_info,
						     0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_vwave_edit_box);
  }

  return g_define_type_id__volatile;
}

void
ags_vwave_edit_box_class_init(AgsVWaveEditBoxClass *vwave_edit_box)
{
}

void
ags_vwave_edit_box_init(AgsVWaveEditBox *vwave_edit_box)
{
  gtk_orientable_set_orientation(GTK_ORIENTABLE(vwave_edit_box),
				 GTK_ORIENTATION_VERTICAL);
}

/**
 * ags_vwave_edit_box_new:
 * 
 * Create a new instance of #AgsVWaveEditBox.
 * 
 * Returns: the new #AgsVWaveEditBox instance
 * 
 * Since: 3.0.0
 */
AgsVWaveEditBox*
ags_vwave_edit_box_new()
{
  AgsVWaveEditBox *vwave_edit_box;

  vwave_edit_box = (AgsVWaveEditBox *) g_object_new(AGS_TYPE_VWAVE_EDIT_BOX,
						    NULL);
  
  return(vwave_edit_box);
}
