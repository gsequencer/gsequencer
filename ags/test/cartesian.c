#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/widget/ags_cartesian.h>

int
main(int argc, char **argv)
{
  GtkWindow *window; 
  AgsCartesian *cartesian;
  
  gtk_init(&argc,
	   &argv);

  window = (GtkWindow *) gtk_window_new(GTK_WINDOW_TOPLEVEL);

  cartesian = ags_cartesian_new();
  gtk_container_add(window,
		    cartesian);
  gtk_widget_show_all(window);
  
  gtk_main();
  
  return(0);
}
