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

#include <ags/widget/ags_scrolled_scale_box.h>

#include <ags/widget/ags_scale_box.h>
#include <ags/widget/ags_scale.h>

void ags_scrolled_scale_box_class_init(AgsScrolledScaleBoxClass *scrolled_scale_box);
void ags_scrolled_scale_box_init(AgsScrolledScaleBox *scrolled_scale_box);
void ags_scrolled_scale_box_finalize(GObject *gobject);

/**
 * SECTION:ags_scrolled_scale_box
 * @short_description: scrolled scale box widget
 * @title: AgsScrolledScaleBox
 * @section_id:
 * @include: ags/widget/ags_scrolled_scale_box.h
 *
 * The #AgsScrolledScaleBox lets you to have a scrolled scale box widget.
 */

static gpointer ags_scrolled_scale_box_parent_class = NULL;

GType
ags_scrolled_scale_box_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_scrolled_scale_box = 0;

    static const GTypeInfo ags_scrolled_scale_box_info = {
      sizeof (AgsScrolledScaleBoxClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_scrolled_scale_box_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsScrolledScaleBox),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_scrolled_scale_box_init,
    };

    ags_type_scrolled_scale_box = g_type_register_static(GTK_TYPE_GRID,
							 "AgsScrolledScaleBox", &ags_scrolled_scale_box_info,
							 0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_scrolled_scale_box);
  }

  return g_define_type_id__volatile;
}

void
ags_scrolled_scale_box_class_init(AgsScrolledScaleBoxClass *scrolled_scale_box)
{
  GObjectClass *gobject;

  ags_scrolled_scale_box_parent_class = g_type_class_peek_parent(scrolled_scale_box);

  /* GObjectClass */
  gobject = (GObjectClass *) scrolled_scale_box;

  gobject->finalize = ags_scrolled_scale_box_finalize;
}

void
ags_scrolled_scale_box_init(AgsScrolledScaleBox *scrolled_scale_box)
{
  /* scrolled window */
  scrolled_scale_box->scrolled_window = (GtkScrolledWindow *) gtk_scrolled_window_new();
  gtk_grid_attach((GtkGrid *) scrolled_scale_box,
		  (GtkWidget *) scrolled_scale_box->scrolled_window,
		  0, 0,
		  1, 1);

  gtk_scrolled_window_set_policy(scrolled_scale_box->scrolled_window,
  				 GTK_POLICY_EXTERNAL,
  				 GTK_POLICY_EXTERNAL);

  gtk_widget_set_vexpand(scrolled_scale_box->scrolled_window,
			 TRUE);

  gtk_widget_set_size_request(scrolled_scale_box->scrolled_window,
			      AGS_SCALE_DEFAULT_WIDTH_REQUEST, -1);

  /* scale box */
  scrolled_scale_box->scale_box = ags_scale_box_new(GTK_ORIENTATION_VERTICAL);
  gtk_scrolled_window_set_child(scrolled_scale_box->scrolled_window,
				scrolled_scale_box->scale_box);
}

void
ags_scrolled_scale_box_finalize(GObject *gobject)
{
  AgsScrolledScaleBox *scrolled_scale_box;
  
  scrolled_scale_box = AGS_SCROLLED_SCALE_BOX(gobject);
  
  /* call parent */
  G_OBJECT_CLASS(ags_scrolled_scale_box_parent_class)->finalize(gobject);
}

/**
 * ags_scrolled_scale_box_new:
 *
 * Create a new #AgsScrolledScaleBox.
 *
 * Returns: a new #AgsScrolledScaleBox
 *
 * Since: 3.0.0
 */
AgsScrolledScaleBox*
ags_scrolled_scale_box_new()
{
  AgsScrolledScaleBox *scrolled_scale_box;

  scrolled_scale_box = (AgsScrolledScaleBox *) g_object_new(AGS_TYPE_SCROLLED_SCALE_BOX,
							    NULL);

  return(scrolled_scale_box);
}
