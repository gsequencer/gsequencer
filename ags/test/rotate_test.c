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

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <stdlib.h>

void
draw_callback(GtkWidget *drawing_area,
	      cairo_t *cr,
	      gint width, gint height,
	      gpointer user_data)
{
  GtkSettings *settings;

  PangoLayout *layout;
  PangoFontDescription *desc;
	
  PangoRectangle ink_rect, logical_rect;

  gchar *font_name;
  gchar *note_str;

  g_message("draw");
  
  settings = gtk_settings_get_default();

  font_name = NULL;
  
  g_object_get(settings,
	       "gtk-font-name", &font_name,
	       NULL);

  note_str = "AAA - 𝅘𝅥";
  
  layout = pango_cairo_create_layout(cr);
  pango_layout_set_text(layout,
			note_str,
			-1);
  desc = pango_font_description_from_string(font_name);
  pango_font_description_set_size(desc,
				  48.0 * PANGO_SCALE);
  pango_layout_set_font_description(layout,
				    desc);
  pango_font_description_free(desc);    

  pango_layout_get_extents(layout,
			   &ink_rect,
			   &logical_rect);

  cairo_set_source_rgba(cr,
			0.0,
			0.0,
			0.0,
			1.0);

#if 1
  cairo_translate(cr,
		  250.0 + (logical_rect.width / PANGO_SCALE), 250.0 + (logical_rect.height / PANGO_SCALE));
  cairo_rotate(cr,
	       2.0 * M_PI * 0.5);
  //  cairo_translate(cr,
  //		  -250.0 - (logical_rect.width / PANGO_SCALE), -250.0 - (logical_rect.height / PANGO_SCALE));
#endif
  
  cairo_move_to(cr,
		100.0,
		100.0);

  pango_cairo_show_layout(cr,
			  layout);
}

void
activate(GtkApplication *app,
	 gpointer user_data)
{
  GtkWindow *window;
  GtkGrid *grid;
  GtkDrawingArea *drawing_area;

  window = gtk_application_window_new(app);

  grid = gtk_grid_new();
  gtk_window_set_child(window,
		       grid);

  drawing_area = gtk_drawing_area_new();
  gtk_widget_set_size_request(drawing_area,
			      500, 500);
  gtk_grid_attach(grid,
		  drawing_area,
		  0, 0,
		  1, 1);

  gtk_drawing_area_set_draw_func(drawing_area,
				 draw_callback,
				 NULL,
				 NULL);
  
  gtk_widget_show(window);
}

int
main(int argc, char **argv)
{
  GtkApplication *app;

  int status;
  
  app = gtk_application_new("org.nongnu.gsequencer.rotate_test",
			    G_APPLICATION_FLAGS_NONE);
  
  g_signal_connect(app, "activate",
		   G_CALLBACK(activate), NULL);

  status = g_application_run(G_APPLICATION(app),
			     argc,
			     argv);

  g_object_unref(app);
  
  return(status);
}
