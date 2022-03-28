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

#include <ags/widget/ags_scrolled_piano.h>

void ags_scrolled_piano_class_init(AgsScrolledPianoClass *scrolled_piano);
void ags_scrolled_piano_init(AgsScrolledPiano *scrolled_piano);
void ags_scrolled_piano_finalize(GObject *gobject);

/**
 * SECTION:ags_scrolled_piano
 * @short_description: scrolled piano widget
 * @title: AgsScrolledPiano
 * @section_id:
 * @include: ags/widget/ags_scrolled_piano.h
 *
 * The #AgsScrolledPiano lets you to have a scrolled piano widget.
 */

static gpointer ags_scrolled_piano_parent_class = NULL;

GType
ags_scrolled_piano_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_scrolled_piano = 0;

    static const GTypeInfo ags_scrolled_piano_info = {
      sizeof (AgsScrolledPianoClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_scrolled_piano_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsScrolledPiano),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_scrolled_piano_init,
    };

    ags_type_scrolled_piano = g_type_register_static(GTK_TYPE_GRID,
						     "AgsScrolledPiano", &ags_scrolled_piano_info,
						     0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_scrolled_piano);
  }

  return g_define_type_id__volatile;
}

void
ags_scrolled_piano_class_init(AgsScrolledPianoClass *scrolled_piano)
{
  GObjectClass *gobject;

  ags_scrolled_piano_parent_class = g_type_class_peek_parent(scrolled_piano);

  /* GObjectClass */
  gobject = (GObjectClass *) scrolled_piano;

  gobject->finalize = ags_scrolled_piano_finalize;
}

void
ags_scrolled_piano_init(AgsScrolledPiano *scrolled_piano)
{
  /* scrolled window */
  scrolled_piano->scrolled_window = (GtkScrolledWindow *) gtk_scrolled_window_new();
  gtk_grid_attach((GtkGrid *) scrolled_piano,
		  (GtkWidget *) scrolled_piano->scrolled_window,
		  0, 0,
		  1, 1);

  gtk_scrolled_window_set_policy(scrolled_piano->scrolled_window,
  				 GTK_POLICY_EXTERNAL,
  				 GTK_POLICY_EXTERNAL);

  gtk_widget_set_vexpand(scrolled_piano->scrolled_window,
			 TRUE);

  gtk_widget_set_hexpand(scrolled_piano->scrolled_window,
			 TRUE);

  /* piano */
  scrolled_piano->piano = ags_piano_new(GTK_ORIENTATION_VERTICAL,
					AGS_PIANO_DEFAULT_KEY_WIDTH,
					AGS_PIANO_DEFAULT_KEY_HEIGHT);
  gtk_scrolled_window_set_child(scrolled_piano->scrolled_window,
				scrolled_piano->piano);
}

void
ags_scrolled_piano_finalize(GObject *gobject)
{
  AgsScrolledPiano *scrolled_piano;
  
  scrolled_piano = AGS_SCROLLED_PIANO(gobject);
  
  /* call parent */
  G_OBJECT_CLASS(ags_scrolled_piano_parent_class)->finalize(gobject);
}

/**
 * ags_scrolled_piano_get_scrolled_window:
 * @scrolled_piano: the #AgsScrolledPiano
 * 
 * Get scrolled window of @scrolled_piano.
 * 
 * Returns: (transfer none): the #GtkScrolled_Window
 *
 * Since: 3.6.8
 */
GtkScrolledWindow*
ags_scrolled_piano_get_scrolled_window(AgsScrolledPiano *scrolled_piano)
{
  if(!AGS_IS_SCROLLED_PIANO(scrolled_piano)){
    return(NULL);
  }
  
  return(scrolled_piano->scrolled_window);
}

/**
 * ags_scrolled_piano_get_piano:
 * @scrolled_piano: the #AgsScrolledPiano
 * 
 * Get piano of @scrolled_piano.
 * 
 * Returns: (transfer none): the #AgsPiano
 *
 * Since: 3.6.8
 */
AgsPiano*
ags_scrolled_piano_get_piano(AgsScrolledPiano *scrolled_piano)
{
  if(!AGS_IS_SCROLLED_PIANO(scrolled_piano)){
    return(NULL);
  }
  
  return(scrolled_piano->piano);
}

/**
 * ags_scrolled_piano_new:
 *
 * Create a new #AgsScrolledPiano.
 *
 * Returns: a new #AgsScrolledPiano
 *
 * Since: 3.0.0
 */
AgsScrolledPiano*
ags_scrolled_piano_new()
{
  AgsScrolledPiano *scrolled_piano;

  scrolled_piano = (AgsScrolledPiano *) g_object_new(AGS_TYPE_SCROLLED_PIANO,
						     NULL);

  return(scrolled_piano);
}
