#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <math.h>

void
draw_callback(GtkWidget *drawing_area,
	      cairo_t *cr,
	      int width, int height,
	      gpointer data)
{
  GdkRGBA fg_color;
  GdkRGBA bg_color;
  GdkRGBA shadow_color;
  GdkRGBA text_color;
  
  gdouble button_width;
  gdouble radius;
  gdouble outline_strength;

  g_message("draw %d %d", width, height);
  
  button_width = 0.0;

  radius = 32.0;

  outline_strength = 2.0;
  
  gdk_rgba_parse(&fg_color,
		 "#101010");
      
  gdk_rgba_parse(&bg_color,
		 "#cbd5d9");
    
  gdk_rgba_parse(&shadow_color,
		 "#ffffff40");

  gdk_rgba_parse(&text_color,
		 "#1a1a1a");

  cairo_set_source_rgba(cr,
			fg_color.red,
			fg_color.green,
			fg_color.blue,
			fg_color.alpha);
  
  cairo_set_line_width(cr, 2.0);
  cairo_set_line_join(cr, CAIRO_LINE_JOIN_MITER);
  
  cairo_new_path(cr);
  cairo_move_to(cr,
		radius,
		radius);
  cairo_line_to(cr,
		-1.0 * sin((0.35 * M_PI) / (0.65 * M_PI)) + (radius),
		sin(((0.65 * M_PI) / (0.35 * M_PI)) + (radius));
  cairo_line_to(cr,
		-1.0 * sin((0.65 * M_PI) / (0.35 * M_PI)) + (radius),
		-1.0 * sin((0.35 * M_PI) / (0.65 * M_PI)) + (radius));
  cairo_close_path(cr);
  cairo_fill(cr);

#if 0
  cairo_new_path(cr);
  cairo_move_to(cr,
		cos((0.65 * M_PI) / (0.35 * M_PI)) + radius,
		-1.0 * sin((0.35 * M_PI) / (0.65 * M_PI)) + radius);
  cairo_line_to(cr,
		-1.0 * cos((0.35 * M_PI) / (0.65 * M_PI)) + radius,
		sin((0.65 * M_PI) / (0.35 * M_PI)) + radius);
  cairo_line_to(cr,
		radius,
		radius);
  cairo_close_path(cr);
  cairo_fill(cr);
#endif
  
  cairo_arc(cr,
	    radius,
	    radius,
	    radius,
	    0.35 * M_PI,
	    0.65 * M_PI);
  cairo_fill(cr);

  cairo_arc(cr,
	    radius,
	    radius,
	    radius,
	    -0.65 * M_PI,
	    -0.35 * M_PI);
  cairo_fill(cr);
}

void
activate(GtkApplication *app,
	 gpointer user_data)
{
  GtkWindow *window;
  GtkGrid *grid;
  GtkDrawingArea *drawing_area;

  guint i, j;
  
  window = gtk_application_window_new(app);

  grid = gtk_grid_new();
  gtk_window_set_child(window,
		       grid);

  drawing_area = gtk_drawing_area_new();
  gtk_widget_set_size_request(drawing_area, 100, 100);
  gtk_grid_attach(grid,
		  drawing_area,
		  0, 0,
		  3, 1);

  gtk_drawing_area_set_draw_func(drawing_area,
				 (GtkDrawingAreaDrawFunc) draw_callback,
				 NULL,
				 NULL);

  
  gtk_widget_show(window);
}

int
main(int argc, char **argv)
{
  GtkApplication *app;

  int status;
  
  app = gtk_application_new("org.nongnu.gsequencer.ags_cairo_path_test",
			    G_APPLICATION_FLAGS_NONE);
  
  g_signal_connect(app, "activate",
		   G_CALLBACK(activate), NULL);

  status = g_application_run(G_APPLICATION(app),
			     argc,
			     argv);

  g_object_unref(app);
  
  return(status);
}
