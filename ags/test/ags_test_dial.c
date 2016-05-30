#include <ags/widget/ags_dial.h>

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

int
main(int argc, char **argv)
{
  GtkWindow *window;

  AgsDial *dial;
  GtkAdjustment *adjustment;

  gtk_init(&argc, &argv);

  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

  adjustment = (GtkAdjustment *) gtk_adjustment_new(0.0, 0.0, 1.0, 0.1, 0.1, 0.0);

  dial = g_object_new(AGS_TYPE_DIAL,
		      "adjustment\0", adjustment,
		      NULL);
  dial->scale_precision = 3.0;
  gtk_container_add(GTK_CONTAINER(window),
		    GTK_WIDGET(dial));

  gtk_widget_show_all(window);
  
  gtk_main();
  
  return(0);
}
