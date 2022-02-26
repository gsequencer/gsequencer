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

#include <ags/widget/ags_scrolled_level_box.h>

#include <ags/widget/ags_level_box.h>
#include <ags/widget/ags_level.h>

void ags_scrolled_level_box_class_init(AgsScrolledLevelBoxClass *scrolled_level_box);
void ags_scrolled_level_box_init(AgsScrolledLevelBox *scrolled_level_box);
void ags_scrolled_level_box_finalize(GObject *gobject);

/**
 * SECTION:ags_scrolled_level_box
 * @short_description: scrolled level box widget
 * @title: AgsScrolledLevelBox
 * @section_id:
 * @include: ags/widget/ags_scrolled_level_box.h
 *
 * The #AgsScrolledLevelBox lets you to have a scrolled level box widget.
 */

static gpointer ags_scrolled_level_box_parent_class = NULL;

GType
ags_scrolled_level_box_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_scrolled_level_box = 0;

    static const GTypeInfo ags_scrolled_level_box_info = {
      sizeof (AgsScrolledLevelBoxClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_scrolled_level_box_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsScrolledLevelBox),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_scrolled_level_box_init,
    };

    ags_type_scrolled_level_box = g_type_register_static(GTK_TYPE_GRID,
							 "AgsScrolledLevelBox", &ags_scrolled_level_box_info,
							 0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_scrolled_level_box);
  }

  return g_define_type_id__volatile;
}

void
ags_scrolled_level_box_class_init(AgsScrolledLevelBoxClass *scrolled_level_box)
{
  GObjectClass *gobject;

  ags_scrolled_level_box_parent_class = g_type_class_peek_parent(scrolled_level_box);

  /* GObjectClass */
  gobject = (GObjectClass *) scrolled_level_box;

  gobject->finalize = ags_scrolled_level_box_finalize;
}

void
ags_scrolled_level_box_init(AgsScrolledLevelBox *scrolled_level_box)
{
  /* scrolled window */
  scrolled_level_box->scrolled_window = (GtkScrolledWindow *) gtk_scrolled_window_new();
  gtk_grid_attach((GtkGrid *) scrolled_level_box,
		  (GtkWidget *) scrolled_level_box->scrolled_window,
		  0, 0,
		  1, 1);

  gtk_scrolled_window_set_policy(scrolled_level_box->scrolled_window,
  				 GTK_POLICY_EXTERNAL,
  				 GTK_POLICY_EXTERNAL);

  gtk_widget_set_vexpand(scrolled_level_box->scrolled_window,
			 TRUE);

  gtk_widget_set_size_request(scrolled_level_box->scrolled_window,
			      AGS_LEVEL_DEFAULT_WIDTH_REQUEST, -1);

  /* level box */
  scrolled_level_box->level_box = ags_level_box_new(GTK_ORIENTATION_VERTICAL);
  gtk_scrolled_window_set_child(scrolled_level_box->scrolled_window,
				scrolled_level_box->level_box);
}

void
ags_scrolled_level_box_finalize(GObject *gobject)
{
  AgsScrolledLevelBox *scrolled_level_box;
  
  scrolled_level_box = AGS_SCROLLED_LEVEL_BOX(gobject);
  
  /* call parent */
  G_OBJECT_CLASS(ags_scrolled_level_box_parent_class)->finalize(gobject);
}

/**
 * ags_scrolled_level_box_get_scrolled_window:
 * @scrolled_level_box: the #AgsScrolledLevel_Box
 * 
 * Get scrolled window of @scrolled_level_box.
 * 
 * Returns: (transfer none): the #GtkScrolledWindow
 *
 * Since: 4.0.0
 */
GtkScrolledWindow*
ags_scrolled_level_box_get_scrolled_window(AgsScrolledLevelBox *scrolled_level_box)
{
  if(!AGS_IS_SCROLLED_LEVEL_BOX(scrolled_level_box)){
    return(NULL);
  }
  
  return(scrolled_level_box->scrolled_window);
}

/**
 * ags_scrolled_level_box_get_level_box:
 * @scrolled_level_box: the #AgsScrolledLevel_Box
 * 
 * Get level box of @scrolled_level_box.
 * 
 * Returns: (transfer none): the #AgsLevelBox
 *
 * Since: 4.0.0
 */
AgsLevelBox*
ags_scrolled_level_box_get_level_box(AgsScrolledLevelBox *scrolled_level_box)
{
  if(!AGS_IS_SCROLLED_LEVEL_BOX(scrolled_level_box)){
    return(NULL);
  }
  
  return(scrolled_level_box->level_box);
}

/**
 * ags_scrolled_level_box_new:
 *
 * Create a new #AgsScrolledLevelBox.
 *
 * Returns: a new #AgsScrolledLevelBox
 *
 * Since: 3.0.0
 */
AgsScrolledLevelBox*
ags_scrolled_level_box_new()
{
  AgsScrolledLevelBox *scrolled_level_box;

  scrolled_level_box = (AgsScrolledLevelBox *) g_object_new(AGS_TYPE_SCROLLED_LEVEL_BOX,
							    NULL);

  return(scrolled_level_box);
}
