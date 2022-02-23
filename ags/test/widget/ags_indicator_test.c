#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/widget/ags_indicator.h>

#include <stdlib.h>

gboolean timeout(AgsIndicator *indicator);

gboolean
timeout(AgsIndicator *indicator)
{
  gtk_adjustment_set_value(indicator->adjustment,
			   1.0 / RAND_MAX * rand());
  
  return(G_SOURCE_CONTINUE);
}

void
activate(GtkApplication *app,
	 gpointer user_data)
{
  GtkWindow *window;
  GtkGrid *grid;
  AgsIndicator *indicator;

  window = gtk_application_window_new(app);

  grid = gtk_grid_new();
  gtk_window_set_child(window,
		       grid);

  indicator = ags_indicator_new(GTK_ORIENTATION_VERTICAL,
				16,
				7);
  gtk_grid_attach(grid,
		  indicator,
		  0, 0,
		  1, 1);
  
  gtk_widget_show(window);

  g_timeout_add(100,
		timeout,
		indicator);
}

int
main(int argc, char **argv)
{
  GtkApplication *app;

  int status;
  
  app = gtk_application_new("org.nongnu.gsequencer.ags_indicator_test",
			    G_APPLICATION_FLAGS_NONE);
  
  g_signal_connect(app, "activate",
		   G_CALLBACK(activate), NULL);

  status = g_application_run(G_APPLICATION(app),
			     argc,
			     argv);

  g_object_unref(app);
  
  return(status);
}
