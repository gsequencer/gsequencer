/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2021 Joël Krähemann
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

#include <ags/widget/ags_scrolled_window.h>

void ags_scrolled_window_class_init(AgsScrolledWindowClass *scrolled_window);
void ags_scrolled_window_init(AgsScrolledWindow *scrolled_window);
void ags_scrolled_window_finalize(GObject *gobject);

static gpointer ags_scrolled_window_parent_class = NULL;

GType
ags_scrolled_window_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_scrolled_window;

    static const GTypeInfo ags_scrolled_window_info = {
      sizeof(AgsScrolledWindowClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_scrolled_window_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsScrolledWindow),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_scrolled_window_init,
    };

    ags_type_scrolled_window = g_type_register_static(GTK_TYPE_GRID,
						      "AgsScrolledWindow", &ags_scrolled_window_info,
						      0);

    g_once_init_leave (&g_define_type_id__volatile, ags_type_scrolled_window);
  }

  return g_define_type_id__volatile;
}

void
ags_scrolled_window_class_init(AgsScrolledWindowClass *scrolled_window)
{
  GObjectClass *gobject;
  GtkWidgetClass *widget;
  GtkContainerClass *container;

  ags_scrolled_window_parent_class = g_type_class_peek_parent(scrolled_window);

  /* GObjectClass */
  gobject = (GObjectClass *) scrolled_window;

  gobject->finalize = ags_scrolled_window_finalize;

  /* GtkWidgetClass */
  widget = (GtkWidgetClass *) scrolled_window;
  
  /* GtkContainerClass */
  container = (GtkContainerClass *) scrolled_window;
}

void
ags_scrolled_window_init(AgsScrolledWindow *scrolled_window)
{
  scrolled_window->viewport = ags_viewport_new();
  gtk_grid_attach(scrolled_window,
		  scrolled_window->viewport,
		  0, 0,
		  1, 1);
  
  scrolled_window->show_vscrollbar = TRUE;
  scrolled_window->show_hscrollbar = TRUE;
  
  scrolled_window->vscrollbar = gtk_scrollbar_new(GTK_ORIENTATION_VERTICAL,
						  NULL);
  gtk_grid_attach(scrolled_window,
		  scrolled_window->vscrollbar,
		  1, 0,
		  1, 1);
  
  scrolled_window->hscrollbar = gtk_scrollbar_new(GTK_ORIENTATION_HORIZONTAL,
						  NULL);
  gtk_grid_attach(scrolled_window,
		  scrolled_window->hscrollbar,
		  0, 1,
		  1, 1);
}

void
ags_scrolled_window_finalize(GObject *gobject)
{
  //TODO:JK: implement me

  G_OBJECT_CLASS(ags_scrolled_window_parent_class)->finalize(gobject);
}

/**
 * ags_scrolled_window_new:
 * 
 * Create a new instance of #AgsScrolledWindow
 *
 * Returns: the new #AgsScrolledWindow
 * 
 * Since: 3.12.1
 */
AgsScrolledWindow*
ags_scrolled_window_new()
{
  AgsScrolledWindow *scrolled_window;

  scrolled_window = (AgsScrolledWindow *) g_object_new(AGS_TYPE_SCROLLED_WINDOW,
						       NULL);
  
  return(scrolled_window);
}
